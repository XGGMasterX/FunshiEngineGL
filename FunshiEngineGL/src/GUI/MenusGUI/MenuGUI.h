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
#ifndef MENUGUI_H
#define MENUGUI_H

// =====================================================================
// PAQUETE: MenuGUI — interfaz de inicio (menu) del motor
// =====================================================================
// Este encabezado documenta el paquete de archivos que modulariza el menu
// de inicio y define la fachada del mismo. Siguiendo buenas practicas de
// disenio (SRP, bajo acoplamiento, alta cohesion, DIP), el paquete separa
// responsabilidades en capas con una direccion de dependencias clara:
//
//   MenuModel.h            -> Logica pura y estado (sin ImGui/GLFW). El
//                             "que": navegacion entre vistas y datos de
//                             configuracion (nombre de proyecto, idioma).
//   MenuView.h/.cpp        -> Presentacion ImGui (el "como se dibuja").
//                             Delega toda accion al modelo; no decide nada.
//   StartMenuPresenter.h   -> Puente modelo<->resto del motor (States/):
//                             responde "menu abierto/cerrado" y sincroniza
//                             cambios externos sin acoplar al motor con las
//                             clases internas del paquete.
//   MenuGUI.h/.cpp         -> Fachada del paquete: ensambla modelo + vista +
//                             presentador y expone la interfaz publica
//                             estable (ConsultarMenu / SetMenuActivo /
//                             ConsultarCierre / PedirCierre). main.cpp solo
//                             conversa con la fachada; las clases internas
//                             pueden reescribirse sin tocar el resto.
//
// Direccion de dependencias (una sola):
//   MenuGUI -> StartMenuPresenter -> MenuModel  <-  MenuView
// El modelo no sabe que existe la vista (inversion de dependencias), la
// vista no sabe que existe main, y main no conoce las clases internas.
//
// Reglas de extension (buenas practicas):
// - Nueva pantalla del menu: agregar valor a MenuModel::Vista + metodo de
//   transicion + metodo de render en MenuView (OCP: switch cerrado).
// - Nueva fuente de datos: propiedad en MenuModel + metodos de acceso +
//   sincronizacion desde el presentador (nunca desde la vista).
// - MenuModel debe permanecer sin dependencias de ImGui/GLFW (testeable).
// =====================================================================

#include "MenuModel.h"
#include "MenuView.h"
#include "StartMenuPresenter.h"

// Fachada del paquete: la interfaz publica con la que el resto del motor
// conversa. Encapsula las clases internas y su ensamblado.
class MenuGUI {
public:
    // Ensambla el paquete: modelo + vista (con la ventana GLFW para "Exit")
    // + presentador. La ventana llega por inyeccion de dependencias.
    MenuGUI(GLFWwindow* window);

    // ---- Interfaz publica estable (la que usa main.cpp) ----

    // "El menu esta abierto?" (per-frame; reemplaza al legacy showMenu).
    bool ConsultarMenu() const noexcept;

    // Sincroniza el estado abierto/cerrado decidido externamente (por
    // ejemplo, la transicion MenuPrincipal<->Edicion de main).
    void SetMenuActivo(bool abierto) noexcept;

    // Dibuja la pantalla activa del menu (delega en la vista interna; la
    // vista ya se auto-oculta si el modelo esta en Vista::Ninguna).
    void Renderizar();

    // "El usuario pidió cerrar el menu (Iniciar Estudio)?" — consumo unico:
    // retorna true una vez por peticion y luego se auto-limpia.
    bool ConsultarCierre() noexcept;

    // Pide el cierre del menu (lo consume ConsultarCierre).
    void PedirCierre() noexcept;

    // Configuracion global del editor configurada en la vista "Opciones"
    // (sensibilidad del mouse look de la camara). main la propaga a la escena
    // cuando la escena corre (el menu esta pausado mientras es visible).
    float getSensibilidadCamara() const noexcept;
    void setSensibilidadCamara(float sensibilidad) noexcept;

    // Perfil de apariencia del editor editado en la vista Opciones. main lo
    // aplica a ImGui (TemaEditor) y a la escena (fondo y grilla).
    const Apariencia& getApariencia() const noexcept;
    void setApariencia(const Apariencia& valor) noexcept;

    // Datos del menu persistidos por EditorConfig (main los aplica al arrancar
    // y los recoge al salir).
    const std::string& getNombreProyecto() const noexcept;
    void setNombreProyecto(const std::string& nombre) noexcept;
    const std::string& getIdioma() const noexcept;
    void setIdioma(const std::string& valor) noexcept;

private:
    // Orden de membresia = orden de construccion: el presentador se construye
    // ANTES que la vista porque la vista recibe su direccion (el presentador
    // solo guarda el puntero al modelo en su ctor, pero el orden correcto
    // elimina cualquier suposicion sobre orden de inicializacion).
    MenuModel model;
    StartMenuPresenter presenter;
    MenuView view;
};

#endif