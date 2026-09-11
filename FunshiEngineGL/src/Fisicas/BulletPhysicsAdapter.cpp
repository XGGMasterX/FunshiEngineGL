#include "BulletPhysicsAdapter.h"

#include "../Objetos/Componentes/RigidBody/RigidBody.h"

BulletPhysicsAdapter::BulletPhysicsAdapter()
    : collisionConfig(new btDefaultCollisionConfiguration()),
      dispatcher(new btCollisionDispatcher(collisionConfig)),
      overlappingPairCache(new btDbvtBroadphase()),
      solver(new btSequentialImpulseConstraintSolver()),
      dynamicsWorld(new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache,
                                                solver, collisionConfig)),
      groundShape(new btStaticPlaneShape(btVector3(0, 1, 0), 0)),
      groundMotionState(new btDefaultMotionState(
          btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)))),
      groundRigidBody(nullptr) {
    dynamicsWorld->setGravity(btVector3(0, -1.0f, 0));
    btRigidBody::btRigidBodyConstructionInfo info(
        0, groundMotionState, groundShape, btVector3(0, 0, 0));
    groundRigidBody = new btRigidBody(info);
    dynamicsWorld->addRigidBody(groundRigidBody);
}

BulletPhysicsAdapter::~BulletPhysicsAdapter() {
    if (dynamicsWorld && groundRigidBody)
        dynamicsWorld->removeRigidBody(groundRigidBody);
    delete groundRigidBody;
    delete groundMotionState;
    delete groundShape;
    delete dynamicsWorld;
    delete solver;
    delete overlappingPairCache;
    delete dispatcher;
    delete collisionConfig;
}

void BulletPhysicsAdapter::stepSimulation(float deltaTime) {
    dynamicsWorld->stepSimulation(deltaTime);
}

void BulletPhysicsAdapter::addRigidBody(RigidBody* body) {
    if (dynamicsWorld && body && body->getRigidBody())
        dynamicsWorld->addRigidBody(body->getRigidBody());
}

void BulletPhysicsAdapter::removeRigidBody(RigidBody* body) {
    if (dynamicsWorld && body && body->getRigidBody())
        dynamicsWorld->removeRigidBody(body->getRigidBody());
}
