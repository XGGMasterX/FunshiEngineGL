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

using Rendering::Backend::IRenderBackend;
using Rendering::Backend::kInvalidHandle;

ShaderProgram ShaderProgram::fromSource(const char* vertexSource,
                                        const char* fragmentSource) {
    ShaderProgram program;
    program.program_ = Rendering::Backend::activeBackend().createProgram(
        vertexSource, fragmentSource);
    return program;
}

ShaderProgram::~ShaderProgram() {
    if (program_ != kInvalidHandle)
        Rendering::Backend::activeBackend().destroyProgram(program_);
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept
    : program_(other.program_) {
    other.program_ = kInvalidHandle;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept {
    if (this != &other) {
        if (program_ != kInvalidHandle)
            Rendering::Backend::activeBackend().destroyProgram(program_);
        program_ = other.program_;
        other.program_ = kInvalidHandle;
        uniformLocations_.clear();
    }
    return *this;
}

void ShaderProgram::use() const {
    if (program_ != kInvalidHandle)
        Rendering::Backend::activeBackend().useProgram(program_);
}

void ShaderProgram::unbind() {
    Rendering::Backend::activeBackend().useProgram(kInvalidHandle);
}

int ShaderProgram::findUniform(const char* name) const {
    auto it = uniformLocations_.find(name);
    if (it != uniformLocations_.end()) return it->second;

    int location = program_ != kInvalidHandle
                       ? Rendering::Backend::activeBackend().uniformLocation(
                             program_, name)
                       : -1;
    uniformLocations_.emplace(name, location);
    return location;
}

void ShaderProgram::setMat4(const char* name, const glm::mat4& value) {
    const int loc = findUniform(name);
    if (loc < 0) return;
    Rendering::Backend::activeBackend().setUniformMat4(loc, value);
}

void ShaderProgram::setMat3(const char* name, const glm::mat3& value) {
    const int loc = findUniform(name);
    if (loc < 0) return;
    Rendering::Backend::activeBackend().setUniformMat3(loc, value);
}

void ShaderProgram::setVec3(const char* name, const glm::vec3& value) {
    const int loc = findUniform(name);
    if (loc < 0) return;
    Rendering::Backend::activeBackend().setUniformVec3(loc, value);
}

void ShaderProgram::setVec4(const char* name, const float* value) {
    const int loc = findUniform(name);
    if (loc < 0) return;
    Rendering::Backend::activeBackend().setUniformVec4(loc, value);
}

void ShaderProgram::setFloat(const char* name, float value) {
    const int loc = findUniform(name);
    if (loc < 0) return;
    Rendering::Backend::activeBackend().setUniformFloat(loc, value);
}

void ShaderProgram::setInt(const char* name, int value) {
    const int loc = findUniform(name);
    if (loc < 0) return;
    Rendering::Backend::activeBackend().setUniformInt(loc, value);
}