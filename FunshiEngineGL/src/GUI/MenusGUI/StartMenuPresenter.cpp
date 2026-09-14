/*
    FunshiEngineGL - Motor de juegos 3D con OpenGL e ImGui
    Copyright 2026 Gianfranco Ivan Enrique

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    SPDX-License-Identifier: Apache-2.0
*/
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