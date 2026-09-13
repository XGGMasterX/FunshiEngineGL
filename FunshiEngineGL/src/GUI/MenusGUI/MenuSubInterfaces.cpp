#include "MenuOpcionesInterface.h"
#include "MenuProyectoInterface.h"

namespace {
void beginMenu() {
    ImGui::SetNextWindowPos(ImVec2(0,0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,0);
    ImGui::PushStyleColor(ImGuiCol_WindowBg,ImVec4(0.1f,0.1f,0.12f,1));
}
}

MenuOpcionesInterface::MenuOpcionesInterface(GLFWwindow* window, bool state)
    : GeneralUserInterface("Opciones",state,ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|
                           ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoDocking),m_window(window){}
void MenuOpcionesInterface::initGUI(){beginMenu();ImGui::Begin(getNameGui().c_str(),&stateGUI,getFlagGui());}
void MenuOpcionesInterface::contentGUI(){ImVec2 c=ImGui::GetWindowSize();c.x*=.5f;c.y*=.5f;ImGui::SetCursorPos(ImVec2(c.x-100,c.y-170));if(ImGui::Button("Volver",ImVec2(200,50)))stateGUI=false;}
void MenuOpcionesInterface::endGUI(){ImGui::End();ImGui::PopStyleVar();ImGui::PopStyleColor();}
void MenuOpcionesInterface::printGUI(){if(stateGUI){initGUI();contentGUI();endGUI();}}

MenuProyectoInterface::MenuProyectoInterface(GLFWwindow* window, bool state)
    : GeneralUserInterface("Config Proyect",state,ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|
                           ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoDocking),m_window(window){}
void MenuProyectoInterface::initGUI(){beginMenu();ImGui::Begin(getNameGui().c_str(),&stateGUI,getFlagGui());}
void MenuProyectoInterface::contentGUI(){ImVec2 c=ImGui::GetWindowSize();c.x*=.5f;c.y*=.5f;ImGui::SetCursorPos(ImVec2(c.x-100,c.y-170));if(ImGui::Button("Volver",ImVec2(200,50)))stateGUI=false;}
void MenuProyectoInterface::endGUI(){ImGui::End();ImGui::PopStyleVar();ImGui::PopStyleColor();}
void MenuProyectoInterface::printGUI(){if(stateGUI){initGUI();contentGUI();endGUI();}}
