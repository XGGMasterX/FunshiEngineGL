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
#ifndef ASSETEXCEPTION_H
#define ASSETEXCEPTION_H

#include "../ExcepcionesCPP/RuntimeException.h"

// Jerarquia de errores de assets. Sustituye al patron actual de
// "std::cerr + return silencioso" en la carga de recursos: quien pide un
// asset ausente o corrupto recibe una excepcion explícita (con stack trace,
// gracias a RuntimeException) en lugar de un objeto a medio cargar.
class AssetNotFoundException : public RuntimeException {
public:
    explicit AssetNotFoundException(const std::string& assetPath)
        : RuntimeException("Asset no encontrado: " + assetPath) {}
};

// El recurso pudo localizarse pero fallo su carga (archivo corrupto, formato
// no soportado por el loader, etc.).
class AssetLoadException : public RuntimeException {
public:
    explicit AssetLoadException(const std::string& assetPath,
                                const std::string& detail = "")
        : RuntimeException("No se pudo cargar el asset '" + assetPath + "'" +
                           (detail.empty() ? std::string() : ": " + detail)) {}
};

#endif