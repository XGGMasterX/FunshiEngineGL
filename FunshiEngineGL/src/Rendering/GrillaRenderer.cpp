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

#include "../GLCompat.h"

void GrillaRenderer::recompilarGrilla(const float colorGrilla[3]) {
    color_[0] = colorGrilla[0];
    color_[1] = colorGrilla[1];
    color_[2] = colorGrilla[2];

    const float tam = tam_;
    const float sep = sep_;
    if (tam <= 0.f || sep <= 0.f) return;

    // Lineas principales (cada 5 unidades): color mas intenso
    if (listasMajor_ != 0) {
        glDeleteLists(listasMajor_, 1);
        listasMajor_ = 0;
    }
    GLuint listMajor = glGenLists(1);
    if (listMajor != 0) {
        glNewList(listMajor, GL_COMPILE);
        glColor3f(colorGrilla[0] * 0.7f, colorGrilla[1] * 0.7f,
                  colorGrilla[2] * 0.7f);
        glBegin(GL_LINES);
        const float majorStep = 5.0f;
        for (float i = -tam; i <= tam; i += majorStep) {
            if (std::fabs(i) < 0.001f) continue;  // Saltar el origen
            glVertex3f(i, 0.f, -tam);
            glVertex3f(i, 0.f, tam);
            glVertex3f(-tam, 0.f, i);
            glVertex3f(tam, 0.f, i);
        }
        glEnd();
        glEndList();
        listasMajor_ = listMajor;
    }

    // Lineas secundarias (cada sep unidades)
    if (listasMinor_ != 0) {
        glDeleteLists(listasMinor_, 1);
        listasMinor_ = 0;
    }
    GLuint listMinor = glGenLists(1);
    if (listMinor != 0) {
        glNewList(listMinor, GL_COMPILE);
        glColor3fv(colorGrilla);
        glBegin(GL_LINES);
        const float majorStep = 5.0f;
        for (float i = -tam; i <= tam; i += sep) {
            if (std::fabs(std::fmod(i, majorStep)) < 0.001f) continue;
            glVertex3f(i, 0.f, -tam);
            glVertex3f(i, 0.f, tam);
            glVertex3f(-tam, 0.f, i);
            glVertex3f(tam, 0.f, i);
        }
        glEnd();
        glEndList();
        listasMinor_ = listMinor;
    }

    // Ejes X y Z: colores brillantes (rojo y verde)
    if (listasAxes_ != 0) {
        glDeleteLists(listasAxes_, 1);
        listasAxes_ = 0;
    }
    GLuint listAxes = glGenLists(1);
    if (listAxes != 0) {
        glNewList(listAxes, GL_COMPILE);
        glColor3f(1.0f, 0.3f, 0.3f);  // Rojo para X
        glBegin(GL_LINES);
        glVertex3f(0.f, 0.f, 0.f);
        glVertex3f(tam * 0.8f, 0.f, 0.f);
        glEnd();
        glColor3f(0.3f, 1.0f, 0.3f);  // Verde para Z
        glBegin(GL_LINES);
        glVertex3f(0.f, 0.f, 0.f);
        glVertex3f(0.f, 0.f, tam * 0.8f);
        glEnd();
        glEndList();
        listasAxes_ = listAxes;
    }
}

void GrillaRenderer::dibujar(const float model[16], const float colorGrilla[3],
                             float tam, float sep) {
    if (!model || !colorGrilla) return;
    if (tam <= 0.f || sep <= 0.f) return;

    // Recompila las listas solo si cambian tamano, separacion o color efectivo;
    // mientras nada cambie, la geometria se reutiliza (glCallList).
    if (listasMajor_ == 0 || tam_ != tam || sep_ != sep ||
        color_[0] != colorGrilla[0] || color_[1] != colorGrilla[1] ||
        color_[2] != colorGrilla[2]) {
        tam_ = tam;
        sep_ = sep;
        recompilarGrilla(colorGrilla);
    }

    glPushMatrix();
    glMultMatrixf(model);
    glDisable(GL_LIGHTING);
    // Lineas suavizadas para todas las partes de la grilla
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    // Dibujar lineas secundarias (1px)
    glLineWidth(1.f);
    if (listasMinor_ != 0) glCallList(listasMinor_);

    // Dibujar lineas principales (2px, color mas intenso)
    glLineWidth(2.f);
    if (listasMajor_ != 0) glCallList(listasMajor_);

    // Dibujar ejes (3px, colores brillantes)
    glLineWidth(3.f);
    if (listasAxes_ != 0) glCallList(listasAxes_);

    glLineWidth(1.f);  // Restaurar ancho por defecto
    glDisable(GL_LINE_SMOOTH);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void GrillaRenderer::destruir() {
    if (listasMajor_ != 0) {
        glDeleteLists(listasMajor_, 1);
        listasMajor_ = 0;
    }
    if (listasMinor_ != 0) {
        glDeleteLists(listasMinor_, 1);
        listasMinor_ = 0;
    }
    if (listasAxes_ != 0) {
        glDeleteLists(listasAxes_, 1);
        listasAxes_ = 0;
    }
}