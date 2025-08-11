#ifndef SETTINGSCOLLIDERESFERA_H
#define SETTINGSCOLLIDERESFERA_H
#include "../Transform/SettingsTransform.h"
#include "../../../Objetos/GameObject.h"

class SettingsColliderEsfera : public SettingsComponent {
protected:
	SettingsTransform* settingsTransform;
	EsfereCollider* myCollider;
public:
	SettingsColliderEsfera(GameObject* objeto) {
		myCollider = objeto->getComponent<EsfereCollider>();
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