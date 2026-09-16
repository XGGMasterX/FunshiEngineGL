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
#include "CubeCollider.h"

#include "../../../GLCompat.h"
#include <memory>
#include <btBulletDynamicsCommon.h>

CubeCollider::CubeCollider(float radio, Transform* transformOfDadObject,
                           GameObject* owner)
    : Collider(radio, transformOfDadObject, owner) {}

std::unique_ptr<btCollisionShape> CubeCollider::createCollisionShape() {
    // Usar el radio como la mitad de tamano (box usa half extents)
    return std::make_unique<btBoxShape>(btVector3(radio, radio, radio));
}

void CubeCollider::dibujarCollider() {
    Transform globalT = getGlobalTransform();

    glPushMatrix();
    float modelArr[16];
    buildMatrixFromTransform(&globalT, modelArr);
    glMultMatrixf(modelArr);
    glDisable(GL_LIGHTING);

    // Dibujo del cubo en origen local
    GLfloat vertices[8][3] = {
        {-radio, -radio, -radio},
        { radio, -radio, -radio},
        { radio,  radio, -radio},
        {-radio,  radio, -radio},
        {-radio, -radio,  radio},
        { radio, -radio,  radio},
        { radio,  radio,  radio},
        {-radio,  radio,  radio}
    };

    GLuint edges[12][2] = {
        {0,1}, {1,2}, {2,3}, {3,0},
        {4,5}, {5,6}, {6,7}, {7,4},
        {0,4}, {1,5}, {2,6}, {3,7}
    };

    glColor3f(0.0f, 1.0f, 0.0f); // verde

    glBegin(GL_LINES);
    for (int i = 0; i < 12; i++) {
        glVertex3fv(vertices[edges[i][0]]);
        glVertex3fv(vertices[edges[i][1]]);
    }
    glEnd();

    glEnable(GL_LIGHTING);
    glPopMatrix();
}