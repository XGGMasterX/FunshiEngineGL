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

    // Unica fuente de verdad de la seleccion del editor. GUI, gizmo e
    // inspectores leen de aqui y publican cambios con selectObject() para que
    // el resto de sistemas se sincronize via EventBus.
    GameObject* selected = nullptr;

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

    // Seleccion del editor: unica fuente de verdad compartida por GUI y gizmo.
    GameObject* getSelectedObject() const noexcept { return selected; }
    // Valida que el objeto siga en la escena y propaga ObjectSelected.
    void selectObject(GameObject* object);
    void clearSelection();
};

#endif
