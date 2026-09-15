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
#ifndef MATERIAL_H
#define MATERIAL_H
#include <string>

#include "Component.h"

// Componente de material: datos del modelo de iluminacion de un objeto
// (AMBIENT/DIFFUSE/SPECULAR/EMISSION/SHININESS) mas los paths de las texturas
// (descriptores, todos opcionales): difusa, especular, normal y emision. Solo
// data + aplicar(); el dibujado real lo decide Modelos3D::dibujar al consultar
// el componente, y el renderer moderno resuelve cada path a una imagen
// compartida via TextureManager.
class Material : public Component {
private:
    float ambient[4];
    float diffuse[4];
    float specular[4];
    float emission[4];
    float shininess;

    std::string diffuseMapPath_;
    std::string specularMapPath_;
    std::string normalMapPath_;
    std::string emissionMapPath_;

    // Lee un path de textura del stream con tope de longitud (formato v2).
    void leerPathTextura(std::ifstream* file, std::string& out);

protected:
    void serializeComponent(std::ofstream* file) override;
    void deserializeComponent(std::ifstream* file) override;

public:
    Material();

    void saveComponent(std::ofstream* file) override;
    void loadComponent(std::ifstream* file) override;

    void setAmbient(float r, float g, float b);
    void setDiffuse(float r, float g, float b);
    void setSpecular(float r, float g, float b);
    void setEmission(float r, float g, float b);
    void setShininess(float value);

    const float* getAmbient() const { return ambient; }
    const float* getDiffuse() const { return diffuse; }
    const float* getSpecular() const { return specular; }
    const float* getEmission() const { return emission; }
    float getShininess() const { return shininess; }

    // Slots de textura (descriptores): path a la imagen compartida por el
    // TextureManager. Vacio = slot sin textura.
    void setDiffuseMapPath(const std::string& path) { diffuseMapPath_ = path; }
    void setSpecularMapPath(const std::string& path) { specularMapPath_ = path; }
    void setNormalMapPath(const std::string& path) { normalMapPath_ = path; }
    void setEmissionMapPath(const std::string& path) { emissionMapPath_ = path; }

    const std::string& getDiffuseMapPath() const { return diffuseMapPath_; }
    const std::string& getSpecularMapPath() const { return specularMapPath_; }
    const std::string& getNormalMapPath() const { return normalMapPath_; }
    const std::string& getEmissionMapPath() const { return emissionMapPath_; }

    bool hasDiffuseMap() const { return !diffuseMapPath_.empty(); }
    bool hasSpecularMap() const { return !specularMapPath_.empty(); }
    bool hasNormalMap() const { return !normalMapPath_.empty(); }
    bool hasEmissionMap() const { return !emissionMapPath_.empty(); }

    // Aplica el material al pipeline GL del objeto actual.
    void aplicar();
};
#endif