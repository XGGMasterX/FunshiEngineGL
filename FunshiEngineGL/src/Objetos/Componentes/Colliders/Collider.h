#ifndef COLLIDER_H
#define COLLIDER_H
#include <memory>
#include "../Component.h"
#include "../Transform.h"

class btCollisionShape;

// Collider: componente geometrico de colision.
//
// RAII: es duenio de su shape de Bullet (createCollisionShape la construye
// una sola vez, lazy) y de su transform local (myTransform). getGlobalTransform
// devuelve el Transform global POR VALOR: sin new/delete manuales.
class Collider : public Component {
protected:
	float radio;
	Transform* transformOfDadObject;
	std::unique_ptr<Transform> myTransform;
	std::unique_ptr<btCollisionShape> collisionShape;

	void serializeComponent(std::ofstream* fileNamePathContentObject) override;
	void deserializeComponent(std::ifstream* fileNamePathContentObject) override;

	// Construye la shape concreta del collider (se llama una sola vez).
	virtual std::unique_ptr<btCollisionShape> createCollisionShape() = 0;

public:
	Collider(float radio, Transform* transformOfDadObject);
	~Collider() override;

	void saveComponent(std::ofstream* fileNamePathContentObject) override;
	void loadComponent(std::ifstream* fileNamePathContentObject) override;

	// radio debe ser positivo
	virtual void setRadio(float radio);
	virtual float getRadio();

	Transform* getDadTransform() { return transformOfDadObject; }

	Transform* getTransform() { return myTransform.get(); }

	// Transform global (hereda la posicion del padre) POR VALOR.
	Transform getGlobalTransform() const;

	// Acceso no-duenio a la shape: la construye lazy y queda viva
	// mientras exista el collider.
	btCollisionShape* getCollisionShape();

	// Descarta la shape cacheada: se reconstruye lazy en el proximo
	// getCollisionShape() (se usa cuando cambia la malla del modelo).
	void invalidateCollisionShape();

	virtual void dibujarCollider() = 0;

	// seria ideal crear un metodo que recorra todos los objetos
	// obtenga sus collider y verifique si se chocan con el mio
	virtual bool isCollision(Collider* other);
};
#endif