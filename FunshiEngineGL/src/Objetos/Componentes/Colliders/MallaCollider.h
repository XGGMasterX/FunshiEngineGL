#ifndef MALLACOLLIDER_H
#define MALLACOLLIDER_H
#include "Collider.h"
#include <GL/gl.h>
class MallaCollider : public Collider {
public:
    MallaCollider(float radio, Transform* transformOfDadObject) :
        Collider(radio, transformOfDadObject) {

    }
    virtual btCollisionShape* createCollisionShape() override {
        //TODO
        return nullptr;
    }

    virtual void dibujarCollider() override {
        //DIBUJAR EN VERDE
    }
};
#endif