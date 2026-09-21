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
#ifndef GRILLARENDERER_H
#define GRILLARENDERER_H

#include <vector>

// Render de la grilla del editor con cache de geometria CPU.
//
// Extraido de GameScene (Fase 1 del desacoplamiento de OpenGL): concentra en
// Rendering la geometria de la grilla (lineas principales cada 5 unidades,
// secundarias cada 'sep' y ejes X/Z de colores) precalculada en vectores CPU
// de pares de segmentos que se reutilizan mientras no cambien tamano,
// separacion o color efectivo. En la Fase 2 las display lists desaparecieron:
// la geometria se envia al backend en cada frame (drawLinePairs), porque en un
// backend moderno (Vulkan) no existe el concepto de call list. El color
// efectivo se calcula afuera (AparienciaUtil::grillaEfectiva), aqui solo se
// dibuja. El ancho de linea se aplica en cada parte (wireframe plano, sin la
// restriction de las display lists).
class GrillaRenderer {
public:
    ~GrillaRenderer() { destruir(); }

    // Dibuja la grilla en la posicion 'model' con tam/sep del componente Grid
    // y el color efectivo. Recompila los vectores CPU solo si cambian esos
    // datos.
    void dibujar(const float model[16], const float colorGrilla[3], float tam,
                 float sep);

    // Descarta los vectores CPU cacheados (higiene defensiva).
    void destruir();

private:
    void recompilarGrilla(const float colorGrilla[3]);

    std::vector<float> minorVertices_;
    std::vector<float> majorVertices_;
    float ejeX_[6] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};
    float ejeZ_[6] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};
    float tam_ = 0.0f;
    float sep_ = 0.0f;
    float color_[3] = {0.0f, 0.0f, 0.0f};
};

#endif