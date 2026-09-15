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
#include "Modelos3D.h"

#include <GL/gl.h>
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>

#include "../Assets/AssetException.h"
#include "../Assets/AssetManager.h"
#include "../Assets/AssimpMeshLoader.h"
#include "../Objetos/Componentes/Material.h"
#include "../ExcepcionesCPP/RuntimeException.h"

Modelos3D::Modelos3D(Entity* origin) : GameObject(origin) {}
Modelos3D::Modelos3D() : GameObject() {}

void Modelos3D::setPath(std::string path) {
    filePath_ = std::move(path);
    setObject();
}

std::string Modelos3D::getPath() { return filePath_; }

void Modelos3D::setObject() {
    mesh_.reset();
    if (filePath_.empty()) return;
    try {
        // Con el AssetManager inyectado la malla se comparte: pedir el mismo
        // path en N objetos devuelve el mismo asset (1 parseo de Assimp).
        // Sin manager se usa el loader local como respaldo (inspector por
        // defecto). Los fallos no matan el editor: queda sin malla y se
        // informa por consola.
        mesh_ = assets_ ? assets_->getMesh(filePath_)
                        : AssimpMeshLoader().load(filePath_);
    } catch (const RuntimeException& e) {
        std::cerr << "Modelos3D: no se pudo cargar malla '" << filePath_
                  << "': " << e.what() << '\n';
        mesh_.reset();
    } catch (const std::exception& e) {
        std::cerr << "Modelos3D: error inesperado al cargar '" << filePath_
                  << "': " << e.what() << '\n';
        mesh_.reset();
    }
}

void Modelos3D::dibujar(float deltaTime) {
    update(deltaTime);
    Transform* transform = getGlobalTransform();
    if (transform) transform->position();
    if (Model* model = getComponent<Model>(); model && model->getPath() != filePath_)
        setPath(model->getPath());
    if (Material* material = getComponent<Material>()) {
        material->aplicar();
    } else if (Color* color = getComponent<Color>()) {
        glMaterialfv(GL_FRONT, GL_DIFFUSE, color->getColor());
    } else {
        const GLfloat white[] = {1.f, 1.f, 1.f, 1.f};
        glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (mesh_) {
        const bool dibujaNormales = mesh_->hasNormals();
        const std::vector<vec3>& normals = mesh_->normals;
        glBegin(GL_TRIANGLES);
        for (unsigned int idx : mesh_->indices) {
            if (idx >= mesh_->vertices.size()) continue;
            if (dibujaNormales) {
                const vec3& normal = normals[idx];
                if (!(normal.x == 0.f && normal.y == 0.f && normal.z == 0.f))
                    glNormal3fv(&normal.x);
            }
            glVertex3fv(&mesh_->vertices[idx].x);
        }
        glEnd();
    }
    glPopMatrix();
}

void Modelos3D::serializeEntity() {
    const size_t length = filePath_.size();
    std::ofstream* out = myBinario->getOfBinariFile();
    out->write(reinterpret_cast<const char*>(&length), sizeof(size_t));
    if (length > 0) out->write(filePath_.data(), static_cast<std::streamsize>(length));
}

void Modelos3D::deserializeEntity() {
    size_t length = 0;
    std::ifstream* in = myBinario->getIfBinariFile();
    in->read(reinterpret_cast<char*>(&length), sizeof(size_t));
    // Cota de sanidad: un length corrupto no debe reservar cientos de MB.
    const size_t toRead = std::min(length, static_cast<size_t>(4096));
    if (toRead > 0) {
        std::string buffer(toRead, '\0');
        in->read(buffer.data(), static_cast<std::streamsize>(toRead));
        filePath_.assign(buffer, 0, toRead);
        setObject();
    } else {
        filePath_.clear();
    }
}

void Modelos3D::saveEntity(std::string filename) {
    const std::string path = filename + "/ObjectN" + std::to_string(getId()) + ".db";
    myBinario = std::make_unique<Binario>(path);
    myBinario->ofOpenBinary();
    
    // Leer todo el contenido del archivo
    std::string fullPath = filename + "BBDDObjetos.txt";
    std::ifstream readFile(fullPath);
    std::string content;
    if (readFile.is_open()) {
        std::string line;
        while (std::getline(readFile, line)) {
            content += line + "\n";
        }
        readFile.close();
    }
    
    // Agregar el nuevo path
    content += path + "\n";
    
    // Escribir todo el contenido de una vez
    std::ofstream writeFile(fullPath);
    if (writeFile.is_open()) {
        writeFile << content;
        writeFile.close();
    }
    
    GameObject::serializeEntity();
    serializeEntity();
    myBinario->ofCloseBinary();
}

void Modelos3D::loadEntity(std::string filename) {
    const std::string path = filename + "/ObjectN" + std::to_string(getId()) + ".db";
    myBinario = std::make_unique<Binario>(path);
    myBinario->ifOpenBinary();
    GameObject::deserializeEntity();
    deserializeEntity();
    myBinario->ifCloseBinary();
}

bool Modelos3D::getBoundingBox(vec3& outMin, vec3& outMax) const {
    return mesh_ && mesh_->computeBounds(outMin, outMax);
}

const std::vector<vec3>& Modelos3D::getVertices() const {
    // Referencia estable compartida para la colision cuando no hay malla
    // cargada: nunca se devuelve nullptr a los consumidores (MallaCollider).
    static const std::vector<vec3> verticesVacios;
    return mesh_ ? mesh_->vertices : verticesVacios;
}