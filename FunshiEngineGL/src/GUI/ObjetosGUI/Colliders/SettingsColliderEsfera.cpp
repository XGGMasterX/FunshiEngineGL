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

	myCollider->dibujarCollider();
}

Component* SettingsColliderEsfera::getComponent() { return myCollider; }