#include "SettingsModel.h"

#include "../../../Objetos/GameObject.h"
#include "../../../Objetos/Componentes/Model.h"
#include <imgui.h>
#include <string>

SettingsModel::SettingsModel(GameObject* objeto) {
	myModel = objeto->getComponent<Model>();
}

void SettingsModel::showDataComponent() {
	// 1. Obtener texto a mostrar
	const std::string& path = myModel->getPath();
	const char* displayText = path.empty() ? "[Arrastra modelo]" : path.c_str();

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
			myModel->setPath(path);
		}
		ImGui::EndDragDropTarget();
	}
}

Component* SettingsModel::getComponent() { return myModel; }