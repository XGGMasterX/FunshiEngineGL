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
#include "StatusBarInterface.h"

#include "../WindowNames.h"
#include "../../Objetos/GameObject.h"
#include "../../Objetos/Componentes/Script.h"
#include "../../Scenes/SceneRegistry.h"
#include "../../Estructuras/ListasEnlazadas/ListasDoblementeEnlazada/ListaDE.h"

#include <cstdio>
#include <imgui.h>

StatusBarInterface::StatusBarInterface(bool stateGUI)
    : GeneralUserInterface(WindowNames::Status, stateGUI,
                           ImGuiWindowFlags_MenuBar) {}

void StatusBarInterface::bindScene(SceneRegistry* scene) { scene_ = scene; }

void StatusBarInterface::setEstadoCompilacion(
    bool enCurso, const std::string& actual, std::size_t hecha,
    std::size_t total,
    const std::vector<ScriptRuntime::ResultadoCarga>& resultados) {
    compilando_ = enCurso;
    actual_ = actual;
    hecha_ = hecha;
    total_ = total;
    resultados_ = resultados;
}

void StatusBarInterface::dibujarToolchain() {
    if (!toolchainListo_) {
        toolchain_ = ScriptRuntime::estadoHerramientas();
        toolchainListo_ = true;
    }

    ImGui::SeparatorText("Herramientas externas");
    ImGui::BulletText("Compilador C++: %s",
                      toolchain_.compiladorCpp.empty()
                          ? "no detectado (defini FUNSHI_CXX)"
                          : toolchain_.compiladorCpp.c_str());
    ImGui::BulletText("Cache de artefactos: %s",
                      toolchain_.cache.empty() ? "-" : toolchain_.cache.c_str());
    if (toolchain_.soporteJava) {
        ImGui::BulletText("javac: %s",
                          toolchain_.javac.empty()
                              ? "no encontrado (defini JAVAC)"
                              : toolchain_.javac.c_str());
        ImGui::BulletText("libjvm: %s",
                          toolchain_.libjvm.empty()
                              ? "no encontrada (JAVA_HOME / FUNSHI_LIBJVM)"
                              : toolchain_.libjvm.c_str());
        ImGui::BulletText("JVM: %s",
                          toolchain_.jvmArrancada
                              ? "arrancada"
                              : "pendiente (se inicia con el primer script Java)");
    } else {
        ImGui::BulletText("Scripts Java: backend no compilado (FUNSHI_JAVA=OFF)");
    }
}

void StatusBarInterface::dibujarScripts() {
    ImGui::SeparatorText("Scripts de la escena");

    // Progreso de la compilacion en curso (la cola de GameScene muestra un
    // frame "Mostrar" antes de bloquear con g++/javac, por eso se ve).
    if (compilando_ && total_ > 0) {
        char buf[128];
        std::snprintf(buf, sizeof(buf), "Compilando... (%zu de %zu)",
                      hecha_ + 1, total_);
        ImGui::ProgressBar(total_ > 0 ? static_cast<float>(hecha_) /
                                            static_cast<float>(total_)
                                      : 0.0f,
                           ImVec2(-1.0f, 0.0f), buf);
        ImGui::TextWrapped("%s", actual_.c_str());
    }

    if (!scene_) {
        ImGui::TextDisabled("Sin escena");
        return;
    }
    ListaDE<GameObject*>* objs = scene_->getGameObjects();
    if (!objs || objs->isEmpty()) {
        ImGui::TextDisabled("No hay objetos");
        return;
    }

    bool alguno = false;
    Position<GameObject*>* pos = objs->first();
    while (pos && pos->getElement()) {
        GameObject* objeto = pos->getElement();
        if (Script* s = objeto->getComponent<Script>()) {
            alguno = true;
            std::string fuente = s->rutaFuente();
            std::string estado = "En espera";
            if (compilando_ && fuente == actual_)
                estado = "Compilando...";
            else if (s->estaCargado())
                estado = "Cargado";
            else if (!s->ultimoError().empty())
                estado = "Error";
            ImGui::BulletText("%s  [%s]",
                              fuente.empty() ? "(sin fuente)" : fuente.c_str(),
                              estado.c_str());
            if (!s->ultimoError().empty())
                ImGui::TextWrapped("-> %s", s->ultimoError().c_str());
        }
        pos = (pos != objs->last()) ? objs->next(pos) : nullptr;
    }
    if (!alguno) ImGui::TextDisabled("No hay scripts en la escena");
}

void StatusBarInterface::dibujarResultados() {
    if (resultados_.empty()) return;
    ImGui::SeparatorText("Ultima compilacion");
    const std::size_t maxMostrar = 8;
    const std::size_t inicio =
        resultados_.size() > maxMostrar ? resultados_.size() - maxMostrar : 0;
    for (std::size_t i = inicio; i < resultados_.size(); ++i) {
        const ScriptRuntime::ResultadoCarga& r = resultados_[i];
        ImGui::BulletText("%s %s", r.ok ? "[OK]" : "[FALLO]",
                          r.nombre.empty() ? "(sin nombre)" : r.nombre.c_str());
        if (!r.mensaje.empty()) {
            std::string corto = r.mensaje;
            if (corto.size() > 300) corto.resize(300);
            ImGui::TextWrapped("   %s", corto.c_str());
        }
    }
}

void StatusBarInterface::contentGUI() {
    dibujarToolchain();
    dibujarScripts();
    dibujarResultados();
}

void StatusBarInterface::printGUI() {
    if (stateGUI) {
        initGUI();
        contentGUI();
        endGUI();
    }
}