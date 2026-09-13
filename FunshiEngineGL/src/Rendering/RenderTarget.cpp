#include "RenderTarget.h"

#include <GLFW/glfw3.h>
#include <iostream>

namespace {

// Los FBO son una extension que el gl.h del sistema no declara en todas las
// plataformas; se cargan por puntero con el mismo mecanismo que usa GLFW.
typedef void (GLAPIENTRY* FN_GenFramebuffers)(GLsizei, GLuint*);
typedef void (GLAPIENTRY* FN_DeleteFramebuffers)(GLsizei, const GLuint*);
typedef void (GLAPIENTRY* FN_BindFramebuffer)(GLenum, GLuint);
typedef void (GLAPIENTRY* FN_FramebufferTexture2D)(GLenum, GLenum, GLenum,
                                                   GLuint, GLint);
typedef void (GLAPIENTRY* FN_GenRenderbuffers)(GLsizei, GLuint*);
typedef void (GLAPIENTRY* FN_DeleteRenderbuffers)(GLsizei, const GLuint*);
typedef void (GLAPIENTRY* FN_BindRenderbuffer)(GLenum, GLuint);
typedef void (GLAPIENTRY* FN_RenderbufferStorage)(GLenum, GLenum, GLsizei,
                                                  GLsizei);
typedef void (GLAPIENTRY* FN_FramebufferRenderbuffer)(GLenum, GLenum, GLenum,
                                                      GLuint);
typedef GLenum (GLAPIENTRY* FN_CheckFramebufferStatus)(GLenum);

FN_GenFramebuffers pfnGenFramebuffers = nullptr;
FN_DeleteFramebuffers pfnDeleteFramebuffers = nullptr;
FN_BindFramebuffer pfnBindFramebuffer = nullptr;
FN_FramebufferTexture2D pfnFramebufferTexture2D = nullptr;
FN_GenRenderbuffers pfnGenRenderbuffers = nullptr;
FN_DeleteRenderbuffers pfnDeleteRenderbuffers = nullptr;
FN_BindRenderbuffer pfnBindRenderbuffer = nullptr;
FN_RenderbufferStorage pfnRenderbufferStorage = nullptr;
FN_FramebufferRenderbuffer pfnFramebufferRenderbuffer = nullptr;
FN_CheckFramebufferStatus pfnCheckFramebufferStatus = nullptr;

template <typename T>
void cargar(const char* nombre, T& destino) {
    if (!destino) destino = reinterpret_cast<T>(glfwGetProcAddress(nombre));
}

bool funcionesCargadas() {
    cargar("glGenFramebuffers", pfnGenFramebuffers);
    cargar("glDeleteFramebuffers", pfnDeleteFramebuffers);
    cargar("glBindFramebuffer", pfnBindFramebuffer);
    cargar("glFramebufferTexture2D", pfnFramebufferTexture2D);
    cargar("glGenRenderbuffers", pfnGenRenderbuffers);
    cargar("glDeleteRenderbuffers", pfnDeleteRenderbuffers);
    cargar("glBindRenderbuffer", pfnBindRenderbuffer);
    cargar("glRenderbufferStorage", pfnRenderbufferStorage);
    cargar("glFramebufferRenderbuffer", pfnFramebufferRenderbuffer);
    cargar("glCheckFramebufferStatus", pfnCheckFramebufferStatus);

    return pfnGenFramebuffers && pfnDeleteFramebuffers && pfnBindFramebuffer &&
           pfnFramebufferTexture2D && pfnGenRenderbuffers &&
           pfnDeleteRenderbuffers && pfnBindRenderbuffer &&
           pfnRenderbufferStorage && pfnFramebufferRenderbuffer &&
           pfnCheckFramebufferStatus;
}

} // namespace

RenderTarget::~RenderTarget() { destroy(); }

void RenderTarget::destroy() {
    if (!pfnDeleteFramebuffers) {
        fbo = 0;
        depthRbo = 0;
        colorTex = 0;
        width = 0;
        height = 0;
        return;
    }
    if (fbo) {
        pfnDeleteFramebuffers(1, &fbo);
        fbo = 0;
    }
    if (depthRbo) {
        pfnDeleteRenderbuffers(1, &depthRbo);
        depthRbo = 0;
    }
    if (colorTex) {
        glDeleteTextures(1, &colorTex);
        colorTex = 0;
    }
    width = 0;
    height = 0;
}

void RenderTarget::resize(int w, int h) {
    if (w <= 0 || h <= 0) return;
    if (width == w && height == h) return;
    if (!funcionesCargadas()) {
        std::cerr << "[RenderTarget] FBO no disponible en este GPU.\n";
        return;
    }

    destroy();

    pfnGenFramebuffers(1, &fbo);
    pfnGenRenderbuffers(1, &depthRbo);
    glGenTextures(1, &colorTex);

    glBindTexture(GL_TEXTURE_2D, colorTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    pfnBindRenderbuffer(GL_RENDERBUFFER, depthRbo);
    pfnRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);
    pfnBindRenderbuffer(GL_RENDERBUFFER, 0);

    pfnBindFramebuffer(GL_FRAMEBUFFER, fbo);
    pfnFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                            colorTex, 0);
    pfnFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_RENDERBUFFER, depthRbo);

    const GLenum estado = pfnCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (estado != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "[RenderTarget] Framebuffer incompleto: " << estado
                  << "\n";
    }
    pfnBindFramebuffer(GL_FRAMEBUFFER, 0);

    width = w;
    height = h;
}

void RenderTarget::bind() {
    if (!fbo || !pfnBindFramebuffer) return;
    pfnBindFramebuffer(GL_FRAMEBUFFER, fbo);
    // En contextos legacy el buffer destino del FBO puede quedar en GL_BACK y
    // entonces todos los draws se descartan (textura negra). Se fuerza el
    // attachment de color del FBO para que el render llegue a la textura.
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
}

void RenderTarget::unbind() {
    if (pfnBindFramebuffer) pfnBindFramebuffer(GL_FRAMEBUFFER, 0);
}