/*
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
#include "Terminal.h"

#include <cstdlib>
#include <filesystem>

#include "../Configuracion/ProjectPaths.h"

#if defined(_WIN32)
#include <shellapi.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

namespace {

namespace fs = std::filesystem;

// Busca un ejecutable en el PATH del proceso. Equivalente portable de
// "command -v" sin pasar por un shell.
bool existeEnPath(const std::string& nombre) {
    if (nombre.empty()) return false;
    const char* pathEnv = std::getenv("PATH");
    if (!pathEnv) return false;
    const std::string separador =
#if defined(_WIN32)
        ";";
#else
        ":";
#endif
    const std::string camino(pathEnv);
    std::size_t inicio = 0;
    while (inicio <= camino.size()) {
        const std::size_t fin = camino.find(separador, inicio);
        const std::string carpeta = camino.substr(
            inicio, fin == std::string::npos ? std::string::npos : fin - inicio);
        if (!carpeta.empty()) {
            std::error_code ec;
            const fs::path completo = fs::path(carpeta) / nombre;
            if (fs::is_regular_file(completo, ec)) return true;
        }
        if (fin == std::string::npos) break;
        inicio = fin + 1;
    }
    return false;
}

#if defined(_WIN32)

// Lanzamiento en Windows con ShellExecute: el motor ya lo usa para abrir
// archivos y carpetas, asi que no sumamos otra via ni dependencias.
Terminal::Resultado ejecutarWindows(const std::string& ruta) {
    const Terminal::VentanaWindows ventana =
        Terminal::componerWindows(ruta, existeEnPath("wt.exe"));
    const HINSTANCE resultado = ShellExecuteA(
        nullptr, "open", ventana.archivo.c_str(),
        ventana.parametros.empty() ? nullptr : ventana.parametros.c_str(),
        nullptr, SW_SHOW);
    if (resultado > 32) return {true, "Terminal abierta en " + ruta};
    return {false, "No se pudo abrir la terminal"};
}

#else

// Lanzamiento en Linux con fork + exec (sin shell): el hijo cambia de directorio
// y se desprende con setsid, asi el editor no queda esperando. Los nombres de
// proyecto pueden traer comillas o metacaracteres: al no pasar por un shell,
// llegan como argumentos opacos.
Terminal::Resultado ejecutarLinux(const std::string& ruta) {
    for (const Terminal::Candidato& candidato : Terminal::candidatos()) {
        if (!existeEnPath(candidato.comando)) continue;
        const std::vector<std::string> argv =
            Terminal::componerLinux(candidato, ruta);
        std::vector<char*> crudos;
        crudos.reserve(argv.size() + 1);
        for (const std::string& argumento : argv)
            crudos.push_back(const_cast<char*>(argumento.c_str()));
        crudos.push_back(nullptr);

        const pid_t hijo = fork();
        if (hijo == 0) {
            setsid();  // Se desprende del grupo de procesos del editor
            if (::chdir(ruta.c_str()) != 0) _exit(126);
            execvp(crudos[0], crudos.data());
            _exit(127);
        }
        if (hijo > 0) {
            // El padre no espera: la terminal vive fuera del editor.
            return {true, "Terminal abierta en " + ruta};
        }
        // fork fallo: se prueba el siguiente candidato.
    }
    return {false, "No se encontro una terminal instalada (probadas: "
                   "gnome-terminal, konsole, xfce4-terminal, alacritty, "
                   "kitty, x-terminal-emulator)"};
}

} // namespace

namespace Terminal {

std::vector<Candidato> candidatos() {
    std::vector<Candidato> lista;
    // Convencion de escritorio: si el usuario fija TERMINAL, manda esa.
    if (const char* preferred = std::getenv("TERMINAL"))
        if (*preferred)
            lista.push_back({preferred, {}, false});
    lista.push_back({"gnome-terminal", {"--working-directory"}, true});
    lista.push_back({"konsole", {"--workdir"}, true});
    lista.push_back({"xfce4-terminal", {"--working-directory"}, true});
    lista.push_back({"alacritty", {"--working-directory"}, true});
    lista.push_back({"kitty", {"--directory"}, true});
    // Ultimos recursos: heredan el directorio por el chdir del hijo.
    lista.push_back({"x-terminal-emulator", {}, false});
    lista.push_back({"lxterminal", {}, false});
    lista.push_back({"st", {}, false});
    return lista;
}

std::vector<std::string> componerLinux(const Candidato& candidato,
                                       const std::string& ruta) {
    std::vector<std::string> argv;
    argv.push_back(candidato.comando);
    for (const std::string& argumento : candidato.argumentos)
        argv.push_back(argumento);
    if (candidato.rutaComoArgumento) argv.push_back(ruta);
    return argv;
}

VentanaWindows componerWindows(const std::string& ruta, bool hayWindowsTerminal) {
    if (hayWindowsTerminal) {
        // Windows Terminal: -d deja el directorio como cwd de la pestana nueva.
        return {"wt.exe", "-d \"" + ruta + "\""};
    }
    // Respaldo garantizado: cmd.exe con un cd al proyecto.
    return {"cmd.exe", "/K cd /d \"" + ruta + "\""};
}

Resultado abrirProyecto(const std::string& nombreProyecto) {
    if (nombreProyecto.empty()) return {false, "No hay un proyecto abierto"};
    const std::string ruta = ProjectPaths::directorioProyecto(nombreProyecto);
    std::error_code ec;
    if (!fs::is_directory(ruta, ec))
        return {false, "No se encontro la carpeta del proyecto: " + ruta};
#if defined(_WIN32)
    return ejecutarWindows(ruta);
#else
    return ejecutarLinux(ruta);
#endif
}

} // namespace Terminal


#endif


