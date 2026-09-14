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
