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
#ifndef MESHGPU_H
#define MESHGPU_H

#include "GLFuncs.h"

// Representación GPU de una Mesh CPU: VAO + VBOs (posición/normal/tangente/
// bitangente/UV) + EBO, con RAII (los buffers se borran en el destructor). La
// malla Mesh viene de AssetManager (compartida); el MeshGPU es un recurso GL
// del renderer y se cachea por identidad de la malla para no re-subir
// geometría redundante. Atributos: 0 = posición (vec3), 1 = normal (vec3),
// 2 = UV (vec2), 3 = tangente (vec3), 4 = bitangente (vec3).
class MeshGPU {
private:
    GLuint vao_ = 0;
    GLuint vboPos_ = 0;
    GLuint vboNormal_ = 0;
    GLuint vboTangent_ = 0;
    GLuint vboBitangent_ = 0;
    GLuint vboUv_ = 0;
    GLuint ebo_ = 0;
    GLsizei indexCount_ = 0;
    bool hasNormal_ = false;
    bool hasUv_ = false;
    bool hasTangent_ = false;

    void destroy();

public:
    MeshGPU() = default;
    ~MeshGPU();
    MeshGPU(const MeshGPU&) = delete;
    MeshGPU& operator=(const MeshGPU&) = delete;
    MeshGPU(MeshGPU&& other) noexcept;
    MeshGPU& operator=(MeshGPU&& other) noexcept;

    // Sube los buffers desde la Mesh CPU. Rendición vacía si no hay índices.
    void upload(const GLfloat* vertices, std::size_t vertexCount,
                const GLfloat* normals, std::size_t normalCount,
                const GLfloat* uvs, std::size_t uvCount,
                const GLfloat* tangents, std::size_t tangentCount,
                const GLfloat* bitangents, std::size_t bitangentCount,
                const GLuint* indices, std::size_t indexCount);

    bool isUploaded() const { return vao_ != 0; }
    GLsizei getIndexCount() const { return indexCount_; }

    // Dibuja la malla subida (glDrawElements). No-op si no hay VAO/iñdexes.
    void draw() const;
};

#endif