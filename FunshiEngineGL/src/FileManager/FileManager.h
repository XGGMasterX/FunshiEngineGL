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
#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <memory>
#include <string>

#include "../GestorDeArchivos/GestorDeArchivos.h"
#include "FileSelection.h"

class GestorDeArchivos;
class FileSystemWatcher;

// Fachada del explorador de archivos (mismo patron que EditorController y la
// fachada MenuGUI): es DUENA del modelo (GestorDeArchivos) y del estado de
// navegacion compartido (FileSelection). Expone las operaciones de dominio
// (crear, copiar, eliminar) que antes estaban inline en las vistas con
// system()/popen()/ifstream. Las vistas solo conversan con esta fachada,
// nunca con GestorDeArchivos ni con el Filesystem directamente.
class FileManager {
public:
    explicit FileManager(const std::string& pathProyect, const std::string& rootName = "MotorGrafico");
    ~FileManager();

    FileManager(const FileManager&) = delete;
    FileManager& operator=(const FileManager&) = delete;

    FileSelection* getSelection() { return &selection; }
    ArbolEnlazado<File*>* getArbol() { return gestor->getTreeFilePath(); }
    const std::string& getPathProyect() const noexcept { return pathProyect; }
    const std::string& getRootName() const noexcept { return rootName; }

    // Cambia la raiz del proyecto que vigila y explora el FileManager
    void setProyecto(const std::string& nuevoPath, const std::string& nuevoRootName);

    // Rescanea el Filesystem y reconstruye el arbol si cambio. Re-resuelve
    // rutaVisible en el nuevo arbol (los punteros al arbol viejo quedarian
    // colgando).
    void refrescar();

    // Cambios externos detectados por FileSystemWatcher: drena el fd (aplica
    // vigilancia a carpetas nuevas) y consume la marca al consultarse. El
    // arbol la usa como condicion de rescaneo ademas de contadorCambios.
    bool huboCambiosExternos();

    bool crearCarpeta(const std::string& ruta);
    bool eliminarCarpeta(const std::string& ruta);
    bool crearArchivo(const std::string& ruta, const std::string& contenido);
    bool copiarCarpeta(const std::string& origen, const std::string& destino);
    bool copiarArchivo(const std::string& origen, const std::string& destino);
    bool renombrar(const std::string& ruta, const std::string& nuevoNombre);

    // Busca por ruta completa en el arbol vigente (navegacion diferida del
    // doble clic). Devuelve nullptr si la ruta ya no existe (carpeta borrada
    // en otro lugar).
    Carpeta* buscarCarpetaPorRuta(const std::string& ruta);

private:
    std::string pathProyect;
    std::string rootName;
    std::unique_ptr<GestorDeArchivos> gestor;
    std::unique_ptr<FileSystemWatcher> vigilante;
    FileSelection selection;
};

#endif