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
#include "TextureGL.h"

#include <cstddef>

#include <GL/gl.h>

TextureGL::~TextureGL() { destroy(); }

void TextureGL::destroy() {
    if (texture_) {
        // glDeleteTextures es GL 1.1 y esta en gl.h (como el resto del upload).
        glDeleteTextures(1, &texture_);
        texture_ = 0;
    }
    width_ = height_ = 0;
}

TextureGL::TextureGL(TextureGL&& other) noexcept {
    texture_ = other.texture_;
    width_ = other.width_;
    height_ = other.height_;
    other.texture_ = 0;
    other.width_ = other.height_ = 0;
}

TextureGL& TextureGL::operator=(TextureGL&& other) noexcept {
    if (this != &other) {
        destroy();
        texture_ = other.texture_;
        width_ = other.width_;
        height_ = other.height_;
        other.texture_ = 0;
        other.width_ = other.height_ = 0;
    }
    return *this;
}

void TextureGL::upload(const Image& image) {
    if (image.isEmpty() || image.width <= 0 || image.height <= 0) return;

    destroy();

    // glGenTextures/glBindTexture/glTexParameteri/glTexImage2D son GL 1.1,
    // estan en gl.h de cualquier plataforma (a diferencia de glActiveTexture).
    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width, image.height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image.pixels.data());

    glBindTexture(GL_TEXTURE_2D, 0);

    width_ = image.width;
    height_ = image.height;
}

void TextureGL::bindUnit(int unit) const {
    if (!texture_ || !GLFuncs::pfnActiveTexture) return;
    GLFuncs::pfnActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + unit));
    glBindTexture(GL_TEXTURE_2D, texture_);
}