#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include <memory>

class IPhysicsBackend;
class RigidBody;

// Fachada del mundo fisico (patron Facade).
//
// Header liviano: no expone Bullet. El backend concreto (BulletPhysicsAdapter)
// se oculta detras de IPhysicsBackend (patron Strategy) y se inyecta por
// constructor; con nullptr se puede montar la fachada sin mundo (tests/editor).
class PhysicsEngine {
public:
    PhysicsEngine();
    explicit PhysicsEngine(std::unique_ptr<IPhysicsBackend> backend);
    ~PhysicsEngine();

    PhysicsEngine(const PhysicsEngine&) = delete;
    PhysicsEngine& operator=(const PhysicsEngine&) = delete;

    void stepSimulation(float deltaTime);
    void addRigidBody(RigidBody* body);
    void removeRigidBody(RigidBody* body);

private:
    std::unique_ptr<IPhysicsBackend> backend_;
};
#endif