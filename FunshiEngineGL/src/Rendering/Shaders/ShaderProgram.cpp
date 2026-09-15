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
#include "ShaderProgram.h"

#include <glm/gtc/type_ptr.hpp>

#include "ShaderException.h"

namespace {

// Lee el log del shader/programa sin reservar mas de lo que el driver reporta.
std::string infoLog(GLuint object, void (*getiv)(GLuint, GLenum, GLint*),
                    void (*getInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*)) {
    GLint length = 0;
    getiv(object, GL_INFO_LOG_LENGTH, &length);
    if (length <= 1) return {};
    std::string log(static_cast<size_t>(length), '\0');
    GLsizei written = 0;
    getInfoLog(object, length, &written, log.data());
    log.resize(static_cast<size_t>(written));
    return log;
}

} // namespace

GLuint ShaderProgram::compilarEtapa(GLenum stage, const char* source) {
    const GLuint id = GLFuncs::pfnCreateShader(stage);
    if (!id) throw ShaderUnavailableException();

    GLFuncs::pfnShaderSource(id, 1, &source, nullptr);
    GLFuncs::pfnCompileShader(id);

    GLint estado = GL_FALSE;
    GLFuncs::pfnGetShaderiv(id, GL_COMPILE_STATUS, &estado);
    if (estado == GL_FALSE) {
        const std::string log = infoLog(id, GLFuncs::pfnGetShaderiv,
                                        GLFuncs::pfnGetShaderInfoLog);
        GLFuncs::pfnDeleteShader(id);
        throw ShaderCompileException(
            stage == GL_VERTEX_SHADER ? "vertex" : "fragment", log);
    }
    return id;
}

ShaderProgram ShaderProgram::fromSource(const char* vertexSource,
                                        const char* fragmentSource) {
    if (!GLFuncs::available()) throw ShaderUnavailableException();

    ShaderProgram program;
    const GLuint vs = compilarEtapa(GL_VERTEX_SHADER, vertexSource);
    const GLuint fs = compilarEtapa(GL_FRAGMENT_SHADER, fragmentSource);

    program.program_ = GLFuncs::pfnCreateProgram();
    if (!program.program_) {
        GLFuncs::pfnDeleteShader(vs);
        GLFuncs::pfnDeleteShader(fs);
        throw ShaderUnavailableException();
    }

    GLFuncs::pfnAttachShader(program.program_, vs);
    GLFuncs::pfnAttachShader(program.program_, fs);
    GLFuncs::pfnLinkProgram(program.program_);

    GLFuncs::pfnDeleteShader(vs);
    GLFuncs::pfnDeleteShader(fs);

    GLint estado = GL_FALSE;
    GLFuncs::pfnGetProgramiv(program.program_, GL_LINK_STATUS, &estado);
    if (estado == GL_FALSE) {
        const std::string log = infoLog(program.program_,
                                        GLFuncs::pfnGetProgramiv,
                                        GLFuncs::pfnGetProgramInfoLog);
        GLFuncs::pfnDeleteProgram(program.program_);
        program.program_ = 0;
        throw ShaderLinkException(log);
    }
    return program;
}

ShaderProgram::~ShaderProgram() {
    if (program_ && GLFuncs::pfnDeleteProgram)
        GLFuncs::pfnDeleteProgram(program_);
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept
    : program_(other.program_) {
    other.program_ = 0;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept {
    if (this != &other) {
        if (program_ && GLFuncs::pfnDeleteProgram)
            GLFuncs::pfnDeleteProgram(program_);
        program_ = other.program_;
        other.program_ = 0;
        uniformLocations_.clear();
    }
    return *this;
}

void ShaderProgram::use() const {
    if (program_) GLFuncs::pfnUseProgram(program_);
}

void ShaderProgram::unbind() {
    GLFuncs::pfnUseProgram(0);
}

GLint ShaderProgram::findUniform(const char* name) const {
    auto it = uniformLocations_.find(name);
    if (it != uniformLocations_.end()) return it->second;

    GLint location = program_ ? GLFuncs::pfnGetUniformLocation(program_, name)
                              : -1;
    uniformLocations_.emplace(name, location);
    return location;
}

void ShaderProgram::setMat4(const char* name, const glm::mat4& value) {
    const GLint loc = findUniform(name);
    if (loc < 0) return;
    GLFuncs::pfnUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::setMat3(const char* name, const glm::mat3& value) {
    const GLint loc = findUniform(name);
    if (loc < 0) return;
    GLFuncs::pfnUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::setVec3(const char* name, const glm::vec3& value) {
    const GLint loc = findUniform(name);
    if (loc < 0) return;
    GLFuncs::pfnUniform3fv(loc, 1, glm::value_ptr(value));
}

void ShaderProgram::setVec4(const char* name, const float* value) {
    const GLint loc = findUniform(name);
    if (loc < 0) return;
    GLFuncs::pfnUniform4fv(loc, 1, value);
}

void ShaderProgram::setFloat(const char* name, float value) {
    const GLint loc = findUniform(name);
    if (loc < 0) return;
    GLFuncs::pfnUniform1f(loc, value);
}

void ShaderProgram::setInt(const char* name, int value) {
    const GLint loc = findUniform(name);
    if (loc < 0) return;
    GLFuncs::pfnUniform1i(loc, value);
}