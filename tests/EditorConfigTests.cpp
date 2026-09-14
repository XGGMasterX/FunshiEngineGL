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
// Pruebas headless de EditorConfig (la configuracion del editor en JSON):
// tolerancia ante archivo ausente/corrupto/parcial y round-trip escrito-leido.
// Sin pila grafica: solo std C++17 + nlohmann/json del intermedio.

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "../FunshiEngineGL/src/Configuracion/EditorConfig.h"

namespace fs = std::filesystem;

namespace {
int total = 0;
int fallos = 0;

#define CHECK(cond, msg)                                                      \
    do {                                                                      \
        ++total;                                                              \
        if (!(cond)) {                                                        \
            ++fallos;                                                         \
            std::cout << "FALLO: " << msg << " (linea " << __LINE__ << ")"    \
                      << std::endl;                                           \
        }                                                                     \
    } while (0)
} // namespace

int main() {
    const fs::path base = fs::temp_directory_path() / "funshi_editorconfig_tests";
    fs::remove_all(base);
    fs::create_directories(base);
    const std::string ruta = (base / "editor_config.json").string();

    // 1. Sin archivo: todo default, sin crashear.
    {
        EditorConfig cfg;
        cfg.cargar(ruta);
        CHECK(cfg.datos().nombreProyecto == "Nuevo Proyecto", "default nombreProyecto");
        CHECK(cfg.datos().idioma == "Espanol", "default idioma");
        CHECK(cfg.datos().sensibilidadCamara == 1.0f, "default sensibilidad");
        CHECK(cfg.datos().ventanaCamarasAbierta == true, "default ventanaCamaras");
        CHECK(cfg.datos().gizmoOperacion == 7, "default gizmoOperacion");
        CHECK(cfg.datos().estadoVentanas.empty(), "default sin ventanas");
    }

    // 2. Round-trip: los valores cambiados sobreviven a guardar/cargar.
    {
        EditorConfig cfg;
        cfg.datos().nombreProyecto = "MiEscena";
        cfg.datos().idioma = "English";
        cfg.datos().sensibilidadCamara = 2.5f;
        cfg.datos().ventanaCamarasAbierta = false;
        cfg.datos().gizmoOperacion = 2;
        cfg.datos().estadoVentanas["BrowseFile"] = false;
        cfg.datos().estadoVentanas["ShowFolder"] = true;
        cfg.guardar(ruta);
        CHECK(fs::exists(ruta), "se escribio el archivo");

        EditorConfig cfg2;
        cfg2.cargar(ruta);
        CHECK(cfg2.datos().nombreProyecto == "MiEscena", "roundtrip nombreProyecto");
        CHECK(cfg2.datos().idioma == "English", "roundtrip idioma");
        CHECK(cfg2.datos().sensibilidadCamara == 2.5f, "roundtrip sensibilidad");
        CHECK(cfg2.datos().ventanaCamarasAbierta == false, "roundtrip ventanaCamaras");
        CHECK(cfg2.datos().gizmoOperacion == 2, "roundtrip gizmoOperacion");
        CHECK(cfg2.datos().estadoVentanas.at("BrowseFile") == false,
              "roundtrip ventana BrowseFile");
        CHECK(cfg2.datos().estadoVentanas.at("ShowFolder") == true,
              "roundtrip ventana ShowFolder");
        CHECK(cfg2.datos().estadoVentanas.size() == 2, "cantidad de ventanas");
    }

    // 3. Archivo corrupto: defaults (sin crash).
    {
        {
            std::ofstream f(ruta, std::ios::trunc);
            f << "{ json roto";
        }
        EditorConfig cfg;
        cfg.cargar(ruta);
        CHECK(cfg.datos().nombreProyecto == "Nuevo Proyecto", "corrupto -> defaults");
        CHECK(cfg.datos().idioma == "Espanol", "corrupto -> defaults idioma");
    }

    // 4. Parcial: el campo presente se aplica, el ausente conserva el default.
    {
        {
            std::ofstream f(ruta, std::ios::trunc);
            f << "{\n  \"version\": 1,\n  \"editor\": {\n"
                 "    \"ventanaCamarasAbierta\": false\n  }\n}\n";
        }
        EditorConfig cfg;
        cfg.cargar(ruta);
        CHECK(cfg.datos().ventanaCamarasAbierta == false,
              "parcial: campo presente se aplica");
        CHECK(cfg.datos().sensibilidadCamara == 1.0f,
              "parcial: campo ausente conserva default");
        CHECK(cfg.datos().nombreProyecto == "Nuevo Proyecto",
              "parcial: sin seccion menu -> default");
    }

    fs::remove_all(base);
    std::cout << "Pruebas: " << total << ", fallos: " << fallos << std::endl;
    if (fallos == 0) std::cout << "EDITORCONFIG TESTS OK" << std::endl;
    return fallos == 0 ? 0 : 1;
}