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