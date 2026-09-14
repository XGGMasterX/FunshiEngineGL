#ifndef ESFERECOLLIDER_H
#define ESFERECOLLIDER_H
#include "Collider.h"

class EsfereCollider : public Collider {
public:
	EsfereCollider(float radio, Transform* transformOfDadObject,
	               GameObject* owner = nullptr);

	std::unique_ptr<btCollisionShape> createCollisionShape() override;
	void dibujarCollider() override;
};
#endif