#ifndef MENUINTERFACE_H
#define MENUINTERFACE_H

#include "../GeneralUserInterface.h"
#if defined(_WIN32)
#include <glfw3.h>
#elif defined(__linux__)
#include <GLFW/glfw3.h>
#endif
#include "./MenuOpcionesInterface.h"
#include "./MenuProyectoInterface.h"

class MenuInterface : public GeneralUserInterface{
private:
    GLFWwindow* m_window;
    MenuOpcionesInterface* opcionesGUI;
    MenuProyectoInterface* configProyectGUI;
    bool menuState;
public:
        // Constructor que recibe la ventana GLFW
        MenuInterface(GLFWwindow* window, bool stateGUI);

        virtual bool getMenusState();
        virtual void initGUI() override;
        virtual void contentGUI() override;
        virtual void endGUI() override;
        virtual MenuOpcionesInterface* getOpcionesGUI();
        virtual MenuProyectoInterface* getConfigProyectGUI();
        virtual void printGUI() override;
};
#endif
