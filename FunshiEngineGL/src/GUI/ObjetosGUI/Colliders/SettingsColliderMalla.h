#ifndef SETTINGSCOLLIDERMALLA_H
#define SETTINGSCOLLIDERMALLA_H
#include "../Transform/SettingsTransform.h"
#include "../../../Objetos/GameObject.h"

class SettingsColliderMalla : public SettingsComponent {
protected:
	SettingsTransform* settingsTransform;
	MallaCollider* myCollider;
public:
	SettingsColliderMalla(GameObject* objeto) {
		myCollider = objeto->getComponent<MallaCollider>();
		this->settingsTransform = new SettingsTransform(myCollider->getTransform());
	}

	virtual void showDataComponent() override {

		static float newRadio = myCollider->getRadio();
		ImGui::InputFloat("Radio", &newRadio);
		if (ImGui::Button("Confirmar")) {
			myCollider->setRadio(newRadio);
			newRadio = 1.0f;
		}
		settingsTransform->showDataComponent();
		myCollider->dibujarCollider();
	}

	virtual Component* getComponent() override {
		return myCollider;
	}
};
#endif