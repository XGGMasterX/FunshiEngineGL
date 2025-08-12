#ifndef SETTINGSRIGIDBODY_H
#define SETTINGSRIGIDBODY_H
#include "../Transform/SettingsTransform.h"
#include "../../../Objetos/GameObject.h"

class SettingsRigidBody : public SettingsComponent {
protected:
	RigidBody* myCollider;
public:
	SettingsRigidBody(GameObject* objeto) {
		myCollider = objeto->getComponent<RigidBody>();
	}

	//CONFIGURAR EL SISTEMA PARA QUE AL ACTIVAR SEA UN GHOST BODY , FALSO UN COMUN BODY
	virtual void showDataComponent() override {
		static bool stateRigidBody = false;
		if (ImGui::Checkbox("Activo", &stateRigidBody)) {
			//codigo
		}
	}

	virtual Component* getComponent() override {
		return myCollider;
	}
};
#endif