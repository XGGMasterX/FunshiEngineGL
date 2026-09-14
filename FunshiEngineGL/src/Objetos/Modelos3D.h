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
#ifndef MODELOS3D_H
#define MODELOS3D_H

#include <string>
#include <vector>

#include "../Matematicas/StructVec3.h"
#include "../Objetos/GameObject.h"

class Modelos3D : public GameObject {
private:
    std::vector<vec3> vertices;
    std::vector<vec3> normals;
    std::vector<unsigned int> indices;
    char filePath[100];

    void setObject();

public:
    explicit Modelos3D(Entity* origin);
    Modelos3D();

    void setPath(std::string path);
    std::string getPath();
    void dibujar(float deltaTime) override;
    bool getBoundingBox(vec3& outMin, vec3& outMax) const;
    // Vertices en espacio local del modelo (para construir shapes de colision).
    const std::vector<vec3>& getVertices() const { return vertices; }

protected:
    void serializeEntity() override;
    void deserializeEntity() override;

public:
    void saveEntity(std::string filename) override;
    void loadEntity(std::string filename) override;
};

#endif
