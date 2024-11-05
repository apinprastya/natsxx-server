#include "server.h"
#include <gtest/gtest.h>

TEST (JsonTest, JsonTest) {
    natsxx::Info myInfo;
    myInfo.name            = "My Name";
    const nlohmann::json j = myInfo;

    GTEST_LOG_ (INFO) << j;

    auto jsonTest = nlohmann::json::parse (R"(
  {
    "pi": 3.141,
    "happy": trueq
  }
)",
    nullptr, false);
    EXPECT_TRUE (jsonTest.is_discarded ());

    jsonTest = nlohmann::json::parse (R"(
  {
    "pi": 3.141,
    "happy": true
  }
)",
    nullptr, false);
    EXPECT_FALSE (jsonTest.is_discarded ());

    // GTEST_LOG_ (INFO) << jsonTest.is_discarded ();
    /*GTEST_LOG_ (INFO)
    << jsonTest.contains ("pi") << " " << jsonTest.contains ("notAvailable");*/
}