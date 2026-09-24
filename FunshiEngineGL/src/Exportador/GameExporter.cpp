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
#include "GameExporter.h"

#include <thread>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;

GameExporter::GameExporter(const Config& cfg) : cfg_(cfg) {}

GameExporter::~GameExporter() {
    if (hiloExportacion_.joinable()) hiloExportacion_.join();
}

void GameExporter::iniciar() {
    terminado_ = false;
    hiloExportacion_ = std::thread(&GameExporter::runExportacion, this);
}

bool GameExporter::haTerminado() const noexcept {
    return terminado_;
}

void GameExporter::runExportacion() {
    log("Iniciando exportación de: " + cfg_.nombreProyectoExportado);
    progreso(0.05f, "Preparando directorio de build");

    // Directorio temporal de build
    std::string buildDir = cfg_.directorioSalida + "_build";
    std::error_code ec;
    fs::remove_all(buildDir, ec);
    fs::create_directories(buildDir, ec);

    if (!generarProyectoCMake(buildDir)) {
        finalizar(false, "Error generando CMake");
        return;
    }
    progreso(0.25f, "CMake generado");

    if (!compilarScriptsUsuario(buildDir)) {
        finalizar(false, "Error compilando scripts de usuario");
        return;
    }
    progreso(0.5f, "Scripts compilados");

    if (!compilarJuego(buildDir)) {
        finalizar(false, "Error compilando el juego");
        return;
    }
    progreso(0.75f, "Juego compilado");

    if (!copiarAssetsYDependencias(buildDir)) {
        finalizar(false, "Error copiando assets/dependencias");
        return;
    }
    progreso(0.9f, "Assets copiados");

    if (!empaquetarDistribucion(buildDir)) {
        finalizar(false, "Error empaquetando distribución");
        return;
    }

    // Limpiar build temporal
    fs::remove_all(buildDir, ec);

    finalizar(true, "Exportación completada en: " + cfg_.directorioSalida);
}

bool GameExporter::generarProyectoCMake(const std::string& buildDir) {
    std::string cmakeContent = R"(
cmake_minimum_required(VERSION 3.15)
project() LANGUAGES CXX

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configuración de runtime (sin editor)
set(FUNSHI_RUNTIME_ONLY 1)

# Rutas del motor (injected por el exportador)
set(FUNSHI_SRC_DIR "")  # Se rellena abajo
set(FUNSHI_CXX_COMPILER "")  # Se rellena abajo

# Proyectos del usuario (scripts)
add_subdirectory(${CMAKE_SOURCE_DIR}/scripts)

# Ejecutable del juego
add_executable(${PROJECT_NAME} "")
target_link_libraries(${PROJECT_NAME} PRIVATE funshi_runtime)

# Instalación: copiar a directorio de distribución
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION .
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
)";

    // Obtener rutas reales
    std::string engineSrc = fs::absolute(fs::path(cfg_.proyectoOrigen).parent_path().parent_path() / "src").string();
    std::string cxxCompiler = "g++"; // TODO: detectar

    // Reemplazar placeholders
    size_t pos;
    while ((pos = cmakeContent.find("FUNSHI_SRC_DIR")) != std::string::npos) {
        cmakeContent.replace(pos, 14, "FUNSHI_SRC_DIR \"" + engineSrc + "\"");
    }
    while ((pos = cmakeContent.find("FUNSHI_CXX_COMPILER")) != std::string::npos) {
        cmakeContent.replace(pos, 19, "FUNSHI_CXX_COMPILER \"" + cxxCompiler + "\"");
    }

    // Toolchain para cross-compile Windows
    if (cfg_.plataforma == Plataforma::Windows) {
        cmakeContent += R"(
# Toolchain MinGW para Windows
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)
set(CMAKE_FIND_LIBRARY_CUSTOM_LIB_SUFFIX ".dll.a")
set(CMAKE_FIND_LIBRARY_CUSTOM_PATH_SUFFIXES "/x86_64-w64-mingw32")
)";
    }

    // Guardar CMakeLists.txt
    std::ofstream cmakeFile(buildDir + "/CMakeLists.txt");
    if (!cmakeFile) return false;
    cmakeFile << cmakeContent;
    cmakeFile.close();

    // Generar CMakeLists.txt para scripts de usuario
    std::string scriptsDir = cfg_.proyectoOrigen + "/scripts";
    if (fs::exists(scriptsDir)) {
        std::string scriptsCMake = "cmake_minimum_required(VERSION 3.15)\n";
        scriptsCMake += "project(UserScripts LANGUAGES CXX)\n";
        scriptsCMake += "set(CMAKE_CXX_STANDARD 17)\n";
        scriptsCMake += "file(GLOB SCRIPT_SOURCES *.cpp)\n";
        scriptsCMake += "foreach(script ${SCRIPT_SOURCES})\n";
        scriptsCMake += "  get_filename_component(name ${script} NAME_WE)\n";
        scriptsCMake += "  add_library(${name} MODULE ${script})\n";
        scriptsCMake += "  target_include_directories(${name} PRIVATE ${FUNSHI_SRC_DIR})\n";
        scriptsCMake += "  target_link_libraries(${name} PRIVATE funshi_runtime)\n";
        scriptsCMake += "endforeach()\n";
        std::error_code ec;
        fs::create_directories(buildDir + "/scripts", ec);
        std::ofstream scm(buildDir + "/scripts/CMakeLists.txt");
        scm << scriptsCMake;
        scm.close();
    }

    return true;
}

bool GameExporter::compilarScriptsUsuario(const std::string& buildDir) {
    std::string scriptsDir = buildDir + "/scripts";
    if (!fs::exists(scriptsDir)) return true; // sin scripts

    // Configurar y compilar scripts
    std::string cmd = "cd " + scriptsDir + " && cmake .. && make -j4";
    int result = std::system(cmd.c_str());
    return result == 0;
}

bool GameExporter::compilarJuego(const std::string& buildDir) {
    std::string cmd = "cd " + buildDir + " && cmake .. && make -j4";
    int result = std::system(cmd.c_str());
    return result == 0;
}

bool GameExporter::copiarAssetsYDependencias(const std::string& buildDir) {
    // Copiar Memory/, Sonidos/, ConfiguracionProyecto.json
    std::vector<std::pair<std::string, std::string>> copiar = {
        {cfg_.proyectoOrigen + "/Memory", cfg_.directorioSalida + "/Data/Memory"},
        {cfg_.proyectoOrigen + "/Sonidos", cfg_.directorioSalida + "/Data/Sonidos"},
        {cfg_.proyectoOrigen + "/ConfiguracionProyecto.json", cfg_.directorioSalida + "/Data/ConfiguracionProyecto.json"}
    };

    for (auto& [src, dst] : copiar) {
        if (fs::exists(src)) {
            std::error_code ec;
            fs::create_directories(fs::path(dst).parent_path(), ec);
            fs::copy(src, dst, fs::copy_options::recursive | fs::copy_options::overwrite_existing, ec);
            if (ec) return false;
        }
    }

    // Copiar dependencias (DLLs/.so) - Bullet, miniaudio, GLFW, etc.
    // Por simplicidad, asumimos que están en rutas estándar del sistema
    // En producción se haría bundle real con ldd/objdump
    return true;
}

bool GameExporter::empaquetarDistribucion(const std::string& buildDir) {
    // El ejecutable compilado está en buildDir/
    std::string exeName = obtenerExtensionEjecutable();
    std::string exeSrc = buildDir + "/" + exeName;
    std::string exeDst = cfg_.directorioSalida + "/" + exeName;

    std::error_code ec;
    fs::create_directories(cfg_.directorioSalida, ec);
    if (!fs::exists(exeSrc)) return false;
    fs::copy_file(exeSrc, exeDst, fs::copy_options::overwrite_existing, ec);
    return !ec;
}

std::string GameExporter::obtenerExtensionEjecutable() const {
    if (cfg_.plataforma == Plataforma::Windows) return cfg_.nombreEjecutable + ".exe";
    return cfg_.nombreEjecutable;
}

std::string GameExporter::obtenerToolchainCMake() const {
    return "";
}

void GameExporter::log(const std::string& msg) {
    if (cfg_.onLog) cfg_.onLog(msg);
    std::cout << "[Exporter] " << msg << std::endl;
}

void GameExporter::progreso(float p, const std::string& etapa) {
    if (cfg_.onProgreso) cfg_.onProgreso(p, etapa);
}

void GameExporter::finalizar(bool ok, const std::string& msg) {
    terminado_ = true;
    exito_ = ok;
    mensajeFinal_ = msg;
    if (cfg_.onFinalizado) cfg_.onFinalizado(ok, msg);
}