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
#include "Ventana.h"

#include <iostream>

Ventana* Ventana::instance = nullptr;

Ventana::Ventana() {
    instance = this;
}

Ventana& Ventana::getInstance() {
    return *instance;
}

GLFWwindow* Ventana::getWindow() {
    return window;
}

int Ventana::getWidth() const {
    return width;
}

int Ventana::getHeight() const {
    return height;
}

int Ventana::initVentana() {
    if (!glfwInit()) return -1;
    window = glfwCreateWindow(width, height, "Fanshi", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    const auto imprimir = [](const char* etiqueta, GLenum glenum) {
        const GLubyte* s = glGetString(glenum);
        std::cout << etiqueta << (s ? reinterpret_cast<const char*>(s)
                                    : "(no disponible)")
                  << std::endl;
    };
    imprimir("GPU: ", GL_RENDERER);
    imprimir("GL_VERSION: ", GL_VERSION);
#ifndef GL_SHADING_LANGUAGE_VERSION
#define GL_SHADING_LANGUAGE_VERSION 0x8B30
#endif
    imprimir("GLSL: ", GL_SHADING_LANGUAGE_VERSION);
#ifndef GL_CONTEXT_PROFILE_MASK
#define GL_CONTEXT_PROFILE_MASK 0x9126
#define GL_CONTEXT_CORE_PROFILE_BIT 0x00000001
#define GL_CONTEXT_COMPATIBILITY_PROFILE_BIT 0x00000002
#endif
    GLint perfilGL = 0;
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &perfilGL);
    std::cout << "Perfil GL: " << perfilGL
              << " (1=core, 2=compatibilidad, 0=desconocido/legacy)"
              << std::endl;
    return 0;
}

void Ventana::redimension(int ventanaWidthEjeX, int ventanaHeightEjeY) {
    width = ventanaWidthEjeX;
    height = ventanaHeightEjeY;
    glViewport(0, 0, width, height);
    const float aspect = static_cast<float>(width) / static_cast<float>(height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspect, 0.1, 500);
    glMatrixMode(GL_MODELVIEW);
}
