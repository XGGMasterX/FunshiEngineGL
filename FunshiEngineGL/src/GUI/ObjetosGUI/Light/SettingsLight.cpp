#include "SettingsLight.h"

#include "../../../Objetos/GameObject.h"
#include "../../../Objetos/Componentes/Light.h"
#include <imgui.h>

SettingsLight::SettingsLight(GameObject* gameObject) {
	this->gameObject = gameObject;
}

void SettingsLight::showDataComponent() {
	Light* light = gameObject->getComponent<Light>();
	if (!light) return;

	ImGui::Text("===Luz===");

	const char* tipos[] = {"Direccional", "Puntual", "Spot"};
	int tipoActual = static_cast<int>(light->getType());
	if (ImGui::Combo("Tipo", &tipoActual, tipos, IM_ARRAYSIZE(tipos))) {
		light->setType(static_cast<LightType>(tipoActual));
	}

	float amb[3] = {light->getAmbient()[0], light->getAmbient()[1],
	                light->getAmbient()[2]};
	if (ImGui::ColorEdit3("Ambient", amb))
		light->setAmbient(amb[0], amb[1], amb[2]);

	float diff[3] = {light->getDiffuse()[0], light->getDiffuse()[1],
	                 light->getDiffuse()[2]};
	if (ImGui::ColorEdit3("Diffuse", diff))
		light->setDiffuse(diff[0], diff[1], diff[2]);

	float spec[3] = {light->getSpecular()[0], light->getSpecular()[1],
	                 light->getSpecular()[2]};
	if (ImGui::ColorEdit3("Specular", spec))
		light->setSpecular(spec[0], spec[1], spec[2]);

	if (light->getType() != LightType::DIRECTIONAL) {
		float constant = light->getConstant();
		float linear = light->getLinear();
		float quadratic = light->getQuadratic();
		if (ImGui::SliderFloat("At. constante", &constant, 0.f, 2.f) ||
		    ImGui::SliderFloat("At. lineal", &linear, 0.f, 1.f) ||
		    ImGui::SliderFloat("At. cuadratica", &quadratic, 0.f, 1.f)) {
			light->setAttenuation(constant, linear, quadratic);
		}
	}

	if (light->getType() == LightType::SPOT) {
		float cutoff = light->getSpotCutOff();
		if (ImGui::SliderFloat("Corte del cono", &cutoff, 1.f, 90.f))
			light->setSpotCutOff(cutoff);
	}
}

Component* SettingsLight::getComponent() {
	return gameObject->getComponent<Light>();
}