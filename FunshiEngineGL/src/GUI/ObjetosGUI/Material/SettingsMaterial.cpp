/*
    FunshiEngineGL - Motor de juegos 3D con OpenGL e ImGui
    Copyright 2026 Gianfranco Ivan Enrique

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    SPDX-License-Identifier: Apache-2.0
*/
#include "SettingsMaterial.h"

#include "../../../Objetos/GameObject.h"
#include "../../../Objetos/Componentes/Material.h"
#include <imgui.h>

SettingsMaterial::SettingsMaterial(GameObject* gameObject) {
	this->gameObject = gameObject;
}

void SettingsMaterial::showDataComponent() {
	Material* material = gameObject->getComponent<Material>();
	if (!material) return;

	ImGui::Text("===Material===");

	float amb[3] = {material->getAmbient()[0], material->getAmbient()[1],
	                material->getAmbient()[2]};
	if (ImGui::ColorEdit3("Ambient", amb))
		material->setAmbient(amb[0], amb[1], amb[2]);

	float diff[3] = {material->getDiffuse()[0], material->getDiffuse()[1],
	                 material->getDiffuse()[2]};
	if (ImGui::ColorEdit3("Diffuse", diff))
		material->setDiffuse(diff[0], diff[1], diff[2]);

	float spec[3] = {material->getSpecular()[0], material->getSpecular()[1],
	                 material->getSpecular()[2]};
	if (ImGui::ColorEdit3("Specular", spec))
		material->setSpecular(spec[0], spec[1], spec[2]);

	float emiss[3] = {material->getEmission()[0], material->getEmission()[1],
	                  material->getEmission()[2]};
	if (ImGui::ColorEdit3("Emission", emiss))
		material->setEmission(emiss[0], emiss[1], emiss[2]);

	float shin = material->getShininess();
	if (ImGui::SliderFloat("Shininess", &shin, 0.f, 128.f))
		material->setShininess(shin);
}

Component* SettingsMaterial::getComponent() {
	return gameObject->getComponent<Material>();
}