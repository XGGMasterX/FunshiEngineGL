#ifndef MENUPROYECTOINTERFACE_H
#define MENUPROYECTOINTERFACE_H
#include "../GeneralUserInterface.h"
using namespace std;

class MenuProyectoInterface : public GeneralUserInterface {
private: 
    GLFWwindow* m_window;
public:
    // Constructor que recibe la ventana GLFW
    MenuProyectoInterface(GLFWwindow* window, bool stateGUI) :
        GeneralUserInterface("Config Proyect", stateGUI,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse) {
        m_window = window;
    }

    virtual void initGUI() override {

        // Configuración de ventana
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

        // Estilo
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.12f, 1.0f));
        ImGui::Begin(getNameGui().c_str(), &stateGUI, getFlagGui());
    }

    virtual void contentGUI() override {
        ImVec2 center = ImGui::GetWindowSize();
        center.x *= 0.5f;
        center.y *= 0.5f;


        ImGui::SetCursorPos(ImVec2(center.x - 100, center.y - 170));
        if (ImGui::Button("Volver", ImVec2(200, 50))) {
            stateGUI = false;
        }
    }

    virtual void endGUI() override {
        ImGui::End();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
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