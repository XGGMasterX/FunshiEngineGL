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

#include "../GLCompat.h"
#include "../Assets/Mesh.h"
#include "../Objetos/Componentes/Color.h"
#include "../Objetos/Componentes/Material.h"
#include "../Objetos/Componentes/Model.h"
#include "../Objetos/Componentes/Transform.h"
#include "../Objetos/Modelos3D.h"

namespace ImmediateRenderer {

// Aplica la matriz modelo, apaga la iluminacion para lineas de color solido
// y restaurar GL_LIGHTING al terminar (mismo ciclo que el codigo inmediato
// original: disable -> draw -> enable -> pop).
static void prepararLineas(const float model[16], float lineWidth) {
    glPushMatrix();
    glMultMatrixf(model);
    glDisable(GL_LIGHTING);
    if (lineWidth != 1.0f) glLineWidth(lineWidth);
}

static void finalizarLineas(float lineWidth) {
    if (lineWidth != 1.0f) glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void dibujarSegmentos(const float* vertices, int vertexCount,
                      const float color[3], const float model[16],
                      float lineWidth) {
    if (!vertices || vertexCount < 2) return;
    prepararLineas(model, lineWidth);
    glColor3f(color[0], color[1], color[2]);
    glBegin(GL_LINES);
    for (int i = 0; i + 1 < vertexCount; i += 2) {
        glVertex3fv(vertices + 3 * i);
        glVertex3fv(vertices + 3 * (i + 1));
    }
    glEnd();
    finalizarLineas(lineWidth);
}

void dibujarAristas(const float* vertices, int vertexCount, const int* edges,
                    int edgeCount, const float color[3],
                    const float model[16], float lineWidth) {
    if (!vertices || vertexCount < 1 || !edges || edgeCount < 1) return;
    prepararLineas(model, lineWidth);
    glColor3f(color[0], color[1], color[2]);
    glBegin(GL_LINES);
    for (int i = 0; i < edgeCount; ++i) {
        const int i0 = edges[2 * i];
        const int i1 = edges[2 * i + 1];
        if (i0 < 0 || i0 >= vertexCount || i1 < 0 || i1 >= vertexCount)
            continue;
        glVertex3fv(vertices + 3 * i0);
        glVertex3fv(vertices + 3 * i1);
    }
    glEnd();
    finalizarLineas(lineWidth);
}

void dibujarPolilinea(const float* vertices, int vertexCount, bool cerrada,
                      const float color[3], const float model[16],
                      float lineWidth) {
    if (!vertices || vertexCount < 2) return;
    prepararLineas(model, lineWidth);
    glColor3f(color[0], color[1], color[2]);
    glBegin(cerrada ? GL_LINE_LOOP : GL_LINE_STRIP);
    for (int i = 0; i < vertexCount; ++i) glVertex3fv(vertices + 3 * i);
    glEnd();
    finalizarLineas(lineWidth);
}

void dibujarModeloLegacy(Modelos3D* modelo) {
    if (!modelo) return;

    // El fallback legacy respeta el pipeline de compatibilidad tal y como lo
    // hacia el viejo Modelos3D::dibujar: transform local con el orden
    // translate/scale/rotate del stack, material (o color, o blanco) via
    // glMaterialfv y malla con normales. Con GL_LIGHTING activa porque la
    // malla se dibuja con normales y material.
    const bool empujado = modelo->getGlobalTransform() != nullptr;
    Transform* transform = modelo->getGlobalTransform();
    if (empujado) {
        glPushMatrix();
        glTranslatef(transform->getTranslatef()[0], transform->getTranslatef()[1],
                     transform->getTranslatef()[2]);
        glScalef(transform->getScalef()[0], transform->getScalef()[1],
                 transform->getScalef()[2]);
        glRotatef(transform->getRotatef()[0], transform->getRotatef()[1],
                  transform->getRotatef()[2], transform->getRotatef()[3]);
    }

    if (Model* model = modelo->getComponent<Model>();
        model && model->getPath() != modelo->getPath())
        modelo->setPath(model->getPath());

    if (Material* material = modelo->getComponent<Material>()) {
        glMaterialfv(GL_FRONT, GL_AMBIENT, material->getAmbient());
        glMaterialfv(GL_FRONT, GL_DIFFUSE, material->getDiffuse());
        glMaterialfv(GL_FRONT, GL_SPECULAR, material->getSpecular());
        glMaterialfv(GL_FRONT, GL_EMISSION, material->getEmission());
        glMaterialf(GL_FRONT, GL_SHININESS, material->getShininess());
    } else if (Color* color = modelo->getComponent<Color>()) {
        glMaterialfv(GL_FRONT, GL_DIFFUSE, color->getColor());
    } else {
        const GLfloat white[] = {1.f, 1.f, 1.f, 1.f};
        glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (const Mesh* mesh = modelo->getMesh()) {
        const bool dibujaNormales = mesh->hasNormals();
        const std::vector<vec3>& normals = mesh->normals;
        glBegin(GL_TRIANGLES);
        for (unsigned int idx : mesh->indices) {
            if (idx >= mesh->vertices.size()) continue;
            if (dibujaNormales) {
                const vec3& normal = normals[idx];
                if (!(normal.x == 0.f && normal.y == 0.f && normal.z == 0.f))
                    glNormal3fv(&normal.x);
            }
            glVertex3fv(&mesh->vertices[idx].x);
        }
        glEnd();
    }

    if (empujado) glPopMatrix();
}

} // namespace ImmediateRenderer