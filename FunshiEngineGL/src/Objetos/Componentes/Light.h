#ifndef LIGHT_H
#define LIGHT_H
#include "Component.h"

// Luz como Component: se adjunta a cualquier GameObject (incluso a un objeto
// vacío sin cuerpo, el "Gizmo"). La posición/dirección de la luz se deriva del
// Transform del objeto dueño, así que moviendo el objeto se mueve la luz.
enum class LightType {
    DIRECTIONAL,
    POINT,
    SPOT
};

class Light : public Component {
private:
    LightType type = LightType::DIRECTIONAL;
    float ambient[3] = {0.1f, 0.1f, 0.1f};
    float diffuse[3] = {1.f, 1.f, 1.f};
    float specular[3] = {1.f, 1.f, 1.f};
    float constant = 1.f;
    float linear = 0.09f;
    float quadratic = 0.032f;
    float spotCutOff = 45.f;

protected:
    void serializeComponent(std::ofstream* file) override;
    void deserializeComponent(std::ifstream* file) override;

public:
    Light();

    void saveComponent(std::ofstream* file) override;
    void loadComponent(std::ifstream* file) override;

    LightType getType() const { return type; }
    void setType(LightType value) { type = value; }

    const float* getAmbient() const { return ambient; }
    const float* getDiffuse() const { return diffuse; }
    const float* getSpecular() const { return specular; }

    void setAmbient(float r, float g, float b);
    void setDiffuse(float r, float g, float b);
    void setSpecular(float r, float g, float b);

    float getConstant() const { return constant; }
    float getLinear() const { return linear; }
    float getQuadratic() const { return quadratic; }
    void setAttenuation(float c, float l, float q);

    float getSpotCutOff() const { return spotCutOff; }
    void setSpotCutOff(float angle);
};
#endif