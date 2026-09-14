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
#ifndef MENUMODEL_H
#define MENUMODEL_H

#include <string>
#include <vector>

// Modelo del paquete MenuGUI: la capa de logica pura del menu de inicio del
// motor (sin ImGui/GLFW ni rendering). Es el patron MVP de este paquete:
// - MenuModel    (este archivo): estado + navegacion entre vistas + datos de
//   configuracion. Independiente de la interfaz y testeable.
// - MenuView     (MenuView.h): presentacion. Solo lee el estado y delega las
//   acciones de vuelta aca; nunca decide la logica.
// - StartMenuPresenter (StartMenuPresenter.h): puente con el resto del motor:
//   traduce el estado del modelo a "el menu esta abierto/cerrado" para main
//   y sincroniza de vuelta cambios externos (p. ej. el nombre leido del
//   proyecto en disco).
// - MenuGUI      (MenuGUI.h): fachada del paquete; main no conoce las clases
//   internas, solo interactua con la interfaz publica del paquete (ver
//   README.md del paquete).
// El modelo centraliza el estado del menu para que las vistas se puedan
// reemplazar o recorrer sin tocar la logica.
class MenuModel {
public:
    enum class Vista { Principal, Opciones, ConfigProyecto, Ninguna };

    // Abre el menu principal (por ejemplo desde el editor con Escape).
    void mostrarMenu();
    // Cierra el menu y deja el motor listo para editar (boton Iniciar Estudio).
    void iniciarEstudio();
    void abrirOpciones();
    void abrirConfigProyecto();
    // Vuelve de Opciones/ConfigProyecto al menu principal.
    void volver();

    bool estaVisible() const noexcept;
    Vista getVista() const noexcept;

    const std::string& getNombreProyecto() const noexcept;
    void setNombreProyecto(const std::string& nombre);

    const std::string& getIdioma() const noexcept;
    void setIdioma(const std::string& valor);
    const std::vector<std::string>& getIdiomas() const noexcept;

    // Sensibilidad global del mouse look de la camara (vista Opciones del
    // menu). Multiplicador aplicado en main al offset del raton; 1.0 = 1:1
    // pixel/grado (comportamiento historico).
    float getSensibilidadCamara() const noexcept;
    void setSensibilidadCamara(float sensibilidad) noexcept;

private:
    Vista vista = Vista::Principal;
    std::string nombreProyecto = "Nuevo Proyecto";
    std::string idioma = "Espanol";
    std::vector<std::string> idiomasDisponibles = {"Espanol", "English"};
    float sensibilidadCamara = 1.0f;
};

#endif