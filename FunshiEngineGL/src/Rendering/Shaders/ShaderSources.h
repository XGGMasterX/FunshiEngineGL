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

// Atributos: 0 = posicion (vec3), 1 = normal (vec3), 2 = uv (vec2),
// 3 = tangente (vec3), 4 = bitangente (vec3). Las UVs alimentan vUv y el muestreo
// de texturas; tangente/bitangente arman el marco TBN para el normal mapping
// cuando el Material define una mapa de normales.
static const char* const kDefaultVertexShader = R"(#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUv;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;

out vec3 vNormalWorld;
out vec3 vWorldPos;
out vec2 vUv;
out mat3 vTbn;

void main() {
    vec4 world = uModel * vec4(aPosition, 1.0);
    vWorldPos = world.xyz;
    vNormalWorld = uNormalMatrix * aNormal;
    vUv = aUv;

    // Marco TBN en espacio mundo: la tangente se re-ortogonaliza contra la
    // normal (Gram-Schmidt) y la bitangente compensa la orientacion (handedness)
    // con el signo del producto mixto para que las caras espejadas no se
    // quiebren. Sin normal map el shader no lo usa (todo es muestreo).
    vec3 t = normalize(uNormalMatrix * aTangent);
    vec3 n = normalize(vNormalWorld);
    t = normalize(t - dot(t, n) * n);
    vec3 b = cross(n, t);
    if (dot(cross(n, t), uNormalMatrix * aBitangent) < 0.0) b = -b;
    vTbn = mat3(t, b, n);

    gl_Position = uProjection * uView * world;
}
)";

static const char* const kDefaultFragmentShader = R"(#version 330 core
in vec3 vNormalWorld;
in vec3 vWorldPos;
in vec2 vUv;
in mat3 vTbn;
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

// Texturas opcionales del Material. Con su flag en 0 el material se colorea
// solo con los uniforms; cuando la textura esta, su texel multiplica
// (MODULA) al canal correspondiente, replicando el modo GL_MODULATE de la GL
// antigua. La normal map perturba la normal en espacio tangente (vTbn).
uniform sampler2D uDiffuseTex;
uniform int uUseTexture;
uniform sampler2D uSpecularTex;
uniform int uUseSpecularMap;
uniform sampler2D uEmissionTex;
uniform int uUseEmissionMap;
uniform sampler2D uNormalTex;
uniform int uUseNormalMap;

vec3 normalizarSeguro(vec3 v) {
    float len = length(v);
    return (len < 1e-8) ? vec3(0.0) : v / len;
}

vec3 contribucionLuz(int i, vec3 N, vec3 V, vec3 fragPos, vec3 baseDiffuse,
                     vec3 baseSpecular) {
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
    vec3 difuso = ndotl * uLightDiffuse[i] * baseDiffuse;

    vec3 especular = vec3(0.0);
    if (ndotl > 0.0 && uMaterialShininess > 0.0) {
        vec3 H = normalizarSeguro(L + V);
        float ndoth = max(dot(N, H), 0.0);
        especular = pow(ndoth, uMaterialShininess) *
                    uLightSpecular[i] * baseSpecular;
    }

    resultado += (difuso + especular) * (atenuacion * factorSpot);
    return resultado;
}

void main() {
    // Normal del fragmento: con normal map se perturba en espacio tangente
    // (vTbn); sin el, es la normal interpolada de la geometria.
    vec3 N = normalizarSeguro(vNormalWorld);
    if (uUseNormalMap == 1) {
        vec3 tn = normalize(texture(uNormalTex, vUv).rgb * 2.0 - 1.0);
        N = normalizarSeguro(vTbn * tn);
    }
    vec3 V = normalizarSeguro(uCameraPosition - vWorldPos);

    // El diffuse modulado que ven las luces: texel de textura por diffuse del
    // material (GL_MODULATE). Sin textura (o con la malla sin UVs) queda igual.
    vec3 baseDiffuse = uMaterialDiffuse.rgb;
    if (uUseTexture == 1) baseDiffuse *= texture(uDiffuseTex, vUv).rgb;
    vec3 baseSpecular = uMaterialSpecular.rgb;
    if (uUseSpecularMap == 1) baseSpecular *= texture(uSpecularTex, vUv).rgb;

    // GL_LIGHT_MODEL_AMBIENT * material ambiente + emision, una sola vez.
    vec3 color = uGlobalAmbient * uMaterialAmbient.rgb;
    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (i >= uLightCount) break;
        color += contribucionLuz(i, N, V, vWorldPos, baseDiffuse, baseSpecular);
    }
    vec3 emision = uMaterialEmission.rgb;
    if (uUseEmissionMap == 1) emision *= texture(uEmissionTex, vUv).rgb;
    color += emision;

    FragColor = vec4(color, uMaterialDiffuse.a);
}
)";

#endif