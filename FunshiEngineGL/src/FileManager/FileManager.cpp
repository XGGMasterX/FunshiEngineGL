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
#include "FileManager.h"

#include <filesystem>

#include "FileSystemWatcher.h"
#include "../GestorDeArchivos/Carpeta.h"
#include "../Herramientas/PathUtils.h"
#include "../Herramientas/TreeGUI/TreeGUI.h"

namespace {
// Busca pre-orden la primera Carpeta cuya ruta completa coincida.
Carpeta* buscarPreOrden(ArbolEnlazado<File*>* arbol,
                        Position<File*>* current,
                        const std::string& ruta) {
    if (!arbol || !current) return nullptr;
    File* elemento = current->getElement();
    if (elemento) {
        // Comparacion de rutas como std::filesystem::path, NO de string crudo:
        // en Windows '/' y '\\' son equivalentes pero la representacion puede
        // diferir segun quien armo la ruta (el arbol vs una ingresada a mano),
        // y string == las trataria como rutas distintas.
        const std::filesystem::path rutaNodo =
            elemento->getPathRoot() + PATH_SEP + elemento->getPathName();
        if (rutaNodo == std::filesystem::path(ruta))
            return dynamic_cast<Carpeta*>(elemento);
    }
    if (arbol->isInternal(current)) {
        Carpeta* encontrado = nullptr;
        TreeIG::forEachChild((TNodo<File*>*)current, [&](Position<File*>* hijo) {
            if (!encontrado) encontrado = buscarPreOrden(arbol, hijo, ruta);
        });
        return encontrado;
    }
    return nullptr;
}
} // namespace

FileManager::FileManager(const std::string& pathProyect, const std::string& rootName)
    : pathProyect(pathProyect),
      rootName(rootName.empty() ? "MotorGrafico" : rootName),
      gestor(new GestorDeArchivos(pathProyect, this->rootName)),
      vigilante(new FileSystemWatcher(pathProyect)) {}

FileManager::~FileManager() = default;

void FileManager::setProyecto(const std::string& nuevoPath, const std::string& nuevoRootName) {
    if (pathProyect == nuevoPath && rootName == nuevoRootName) return;
    pathProyect = nuevoPath;
    rootName = nuevoRootName.empty() ? "MotorGrafico" : nuevoRootName;
    std::error_code ec;
    std::filesystem::create_directories(pathProyect, ec);
    vigilante = std::make_unique<FileSystemWatcher>(pathProyect);
    gestor->setTreeFilePath(pathProyect, rootName);
    FileSelection* sel = getSelection();
    sel->rutaVisible = pathProyect;
    sel->carpetaActual = nullptr;
    if (gestor->getTreeFilePath() && !gestor->getTreeFilePath()->isEmpty()) {
        Position<File*>* rootPos = gestor->getTreeFilePath()->rootOfTree();
        if (rootPos && rootPos->getElement())
            sel->carpetaActual = dynamic_cast<Carpeta*>(rootPos->getElement());
    }
    sel->navegacionPendiente.clear();
    sel->contadorCambios++;
}

void FileManager::refrescar() {
    FileSelection* sel = getSelection();
    const std::string rutaVisible = sel->rutaVisible;
    // GestorDeArchivos ya comprueba por rutas y solo reconstruye si cambio.
    gestor->setTreeFilePath(pathProyect, rootName);
    // Los punteros al arbol viejo quedaron liberados (o apuntarian a una
    // seleccion caducada): re-resolvemos la carpeta visible por su ruta.
    sel->carpetaActual = nullptr;
    if (!rutaVisible.empty())
        sel->carpetaActual = buscarCarpetaPorRuta(rutaVisible);
    if (!sel->carpetaActual && gestor->getTreeFilePath() && !gestor->getTreeFilePath()->isEmpty()) {
        Position<File*>* rootPos = gestor->getTreeFilePath()->rootOfTree();
        if (rootPos && rootPos->getElement()) {
            sel->carpetaActual = dynamic_cast<Carpeta*>(rootPos->getElement());
            if (sel->carpetaActual) sel->rutaVisible = pathProyect;
        }
    }
}

bool FileManager::huboCambiosExternos() {
    return vigilante ? vigilante->huboCambiosYConsumir() : false;
}

bool FileManager::crearCarpeta(const std::string& ruta) {
    return gestor->crearCarpeta(ruta);
}

bool FileManager::eliminarCarpeta(const std::string& ruta) {
    return gestor->eliminarCarpeta(ruta);
}

bool FileManager::crearArchivo(const std::string& ruta, const std::string& contenido) {
    return gestor->crearArchivo(ruta, contenido);
}

bool FileManager::copiarCarpeta(const std::string& origen, const std::string& destino) {
    return gestor->copiarCarpeta(origen, destino);
}

bool FileManager::copiarArchivo(const std::string& origen, const std::string& destino) {
    return gestor->copiarArchivo(origen, destino);
}

bool FileManager::renombrar(const std::string& ruta, const std::string& nuevoNombre) {
    return gestor->renombrar(ruta, nuevoNombre);
}

Carpeta* FileManager::buscarCarpetaPorRuta(const std::string& ruta) {
    ArbolEnlazado<File*>* arbol = gestor->getTreeFilePath();
    if (!arbol || arbol->isEmpty()) return nullptr;
    return buscarPreOrden(arbol, arbol->rootOfTree(), ruta);
}