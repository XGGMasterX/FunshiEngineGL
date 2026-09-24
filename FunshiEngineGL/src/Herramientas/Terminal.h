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
#ifndef TERMINAL_H
#define TERMINAL_H

#include <string>
#include <vector>

// Terminal del sistema abierta DESDE el editor y ubicada en el proyecto.
//
// Es la version liviana de una terminal integrada: no incrusta una PTY en la
// ventana (eso exigiria conpty/ptyprocess y una dependencia nueva), si que
// lanza la terminal nativa del sistema operativo con el directorio del proyecto
// como working directory, igual que el atajo de VS Code. Un clic y ya se esta
// en la carpeta correcta.
//
// Decision de seguridad (misma regla que el explorador de archivos): nunca se
// pasa la ruta por system()/shell. En Linux se hace fork + exec directo con
// chdir en el hijo, y en Windows se usa ShellExecute con parametros, de modo
// que un nombre de proyecto con comillas o metacaracteres no puede inyectar una
// orden.
namespace Terminal {

// Emulador de Linux y como se le pasa el directorio de trabajo. Casi todos lo
// reciben en la linea de ordenes; los que no, se resuelven con chdir en el
// hijo (rutaComoArgumento = false).
struct Candidato {
    std::string comando;
    std::vector<std::string> argumentos;
    bool rutaComoArgumento;
};

// Candidatos de Linux por orden de preferencia. Si la variable de entorno
// TERMINAL esta definida, se prueba primero (es la convencion de la mayoria de
// los escritorios y permite forzar uno concreto).
std::vector<Candidato> candidatos();

// argv completo con el que se lanzaria el candidato desde la ruta dada. Es
// codigo puro: el ejecutable real solo lo usa en el fork del hijo.
std::vector<std::string> componerLinux(const Candidato& candidato,
                                       const std::string& ruta);

// Como se lanza la terminal en Windows: Windows Terminal si esta instalado y si
// no el cmd.exe de siempre (con un /K cd al directorio del proyecto).
struct VentanaWindows {
    std::string archivo;
    std::string parametros;
};
VentanaWindows componerWindows(const std::string& ruta, bool hayWindowsTerminal);

struct Resultado {
    bool abierta = false;
    std::string mensaje;
};

// Abre la terminal con el directorio del proyecto como working directory. Si
// nombreProyecto esta vacio o no hay terminal disponible lo informa en vez de
// fallar en silencio (el editor muestra ese mensaje en la barra de estado).
Resultado abrirProyecto(const std::string& nombreProyecto);

} // namespace Terminal

#endif
