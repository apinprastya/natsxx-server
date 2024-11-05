#include "subscriber.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>


struct SubscribeTestItem {
    std::string topic;
    std::vector<std::string> subscribers;
};

TEST (Topic, Segment) {
    natsxx::SubscriberNode<std::string> pubsub;

    // Subscriptions
    int pubRes = pubsub.subscribe ("aa.bb.*", "aa.bb.*");
    EXPECT_EQ (pubRes, 0);
    pubRes = pubsub.subscribe ("aa.*", "aa.*");
    EXPECT_EQ (pubRes, 0);
    pubRes = pubsub.subscribe ("aa.>", "aa.>");
    EXPECT_EQ (pubRes, 0);
    pubRes = pubsub.subscribe ("aa.*.cc", "aa.*.cc");
    EXPECT_EQ (pubRes, 0);
    pubRes = pubsub.subscribe ("aa.*.cc.dd", "aa.*.cc.dd");
    EXPECT_EQ (pubRes, 0);
    pubRes = pubsub.subscribe ("aa.*.>", "aa.*.>");
    EXPECT_EQ (pubRes, 0);
    pubRes = pubsub.subscribe ("aa.*.>", "aa.*.>");
    EXPECT_EQ (pubRes, -1);


    std::vector<SubscribeTestItem> testCase{
        { SubscribeTestItem{ "aa.bb.cc",
        std::vector<std::string>{ "aa.bb.*", "aa.>", "aa.*.cc", "aa.*.>" } } },
        { SubscribeTestItem{
        "aa.bbb.cc", std::vector<std::string>{ "aa.>", "aa.*.cc", "aa.*.>" } } },
        { SubscribeTestItem{ "aa.bbb.*", std::vector<std::string>{ "aa.>", "aa.*.>" } } },
        { SubscribeTestItem{ "aa.bbb.cc.dd",
        std::vector<std::string>{ "aa.>", "aa.*.>", "aa.*.cc.dd" } } },
    };

    for (const auto& item : testCase) {
        auto result = pubsub.getSubscriber (item.topic);
        EXPECT_EQ (result.size (), item.subscribers.size ());
        for (const auto& topic : item.subscribers) {
            auto it = std::find (result.begin (), result.end (), topic);
            EXPECT_TRUE (it != result.end ());
        }
    }
}

int main (int argc, char** argv) {
    ::testing::InitGoogleTest (&argc, argv);
    return RUN_ALL_TESTS ();
}