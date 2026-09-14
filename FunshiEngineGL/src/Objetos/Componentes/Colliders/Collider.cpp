#include "Collider.h"

#include <cmath>
#include <btBulletDynamicsCommon.h>

void Collider::serializeComponent(std::ofstream* fileNamePathContentObject) {
    fileNamePathContentObject->write(reinterpret_cast<const char*>(&radio),
                                     sizeof(float));
    transformOfDadObject->saveComponent(fileNamePathContentObject);
    myTransform->saveComponent(fileNamePathContentObject);
}

void Collider::deserializeComponent(std::ifstream* fileNamePathContentObject) {
    fileNamePathContentObject->read(reinterpret_cast<char*>(&radio),
                                    sizeof(float));
    transformOfDadObject->loadComponent(fileNamePathContentObject);
    myTransform->loadComponent(fileNamePathContentObject);
}

Collider::Collider(float radio, Transform* transformOfDadObject)
    : radio(radio),
      transformOfDadObject(transformOfDadObject),
      myTransform(std::make_unique<Transform>()) {}

Collider::~Collider() = default;

void Collider::saveComponent(std::ofstream* fileNamePathContentObject) {
    serializeComponent(fileNamePathContentObject);
}

void Collider::loadComponent(std::ifstream* fileNamePathContentObject) {
    deserializeComponent(fileNamePathContentObject);
}

void Collider::setRadio(float radio) {
    if (radio > 0) {
        this->radio = radio;
    }
}

float Collider::getRadio() { return radio; }

Transform Collider::getGlobalTransform() const {
    Transform resultado;
    float* myPos = myTransform->getTranslatef();
    float* dadPos = transformOfDadObject->getTranslatef();

    // desplazamiento del padre heredado
    float dx = myPos[0] + dadPos[0];
    float dy = myPos[1] + dadPos[1];
    float dz = myPos[2] + dadPos[2];
    resultado.setTranslatef(dx, dy, dz);
    float* myRots = myTransform->getRotatef();
    resultado.setRotatef(myRots[0], myRots[1], myRots[2], myRots[3]);
    float* myScales = myTransform->getScalef();
    float* dadScales = transformOfDadObject->getScalef();
    resultado.setScalef(myScales[0] * dadScales[0],
                        myScales[1] * dadScales[1],
                        myScales[2] * dadScales[2]);
    return resultado;
}

btCollisionShape* Collider::getCollisionShape() {
    if (!collisionShape) {
        collisionShape = createCollisionShape();
        // Respaldo defensivo: nunca devolver nullptr a RigidBody/Bullet.
        if (!collisionShape) {
            collisionShape = std::make_unique<btSphereShape>(radio);
        }
    }
    return collisionShape.get();
}

void Collider::invalidateCollisionShape() {
    collisionShape.reset();
}

bool Collider::isCollision(Collider* other) {
    if (!other) return false;

    Transform myT = this->getGlobalTransform();
    Transform otherT = other->getGlobalTransform();

    float* myPos = myT.getTranslatef();
    float* otherPos = otherT.getTranslatef();

    float dxmyTransform = myPos[0];
    float dymyTransform = myPos[1];
    float dzmyTransform = myPos[2];

    float dxotherTransform = otherPos[0];
    float dyotherTransform = otherPos[1];
    float dzotherTransform = otherPos[2];

    float distanciaSinProcesar =
        (dxmyTransform - dxotherTransform) *
            (dxmyTransform - dxotherTransform) +
        (dymyTransform - dyotherTransform) *
            (dymyTransform - dyotherTransform) +
        (dzmyTransform - dzotherTransform) * (dzmyTransform - dzotherTransform);
    float distancia = sqrt(distanciaSinProcesar);
    return distancia < getRadio() + other->getRadio();
}