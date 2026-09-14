#include "EditorController.h"

#include "SceneRegistry.h"
#include "../Fisicas/PhysicsEngine.h"
#include "../Objetos/GameObject.h"
#include "../Objetos/Componentes/RigidBody/RigidBody.h"
#include "../Events/EventBus.h"

EditorController::EditorController(SceneRegistry* value, PhysicsEngine* world,
                                   EventBus* bus)
    : scene(value), physics(world), events(bus) {}

void EditorController::setScene(SceneRegistry* value) noexcept { scene = value; }
void EditorController::setPhysics(PhysicsEngine* value) noexcept { physics = value; }
void EditorController::setEventBus(EventBus* value) noexcept { events = value; }

GameObject* EditorController::createGameObject(std::unique_ptr<GameObject> object,
                                               GameObject* parent) {
    GameObject* created = scene ? scene->createObject(std::move(object), parent) : nullptr;
    if (created && events)
        events->publish({SceneEventType::ObjectCreated, created, parent});
    return created;
}

bool EditorController::deleteGameObject(GameObject* object) {
    if (!scene || !scene->contains(object)) return false;
    if (object == scene->getRoot()) {
        clearScene();
        return true;
    }
    if (physics && object) {
        if (RigidBody* body = object->getComponent<RigidBody>())
            physics->removeRigidBody(body);
    }
    if (events)
        events->publish({SceneEventType::ObjectDeleted, object, nullptr});
    // La seleccion no debe apuntar a un objeto que se va a liberar.
    if (selected == object) {
        selected = nullptr;
        if (events)
            events->publish({SceneEventType::ObjectSelected, nullptr, nullptr});
    }
    const bool deleted = scene->deleteObject(object);
    return deleted;
}

bool EditorController::deleteObjectByID(int id) {
    if (!scene) return false;
    scene->refreshGameObjectView();
    GameObject* object = nullptr;
    if (scene->getGameObjects() && !scene->getGameObjects()->isEmpty()) {
        auto* position = scene->getGameObjects()->first();
        while (position) {
            if (position->getElement()->getId() == id) {
                object = position->getElement();
                break;
            }
            position = (position != scene->getGameObjects()->last())
                           ? scene->getGameObjects()->next(position)
                           : nullptr;
        }
    }
    if (!object && scene->getRoot() && scene->getRoot()->getId() == id)
        object = scene->getRoot();
    return deleteGameObject(object);
}

bool EditorController::reparentGameObject(GameObject* object, GameObject* parent) {
    const bool changed = scene && scene->reparent(object, parent);
    if (changed && events)
        events->publish({SceneEventType::ObjectReparented, object, parent});
    return changed;
}

void EditorController::clearScene() {
    if (!scene) return;
    scene->refreshGameObjectView();
    auto* objects = scene->getGameObjects();
    if (physics && objects && !objects->isEmpty()) {
        auto* position = objects->first();
        while (position) {
            GameObject* object = position->getElement();
            if (object) {
                if (auto* body = object->getComponent<RigidBody>())
                    physics->removeRigidBody(body);
            }
            position = (position != objects->last())
                           ? objects->next(position)
                           : nullptr;
        }
    }
    if (physics && scene->getRoot()) {
        if (auto* body = scene->getRoot()->getComponent<RigidBody>())
            physics->removeRigidBody(body);
    }
    scene->clear();
    selected = nullptr;
    if (events) {
        events->publish({SceneEventType::SceneCleared, nullptr, nullptr});
        events->publish({SceneEventType::ObjectSelected, nullptr, nullptr});
    }
}

void EditorController::registerSceneRigidBodies() {
    if (!scene || !physics) return;
    scene->refreshGameObjectView();
    auto* objects = scene->getGameObjects();
    auto reRegistrar = [this](GameObject* object) {
        if (!object) return;
        RigidBody* body = object->getComponent<RigidBody>();
        if (!body) return;
        // La malla recien deserializada: descartar la shape provisional
        // (esfera de respaldo) y recrear el cuerpo con la shape real.
        if (Collider* collider = object->getComponent<Collider>())
            collider->invalidateCollisionShape();
        physics->removeRigidBody(body);
        body->createRigidBody();
        physics->addRigidBody(body);
    };
    if (objects && !objects->isEmpty()) {
        auto* position = objects->first();
        while (position) {
            reRegistrar(position->getElement());
            position = (position != objects->last())
                           ? objects->next(position)
                           : nullptr;
        }
    }
    reRegistrar(scene->getRoot());
}

void EditorController::selectObject(GameObject* object) {
    // Nunca seleccionar un puntero que ya no pertenece a la escena.
    if (object && scene && !scene->contains(object)) return;
    selected = object;
    if (events)
        events->publish({SceneEventType::ObjectSelected, selected, nullptr});
}

void EditorController::clearSelection() { selectObject(nullptr); }

bool EditorController::addComponent(GameObject* object,
                                    std::unique_ptr<Component> component) {
    if (!scene || !scene->contains(object) || !component) return false;
    RigidBody* body = dynamic_cast<RigidBody*>(component.get());
    object->addComponent(std::move(component));
    if (physics && body) physics->addRigidBody(body);
    if (events)
        events->publish({SceneEventType::ComponentChanged, object, nullptr});
    return true;
}

bool EditorController::removeComponent(GameObject* object, Component* component) {
    if (!scene || !scene->contains(object) || !component) return false;
    if (physics) {
        if (auto* body = dynamic_cast<RigidBody*>(component))
            physics->removeRigidBody(body);
    }
    object->deleteComponent(component);
    if (events)
        events->publish({SceneEventType::ComponentChanged, object, nullptr});
    return true;
}
