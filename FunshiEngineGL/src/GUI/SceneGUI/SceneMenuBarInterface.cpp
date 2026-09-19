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
    // Boton play/stop: el texto y el color cambian segun el estado para que
    // quede claro que el mismo boton activa y detiene la simulacion.
    const bool activo = *toggleBool;
    const ImVec4 color =
        activo ? ImVec4(0.72f, 0.22f, 0.22f, 1.0f)   // Detener (rojo)
               : ImVec4(0.16f, 0.55f, 0.24f, 1.0f);  // Activar (verde)
    const ImVec4 hover = activo ? ImVec4(0.85f, 0.30f, 0.30f, 1.0f)
                                : ImVec4(0.24f, 0.68f, 0.32f, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hover);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
    if (ImGui::Button(activo ? "Detener" : "Activar", ImVec2(100, 30))) {
        *toggleBool = !*toggleBool;
        cargarScripts = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    ImGui::Text(activo ? "Estado: ACTIVO" : "Estado: INACTIVO");
}
void SceneMenuBarInterface::endGUI() { ImGui::PopID(); ImGui::End(); }
void SceneMenuBarInterface::printGUI() {
    if (stateGUI) { initGUI(); contentGUI(); endGUI(); }
}
