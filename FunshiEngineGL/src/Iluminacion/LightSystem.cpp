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
#include "LightSystem.h"

#include <GL/gl.h>
#include <cmath>

#include "../Estructuras/ListasEnlazadas/ListasDoblementeEnlazada/ListaDE.h"
#include "../Objetos/Componentes/Light.h"
#include "../Objetos/Componentes/Transform.h"
#include "../Objetos/GameObject.h"


namespace {

// Rota el vector "adelante" (0,0,-1) segun el Transform del objeto (glRotate:
// angulo + eje). El resultado es la direccion de la luz desacoplada de la
// orientacion del "Gizmo" que la representa.
void rotarAdelante(float anguloGrados, const float* eje, float out[3]) {
    const float v[3] = {0.f, 0.f, -1.f};
    out[0] = v[0]; out[1] = v[1]; out[2] = v[2];

    if (anguloGrados == 0.f) return;

    float k[3] = {eje[0], eje[1], eje[2]};
    const float norma =
        std::sqrt(k[0] * k[0] + k[1] * k[1] + k[2] * k[2]);
    if (norma < 1e-6f) return;

    k[0] /= norma; k[1] /= norma; k[2] /= norma;

    const float radianes = anguloGrados * 3.14159265358979f / 180.f;
    const float c = std::cos(radianes);
    const float s = std::sin(radianes);
    const float dot =
        v[0] * k[0] + v[1] * k[1] + v[2] * k[2];

    const float cross[3] = {
        k[1] * v[2] - k[2] * v[1],
        k[2] * v[0] - k[0] * v[2],
        k[0] * v[1] - k[1] * v[0]};

    out[0] = v[0] * c + cross[0] * s + k[0] * dot * (1.f - c);
    out[1] = v[1] * c + cross[1] * s + k[1] * dot * (1.f - c);
    out[2] = v[2] * c + cross[2] * s + k[2] * dot * (1.f - c);
}

// Parametriza un slot GL con los datos del componente Light y el Transform
// global del objeto que lo lleva.
void aplicarLightComponent(GLenum lightID, Light* light, Transform* transform) {
    const LightType type = light->getType();

    float px = 0.f, py = 0.f, pz = 0.f;
    float dir[3] = {0.f, 0.f, -1.f};

    if (transform) {
        px = transform->getTranslatef()[0];
        py = transform->getTranslatef()[1];
        pz = transform->getTranslatef()[2];

        if (type != LightType::POINT)
            rotarAdelante(transform->getRotatef()[0],
                          &transform->getRotatef()[1], dir);
    }

    const bool direccional = (type == LightType::DIRECTIONAL);

    const GLfloat pos[4] = {
        direccional ? dir[0] : px,
        direccional ? dir[1] : py,
        direccional ? dir[2] : pz,
        direccional ? 0.f : 1.f};
    const GLfloat amb[4] = {
        light->getAmbient()[0], light->getAmbient()[1], light->getAmbient()[2], 1.f};
    const GLfloat diff[4] = {
        light->getDiffuse()[0], light->getDiffuse()[1], light->getDiffuse()[2], 1.f};
    const GLfloat spec[4] = {
        light->getSpecular()[0], light->getSpecular()[1], light->getSpecular()[2], 1.f};

    glEnable(lightID);
    glLightfv(lightID, GL_POSITION, pos);
    glLightfv(lightID, GL_AMBIENT, amb);
    glLightfv(lightID, GL_DIFFUSE, diff);
    glLightfv(lightID, GL_SPECULAR, spec);

    if (type == LightType::POINT || type == LightType::SPOT) {
        glLightf(lightID, GL_CONSTANT_ATTENUATION, light->getConstant());
        glLightf(lightID, GL_LINEAR_ATTENUATION, light->getLinear());
        glLightf(lightID, GL_QUADRATIC_ATTENUATION, light->getQuadratic());
    }
    if (type == LightType::SPOT) {
        const GLfloat spotDir[3] = {dir[0], dir[1], dir[2]};
        glLightfv(lightID, GL_SPOT_DIRECTION, spotDir);
        glLightf(lightID, GL_SPOT_CUTOFF, light->getSpotCutOff());
        glLightf(lightID, GL_SPOT_EXPONENT, 1.f);
    }
}

} // namespace


LightSystem::LightSystem() = default;

void LightSystem::setGlobalAmbient(float r, float g, float b) {
    globalAmbient_[0] = r;
    globalAmbient_[1] = g;
    globalAmbient_[2] = b;
    globalAmbient_[3] = 1.f;
}

void LightSystem::beginFrame(ListaDE<GameObject*>* objects) {
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient_);

    // Apagar todos los slots garantiza que luces removidas no sigan activas:
    // el sistema es dueno del estado de luz, no vive sobre estado heredado.
    for (int i = 0; i < 8; ++i) glDisable(GL_LIGHT0 + i);

    /*
     * Escanear la escena y asignar un slot a cada componente Light
     * encontrado. El orden de recorrido es estable (orden del arbol),
     * asi que cada luz conserva su slot entre frames.
     */
    int slot = 0;

    if (objects && !objects->isEmpty()) {
        Position<GameObject*>* pos = objects->first();
        while (pos && pos->getElement() && slot < 8) {
            GameObject* object = pos->getElement();

            if (Light* light = object->getComponent<Light>()) {
                aplicarLightComponent(
                    static_cast<GLenum>(GL_LIGHT0 + slot),
                    light,
                    object->getGlobalTransform());
                ++slot;
            }

            pos = (pos != objects->last()) ? objects->next(pos) : nullptr;
        }
    }
}