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
#ifndef COMPORTAMIENTOCARGADO_H
#define COMPORTAMIENTOCARGADO_H

#include <string>
#include <vector>

#include "Reflection/BehaviourReflection.h"
#include "ScriptGameObject.h"

// Un comportamiento ya compilado/cargado y (posiblemente) instanciado por un
// backend. `manejador`/`instancia` son opacos: cada backend les da su
// significado propio (handle de dlopen / puntero IScriptBehaviour para C++;
// referencia global JNI / jobject para Java).
struct ComportamientoCargado {
    std::string fuente;    // ruta del fuente (.cpp / .java)
    std::string lenguaje;  // "cpp" | "java"
    std::string artefacto; // .so/.dll/.dylib o carpeta de clases compiladas
    void* manejador = nullptr;  // handle de biblioteca (C++) / JVM ref (Java)
    void* instancia = nullptr;  // IScriptBehaviour* (C++) / jobject (Java)
    void* datos = nullptr;     // estado privado del backend (p. ej. jfieldIDs)
    std::vector<ReflejoScripts::DefCampo> campos; // reflexion del comportamiento
    std::string mtimeFuente;                  // para hot reload (idempotente)
    bool cargado = false;

    bool valido() const { return cargado && instancia != nullptr; }
};

// Inyecta/extrae el arbol de valores de SerializeField sobre la instancia.
// Empareja por nombre para resistir reordenamientos en el fuente.
inline void inyectarCampos(const ComportamientoCargado& c,
                           const std::vector<ReflejoScripts::ValorCampo>& valores) {
    if (!c.valido()) return;
    for (const ReflejoScripts::DefCampo& def : c.campos) {
        const ReflejoScripts::ValorCampo* valor = nullptr;
        for (const auto& v : valores) {
            if (v.nombre == def.nombre) {
                valor = &v;
                break;
            }
        }
        if (valor) ReflejoScripts::escribirCampo(def, c.instancia, *valor);
        else ReflejoScripts::escribirCampo(def, c.instancia,
                                           ReflejoScripts::valorPorDefecto(def));
    }
}

inline std::vector<ReflejoScripts::ValorCampo> extraerCampos(
    const ComportamientoCargado& c) {
    std::vector<ReflejoScripts::ValorCampo> valores;
    if (!c.valido()) return valores;
    valores.reserve(c.campos.size());
    for (const ReflejoScripts::DefCampo& def : c.campos)
        valores.push_back(ReflejoScripts::leerCampo(def, c.instancia));
    return valores;
}

#endif // COMPORTAMIENTOCARGADO_H