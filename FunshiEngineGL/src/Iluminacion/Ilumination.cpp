#include "Ilumination.h"

Ilumination::Ilumination(LightType t, GLenum id)
    : type(t), position(0,0,0), direction(0,-1,0),
      ambient(0.1f,0.1f,0.1f), diffuse(1,1,1), specular(1,1,1),
      constant(1), linear(0.09f), quadratic(0.032f), spotCutOff(45), lightID(id) {}
void Ilumination::setPosition(float x,float y,float z) { position=Vec3(x,y,z); }
void Ilumination::setDirection(float x,float y,float z) { direction=Vec3(x,y,z); }
void Ilumination::setAmbient(float r,float g,float b) { ambient=Vec3(r,g,b); }
void Ilumination::setDiffuse(float r,float g,float b) { diffuse=Vec3(r,g,b); }
void Ilumination::setSpecular(float r,float g,float b) { specular=Vec3(r,g,b); }
void Ilumination::setAttenuation(float c,float l,float q) { constant=c; linear=l; quadratic=q; }
void Ilumination::setSpotCutOff(float angle) { spotCutOff=angle; }

void Ilumination::apply() {
    glEnable(GL_LIGHTING);
    glEnable(lightID);
    GLfloat pos[4] = {position.x,position.y,position.z,type==LightType::DIRECTIONAL?0.f:1.f};
    GLfloat amb[4] = {ambient.x,ambient.y,ambient.z,1.f};
    GLfloat diff[4] = {diffuse.x,diffuse.y,diffuse.z,1.f};
    GLfloat spec[4] = {specular.x,specular.y,specular.z,1.f};
    glLightfv(lightID,GL_POSITION,pos); glLightfv(lightID,GL_AMBIENT,amb);
    glLightfv(lightID,GL_DIFFUSE,diff); glLightfv(lightID,GL_SPECULAR,spec);
    if (type==LightType::POINT || type==LightType::SPOT) {
        glLightf(lightID,GL_CONSTANT_ATTENUATION,constant);
        glLightf(lightID,GL_LINEAR_ATTENUATION,linear);
        glLightf(lightID,GL_QUADRATIC_ATTENUATION,quadratic);
    }
    if (type==LightType::SPOT) {
        GLfloat dir[3]={direction.x,direction.y,direction.z};
        glLightfv(lightID,GL_SPOT_DIRECTION,dir);
        glLightf(lightID,GL_SPOT_CUTOFF,spotCutOff);
        glLightf(lightID,GL_SPOT_EXPONENT,1.f);
    }
}
