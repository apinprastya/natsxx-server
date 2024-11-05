#include "asyncengine_uvw.h"
#include "client.h"
#include "transport.h"
#include "transport_uvw.h"
#include <cstring>
#include <memory>
#include <spdlog/spdlog.h>
#include <uvw.hpp>
#include <uvw/timer.h>

namespace natsxx {

AsyncEngineUvw::AsyncEngineUvw () {
    mLoop = uvw::loop::get_default ();
}

std::shared_ptr<ServerTransport> AsyncEngineUvw::newServerTransport () {
    return std::make_shared<ServerTransportUvw> (mLoop);
}

std::shared_ptr<Timer> AsyncEngineUvw::newTimer () {
    return std::make_shared<TimerUvw> (mLoop);
}

int AsyncEngineUvw::runLoop () {
    return mLoop->run ();
}

void AsyncEngineUvw::stop () {
    mLoop->stop ();
}

TimerUvw::TimerUvw (std::shared_ptr<uvw::loop> loop) {
    mTimerHandler = loop->resource<uvw::timer_handle> ();
    mTimerHandler->on<uvw::timer_event> (
    [this] (const uvw::timer_event&, uvw::timer_handle& handle) {
        if (mOnTimeout != nullptr)
            mOnTimeout ();
    });
}

TimerUvw::~TimerUvw () {
    mTimerHandler->stop ();
}

void TimerUvw::onTimeout (std::function<void ()> onTimeout) {
    mOnTimeout = onTimeout;
}

void TimerUvw::start (std::chrono::duration<uint64_t, std::milli> duration,
std::chrono::duration<uint64_t, std::milli> repeat) {
    mTimerHandler->start (duration, repeat);
}

void TimerUvw::stop () {
    mTimerHandler->stop ();
}

ServerTransportUvw::ServerTransportUvw (std::shared_ptr<uvw::loop> loop)
: mLoop (loop) {
    mTcp = loop->resource<uvw::tcp_handle> ();
    mTcp->on<uvw::listen_event> ([this] (const uvw::listen_event&, uvw::tcp_handle& srv) {
        std::shared_ptr<uvw::tcp_handle> tcpClient =
        srv.parent ().resource<uvw::tcp_handle> ();

        std::unique_ptr<Transport> libuvTransport =
        std::make_unique<TransportUvw> (tcpClient);
        auto client = std::make_shared<Client> (std::move (libuvTransport));
        // mClients.push_back (client);
        srv.accept (*tcpClient);
        // spdlog::debug("new connection: {}:{}", tcpClient.)

        char* data =
        "INFO "
        "{\"server_id\":"
        "\"NCEUKVMQR4KCNGMKEAIEFS5OF4VMI34DXCTZ5HBFR4YSLETPHFDEWIRQ\",\"server_"
        "name\":\"NCEUKVMQR4KCNGMKEAIEFS5OF4VMI34DXCTZ5HBFR4YSLETPHFDEWIRQ\","
        "\"version\":\"2.11.0-dev\",\"proto\" : 1,\"go\" : "
        "\"go1.23.3\",\"host\" : \"0.0.0.0\",\"port\" : 4222,\"headers\" : "
        "true,\"max_payload\" : "
        "1048576,\"client_id\":5,\"client_ip\":\"127.0.0.1\",\"xkey\":"
        "\"XBRNVBBFW45EB3RA7JI3D6HU6ROXESE2EU2IXXTWYOCENKIGI5AW2GU2\"}\r\n";

        tcpClient->write (data, strlen (data));

        tcpClient->read ();

        if (mOnNewClientCallback != nullptr)
            mOnNewClientCallback (client);
    });
}

int ServerTransportUvw::listen () {
    return mTcp->listen ();
}

int ServerTransportUvw::bind (const std::string& ip, int port) {
    return mTcp->bind (ip, port);
}

void ServerTransportUvw::close () {
    mTcp->close ();
}

void ServerTransportUvw::onNewClient (
std::function<void (std::shared_ptr<Client>)> onNewClientCallback) {
    mOnNewClientCallback = onNewClientCallback;
}

} // namespace natsxx