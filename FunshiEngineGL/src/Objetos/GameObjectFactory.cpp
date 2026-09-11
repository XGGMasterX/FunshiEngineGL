#include "GameObjectFactory.h"

#include "Modelos3D.h"

std::unique_ptr<GameObject> GameObjectFactory::createModelObject() {
    return std::make_unique<Modelos3D>();
}
