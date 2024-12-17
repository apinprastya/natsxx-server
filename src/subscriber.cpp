#include "subscriber.h"
#include "client.h"
#include <memory>
#include <vector>

namespace natsxx {

Subscriber::Subscriber (const std::string& subject, const std::string& subId, Client* client)
: mSubject (subject), mId (subId), mClient (client) {
}

std::vector<std::string> SubscriberManager::splitTopic (const std::string& topic) const {
    std::vector<std::string> parts;
    std::stringstream ss (topic);
    std::string segment;
    while (std::getline (ss, segment, '.')) {
        parts.push_back (segment);
    }
    return parts;
}

int SubscriberManager::addSubscriber (std::shared_ptr<Subscriber> subscriber) {
    auto parts = splitTopic (subscriber->getSubject ());
    return root.subscribe (parts, 0, subscriber);
}

void SubscriberManager::unsubscribeClientId (int id) {
    root.removeIf ([id] (std::shared_ptr<Subscriber> item) -> bool {
        return item->getClient ()->getId () == id;
    });
}

std::vector<std::shared_ptr<Subscriber>> SubscriberManager::getSubscriber (
const std::string& subject) {
    std::vector<std::shared_ptr<Subscriber>> result;
    auto parts = splitTopic (subject);
    root.getSubscriber (parts, 0, subject, result, false);
    return result;
}

} // namespace natsxx