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
#ifndef ICONOSGUI_H
#define ICONOSGUI_H

#include <string>
#include <imgui.h>

#include "../../GLCompat.h"

class IconosGUI {
public:
    IconosGUI();
    ~IconosGUI();

    void init();

    bool estaInicializado() const { return inicializado; }

    ImTextureID getIconoCarpeta() const { return iconoCarpeta; }
    ImTextureID getIconoArchivo() const { return iconoArchivo; }
    ImTextureID getIconoCpp() const { return iconoCpp; }
    ImTextureID getIconoHpp() const { return iconoHpp; }
    ImTextureID getIconoJava() const { return iconoJava; }
    ImTextureID getIconoGameObject() const { return iconoGameObject; }
    ImTextureID getIconoPorExtension(const std::string& extension) const;

private:
    ImTextureID cargarPNG(const char* nombrePNG);

    ImTextureID iconoCarpeta = ImTextureID_Invalid;
    ImTextureID iconoArchivo = ImTextureID_Invalid;
    ImTextureID iconoCpp = ImTextureID_Invalid;
    ImTextureID iconoHpp = ImTextureID_Invalid;
    ImTextureID iconoJava = ImTextureID_Invalid;
    ImTextureID iconoGameObject = ImTextureID_Invalid;
    bool inicializado = false;
};

#endif