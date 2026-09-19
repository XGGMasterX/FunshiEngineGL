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

std::string EditorConfig::directorioBaseMotorGrafico() {
#if defined(_WIN32)
    return "C:/MotorGraficoArchivos";
#else
    const char* home = std::getenv("HOME");
    return std::string(home ? home : ".") + "/MotorGrafico";
#endif
}

std::string EditorConfig::directorioProyecto(const std::string& nombreProyecto) {
    const std::string nombre = nombreProyecto.empty() ? "Nuevo Proyecto" : nombreProyecto;
    return directorioBaseMotorGrafico() + "/" + nombre;
}

std::string EditorConfig::directorioMemory(const std::string& nombreProyecto) {
    return directorioProyecto(nombreProyecto) + "/Memory";
}

std::string EditorConfig::directorioSrc(const std::string& nombreProyecto) {
    return directorioProyecto(nombreProyecto) + "/" + nombreRaizSrc(nombreProyecto);
}

std::string EditorConfig::nombreRaizSrc(const std::string& nombreProyecto) {
    const std::string nombre = nombreProyecto.empty() ? "Nuevo Proyecto" : nombreProyecto;
    return "src" + nombre;
}

std::string EditorConfig::rutaConfiguracion(const std::string& nombreProyecto) {
    return directorioMemory(nombreProyecto) + "/Configuracion.json";
}

std::string EditorConfig::directorioBinarios(const std::string& nombreProyecto) {
    return directorioMemory(nombreProyecto) + "/Binarios";
}

std::string EditorConfig::rutaScenePrefijo(const std::string& nombreProyecto) {
    return directorioBinarios(nombreProyecto) + "/Scene";
}

std::string EditorConfig::rutaSceneBBDD(const std::string& nombreProyecto) {
    return directorioBinarios(nombreProyecto) + "/SceneBBDDObjetos.txt";
}

std::string EditorConfig::rutaSceneDir(const std::string& nombreProyecto) {
    return directorioBinarios(nombreProyecto) + "/Scene/";
}

std::string EditorConfig::rutaImguiIni(const std::string& nombreProyecto) {
    return directorioMemory(nombreProyecto) + "/imgui.ini";
}

void EditorConfig::asegurarEstructuraProyecto(const std::string& nombreProyecto) {
    const std::string nombre = nombreProyecto.empty() ? "Nuevo Proyecto" : nombreProyecto;
    const std::string dirMemory = directorioMemory(nombre);
    const std::string dirScene = dirMemory + "/Binarios/Scene";
    const std::string dirSrc = directorioSrc(nombre);

    std::error_code ec;
    std::filesystem::create_directories(dirScene, ec);
    std::filesystem::create_directories(dirSrc, ec);

    // Migracion automatica si venimos de la version anterior donde se guardaba
    // directamente en MotorGrafico:
    const std::string base = directorioBaseMotorGrafico();
    const std::string oldBBDD = base + "/Binarios/SceneBBDDObjetos.txt";
    const std::string newBBDD = dirMemory + "/Binarios/SceneBBDDObjetos.txt";
    if (!std::filesystem::exists(newBBDD, ec) && std::filesystem::exists(oldBBDD, ec)) {
        std::filesystem::copy_file(oldBBDD, newBBDD, std::filesystem::copy_options::overwrite_existing, ec);
        const std::string oldSceneDir = base + "/Binarios/Scene";
        if (std::filesystem::exists(oldSceneDir, ec)) {
            std::filesystem::copy(oldSceneDir, dirScene,
                                  std::filesystem::copy_options::recursive |
                                  std::filesystem::copy_options::overwrite_existing, ec);
        }
    }

    const std::string oldConfig = base + "/Configuracion.json";
    const std::string newConfig = dirMemory + "/Configuracion.json";
    if (!std::filesystem::exists(newConfig, ec) && std::filesystem::exists(oldConfig, ec)) {
        std::filesystem::copy_file(oldConfig, newConfig, std::filesystem::copy_options::overwrite_existing, ec);
    }
}

std::string EditorConfig::rutaPorDefecto() {
    return rutaConfiguracion("Nuevo Proyecto");
}

std::string EditorConfig::directorioProyectoPorDefecto() {
    return directorioMemory("Nuevo Proyecto");
}

void EditorConfig::cargar(const std::string& ruta) {
    std::ifstream in(ruta);
    if (!in.is_open()) {
        if (ruta == rutaPorDefecto()) {
            const std::string legacyRuta = directorioBaseMotorGrafico() + "/Configuracion.json";
            in.open(legacyRuta);
            if (!in.is_open()) return;
        } else {
            return;
        }
    }

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
        if (editor.contains("camaraActivaId") && editor["camaraActivaId"].is_number_integer())
            datos_.camaraActivaId = editor["camaraActivaId"].get<int>();

        if (editor.contains("ventanas") && editor["ventanas"].is_object()) {
            for (auto it = editor["ventanas"].begin();
                 it != editor["ventanas"].end(); ++it) {
                if (it.value().is_boolean())
                    datos_.estadoVentanas[it.key()] = it.value().get<bool>();
            }
        }
    }

    // Apariencia (tema, modo B/N, acento y fondo 3D). Tolerante: cada campo
    // ausente o invalido conserva el default del perfil.
    if (j.contains("apariencia") && j["apariencia"].is_object()) {
        const nlohmann::json& ap = j["apariencia"];
        if (ap.contains("temaClaro") && ap["temaClaro"].is_boolean())
            datos_.apariencia.temaClaro = ap["temaClaro"].get<bool>();
        if (ap.contains("blancoYNegro") && ap["blancoYNegro"].is_boolean())
            datos_.apariencia.blancoYNegro = ap["blancoYNegro"].get<bool>();
        if (ap.contains("acento") && ap["acento"].is_array() &&
            ap["acento"].size() == 4) {
            for (int i = 0; i < 4; ++i)
                if (ap["acento"][i].is_number())
                    datos_.apariencia.acento[i] = ap["acento"][i].get<float>();
        }
        if (ap.contains("fondo") && ap["fondo"].is_array() &&
            ap["fondo"].size() == 3) {
            for (int i = 0; i < 3; ++i)
                if (ap["fondo"][i].is_number())
                    datos_.apariencia.fondo[i] = ap["fondo"][i].get<float>();
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
    j["editor"]["camaraActivaId"] = datos_.camaraActivaId;
    for (const auto& [nombre, abierta] : datos_.estadoVentanas)
        j["editor"]["ventanas"][nombre] = abierta;

    j["apariencia"]["temaClaro"] = datos_.apariencia.temaClaro;
    j["apariencia"]["blancoYNegro"] = datos_.apariencia.blancoYNegro;
    j["apariencia"]["acento"] = {datos_.apariencia.acento[0],
                                 datos_.apariencia.acento[1],
                                 datos_.apariencia.acento[2],
                                 datos_.apariencia.acento[3]};
    j["apariencia"]["fondo"] = {datos_.apariencia.fondo[0],
                                datos_.apariencia.fondo[1],
                                datos_.apariencia.fondo[2]};

    std::error_code ec;
    const std::string::size_type sep = ruta.find_last_of("/\\");
    if (sep != std::string::npos)
        std::filesystem::create_directories(ruta.substr(0, sep), ec);

    std::ofstream out(ruta);
    if (!out.is_open()) return;
    out << j.dump(2) << '\n';
}