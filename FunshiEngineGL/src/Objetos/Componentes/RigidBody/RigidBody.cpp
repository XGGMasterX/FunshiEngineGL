#include "RigidBody.h"

#include <btBulletDynamicsCommon.h>

RigidBody::RigidBody(Collider* collider, float mass)
    : collider(collider), mass(mass) {
    pos[0] = pos[1] = pos[2] = 0.0f;
    rot[0] = 0.0f; // angulo en grados
    rot[1] = 0.0f;
    rot[2] = 1.0f; // eje Y por defecto
    rot[3] = 0.0f;
    createRigidBody();
}

RigidBody::~RigidBody() {
    if (rigidBody) {
        delete rigidBody->getMotionState();
        delete rigidBody;
    }
}

void RigidBody::createRigidBody() {
    // 1. Obtener la posicion GLOBAL del collider (no la local)
    Transform* globalTransform = collider->getGlobalTransform();
    float* tr = globalTransform->getTranslatef();
    pos[0] = tr[0];
    pos[1] = tr[1];
    pos[2] = tr[2];

    // 2. Obtener la rotacion (ya en angulo+eje)
    float* rotAxisAngle = globalTransform->getRotatef(); // [angulo, ejeX, ejeY, ejeZ]
    delete globalTransform;

// Convertir de angulo+eje a cuaternion (copiar antes de liberar el
    // transform global, que es quien posee los arrays de rotacion).
    btVector3 axis(rotAxisAngle[1], rotAxisAngle[2], rotAxisAngle[3]);
    delete globalTransform;
    if (axis.length2() == 0) {
        axis = btVector3(0, 1, 0); // eje por defecto si no hay rotacion
    }
    btQuaternion q;
    q.setRotation(axis.normalized(), rotAxisAngle[0] * SIMD_RADS_PER_DEG);

    // Guardar cuaternion en rot[]
    rot[0] = q.x();
    rot[1] = q.y();
    rot[2] = q.z();
    rot[3] = q.w();

    // Crear shape desde el collider
    btCollisionShape* shape = collider->createCollisionShape();

    // Configurar transform inicial del rigid body
    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(pos[0], pos[1], pos[2]));
    startTransform.setRotation(q);

    // Calcular inercia (si mass != 0)
    btVector3 localInertia(0, 0, 0);
    if (mass != 0.f) shape->calculateLocalInertia(mass, localInertia);

    // Crear motion state y rigid body
    btDefaultMotionState* motionState =
        new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape,
                                                    localInertia);
    rigidBody = new btRigidBody(rbInfo);
}

void RigidBody::syncPhysicsToGameObject() {
    btTransform trans;
    rigidBody->getMotionState()->getWorldTransform(trans);
    btVector3 origin = trans.getOrigin(); // Posicion global deseada (fisica)
    btQuaternion quat = trans.getRotation();

    // 1. Obtener los transforms
    Transform* dadTransform = collider->getDadTransform();
    Transform* colliderTransform = collider->getTransform();

    if (dadTransform != nullptr) {
        // 2. Guardar el offset local actual del collider (antes de mover al padre)
        float offsetX = colliderTransform->getTranslatef()[0];
        float offsetY = colliderTransform->getTranslatef()[1];
        float offsetZ = colliderTransform->getTranslatef()[2];

        // 3. Mover al padre a la posicion global deseada MENOS el offset local
        dadTransform->setTranslatef(origin.x() - offsetX, origin.y() - offsetY,
                                    origin.z() - offsetZ);
    } else {
        // Si no hay padre, actualizar directamente el collider
        colliderTransform->setTranslatef(origin.x(), origin.y(), origin.z());
    }

    // Rotacion (opcional, misma logica que antes)
    btScalar angle = quat.getAngle();
    btVector3 axis = quat.getAxis();
    colliderTransform->setRotatef(btDegrees(angle), axis.x(), axis.y(),
                                  axis.z());

    // Guardar datos en los arrays
    pos[0] = origin.x();
    pos[1] = origin.y();
    pos[2] = origin.z();
    rot[0] = quat.x();
    rot[1] = quat.y();
    rot[2] = quat.z();
    rot[3] = quat.w();
}

void RigidBody::saveComponent(std::ofstream* fileNamePathContentObject) {
    serializeComponent(fileNamePathContentObject);
}

void RigidBody::loadComponent(std::ifstream* fileNamePathContentObject) {
    deserializeComponent(fileNamePathContentObject);

    // Luego crea el rigidBody con estos datos
    createRigidBody();
}

void RigidBody::serializeComponent(std::ofstream* fileNamePathContentObject) {
    // Guarda masa
    fileNamePathContentObject->write(reinterpret_cast<const char*>(&mass),
                                     sizeof(float));
    // Guarda posicion y rotacion
    fileNamePathContentObject->write(reinterpret_cast<const char*>(pos),
                                     sizeof(float) * 3);
    fileNamePathContentObject->write(reinterpret_cast<const char*>(rot),
                                     sizeof(float) * 4);
    // Tambien deberia guardar el tipo de collider para reconstruirlo al cargar
}

void RigidBody::deserializeComponent(std::ifstream* fileNamePathContentObject) {
    // Leer masa
    fileNamePathContentObject->read(reinterpret_cast<char*>(&mass),
                                    sizeof(float));
    // Leer posicion y rotacion
    fileNamePathContentObject->read(reinterpret_cast<char*>(pos),
                                    sizeof(float) * 3);
    fileNamePathContentObject->read(reinterpret_cast<char*>(rot),
                                    sizeof(float) * 4);
}