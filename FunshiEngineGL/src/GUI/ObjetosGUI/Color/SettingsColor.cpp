#include "SettingsColor.h"

#include "../../../Objetos/GameObject.h"
#include "../../../Objetos/Componentes/Color.h"
#include <imgui.h>

SettingsColor::SettingsColor(GameObject* gameObject) {
	this->gameObject = gameObject;
}

void SettingsColor::showDataComponent() {
	ImGui::Text("===ObjectColor===");
	ImGui::ColorEdit3("Color", gameObject->auxColor);
	gameObject->setColor(gameObject->auxColor);
}

Component* SettingsColor::getComponent() {
	return gameObject->getComponent<Color>();
}