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
#include "MenuGUI.h"
#include "../../Events/EditorEventBus.h"

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

const Apariencia& MenuGUI::getApariencia() const noexcept {
    return model.getApariencia();
}

void MenuGUI::setApariencia(const Apariencia& valor) noexcept {
    model.setApariencia(valor);
    // Publicar el perfil al canal de GUI: los suscriptores (main: estilo de
    // ImGui, fondo/grilla de la escena y persistencia) reaccionan al cambio
    // en vivo sin que nadie relea el modelo a mano.
    if (busEditor) {
        EditorEvent ev;
        ev.type = EditorEventType::AparienciaCambio;
        ev.apariencia = valor;
        busEditor->publish(ev);
    }
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
    // El idioma viaja por el bus: main lo persiste al instante (sobrevive un
    // cierre brusco) y las etiquetas sensibles se refrescan via onLanguageChanged.
    if (busEditor) {
        EditorEvent ev;
        ev.type = EditorEventType::IdiomaCambio;
        ev.idioma = valor;
        busEditor->publish(ev);
    }
}

void MenuGUI::setEditorEventBus(EditorEventBus* bus) noexcept {
    busEditor = bus;
}