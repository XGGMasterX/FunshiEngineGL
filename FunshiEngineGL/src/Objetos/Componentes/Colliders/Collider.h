#ifndef COLLIDER_H
#define COLLIDER_H
#include "../Component.h"
#include "../Transform.h"
#include <btBulletDynamicsCommon.h>

class Collider : public Component {
protected:
	float radio;
	Transform* transformOfDadObject;
	Transform* myTransform;

	void serializeComponent(std::ofstream* fileNamePathContentObject) override {
		fileNamePathContentObject->write(reinterpret_cast<const char*>(&radio), sizeof(float));
		transformOfDadObject->saveComponent(fileNamePathContentObject);
		myTransform->saveComponent(fileNamePathContentObject);
	}

	void deserializeComponent(std::ifstream* fileNamePathContentObject) override {
		fileNamePathContentObject->read(reinterpret_cast<char*>(&radio), sizeof(float));
		transformOfDadObject->loadComponent(fileNamePathContentObject);
		myTransform->loadComponent(fileNamePathContentObject);
	}

public:

	//radio debe ser positivo
	Collider(float radio,Transform* transformOfDadObject) {
		this->radio = radio;
		this->transformOfDadObject = transformOfDadObject;
		this->myTransform = new Transform();
	}

	void saveComponent(std::ofstream* fileNamePathContentObject) override {
		serializeComponent(fileNamePathContentObject);
	}

	void loadComponent(std::ifstream* fileNamePathContentObject) override {
		deserializeComponent(fileNamePathContentObject);
	}

	virtual btCollisionShape* createCollisionShape() = 0;

	//radio debe ser positivo
	virtual void setRadio(float radio) {
		if (radio > 0) {
			this->radio = radio;
		}
	}
	virtual float getRadio() {
		return radio;
	}

	Transform* getDadTransform() {
		return transformOfDadObject;
	}

	//ME DA LA POSICION REAL EN COORDENADAS DESPLAZADAS (PADRE)
	Transform* getTransform() {
		return myTransform;
	}
	Transform* getGlobalTransform() {
		Transform* resultado = new Transform();
		float* myPos = myTransform->getTranslatef();
		float* dadPos = transformOfDadObject->getTranslatef();

		//me doy el desplazamiento del padre heredado
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

	virtual void dibujarCollider() = 0;

	//seria ideal crear un metodo que recorra todos los objetos
	//obtenga sus collider y verifique si se chocan con el mio
	//si es true entonces devolver quien es el objeto
	virtual bool isCollision(Collider* other) {
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

		float distanciaSinProcesar = (dxmyTransform - dxotherTransform)*(dxmyTransform - dxotherTransform) +
			(dymyTransform - dyotherTransform)*(dymyTransform - dyotherTransform) +
			(dzmyTransform - dzotherTransform)*(dzmyTransform - dzotherTransform);
		float distancia = sqrt(distanciaSinProcesar);
		return distancia < getRadio()+other->getRadio();
	}
};
#endif