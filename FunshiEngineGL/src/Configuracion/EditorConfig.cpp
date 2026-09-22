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
#include <cstring>
#include <filesystem>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// ============================================================================
// Persistencia de la configuracion del editor en JSON (nlohmann, vendoriado en
// External/nlohmann). Carga tolerante: si el archivo falta, esta corrupto o le
// faltan campos, se quedan los valores por defecto y se conserva lo que si se
// pudo leer. El binario solo persiste datos de escena, no configuracion.
// ============================================================================

namespace {

// Directorio del ejecutable (sin el nombre del binario, con separador final).
// Es el ancla de los datos del motor: la config y los proyectos viven junto al
// binario, sin depender del directorio desde el que se lance ni del HOME.
std::string directorioEjecutable() {
#ifdef _WIN32
    char exe[MAX_PATH] = {};
    const DWORD n = GetModuleFileNameA(nullptr, exe, MAX_PATH);
    if (n == 0 || n >= MAX_PATH) return "";
    const std::string path(exe, static_cast<std::size_t>(n));
    const std::size_t sep = path.find_last_of("\\/");
    return (sep == std::string::npos) ? "" : path.substr(0, sep + 1);
#else
    char link[4096] = {};
    const ssize_t n = readlink("/proc/self/exe", link, sizeof(link) - 1);
    if (n <= 0) return "";
    link[n] = '\0';
    const std::string path(link);
    const std::size_t sep = path.find_last_of('/');
    return (sep == std::string::npos) ? "" : path.substr(0, sep + 1);
#endif
}

} // namespace

// ============================================================================

std::string EditorConfig::directorioBaseMotorGrafico() {
    // Relativo al ejecutable: los datos de config/proyectos se crean junto al
    // binario (build portable), no en el HOME ni en el directorio de lanzamiento.
    const std::string exeDir = directorioEjecutable();
    return exeDir.empty() ? "MotorGrafico" : exeDir + "MotorGrafico";
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

std::string EditorConfig::rutaConfiguracionGeneral() {
    // Configuracion general: hermana de las carpetas de proyecto, no dentro de ninguna.
    // Guarda solo lo independiente del proyecto: apariencia, idioma, sensibilidad,
    // y el ultimo proyecto abierto para saber cual cargar al arrancar.
    return directorioBaseMotorGrafico() + "/Configuracion.json";
}

std::string EditorConfig::rutaConfiguracionProyecto(const std::string& nombreProyecto) {
    // Configuracion especifica del proyecto: estado de ventanas, gizmo, camara activa.
    return directorioMemory(nombreProyecto) + "/ConfiguracionProyecto.json";
}

std::string EditorConfig::rutaConfiguracion(const std::string& nombreProyecto) {
    // Alias de compatibilidad: apunta a la configuracion del proyecto.
    return rutaConfiguracionProyecto(nombreProyecto);
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

std::string EditorConfig::directorioSonidos(const std::string& nombreProyecto) {
    return directorioProyecto(nombreProyecto) + "/Sonidos";
}

std::string EditorConfig::directorioInterfaces(const std::string& nombreProyecto) {
    return directorioMemory(nombreProyecto) + "/Interfaces";
}

void EditorConfig::asegurarEstructuraProyecto(const std::string& nombreProyecto) {
    const std::string nombre = nombreProyecto.empty() ? "Nuevo Proyecto" : nombreProyecto;
    const std::string dirMemory = directorioMemory(nombre);
    const std::string dirScene = dirMemory + "/Binarios/Scene";
    const std::string dirSrc = directorioSrc(nombre);
    // Assets del nuevo sistema: sonidos importados por el usuario y las
    // interfaces creadas en el creador (se guardan como JSON en Memory).
    const std::string dirSonidos = directorioSonidos(nombre);
    const std::string dirInterfaces = directorioInterfaces(nombre);

    std::error_code ec;
    std::filesystem::create_directories(dirScene, ec);
    std::filesystem::create_directories(dirSrc, ec);
    std::filesystem::create_directories(dirSonidos, ec);
    std::filesystem::create_directories(dirInterfaces, ec);

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
    // La "ruta por defecto" al arrancar sin argumento es la config general,
    // que vive en la raiz de MotorGrafico junto a las carpetas de proyecto.
    // Desde ahi se lee el ultimo proyecto abierto para saber que proyecto cargar.
    return rutaConfiguracionGeneral();
}

std::string EditorConfig::directorioProyectoPorDefecto() {
    return directorioMemory("Nuevo Proyecto");
}

// ============================================================================
// Helpers internos de lectura/escritura JSON
// ============================================================================

// Escribe un bloque JSON en disco, creando los directorios necesarios.
static void escribirJson(const std::string& ruta, const nlohmann::json& j) {
    std::error_code ec;
    const std::string::size_type sep = ruta.find_last_of("/\\");
    if (sep != std::string::npos)
        std::filesystem::create_directories(ruta.substr(0, sep), ec);
    std::ofstream out(ruta);
    if (!out.is_open()) return;
    out << j.dump(2) << '\n';
}

// Intenta abrir `ruta`; retorna el json parseado o un objeto vacio si falla.
static nlohmann::json leerJson(const std::string& ruta) {
    std::ifstream in(ruta);
    if (!in.is_open()) return nlohmann::json::object();
    nlohmann::json j;
    try {
        in >> j;
    } catch (...) {
        return nlohmann::json::object();
    }
    return j.is_object() ? j : nlohmann::json::object();
}

// ============================================================================
// Carga: la funcion publica `cargar` es el punto de entrada historico;
// delega en cargarGeneral + cargarProyecto para el arranque completo.
// ============================================================================

void EditorConfig::cargar(const std::string& ruta) {
    // ruta puede ser la config general (nueva) o la legacy; en ambos casos
    // cargamos primero la config general para obtener nombreProyecto, y luego
    // la config especifica de ese proyecto.
    cargarGeneral(ruta.empty() ? rutaConfiguracionGeneral() : ruta);
    cargarProyecto(datos_.nombreProyecto);
}

void EditorConfig::cargarGeneral(const std::string& ruta) {
    // Determina la ruta a leer: si llega vacia usa la ruta canonica general.
    const std::string rutaReal = ruta.empty() ? rutaConfiguracionGeneral() : ruta;
    nlohmann::json j = leerJson(rutaReal);

    // Compatibilidad: si el archivo no existe y se esta usando la ruta canonica,
    // busca el archivo legacy en la raiz de MotorGrafico (para instalaciones
    // que todavia tienen Configuracion.json en la raiz, antes de esta separacion).
    // No aplica si se paso una ruta explicita distinta (p. ej. tests).
    const std::string rutaCanonica = rutaConfiguracionGeneral();
    if (j.empty() && rutaReal == rutaCanonica) {
        // La ruta canonica ya ES la raiz de MotorGrafico/Configuracion.json,
        // asi que no hay legacy distinto que buscar; simplemente no hay archivo.
    }
    if (j.empty()) return;

    if (j.contains("version") && j["version"].is_number_integer())
        datos_.version = j["version"].get<int>();

    // Ultimo proyecto abierto: permite arrancar directamente en el proyecto
    // que se estaba editando sin que el usuario tenga que seleccionarlo.
    if (j.contains("ultimoProyecto") && j["ultimoProyecto"].is_string())
        datos_.nombreProyecto = j["ultimoProyecto"].get<std::string>();
    // Compatibilidad con archivos viejos que guardaban el proyecto en "menu/proyecto"
    else if (j.contains("menu") && j["menu"].is_object()) {
        const auto& menu = j["menu"];
        if (menu.contains("proyecto") && menu["proyecto"].is_string())
            datos_.nombreProyecto = menu["proyecto"].get<std::string>();
        if (menu.contains("idioma") && menu["idioma"].is_string())
            datos_.idioma = menu["idioma"].get<std::string>();
        if (menu.contains("sensibilidadCamara") && menu["sensibilidadCamara"].is_number())
            datos_.sensibilidadCamara = menu["sensibilidadCamara"].get<float>();
    }

    if (j.contains("idioma") && j["idioma"].is_string())
        datos_.idioma = j["idioma"].get<std::string>();
    if (j.contains("sensibilidadCamara") && j["sensibilidadCamara"].is_number())
        datos_.sensibilidadCamara = j["sensibilidadCamara"].get<float>();
    if (j.contains("sensibilidadMovimientoCamara") &&
        j["sensibilidadMovimientoCamara"].is_number())
        datos_.sensibilidadMovimientoCamara =
            j["sensibilidadMovimientoCamara"].get<float>();

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

void EditorConfig::cargarProyecto(const std::string& nombreProyecto,
                                  const std::string& ruta) {
    const std::string rutaReal =
        ruta.empty() ? rutaConfiguracionProyecto(nombreProyecto) : ruta;
    nlohmann::json j = leerJson(rutaReal);

    // Compatibilidad: si el archivo nuevo no existe y se usa la ruta canonica,
    // intenta el viejo nombre Configuracion.json que vivía en Memory antes de
    // esta separacion. No aplica con rutas explícitas (p. ej. tests).
    const std::string rutaCanonica = rutaConfiguracionProyecto(nombreProyecto);
    if (j.empty() && rutaReal == rutaCanonica) {
        j = leerJson(directorioMemory(nombreProyecto) + "/Configuracion.json");
    }
    if (j.empty()) return;

    if (j.contains("editor") && j["editor"].is_object()) {
        const nlohmann::json& editor = j["editor"];
        if (editor.contains("ventanaCamarasAbierta") &&
            editor["ventanaCamarasAbierta"].is_boolean())
            datos_.ventanaCamarasAbierta = editor["ventanaCamarasAbierta"].get<bool>();
        if (editor.contains("gizmoOperacion") && editor["gizmoOperacion"].is_number_integer())
            datos_.gizmoOperacion = editor["gizmoOperacion"].get<int>();
        if (editor.contains("gizmoGlobal") && editor["gizmoGlobal"].is_boolean())
            datos_.gizmoGlobal = editor["gizmoGlobal"].get<bool>();
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
}

// ============================================================================
// Guardado
// ============================================================================

void EditorConfig::guardar(const std::string& ruta) {
    // Guardado completo de compatibilidad: guarda general + proyecto.
    // En el flujo nuevo, main llama a guardarGeneral/guardarProyecto por separado.
    guardarGeneral();
    guardarProyecto(datos_.nombreProyecto);
}

void EditorConfig::guardarGeneral(const std::string& ruta) {
    const std::string rutaReal = ruta.empty() ? rutaConfiguracionGeneral() : ruta;
    nlohmann::json j;
    j["version"] = datos_.version;
    // Ultimo proyecto abierto: al arrancar se retoma este proyecto sin pedir al
    // usuario. Con el nombre vacio (primer arranque sin proyecto elegido) la
    // clave no se escribe: de lo contrario al releer quedaria "Nuevo Proyecto"
    // y se crearían sus carpetas solas la proxima vez.
    if (!datos_.nombreProyecto.empty()) j["ultimoProyecto"] = datos_.nombreProyecto;
    j["idioma"] = datos_.idioma;
    j["sensibilidadCamara"] = datos_.sensibilidadCamara;
    j["sensibilidadMovimientoCamara"] = datos_.sensibilidadMovimientoCamara;
    j["apariencia"]["temaClaro"] = datos_.apariencia.temaClaro;
    j["apariencia"]["blancoYNegro"] = datos_.apariencia.blancoYNegro;
    j["apariencia"]["acento"] = {datos_.apariencia.acento[0],
                                 datos_.apariencia.acento[1],
                                 datos_.apariencia.acento[2],
                                 datos_.apariencia.acento[3]};
    j["apariencia"]["fondo"] = {datos_.apariencia.fondo[0],
                                datos_.apariencia.fondo[1],
                                datos_.apariencia.fondo[2]};
    escribirJson(rutaReal, j);
}

void EditorConfig::guardarProyecto(const std::string& nombreProyecto,
                                   const std::string& ruta) {
    const std::string rutaReal =
        ruta.empty() ? rutaConfiguracionProyecto(nombreProyecto) : ruta;
    nlohmann::json j;
    j["version"] = datos_.version;
    j["editor"]["ventanaCamarasAbierta"] = datos_.ventanaCamarasAbierta;
    j["editor"]["gizmoOperacion"] = datos_.gizmoOperacion;
    j["editor"]["gizmoGlobal"] = datos_.gizmoGlobal;
    j["editor"]["camaraActivaId"] = datos_.camaraActivaId;
    for (const auto& [nombre, abierta] : datos_.estadoVentanas)
        j["editor"]["ventanas"][nombre] = abierta;
    escribirJson(rutaReal, j);
}