#pragma once

#include <functional>
#include <span>

namespace natsxx {

class Transport {
    public:
    virtual ~Transport () {
    }
    virtual void init ()                                 = 0;
    virtual int send (const std::span<const char>& data) = 0;
    virtual int close ()                                 = 0;
    virtual void onData (
    std::function<void (const std::span<const char>&)> onDataCallback) = 0;
    virtual void onClosed (std::function<void ()>)                     = 0;
};

} // namespace natsxx