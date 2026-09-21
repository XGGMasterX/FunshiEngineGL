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
#ifndef IMMEDIATERENDERER_H
#define IMMEDIATERENDERER_H

class Modelos3D;

// Render primitivo via el pipeline de compatibilidad del backend grafico.
//
// Concentra TODO el uso de modo inmediato del engine en un solo modulo de
// Rendering: los marcadores (luz/camara), los wireframes de los colliders y el
// fallback de los modelos cuando el pipeline moderno (MeshRenderer) no esta
// disponible ya no tocan la API grafica concreta desde fuera de aqui.
//
// Cada funcion aplica la matriz modelo (columna-mayor, 16 floats como la
// produce buildMatrixFromTransform) pusheando al stack del backend, apaga la
// iluminacion para dibujar lineas de color solido o la deja encendida para las
// mallas con normales, y restaura el estado al terminar. Este modulo es el
// candidato natural para desaparecer cuando exista un backend grafico propio:
// sus consumidores solo necesitan "dibujar estas lineas / esta malla".
namespace ImmediateRenderer {

// Dibuja segmentos explícitos: 'vertices' es un arreglo de vertexCount*3
// floats y se consume en pares (cada par [0..1], [2..3], ... es una linea).
void dibujarSegmentos(const float* vertices, int vertexCount,
                      const float color[3], const float model[16],
                      float lineWidth = 1.0f);

// Dibuja lineas entre pares de indices: 'edges' es un arreglo de edgeCount*2
// ints; cada par {i,j} traza el segmento vertices[i] -> vertices[j].
void dibujarAristas(const float* vertices, int vertexCount, const int* edges,
                    int edgeCount, const float color[3],
                    const float model[16], float lineWidth = 1.0f);

// Dibuja una polilinea de vertexCount puntos (lineWidth por defecto 1).
// 'cerrada' == true cierra el loop (GL_LINE_LOOP).
void dibujarPolilinea(const float* vertices, int vertexCount, bool cerrada,
                      const float color[3], const float model[16],
                      float lineWidth = 1.0f);

// Fallback legacy de un Modelos3D: reproduce exactamente el dibujado
// glBegin/glEnd previo (transform local con el orden translate/scale/rotate
// del stack, material o color via API del backend, malla con normales). Se usa
// cuando MeshRenderer no puede (shader/VAO ausente o malla sin normales).
// Debe llamarse con las matrices de vista/proyeccion ya cargadas.
void dibujarModeloLegacy(Modelos3D* modelo);

} // namespace ImmediateRenderer

#endif