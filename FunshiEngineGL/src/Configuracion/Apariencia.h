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
#ifndef APARIENCIA_H
#define APARIENCIA_H

// ============================================================================
// Perfil de apariencia del editor.
//
// Es un tipo de DATOS PURO (sin ImGui/OpenGL) para que la capa de
// configuracion (EditorConfig) pueda persistirlo y la capa de presentacion
// (GUI/Tema/TemaEditor) se encargue de aplicarlo. Las funciones de
// resolucion (fondo/grilla) viven aca porque son logica de color pura y las
// necesitan tanto la escena (grilla/fondo) como la GUI, sin acoplarlas.
//
// Modo "blanco y negro": fuerza el fondo del viewport y la grilla a blanco o
// negro segun el tema (claro/oscuro). El modo normal respeta los colores
// elegidos por el usuario.
// ============================================================================

struct Apariencia {
    // Tema base de la interfaz ImGui: false = oscuro (por defecto), true = claro.
    bool temaClaro = false;
    // Modo monocromo: fondo + grilla contrastados en blanco/negro.
    bool blancoYNegro = false;
    // Color de acento de la interfaz (RGB + alpha). Azul historico de Dear
    // ImGui; el usuario puede cambiarlo para que la UI no sea siempre azul.
    float acento[4] = {0.26f, 0.59f, 0.98f, 1.0f};
    // Color de fondo de la vista 3D (glClearColor). Gris oscuro historico.
    float fondo[3] = {0.10f, 0.10f, 0.10f};

    // Restablece el perfil a los valores de fabrica.
    void restablecer() { *this = Apariencia{}; }
};

// Igualdad exacta por campos: la usan main/GUI para aplicar el estilo solo
// cuando el perfil cambia (evita reescribir el estilo de ImGui cada frame).
inline bool operator==(const Apariencia& a, const Apariencia& b) {
    for (int i = 0; i < 4; ++i)
        if (a.acento[i] != b.acento[i]) return false;
    for (int i = 0; i < 3; ++i)
        if (a.fondo[i] != b.fondo[i]) return false;
    return a.temaClaro == b.temaClaro && a.blancoYNegro == b.blancoYNegro;
}
inline bool operator!=(const Apariencia& a, const Apariencia& b) {
    return !(a == b);
}

namespace AparienciaUtil {

// Luminancia perceptual (Rec. 709): convierte un color a escala de grises.
inline float luminancia(float r, float g, float b) {
    return 0.2126f * r + 0.7152f * g + 0.0722f * b;
}

inline void aGris(const float in[3], float out[3]) {
    const float v = luminancia(in[0], in[1], in[2]);
    out[0] = out[1] = out[2] = v;
}

// Color de fondo efectivo del viewport 3D. En modo B/N se fuerza a blanco si
// el tema es claro o a negro si es oscuro (contrasta con la grilla).
inline void fondoEfectivo(const Apariencia& ap, float out[3]) {
    if (ap.blancoYNegro) {
        const float v = ap.temaClaro ? 1.0f : 0.0f;
        out[0] = out[1] = out[2] = v;
        return;
    }
    out[0] = ap.fondo[0];
    out[1] = ap.fondo[1];
    out[2] = ap.fondo[2];
}

// Color de la grilla segun el perfil: en modo B/N se ignora el color del
// componente Grid y se usa el opuesto al fondo para que siempre se vea.
inline void grillaEfectiva(const Apariencia& ap, const float componente[3],
                           float out[3]) {
    if (ap.blancoYNegro) {
        const float v = ap.temaClaro ? 0.0f : 1.0f;
        out[0] = out[1] = out[2] = v;
        return;
    }
    out[0] = componente[0];
    out[1] = componente[1];
    out[2] = componente[2];
}

} // namespace AparienciaUtil

#endif
