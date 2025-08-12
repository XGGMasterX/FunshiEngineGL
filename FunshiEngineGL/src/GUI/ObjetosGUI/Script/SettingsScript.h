#ifndef SETTINGSSCRIPT_H
#define SETTINGSSCRIPT_H
#include "../../../Objetos/GameObject.h"
#include "../SettingsComponent.h"

class SettingsScript : public SettingsComponent {
protected:
	Script* myScript;
public:
	SettingsScript(GameObject* objeto) {
		myScript = objeto->getComponent<Script>();
	}

	//CONFIGURAR EL SISTEMA PARA QUE AL ACTIVAR SEA UN GHOST BODY , FALSO UN COMUN BODY
    virtual void showDataComponent() override {
        // 1. Obtener texto a mostrar
        const std::string& className = myScript->getNameClass();
        const char* displayText = className.empty() ? "[Arrastra script]" : className.c_str();

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

	virtual Component* getComponent() override {
		return myScript;
	}
};
#endif