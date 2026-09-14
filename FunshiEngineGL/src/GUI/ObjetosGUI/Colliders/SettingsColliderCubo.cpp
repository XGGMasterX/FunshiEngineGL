#include "SettingsColliderCubo.h"

#include "../Transform/SettingsTransform.h"
#include "../../../Objetos/GameObject.h"
#include "../../../Objetos/Componentes/Colliders/CubeCollider.h"
#include "../../../Scenes/EditorController.h"
#include <imgui.h>

SettingsColliderCubo::SettingsColliderCubo(GameObject* objeto) {
	myCollider = objeto->getComponent<CubeCollider>();
	this->settingsTransform =
	    new SettingsTransform(myCollider->getTransform(), objeto);
	this->newRadio = myCollider->getRadio();
}

SettingsColliderCubo::~SettingsColliderCubo() { delete settingsTransform; }

void SettingsColliderCubo::setEditor(EditorController* editor) {
	this->editor = editor;
}

void SettingsColliderCubo::showDataComponent() {
	ImGui::InputFloat("Radio", &newRadio);
	if (ImGui::Button("Confirmar")) {
		if (newRadio > 0 && newRadio != myCollider->getRadio()) {
			myCollider->setRadio(newRadio);
			// La shape de Bullet se cachea con el radio viejo: sin
			// reconstruirla, la fisica sigue chocando con la caja del radio
			// INICIAL (p.ej. 5) y el cuerpo en el mundo seria recreado sin la
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

Component* SettingsColliderCubo::getComponent() { return myCollider; }