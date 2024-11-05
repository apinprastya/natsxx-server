#include "transport_uvw.h"
#include "spdlog/spdlog.h"
#include <functional>
#include <span>
#include <uvw/stream.h>

namespace natsxx {

std::unique_ptr<char[]> createUniquePtrFromSpan (const std::span<const char>& data) {
    // Allocate new array to hold the data
    auto ptr = std::make_unique<char[]> (data.size ());

    // Copy the data from the span into the new array
    std::memcpy (ptr.get (), data.data (), data.size ());

    return ptr; // Return the unique_ptr
}

TransportUvw::TransportUvw (std::shared_ptr<uvw::tcp_handle> client)
: mClient (client) {
    mClient->on<uvw::data_event> (std::bind (&TransportUvw::onDataUvw, this,
    std::placeholders::_1, std::placeholders::_2));
    mClient->on<uvw::end_event> ([this] (const uvw::end_event&, uvw::tcp_handle& client) {
        spdlog::debug ("client socket closing");
        client.close ();
        if (mOnClosedCallback != nullptr) {
            mOnClosedCallback ();
        }
    });
}

void TransportUvw::init () {
}

int TransportUvw::send (const std::span<const char>& data) {
    if (mClient->closing ()) {
        return -1;
    }
    auto tempData = createUniquePtrFromSpan (data);
    return mClient->write (std::move (tempData), data.size ());
}

int TransportUvw::close () {
    mClient->close ();
    return 0;
}

void TransportUvw::onData (std::function<void (const std::span<const char>&)> onDataCallback) {
    mOnDataCallback = onDataCallback;
}

void TransportUvw::onClosed (std::function<void ()> onClosedCallback) {
    mOnClosedCallback = onClosedCallback;
}

void TransportUvw::onDataUvw (const uvw::data_event& data, uvw::tcp_handle& client) {
    if (mOnDataCallback != nullptr) {
        mOnDataCallback (std::span{ data.data.get (), data.length });
    }
}

} // namespace natsxx