#ifndef SCENEMENUBARINTERFACE_H
#define SCENEMENUBARINTERFACE_H
#include <iostream>
#include <string>
#include "../ObjetosGUI/SettingsObjectInterface.h"
#include <imgui.h>

using namespace std;

class SceneMenuBarInterface : public GeneralUserInterface {
protected:
    bool* toggleBool;  // Puntero al booleano externo que queremos controlar

public:
    SceneMenuBarInterface(bool stateGUI) :
        GeneralUserInterface("MenuBar", stateGUI, ImGuiWindowFlags_MenuBar){
    }

    void setActivador(bool* targetBool) {
        toggleBool = targetBool;
    }

    virtual void initGUI() override {
        ImGui::Begin(getNameGui().c_str(), &stateGUI, getFlagGui());
        ImGui::PushID(this);
    }

    virtual void contentGUI() override {
        // Centramos el botón en la ventana
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 100) * 0.5f);  // 100 = ancho del botón

        // Botón verde (color cuando está activo)
        ImVec4 buttonColor = *toggleBool ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f)  // Verde brillante
            : ImVec4(0.1f, 0.5f, 0.1f, 1.0f); // Verde oscuro

        ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
        if (ImGui::Button("Activar", ImVec2(100, 30))) {
            *toggleBool = !(*toggleBool);  // Alternar el valor del booleano
        }
        ImGui::PopStyleColor();

        // Opcional: Mostrar estado actual
        ImGui::SameLine();
        ImGui::Text(*toggleBool ? "Estado: ACTIVO" : "Estado: INACTIVO");
    }

    virtual void endGUI() override {
        ImGui::PopID();
        ImGui::End();
    }

    virtual void printGUI() override {
        if (stateGUI) {
            initGUI();
            contentGUI();
            endGUI();
        }
    }
};
#endif