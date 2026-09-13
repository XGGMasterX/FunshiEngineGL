#ifndef MENUVIEW_H
#define MENUVIEW_H

#include "../GeneralUserInterface.h"
// GLFW se incluye solo por el botón "Exit"; la lógica de navegación y los
// datos viven en StartMenuModel, no acá.
#if defined(_WIN32)
#include <glfw3.h>
#elif defined(__linux__)
#include <GLFW/glfw3.h>
#endif

#include "MenuModel.h"
#include "StartMenuPresenter.h"

// Vista del menu de inicio: pantalla completa a traves del paquete MenuGUI.
// Es la capa de presentacion del patron MVP que define este paquete. NO
// contiene logica de navegacion ni de datos: dibuja a partir del estado del
// modelo y, ante los clics del usuario, delega la accion de vuelta al modelo
// (la clase base es la vista). Asi la interfaz se puede reemplazar o recorrer
// sin tocar la logica, y cada pantalla se reimplementa sin afectar a las demas.
class MenuView : public GeneralUserInterface {
private:
    MenuModel* model;
    // Decisiones que afectan al resto del motor (cerrar el menu = "Iniciar
    // Estudio") se notifican por el presenter, nunca directamente al modelo:
    // es el presenter quien informa a main via DebeCerrar().
    StartMenuPresenter* presenter;
    GLFWwindow* window;
    // Buffer de edicion del nombre del proyecto. Vive en la vista porque es
    // estado de UI (la vista avisa al modelo del cambio, no al reves).
    char nombreProyectoBuffer[128];

    void renderizarPrincipal();
    void renderizarOpciones();
    void renderizarConfigProyecto();

public:
    MenuView(MenuModel* model, StartMenuPresenter* presenter, GLFWwindow* window);

    void initGUI() override;
    void contentGUI() override;
    void endGUI() override;
    void printGUI() override;
};

#endif