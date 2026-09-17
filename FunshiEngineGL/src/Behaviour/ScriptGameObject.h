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
#ifndef SCRIPTGAMEOBJECT_H
#define SCRIPTGAMEOBJECT_H

// Interfaz que ven los scripts sobre GameObject. Para que el .so/.dll de un
// script NO tenga que enlazar contra el motor (ni depender de las cabeceras que
// arrastran Bullet/Assimp), el motor inyecta una TABLA DE PUNTEROS A FUNCION:
// IScriptBehaviour::conectarApi(). Los scripts la usan via `this->api->...`.

class GameObject;

namespace MotorScript {

// Tabla de acceso a transform/nombre/log. Implementada por el motor
// (ScriptGameObject.cpp) y entregada al comportamiento en su creacion.
struct ApiScriptGameObject {
    const char* (*nombre)(const void* objeto);
    float (*posicionX)(const void* objeto);
    float (*posicionY)(const void* objeto);
    float (*posicionZ)(const void* objeto);
    void (*fijarPosicion)(void* objeto, float x, float y, float z);
    void (*fijarEscala)(void* objeto, float x, float y, float z);
    void (*fijarRotacionEjes)(void* objeto, float angulo, float x, float y,
                              float z);
    void (*imprimirConsola)(const char* texto);
};

// Tabla implementada por el motor (ScriptGameObject.cpp). No la usan los
// scripts directamente; el backend se la entrega a IScriptBehaviour::conectarApi.
const ApiScriptGameObject* tablaApi();

} // namespace MotorScript

#endif // SCRIPTGAMEOBJECT_H