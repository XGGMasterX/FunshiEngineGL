#include "MallaCollider.h"

#include <btBulletDynamicsCommon.h>

MallaCollider::MallaCollider(float radio, Transform* transformOfDadObject)
    : Collider(radio, transformOfDadObject) {}

btCollisionShape* MallaCollider::createCollisionShape() {
    // TODO: construir la shape desde la malla del modelo.
    return nullptr;
}

void MallaCollider::dibujarCollider() {
    // TODO: dibujar el collider en verde.
}