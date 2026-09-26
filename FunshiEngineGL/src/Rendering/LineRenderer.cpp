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
#include "LineRenderer.h"

#include <cmath>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Backend/IRenderBackend.h"
#include "LineBatch.h"
#include "Shaders/ShaderProgram.h"
#include "Shaders/ShaderSources.h"

namespace {

// Distancia al plano cercano a partir de la matriz de proyeccion de la GL:
// con m[2][2] = -(f+n)/(f-n) y m[2][3] = -2fn/(f-n) sale n = m23/(m22-1).
// La usa el shader para recortar los segmentos contra el plano cercano.
float nearDesdeProyeccion(const glm::mat4& p) {
    const float den = p[2][2] - 1.0f;
    if (std::fabs(den) < 1e-6f) return 0.01f;
    const float near = p[2][3] / den;
    if (!std::isfinite(near) || near <= 0.0f) return 0.01f;
    return near;
}

} // namespace

LineRenderer::LineRenderer() = default;

LineRenderer::~LineRenderer() = default;

void LineRenderer::reset() { shader_.reset(); }

void LineRenderer::setViewport(int width, int height) noexcept {
    viewportWidth_ = width > 0 ? width : 0;
    viewportHeight_ = height > 0 ? height : 0;
}

bool LineRenderer::inicializar() {
    if (shader_) return true;
    if (!Rendering::Backend::activeBackend().init()) return false;
    try {
        shader_ = std::make_unique<ShaderProgram>(
            ShaderProgram::fromSource(kLineVertexShader, kLineFragmentShader));
    } catch (const std::exception& e) {
        std::cerr << "[LineRenderer] Shader de lineas no disponible: " << e.what()
                  << '\n';
        shader_.reset();
    }
    return shader_ != nullptr;
}

void LineRenderer::dibujar(const LineBatch& batch, const float model[16],
                           const float view[16], const float projection[16],
                           float anchoPx) {
    if (!inicializar()) return;
    if (!batch.isUploaded() || batch.getVertexCount() == 0) return;

    // Sin tamano de viewport no se puede pasar de pixeles a NDC: se omite en
    // vez de dibujar lineas de ancho arbitrario.
    if (viewportWidth_ <= 0 || viewportHeight_ <= 0) return;

    const glm::mat4 modelMatrix = model ? glm::make_mat4(model)
                                        : glm::mat4(1.0f);
    const glm::mat4 viewMatrix = view ? glm::make_mat4(view) : glm::mat4(1.0f);
    const glm::mat4 projMatrix =
        projection ? glm::make_mat4(projection) : glm::mat4(1.0f);

    shader_->use();
    shader_->setMat4("uModel", modelMatrix);
    shader_->setMat4("uView", viewMatrix);
    shader_->setMat4("uProjection", projMatrix);
    const float viewport[4] = {static_cast<float>(viewportWidth_),
                               static_cast<float>(viewportHeight_), 0.0f, 0.0f};
    shader_->setVec4("uViewport", viewport);
    shader_->setFloat("uWidth", anchoPx > 0.0f ? anchoPx : 1.0f);
    shader_->setFloat("uNear", nearDesdeProyeccion(projMatrix));

    batch.draw();
    ShaderProgram::unbind();
}
