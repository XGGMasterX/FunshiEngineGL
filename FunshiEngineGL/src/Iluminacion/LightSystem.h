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
#ifndef LIGHTSYSTEM_H
#define LIGHTSYSTEM_H

// Subsistema de iluminacion: es dueno del estado GL de luces (GL_LIGHT0..
// GL_LIGHT7 y GL_LIGHT_MODEL_AMBIENT). Cada frame escanea los GameObjects de
// la escena, toma sus componentes Light y parametriza los slots hardware.
// GameScene delega en el; no vive logica de luz en el frame loop ni en los
// componentes (Light es solo data). Tambien expone los mismos datos en CPU
// (collectLights) para que el renderer moderno (shaders) los suba como
// uniforms con la MISMA semantica que el pipeline inmediato.
class GameObject;

template <typename T>
class Position;

template <typename T>
class ListaDE;

// Descripcion CPU de una luz del escenario, lista para uniforms del shader.
// Es la contraparte de aplicarLightComponent: position/direction se derivan
// del Transform del objeto dueño (como GL_LIGHT0..7), no del estado GL.
struct LightData {
    int type = 0;              // 0 direccional, 1 punto, 2 spot (LightType)
    float worldPos[3] = {0.f, 0.f, 0.f};   // dir para direccional, pos punto/spot
    float direction[3] = {0.f, 0.f, -1.f}; // forward del objeto (spot/dir)
    float ambient[3] = {0.f, 0.f, 0.f};
    float diffuse[3] = {1.f, 1.f, 1.f};
    float specular[3] = {1.f, 1.f, 1.f};
    float constant = 1.f;
    float linear = 0.f;
    float quadratic = 0.f;
    float spotCutoffDegrees = 45.f;
};

class LightSystem {
public:
    // Slots de luz disponibles (limite de GL_LIGHT0..GL_LIGHT7).
    static constexpr int kMaxLights = 8;

    LightSystem();
    ~LightSystem() = default;

    // Luz global del modelo (GL_LIGHT_MODEL_AMBIENT), default gris tenue.
    void setGlobalAmbient(float r, float g, float b);

    const float* getGlobalAmbient() const noexcept { return globalAmbient_; }

    // Dueno del estado GL de luces: habilita GL_LIGHTING, configura el modelo,
    // apaga los 8 slots y enciende/parametriza cada componente Light encontrado
    // en los objetos de la escena. Debe llamarse cada frame, antes de dibujar.
    void beginFrame(ListaDE<GameObject*>* objects);

    // Version CPU de beginFrame para el renderer con shaders: llena luces[]
    // (hasta kMaxLights) con la misma semantica que parametriza los slots GL
    // (escanen del arbol, posicion = Transform, direccional usa rotation del
    // forward). outCount queda con la cantidad de luces recogidas.
    void collectLights(ListaDE<GameObject*>* objects, LightData luces[],
                       int& outCount) const;

private:
    void aplicarLuz(ListaDE<GameObject*>* objects, int& slotsEnabled);

    float globalAmbient_[4] = {0.15f, 0.15f, 0.15f, 1.f};
};

#endif