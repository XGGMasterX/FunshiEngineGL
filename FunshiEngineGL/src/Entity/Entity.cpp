/*
    FunshiEngineGL - Motor de juegos 3D con OpenGL e ImGui
    Copyright 2026 Gianfranco Ivan Enrique

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    SPDX-License-Identifier: Apache-2.0
*/
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

