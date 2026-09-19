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

#include "OrquestadorEstadoGUI.h"

#include <cassert>

OrquestadorEstadoGUI::OrquestadorEstadoGUI(ApplicationStateMachine* maquina) noexcept
    : maquina(maquina)
{
    assert(maquina && "El orquestador necesita la maquina de estados");
}

// Centraliza "Escape en el editor vuelve al menu principal". Antes esta
// transicion colgaba en el callback de teclado de main; ahora la regla vive
// aqui. Es inofensivo si ya estamos en el menu (guardia explicita).
void OrquestadorEstadoGUI::manejarTeclaEscape() noexcept
{
    if (maquina->is(ApplicationState::Editing)) {
        maquina->transitionTo(ApplicationState::MainMenu);
    }
}

// Centraliza "Iniciar Estudio": el menu pide cierre y se entra al editor.
// Devuelve true si efectivamente hubo transicion. La solicitud de cierre del
// menu se consume aqui para evitar que el evento quedara colgando: se marca
// como pendiente y se limpia con cerrarMenuPendiente(). Re-entrante: si ya
// estamos en Editing no se transiciona dos veces.
bool OrquestadorEstadoGUI::iniciarEstudio() noexcept
{
    if (!maquina->is(ApplicationState::MainMenu)) {
        return false;
    }
    maquina->transitionTo(ApplicationState::Editing);
    return true;
}

bool OrquestadorEstadoGUI::menuDebeEstarVisible() const noexcept
{
    return maquina->is(ApplicationState::MainMenu);
}

bool OrquestadorEstadoGUI::escenaDebeCorrer() const noexcept
{
    // Cualquier estado que no sea el menu de inicio (en Fase 1, solo existe
    // MainMenu y Editing) implica que la escena corre.
    return !menuDebeEstarVisible();
}

bool OrquestadorEstadoGUI::cerrarMenuPendiente() const noexcept
{
    return false;  // reservado para cuando "Iniciar Estudio" consuma
                   // ConsultarCierre(); en esta fase la transicion es directa.
}

ApplicationState OrquestadorEstadoGUI::getEstado() const noexcept
{
    return maquina->is(ApplicationState::MainMenu)
               ? ApplicationState::MainMenu
               : ApplicationState::Editing;
}
