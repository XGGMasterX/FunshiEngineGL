#ifndef COLOR_H
#define COLOR_H
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "Component.h"

typedef float color[4];

class Color : public Component {
private:
    float f1 = 0.0f;
    float f2 = 0.0f;
    float f3 = 0.0f;
    color range = { 0.0f, 0.0f, 0.0f, 1.0f };

protected:
    virtual void serializeComponent(std::ofstream* file) override {
        file->write(reinterpret_cast<const char*>(&f1), sizeof(float));
        file->write(reinterpret_cast<const char*>(&f2), sizeof(float));
        file->write(reinterpret_cast<const char*>(&f3), sizeof(float));
    }

    virtual void deserializeComponent(std::ifstream* file) override {
        if (!file || !file->is_open()) {
            std::cerr << "Error: archivo inválido o no abierto para lectura.\n";
            return;
        }
        file->read(reinterpret_cast<char*>(&f1), sizeof(float));
        file->read(reinterpret_cast<char*>(&f2), sizeof(float));
        file->read(reinterpret_cast<char*>(&f3), sizeof(float));

        range[0] = f1;
        range[1] = f2;
        range[2] = f3;
        range[3] = 1.0f;
    }

public:
    Color() {
        range[0] = f1;
        range[1] = f2;
        range[2] = f3;
        range[3] = 1.0f;
    }

    void saveComponent(std::ofstream* file) {
        serializeComponent(file);
    }

    void loadComponent(std::ifstream* file) {
        deserializeComponent(file);
    }

    void setColor(const color cor) {
        f1 = cor[0];
        f2 = cor[1];
        f3 = cor[2];
        range[0] = f1;
        range[1] = f2;
        range[2] = f3;
        range[3] = 1.0f;
    }

    void setColor(float r, float g, float b) {
        f1 = r;
        f2 = g;
        f3 = b;
        range[0] = f1;
        range[1] = f2;
        range[2] = f3;
        range[3] = 1.0f;
    }

    // Devuelve directamente el puntero interno al arreglo RGBA
    const float* getColor() const {
        return range;
    }
};

color rojo = { 2.0,0.0,0.0 };
color verde = { 0.0,1.0,0.0 };
color aguaMarina = { 0.0,1.0,1.0 };
color celeste = { 0.0,0.0,1.0 };
color vermelho = { 0.85, 0.12, 0.0 };
color azul = { 0.0, 0.15,0.35 };
color preto = { 0.0, 0.0, 0.0 };
color branco = { 1.0, 1.0, 1.0 };
color branco_gelo = { 0.88,0.91,0.89 };
color amarelo = { 1.0, 1.0, 0.0 };
color violeta = { 0.54, 0.17, 0.88 };
color cinza = { 0.8, 0.8, 0.8 };
color cinza_escuro = { 0.67,0.67,0.67 };
color laranja = { 1.0, 0.6, 0.2 };
#endif
