#include "asyncengine_uvw.h"
#include "server.h"
#include <spdlog/common.h>
#include <spdlog/spdlog.h>


int main () {
    spdlog::set_level (spdlog::level::debug);

    natsxx::AsyncEngineUvw asyncEngine;
    natsxx::Server server (&asyncEngine);
    int ret = server.run ();
    if (ret != 0) {
        return ret;
    }

    return 0;
}