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
#include "ImmediateRenderer.h"

#include "../Assets/Mesh.h"
#include "Backend/IRenderBackend.h"
#include "../Objetos/Componentes/Color.h"
#include "../Objetos/Componentes/Material.h"
#include "../Objetos/Componentes/Model.h"
#include "../Objetos/Componentes/Transform.h"
#include "../Objetos/Modelos3D.h"

namespace ImmediateRenderer {

using RenderUI = Rendering::Backend::IRenderBackend;

// Aplica la matriz modelo, apaga la iluminacion para lineas de color solido
// y restaurar la iluminacion al terminar (mismo ciclo que el codigo inmediato
// original: disable -> draw -> enable -> pop). El estado del ancho de linea es
// responsabilidad del backend (no se guarda en call lists/comandos).
static void prepararLineas(const float model[16], float lineWidth) {
    RenderUI& b = Rendering::Backend::activeBackend();
    b.pushMatrix();
    b.multMatrix(model);
    b.setLightingEnabled(false);
    if (lineWidth != 1.0f) b.setLineWidth(lineWidth);
}

static void finalizarLineas(float lineWidth) {
    RenderUI& b = Rendering::Backend::activeBackend();
    if (lineWidth != 1.0f) b.setLineWidth(1.0f);
    b.setLightingEnabled(true);
    b.popMatrix();
}

void dibujarSegmentos(const float* vertices, int vertexCount,
                      const float color[3], const float model[16],
                      float lineWidth) {
    if (!vertices || vertexCount < 2) return;
    RenderUI& b = Rendering::Backend::activeBackend();
    prepararLineas(model, lineWidth);
    b.setSolidColor(color[0], color[1], color[2]);
    b.drawLinePairs(vertices, vertexCount);
    finalizarLineas(lineWidth);
}

void dibujarAristas(const float* vertices, int vertexCount, const int* edges,
                    int edgeCount, const float color[3],
                    const float model[16], float lineWidth) {
    if (!vertices || vertexCount < 1 || !edges || edgeCount < 1) return;
    RenderUI& b = Rendering::Backend::activeBackend();
    prepararLineas(model, lineWidth);
    b.setSolidColor(color[0], color[1], color[2]);
    b.drawIndexedLines(vertices, vertexCount, edges, edgeCount);
    finalizarLineas(lineWidth);
}

void dibujarPolilinea(const float* vertices, int vertexCount, bool cerrada,
                      const float color[3], const float model[16],
                      float lineWidth) {
    if (!vertices || vertexCount < 2) return;
    RenderUI& b = Rendering::Backend::activeBackend();
    prepararLineas(model, lineWidth);
    b.setSolidColor(color[0], color[1], color[2]);
    b.drawLineStrip(vertices, vertexCount, cerrada);
    finalizarLineas(lineWidth);
}

void dibujarModeloLegacy(Modelos3D* modelo) {
    if (!modelo) return;

    RenderUI& b = Rendering::Backend::activeBackend();

    // El fallback legacy respeta el pipeline de compatibilidad tal y como lo
    // hacia el viejo Modelos3D::dibujar: transform local con el orden
    // translate/scale/rotate del stack, material (o color, o blanco) via la
    // API del backend y malla con normales. Con la iluminacion activa porque
    // la malla se dibuja con normales y material.
    const bool empujado = modelo->getGlobalTransform() != nullptr;
    Transform* transform = modelo->getGlobalTransform();
    if (empujado) {
        b.pushMatrix();
        b.applyTransform(transform->getTranslatef(), transform->getScalef(),
                         transform->getRotatef());
    }

    if (Model* model = modelo->getComponent<Model>();
        model && model->getPath() != modelo->getPath())
        modelo->setPath(model->getPath());

    if (Material* material = modelo->getComponent<Material>()) {
        b.setMaterial(material->getAmbient(), material->getDiffuse(),
                      material->getSpecular(), material->getEmission(),
                      material->getShininess());
    } else if (Color* color = modelo->getComponent<Color>()) {
        // El pipeline inmediato solo seteaba GL_DIFFUSE al color; el resto
        // quedaba con el default GL (ambiente gris tenue, sin especular).
        const float ambient[4] = {0.2f, 0.2f, 0.2f, 1.f};
        const float specular[4] = {0.f, 0.f, 0.f, 1.f};
        const float emission[4] = {0.f, 0.f, 0.f, 1.f};
        b.setMaterial(ambient, color->getColor(), specular, emission, 0.f);
    } else {
        const float white[4] = {1.f, 1.f, 1.f, 1.f};
        const float ambient[4] = {0.2f, 0.2f, 0.2f, 1.f};
        const float specular[4] = {0.f, 0.f, 0.f, 1.f};
        const float emission[4] = {0.f, 0.f, 0.f, 1.f};
        b.setMaterial(ambient, white, specular, emission, 0.f);
    }

    b.setPolygonFill();
    if (const Mesh* mesh = modelo->getMesh()) {
        if (mesh->hasNormals()) {
            b.drawTriangles(
                reinterpret_cast<const float*>(mesh->vertices.data()),
                static_cast<int>(mesh->vertices.size()),
                reinterpret_cast<const float*>(mesh->normals.data()),
                static_cast<int>(mesh->normals.size()),
                mesh->indices.data(), static_cast<int>(mesh->indices.size()));
        } else {
            // Malla sin normales: misma degradacion que el codigo original
            // (GL_NORMAL se quedaba en {0,0,0} y la cara salia apagada).
            b.drawTriangles(
                reinterpret_cast<const float*>(mesh->vertices.data()),
                static_cast<int>(mesh->vertices.size()), nullptr, 0,
                mesh->indices.data(), static_cast<int>(mesh->indices.size()));
        }
    }

    if (empujado) b.popMatrix();
}

} // namespace ImmediateRenderer