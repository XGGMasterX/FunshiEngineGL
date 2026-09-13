#include "SettingsColliderCubo.h"

#include "../Transform/SettingsTransform.h"
#include "../../../Objetos/GameObject.h"
#include "../../../Objetos/Componentes/Colliders/CubeCollider.h"
#include <imgui.h>

SettingsColliderCubo::SettingsColliderCubo(GameObject* objeto) {
	myCollider = objeto->getComponent<CubeCollider>();
	this->settingsTransform =
	    new SettingsTransform(myCollider->getTransform());
	this->newRadio = myCollider->getRadio();
}

SettingsColliderCubo::~SettingsColliderCubo() { delete settingsTransform; }

void SettingsColliderCubo::showDataComponent() {
	ImGui::InputFloat("Radio", &newRadio);
	if (ImGui::Button("Confirmar")) {
		myCollider->setRadio(newRadio);
	}
	settingsTransform->showDataComponent();
	myCollider->dibujarCollider();
}

Component* SettingsColliderCubo::getComponent() { return myCollider; }