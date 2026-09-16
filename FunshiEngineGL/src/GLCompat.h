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

// El gl.h legacy de Windows no define GL_CLAMP_TO_EDGE (GL 1.2+).
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

#include <GLFW/glfw3.h>

#endif