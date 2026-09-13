#include "Model.h"

#include <cstring>
#include <iostream>

using namespace std;

void Model::setPath(string path) {
#if defined(_WIN32)
    strncpy_s(this->filePath, sizeof(this->filePath), path.c_str(), _TRUNCATE);
#elif defined(__linux__)
    strncpy(this->filePath, path.c_str(), sizeof(this->filePath) - 1);
    this->filePath[sizeof(this->filePath) - 1] = '\0';
#endif
}

void Model::serializeComponent(std::ofstream* fileNamePathContentObject) {
    size_t len = strnlen(filePath, sizeof(filePath));
    fileNamePathContentObject->write(reinterpret_cast<const char*>(&len),
                                     sizeof(size_t));
    if (len > 0) {
        fileNamePathContentObject->write(filePath, len);
    } else {
        std::cout << "No hay un path en el Objeto para almacenarlo" << std::endl;
    }
}

void Model::deserializeComponent(std::ifstream* fileNamePathContentObject) {
    size_t len = 0;
    fileNamePathContentObject->read(reinterpret_cast<char*>(&len),
                                    sizeof(size_t));

    if (len > 0) {
        char buffer[100] = { 0 };
        size_t toRead = std::min(len, static_cast<size_t>(sizeof(buffer) - 1));
        fileNamePathContentObject->read(buffer, toRead);
        buffer[toRead] = '\0';

#if defined(_WIN32)
        strncpy_s(filePath, sizeof(filePath), buffer, _TRUNCATE);
#elif defined(__linux__)
        strncpy(this->filePath, buffer, sizeof(this->filePath) - 1);
        this->filePath[sizeof(this->filePath) - 1] = '\0';
#endif
        std::cout << filePath << std::endl;
        setPath(filePath); // reconstruye buffers
    } else {
        filePath[0] = '\0';
        std::cout << "No hay un path en el binario" << std::endl;
    }
}

void Model::saveComponent(std::ofstream* fileNamePathContentObject) {
    serializeComponent(fileNamePathContentObject);
}

void Model::loadComponent(std::ifstream* fileNamePathContentObject) {
    deserializeComponent(fileNamePathContentObject);
}