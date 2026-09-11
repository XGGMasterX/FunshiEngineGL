#include "Binario.h"

#include <cstdio>
#include <utility>

Binario::Binario(std::string path)
    : path(std::move(path)), ofBin(nullptr), ifBin(nullptr) {}

Binario::~Binario() {
    if (ofBin != nullptr) {
        if (ofBin->is_open()) ofBin->close();
        delete ofBin;
    }
    if (ifBin != nullptr) {
        if (ifBin->is_open()) ifBin->close();
        delete ifBin;
    }
}

std::string Binario::getPath() { return path; }

void Binario::ofOpenBinary() {
    std::remove(path.c_str());
    ofBin = new std::ofstream(path, std::ios::binary);
}

void Binario::ifOpenBinary() {
    ifBin = new std::ifstream(path, std::ios::binary);
}

void Binario::ofCloseBinary() { if (ofBin) ofBin->close(); }
void Binario::ifCloseBinary() { if (ifBin) ifBin->close(); }
std::ofstream* Binario::getOfBinariFile() { return ofBin; }
std::ifstream* Binario::getIfBinariFile() { return ifBin; }
