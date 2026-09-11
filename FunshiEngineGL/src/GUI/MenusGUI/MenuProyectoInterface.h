#ifndef MENUPROYECTOINTERFACE_H
#define MENUPROYECTOINTERFACE_H
#include "../GeneralUserInterface.h"
#if defined(_WIN32)
#include <glfw3.h>
#elif defined(__linux__)
#include <GLFW/glfw3.h>
#endif
using namespace std;

class MenuProyectoInterface : public GeneralUserInterface {
private:
    GLFWwindow* m_window;
public:
    MenuProyectoInterface(GLFWwindow* window, bool stateGUI);
    virtual void initGUI() override;
    virtual void contentGUI() override;
    virtual void endGUI() override;
    virtual void printGUI() override;
};
#endif
