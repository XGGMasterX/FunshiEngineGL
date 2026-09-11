#include "SceneMenuBarInterface.h"

SceneMenuBarInterface::SceneMenuBarInterface(bool state)
    : GeneralUserInterface("MenuBar", state, ImGuiWindowFlags_MenuBar), toggleBool(nullptr) {}
void SceneMenuBarInterface::setActivador(bool* target) { toggleBool = target; }
bool* SceneMenuBarInterface::getActivador() { return toggleBool; }
bool SceneMenuBarInterface::getCargarScripts() { return cargarScripts; }
void SceneMenuBarInterface::setCargarScripts(bool value) { cargarScripts = value; }
void SceneMenuBarInterface::initGUI() {
    ImGui::Begin(getNameGui().c_str(), &stateGUI, getFlagGui());
    ImGui::PushID(this);
}
void SceneMenuBarInterface::contentGUI() {
    if (!toggleBool) return;
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 100) * 0.5f);
    const ImVec4 color = *toggleBool ? ImVec4(0.2f,0.8f,0.2f,1) : ImVec4(0.1f,0.5f,0.1f,1);
    ImGui::PushStyleColor(ImGuiCol_Button, color);
    if (ImGui::Button("Activar", ImVec2(100,30))) {
        *toggleBool = !*toggleBool;
        cargarScripts = true;
    }
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::Text(*toggleBool ? "Estado: ACTIVO" : "Estado: INACTIVO");
}
void SceneMenuBarInterface::endGUI() { ImGui::PopID(); ImGui::End(); }
void SceneMenuBarInterface::printGUI() {
    if (stateGUI) { initGUI(); contentGUI(); endGUI(); }
}
