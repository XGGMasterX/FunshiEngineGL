#include "SettingsCamera.h"

#include "../../../Objetos/GameObject.h"
#include "../../../Objetos/Componentes/CameraComponent.h"
#include <imgui.h>

SettingsCamera::SettingsCamera(GameObject* gameObject) {
	this->gameObject = gameObject;
}

void SettingsCamera::showDataComponent() {
	CameraComponent* camera = gameObject->getComponent<CameraComponent>();
	if (!camera) return;

	ImGui::Text("===Camara===");

	float fov = camera->getFov();
	if (ImGui::SliderFloat("FOV", &fov, 1.f, 120.f))
		camera->setFov(fov);

	float nearPlane = camera->getNearPlane();
	if (ImGui::SliderFloat("Near", &nearPlane, 0.01f, 10.f))
		camera->setNearPlane(nearPlane);

	float farPlane = camera->getFarPlane();
	if (ImGui::SliderFloat("Far", &farPlane, 10.f, 5000.f))
		camera->setFarPlane(farPlane);

	float speed = camera->getSpeed();
	if (ImGui::SliderFloat("Velocidad", &speed, 0.5f, 30.f))
		camera->setSpeed(speed);

	bool pintar = camera->getPintar();
	if (ImGui::Checkbox("Vista previa", &pintar))
		camera->setPintar(pintar);
}

Component* SettingsCamera::getComponent() {
	return gameObject->getComponent<CameraComponent>();
}