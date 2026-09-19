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
#ifndef TEMAEDITOR_H
#define TEMAEDITOR_H

#include <imgui.h>

#include "../../Configuracion/Apariencia.h"

// ============================================================================
// Aplicador del perfil de apariencia a ImGui.
//
// Separa la logica "como se ve la interfaz" (esta capa, depende de ImGui) del
// dato "que prefiere el usuario" (Apariencia, puro y persistible). main llama
// a aplicarEstilo() una sola vez al arrancar y cada vez que el perfil cambia
// desde la vista Opciones.
//
// El color de acento se inyecta en los roles visuales de ImGui (botones,
// headers, tabs, sliders, checks, etc.) para que la interfaz no dependa del
// azul de fabrica. Los widgets que necesitan el mismo acento (por ejemplo el
// overlay de Estado) lo derivan con ImGui::GetStyleColorVec4.
// ============================================================================
namespace TemaEditor {

// Color de acento resuelto: en modo B/N se devuelve su version en escala de
// grises (asi toda la interfaz queda monocroma).
ImVec4 acento(const Apariencia& ap);

// Aplica el tema base (claro/oscuro), el acento y un conjunto coherente de
// redondeos/espaciados. Idempotente: llamarlo con el mismo perfil es inocuo.
void aplicarEstilo(const Apariencia& ap);

} // namespace TemaEditor

#endif
