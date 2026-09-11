#ifndef EVENT_BUS_H
#define EVENT_BUS_H

#include <functional>
#include <unordered_map>

class GameObject;

enum class SceneEventType {
    ObjectCreated,
    ObjectDeleted,
    ObjectReparented,
    ComponentChanged,
    SceneCleared,
    ObjectSelected
};

struct SceneEvent {
    SceneEventType type;
    GameObject* object = nullptr;
    GameObject* relatedObject = nullptr;
};

class EventBus {
private:
    using Callback = std::function<void(const SceneEvent&)>;
    std::unordered_map<size_t, Callback> listeners;
    size_t nextToken = 1;

public:
    size_t subscribe(Callback callback) ;

    void unsubscribe(size_t token) ;

    void publish(const SceneEvent& event) ;
};

#endif
