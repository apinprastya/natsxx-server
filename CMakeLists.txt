cmake_minimum_required(VERSION 3.15)

project(natsxx VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(uvw REQUIRED)
find_package(CLI11 REQUIRED)
find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(fmt REQUIRED)
find_package(GTest REQUIRED)

enable_testing()

add_executable(${PROJECT_NAME} 
    src/main.cpp 
    src/cli.cpp 
    src/config.cpp
    src/server.cpp
    src/client.cpp
    src/transport_uvw.cpp
    src/parser.cpp
    src/asyncengine_uvw.cpp
    src/subscriber.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC include)

target_link_libraries(${PROJECT_NAME} uvw::uvw CLI11::CLI11 spdlog::spdlog nlohmann_json::nlohmann_json gtest::gtest fmt::fmt)

add_subdirectory(tests)