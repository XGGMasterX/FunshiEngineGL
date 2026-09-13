#ifndef MATERIAL_H
#define MATERIAL_H
#include "Component.h"

// Componente de material: datos del modelo de iluminacion de un objeto
// (AMBIENT/DIFFUSE/SPECULAR/EMISSION/SHININESS). Solo data + aplicar();
// el dibujado real lo decide Modelos3D::dibujar al consultar el componente.
class Material : public Component {
private:
    float ambient[4];
    float diffuse[4];
    float specular[4];
    float emission[4];
    float shininess;

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

    // Aplica el material al pipeline GL del objeto actual.
    void aplicar();
};
#endif