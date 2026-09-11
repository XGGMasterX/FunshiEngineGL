#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H


#include <btBulletDynamicsCommon.h>

class RigidBody;

class PhysicsEngine {
private:
    btDefaultCollisionConfiguration* collisionConfig;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* overlappingPairCache;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;

public:
    PhysicsEngine() {
        collisionConfig = new btDefaultCollisionConfiguration();
        dispatcher = new btCollisionDispatcher(collisionConfig);
        overlappingPairCache = new btDbvtBroadphase();
        solver = new btSequentialImpulseConstraintSolver();
        dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfig);
        getWorld()->setGravity(btVector3(0, -1.0f, 0));


        // Crear el suelo como un plano estático
        btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
        // El vector (0,1,0) es la normal del plano (eje Y hacia arriba)
        // El segundo parámetro es la distancia desde el origen al plano, 0 para que pase por Y=0

        // Crear un MotionState para el suelo
        btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
        // Lo posicionamos justo en Y = -1 para que el suelo quede justo debajo de Y=0 (ajustalo según necesites)

        // Masa cero indica que es estático (no se moverá)
        btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));

        // Crear el rigid body para el suelo
        btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);

        // Añadir el suelo al mundo dinámico
        getWorld()->addRigidBody(groundRigidBody);

    }

    ~PhysicsEngine() {
        delete dynamicsWorld;
        delete solver;
        delete overlappingPairCache;
        delete dispatcher;
        delete collisionConfig;
    }

    void stepSimulation(float deltaTime) {
        dynamicsWorld->stepSimulation(deltaTime);
    }

    void addRigidBody(btRigidBody* body) {
        dynamicsWorld->addRigidBody(body);
    }

    void addRigidBody(RigidBody* body);
    void removeRigidBody(RigidBody* body);

    btDiscreteDynamicsWorld* getWorld() { return dynamicsWorld; }
};
#endif
