#ifndef EDITOR_CONTROLLER_H
#define EDITOR_CONTROLLER_H

#include <memory>

class Component;
class GameObject;
class PhysicsEngine;
class SceneRegistry;
class EventBus;

/**
 * Application-facing editor operations. It does not own the registry or
 * physics world; both are injected and outlive this controller.
 */
class EditorController {
private:
    SceneRegistry* scene = nullptr;
    PhysicsEngine* physics = nullptr;
    EventBus* events = nullptr;

public:
    EditorController(SceneRegistry* scene, PhysicsEngine* physics = nullptr,
                     EventBus* events = nullptr);

    void setScene(SceneRegistry* scene) noexcept;
    void setPhysics(PhysicsEngine* physics) noexcept;
    void setEventBus(EventBus* events) noexcept;

    // Raw pointers are non-owning scene views.
    GameObject* createGameObject(std::unique_ptr<GameObject> object,
                                 GameObject* parent = nullptr);
    bool deleteGameObject(GameObject* object);
    bool deleteObjectByID(int id);
    bool reparentGameObject(GameObject* object, GameObject* parent);
    void clearScene();
    bool addComponent(GameObject* object, std::unique_ptr<Component> component);
    bool removeComponent(GameObject* object, Component* component);
};

#endif
