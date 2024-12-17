#pragma once

#include "asyncengine.h"
#include "parser.h"
#include <chrono>
#include <cstdint>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace natsxx {

class Server;
class Transport;

class RoundTrip {
    public:
    RoundTrip () = default;

    inline void setStartToNow () {
        mLastCheck = std::chrono::steady_clock::now ();
    }

    inline void calculateRrt () {
        rrt = std::chrono::duration_cast<std::chrono::microseconds> (
        std::chrono::steady_clock::now () - mLastCheck);
    }

    inline uint64_t rrtMilis () {
        return static_cast<uint64_t> (rrt.count ());
    }

    private:
    std::chrono::steady_clock::time_point mLastCheck;
    std::chrono::microseconds rrt;
};

struct SubjectPermissions {
    std::optional<std::vector<std::string>> allow;
    std::optional<std::vector<std::string>> deny;

    friend void from_json (const nlohmann::json& j, SubjectPermissions& p) {
        p.allow = j.at ("allow").get<std::vector<std::string>> ();
        p.deny  = j.at ("deny").get<std::vector<std::string>> ();
    }
};

struct ClientOpts {
    bool echo{};
    bool verbose{};
    bool pedantic{};
    bool tlsRequired{};
    std::optional<std::string> nkey;
    std::optional<std::string> jwt;
    std::optional<std::string> sig;
    std::optional<std::string> authToken;
    std::optional<std::string> user;
    std::optional<std::string> password;
    std::string name;
    std::string lang;
    std::string version;
    int protocol;
    std::optional<std::string> account;
    std::optional<bool> newAccount;
    std::optional<bool> headers;
    std::optional<bool> noResponders;

    // routes and leaf nodes
    std::optional<SubjectPermissions> importPerm;
    std::optional<SubjectPermissions> exportPerm;

    // leaf nodes
    std::optional<std::string> remoteAccount;

    friend void from_json (const nlohmann::json& j, ClientOpts& p) {
        j.at ("echo").get_to (p.echo);
        j.at ("verbose").get_to (p.verbose);
        j.at ("pedantic").get_to (p.pedantic);
        j.at ("tls_required").get_to (p.tlsRequired);
        if (j.contains ("nkey"))
            p.nkey = j.at ("nkey").get<std::string> ();
        if (j.contains ("jwt"))
            p.jwt = j.at ("jwt").get<std::string> ();
        if (j.contains ("sig"))
            p.sig = j.at ("sig").get<std::string> ();
        if (j.contains ("auth_token"))
            p.authToken = j.at ("auth_token").get<std::string> ();
        if (j.contains ("user"))
            p.user = j.at ("user").get<std::string> ();
        if (j.contains ("pass"))
            p.password = j.at ("pass").get<std::string> ();
        j.at ("name").get_to (p.name);
        j.at ("lang").get_to (p.lang);
        j.at ("version").get_to (p.version);
        j.at ("protocol").get_to (p.protocol);
        if (j.contains ("account"))
            p.account = j.at ("account").get<std::string> ();
        if (j.contains ("new_account"))
            p.newAccount = j.at ("new_account").get<bool> ();
        if (j.contains ("headers"))
            p.headers = j.at ("headers").get<bool> ();
        if (j.contains ("no_responders"))
            p.noResponders = j.at ("no_responders").get<bool> ();
        if (j.contains ("import"))
            p.importPerm = j.at ("import").get<SubjectPermissions> ();
        if (j.contains ("export"))
            p.exportPerm = j.at ("export").get<SubjectPermissions> ();
        if (j.contains ("remote_account"))
            p.remoteAccount = j.at ("remote_account").get<std::string> ();
    }
};

class Client : public std::enable_shared_from_this<Client> {
    public:
    Client (std::unique_ptr<Transport> transport);
    ~Client ();
    void setup (int id, Server* server);
    inline int getId () {
        return mId;
    }

    private:
    int mId{};
    Server* mServer = nullptr;
    std::unique_ptr<MessageParser> mParser{};
    std::unique_ptr<Transport> mTransport{};
    ParserState mParserState{};
    ClientOpts mOpts;
    std::shared_ptr<Timer> mPingTimer{};
    RoundTrip mRoundTrip{};

    private:
    void onData (const std::span<const char>& data);
    void onClosed ();

    void processConnect (const std::span<const char>& data);
    void processPing ();
    void sendPing ();
    void sendError (const std::string& err);
    void sendOK ();
    void processPong ();
    void processSubscribe (const std::vector<std::string_view>& args);
    void processPublish (const PublishArg& publishArg, const std::span<const char>& data);
    void sendMessage (const std::span<const char>& data);

    friend class MessageParser;
};

} // namespace natsxx