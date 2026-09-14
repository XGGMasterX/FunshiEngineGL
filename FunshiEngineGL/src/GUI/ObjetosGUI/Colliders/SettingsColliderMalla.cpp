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
		if (newRadio > 0 && newRadio != myCollider->getRadio()) {
			myCollider->setRadio(newRadio);
			// La shape de Bullet se cachea con el radio viejo: sin
			// reconstruirla, la fisica sigue chocando con la malla/esfera del
			// radio INICIAL y el cuerpo en el mundo seria recreado sin la
			// shape nueva. refreshRigidBody invalida la shape, saca el cuerpo
			// viejo del mundo, lo recrea y re-registra.
			if (editor && myCollider->getOwner())
				editor->refreshRigidBody(myCollider->getOwner());
			else
				myCollider->invalidateCollisionShape();
		}
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