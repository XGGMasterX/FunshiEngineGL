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
#include "EsfereCollider.h"

#include <cmath>
#include "../../../GLCompat.h"
#include <memory>
#include <btBulletDynamicsCommon.h>

EsfereCollider::EsfereCollider(float radio, Transform* transformOfDadObject,
                               GameObject* owner)
    : Collider(radio, transformOfDadObject, owner) {}

std::unique_ptr<btCollisionShape> EsfereCollider::createCollisionShape() {
    return std::make_unique<btSphereShape>(radio);
}

void EsfereCollider::dibujarCollider() {
    Transform globalT = getGlobalTransform();

    const float PI = 3.14159265358979323846f;

    glPushMatrix();
    float modelArr[16];
    buildMatrixFromTransform(&globalT, modelArr);
    glMultMatrixf(modelArr);
    glDisable(GL_LIGHTING);

    const int meridians = 8;
    const int parallels = 4;

    glColor3f(0.0f, 1.0f, 0.0f);

    for (int m = 0; m < meridians; ++m) {
        float angle = (2.0f * PI * m) / meridians;

        glBegin(GL_LINE_STRIP);
        for (int p = 0; p <= 20; ++p) {
            float lat = PI * float(p) / 20.0f - PI / 2.0f;

            float x = radio * cosf(lat) * cosf(angle);
            float y = radio * sinf(lat);
            float z = radio * cosf(lat) * sinf(angle);

            glVertex3f(x, y, z);
        }
        glEnd();
    }

    for (int p = 1; p <= parallels; ++p) {
        float lat = PI * p / (parallels + 1) - PI / 2.0f;

        glBegin(GL_LINE_LOOP);
        for (int m = 0; m < 40; ++m) {
            float lon = 2.0f * PI * m / 40.0f;

            float x = radio * cosf(lat) * cosf(lon);
            float y = radio * sinf(lat);
            float z = radio * cosf(lat) * sinf(lon);

            glVertex3f(x, y, z);
        }
        glEnd();
    }

    glEnable(GL_LIGHTING);
    glPopMatrix();
}