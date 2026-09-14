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
#include "Script.h"

void Script::serializeComponent(std::ofstream* fileNamePathContentObject) {
    // 1. Serializar longitud del path + contenido
    size_t pathLength = dllPath.size();
    fileNamePathContentObject->write(reinterpret_cast<const char*>(&pathLength),
                                     sizeof(size_t));
    fileNamePathContentObject->write(dllPath.c_str(), pathLength);

    // 2. Serializar longitud del nombre de clase + contenido
    size_t nameLength = nameClass.size();
    fileNamePathContentObject->write(reinterpret_cast<const char*>(&nameLength),
                                     sizeof(size_t));
    fileNamePathContentObject->write(nameClass.c_str(), nameLength);
}

void Script::deserializeComponent(std::ifstream* fileNamePathContentObject) {
    // 1. Deserializar path
    size_t pathLength = 0;
    fileNamePathContentObject->read(reinterpret_cast<char*>(&pathLength),
                                    sizeof(size_t));
    dllPath.resize(pathLength);
    fileNamePathContentObject->read(&dllPath[0], pathLength);

    // 2. Deserializar nombre de clase
    size_t nameLength = 0;
    fileNamePathContentObject->read(reinterpret_cast<char*>(&nameLength),
                                    sizeof(size_t));
    nameClass.resize(nameLength);
    fileNamePathContentObject->read(&nameClass[0], nameLength);
}

void Script::setDllPath(std::string dllPath) {
    this->dllPath = dllPath; // Guarda el path completo

    // Extraer nombre de clase (entre el ultimo '/' y '.')
    size_t lastSlash = dllPath.find_last_of("/\\");
    size_t lastDot = dllPath.find_last_of('.');

    bool isValidScript =
        !dllPath.empty() && dllPath.find(".cpp") != std::string::npos;

    if (lastSlash != std::string::npos && lastDot != std::string::npos &&
        lastDot > lastSlash && isValidScript) {
        this->nameClass =
            dllPath.substr(lastSlash + 1, lastDot - lastSlash - 1);
    } else {
        this->nameClass = "Debe ser <ClassName>.cpp"; // O valor por defecto
    }
}

void Script::saveComponent(std::ofstream* fileNamePathContentObject) {
    serializeComponent(fileNamePathContentObject);
}

void Script::loadComponent(std::ifstream* fileNamePathContentObject) {
    deserializeComponent(fileNamePathContentObject);
}