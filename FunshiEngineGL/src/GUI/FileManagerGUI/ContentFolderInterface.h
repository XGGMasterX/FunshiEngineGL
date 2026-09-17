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
#ifndef CONTENTFOLDERINTERFACE_H
#define CONTENTFOLDERINTERFACE_H

#include <filesystem>
#include <string>
#include <vector>

#include "../GeneralUserInterface.h"

class FileManager;
class IconosGUI;

// Panel "ShowFolder": muestra el contenido de la carpeta seleccionada en el
// arbol. Ya no se enlaza al arbol por puntero ni le pide el contenido: lee
// cada frame la seleccion compartida (FileSelection), se muestra a si mismo
// cuando hay carpeta (y se oculta si no) y notifica su navegacion por doble
// clic dejando la ruta pendiente en la seleccion (R3). Las operaciones de
// Filesystem van a la fachada FileManager, nunca a system(). (R1)
class ContentFolderInterface : public GeneralUserInterface {
private:
    // Entrada del grid (R5): el directorio se lee en disco SOLO cuando cambia
    // la ruta mostrada o su mtime; el dibujo del grid usa este cache en vez de
    // re-scanear con directory_iterator cada frame.
    struct GridEntry {
        std::string nombre;
        std::string fullPath;
        bool esCarpeta = false;
        std::string extension;
    };

    FileManager* fileManager = nullptr;
    bool abrirPopupNombre = false;
    bool creandoCarpeta = false;
    bool creandoScript = false;
    bool creandoScriptJava = false;
    char nombreNuevo[128] = "";
    IconosGUI* iconosGUI = nullptr;

    // Estado de renombrado (R6): ruta del elemento, si es carpeta (sube el
    // contador del arbol) y el buffer con el nombre a confirmar en el modal.
    std::string renombrarRuta;
    bool renombrarEsCarpeta = false;
    bool abrirPopupRenombrar = false;
    char bufferRenombrar[128] = "";

    // Cache del grid (R5).
    std::vector<GridEntry> cacheEntradas;
    std::string cacheCarpeta;
    std::filesystem::file_time_type cacheMtime{};

    std::string seleccionarCarpetaSistema();
    std::string seleccionarArchivoSistema();
    void crearNuevoElemento();
    void copiarElementoSuelto(const std::string& origen, const std::string& destFolder);
    void recorrer(const std::string& path);

public:
    ContentFolderInterface(bool stateGUI, FileManager* fileManager);

    void setIconosGUI(IconosGUI* iconosG);

    virtual void initGUI() override;
    virtual void contentGUI() override;
    virtual void endGUI() override;
    virtual void printGUI() override;
};

#endif