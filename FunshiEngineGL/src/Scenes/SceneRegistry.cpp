#include "SceneRegistry.h"

#include <algorithm>
#include <functional>

#include "../Objetos/GameObject.h"
#include "../Objetos/Modelos3D.h"
#include "../Objetos/Componentes/Transform.h"

SceneRegistry::SceneRegistry() { createDefaultRoot(); }

SceneRegistry::~SceneRegistry() {
    gameObjects.clear();
    while (!entitys.isEmpty()) entitys.deleteRoot();
    ownedGameObjects.clear();
}

void SceneRegistry::createDefaultRoot() {
    auto root = std::make_unique<Modelos3D>();
    root->setId(0);
    root->setParentEntity(nullptr);
    GameObject* rootRaw = root.get();
    ownedGameObjects.emplace_back(std::move(root));
    entitys.createRoot(rootRaw);
}

GameObject* SceneRegistry::getRoot() const noexcept {
    if (entitys.isEmpty()) return nullptr;
    return const_cast<ArbolEnlazado<GameObject*>*>(&entitys)
        ->rootOfTree()->getElement();
}

bool SceneRegistry::contains(GameObject* object) const noexcept {
    return object != nullptr &&
           std::any_of(ownedGameObjects.begin(), ownedGameObjects.end(),
                       [object](const std::unique_ptr<GameObject>& candidate) {
                           return candidate.get() == object;
                       });
}

void SceneRegistry::refreshTransformOrigins(Position<GameObject*>* parent) {
    if (!parent || !entitys.isInternal(parent)) return;
    auto* children = entitys.childsOf(parent);
    Position<Position<GameObject*>*>* child = children->first();
    while (child) {
        Position<GameObject*>* childPosition = child->getElement();
        childPosition->getElement()->setParentEntity(parent->getElement());
        refreshTransformOrigins(childPosition);
        child = (child != children->last()) ? children->next(child) : nullptr;
    }
    delete children;
}

void SceneRegistry::refreshGameObjectView() {
    gameObjects.clear();
    if (entitys.isEmpty()) return;

    refreshTransformOrigins(entitys.rootOfTree());
    std::function<void(Position<GameObject*>*)> appendChildren =
        [&](Position<GameObject*>* parent) {
            if (!entitys.isInternal(parent)) return;
            auto* children = entitys.childsOf(parent);
            Position<Position<GameObject*>*>* child = children->first();
            while (child) {
                Position<GameObject*>* childPosition = child->getElement();
                gameObjects.addLast(childPosition->getElement());
                appendChildren(childPosition);
                child = (child != children->last()) ? children->next(child) : nullptr;
            }
            delete children;
        };
    appendChildren(entitys.rootOfTree());
}

GameObject* SceneRegistry::createObject(std::unique_ptr<GameObject> object,
                                        GameObject* parent) {
    if (!object) return nullptr;
    if (entitys.isEmpty()) {
        object->setOriginTransform(nullptr);
        GameObject* raw = object.get();
        ownedGameObjects.emplace_back(std::move(object));
        entitys.createRoot(raw);
        refreshGameObjectView();
        return raw;
    }

    if (!parent) parent = getRoot();
    Position<GameObject*>* parentPosition =
        parent ? entitys.whatIsPositionOf(parent) : nullptr;
    if (!parentPosition) return nullptr;

    if (object->getId() == 0) {
        int nextId = 1;
        for (const auto& candidate : ownedGameObjects)
            nextId = std::max(nextId, candidate->getId() + 1);
        object->setId(nextId);
    }

    GameObject* raw = object.get();
    ownedGameObjects.emplace_back(std::move(object));
    entitys.addNodeChildOf(parentPosition, raw);
    raw->setParentEntity(parent);
    refreshGameObjectView();
    return raw;
}

bool SceneRegistry::replaceRoot(std::unique_ptr<GameObject> root) {
    if (!root) return false;
    clear();
    entitys.deleteRoot();
    ownedGameObjects.clear();
    root->setParentEntity(nullptr);
    GameObject* raw = root.get();
    ownedGameObjects.emplace_back(std::move(root));
    entitys.createRoot(raw);
    refreshGameObjectView();
    return true;
}

bool SceneRegistry::deleteObject(GameObject* object) {
    if (!contains(object) || entitys.isEmpty()) return false;
    Position<GameObject*>* position = entitys.whatIsPositionOf(object);
    if (!position) return false;

    if (position == entitys.rootOfTree()) {
        clear();
        return true;
    }

    entitys.deleteNode(position);
    ownedGameObjects.erase(
        std::remove_if(ownedGameObjects.begin(), ownedGameObjects.end(),
                       [object](const std::unique_ptr<GameObject>& candidate) {
                           return candidate.get() == object;
                       }),
        ownedGameObjects.end());
    refreshGameObjectView();
    return true;
}

bool SceneRegistry::deleteObjectByID(int id) {
    for (const auto& object : ownedGameObjects) {
        if (object->getId() == id) return deleteObject(object.get());
    }
    return false;
}

bool SceneRegistry::reparent(GameObject* object, GameObject* parent) {
    if (!contains(object) || !contains(parent) || object == parent ||
        object == getRoot())
        return false;
    Position<GameObject*>* objectPosition = entitys.whatIsPositionOf(object);
    Position<GameObject*>* parentPosition = entitys.whatIsPositionOf(parent);
    if (!objectPosition || !parentPosition) return false;

    try {
        Position<GameObject*>* current = parentPosition;
        while (current != entitys.rootOfTree()) {
            if (current == objectPosition) return false;
            current = entitys.dadOf(current);
        }
        entitys.positionToChildOf(parentPosition, objectPosition);
    } catch (...) {
        return false;
    }
    object->setParentEntity(parent);
    refreshGameObjectView();
    return true;
}

void SceneRegistry::clear() {
    gameObjects.clear();
    while (!entitys.isEmpty()) entitys.deleteRoot();
    ownedGameObjects.clear();
    createDefaultRoot();
}
