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
#include "MenuModel.h"

// ============================================================================
// Implementacion de la logica pura del menu de inicio. Este archivo no
// depende de ImGui ni GLFW: el modelo debe permanecer testeable y aislado
// de la presentacion (ver MenuGUI.h y README.md del paquete).
// ============================================================================

void MenuModel::mostrarMenu() { vista = Vista::Principal; }

void MenuModel::iniciarEstudio() { vista = Vista::Ninguna; }

void MenuModel::abrirOpciones() { vista = Vista::Opciones; }

void MenuModel::abrirConfigProyecto() { vista = Vista::ConfigProyecto; }

void MenuModel::volver() {
    if (vista == Vista::Opciones || vista == Vista::ConfigProyecto) {
        vista = Vista::Principal;
    }
}

bool MenuModel::estaVisible() const noexcept { return vista != Vista::Ninguna; }

MenuModel::Vista MenuModel::getVista() const noexcept { return vista; }

const std::string& MenuModel::getNombreProyecto() const noexcept {
    return nombreProyecto;
}

void MenuModel::setNombreProyecto(const std::string& nombre) {
    nombreProyecto = nombre;
}

const std::string& MenuModel::getIdioma() const noexcept { return idioma; }

void MenuModel::setIdioma(const std::string& valor) { idioma = valor; }

const std::vector<std::string>& MenuModel::getIdiomas() const noexcept {
    return idiomasDisponibles;
}

float MenuModel::getSensibilidadCamara() const noexcept {
    return sensibilidadCamara;
}

void MenuModel::setSensibilidadCamara(float sensibilidad) noexcept {
    if (sensibilidad > 0.0f) sensibilidadCamara = sensibilidad;
}