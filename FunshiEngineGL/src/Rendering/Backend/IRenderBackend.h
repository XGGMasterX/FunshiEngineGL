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
#ifndef IRENDERBACKEND_H
#define IRENDERBACKEND_H

#include <cstddef>
#include <cstdint>

#include <glm/glm.hpp>

namespace Rendering {
namespace Backend {

// Handle opaco a un recurso GPU (0 = vacio/null). El engine trabaja a traves
// de la interfaz; el backend concreto (OpenGL3Backend hoy, VulkanBackend en el
// futuro) traduce cada handle a su objeto nativo (GLuint, VkBuffer, ...).
using Handle = std::uint64_t;
constexpr Handle kInvalidHandle = 0;

// Geometria CPU a subir a GPU (misma informacion que los parametros posicionales
// de MeshGPU::upload, pero agrupada para la interfaz).
struct MeshData {
    const float* vertices = nullptr;
    std::size_t vertexCount = 0;
    const float* normals = nullptr;
    std::size_t normalCount = 0;
    const float* uvs = nullptr;
    std::size_t uvCount = 0;
    const float* tangents = nullptr;
    std::size_t tangentCount = 0;
    const float* bitangents = nullptr;
    std::size_t bitangentCount = 0;
    const unsigned int* indices = nullptr;
    std::size_t indexCount = 0;
};

// Pixels RGBA8 sin firmar de una imagen CPU (para subir texturas 2D).
struct Image2D {
    int width = 0;
    int height = 0;
    const unsigned char* pixels = nullptr;
};

// Contrato de backend grafico. Los recursos se crean/destruyen por handle y
// las operaciones de dibujado son inmediatas o pequenas (malla indexada,
// primitivas de linea, triangulos del fallback legacy). Los consumidores de la
// capa de entidades nunca ven GL: todo pasa por aca (o por sus wrappers RAII).
class IRenderBackend {
public:
    virtual ~IRenderBackend() = default;

    // Carga las funciones de GPU (por puntero, "same fashion as GLFW"). Devuelve
    // false si el pipeline moderno no esta disponible (degradacion al inmediato).
    virtual bool init() = 0;
    virtual bool available() const = 0;

    // --- Malla GPU (VAO/VBO/EBO o su analogo nativo) -------------------------
    virtual Handle createMesh(const MeshData& data) = 0;
    virtual void destroyMesh(Handle mesh) = 0;
    virtual void drawMesh(Handle mesh, unsigned int indexCount) = 0;

    // --- Textura 2D ----------------------------------------------------------
    virtual Handle createTexture2D(const Image2D& image) = 0;
    virtual void destroyTexture2D(Handle texture) = 0;
    // Bindeo del sampleo en la unidad indicada (GL_TEXTURE0 + unit).
    virtual void bindTexture2D(Handle texture, int unit) = 0;

    // --- Programa de shaders (pipeline moderno) ------------------------------
    // createProgram puede lanzar (falla de compilacion/limite): la excepcion
    // es especifica del backend (ShaderCompile/Link/Unavailable) pero el
    // contrato no impone un tipo.
    virtual Handle createProgram(const char* vertexSource,
                                 const char* fragmentSource) = 0;
    virtual void destroyProgram(Handle program) = 0;
    virtual void useProgram(Handle program) = 0;
    virtual int uniformLocation(Handle program, const char* name) = 0;
    virtual void setUniformMat4(int location, const glm::mat4& value) = 0;
    virtual void setUniformMat3(int location, const glm::mat3& value) = 0;
    virtual void setUniformVec3(int location, const glm::vec3& value) = 0;
    virtual void setUniformVec4(int location, const float* value) = 0;
    virtual void setUniformFloat(int location, float value) = 0;
    virtual void setUniformInt(int location, int value) = 0;

    // --- Render target (FBO texturizado para viewports) ----------------------
    virtual Handle createRenderTarget(int width, int height) = 0;
    virtual void destroyRenderTarget(Handle target) = 0;
    virtual void bindRenderTarget(Handle target) = 0;
    virtual void bindDefaultFramebuffer() = 0;
    // Handle de la textura de color del target (para mostrarla como ImTextureID).
    virtual Handle renderTargetColorTexture(Handle target) const = 0;

    // --- Estado del pipeline de compatibilidad / stack de matrices -----------
    // Operaciones heredadas del modo inmediato (grilla, wireframes, fallback
    // legacy). El backend las traduce a su API nativa; en Vulkan estas llamadas
    // desaparecen a favor de buffers, por eso aqui solo se declara el minimo.
    virtual void pushMatrix() = 0;
    virtual void popMatrix() = 0;
    virtual void multMatrix(const float mat4[16]) = 0;
    // translate/scale/rotate en ese orden exacto (semantica del stack legacy).
    virtual void applyTransform(const float translate[3], const float scale[3],
                                const float rotate4[4]) = 0;
    virtual void setLightingEnabled(bool enabled) = 0;
    virtual void setPolygonFill() = 0;
    virtual void setSolidColor(float r, float g, float b) = 0;
    virtual void setMaterial(const float ambient[4], const float diffuse[4],
                             const float specular[4], const float emission[4],
                             float shininess) = 0;
    virtual void setLineWidth(float width) = 0;
    virtual void setLineSmoothing(bool enabled) = 0;

    // --- Primitivas inmediatas (en el espacio local del modelo actual) -------
    // vertices planos; cada par consecutivo [0..1], [2..3], ... es un segmento.
    virtual void drawLinePairs(const float* vertices, int vertexCount) = 0;
    // Lineas por indices: edges es un arreglo de edgeCount*2 ints.
    virtual void drawIndexedLines(const float* vertices, int vertexCount,
                                  const int* edgeIndices, int edgeCount) = 0;
    // Polilinea abierta (GL_LINE_STRIP) o cerrada (GL_LINE_LOOP).
    virtual void drawLineStrip(const float* vertices, int vertexCount,
                               bool closed) = 0;
    // Triangulos indexados con normales (fallback legacy de malla).
    virtual void drawTriangles(const float* vertices, int vertexCount,
                               const float* normals, int normalCount,
                               const unsigned int* indices, int indexCount) = 0;
};

// Backend activo del engine (un solo contexto GL; el singleton se cambia en la
// migracion a Vulkan). El concreto vive en OpenGL3Backend.cpp.
IRenderBackend& activeBackend();

} // namespace Backend
} // namespace Rendering

#endif