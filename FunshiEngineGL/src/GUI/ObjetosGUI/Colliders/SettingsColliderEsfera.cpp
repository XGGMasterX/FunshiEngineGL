#include "SettingsColliderEsfera.h"

#include "../Transform/SettingsTransform.h"
#include "../../../Objetos/GameObject.h"
#include "../../../Objetos/Componentes/Colliders/EsfereCollider.h"
#include "../../../Scenes/EditorController.h"
#include <imgui.h>

SettingsColliderEsfera::SettingsColliderEsfera(GameObject* objeto) {
	myCollider = objeto->getComponent<EsfereCollider>();
	this->settingsTransform =
	    new SettingsTransform(myCollider->getTransform(), objeto);
	this->newRadio = myCollider->getRadio();
}

SettingsColliderEsfera::~SettingsColliderEsfera() { delete settingsTransform; }

void SettingsColliderEsfera::setEditor(EditorController* editor) {
	this->editor = editor;
}

void SettingsColliderEsfera::showDataComponent() {
	ImGui::InputFloat("Radio", &newRadio);
	if (ImGui::Button("Confirmar")) {
		if (newRadio > 0 && newRadio != myCollider->getRadio()) {
			myCollider->setRadio(newRadio);
			// La shape de Bullet se cachea con el radio viejo: sin
			// reconstruirla, la fisica sigue chocando con la esfera del radio
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

	// Editar el offset local del collider con el gizmo de la escena 3D: la
	// unica fuente de verdad es el EditorController (unic persona que setea el
	// GizmoTarget con { local = myTransform, parent = global del objeto }).
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

Component* SettingsColliderEsfera::getComponent() { return myCollider; }