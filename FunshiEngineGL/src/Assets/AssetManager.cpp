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
#include "AssetManager.h"

#include "AssetException.h"
#include "AssetPath.h"

AssetManager::AssetManager(std::unique_ptr<IMeshLoader> loader)
    : loader_(std::move(loader)) {}

std::shared_ptr<const Mesh> AssetManager::getMesh(const std::string& path) {
    const std::string key = AssetPath::normalize(path);
    const auto it = meshes_.find(key);
    if (it != meshes_.end()) return it->second;

    if (!loader_)
        throw AssetNotFoundException(key);

    std::shared_ptr<Mesh> mesh = loader_->load(key); // puede lanzar
    if (!mesh || mesh->isEmpty())
        throw AssetLoadException(key, "el loader devolvio una malla vacia");

    return meshes_.emplace(key, std::move(mesh)).first->second;
}

void AssetManager::putMesh(const std::string& path, std::shared_ptr<Mesh> mesh) {
    if (!mesh || mesh->isEmpty())
        throw AssetLoadException(path, "no se puede registrar una malla vacia");
    meshes_[AssetPath::normalize(path)] = std::move(mesh);
}

bool AssetManager::containsMesh(const std::string& path) const {
    return meshes_.find(AssetPath::normalize(path)) != meshes_.end();
}

size_t AssetManager::meshCount() const { return meshes_.size(); }

void AssetManager::removeMesh(const std::string& path) {
    meshes_.erase(AssetPath::normalize(path));
}

void AssetManager::clearUnusedMeshes() {
    for (auto it = meshes_.begin(); it != meshes_.end();) {
        // use_count == 1 => solo el manager referencia esa malla: nadie la
        // esta usando en este momento, es seguro evictarla.
        if (it->second.use_count() == 1)
            it = meshes_.erase(it);
        else
            ++it;
    }
}

void AssetManager::reloadMesh(const std::string& path) {
    const std::string key = AssetPath::normalize(path);
    const auto it = meshes_.find(key);
    if (it == meshes_.end()) {
        getMesh(key); // carga inicial si no existia
        return;
    }
    if (!loader_)
        throw AssetLoadException(key, "no hay loader registrado para recargar");

    std::shared_ptr<Mesh> fresh = loader_->load(key);
    // Swap: los holders actuales siguen apuntando al Mesh viejo (valido);
    // el nuevo queda para los proximos getMesh().
    it->second = std::move(fresh);
}

void AssetManager::setLoader(std::unique_ptr<IMeshLoader> loader) {
    loader_ = std::move(loader);
}