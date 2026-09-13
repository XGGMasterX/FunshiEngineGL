#ifndef SETTINGSRIGIDBODY_H
#define SETTINGSRIGIDBODY_H
#include "../SettingsComponent.h"
class GameObject;
class RigidBody;

class SettingsRigidBody : public SettingsComponent {
protected:
	RigidBody* myCollider;
	bool stateRigidBody = false;
public:
	SettingsRigidBody(GameObject* objeto);

	// CONFIGURAR EL SISTEMA PARA QUE AL ACTIVAR SEA UN GHOST BODY, FALSO UN COMUN BODY
	void showDataComponent() override;
	Component* getComponent() override;
};
#endif