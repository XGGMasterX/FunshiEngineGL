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
#include "Material.h"

#include <cstdint>
#include <cstring>
#include <iostream>

#include <GL/gl.h>

namespace {
// Longitud maxima de path de textura en disco. El deserializador descarta
// paths por encima (cota estilo Modelos3D) para no leer buffers gigantes de
// un archivo corrupto, omitiendo la misma cantidad de bytes para que los
// siguientes componentes sigan alineados.
const std::size_t kMaxTexturePathLength = 4096;
// Marcador binario al inicio del payload Material que las escenas viejas no
// tienen (el formato legacy es solo los 5 campos float, sin magic). Al leer,
// si los primeros 4 bytes no son este magic se rebobina y se lee el formato
// antiguo: la textura queda vacia para no romper escenas guardadas antes de
// la rama de texturas.
const char kMaterialMagic[4] = {'F', 'M', 't', 'A'};
} // namespace

Material::Material() {
    ambient[0] = 0.25f; ambient[1] = 0.25f; ambient[2] = 0.25f; ambient[3] = 1.f;
    diffuse[0] = 1.f; diffuse[1] = 1.f; diffuse[2] = 1.f; diffuse[3] = 1.f;
    specular[0] = 0.5f; specular[1] = 0.5f; specular[2] = 0.5f; specular[3] = 1.f;
    emission[0] = 0.f; emission[1] = 0.f; emission[2] = 0.f; emission[3] = 1.f;
    shininess = 32.f;
}

void Material::serializeComponent(std::ofstream* file) {
    if (!file || !file->is_open()) return;
    // Magic primero: distingue este formato del viejo (ver abajo).
    file->write(kMaterialMagic, sizeof(kMaterialMagic));
    file->write(reinterpret_cast<const char*>(ambient), sizeof(ambient));
    file->write(reinterpret_cast<const char*>(diffuse), sizeof(diffuse));
    file->write(reinterpret_cast<const char*>(specular), sizeof(specular));
    file->write(reinterpret_cast<const char*>(emission), sizeof(emission));
    file->write(reinterpret_cast<const char*>(&shininess), sizeof(float));
    std::uint32_t len = static_cast<std::uint32_t>(texturePath_.size());
    file->write(reinterpret_cast<const char*>(&len), sizeof(len));
    file->write(texturePath_.data(), static_cast<std::streamsize>(len));
}

void Material::deserializeComponent(std::ifstream* file) {
    if (!file || !file->is_open()) {
        std::cerr << "Error: archivo invalido o no abierto para lectura (Material).\n";
        return;
    }
    char magic[4];
    file->read(magic, sizeof(magic));
    const bool formatoNuevo =
        file->gcount() == static_cast<std::streamsize>(sizeof(magic)) &&
        std::memcmp(magic, kMaterialMagic, sizeof(kMaterialMagic)) == 0;
    if (!formatoNuevo) {
        // Formato legacy (escenas viejas sin textura): rebobinar los 4 bytes
        // leidos y leer los 5 campos como antes.
        file->seekg(-static_cast<std::streamoff>(sizeof(magic)),
                    std::ios::cur);
    }
    texturePath_.clear();

    file->read(reinterpret_cast<char*>(ambient), sizeof(ambient));
    file->read(reinterpret_cast<char*>(diffuse), sizeof(diffuse));
    file->read(reinterpret_cast<char*>(specular), sizeof(specular));
    file->read(reinterpret_cast<char*>(emission), sizeof(emission));
    file->read(reinterpret_cast<char*>(&shininess), sizeof(float));

    // Solo en formato nuevo hay path de textura al final del payload.
    if (formatoNuevo) {
        std::uint32_t len = 0;
        file->read(reinterpret_cast<char*>(&len), sizeof(len));
        const std::size_t pathLen = len;
        if (pathLen <= kMaxTexturePathLength) {
            texturePath_.assign(static_cast<std::size_t>(pathLen), '\0');
            if (pathLen > 0) {
                file->read(&texturePath_[0],
                           static_cast<std::streamsize>(pathLen));
            }
        } else {
            // Path demasiado largo: descartar los bytes para no desalinear a
            // los componentes siguientes y dejar la textura vacia.
            file->seekg(static_cast<std::streamoff>(pathLen), std::ios::cur);
        }
    }
}

void Material::saveComponent(std::ofstream* file) { serializeComponent(file); }

void Material::loadComponent(std::ifstream* file) { deserializeComponent(file); }

void Material::setAmbient(float r, float g, float b) {
    ambient[0] = r; ambient[1] = g; ambient[2] = b; ambient[3] = 1.f;
}
void Material::setDiffuse(float r, float g, float b) {
    diffuse[0] = r; diffuse[1] = g; diffuse[2] = b; diffuse[3] = 1.f;
}
void Material::setSpecular(float r, float g, float b) {
    specular[0] = r; specular[1] = g; specular[2] = b; specular[3] = 1.f;
}
void Material::setEmission(float r, float g, float b) {
    emission[0] = r; emission[1] = g; emission[2] = b; emission[3] = 1.f;
}
void Material::setShininess(float value) { shininess = value; }

void Material::aplicar() {
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}