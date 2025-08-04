#ifndef MENUINTERFACE_H
#define MENUINTERFACE_H

#include "../GeneralUserInterface.h"
#if defined(_WIN32)
#include <glfw3.h>
#elif defined(__linux__)
#include <GLFW/glfw3.h>
#endif
#include "./MenuOpcionesInterface.h"
#include "./MenuProyectoInterface.h"

class MenuInterface : public GeneralUserInterface{
private:
    GLFWwindow* m_window;
    MenuOpcionesInterface* opcionesGUI;
    MenuProyectoInterface* configProyectGUI;
    bool menuState;
public:
        // Constructor que recibe la ventana GLFW
        MenuInterface(GLFWwindow* window, bool stateGUI) :
            GeneralUserInterface("Menu", stateGUI,
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoCollapse){
            m_window = window;
            opcionesGUI = new MenuOpcionesInterface(window,!stateGUI);
            configProyectGUI = new MenuProyectoInterface(window,!stateGUI);
            menuState = stateGUI;
        }

        virtual bool getMenusState() {
            return (stateGUI || opcionesGUI->getStateGui() || configProyectGUI->getStateGui());
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
            // Contenido centrado
            ImVec2 center = ImGui::GetWindowSize();
            center.x *= 0.5f;
            center.y *= 0.5f;


            ImGui::SetCursorPos(ImVec2(center.x - 100, center.y - 170));
            if (ImGui::Button("Iniciar Estudio", ImVec2(200, 50))) {
                stateGUI = false;
            }

            //------------

            ImGui::SetCursorPos(ImVec2(center.x - 100, center.y - 110));
            ImVec2 buttonPos = ImGui::GetCursorScreenPos(); // Posición del botón antes de dibujarlo
            float buttonWidth = 200;
            float buttonHeight = 50;

            if (ImGui::Button("Config Proyect", ImVec2(buttonWidth, buttonHeight))) {
                stateGUI = false;
                configProyectGUI->setStateGui(!stateGUI);
            }

            // Mostrar texto centrado verticalmente al lado derecho
            ImGui::SameLine();
            ImGui::SetCursorScreenPos(ImVec2(
                buttonPos.x + buttonWidth + 10,
                buttonPos.y - 2 + (buttonHeight - ImGui::GetTextLineHeight()) * 0.5f
            ));
            ImGui::Text("Nombre Proyecto");

            //------------

            ImGui::SetCursorPos(ImVec2(center.x - 100, center.y - 50));
            buttonPos = ImGui::GetCursorScreenPos(); // Reutilizamos la variable

            if (ImGui::Button("Opciones", ImVec2(buttonWidth, buttonHeight))) {
                stateGUI = false;
                opcionesGUI->setStateGui(!stateGUI);
            }

            ImGui::SameLine();
            ImGui::SetCursorScreenPos(ImVec2(
                buttonPos.x + buttonWidth + 10,
                buttonPos.y + (buttonHeight - ImGui::GetTextLineHeight()) * 0.5f
            ));
            ImGui::Text("Idioma");


            //------------

            ImGui::SetCursorPos(ImVec2(center.x - 100, center.y + 10));
            if (ImGui::Button("Exit", ImVec2(200, 50))) {
                glfwSetWindowShouldClose(m_window, GLFW_TRUE);
            }
        }

        virtual void endGUI() override {
            ImGui::End();
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
        }

        virtual MenuOpcionesInterface* getOpcionesGUI() {
            return opcionesGUI;
        }

        virtual MenuProyectoInterface* getConfigProyectGUI() {
            return configProyectGUI;
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
