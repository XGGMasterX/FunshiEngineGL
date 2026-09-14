#include "SettingsColliderMalla.h"

#include "../Transform/SettingsTransform.h"
#include "../../../Objetos/GameObject.h"
#include "../../../Objetos/Componentes/Colliders/MallaCollider.h"
#include "../../../Scenes/EditorController.h"
#include <imgui.h>

SettingsColliderMalla::SettingsColliderMalla(GameObject* objeto) {
	myCollider = objeto->getComponent<MallaCollider>();
	this->settingsTransform =
	    new SettingsTransform(myCollider->getTransform(), objeto);
	this->newRadio = myCollider->getRadio();
}

SettingsColliderMalla::~SettingsColliderMalla() { delete settingsTransform; }

void SettingsColliderMalla::setEditor(EditorController* editor) {
	this->editor = editor;
}

void SettingsColliderMalla::showDataComponent() {
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

Component* SettingsColliderMalla::getComponent() { return myCollider; }