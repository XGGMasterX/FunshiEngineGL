#ifndef EDITOR_CONTROLLER_H
#define EDITOR_CONTROLLER_H

#include <memory>

class Component;
class GameObject;
class PhysicsEngine;
class SceneRegistry;
class EventBus;
class Transform;

// Objetivo generico del gizmo: el Transform LOCAL a editar mas el Transform
// GLOBAL del contexto padre (para recomponer/mover en espacio local). Con
// owner = objeto con RigidBody a sincronizar tras la edicion.
//
// Es la misma maquinaria para editar el transform de un GameObject o el
// transform local (offset) de un componente como un collider: cambia el par
// {local, parentGlobal} y el gizmo hace lo mismo.
struct GizmoTarget {
    Transform* local = nullptr;
    Transform* parentGlobal = nullptr;
    GameObject* owner = nullptr;
};

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

    // Objetivo del gizmo cuando NO es el transform del objeto seleccionado
    // (p. ej. el offset local de un collider). Se limpia al cambiar de
    // seleccion o borrar objetos.
    GizmoTarget gizmoTarget;

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

    // Registra los RigidBody de TODA la escena en el mundo de fisica. Al
    // deserializar una escena, los componentes se crean con GameObject::
    // addComponent (no via EditorController), por lo que los cuerpos nunca
    // entran al mundo y los colliders quedan con la shape de respaldo (la malla
    // aun no estaba cargada). Este metodo invalida la shape (se reconstruye con
    // la malla ya disponible) y recrea+registra cada cuerpo.
    void registerSceneRigidBodies();

    // Seleccion del editor: unica fuente de verdad compartida por GUI y gizmo.
    GameObject* getSelectedObject() const noexcept { return selected; }
    // Valida que el objeto siga en la escena y propaga ObjectSelected.
    void selectObject(GameObject* object);
    void clearSelection();

    // Objetivo del gizmo: si se setea un GizmoTarget, el gizmo de GameScene
    // edita ESE transform (local) en vez del del objeto seleccionado. Se usa
    // para editar el offset local del collider desde SettingsCollider*.
    void setGizmoTarget(const GizmoTarget& target);
    void clearGizmoTarget();
    bool hasGizmoTarget() const noexcept { return gizmoTarget.local != nullptr; }
    const GizmoTarget& getGizmoTarget() const noexcept { return gizmoTarget; }
};

#endif
