#ifndef COLLIDER_H
#define COLLIDER_H
#include "../Component.h"
#include "../Transform.h"

class btCollisionShape;

class Collider : public Component {
protected:
	float radio;
	Transform* transformOfDadObject;
	Transform* myTransform;

	void serializeComponent(std::ofstream* fileNamePathContentObject) override;
	void deserializeComponent(std::ifstream* fileNamePathContentObject) override;

public:
	Collider(float radio, Transform* transformOfDadObject);

	void saveComponent(std::ofstream* fileNamePathContentObject) override;
	void loadComponent(std::ifstream* fileNamePathContentObject) override;

	virtual btCollisionShape* createCollisionShape() = 0;

	// radio debe ser positivo
	virtual void setRadio(float radio);
	virtual float getRadio();

	Transform* getDadTransform() { return transformOfDadObject; }

	Transform* getTransform() { return myTransform; }

	Transform* getGlobalTransform();

	virtual void dibujarCollider() = 0;

	// seria ideal crear un metodo que recorra todos los objetos
	// obtenga sus collider y verifique si se chocan con el mio
	virtual bool isCollision(Collider* other);
};
#endif