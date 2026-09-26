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
#ifndef LINERENDERER_H
#define LINERENDERER_H

#include <memory>

class LineBatch;
class ShaderProgram;

// Renderer de las lineas del engine con el pipeline moderno (VBO/VAO + shader
// de lineas gruesas): reemplaza a ImmediateRenderer (glBegin/glEnd + glLineWidth)
// que cubria la grilla, los marcadores de luz/camara y los wireframes de los
// colliders. El ancho se resuelve en pixeles dentro del shader, asi que las
// lineas se ven igual de gruesas a cualquier distancia (GL_LINES solo garantiza
// 1 px en un contexto core).
//
// El batch se mantiene fuera del renderer: cada consumidor (GrillaRenderer,
// SceneRenderer, los colliders) es dueño de su LineBatch y lo reutiliza frame a
// frame, de modo que por pase hay un solo draw por consumidor.
class LineRenderer {
public:
    LineRenderer();
    ~LineRenderer();

    bool available() const noexcept { return shader_ != nullptr; }

    // Tamano en pixeles del framebuffer donde se dibuja. El shader lo necesita
    // para convertir el ancho de linea de pixeles a NDC; hay que informarlo
    // cuando cambia el viewport (la pasada principal y cada vista previa).
    void setViewport(int width, int height) noexcept;

    // Dibuja el batch con el ancho dado en pixeles. No-op si el shader no esta
    // disponible o el batch esta vacio. "model" es la matriz del objeto que
    // aporta la geometria (la del objeto "Grilla", la del collider) o null para
    // geometria ya en mundo (marcadores).
    void dibujar(const LineBatch& batch, const float model[16],
                 const float view[16], const float projection[16],
                 float anchoPx);

    // Libera el shader (por ejemplo al reiniciar el contexto grafico).
    void reset();

private:
    std::unique_ptr<ShaderProgram> shader_;
    int viewportWidth_ = 0;
    int viewportHeight_ = 0;

    // Compila el shader de lineas la primera vez. No lanza: si el pipeline
    // moderno no esta disponible, deja available() == false.
    bool inicializar();
};

#endif
