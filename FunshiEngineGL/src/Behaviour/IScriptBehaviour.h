#ifndef ISCRIPTBEHAVIOUR_H
#define ISCRIPTBEHAVIOUR_H
#include "../Objetos/GameObject.h"

class IScriptBehaviour {
public:
    virtual ~IScriptBehaviour() {}
    virtual void onStart(GameObject* owner) = 0;
    virtual void onUpdate(GameObject* owner, float deltaTime) = 0;
};
#endif