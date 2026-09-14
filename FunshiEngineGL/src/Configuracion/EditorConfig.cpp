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
#include "EditorConfig.h"

#include <nlohmann/json.hpp>

#include <cstdlib>
#include <filesystem>
#include <fstream>

// ============================================================================
// Persistencia de la configuracion del editor en JSON (nlohmann, vendoriado en
// External/nlohmann). Carga tolerante: si el archivo falta, esta corrupto o le
// faltan campos, se quedan los valores por defecto y se conserva lo que si se
// pudo leer. El binario solo persiste datos de escena, no configuracion.
// ============================================================================

std::string EditorConfig::rutaPorDefecto() {
    return directorioProyectoPorDefecto() + "/Configuracion.json";
}

std::string EditorConfig::directorioProyectoPorDefecto() {
#if defined(_WIN32)
    return "C:/MotorGraficoArchivos";
#else
    const char* home = std::getenv("HOME");
    return std::string(home ? home : ".") + "/MotorGrafico";
#endif
}

void EditorConfig::cargar(const std::string& ruta) {
    std::ifstream in(ruta);
    if (!in.is_open()) return; // no existe todavia: defaults

    nlohmann::json j;
    try {
        in >> j;
    } catch (...) {
        return; // archivo corrupto: defaults
    }
    if (!j.is_object()) return;

    if (j.contains("version") && j["version"].is_number_integer())
        datos_.version = j["version"].get<int>();

    if (j.contains("menu") && j["menu"].is_object()) {
        const nlohmann::json& menu = j["menu"];
        if (menu.contains("proyecto") && menu["proyecto"].is_string())
            datos_.nombreProyecto = menu["proyecto"].get<std::string>();
        if (menu.contains("idioma") && menu["idioma"].is_string())
            datos_.idioma = menu["idioma"].get<std::string>();
        // El slider guarda floats; aceptar tambien enteros (p. ej. 1) por si
        // el archivo se edito a mano.
        if (menu.contains("sensibilidadCamara") && menu["sensibilidadCamara"].is_number())
            datos_.sensibilidadCamara = menu["sensibilidadCamara"].get<float>();
    }

    if (j.contains("editor") && j["editor"].is_object()) {
        const nlohmann::json& editor = j["editor"];
        if (editor.contains("ventanaCamarasAbierta") &&
            editor["ventanaCamarasAbierta"].is_boolean())
            datos_.ventanaCamarasAbierta = editor["ventanaCamarasAbierta"].get<bool>();
        if (editor.contains("gizmoOperacion") && editor["gizmoOperacion"].is_number_integer())
            datos_.gizmoOperacion = editor["gizmoOperacion"].get<int>();

        if (editor.contains("ventanas") && editor["ventanas"].is_object()) {
            for (auto it = editor["ventanas"].begin();
                 it != editor["ventanas"].end(); ++it) {
                if (it.value().is_boolean())
                    datos_.estadoVentanas[it.key()] = it.value().get<bool>();
            }
        }
    }
}

void EditorConfig::guardar(const std::string& ruta) {
    nlohmann::json j;
    j["version"] = datos_.version;
    j["menu"]["proyecto"] = datos_.nombreProyecto;
    j["menu"]["idioma"] = datos_.idioma;
    j["menu"]["sensibilidadCamara"] = datos_.sensibilidadCamara;
    j["editor"]["ventanaCamarasAbierta"] = datos_.ventanaCamarasAbierta;
    j["editor"]["gizmoOperacion"] = datos_.gizmoOperacion;
    for (const auto& [nombre, abierta] : datos_.estadoVentanas)
        j["editor"]["ventanas"][nombre] = abierta;

    std::error_code ec;
    const std::string::size_type sep = ruta.find_last_of("/\\");
    if (sep != std::string::npos)
        std::filesystem::create_directories(ruta.substr(0, sep), ec);

    std::ofstream out(ruta);
    if (!out.is_open()) return;
    out << j.dump(2) << '\n';
}