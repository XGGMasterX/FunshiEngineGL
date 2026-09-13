#include "Material.h"

#include <GL/gl.h>
#include <iostream>

Material::Material() {
    ambient[0] = 0.25f; ambient[1] = 0.25f; ambient[2] = 0.25f; ambient[3] = 1.f;
    diffuse[0] = 1.f; diffuse[1] = 1.f; diffuse[2] = 1.f; diffuse[3] = 1.f;
    specular[0] = 0.5f; specular[1] = 0.5f; specular[2] = 0.5f; specular[3] = 1.f;
    emission[0] = 0.f; emission[1] = 0.f; emission[2] = 0.f; emission[3] = 1.f;
    shininess = 32.f;
}

void Material::serializeComponent(std::ofstream* file) {
    if (!file || !file->is_open()) return;
    file->write(reinterpret_cast<const char*>(ambient), sizeof(ambient));
    file->write(reinterpret_cast<const char*>(diffuse), sizeof(diffuse));
    file->write(reinterpret_cast<const char*>(specular), sizeof(specular));
    file->write(reinterpret_cast<const char*>(emission), sizeof(emission));
    file->write(reinterpret_cast<const char*>(&shininess), sizeof(float));
}

void Material::deserializeComponent(std::ifstream* file) {
    if (!file || !file->is_open()) {
        std::cerr << "Error: archivo invalido o no abierto para lectura (Material).\n";
        return;
    }
    file->read(reinterpret_cast<char*>(ambient), sizeof(ambient));
    file->read(reinterpret_cast<char*>(diffuse), sizeof(diffuse));
    file->read(reinterpret_cast<char*>(specular), sizeof(specular));
    file->read(reinterpret_cast<char*>(emission), sizeof(emission));
    file->read(reinterpret_cast<char*>(&shininess), sizeof(float));
}

void Material::saveComponent(std::ofstream* file) { serializeComponent(file); }

void Material::loadComponent(std::ifstream* file) { deserializeComponent(file); }

void Material::setAmbient(float r, float g, float b) {
    ambient[0] = r; ambient[1] = g; ambient[2] = b; ambient[3] = 1.f;
}
void Material::setDiffuse(float r, float g, float b) {
    diffuse[0] = r; diffuse[1] = g; diffuse[2] = b; diffuse[3] = 1.f;
}
void Material::setSpecular(float r, float g, float b) {
    specular[0] = r; specular[1] = g; specular[2] = b; specular[3] = 1.f;
}
void Material::setEmission(float r, float g, float b) {
    emission[0] = r; emission[1] = g; emission[2] = b; emission[3] = 1.f;
}
void Material::setShininess(float value) { shininess = value; }

void Material::aplicar() {
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}