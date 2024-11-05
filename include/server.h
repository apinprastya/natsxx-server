#pragma once

#include "asyncengine.h"
#include "subscriber.h"
#include <memory>
#include <nlohmann/json.hpp>

namespace natsxx {

class Client;

struct Info {
    std::string id{};
    std::string name{};
    std::string version{};
    int proto{};
    std::string host{};
    int port{};
    bool headers{};
    int maxPayload{};
    std::string clientIp{};
    int clientId{};

    friend void to_json (nlohmann::json& j, const Info& p) {
        j = nlohmann::json{
            { "id", p.id },
            { "name", p.name },
            { "version", p.version },
            { "proto", p.proto },
            { "host", p.host },
            { "port", p.port },
            { "headers", p.headers },
            { "max_payload", p.maxPayload },
            { "client_ip", p.clientIp },
            { "client_id", p.clientId },
        };
    }
};

struct ServerConfig {
    int pingIntervalSecond{ 5 };
};

class Server : public std::enable_shared_from_this<Server> {
    public:
    Server (AsyncEngine* asyncEngine);
    int run ();

    inline ServerConfig* getServerConfig () {
        return &mServerConfig;
    }

    inline AsyncEngine* getAsyncEngine () {
        return mAsyncEngine;
    }

    inline SubscriberManager* getSubscriberManager () {
        return &mSubscriberManager;
    }

    private:
    int mClientIdCounter{};
    ServerConfig mServerConfig;
    SubscriberManager mSubscriberManager;
    AsyncEngine* mAsyncEngine;
    std::vector<std::shared_ptr<Client>> mClients{};
};

} // namespace natsxx