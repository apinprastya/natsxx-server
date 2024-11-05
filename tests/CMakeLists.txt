find_package(GTest REQUIRED CONFIG)
find_package(nlohmann_json REQUIRED)

# json
add_executable(JsonTest test_json.cpp)
target_include_directories(JsonTest PUBLIC ../include)
target_link_libraries(JsonTest nlohmann_json::nlohmann_json gtest::gtest)

# topic segments
add_executable(Subject test_subject.cpp)
target_include_directories(Subject PUBLIC ../include)
target_link_libraries(Subject gtest::gtest)

# parser
add_executable(Parser test_parser.cpp)
target_include_directories(Parser PUBLIC ../include)
target_link_libraries(Parser gtest::gtest)

# buffer
add_executable(Buffer test_buffer.cpp)
target_include_directories(Buffer PUBLIC ../include)
target_link_libraries(Buffer gtest::gtest)

# Register the test executable with CTest
add_test(NAME JsonTest COMMAND JsonTest)
add_test(NAME Subject COMMAND SubjectTest)
add_test(NAME Parser COMMAND Parser)
add_test(NAME Buffer COMMAND Buffer)