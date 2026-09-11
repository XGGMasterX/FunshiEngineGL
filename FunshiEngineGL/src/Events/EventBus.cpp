#include "EventBus.h"

size_t EventBus::subscribe(Callback callback) {
    size_t token = nextToken++;
    listeners[token] = callback;
    return token;
}

void EventBus::unsubscribe(size_t token) {
    listeners.erase(token);
}

void EventBus::publish(const SceneEvent& event) {
    for (auto& pair : listeners) {
        pair.second(event);
    }
}
