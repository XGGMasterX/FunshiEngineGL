#include "SettingsColliderCubo.h"

#include "../Transform/SettingsTransform.h"
#include "../../../Objetos/GameObject.h"
#include "../../../Objetos/Componentes/Colliders/CubeCollider.h"
#include "../../../Scenes/EditorController.h"
#include <imgui.h>

SettingsColliderCubo::SettingsColliderCubo(GameObject* objeto) {
	myCollider = objeto->getComponent<CubeCollider>();
	this->settingsTransform =
	    new SettingsTransform(myCollider->getTransform());
	this->newRadio = myCollider->getRadio();
}

SettingsColliderCubo::~SettingsColliderCubo() { delete settingsTransform; }

void SettingsColliderCubo::setEditor(EditorController* editor) {
	this->editor = editor;
}

void SettingsColliderCubo::showDataComponent() {
	ImGui::InputFloat("Radio", &newRadio);
	if (ImGui::Button("Confirmar")) {
		myCollider->setRadio(newRadio);
	}
	settingsTransform->showDataComponent();

	if (editor && myCollider->getOwner()) {
		bool activo = editor->hasGizmoTarget() &&
		              editor->getGizmoTarget().local == myCollider->getTransform();
		if (ImGui::Checkbox("Editar con gizmo", &activo)) {
			if (activo) {
				GizmoTarget t;
				t.local = myCollider->getTransform();
				t.parentGlobal = myCollider->getOwner()->getGlobalTransform();
				t.owner = myCollider->getOwner();
				editor->setGizmoTarget(t);
			} else {
				editor->clearGizmoTarget();
			}
		}
	}

	myCollider->dibujarCollider();
}

Component* SettingsColliderCubo::getComponent() { return myCollider; }