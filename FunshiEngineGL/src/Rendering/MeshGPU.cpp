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
#include "MeshGPU.h"

#include <cstddef>

#include "../GLCompat.h"

namespace {

// Sube un buffer con N floats y lo enlaza como atributo si hay datos.
bool subirBuffer(GLenum target, GLuint buffer, const GLfloat* data,
                 std::size_t count) {
    if (!data || count == 0) return false;
    GLFuncs::pfnBindBuffer(target, buffer);
    GLFuncs::pfnBufferData(
        target, static_cast<GLsizeiptr>(count * sizeof(GLfloat)), data,
        GL_STATIC_DRAW);
    return true;
}

} // namespace

MeshGPU::~MeshGPU() { destroy(); }

void MeshGPU::destroy() {
    if (GLFuncs::pfnDeleteVertexArrays && vao_) {
        GLFuncs::pfnDeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }
    if (GLFuncs::pfnDeleteBuffers &&
        (vboPos_ || vboNormal_ || vboTangent_ || vboBitangent_ || vboUv_ ||
         ebo_)) {
        const GLuint buffers[6] = {vboPos_, vboNormal_, vboTangent_,
                                   vboBitangent_, vboUv_, ebo_};
        GLFuncs::pfnDeleteBuffers(6, buffers);
    }
    vboPos_ = vboNormal_ = vboTangent_ = vboBitangent_ = vboUv_ = ebo_ = 0;
    indexCount_ = 0;
    hasNormal_ = false;
    hasUv_ = false;
    hasTangent_ = false;
}

MeshGPU::MeshGPU(MeshGPU&& other) noexcept {
    vao_ = other.vao_;
    vboPos_ = other.vboPos_;
    vboNormal_ = other.vboNormal_;
    vboTangent_ = other.vboTangent_;
    vboBitangent_ = other.vboBitangent_;
    vboUv_ = other.vboUv_;
    ebo_ = other.ebo_;
    indexCount_ = other.indexCount_;
    hasNormal_ = other.hasNormal_;
    hasUv_ = other.hasUv_;
    hasTangent_ = other.hasTangent_;
    other.vao_ = other.vboPos_ = other.vboNormal_ = other.vboTangent_ =
        other.vboBitangent_ = other.vboUv_ = other.ebo_ = 0;
    other.indexCount_ = 0;
    other.hasNormal_ = other.hasUv_ = other.hasTangent_ = false;
}

MeshGPU& MeshGPU::operator=(MeshGPU&& other) noexcept {
    if (this != &other) {
        destroy();
        vao_ = other.vao_;
        vboPos_ = other.vboPos_;
        vboNormal_ = other.vboNormal_;
        vboTangent_ = other.vboTangent_;
        vboBitangent_ = other.vboBitangent_;
        vboUv_ = other.vboUv_;
        ebo_ = other.ebo_;
        indexCount_ = other.indexCount_;
        hasNormal_ = other.hasNormal_;
        hasUv_ = other.hasUv_;
        hasTangent_ = other.hasTangent_;
        other.vao_ = other.vboPos_ = other.vboNormal_ = other.vboTangent_ =
            other.vboBitangent_ = other.vboUv_ = other.ebo_ = 0;
        other.indexCount_ = 0;
        other.hasNormal_ = other.hasUv_ = other.hasTangent_ = false;
    }
    return *this;
}

void MeshGPU::upload(const GLfloat* vertices, std::size_t vertexCount,
                     const GLfloat* normals, std::size_t normalCount,
                     const GLfloat* uvs, std::size_t uvCount,
                     const GLfloat* tangents, std::size_t tangentCount,
                     const GLfloat* bitangents, std::size_t bitangentCount,
                     const GLuint* indices, std::size_t indexCount) {
    if (indexCount == 0 || !vertices || vertexCount == 0) return;

    destroy();

    GLFuncs::pfnGenVertexArrays(1, &vao_);
    GLFuncs::pfnBindVertexArray(vao_);

    GLFuncs::pfnGenBuffers(6, &vboPos_);

    // Posición -> atributo 0.
    if (subirBuffer(GL_ARRAY_BUFFER, vboPos_, vertices, vertexCount * 3)) {
        GLFuncs::pfnEnableVertexAttribArray(0);
        GLFuncs::pfnVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0,
                                        (const void*)nullptr);
    }

    // Normal -> atributo 1 (opcional, si hay una por vértice).
    if (normals && normalCount == vertexCount &&
        subirBuffer(GL_ARRAY_BUFFER, vboNormal_, normals, normalCount * 3)) {
        GLFuncs::pfnEnableVertexAttribArray(1);
        GLFuncs::pfnVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0,
                                        (const void*)nullptr);
        hasNormal_ = true;
    }

    // Tangente/Bitangente -> atributos 3/4 (normal mapping, opcionales).
    if (tangents && tangentCount == vertexCount && bitangents &&
        bitangentCount == vertexCount) {
        if (subirBuffer(GL_ARRAY_BUFFER, vboTangent_, tangents,
                        tangentCount * 3)) {
            GLFuncs::pfnEnableVertexAttribArray(3);
            GLFuncs::pfnVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0,
                                            (const void*)nullptr);
        }
        if (subirBuffer(GL_ARRAY_BUFFER, vboBitangent_, bitangents,
                        bitangentCount * 3)) {
            GLFuncs::pfnEnableVertexAttribArray(4);
            GLFuncs::pfnVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0,
                                            (const void*)nullptr);
        }
        hasTangent_ = true;
    }

    // UV -> atributo 2 (opcional, si hay una por vértice).
    if (uvs && uvCount == vertexCount &&
        subirBuffer(GL_ARRAY_BUFFER, vboUv_, uvs, uvCount * 2)) {
        GLFuncs::pfnEnableVertexAttribArray(2);
        GLFuncs::pfnVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0,
                                        (const void*)nullptr);
        hasUv_ = true;
    }

    // Índices -> EBO (se graban en el VAO por estar bindeados con el VAO activo).
    if (indices && indexCount > 0) {
        GLFuncs::pfnBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
        GLFuncs::pfnBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(indexCount * sizeof(GLuint)), indices,
            GL_STATIC_DRAW);
        indexCount_ = static_cast<GLsizei>(indexCount);
    }

    GLFuncs::pfnBindVertexArray(0);
}

void MeshGPU::draw() const {
    if (!vao_ || indexCount_ == 0) return;
    GLFuncs::pfnBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, indexCount_, GL_UNSIGNED_INT,
                   (const void*)nullptr);
    GLFuncs::pfnBindVertexArray(0);
}