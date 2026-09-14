#include "MenuGUI.h"

MenuGUI::MenuGUI(GLFWwindow* window)
    : view(&model, &presenter, window), presenter(&model) {}

bool MenuGUI::ConsultarMenu() const noexcept {
    // Delegado en el presentador: main no interpreta el modelo directamente.
    return presenter.MenuEstaActivo();
}

void MenuGUI::SetMenuActivo(bool abierto) noexcept {
    if (abierto) {
        model.mostrarMenu();
    } else if (model.estaVisible()) {
        model.iniciarEstudio();
    }
}

bool MenuGUI::ConsultarCierre() noexcept {
    // Consumo unico: el presenter auto-limpia la peticion al entregarla.
    return presenter.DebeCerrar();
}

void MenuGUI::Renderizar() { view.printGUI(); }

void MenuGUI::PedirCierre() noexcept {
    presenter.PedirCierre();
}

float MenuGUI::getSensibilidadCamara() const noexcept {
    return model.getSensibilidadCamara();
}

void MenuGUI::setSensibilidadCamara(float sensibilidad) noexcept {
    model.setSensibilidadCamara(sensibilidad);
}

const std::string& MenuGUI::getNombreProyecto() const noexcept {
    return model.getNombreProyecto();
}

void MenuGUI::setNombreProyecto(const std::string& nombre) noexcept {
    model.setNombreProyecto(nombre);
}

const std::string& MenuGUI::getIdioma() const noexcept {
    return model.getIdioma();
}

void MenuGUI::setIdioma(const std::string& valor) noexcept {
    model.setIdioma(valor);
}