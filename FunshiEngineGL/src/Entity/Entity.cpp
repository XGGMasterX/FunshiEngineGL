#include "Entity.h"
#include <algorithm>

Entity::Entity()
    : components(std::make_unique<ListaDE<Component*>>()) {
    auto transform = std::make_unique<Transform>();
    components->addLast(transform.get());
    componentOwners.push_back(std::move(transform));
}

Entity::Entity(Entity* origin)
    : components(std::make_unique<ListaDE<Component*>>()),
      parentEntity(origin),
      transformOrigin(origin != nullptr ? origin->getComponent<Transform>() : nullptr) {
    auto transform = std::make_unique<Transform>();
    components->addLast(transform.get());
    componentOwners.push_back(std::move(transform));
    if (parentEntity) {
        parentEntity->childEntities.push_back(this);
    }
}

Entity::~Entity() {
    if (parentEntity) {
        auto& sibs = parentEntity->childEntities;
        sibs.erase(std::remove(sibs.begin(), sibs.end(), this), sibs.end());
        parentEntity = nullptr;
    }
    for (auto* child : childEntities) {
        if (child) {
            child->parentEntity = nullptr;
            child->transformOrigin = nullptr;
        }
    }
    childEntities.clear();
    clearComponents();
}

void Entity::clearComponents() {
    if (components) components->clear();
    componentOwners.clear();
}

Binario* Entity::getMyBinario() { return myBinario.get(); }

void Entity::setOriginTransform(Transform* newOriginTransform) {
    ownedTransformOrigin.reset();
    transformOrigin = newOriginTransform;
}

void Entity::setParentEntity(Entity* parent) {
    if (parentEntity == parent) return;
    if (parentEntity) {
        auto& sibs = parentEntity->childEntities;
        sibs.erase(std::remove(sibs.begin(), sibs.end(), this), sibs.end());
    }
    parentEntity = parent;
    if (parentEntity) {
        parentEntity->childEntities.push_back(this);
        setOriginTransform(parentEntity->getComponent<Transform>());
    } else {
        setOriginTransform(nullptr);
    }
}

