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
#include "Mesh.h"

#include <algorithm>

bool Mesh::computeBounds(vec3& outMin, vec3& outMax) const {
    if (vertices.empty()) return false;
    outMin = vertices.front();
    outMax = vertices.front();
    for (size_t i = 1; i < vertices.size(); ++i) {
        const vec3& v = vertices[i];
        outMin.x = std::min(outMin.x, v.x);
        outMin.y = std::min(outMin.y, v.y);
        outMin.z = std::min(outMin.z, v.z);
        outMax.x = std::max(outMax.x, v.x);
        outMax.y = std::max(outMax.y, v.y);
        outMax.z = std::max(outMax.z, v.z);
    }
    return true;
}