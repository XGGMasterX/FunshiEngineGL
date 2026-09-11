#include "Carpeta.h"

#include <utility>

Carpeta::Carpeta(std::string pathName) : File(std::move(pathName)), open(false) {}
bool Carpeta::isOpen() { return open; }
void Carpeta::setStateOpenOrClose(bool value) { open = value; }
