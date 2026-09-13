#ifndef MALLACOLLIDER_H
#define MALLACOLLIDER_H
#include "Collider.h"

class MallaCollider : public Collider {
public:
    MallaCollider(float radio, Transform* transformOfDadObject);

    btCollisionShape* createCollisionShape() override;
    void dibujarCollider() override;
};
#endif