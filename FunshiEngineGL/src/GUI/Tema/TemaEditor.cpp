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
#include "TemaEditor.h"

namespace {

ImVec4 conAlpha(const ImVec4& c, float a) {
    return ImVec4(c.x, c.y, c.z, a);
}

ImVec4 escalar(const ImVec4& c, float f) {
    return ImVec4(c.x * f, c.y * f, c.z * f, c.w);
}

ImVec4 aGris(const ImVec4& c) {
    const float v = AparienciaUtil::luminancia(c.x, c.y, c.z);
    return ImVec4(v, v, v, c.w);
}

} // namespace

ImVec4 TemaEditor::acento(const Apariencia& ap) {
    ImVec4 c(ap.acento[0], ap.acento[1], ap.acento[2], ap.acento[3]);
    return ap.blancoYNegro ? aGris(c) : c;
}

void TemaEditor::aplicarEstilo(const Apariencia& ap) {
    // Base clara u oscura de fabrica; luego se pisan los roles de acento.
    if (ap.temaClaro) {
        ImGui::StyleColorsLight();
    } else {
        ImGui::StyleColorsDark();
    }

    ImGuiStyle& s = ImGui::GetStyle();

    // Geometria coherente para toda la interfaz (bordes suaves y aire).
    s.WindowRounding = 6.0f;
    s.ChildRounding = 4.0f;
    s.FrameRounding = 4.0f;
    s.PopupRounding = 4.0f;
    s.ScrollbarRounding = 6.0f;
    s.GrabRounding = 4.0f;
    s.TabRounding = 4.0f;
    s.WindowBorderSize = 1.0f;
    s.ChildBorderSize = 1.0f;
    s.FrameBorderSize = 0.0f;
    s.WindowPadding = ImVec2(10.0f, 10.0f);
    s.FramePadding = ImVec2(8.0f, 5.0f);
    s.ItemSpacing = ImVec2(8.0f, 6.0f);
    s.ScrollbarSize = 12.0f;
    s.GrabMinSize = 10.0f;

    const ImVec4 ac = acento(ap);
    const ImVec4 acOscuro = escalar(ac, 0.75f);
    const ImVec4 acClaro = escalar(ac, 1.20f);

    // Roles de acento. Se usan con transparencia donde ImGui espera fondos.
    s.Colors[ImGuiCol_Button] = conAlpha(ac, 0.55f);
    s.Colors[ImGuiCol_ButtonHovered] = ac;
    s.Colors[ImGuiCol_ButtonActive] = acOscuro;
    s.Colors[ImGuiCol_Header] = conAlpha(ac, 0.45f);
    s.Colors[ImGuiCol_HeaderHovered] = conAlpha(ac, 0.80f);
    s.Colors[ImGuiCol_HeaderActive] = ac;
    s.Colors[ImGuiCol_CheckMark] = ac;
    s.Colors[ImGuiCol_SliderGrab] = acOscuro;
    s.Colors[ImGuiCol_SliderGrabActive] = acClaro;
    s.Colors[ImGuiCol_TitleBgActive] = acOscuro;
    s.Colors[ImGuiCol_TabHovered] = conAlpha(ac, 0.80f);
    s.Colors[ImGuiCol_TabSelected] = conAlpha(ac, 0.65f);
    s.Colors[ImGuiCol_TabSelectedOverline] = ac;
    s.Colors[ImGuiCol_SeparatorHovered] = conAlpha(ac, 0.78f);
    s.Colors[ImGuiCol_SeparatorActive] = ac;
    s.Colors[ImGuiCol_ResizeGrip] = conAlpha(ac, 0.25f);
    s.Colors[ImGuiCol_ResizeGripHovered] = conAlpha(ac, 0.67f);
    s.Colors[ImGuiCol_ResizeGripActive] = conAlpha(ac, 0.95f);
    s.Colors[ImGuiCol_TextSelectedBg] = conAlpha(ac, 0.35f);
    s.Colors[ImGuiCol_FrameBgHovered] = conAlpha(ac, 0.40f);
    s.Colors[ImGuiCol_FrameBgActive] = conAlpha(ac, 0.67f);
    s.Colors[ImGuiCol_DockingPreview] = conAlpha(ac, 0.70f);
    s.Colors[ImGuiCol_NavCursor] = ac;

    // Modo blanco y negro: se desatura TODA la paleta (incluido el acento ya
    // aplicado) preservando las transparencias. El resultado es una interfaz
    // monocroma coherente con el fondo y la grilla del viewport.
    if (ap.blancoYNegro) {
        for (int i = 0; i < ImGuiCol_COUNT; ++i) {
            s.Colors[i] = aGris(s.Colors[i]);
        }
    }
}
