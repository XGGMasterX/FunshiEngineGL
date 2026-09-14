#ifndef MALLACOLLIDER_H
#define MALLACOLLIDER_H
#include "Collider.h"

class GameObject;

class MallaCollider : public Collider {
private:
	// Duenio del modelo: no se libera aqui (vive en la escena).
	GameObject* meshOwner = nullptr;

public:
	MallaCollider(float radio, Transform* transformOfDadObject,
	              GameObject* meshOwner = nullptr);

	std::unique_ptr<btCollisionShape> createCollisionShape() override;
	void dibujarCollider() override;
};
#endif