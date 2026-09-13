#include "SettingsColliderMalla.h"

#include "../Transform/SettingsTransform.h"
#include "../../../Objetos/GameObject.h"
#include "../../../Objetos/Componentes/Colliders/MallaCollider.h"
#include <imgui.h>

SettingsColliderMalla::SettingsColliderMalla(GameObject* objeto) {
	myCollider = objeto->getComponent<MallaCollider>();
	this->settingsTransform =
	    new SettingsTransform(myCollider->getTransform());
	this->newRadio = myCollider->getRadio();
}

SettingsColliderMalla::~SettingsColliderMalla() { delete settingsTransform; }

void SettingsColliderMalla::showDataComponent() {
	ImGui::InputFloat("Radio", &newRadio);
	if (ImGui::Button("Confirmar")) {
		myCollider->setRadio(newRadio);
	}
	settingsTransform->showDataComponent();
	myCollider->dibujarCollider();
}

Component* SettingsColliderMalla::getComponent() { return myCollider; }