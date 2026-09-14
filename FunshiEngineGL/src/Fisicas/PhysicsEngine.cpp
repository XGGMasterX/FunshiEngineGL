#include "PhysicsEngine.h"

#include "BulletPhysicsAdapter.h"
#include "IPhysicsBackend.h"

PhysicsEngine::PhysicsEngine()
    : backend_(std::make_unique<BulletPhysicsAdapter>()) {}

PhysicsEngine::PhysicsEngine(std::unique_ptr<IPhysicsBackend> backend)
    : backend_(std::move(backend)) {}

PhysicsEngine::~PhysicsEngine() = default;

void PhysicsEngine::stepSimulation(float deltaTime) {
    if (backend_) backend_->stepSimulation(deltaTime);
}

void PhysicsEngine::addRigidBody(RigidBody* body) {
    if (backend_) backend_->addRigidBody(body);
}

void PhysicsEngine::removeRigidBody(RigidBody* body) {
    if (backend_) backend_->removeRigidBody(body);
}