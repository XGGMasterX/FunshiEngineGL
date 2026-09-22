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
#ifndef SCENEMENUBARINTERFACE_H
#define SCENEMENUBARINTERFACE_H
#include <iostream>
#include <map>
#include <string>
#include "../ObjetosGUI/SettingsObjectInterface.h"
#include "../../Events/EditorEventBus.h"
#include <imgui.h>

using namespace std;

class SceneMenuBarInterface : public GeneralUserInterface {
protected:
    bool* toggleBool = nullptr;
    // Sistema de coordenadas del gizmo (true = GLOBAL/ejes del mundo, el gizmo
    // no rota con el objeto; false = LOCAL). Puntero NO propietario al estado
    // de GameScene, como toggleBool (la escena lo cede en su constructor).
    bool* gizmoGlobal = nullptr;
    bool cargarScripts = false;
    // Canal de GUI interna: el menu "Ventanas" publica VentanaEstadoCambio y
    // main/GUIManager aplican y persisten la visibilidad de cada panel.
    EditorEventBus* busEditor = nullptr;
    // Ultimos estados conocidos de las ventanas persistentes del editor
    // (etiqueta por WindowName). La alimenta GUIManager cada frame para que
    // las casillas del menu reflejen el estado real (incluido el cierre con X).
    std::map<std::string, bool> ventanas_;

public:
    SceneMenuBarInterface(bool stateGUI);
    void setActivador(bool* targetBool);
    bool* getActivador();
    // Cece el puntero al estado de coordendas globales del gizmo (GameScene).
    void setGizmoGlobal(bool* target);
    bool getCargarScripts();
    void setCargarScripts(bool value);
    void setEditorEventBus(EditorEventBus* bus);
    void setVentanas(const std::map<std::string, bool>& estados);
    virtual void initGUI() override;
    virtual void contentGUI() override;
    virtual void endGUI() override;
    virtual void printGUI() override;
};
#endif
