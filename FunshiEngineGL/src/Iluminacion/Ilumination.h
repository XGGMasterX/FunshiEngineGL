#ifndef ILLUMINATION_H
#define ILLUMINATION_H

#include <GL/gl.h>

struct Vec3 {
    float x, y, z;
    Vec3(float X=0, float Y=0, float Z=0) : x(X), y(Y), z(Z) {}
};

enum class LightType {
    DIRECTIONAL,
    POINT,
    SPOT
};

class Ilumination {
private:
    LightType type;
    Vec3 position;
    Vec3 direction;
    Vec3 ambient;
    Vec3 diffuse;
    Vec3 specular;
    float constant;
    float linear;
    float quadratic;
    float spotCutOff;

    GLenum lightID; // GL_LIGHT0, GL_LIGHT1, etc.

public:
    Ilumination(LightType t = LightType::DIRECTIONAL, GLenum id = GL_LIGHT0);
    void setPosition(float x, float y, float z);
    void setDirection(float x, float y, float z);
    void setAmbient(float r, float g, float b);
    void setDiffuse(float r, float g, float b);
    void setSpecular(float r, float g, float b);
    void setAttenuation(float c, float l, float q);
    void setSpotCutOff(float angle);
    void apply();
};

#endif
