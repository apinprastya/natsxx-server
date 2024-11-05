#include "server.h"
#include "client.h"
#include <cstring>
#include <memory>
#include <string>
#include <uvw.hpp>

using namespace natsxx;

Server::Server (AsyncEngine* asyncEngine) : mAsyncEngine (asyncEngine) {
}

int Server::run () {
    auto tcpServer = mAsyncEngine->newServerTransport ();
    tcpServer->onNewClient ([this] (std::shared_ptr<Client> client) {
        client->setup (++mClientIdCounter, this);
        mClients.push_back (client);
    });
    int bind = tcpServer->bind ("0.0.0.0", 4222);
    if (bind < 0) {
        return bind;
    }
    int listen = tcpServer->listen ();
    if (listen < 0) {
        return listen;
    }
    return mAsyncEngine->runLoop ();
}
