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
#ifndef GLFUNCS_H
#define GLFUNCS_H

#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <type_traits>

// Las funciones modernas (shaders + VAO/VBO) no estan declaradas en el gl.h
// del sistema en todas las plataformas. Se cargan por puntero con el mismo
// mecanismo que usa RenderTarget para los FBO ("same fashion as GLFW"): todos
// los accesos a estas funciones pasan por este namespace. Si alguna no existe
// en el driver, available() == false y el renderer degrada al modo inmediato.
namespace GLFuncs {

typedef GLuint (GLAPIENTRY* FN_CreateShader)(GLenum);
typedef void (GLAPIENTRY* FN_ShaderSource)(GLuint, GLsizei, const GLchar**,
                                           const GLint*);
typedef void (GLAPIENTRY* FN_CompileShader)(GLuint);
typedef void (GLAPIENTRY* FN_GetShaderiv)(GLuint, GLenum, GLint*);
typedef void (GLAPIENTRY* FN_GetShaderInfoLog)(GLuint, GLsizei, GLsizei*,
                                               GLchar*);
typedef void (GLAPIENTRY* FN_DeleteShader)(GLuint);
typedef GLuint (GLAPIENTRY* FN_CreateProgram)(void);
typedef void (GLAPIENTRY* FN_AttachShader)(GLuint, GLuint);
typedef void (GLAPIENTRY* FN_LinkProgram)(GLuint);
typedef void (GLAPIENTRY* FN_GetProgramiv)(GLuint, GLenum, GLint*);
typedef void (GLAPIENTRY* FN_GetProgramInfoLog)(GLuint, GLsizei, GLsizei*,
                                                GLchar*);
typedef void (GLAPIENTRY* FN_DeleteProgram)(GLuint);
typedef void (GLAPIENTRY* FN_UseProgram)(GLuint);
typedef GLint (GLAPIENTRY* FN_GetUniformLocation)(GLuint, const GLchar*);
typedef GLint (GLAPIENTRY* FN_GetAttribLocation)(GLuint, const GLchar*);
typedef void (GLAPIENTRY* FN_UniformMatrix4fv)(GLint, GLsizei, GLboolean,
                                               const GLfloat*);
typedef void (GLAPIENTRY* FN_UniformMatrix3fv)(GLint, GLsizei, GLboolean,
                                               const GLfloat*);
typedef void (GLAPIENTRY* FN_Uniform3fv)(GLint, GLsizei, const GLfloat*);
typedef void (GLAPIENTRY* FN_Uniform4fv)(GLint, GLsizei, const GLfloat*);
typedef void (GLAPIENTRY* FN_Uniform1f)(GLint, GLfloat);
typedef void (GLAPIENTRY* FN_Uniform1i)(GLint, GLint);
typedef void (GLAPIENTRY* FN_ActiveTexture)(GLenum);
typedef void (GLAPIENTRY* FN_GenVertexArrays)(GLsizei, GLuint*);
typedef void (GLAPIENTRY* FN_DeleteVertexArrays)(GLsizei, const GLuint*);
typedef void (GLAPIENTRY* FN_BindVertexArray)(GLuint);
typedef void (GLAPIENTRY* FN_EnableVertexAttribArray)(GLuint);
typedef void (GLAPIENTRY* FN_VertexAttribPointer)(GLuint, GLint, GLenum,
                                                  GLboolean, GLsizei,
                                                  const void*);
typedef void (GLAPIENTRY* FN_GenBuffers)(GLsizei, GLuint*);
typedef void (GLAPIENTRY* FN_DeleteBuffers)(GLsizei, const GLuint*);
typedef void (GLAPIENTRY* FN_BindBuffer)(GLenum, GLuint);
typedef void (GLAPIENTRY* FN_BufferData)(GLenum, GLsizeiptr, const void*,
                                         GLenum);

inline FN_CreateShader pfnCreateShader = nullptr;
inline FN_ShaderSource pfnShaderSource = nullptr;
inline FN_CompileShader pfnCompileShader = nullptr;
inline FN_GetShaderiv pfnGetShaderiv = nullptr;
inline FN_GetShaderInfoLog pfnGetShaderInfoLog = nullptr;
inline FN_DeleteShader pfnDeleteShader = nullptr;
inline FN_CreateProgram pfnCreateProgram = nullptr;
inline FN_AttachShader pfnAttachShader = nullptr;
inline FN_LinkProgram pfnLinkProgram = nullptr;
inline FN_GetProgramiv pfnGetProgramiv = nullptr;
inline FN_GetProgramInfoLog pfnGetProgramInfoLog = nullptr;
inline FN_DeleteProgram pfnDeleteProgram = nullptr;
inline FN_UseProgram pfnUseProgram = nullptr;
inline FN_GetUniformLocation pfnGetUniformLocation = nullptr;
inline FN_GetAttribLocation pfnGetAttribLocation = nullptr;
inline FN_UniformMatrix4fv pfnUniformMatrix4fv = nullptr;
inline FN_UniformMatrix3fv pfnUniformMatrix3fv = nullptr;
inline FN_Uniform3fv pfnUniform3fv = nullptr;
inline FN_Uniform4fv pfnUniform4fv = nullptr;
inline FN_Uniform1f pfnUniform1f = nullptr;
inline FN_Uniform1i pfnUniform1i = nullptr;
inline FN_ActiveTexture pfnActiveTexture = nullptr;
inline FN_GenVertexArrays pfnGenVertexArrays = nullptr;
inline FN_DeleteVertexArrays pfnDeleteVertexArrays = nullptr;
inline FN_BindVertexArray pfnBindVertexArray = nullptr;
inline FN_EnableVertexAttribArray pfnEnableVertexAttribArray = nullptr;
inline FN_VertexAttribPointer pfnVertexAttribPointer = nullptr;
inline FN_GenBuffers pfnGenBuffers = nullptr;
inline FN_DeleteBuffers pfnDeleteBuffers = nullptr;
inline FN_BindBuffer pfnBindBuffer = nullptr;
inline FN_BufferData pfnBufferData = nullptr;

template <typename T>
inline void cargarFuncion(const char* nombre, T& destino) {
    if (!destino) destino = reinterpret_cast<T>(glfwGetProcAddress(nombre));
}

inline bool init() {
    cargarFuncion("glCreateShader", pfnCreateShader);
    cargarFuncion("glShaderSource", pfnShaderSource);
    cargarFuncion("glCompileShader", pfnCompileShader);
    cargarFuncion("glGetShaderiv", pfnGetShaderiv);
    cargarFuncion("glGetShaderInfoLog", pfnGetShaderInfoLog);
    cargarFuncion("glDeleteShader", pfnDeleteShader);
    cargarFuncion("glCreateProgram", pfnCreateProgram);
    cargarFuncion("glAttachShader", pfnAttachShader);
    cargarFuncion("glLinkProgram", pfnLinkProgram);
    cargarFuncion("glGetProgramiv", pfnGetProgramiv);
    cargarFuncion("glGetProgramInfoLog", pfnGetProgramInfoLog);
    cargarFuncion("glDeleteProgram", pfnDeleteProgram);
    cargarFuncion("glUseProgram", pfnUseProgram);
    cargarFuncion("glGetUniformLocation", pfnGetUniformLocation);
    cargarFuncion("glGetAttribLocation", pfnGetAttribLocation);
    cargarFuncion("glUniformMatrix4fv", pfnUniformMatrix4fv);
    cargarFuncion("glUniformMatrix3fv", pfnUniformMatrix3fv);
    cargarFuncion("glUniform3fv", pfnUniform3fv);
    cargarFuncion("glUniform4fv", pfnUniform4fv);
    cargarFuncion("glUniform1f", pfnUniform1f);
    cargarFuncion("glUniform1i", pfnUniform1i);
    cargarFuncion("glActiveTexture", pfnActiveTexture);
    cargarFuncion("glGenVertexArrays", pfnGenVertexArrays);
    cargarFuncion("glDeleteVertexArrays", pfnDeleteVertexArrays);
    cargarFuncion("glBindVertexArray", pfnBindVertexArray);
    cargarFuncion("glEnableVertexAttribArray", pfnEnableVertexAttribArray);
    cargarFuncion("glVertexAttribPointer", pfnVertexAttribPointer);
    cargarFuncion("glGenBuffers", pfnGenBuffers);
    cargarFuncion("glDeleteBuffers", pfnDeleteBuffers);
    cargarFuncion("glBindBuffer", pfnBindBuffer);
    cargarFuncion("glBufferData", pfnBufferData);

    return pfnCreateShader && pfnShaderSource && pfnCompileShader &&
           pfnGetShaderiv && pfnGetShaderInfoLog && pfnDeleteShader &&
           pfnCreateProgram && pfnAttachShader && pfnLinkProgram &&
           pfnGetProgramiv && pfnGetProgramInfoLog && pfnDeleteProgram &&
           pfnUseProgram && pfnGetUniformLocation && pfnGetAttribLocation &&
           pfnUniformMatrix4fv && pfnUniformMatrix3fv && pfnUniform3fv &&
           pfnUniform4fv && pfnUniform1f && pfnUniform1i && pfnActiveTexture &&
           pfnGenVertexArrays &&
           pfnDeleteVertexArrays && pfnBindVertexArray &&
           pfnEnableVertexAttribArray && pfnVertexAttribPointer &&
           pfnGenBuffers && pfnDeleteBuffers && pfnBindBuffer && pfnBufferData;
}

inline bool available() { return pfnCreateShader != nullptr; }

} // namespace GLFuncs

#endif