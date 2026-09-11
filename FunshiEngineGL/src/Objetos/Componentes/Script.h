#ifndef SCRIPT_H
#define SCRIPT_H

#include "Component.h"
#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#endif
#include <string>

class Script : public Component{
private:
    std::string dllPath;
    std::string nameClass;
protected:
    virtual void serializeComponent(std::ofstream* fileNamePathContentObject) override {
        // 1. Serializar longitud del path + contenido
        size_t pathLength = dllPath.size();
        fileNamePathContentObject->write(reinterpret_cast<const char*>(&pathLength), sizeof(size_t));
        fileNamePathContentObject->write(dllPath.c_str(), pathLength);

        // 2. Serializar longitud del nombre de clase + contenido
        size_t nameLength = nameClass.size();
        fileNamePathContentObject->write(reinterpret_cast<const char*>(&nameLength), sizeof(size_t));
        fileNamePathContentObject->write(nameClass.c_str(), nameLength);
    }

    virtual void deserializeComponent(std::ifstream* fileNamePathContentObject) override {
        // 1. Deserializar path
        size_t pathLength = 0;
        fileNamePathContentObject->read(reinterpret_cast<char*>(&pathLength), sizeof(size_t));
        dllPath.resize(pathLength);
        fileNamePathContentObject->read(&dllPath[0], pathLength);

        // 2. Deserializar nombre de clase
        size_t nameLength = 0;
        fileNamePathContentObject->read(reinterpret_cast<char*>(&nameLength), sizeof(size_t));
        nameClass.resize(nameLength);
        fileNamePathContentObject->read(&nameClass[0], nameLength);
    }

public:
    void setDllPath(std::string dllPath) {
            this->dllPath = dllPath; // Guarda el path completo

            // Extraer nombre de clase (entre el último '/' y '.')
            size_t lastSlash = dllPath.find_last_of("/\\");
            size_t lastDot = dllPath.find_last_of('.');

            bool isValidScript = !dllPath.empty() &&
                dllPath.find(".cpp") != std::string::npos;

            if (lastSlash != std::string::npos && lastDot != std::string::npos && lastDot > lastSlash && isValidScript) {
                this->nameClass = dllPath.substr(lastSlash + 1, lastDot - lastSlash - 1);
            }
            else {
                this->nameClass = "Debe ser <ClassName>.cpp"; // O valor por defecto
            }
        
    }

    std::string getPath() {
        return dllPath;
    }

    std::string getNameClass() {
        return nameClass;
    }

    virtual void saveComponent(std::ofstream* fileNamePathContentObject) override {
        serializeComponent(fileNamePathContentObject);
    }

    virtual void loadComponent(std::ifstream* fileNamePathContentObject) override {
        deserializeComponent(fileNamePathContentObject);
    }

};
#endif
