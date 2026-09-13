#include "MenuInterface.h"

MenuInterface::MenuInterface(GLFWwindow* window, bool state)
    : GeneralUserInterface("Menu", state, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|
                           ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoDocking),
      m_window(window), opcionesGUI(new MenuOpcionesInterface(window,!state)),
      configProyectGUI(new MenuProyectoInterface(window,!state)), menuState(state) {}
bool MenuInterface::getMenusState() {
    return stateGUI || opcionesGUI->getStateGui() || configProyectGUI->getStateGui();
}
void MenuInterface::initGUI() {
    ImGui::SetNextWindowPos(ImVec2(0,0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,0);
    ImGui::PushStyleColor(ImGuiCol_WindowBg,ImVec4(0.1f,0.1f,0.12f,1));
    ImGui::Begin(getNameGui().c_str(),&stateGUI,getFlagGui());
}
void MenuInterface::contentGUI() {
    ImVec2 center=ImGui::GetWindowSize(); center.x*=.5f; center.y*=.5f;
    ImGui::SetCursorPos(ImVec2(center.x-100,center.y-170));
    if(ImGui::Button("Iniciar Estudio",ImVec2(200,50))) stateGUI=false;
    ImGui::SetCursorPos(ImVec2(center.x-100,center.y-110));
    ImVec2 buttonPos=ImGui::GetCursorScreenPos(); const float w=200,h=50;
    if(ImGui::Button("Config Proyect",ImVec2(w,h))){stateGUI=false;configProyectGUI->setStateGui(true);}
    ImGui::SameLine(); ImGui::SetCursorScreenPos(ImVec2(buttonPos.x+w+10,buttonPos.y-2+(h-ImGui::GetTextLineHeight())*.5f)); ImGui::Text("Nombre Proyecto");
    ImGui::SetCursorPos(ImVec2(center.x-100,center.y-50)); buttonPos=ImGui::GetCursorScreenPos();
    if(ImGui::Button("Opciones",ImVec2(w,h))){stateGUI=false;opcionesGUI->setStateGui(true);}
    ImGui::SameLine(); ImGui::SetCursorScreenPos(ImVec2(buttonPos.x+w+10,buttonPos.y+(h-ImGui::GetTextLineHeight())*.5f)); ImGui::Text("Idioma");
    ImGui::SetCursorPos(ImVec2(center.x-100,center.y+10));
    if(ImGui::Button("Exit",ImVec2(200,50))) glfwSetWindowShouldClose(m_window,GLFW_TRUE);
}
void MenuInterface::endGUI(){ImGui::End();ImGui::PopStyleVar();ImGui::PopStyleColor();}
MenuOpcionesInterface* MenuInterface::getOpcionesGUI(){return opcionesGUI;}
MenuProyectoInterface* MenuInterface::getConfigProyectGUI(){return configProyectGUI;}
void MenuInterface::printGUI(){if(stateGUI){initGUI();contentGUI();endGUI();}}
