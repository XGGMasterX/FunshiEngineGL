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
#include "ExportDialog.h"

#include <imgui.h>
#include <chrono>
#include <memory>
#include <cmath>

ExportDialog::ExportDialog(Callback onCerrar) : onCerrar_(std::move(onCerrar)) {}

ExportDialog::~ExportDialog() = default;

void ExportDialog::render() {
    if (!abierto_) return;

    ImGui::OpenPopup("Exportar Juego");
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Exportar Juego", &abierto_, ImGuiWindowFlags_AlwaysAutoResize)) {
        if (!exportando_) {
            ImGui::Text("Configuración de exportación");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::InputText("Nombre del ejecutable", nombreEjecutable_, IM_ARRAYSIZE(nombreEjecutable_));
            ImGui::InputText("Nombre del proyecto exportado", nombreProyectoExportado_, IM_ARRAYSIZE(nombreProyectoExportado_));

            const char* plataformas[] = { "Linux (nativo)", "Windows (cross-compile MinGW)" };
            ImGui::Combo("Plataforma objetivo", &plataformaIdx_, plataformas, IM_ARRAYSIZE(plataformas));

            ImGui::Spacing();
            ImGui::Separator();

            if (ImGui::Button("Exportar", ImVec2(120, 0))) {
                iniciarExportacion();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancelar", ImVec2(120, 0))) {
                abierto_ = false;
            }
        } else {
            // UI de progreso con spinner indeterminado
            ImGui::Text("Exportando: %s", nombreProyectoExportado_);
            ImGui::Separator();
            ImGui::Spacing();

            // Spinner animado
            renderSpinner();
            ImGui::SameLine();
            ImGui::Text("  %s", etapaActual_.c_str());

            ImGui::ProgressBar(progreso_, ImVec2(300, 0), "");

            if (!ultimoLog_.empty()) {
                ImGui::Spacing();
                ImGui::BeginChild("Log", ImVec2(400, 150), true);
                ImGui::TextWrapped("%s", ultimoLog_.c_str());
                ImGui::EndChild();
            }

            ImGui::Spacing();
            ImGui::Separator();
            if (ImGui::Button("Cerrar", ImVec2(120, 0))) {
                if (exporter_ && !exporter_->haTerminado()) {
                    // Esperar o cancelar - por simplicidad esperamos
                }
                abierto_ = false;
            }
        }
        ImGui::EndPopup();
    }

    if (!abierto_ && onCerrar_) {
        Resultado r;
        r.exportar = false;
        onCerrar_(r);
    }
}

void ExportDialog::iniciarExportacion() {
    exportando_ = true;
    progreso_ = 0.0f;
    etapaActual_ = "Iniciando...";
    ultimoLog_.clear();

    // Obtener directorio del proyecto actual desde EditorConfig
    // Por ahora usar ruta relativa
    GameExporter::Config cfg;
    cfg.proyectoOrigen = "MotorGrafico/Proyects/ProyectoActual"; // TODO: desde EditorConfig
    cfg.nombreEjecutable = nombreEjecutable_;
    cfg.nombreProyectoExportado = nombreProyectoExportado_;
    cfg.plataforma = (plataformaIdx_ == 0) ? GameExporter::Plataforma::Linux : GameExporter::Plataforma::Windows;
    cfg.directorioSalida = "MotorGrafico/Exportaciones/" + std::string(nombreProyectoExportado_);

    cfg.onLog = [this](const std::string& msg) { agregarLog(msg); };
    cfg.onProgreso = [this](float p, const std::string& etapa) { actualizarProgreso(p, etapa); };
    cfg.onFinalizado = [this](bool exito, const std::string& msg) { finalizarExportacion(exito, msg); };

    exporter_ = std::make_unique<GameExporter>(cfg);
    exporter_->iniciar();
}

void ExportDialog::actualizarProgreso(float p, const std::string& etapa) {
    progreso_ = p;
    etapaActual_ = etapa;
}

void ExportDialog::finalizarExportacion(bool exito, const std::string& msg) {
    exportando_ = false;
    progreso_ = exito ? 1.0f : 0.0f;
    etapaActual_ = exito ? "Completado" : "Error";
    agregarLog(msg);

    if (onCerrar_) {
        Resultado r;
        r.exportar = exito;
        r.config = exporter_ ? GameExporter::Config() : GameExporter::Config();
        onCerrar_(r);
    }
}

void ExportDialog::agregarLog(const std::string& msg) {
    ultimoLog_ = msg;
}

void ExportDialog::renderSpinner(float radius, float thickness) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 center = ImGui::GetCursorScreenPos();
    center.x += radius;
    center.y += radius;

    float t = ImGui::GetTime() * 8.0f;
    float a_min = 3.14159f * 2.0f * fmodf(t, 1.0f);
    float a_max = a_min + 3.14159f * 2.0f * 0.75f;

    draw_list->PathArcTo(center, radius - thickness, a_min, a_max);
    draw_list->PathStroke(ImGui::GetColorU32(ImGuiCol_ButtonHovered), 0, thickness);
    ImGui::Dummy(ImVec2(radius * 2, radius * 2));
}