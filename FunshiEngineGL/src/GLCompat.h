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
#ifndef GLCOMPAT_H
#define GLCOMPAT_H

// Punto unico de inclusion de las cabeceras OpenGL legacy (gl.h/glu.h) junto
// con GLFW, homogeneo en Linux, macOS y Windows.
//
// En Windows el gl.h/glu.h del Windows SDK son los "antiguos" de OpenGL 1.1:
//  - dependen de windows.h para compilar (WINGDIAPI, APIENTRY, CALLBACK);
//  - no declaran constantes posteriores a GL 1.1 (GL_CLAMP_TO_EDGE).
// Ademas glfw3.h, si no se le indica lo contrario, incluye gl.h/glu.h por su
// cuenta (GLFW_INCLUDE_NONE), arrastrando esos problemas de nuevo. Por eso
// aquí se incluye windows.h primero, se agregan las constantes que faltan y
// solo después se trae GLFW.

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#define GLFW_INCLUDE_NONE

#include <GL/gl.h>
#include <GL/glu.h>

// El gl.h legacy de Windows es OpenGL 1.1: no define GL_CLAMP_TO_EDGE (GL
// 1.2+), ni los tipos y constantes de GL 1.5/2.0/3.0 que el motor usa para los
// shaders (GL_VERTEX_SHADER, GL_ARRAY_BUFFER...) y FBO (GL_FRAMEBUFFER...).
// En Linux/macOS esas definiciones ya las trae gl.h/glext; en Windows se
// agregan aquí con guards para no pisar cabeceras que sí las declaren.
#ifdef _WIN32
#include <cstddef>
#ifndef GL_VERSION_1_5
typedef std::ptrdiff_t GLintptr;
typedef std::ptrdiff_t GLsizeiptr;
#endif
#ifndef GL_VERSION_2_0
typedef char GLchar;
#endif

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif
#ifndef GL_TEXTURE0
#define GL_TEXTURE0 0x84C0
#endif
#ifndef GL_ARRAY_BUFFER
#define GL_ARRAY_BUFFER 0x8892
#endif
#ifndef GL_ELEMENT_ARRAY_BUFFER
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#endif
#ifndef GL_STATIC_DRAW
#define GL_STATIC_DRAW 0x88E4
#endif
#ifndef GL_VERTEX_SHADER
#define GL_VERTEX_SHADER 0x8B31
#endif
#ifndef GL_FRAGMENT_SHADER
#define GL_FRAGMENT_SHADER 0x8B30
#endif
#ifndef GL_COMPILE_STATUS
#define GL_COMPILE_STATUS 0x8B81
#endif
#ifndef GL_LINK_STATUS
#define GL_LINK_STATUS 0x8B82
#endif
#ifndef GL_INFO_LOG_LENGTH
#define GL_INFO_LOG_LENGTH 0x8B84
#endif
#ifndef GL_RGBA8
#define GL_RGBA8 0x8058
#endif
#ifndef GL_RENDERBUFFER
#define GL_RENDERBUFFER 0x8D41
#endif
#ifndef GL_FRAMEBUFFER
#define GL_FRAMEBUFFER 0x8D40
#endif
#ifndef GL_COLOR_ATTACHMENT0
#define GL_COLOR_ATTACHMENT0 0x8CE0
#endif
#ifndef GL_DEPTH_ATTACHMENT
#define GL_DEPTH_ATTACHMENT 0x8D00
#endif
#ifndef GL_FRAMEBUFFER_COMPLETE
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#endif
#ifndef GL_DEPTH_COMPONENT24
#define GL_DEPTH_COMPONENT24 0x81A6
#endif
#endif

#include <GLFW/glfw3.h>

#endif