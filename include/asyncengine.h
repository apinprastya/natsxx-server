#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <string>

namespace natsxx {

class Client;

class ServerTransport {
    public:
    virtual int listen ()                              = 0;
    virtual int bind (const std::string& ip, int port) = 0;
    virtual void close ()                              = 0;
    virtual void onNewClient (
    std::function<void (std::shared_ptr<Client>)> onNewClientCallback) = 0;
};

class Timer {
    public:
    virtual void onTimeout (std::function<void ()> onTimeout) = 0;
    virtual void start (std::chrono::duration<uint64_t, std::milli> duration,
    std::chrono::duration<uint64_t, std::milli> repeat)       = 0;
    virtual void stop ()                                      = 0;
};

class AsyncEngine {
    public:
    virtual std::shared_ptr<ServerTransport> newServerTransport () = 0;
    virtual int runLoop ()                                         = 0;
    virtual void stop ()                                           = 0;
    virtual std::shared_ptr<Timer> newTimer ()                     = 0;
};
} // namespace natsxx