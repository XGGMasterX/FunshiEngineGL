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

#include <GL/gl.h>
#include <GL/glu.h>
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
    std::cout << "GPU: " << glGetString(GL_RENDERER) << std::endl;
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
