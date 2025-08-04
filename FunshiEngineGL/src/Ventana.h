#ifndef Ventana_H
#define Ventana_H

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
#include <iostream>

class Ventana {
private:
    GLFWwindow* window;

public:
    //Ventana(){}

    GLFWwindow* getWindow() {
        return window;
    }

    int initVentana() {

        //Llamada a la libreria
        if (!glfwInit())
            return -1;

        //Delarar mode para utilizar el ancho y largo de la pantalla
        //glfwGetPrimaryMonitor();



        //Declaracion de la ventana
        window = glfwCreateWindow(800, 600, "Fanshi", NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            return -1;
        }

        /* Make the window's context current */
        glfwMakeContextCurrent(window);

        //Tomando Valores de Inicio
        std::cout << "GPU: " << glGetString(GL_RENDERER) << std::endl;
    return 0;
    }


    void redimension(int ventanaWidthEjeX, int ventanaHeightEjeY) {


        //Escalamos los Objetos
        glViewport(0, 0, ventanaWidthEjeX, ventanaHeightEjeY);
        float aspect = (float)ventanaWidthEjeX / (float)ventanaHeightEjeY;

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        //Fov,Tam,etc
        gluPerspective(45.0, aspect, 0.1, 500);
        glMatrixMode(GL_MODELVIEW);

    }


};
#endif
