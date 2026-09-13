#ifndef CUBECOLLIDER_H
#define CUBECOLLIDER_H
#include "Collider.h"

class CubeCollider : public Collider {
public:
    CubeCollider(float radio, Transform* transformOfDadObject);

    btCollisionShape* createCollisionShape() override;
    void dibujarCollider() override;
};
#endif