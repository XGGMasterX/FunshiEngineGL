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
#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

#include "../GLFuncs.h"

// Programa de shaders RAII: compila/linkea y borra el handle GL en el
// destructor. La factory fromSource() lanza ShaderCompileException /
// ShaderLinkException / ShaderUnavailableException si el pipeline moderno no
// esta disponible o el shader falla. Los setters de uniforms cachean la
// location (glGetUniformLocation es caro y los shaders optimizados devuelven
// -1 en uniforms no usadas, que se ignoran sin ruido).
class ShaderProgram {
private:
    GLuint program_ = 0;
    mutable std::unordered_map<std::string, GLint> uniformLocations_;

    static GLuint compilarEtapa(GLenum stage, const char* source);
    GLint findUniform(const char* name) const;

    ShaderProgram() = default;

public:
    ~ShaderProgram();
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;
    ShaderProgram(ShaderProgram&& other) noexcept;
    ShaderProgram& operator=(ShaderProgram&& other) noexcept;

    static ShaderProgram fromSource(const char* vertexSource,
                                    const char* fragmentSource);

    GLuint getProgramId() const { return program_; }
    bool isValid() const { return program_ != 0; }

    void use() const;
    static void unbind();

    void setMat4(const char* name, const glm::mat4& value);
    void setMat3(const char* name, const glm::mat3& value);
    void setVec3(const char* name, const glm::vec3& value);
    void setVec4(const char* name, const float* value);
    void setFloat(const char* name, float value);
    void setInt(const char* name, int value);
};

#endif