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
#ifndef SHADERSOURCES_H
#define SHADERSOURCES_H

// Shader por defecto del renderer moderno: Blinn-Phong en espacio mundo que
// replica el modelo de iluminacion fixed-function que ya usa el motor
// (LightSystem + Material), para que el pasaje a VBO/VAO no sea una regresion
// visual. El renderer sube luces/material/camara como uniforms; la grilla y
// los gizmos siguen siendo modo inmediato.

// Atributos: 0 = posicion (vec3), 1 = normal (vec3), 2 = uv (vec2, reservado
// para la rama de texturas).
static const char* const kDefaultVertexShader = R"(#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUv;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;

out vec3 vNormalWorld;
out vec3 vWorldPos;

void main() {
    vec4 world = uModel * vec4(aPosition, 1.0);
    vWorldPos = world.xyz;
    vNormalWorld = uNormalMatrix * aNormal;
    gl_Position = uProjection * uView * world;
}
)";

static const char* const kDefaultFragmentShader = R"(#version 330 core
in vec3 vNormalWorld;
in vec3 vWorldPos;
out vec4 FragColor;

uniform vec3 uCameraPosition;
uniform vec3 uGlobalAmbient;
uniform int uLightCount;
const int MAX_LIGHTS = 8;
uniform int uLightTypes[MAX_LIGHTS];       // 0 direccional, 1 punto, 2 spot
uniform vec3 uLightWorldPos[MAX_LIGHTS];   // dir para direccional, pos punto/spot
uniform vec3 uLightDir[MAX_LIGHTS];        // direccion forward de cara
uniform vec3 uLightAmbient[MAX_LIGHTS];
uniform vec3 uLightDiffuse[MAX_LIGHTS];
uniform vec3 uLightSpecular[MAX_LIGHTS];
uniform float uLightConstant[MAX_LIGHTS];
uniform float uLightLinear[MAX_LIGHTS];
uniform float uLightQuadratic[MAX_LIGHTS];
uniform float uSpotCutoffCos[MAX_LIGHTS];

uniform vec4 uMaterialAmbient;
uniform vec4 uMaterialDiffuse;
uniform vec4 uMaterialSpecular;
uniform vec4 uMaterialEmission;
uniform float uMaterialShininess;

vec3 normalizarSeguro(vec3 v) {
    float len = length(v);
    return (len < 1e-8) ? vec3(0.0) : v / len;
}

vec3 contribucionLuz(int i, vec3 N, vec3 V, vec3 fragPos) {
    int type = uLightTypes[i];

    vec3 L;
    float atenuacion = 1.0;
    if (type == 0) {
        L = -normalizarSeguro(uLightWorldPos[i]);
    } else {
        vec3 toLight = uLightWorldPos[i] - fragPos;
        float dist = length(toLight);
        if (dist < 1e-5) return uLightAmbient[i] * uMaterialAmbient.rgb;
        L = toLight / dist;
        atenuacion = 1.0 / max(1e-5,
            uLightConstant[i] + uLightLinear[i] * dist +
            uLightQuadratic[i] * dist * dist);
    }

    // El ambiente de cada luz NO se atenua con la distancia (GL: matAmbient *
    // lightAmbient, sin factor). El diffuse/specular si.
    vec3 resultado = uLightAmbient[i] * uMaterialAmbient.rgb;

    float factorSpot = 1.0;
    if (type == 2) {
        float cosAngulo = dot(-L, normalizarSeguro(uLightDir[i]));
        if (cosAngulo < uSpotCutoffCos[i]) {
            return resultado; // fuera del cono: solo ambiente
        }
        factorSpot = cosAngulo; // GL_SPOT_EXPONENT = 1 -> pow(cos, 1)
    }

    float ndotl = max(dot(N, L), 0.0);
    vec3 difuso = ndotl * uLightDiffuse[i] * uMaterialDiffuse.rgb;

    vec3 especular = vec3(0.0);
    if (ndotl > 0.0 && uMaterialShininess > 0.0) {
        vec3 H = normalizarSeguro(L + V);
        float ndoth = max(dot(N, H), 0.0);
        especular = pow(ndoth, uMaterialShininess) *
                    uLightSpecular[i] * uMaterialSpecular.rgb;
    }

    resultado += (difuso + especular) * (atenuacion * factorSpot);
    return resultado;
}

void main() {
    vec3 N = normalizarSeguro(vNormalWorld);
    vec3 V = normalizarSeguro(uCameraPosition - vWorldPos);

    // GL_LIGHT_MODEL_AMBIENT * material ambiente + emision, una sola vez.
    vec3 color = uGlobalAmbient * uMaterialAmbient.rgb;
    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (i >= uLightCount) break;
        color += contribucionLuz(i, N, V, vWorldPos);
    }
    color += uMaterialEmission.rgb;

    FragColor = vec4(color, uMaterialDiffuse.a);
}
)";

#endif