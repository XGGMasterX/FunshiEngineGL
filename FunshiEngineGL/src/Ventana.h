#ifndef VENTANA_H
#define VENTANA_H

#if defined(_WIN32)
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#if defined(_WIN32)
#include <glfw3.h>
#elif defined(__linux__)
#include <GLFW/glfw3.h>
#endif

class Ventana {
private:
    static Ventana* instance;
    GLFWwindow* window = nullptr;
    int width = 800;
    int height = 600;

public:
    Ventana();
    static Ventana& getInstance();
    GLFWwindow* getWindow();
    int getWidth() const;
    int getHeight() const;
    int initVentana();
    void redimension(int ventanaWidthEjeX, int ventanaHeightEjeY);
};

#endif
