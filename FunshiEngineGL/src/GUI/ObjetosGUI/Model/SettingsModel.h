#ifndef SETTINGSMODEL_H
#define SETTINGSMODEL_H
#include "../../../Objetos/GameObject.h"
#include "../SettingsComponent.h"

class SettingsModel : public SettingsComponent {
protected:
	Model* myModel;
public:
	SettingsModel(GameObject* objeto) {
		myModel = objeto->getComponent<Model>();
	}

	//CONFIGURAR EL SISTEMA PARA QUE AL ACTIVAR SEA UN GHOST BODY , FALSO UN COMUN BODY
    virtual void showDataComponent() override {
        // 1. Obtener texto a mostrar
        const std::string& path = myModel->getPath();
        const char* displayText = path.empty() ? "[Arrastra script]" : path.c_str();

        // 2. Calcular ancho exacto del texto + padding
        float textWidth = ImGui::CalcTextSize(displayText).x;
        float buttonWidth = textWidth + ImGui::GetStyle().FramePadding.x * 2; // Padding horizontal

        // 4. Botón ajustado al texto (pegado a izquierda)
        ImGui::SetNextItemWidth(buttonWidth); // Fijar ancho exacto
        if (ImGui::Button(displayText)) {    // Sin tamaño fijo (usa el SetNextItemWidth)
            // Acción opcional al click
        }

        // 5. Drag & Drop
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ARCHIVO_PATH")) {
                const char* path = (const char*)payload->Data;
                myModel->setPath(path);
            }
            ImGui::EndDragDropTarget();
        }
 }

	virtual Component* getComponent() override {
		return myModel;
	}
};
#endif
