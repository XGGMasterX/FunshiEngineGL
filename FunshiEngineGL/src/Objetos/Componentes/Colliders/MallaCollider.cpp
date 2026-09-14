#include "MallaCollider.h"

#include <memory>
#include <btBulletDynamicsCommon.h>

MallaCollider::MallaCollider(float radio, Transform* transformOfDadObject)
    : Collider(radio, transformOfDadObject) {}

std::unique_ptr<btCollisionShape> MallaCollider::createCollisionShape() {
    // TODO: construir la shape desde la malla del modelo.
    // Mientras tanto se cae a la shape de respaldo de Collider (esfera).
    return nullptr;
}

void MallaCollider::dibujarCollider() {
    // TODO: dibujar el collider en verde.
}