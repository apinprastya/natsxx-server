#pragma once

#include "asyncengine.h"
#include <functional>
#include <memory>

namespace uvw {
class tcp_handle;
class timer_handle;
class loop;
} // namespace uvw

namespace natsxx {

class AsyncEngineUvw : public AsyncEngine {
    public:
    AsyncEngineUvw ();
    std::shared_ptr<ServerTransport> newServerTransport () override;
    int runLoop () override;
    void stop () override;
    std::shared_ptr<Timer> newTimer () override;

    private:
    std::shared_ptr<uvw::loop> mLoop;
};

class TimerUvw : public Timer {
    public:
    TimerUvw (std::shared_ptr<uvw::loop> loop);
    ~TimerUvw ();
    void onTimeout (std::function<void ()> onTimeout);
    void start (std::chrono::duration<uint64_t, std::milli> duration,
    std::chrono::duration<uint64_t, std::milli> repeat);
    void stop ();

    private:
    std::shared_ptr<uvw::timer_handle> mTimerHandler;
    std::function<void ()> mOnTimeout = nullptr;
};

class ServerTransportUvw : public ServerTransport {
    public:
    ServerTransportUvw (std::shared_ptr<uvw::loop> loop);

    int listen ();
    int bind (const std::string& ip, int port);
    void close ();
    void onNewClient (std::function<void (std::shared_ptr<Client>)> onNewClientCallback);

    private:
    std::shared_ptr<uvw::loop> mLoop;
    std::shared_ptr<uvw::tcp_handle> mTcp;
    std::function<void (std::shared_ptr<Client>)> mOnNewClientCallback = nullptr;
};

} // namespace natsxx