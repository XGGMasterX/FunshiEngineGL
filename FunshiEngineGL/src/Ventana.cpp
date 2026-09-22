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

#include "Rendering/Backend/IRenderBackend.h"

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

    // Antialiasing por multisampling (4x): el contexto por defecto de GLFW se
    // crea sin samples y las lineas (grilla, ejes, wireframes) se ven
    // escalonadas ("a dientes"). Con MSAA el framebuffer se suaviza entero.
    glfwWindowHint(GLFW_SAMPLES, 4);
    window = glfwCreateWindow(width, height, "FunshiEngineGL", nullptr, nullptr);
    if (!window) {
        // Driver sin framebuffer multisampleado: se reintenta sin MSAA antes
        // de abortar (la escena se dibuja igual, solo sin suavizado de lineas).
        glfwWindowHint(GLFW_SAMPLES, 0);
        window = glfwCreateWindow(width, height, "FunshiEngineGL", nullptr, nullptr);
        if (!window) {
            glfwTerminate();
            return -1;
        }
    }
    glfwMakeContextCurrent(window);
    // Info del contexto (GPU, versiones, perfil): lo reporta el backend; aca
    // solo se imprime para los logs de arranque.
    std::cout << Rendering::Backend::activeBackend().diagnosticoGPU()
              << std::endl;
    return 0;
}

void Ventana::redimension(int ventanaWidthEjeX, int ventanaHeightEjeY) {
    width = ventanaWidthEjeX;
    height = ventanaHeightEjeY;
    // El viewport del framebuffer lo fija el backend; las matrices de proyec-
    // cion se establecen cada frame en la pasada de la escena (no viven aca).
    Rendering::Backend::activeBackend().setViewport(0, 0, width, height);
}
