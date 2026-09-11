#ifndef BULLET_PHYSICS_ADAPTER_H
#define BULLET_PHYSICS_ADAPTER_H

#include "IPhysicsBackend.h"
#include <btBulletDynamicsCommon.h>

class BulletPhysicsAdapter final : public IPhysicsBackend {
private:
    btDefaultCollisionConfiguration* collisionConfig;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* overlappingPairCache;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;
    btCollisionShape* groundShape;
    btDefaultMotionState* groundMotionState;
    btRigidBody* groundRigidBody;

public:
    BulletPhysicsAdapter();
    ~BulletPhysicsAdapter() override;
    void stepSimulation(float deltaTime) override;
    void addRigidBody(RigidBody* body) override;
    void removeRigidBody(RigidBody* body) override;
};

#endif
