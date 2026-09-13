#ifndef ICONOSGUI_H
#define ICONOSGUI_H

#include <string>
#include <imgui.h>

#if defined(_WIN32)
#include <glfw3.h>
#elif defined(__linux__)
#include <GLFW/glfw3.h>
#endif

class IconosGUI {
public:
    IconosGUI();
    ~IconosGUI();

    void init();

    bool estaInicializado() const { return inicializado; }

    ImTextureID getIconoCarpeta() const { return iconoCarpeta; }
    ImTextureID getIconoArchivo() const { return iconoArchivo; }
    ImTextureID getIconoCpp() const { return iconoCpp; }
    ImTextureID getIconoHpp() const { return iconoHpp; }
    ImTextureID getIconoGameObject() const { return iconoGameObject; }
    ImTextureID getIconoPorExtension(const std::string& extension) const;

private:
    ImTextureID cargarPNG(const char* nombrePNG);

    ImTextureID iconoCarpeta = ImTextureID_Invalid;
    ImTextureID iconoArchivo = ImTextureID_Invalid;
    ImTextureID iconoCpp = ImTextureID_Invalid;
    ImTextureID iconoHpp = ImTextureID_Invalid;
    ImTextureID iconoGameObject = ImTextureID_Invalid;
    bool inicializado = false;
};

#endif