#include "PhysicsEngine.h"

#include "../Objetos/Componentes/RigidBody/RigidBody.h"

void PhysicsEngine::addRigidBody(RigidBody* body) {
    if (body && body->getRigidBody()) dynamicsWorld->addRigidBody(body->getRigidBody());
}

void PhysicsEngine::removeRigidBody(RigidBody* body) {
    if (body && body->getRigidBody()) dynamicsWorld->removeRigidBody(body->getRigidBody());
}
