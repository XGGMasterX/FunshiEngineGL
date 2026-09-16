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
#include "Transform.h"

#include <cmath>
#include "../../GLCompat.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

Transform::Transform() {
    // Inicializar arrays auxiliares
    for (int i = 0; i < 3; i++) {
        arrTranslatef[i] = objectTranslatef[i];
        arrScalef[i] = objectScalef[i];
    }
    for (int i = 0; i < 4; i++) {
        arrRotatef[i] = objectRotatef[i];
    }
}

void Transform::serializeComponent(std::ofstream* fileNamePathContentObject) {
    fileNamePathContentObject->write(
        reinterpret_cast<const char*>(&objectTranslatef), sizeof(float) * 3);
    fileNamePathContentObject->write(
        reinterpret_cast<const char*>(&objectScalef), sizeof(float) * 3);
    fileNamePathContentObject->write(
        reinterpret_cast<const char*>(&objectRotatef), sizeof(float) * 4);
}

void Transform::deserializeComponent(std::ifstream* fileNamePathContentObject) {
    fileNamePathContentObject->read(
        reinterpret_cast<char*>(&objectTranslatef), sizeof(float) * 3);
    fileNamePathContentObject->read(reinterpret_cast<char*>(&objectScalef),
                                    sizeof(float) * 3);
    fileNamePathContentObject->read(reinterpret_cast<char*>(&objectRotatef),
                                    sizeof(float) * 4);

    // Actualizar los arrays auxiliares
    for (int i = 0; i < 3; i++) {
        arrTranslatef[i] = objectTranslatef[i];
        arrScalef[i] = objectScalef[i];
    }
    for (int i = 0; i < 4; i++) {
        arrRotatef[i] = objectRotatef[i];
    }
}

void Transform::setTranslatef(float x, float y, float z) {
    objectTranslatef[0] = x;
    objectTranslatef[1] = y;
    objectTranslatef[2] = z;

    // Actualizar el array auxiliar
    for (int i = 0; i < 3; i++) {
        arrTranslatef[i] = objectTranslatef[i];
    }
}

void Transform::setRotatef(float angle, float x, float y, float z) {
    objectRotatef[0] = angle;
    objectRotatef[1] = x;
    objectRotatef[2] = y;
    objectRotatef[3] = z;

    // Actualizar el array auxiliar
    for (int i = 0; i < 4; i++) {
        arrRotatef[i] = objectRotatef[i];
    }
}

void Transform::setScalef(float x, float y, float z) {
    objectScalef[0] = x;
    objectScalef[1] = y;
    objectScalef[2] = z;

    // Actualizar el array auxiliar
    for (int i = 0; i < 3; i++) {
        arrScalef[i] = objectScalef[i];
    }
}

void Transform::saveComponent(std::ofstream* fileNamePathContentObject) {
    serializeComponent(fileNamePathContentObject);
}

void Transform::loadComponent(std::ifstream* fileNamePathContentObject) {
    deserializeComponent(fileNamePathContentObject);
}

void Transform::position() {
    glPushMatrix();
    // Reposicionamiento del Objeto En Cuestion
    glTranslatef(getTranslatef()[0], getTranslatef()[1], getTranslatef()[2]);

    // Escalacion De Objeto En Cuestion
    glScalef(getScalef()[0], getScalef()[1], getScalef()[2]);

    // Rotacion de Objeto En Cuestion
    glRotatef(getRotatef()[0], getRotatef()[1], getRotatef()[2],
              getRotatef()[3]);
}

void buildMatrixFromTransform(Transform* t, float outMatrix[16]) {
    glm::mat4 mat(1.0f);
    mat = glm::translate(
        mat, glm::vec3(t->getTranslatef()[0], t->getTranslatef()[1],
                       t->getTranslatef()[2]));

    float angle = t->getRotatef()[0];
    glm::vec3 axis(t->getRotatef()[1], t->getRotatef()[2], t->getRotatef()[3]);
    if (glm::length(axis) > 0.0001f)
        mat = glm::rotate(mat, glm::radians(angle), glm::normalize(axis));

    mat = glm::scale(
        mat, glm::vec3(t->getScalef()[0], t->getScalef()[1], t->getScalef()[2]));

    const float* ptr = glm::value_ptr(mat);
    for (int i = 0; i < 16; i++) outMatrix[i] = ptr[i];
}

void decomposeMatrixToTransform(float inMatrix[16], Transform* t) {
    glm::mat4 mat = glm::make_mat4(inMatrix);

    // Defensa contra NaN/Inf: si la matriz de entrada tiene valores no
    // finitos (p.ej. por glm::inverse de un padre singular, o un drag del
    // gizmo sobre datos corruptos), glm::decompose los propaga y el objeto
    // queda en -nan -> desaparece de la escena. Ante eso se ABORTA sin tocar
    // el transform (se conserva el valor anterior en vez de corromperlo).
    const float* M = inMatrix;
    for (int i = 0; i < 16; ++i) {
        if (!std::isfinite(M[i])) return;
    }

    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(mat, scale, rotation, translation, skew, perspective);

    // Cualquier componente no finito en el resultado: abortar sin tocar.
    if (!std::isfinite(translation.x) || !std::isfinite(translation.y) ||
        !std::isfinite(translation.z) || !std::isfinite(scale.x) ||
        !std::isfinite(scale.y) || !std::isfinite(scale.z))
        return;

    t->setTranslatef(translation.x, translation.y, translation.z);

    rotation = glm::normalize(rotation);
    float angle = glm::degrees(glm::angle(rotation));
    glm::vec3 axis = glm::axis(rotation);
    if (std::isnan(angle) || !std::isfinite(angle) ||
        glm::length(axis) < 0.0001f || !std::isfinite(axis.x) ||
        !std::isfinite(axis.y) || !std::isfinite(axis.z)) {
        angle = 0.0f;
        axis = glm::vec3(0, 1, 0);
    }
    t->setRotatef(angle, axis.x, axis.y, axis.z);

    // No se admite escala no finita: 0 (ejes aplanados) y NaN/Inf corrompen
    // toda la cadena de matrices posterior.
    if (std::isfinite(scale.x) && std::isfinite(scale.y) &&
        std::isfinite(scale.z))
        t->setScalef(scale.x, scale.y, scale.z);
}