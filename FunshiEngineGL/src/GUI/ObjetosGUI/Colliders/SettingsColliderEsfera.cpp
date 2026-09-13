#include "SettingsColliderEsfera.h"

#include "../Transform/SettingsTransform.h"
#include "../../../Objetos/GameObject.h"
#include "../../../Objetos/Componentes/Colliders/EsfereCollider.h"
#include <imgui.h>

SettingsColliderEsfera::SettingsColliderEsfera(GameObject* objeto) {
	myCollider = objeto->getComponent<EsfereCollider>();
	this->settingsTransform =
	    new SettingsTransform(myCollider->getTransform());
	this->newRadio = myCollider->getRadio();
}

SettingsColliderEsfera::~SettingsColliderEsfera() { delete settingsTransform; }

void SettingsColliderEsfera::showDataComponent() {
	ImGui::InputFloat("Radio", &newRadio);
	if (ImGui::Button("Confirmar")) {
		myCollider->setRadio(newRadio);
	}
	settingsTransform->showDataComponent();
	myCollider->dibujarCollider();
}

Component* SettingsColliderEsfera::getComponent() { return myCollider; }