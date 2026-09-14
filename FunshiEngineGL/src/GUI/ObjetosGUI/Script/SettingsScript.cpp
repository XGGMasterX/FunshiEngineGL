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
#include "SettingsScript.h"

#include "../../../Objetos/GameObject.h"
#include "../../../Objetos/Componentes/Script.h"
#include <imgui.h>
#include <string>

SettingsScript::SettingsScript(GameObject* objeto) {
	myScript = objeto->getComponent<Script>();
}

void SettingsScript::showDataComponent() {
	// 1. Obtener texto a mostrar
	const std::string& className = myScript->getNameClass();
	const char* displayText = className.empty() ? "[Arrastra script]" : className.c_str();

	// 2. Calcular ancho exacto del texto + padding
	float textWidth = ImGui::CalcTextSize(displayText).x;
	float buttonWidth = textWidth + ImGui::GetStyle().FramePadding.x * 2;

	// 4. Boton ajustado al texto (pegado a izquierda)
	ImGui::SetNextItemWidth(buttonWidth);
	if (ImGui::Button(displayText)) {
		// Accion opcional al click
	}

	// 5. Drag & Drop
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload =
		        ImGui::AcceptDragDropPayload("ARCHIVO_PATH")) {
			const char* path = (const char*)payload->Data;
			myScript->setDllPath(path);
		}
		ImGui::EndDragDropTarget();
	}

	// 7. Mostrar path debajo (opcional)
	if (!myScript->getPath().empty()) {
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(150, 150, 150, 255));
		ImGui::TextUnformatted(myScript->getPath().c_str());
		ImGui::PopStyleColor();
	}
}

Component* SettingsScript::getComponent() { return myScript; }