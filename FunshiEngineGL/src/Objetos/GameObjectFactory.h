#ifndef GAME_OBJECT_FACTORY_H
#define GAME_OBJECT_FACTORY_H

#include <memory>

class GameObject;

class GameObjectFactory {
public:
    static std::unique_ptr<GameObject> createModelObject();
};

#endif
