#include "StartMenuPresenter.h"

// ============================================================================
// Implementacion del presentador: puente entre el modelo puro (MenuModel) y
// el resto del motor. No dibuja nada (eso es MenuView) ni contiene logica de
// datos: solo traduce el estado del modelo a las preguntas per-frame de main
// y sincroniza de vuelta cambios externos (ver MenuGUI.h del paquete).
// ============================================================================

StartMenuPresenter::StartMenuPresenter(MenuModel* modelo) : model(modelo) {}

bool StartMenuPresenter::MenuEstaActivo() const noexcept {
    return model->estaVisible();
}

void StartMenuPresenter::PedirCierre() noexcept { pedirCierre = true; }

bool StartMenuPresenter::DebeCerrar() noexcept {
    if (pedirCierre) {
        pedirCierre = false;
        if (model->estaVisible()) model->iniciarEstudio();
        return true;
    }
    return false;
}

void StartMenuPresenter::NotificarNombreProyecto(const std::string& nombre) {
    model->setNombreProyecto(nombre);
}

const MenuModel* StartMenuPresenter::GetModel() const noexcept {
    return model;
}