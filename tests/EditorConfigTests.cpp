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
        CHECK(cfg.datos().camaraActivaId == -1, "default camaraActivaId");
        CHECK(cfg.datos().estadoVentanas.empty(), "default sin ventanas");
        CHECK(cfg.datos().apariencia.temaClaro == false, "default tema oscuro");
        CHECK(cfg.datos().apariencia.blancoYNegro == false, "default no B/N");
        CHECK(cfg.datos().apariencia.acento[3] == 1.0f, "default acento opaco");
    }

    // 2. Round-trip: los valores cambiados sobreviven a guardar/cargar.
    {
        EditorConfig cfg;
        cfg.datos().nombreProyecto = "MiEscena";
        cfg.datos().idioma = "English";
        cfg.datos().sensibilidadCamara = 2.5f;
        cfg.datos().ventanaCamarasAbierta = false;
        cfg.datos().gizmoOperacion = 2;
        cfg.datos().camaraActivaId = 7;
        cfg.datos().estadoVentanas["BrowseFile"] = false;
        cfg.datos().estadoVentanas["ShowFolder"] = true;
        cfg.datos().apariencia.temaClaro = true;
        cfg.datos().apariencia.blancoYNegro = true;
        cfg.datos().apariencia.acento[0] = 0.9f;
        cfg.datos().apariencia.acento[1] = 0.1f;
        cfg.datos().apariencia.acento[2] = 0.2f;
        cfg.datos().apariencia.acento[3] = 0.5f;
        cfg.datos().apariencia.fondo[0] = 0.3f;
        cfg.datos().apariencia.fondo[1] = 0.4f;
        cfg.datos().apariencia.fondo[2] = 0.5f;
        cfg.guardar(ruta);
        CHECK(fs::exists(ruta), "se escribio el archivo");

        EditorConfig cfg2;
        cfg2.cargar(ruta);
        CHECK(cfg2.datos().nombreProyecto == "MiEscena", "roundtrip nombreProyecto");
        CHECK(cfg2.datos().idioma == "English", "roundtrip idioma");
        CHECK(cfg2.datos().sensibilidadCamara == 2.5f, "roundtrip sensibilidad");
        CHECK(cfg2.datos().ventanaCamarasAbierta == false, "roundtrip ventanaCamaras");
        CHECK(cfg2.datos().gizmoOperacion == 2, "roundtrip gizmoOperacion");
        CHECK(cfg2.datos().camaraActivaId == 7, "roundtrip camaraActivaId");
        CHECK(cfg2.datos().estadoVentanas.at("BrowseFile") == false,
              "roundtrip ventana BrowseFile");
        CHECK(cfg2.datos().estadoVentanas.at("ShowFolder") == true,
              "roundtrip ventana ShowFolder");
        CHECK(cfg2.datos().estadoVentanas.size() == 2, "cantidad de ventanas");
        CHECK(cfg2.datos().apariencia.temaClaro == true, "roundtrip temaClaro");
        CHECK(cfg2.datos().apariencia.blancoYNegro == true, "roundtrip blancoYNegro");
        CHECK(cfg2.datos().apariencia.acento[0] == 0.9f, "roundtrip acento r");
        CHECK(cfg2.datos().apariencia.acento[3] == 0.5f, "roundtrip acento a");
        CHECK(cfg2.datos().apariencia.fondo[2] == 0.5f, "roundtrip fondo b");
        CHECK(cfg2.datos().apariencia == cfg.datos().apariencia,
              "roundtrip Apariencia completa");
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

    // 5. Nuevo sistema de guardado por proyecto:
    // Cada proyecto genera su carpeta en MotorGrafico, con Memory (escena, config, imgui)
    // y su hermano srcProyectName como raiz del explorador de archivos.
    {
        const std::string baseMotor = EditorConfig::directorioBaseMotorGrafico();
        CHECK(!baseMotor.empty(), "directorioBaseMotorGrafico no vacio");

        const std::string proyNombre = "JuegoPrueba";
        const std::string proyDir = EditorConfig::directorioProyecto(proyNombre);
        const std::string memDir = EditorConfig::directorioMemory(proyNombre);
        const std::string srcDir = EditorConfig::directorioSrc(proyNombre);
        const std::string rootName = EditorConfig::nombreRaizSrc(proyNombre);

        CHECK(rootName == "srcJuegoPrueba", "nombreRaizSrc correcto");
        CHECK(proyDir == baseMotor + "/" + proyNombre, "directorioProyecto correcto");
        CHECK(memDir == proyDir + "/Memory", "directorioMemory dentro del proyecto");
        CHECK(srcDir == proyDir + "/srcJuegoPrueba", "directorioSrc hermano de Memory");
        CHECK(EditorConfig::rutaConfiguracion(proyNombre) == memDir + "/Configuracion.json",
              "rutaConfiguracion dentro de Memory");
        CHECK(EditorConfig::rutaSceneBBDD(proyNombre) == memDir + "/Binarios/SceneBBDDObjetos.txt",
              "rutaSceneBBDD dentro de Memory/Binarios");
        CHECK(EditorConfig::rutaSceneDir(proyNombre) == memDir + "/Binarios/Scene/",
              "rutaSceneDir dentro de Memory/Binarios/Scene/");
        CHECK(EditorConfig::rutaImguiIni(proyNombre) == memDir + "/imgui.ini",
              "rutaImguiIni dentro de Memory");

        // Asegurar que la creacion de estructura crea las carpetas en disco
        EditorConfig::asegurarEstructuraProyecto(proyNombre);
        CHECK(fs::is_directory(memDir + "/Binarios/Scene"),
              "asegurarEstructuraProyecto creo Memory/Binarios/Scene");
        CHECK(fs::is_directory(srcDir),
              "asegurarEstructuraProyecto creo srcJuegoPrueba");

        // Limpieza de prueba
        std::error_code ec;
        fs::remove_all(proyDir, ec);
    }

    fs::remove_all(base);
    std::cout << "Pruebas: " << total << ", fallos: " << fallos << std::endl;
    if (fallos == 0) std::cout << "EDITORCONFIG TESTS OK" << std::endl;
    return fallos == 0 ? 0 : 1;
}