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
#include "MallaCollider.h"

#include <vector>
#include <btBulletDynamicsCommon.h>

#include "../../../Objetos/GameObject.h"
#include "../../../Objetos/Modelos3D.h"
#include "../../../Rendering/ImmediateRenderer.h"

MallaCollider::MallaCollider(float radio, Transform* transformOfDadObject,
                             GameObject* owner)
    : Collider(radio, transformOfDadObject, owner) {}

std::unique_ptr<btCollisionShape> MallaCollider::createCollisionShape() {
    // Convex hull de los vertices del modelo (es el colisionador geometrico
    // exacto de la malla). Sin malla cargada se cae a la shape de respaldo.
    Modelos3D* modelo = dynamic_cast<Modelos3D*>(owner);
    if (!modelo) return nullptr;

    const std::vector<vec3>& vertices = modelo->getVertices();
    if (vertices.size() < 4) return nullptr;

    auto hull = std::make_unique<btConvexHullShape>();
    for (const vec3& v : vertices) {
        hull->addPoint(btVector3(v.x, v.y, v.z));
    }
    return hull;
}

void MallaCollider::dibujarCollider() {
    Transform globalT = getGlobalTransform();
    float modelArr[16];
    buildMatrixFromTransform(&globalT, modelArr);

    const float verde[3] = {0.0f, 1.0f, 0.0f};

    // Dibujo el hull real (bordes del convex hull) en vez de la caja
    // aproximada. getCollisionShape() construye la shape de forma lazy si
    // todavia no existe; es lo que permite ver el hull apenas se crea el
    // collider, sin esperar a que la fisica lo genere.
    btConvexHullShape* hull =
        dynamic_cast<btConvexHullShape*>(getCollisionShape());
    if (hull && hull->getNumPoints() > 0) {
        const btVector3* points = hull->getUnscaledPoints();
        const int numPoints = hull->getNumPoints();
        // Pares consecutivos (i -> i+1 modulo n): poligono cerrado con la
        // misma topologia que el dibujado inmediato original.
        std::vector<float> segs(static_cast<size_t>(numPoints) * 6);
        for (int i = 0; i < numPoints; ++i) {
            const btVector3& a = points[i];
            const btVector3& b = points[(i + 1) % numPoints];
            segs[6 * i + 0] = a.x();
            segs[6 * i + 1] = a.y();
            segs[6 * i + 2] = a.z();
            segs[6 * i + 3] = b.x();
            segs[6 * i + 4] = b.y();
            segs[6 * i + 5] = b.z();
        }
        ImmediateRenderer::dibujarSegmentos(
            segs.data(), static_cast<int>(segs.size() / 3), verde, modelArr);
    } else {
        // Respaldo grafico: caja envolvente rapida al radio.
        float r = getRadio();
        const float box[8][3] = {
            {-r, -r, -r}, { r, -r, -r}, { r,  r, -r}, {-r,  r, -r},
            {-r, -r,  r}, { r, -r,  r}, { r,  r,  r}, {-r,  r,  r}
        };
        const int edges[12][2] = {
            {0,1}, {1,2}, {2,3}, {3,0},
            {4,5}, {5,6}, {6,7}, {7,4},
            {0,4}, {1,5}, {2,6}, {3,7}
        };
        ImmediateRenderer::dibujarAristas(&box[0][0], 8, &edges[0][0], 12,
                                          verde, modelArr);
    }
}