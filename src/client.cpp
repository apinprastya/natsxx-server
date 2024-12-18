#include "client.h"
#include "asyncengine.h"
#include "buffer.h"
#include "parser.h"
#include "server.h"
#include "transport.h"
#include "util.h"
#include <chrono>
#include <cstring>
#include <memory>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <span>
#include <spdlog/spdlog.h>
#include <string>
#include <subscriber.h>
#include <utility>

namespace natsxx {

Client::Client (std::unique_ptr<Transport> transport)
: mTransport (std::move (transport)) {
    mParser = std::make_unique<MessageParser> (this);
}

Client::~Client () {
}

void Client::setup (int id, Server* server) {
    mId     = id;
    mServer = server;
    mTransport->onData (std::bind (&Client::onData, this, std::placeholders::_1));
    mTransport->onClosed (std::bind (&Client::onClosed, this));
}

void Client::onData (const std::span<const char>& data) {
    if (spdlog::get_level () == spdlog::level::debug) {
        auto ch = std::string (data.data (), data.size ());
        replace_all (ch, "\t", "\\t");
        replace_all (ch, " ", "\\s");
        // replace_all (ch, "\n", "\\n");
        spdlog::debug ("client new data: {}", ch);
    }
    auto result = mParser->parseMessage (data);
    if (result.has_value ()) {
        spdlog::error ("error parsing message: {}", result.value ().errorString ());
    }
}

void Client::onClosed () {
    mPingTimer->stop ();
    mServer->getSubscriberManager ()->unsubscribeClientId (mId);
}

void Client::processConnect (const std::span<const char>& data) {
    spdlog::debug ("Connect args: {}", data.data ());

    auto dataJson = nlohmann::json::parse (data.begin (), data.end (), nullptr, false);
    if (dataJson.is_discarded ()) {
        spdlog::error (
        "unable to parse json: {}", std::string (data.begin (), data.end ()));
        return;
    }
    auto reqArg = dataJson.get<ClientOpts> ();
    spdlog::debug ("{} {} {}", reqArg.name, reqArg.lang, reqArg.version);

    auto asyncEngine = mServer->getAsyncEngine ();
    mPingTimer       = asyncEngine->newTimer ();
    mPingTimer->onTimeout ([this] () { sendPing (); });
    mPingTimer->start (std::chrono::seconds{ mServer->getServerConfig ()->pingIntervalSecond },
    std::chrono::seconds{ mServer->getServerConfig ()->pingIntervalSecond });
}

void Client::processPing () {
    spdlog::debug ("process ping message");
    constexpr const char* pongMessage = "PONG\r\n";
    const std::size_t length          = std::strlen (pongMessage);
    std::span<char> spanData ((char*)pongMessage, length);
    mTransport->send (spanData);
}

void Client::sendPing () {
    spdlog::debug ("sending ping");
    constexpr const char* pongMessage = "PING\r\n";
    const std::size_t length          = std::strlen (pongMessage);
    std::span<char> spanData ((char*)pongMessage, length);
    mTransport->send (spanData);
    mRoundTrip.setStartToNow ();
}

void Client::sendError (const std::string& err) {
    spdlog::debug ("sending error");
    auto messageStr = fmt::format ("-ERR '{}'\r\n", err);
    std::span<char> spanData (messageStr.data (), messageStr.size ());
    mTransport->send (spanData);
}

void Client::sendOK () {
    spdlog::debug ("sending ok");
    constexpr const char* pongMessage = "+OK\r\n";
    const std::size_t length          = std::strlen (pongMessage);
    std::span<char> spanData ((char*)pongMessage, length);
    mTransport->send (spanData);
}

void Client::processPong () {
    spdlog::debug ("got pong message");
    mRoundTrip.calculateRrt ();
}

void Client::processSubscribe (const std::vector<std::string_view>& args) {
    if (args.size () == 2)
        spdlog::debug ("got subscibe message: {} : {}", args[0], args[1]);
    else if (args.size () == 3)
        spdlog::debug ("got subscibe message: {} : {} : {}", args[0], args[1], args[2]);
    auto subject    = std::string (args[0]);
    auto subId      = std::string (args[1]);
    auto subscriber = std::make_shared<Subscriber> (subject, subId, this);
    mServer->getSubscriberManager ()->addSubscriber (subscriber);
}

void Client::processPublish (const PublishArg& publishArg,
const std::span<const char>& data) {
    spdlog::debug ("publish subject: {}; reply: {}; data: {}", publishArg.subject,
    publishArg.reply, std::string (data.begin (), data.end ()));
    auto subscribers =
    mServer->getSubscriberManager ()->getSubscriber (publishArg.subject);
    spdlog::debug ("subscriber length: {}", subscribers.size ());
    for (const auto& subcriber : subscribers) {
        CharBuffer buffer;
        buffer.write ("MSG ");
        buffer.write (publishArg.subject);
        buffer.write (" ");
        buffer.write (subcriber->getId ());
        if (!publishArg.reply.empty ()) {
            buffer.write (" ");
            buffer.write (publishArg.reply);
        }
        buffer.write (" ");
        buffer.write (std::to_string (data.size ()));
        buffer.write ("\r\n");
        buffer.write (data.data (), data.size ());
        buffer.write ("\r\n");

        auto bufferStr = buffer.getBuffer ();
        spdlog::debug (
        "buffer value: {}", std::string{ bufferStr.begin (), bufferStr.end () });
        subcriber->getClient ()->sendMessage (buffer.getBuffer ());
    }
}

void Client::sendMessage (const std::span<const char>& data) {
    auto result = mTransport->send (data);
    if (result != 0) {
        spdlog::error ("unable to send message: {}", result);
    }
}

} // namespace natsxx