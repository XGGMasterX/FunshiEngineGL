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
#ifndef STARTMENUPRESENTER_H
#define STARTMENUPRESENTER_H

#include "MenuModel.h"

// Presentador del paquete MenuGUI: el intermediario entre el modelo puro
// (MenuModel) y el resto del motor. En la practica responde dos preguntas por
// frame, sin que main tenga que interpretar el modelo directamente:
//  1) "el menu esta abierto?" -> EstaAbierto() / MenuEstaActivo()
//  2) "el usuario pidió cerrar el menu?" -> DebeCerrar() (uso unico, se auto-
//     limpia para no repetir la transicion en el frame siguiente)
// Tambien ofrece sincronizacion de vuelta (NotificarNombreProyecto) para que
// cambios externos al menu (p. ej. el nombre leido del proyecto en disco)
// se reflejen en la UI sin acoplar al motor con las clases internas del
// paquete. No dibuja nada: eso es MenuView.
class StartMenuPresenter {
public:
    explicit StartMenuPresenter(MenuModel* modelo);

    // "El menu esta abierto?" (interfaz legacy para main).
    bool MenuEstaActivo() const noexcept;

    // Pide el cierre para el proximo frame; DebeCerrar() lo consume una vez.
    void PedirCierre() noexcept;

    bool DebeCerrar() noexcept;

    // Sincronizacion de vuelta: cambios externos al nombre del proyecto.
    void NotificarNombreProyecto(const std::string& nombre);

    // Acceso de solo lectura al modelo para la vista (MenuView).
    const MenuModel* GetModel() const noexcept;

private:
    MenuModel* model;
    bool pedirCierre = false;
};

#endif