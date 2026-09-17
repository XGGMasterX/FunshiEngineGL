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
#include "ScriptGameObject.h"

#include <iostream>

#include "../Objetos/GameObject.h"
#include "../Objetos/Componentes/Transform.h"

namespace MotorScript {

namespace {
const char* nombreDelObjeto(const void* objeto) {
    const GameObject* o = static_cast<const GameObject*>(objeto);
    return o ? o->inputName : "";
}

float posicionEje(const void* objeto, int eje) {
    const GameObject* o = static_cast<const GameObject*>(objeto);
    if (!o) return 0.0f;
    GameObject* obj = const_cast<GameObject*>(o);
    Transform* transform = obj->getComponent<Transform>();
    return transform ? transform->getTranslatef()[eje] : 0.0f;
}

void fijarPosicion(void* objeto, float x, float y, float z) {
    GameObject* o = static_cast<GameObject*>(objeto);
    if (!o) return;
    Transform* transform = o->getComponent<Transform>();
    if (transform) transform->setTranslatef(x, y, z);
}

void fijarEscala(void* objeto, float x, float y, float z) {
    GameObject* o = static_cast<GameObject*>(objeto);
    if (!o) return;
    Transform* transform = o->getComponent<Transform>();
    if (transform) transform->setScalef(x, y, z);
}

void fijarRotacionEjes(void* objeto, float angulo, float x, float y, float z) {
    GameObject* o = static_cast<GameObject*>(objeto);
    if (!o) return;
    Transform* transform = o->getComponent<Transform>();
    if (transform) transform->setRotatef(angulo, x, y, z);
}

void imprimirConsola(const char* texto) {
    if (texto) std::cout << "[script] " << texto << std::endl;
}
} // namespace

const ApiScriptGameObject* tablaApi() {
    static const ApiScriptGameObject tabla = {
        /* .nombre        = */ nombreDelObjeto,
        /* .posicionX     = */ [](const void* o) { return posicionEje(o, 0); },
        /* .posicionY     = */ [](const void* o) { return posicionEje(o, 1); },
        /* .posicionZ     = */ [](const void* o) { return posicionEje(o, 2); },
        /* .fijarPosicion = */ fijarPosicion,
        /* .fijarEscala   = */ fijarEscala,
        /* .fijarRotacionEjes = */ fijarRotacionEjes,
        /* .imprimirConsola   = */ imprimirConsola,
    };
    return &tabla;
}
} // namespace MotorScript