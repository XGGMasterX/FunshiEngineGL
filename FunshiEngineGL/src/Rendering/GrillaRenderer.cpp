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
#include "GrillaRenderer.h"

#include <cmath>

#include "Backend/IRenderBackend.h"

namespace {

// Empuja un segmento [x0,y0,z0]-[x1,y1,z1] al arreglo de vertices planos.
void agregarSegmento(std::vector<float>& out, float x0, float y0, float z0,
                     float x1, float y1, float z1) {
    out.push_back(x0);
    out.push_back(y0);
    out.push_back(z0);
    out.push_back(x1);
    out.push_back(y1);
    out.push_back(z1);
}

} // namespace

void GrillaRenderer::recompilarGrilla(const float colorGrilla[3]) {
    color_[0] = colorGrilla[0];
    color_[1] = colorGrilla[1];
    color_[2] = colorGrilla[2];

    const float tam = tam_;
    const float sep = sep_;
    if (tam <= 0.f || sep <= 0.f) return;

    minorVertices_.clear();
    majorVertices_.clear();

    // Lineas secundarias (cada sep unidades) que no coinciden con una linea
    // principal (cada 5 unidades).
    const float majorStep = 5.0f;
    for (float i = -tam; i <= tam; i += sep) {
        if (std::fabs(std::fmod(i, majorStep)) < 0.001f) continue;
        agregarSegmento(minorVertices_, i, 0.f, -tam, i, 0.f, tam);
        agregarSegmento(minorVertices_, -tam, 0.f, i, tam, 0.f, i);
    }

    // Lineas principales (cada 5 unidades): saltar el origen (lo pinta el eje).
    for (float i = -tam; i <= tam; i += majorStep) {
        if (std::fabs(i) < 0.001f) continue;
        agregarSegmento(majorVertices_, i, 0.f, -tam, i, 0.f, tam);
        agregarSegmento(majorVertices_, -tam, 0.f, i, tam, 0.f, i);
    }

    // Ejes X y Z (incluyen el origen; el centro se respeta por simetria).
    const float ext = tam * 0.8f;
    ejeX_[0] = 0.f;  ejeX_[1] = 0.f;  ejeX_[2] = 0.f;  // X
    ejeX_[3] = ext;  ejeX_[4] = 0.f;  ejeX_[5] = 0.f;
    ejeZ_[0] = 0.f;  ejeZ_[1] = 0.f;  ejeZ_[2] = 0.f;  // Z
    ejeZ_[3] = 0.f;  ejeZ_[4] = 0.f;  ejeZ_[5] = ext;
}

void GrillaRenderer::dibujar(const float model[16], const float colorGrilla[3],
                             float tam, float sep) {
    if (!model || !colorGrilla) return;
    if (tam <= 0.f || sep <= 0.f) return;

    // Recalcula los vectores CPU solo si cambian tamano, separacion o color
    // efectivo; mientras nada cambie, la geometria se reutiliza.
    if (minorVertices_.empty() || tam_ != tam || sep_ != sep ||
        color_[0] != colorGrilla[0] || color_[1] != colorGrilla[1] ||
        color_[2] != colorGrilla[2]) {
        tam_ = tam;
        sep_ = sep;
        recompilarGrilla(colorGrilla);
    }

    Rendering::Backend::IRenderBackend& b =
        Rendering::Backend::activeBackend();
    b.pushMatrix();
    b.multMatrix(model);
    b.setLightingEnabled(false);
    // Lineas suavizadas para todas las partes de la grilla.
    b.setLineSmoothing(true);

    // Lineas secundarias (1px).
    b.setLineWidth(1.f);
    if (!minorVertices_.empty())
        b.drawLinePairs(minorVertices_.data(),
                        static_cast<int>(minorVertices_.size() / 3));

    // Lineas principales (2px, color mas intenso).
    b.setSolidColor(colorGrilla[0] * 0.7f, colorGrilla[1] * 0.7f,
                    colorGrilla[2] * 0.7f);
    b.setLineWidth(2.f);
    if (!majorVertices_.empty())
        b.drawLinePairs(majorVertices_.data(),
                        static_cast<int>(majorVertices_.size() / 3));

    // Ejes (3px, colores brillantes): X roja, Z verde.
    b.setLineWidth(3.f);
    b.setSolidColor(1.0f, 0.3f, 0.3f);
    b.drawLinePairs(ejeX_, 2);
    b.setSolidColor(0.3f, 1.0f, 0.3f);
    b.drawLinePairs(ejeZ_, 2);

    b.setLineWidth(1.f); // Restaurar ancho por defecto
    b.setLineSmoothing(false);
    b.setLightingEnabled(true);
    b.popMatrix();
}

void GrillaRenderer::destruir() {
    minorVertices_.clear();
    majorVertices_.clear();
}