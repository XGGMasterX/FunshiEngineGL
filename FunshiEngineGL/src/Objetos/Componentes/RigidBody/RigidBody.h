#ifndef RIGIDBODY_H
#define RIGIDBODY_H
#include "../Colliders/Collider.h"
#include <btBulletDynamicsCommon.h>

class RigidBody : public Component {
private:
    btRigidBody* rigidBody = nullptr;
    Collider* collider = nullptr;
    float mass = 1.0f;
    // Guarda la posición y rotación para serializar
    float pos[3];
    float rot[4]; // quaternion x,y,z,w

    void serializeComponent(std::ofstream* fileNamePathContentObject) override {
        // Guarda masa
        fileNamePathContentObject->write(reinterpret_cast<const char*>(&mass), sizeof(float));
        // Guarda posición y rotación
        fileNamePathContentObject->write(reinterpret_cast<const char*>(pos), sizeof(float) * 3);
        fileNamePathContentObject->write(reinterpret_cast<const char*>(rot), sizeof(float) * 4);

        // También podrías guardar el tipo de collider para reconstruirlo al cargar
    }

    void deserializeComponent(std::ifstream* fileNamePathContentObject) override {
        // Leer masa
        fileNamePathContentObject->read(reinterpret_cast<char*>(&mass), sizeof(float));
        // Leer posición y rotación
        fileNamePathContentObject->read(reinterpret_cast<char*>(pos), sizeof(float) * 3);
        fileNamePathContentObject->read(reinterpret_cast<char*>(rot), sizeof(float) * 4);
    }

public:
    RigidBody(Collider* collider, float mass) : collider(collider), mass(mass) {
        pos[0] = pos[1] = pos[2] = 0.0f;
        rot[0] = 0.0f; // ángulo en grados
        rot[1] = 0.0f;
        rot[2] = 1.0f; // eje Y por defecto
        rot[3] = 0.0f;
        createRigidBody();
    }

    ~RigidBody() {
        if (rigidBody) {
            delete rigidBody->getMotionState();
            delete rigidBody;
        }
    }

    void createRigidBody() {
        // 1. Obtener la posición GLOBAL del collider (no la local)
        Transform* globalTransform = collider->getGlobalTransform(); // ¡Usamos getGlobalTransform()!
        float* tr = globalTransform->getTranslatef();
        pos[0] = tr[0];
        pos[1] = tr[1];
        pos[2] = tr[2];

        // 2. Obtener la rotación (ya en ángulo+eje, asumiendo que getRotatef() devuelve valores globales)
        float* rotAxisAngle = globalTransform->getRotatef(); // [ángulo, ejeX, ejeY, ejeZ]

        // Convertir de ángulo+eje a cuaternión
        btVector3 axis(rotAxisAngle[1], rotAxisAngle[2], rotAxisAngle[3]);
        if (axis.length2() == 0) {
            axis = btVector3(0, 1, 0); // eje por defecto si no hay rotación
        }
        btQuaternion q;
        q.setRotation(axis.normalized(), rotAxisAngle[0] * SIMD_RADS_PER_DEG);

        // Guardar cuaternión en rot[]
        rot[0] = q.x();
        rot[1] = q.y();
        rot[2] = q.z();
        rot[3] = q.w();

        // Crear shape desde el collider
        btCollisionShape* shape = collider->createCollisionShape();

        // Configurar transform inicial del rigid body
        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(pos[0], pos[1], pos[2])); // Posición global correcta
        startTransform.setRotation(q);

        // Calcular inercia (si mass != 0)
        btVector3 localInertia(0, 0, 0);
        if (mass != 0.f)
            shape->calculateLocalInertia(mass, localInertia);

        // Crear motion state y rigid body
        btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
        rigidBody = new btRigidBody(rbInfo);
    }


    //ALTERAR EL DAD TRANSFORM
    void syncPhysicsToGameObject() {
        btTransform trans;
        rigidBody->getMotionState()->getWorldTransform(trans);
        btVector3 origin = trans.getOrigin();  // Posición global deseada (física)
        btQuaternion quat = trans.getRotation();

        // 1. Obtener los transforms
        Transform* dadTransform = collider->getDadTransform();
        Transform* colliderTransform = collider->getTransform();

        if (dadTransform != nullptr) {
            // 2. Guardar el offset local actual del collider (antes de mover al padre)
            float offsetX = colliderTransform->getTranslatef()[0];
            float offsetY = colliderTransform->getTranslatef()[1];
            float offsetZ = colliderTransform->getTranslatef()[2];

            // 3. Mover al padre a la posición global deseada MENOS el offset local
            dadTransform->setTranslatef(
                origin.x() - offsetX,
                origin.y() - offsetY,
                origin.z() - offsetZ
            );

            // 4. El collider mantiene su offset local (no necesita cambios)
        }
        else {
            // Si no hay padre, actualizar directamente el collider
            colliderTransform->setTranslatef(origin.x(), origin.y(), origin.z());
        }

        // Rotación (opcional, misma lógica que antes)
        btScalar angle = quat.getAngle();
        btVector3 axis = quat.getAxis();
        colliderTransform->setRotatef(btDegrees(angle), axis.x(), axis.y(), axis.z());

        // Guardar datos en los arrays
        pos[0] = origin.x(); pos[1] = origin.y(); pos[2] = origin.z();
        rot[0] = quat.x(); rot[1] = quat.y(); rot[2] = quat.z(); rot[3] = quat.w();
    }

    void saveComponent(std::ofstream* fileNamePathContentObject) override {
        serializeComponent(fileNamePathContentObject);
    }

    void loadComponent(std::ifstream* fileNamePathContentObject) override {
        deserializeComponent(fileNamePathContentObject);

        // Luego crea el rigidBody con estos datos
        createRigidBody();
    }

    btRigidBody* getRigidBody() {
        return rigidBody;
    }
};

#endif