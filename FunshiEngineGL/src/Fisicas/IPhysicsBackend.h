#ifndef IPHYSICS_BACKEND_H
#define IPHYSICS_BACKEND_H

class RigidBody;

class IPhysicsBackend {
public:
    virtual ~IPhysicsBackend() = default;
    virtual void stepSimulation(float deltaTime) = 0;
    virtual void addRigidBody(RigidBody* body) = 0;
    virtual void removeRigidBody(RigidBody* body) = 0;
};

#endif
