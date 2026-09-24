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
// Pruebas headless de la terminal del proyecto (Herramientas/Terminal):
// la eleccion de emulador por sistema operativo y el armado de la linea de
// ejecucion son codigo puro y se verifican aqui sin escritorio ni shell.
// Lo que NO se ejercita (y por diseño no debe) es el fork/exec de Linux ni el
// ShellExecute de Windows: abririan ventanas de verdad.

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "Herramientas/Terminal.h"

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

bool contiene(const std::vector<Terminal::Candidato>& lista,
              const std::string& comando) {
    for (const Terminal::Candidato& c : lista)
        if (c.comando == comando) return true;
    return false;
}

} // namespace

int main() {
    // 1. La lista de candidatos cubre los emuladores habituales de Linux.
    const std::vector<Terminal::Candidato> lista = Terminal::candidatos();
    CHECK(!lista.empty(), "hay candidatos de terminal");
    CHECK(contiene(lista, "gnome-terminal"), "incluye gnome-terminal");
    CHECK(contiene(lista, "konsole"), "incluye konsole");
    CHECK(contiene(lista, "xfce4-terminal"), "incluye xfce4-terminal");
    CHECK(contiene(lista, "alacritty"), "incluye alacritty");
    CHECK(contiene(lista, "kitty"), "incluye kitty");
    CHECK(contiene(lista, "x-terminal-emulator"), "incluye x-terminal-emulator");
    for (const Terminal::Candidato& c : lista)
        CHECK(!c.comando.empty(), "ningun candidato viene sin comando");

    // 2. Emulador que acepta la ruta: la recibe como ultimo argumento.
    {
        const Terminal::Candidato gnome = {"gnome-terminal",
                                          {"--working-directory"}, true};
        const std::vector<std::string> argv =
            Terminal::componerLinux(gnome, "/home/user/Proyectos/Mi Juego");
        CHECK(argv.size() == 3, "argv = comando + flag + ruta");
        CHECK(argv[0] == "gnome-terminal", "argv[0] es el comando");
        CHECK(argv[1] == "--working-directory", "argv[1] es el flag del SO");
        CHECK(argv[2] == "/home/user/Proyectos/Mi Juego",
              "la ruta viaja tal cual, con espacios y sin quoting de shell");
    }

    // 3. Emulador que NO acepta la ruta: la resuelve el chdir del hijo, asi que
    //    la ruta no debe aparecer en el argv.
    {
        const Terminal::Candidato generico = {"x-terminal-emulator", {}, false};
        const std::vector<std::string> argv =
            Terminal::componerLinux(generico, "/tmp/proyecto");
        CHECK(argv.size() == 1, "argv solo con el comando");
        CHECK(argv[0] == "x-terminal-emulator", "argv[0] es el comando");
        for (const std::string& argumento : argv)
            CHECK(argumento != "/tmp/proyecto",
                  "la ruta no se pasa como argumento si el emulador no la admite");
    }

    // 4. Windows: con Windows Terminal se abre la pestana en la ruta; sin el, el
    //    respaldo cmd.exe hace el cd.
    {
        const Terminal::VentanaWindows conWt =
            Terminal::componerWindows("C:\\Proyectos\\Mi Juego", true);
        CHECK(conWt.archivo == "wt.exe", "con Windows Terminal se usa wt.exe");
        CHECK(conWt.parametros == "-d \"C:\\Proyectos\\Mi Juego\"",
              "wt recibe la ruta entrecomillada en -d");

        const Terminal::VentanaWindows sinWt =
            Terminal::componerWindows("C:\\Proyectos\\Mi Juego", false);
        CHECK(sinWt.archivo == "cmd.exe", "sin Windows Terminal se usa cmd.exe");
        CHECK(sinWt.parametros == "/K cd /d \"C:\\Proyectos\\Mi Juego\"",
              "cmd recibe el cd al proyecto entrecomillado");
    }

    // 5. Casos que deben fallar SIN lanzar nada: sin proyecto y con una carpeta
    //    de proyecto inexistente.
    {
        const Terminal::Resultado sinProyecto = Terminal::abrirProyecto("");
        CHECK(!sinProyecto.abierta, "sin proyecto no se abre terminal");
        CHECK(!sinProyecto.mensaje.empty(), "sin proyecto informa el motivo");

        const Terminal::Resultado inexistente =
            Terminal::abrirProyecto("__proyecto_que_no_existe_12345__");
        CHECK(!inexistente.abierta, "carpeta inexistente no abre terminal");
        CHECK(!inexistente.mensaje.empty(),
              "carpeta inexistente informa el motivo");
    }

    std::cout << "Pruebas: " << total << ", fallos: " << fallos << std::endl;
    if (fallos == 0) std::cout << "TERMINAL TESTS OK" << std::endl;
    return fallos == 0 ? 0 : 1;
}
