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
#ifndef FILESYSTEMWATCHER_H
#define FILESYSTEMWATCHER_H

#include <chrono>
#include <string>
#include <unordered_map>

#if !defined(__linux__)
#include <filesystem>
#endif

// Observa el directorio del proyecto y avisa cuando algo cambia FUERA de la
// mano del editor (Dolphin, terminal, un build, un importador...). El arbol
// de archivos hoy solo se rescancea cuando una operacion interna sube
// FileSelection::contadorCambios; el watcher cierra ese hueco.
//
// En Linux usa inotify con UN SOLO fd y sin hilos: el fd se abre en modo
// non-blocking y se vacia una vez por frame desde el hilo de la GUI. La
// vigilancia es recursiva (un watch por carpeta) y se re-aplica sobre las
// carpetas que aparecen al vuelo. En plataformas sin inotify cae a un polling
// temporal (un rescaneo cada 3s), que con el arbol "barato" de
// compareTreesByPath solo reconstruye si realmente cambio algo.
class FileSystemWatcher {
public:
    explicit FileSystemWatcher(const std::string& raiz);
    ~FileSystemWatcher();

    FileSystemWatcher(const FileSystemWatcher&) = delete;
    FileSystemWatcher& operator=(const FileSystemWatcher&) = delete;

    // Vacía los eventos pendientes del fd (aplicando vigilancia a carpetas
    // recien creadas) y devuelve true si hubo cambios; consume la marca.
    bool huboCambiosYConsumir();

private:
    void aplicarVigilanciaRecursiva();
    void agregarWatch(const std::string& rutaCarpeta);
    void agregarWatchRama(const std::string& raizRama);
    void leerEventos();

    std::string raiz_;
    // Linux: fd de inotify + mapa watch descriptor -> ruta de la carpeta.
    int fd_ = -1;
    std::unordered_map<int, std::string> wdDeCarpeta_;
    bool huboCambios_ = false;
#if !defined(__linux__)
    std::chrono::steady_clock::time_point ultimoPulso_;
    std::filesystem::file_time_type ultimoMtimeRaiz_{};
#endif
};

#endif