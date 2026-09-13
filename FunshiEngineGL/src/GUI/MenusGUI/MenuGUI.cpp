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