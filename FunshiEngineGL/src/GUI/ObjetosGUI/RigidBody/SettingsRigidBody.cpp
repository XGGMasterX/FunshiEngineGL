#include "SettingsRigidBody.h"

#include "../../../Objetos/GameObject.h"
#include <imgui.h>

SettingsRigidBody::SettingsRigidBody(GameObject* objeto) {
	myCollider = objeto->getComponent<RigidBody>();
}

void SettingsRigidBody::showDataComponent() {
	if (ImGui::Checkbox("Activo", &stateRigidBody)) {
		// CONFIGURAR EL SISTEMA PARA QUE AL ACTIVAR SEA UN GHOST BODY,
		// FALSO UN COMUN BODY
	}
}

Component* SettingsRigidBody::getComponent() { return myCollider; }