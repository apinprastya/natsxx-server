#pragma once

#include "transport.h"
#include <uvw.hpp>

namespace natsxx {

class TransportUvw : public Transport {
    public:
    TransportUvw (std::shared_ptr<uvw::tcp_handle> client);
    virtual void init () override;
    virtual int send (const std::span<const char>& data) override;
    virtual int close () override;
    virtual void onData (
    std::function<void (const std::span<const char>&)> onDataCallback) override;
    virtual void onClosed (std::function<void ()> onClosedCallback) override;

    protected:
    void onDataUvw (const uvw::data_event& data, uvw::tcp_handle& client);

    private:
    std::shared_ptr<uvw::tcp_handle> mClient;
    std::function<void (std::span<char>)> mOnDataCallback;
    std::function<void ()> mOnClosedCallback;
};

} // namespace natsxx