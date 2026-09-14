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
#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include <GL/gl.h>

// Render-to-texture (FBO) para las vistas previas de camara (Fase 2).
// Carga las funciones de framebuffer por puntero via glfwGetProcAddress, asi
// funciona igual en Linux y Windows sin depender de GLAD/glew externo.
class RenderTarget {
private:
    GLuint fbo = 0;
    GLuint colorTex = 0;
    GLuint depthRbo = 0;
    int width = 0;
    int height = 0;

    void destroy();

public:
    RenderTarget() = default;
    ~RenderTarget();

    RenderTarget(const RenderTarget&) = delete;
    RenderTarget& operator=(const RenderTarget&) = delete;

    void resize(int w, int h);
    void bind();
    static void unbind();

    GLuint getColorTexture() const { return colorTex; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
};
#endif