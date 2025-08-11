#ifndef SETTINGSCOLOR_H
#define SETTINGSCOLOR_H
#include "../SettingsComponent.h"
#include "../../../Objetos/GameObject.h"

class SettingsColor : public SettingsComponent {
	GameObject* gameObject;
public:
	SettingsColor(GameObject* gameObject) {
		this->gameObject = gameObject;
	}

	virtual void showDataComponent() override {
		ImGui::Text("===ObjectColor===");
		ImGui::ColorEdit3("Color", gameObject->auxColor);
		gameObject->setColor(gameObject->auxColor);
	}

	virtual Component* getComponent() {
		return gameObject->getComponent<Color>();
	}
};
#endif
