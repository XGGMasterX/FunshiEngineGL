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
#ifndef ASSIMPMESHLOADER_H
#define ASSIMPMESHLOADER_H

#include "AssetManager.h"

// Carga meshes desde disco con Assimp (vertices, normales, UVs e indices),
// como IMeshLoader del AssetManager. Vive en su propio .cpp con Assimp
// para que las pruebas headless del paquete Assets no enlacen la libreria.
class AssimpMeshLoader : public IMeshLoader {
public:
    // Devuelve la malla, o lanza AssetLoadException si el archivo no puede
    // leerse o no tiene geometria util.
    std::shared_ptr<Mesh> load(const std::string& path) override;
};

#endif