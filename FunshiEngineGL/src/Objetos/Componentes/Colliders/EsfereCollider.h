#ifndef ESFERECOLLIDER_H
#define ESFERECOLLIDER_H
#include "Collider.h"

class EsfereCollider : public Collider {
public:
	EsfereCollider(float radio, Transform* transformOfDadObject);

	btCollisionShape* createCollisionShape() override;
	void dibujarCollider() override;
};
#endif