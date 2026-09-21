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

// Render de la grilla del editor con cache de display lists.
//
// Extraido de GameScene (Fase 1 del desacoplamiento de OpenGL): concentra en
// Rendering la geometria de la grilla (lineas principales cada 5 unidades,
// secundarias cada 'sep' y ejes X/Z de colores) compilada en display lists
// que se reutilizan mientras no cambien tamano, separacion o color efectivo.
// El color efectivo se calcula afuera (AparienciaUtil::grillaEfectiva), aqui
// solo se dibuja. glLineWidth no se guarda en display lists, por eso cada
// parte se aplica con su ancho al llamar las listas.
class GrillaRenderer {
public:
    ~GrillaRenderer() { destruir(); }

    // Dibuja la grilla en la posicion 'model' con tam/sep del componente Grid
    // y el color efectivo. Recompila las listas solo si cambian esos datos.
    void dibujar(const float model[16], const float colorGrilla[3], float tam,
                 float sep);

    // Libera las display lists (higiene defensiva: contexto GL vivo).
    void destruir();

private:
    void recompilarGrilla(const float colorGrilla[3]);

    unsigned int listasMajor_ = 0;
    unsigned int listasMinor_ = 0;
    unsigned int listasAxes_ = 0;
    float tam_ = 0.0f;
    float sep_ = 0.0f;
    float color_[3] = {0.0f, 0.0f, 0.0f};
};

#endif