#include "Collider.h"

#include <cfloat>
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

Collider::Collider(float radio, Transform* transformOfDadObject) {
    this->radio = radio;
    this->transformOfDadObject = transformOfDadObject;
    this->myTransform = new Transform();
}

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

Transform* Collider::getGlobalTransform() {
    Transform* resultado = new Transform();
    float* myPos = myTransform->getTranslatef();
    float* dadPos = transformOfDadObject->getTranslatef();

    // desplazamiento del padre heredado
    float dx = myPos[0] + dadPos[0];
    float dy = myPos[1] + dadPos[1];
    float dz = myPos[2] + dadPos[2];
    resultado->setTranslatef(dx, dy, dz);
    float* myRots = myTransform->getRotatef();
    resultado->setRotatef(myRots[0], myRots[1], myRots[2], myRots[3]);
    float* myScales = myTransform->getScalef();
    resultado->setScalef(myScales[0], myScales[1], myScales[2]);
    return resultado;
}

bool Collider::isCollision(Collider* other) {
    Transform* myTransform = this->getGlobalTransform();
    Transform* otherTransform = other->getGlobalTransform();

    if (!myTransform || !otherTransform) return FLT_MAX;

    float* myPos = myTransform->getTranslatef();
    float* otherPos = otherTransform->getTranslatef();

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