#ifndef SETTINGSCOLLIDERCUBO_H
#define SETTINGSCOLLIDERCUBO_H
#include "../Transform/SettingsTransform.h"
#include "../../../Objetos/GameObject.h"

class SettingsColliderCubo : public SettingsComponent {
protected:
	SettingsTransform* settingsTransform;
	CubeCollider* myCollider;
public:
	SettingsColliderCubo(GameObject* objeto) {
		myCollider = objeto->getComponent<CubeCollider>();
		this->settingsTransform = new SettingsTransform(myCollider->getTransform());
	}

	virtual void showDataComponent() override {

		static float newRadio = myCollider->getRadio();
		ImGui::InputFloat("Radio", &newRadio);
		if (ImGui::Button("Confirmar")) {
			myCollider->setRadio(newRadio);
		}
		settingsTransform->showDataComponent();
		myCollider->dibujarCollider();
	}

	virtual Component* getComponent() override {
		return myCollider;
	}
};
#endif
