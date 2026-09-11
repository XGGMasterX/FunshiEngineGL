#include "Entity.h"

Entity::Entity()
    : components(std::make_unique<ListaDE<Component*>>()) {
    auto transform = std::make_unique<Transform>();
    components->addLast(transform.get());
    componentOwners.push_back(std::move(transform));
}

Entity::Entity(Entity* origin)
    : components(std::make_unique<ListaDE<Component*>>()),
      transformOrigin(origin != nullptr ? origin->getComponent<Transform>() : nullptr) {
    auto transform = std::make_unique<Transform>();
    components->addLast(transform.get());
    componentOwners.push_back(std::move(transform));
}

Entity::~Entity() = default;

void Entity::clearComponents() {
    if (components) components->clear();
    componentOwners.clear();
}

Binario* Entity::getMyBinario() { return myBinario.get(); }

void Entity::setOriginTransform(Transform* newOriginTransform) {
    ownedTransformOrigin.reset();
    transformOrigin = newOriginTransform;
}
