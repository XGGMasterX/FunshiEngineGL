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
#define GLFW_INCLUDE_NONE
#include "EditorInput.h"

#include <cmath>

#include <GLFW/glfw3.h>
#include <imgui.h>

#include "../Scenes/GameScene.h"
#include "../Objetos/Componentes/CameraComponent.h"
#include "../States/ApplicationStateMachine.h"
#include "../States/OrquestadorEstadoGUI.h"
#include "ImGuizmo.h"

EditorInput* EditorInput::instancia = nullptr;

EditorInput::EditorInput(GameScene* escena, ApplicationStateMachine* maquina,
                         OrquestadorEstadoGUI* orquestador)
    : scene(escena), appState(maquina), orquestador(orquestador) {
    instancia = this;
}

void EditorInput::registrarCallbacks(GLFWwindow* window) {
    glfwSetKeyCallback(window, EditorInput::teclado_callback);
    glfwSetCursorPosCallback(window, EditorInput::mouse_callback);
    // Clic derecho navega junto con los paneles; el backend de ImGui encadena
    // estas callbacks (se registran antes de ImGui_ImplGlfw_InitForOpenGL).
    glfwSetMouseButtonCallback(window, EditorInput::mouse_button_callback);
}

void EditorInput::teclado_callback(GLFWwindow* window, int key, int scancode,
                                   int action, int mods) {
    if (instancia) instancia->onKey(window, key, scancode, action, mods);
}

void EditorInput::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (instancia) instancia->onMouse(window, xpos, ypos);
}

void EditorInput::mouse_button_callback(GLFWwindow* window, int button,
                                        int action, int mods) {
    if (instancia) instancia->onMouseButton(window, button, action, mods);
}

void EditorInput::aplicarModoCursor(GLFWwindow* window) {
    if (!window || !scene || !appState) return;
    const bool enEditor = appState->is(ApplicationState::Editing);
    const bool ocultar =
        enEditor && (mouseDerechoParaNavegar || !scene->isEditorActivo());
    const int modo = ocultar ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
    if (glfwGetInputMode(window, GLFW_CURSOR) != modo) {
        glfwSetInputMode(window, GLFW_CURSOR, modo);
        // Al atrapar/soltar el cursor (GLFW_CURSOR_DISABLED mueve el cursor
        // virtual al centro pero el OS queda donde estaba) se descarta el
        // delta acumulado del look.
        firstTimeMouseX = true;
        firstTimeMouseY = true;
    }
}

void EditorInput::descartarDeltaLook() {
    firstTimeMouseX = true;
    firstTimeMouseY = true;
}

void EditorInput::setAccionGuardar(std::function<void()> accion) {
    accionGuardar = std::move(accion);
}

void EditorInput::aplicarMovimiento(float deltaTime) {
    // La camara del editor solo se mueve con WASD/Espacio/Shift dentro del
    // editor (ni en el menu ni cuando ImGui esta capturando el teclado, p. ej.
    // mientras se edita un InputText).
    if (!scene || !appState || !appState->is(ApplicationState::Editing)) return;
    if (ImGui::GetIO().WantCaptureKeyboard) return;
    // Durante orbita (editor oculto + clic derecho): bloquear traslacion WASD.
    if (orbitando) return;
    // Misma regla que la mirada (EditorInput::onMouse): con las interfaces del
    // editor visibles el WASD no traslada la camara; hace falta ocultarlas (E)
    // o navegar con el clic derecho sostenido sobre el viewport. Sin esto se
    // podia volar por la escena sin "esconder" el editor.
    if (scene->isEditorActivo() && !mouseDerechoParaNavegar) return;

    CameraComponent* camara = scene->getActiveCamera();
    if (!camara) return;

    // Vector de direccion combinado [derecha, arriba, adelante] a partir de la
    // maquina de estado de teclas. Las diagonales (W+A, W+D, etc.) suman ejes
    // y el vector se NORMALIZA cuando el modulo supera 1: mover dos ejes a la
    // vez queda a la misma velocidad que un eje solo (antes, W y D eran cada
    // una una callback que escribia el Transform -> diagonal a sqrt(2) y con
    // reinicios de transform).
    float x = 0.f, y = 0.f, z = 0.f;
    if (teclaDerecha) x += 1.0f;
    if (teclaIzquierda) x -= 1.0f;
    if (teclaArriba) y += 1.0f;
    if (teclaAbajo) y -= 1.0f;
    if (teclaAdelante) z += 1.0f;
    if (teclaAtras) z -= 1.0f;
    if (x == 0.f && y == 0.f && z == 0.f) return;

    const float modulo =
        std::sqrt(x * x + y * y + z * z);
    const float factor = modulo > 1.0f ? 1.0f / modulo : 1.0f;
    const float direccion[3] = {x * factor, y * factor, z * factor};

    const float dt = deltaTime * scene->getSensibilidadMovimientoCamara();
    camara->moverDireccion(direccion, dt);
}

void EditorInput::onKey(GLFWwindow* window, int key, int scancode, int action,
                        int mods) {
    (void)scancode;
    (void)mods;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        // Volver al menu de inicio desde el editor: la maquina de estados es
        // la fuente de verdad; main refleja su decision en la fachada MenuGUI.
        if (ImGui::GetIO().WantCaptureKeyboard) {
            // Un InputText de ImGui esta activo: Escape revierte el texto en
            // edicion y no corta la edicion de datos del editor.
        } else if (appState && appState->is(ApplicationState::Editing)) {
            // La regla vive en el orquestador de estados de GUI.
            orquestador->manejarTeclaEscape();
        }
        aplicarModoCursor(window);
        return;
    }

    // Ctrl+S: guardado en caliente del proyecto (misma rutina que el guardado
    // al salir, inyectada por main). Se intercepta ANTES de la maquina de
    // movimiento para que el "S" con Ctrl no mueva la camara hacia atras, y
    // con el mismo guard que Escape: si un campo de texto de ImGui esta
    // capturando el teclado, la combinacion es del editor de texto.
    if (key == GLFW_KEY_S && (mods & GLFW_MOD_CONTROL) && action == GLFW_PRESS) {
        if (accionGuardar && !ImGui::GetIO().WantCaptureKeyboard) accionGuardar();
        return;
    }

    // F5/F6/F7: teclas de funcion de la simulacion (Play/Pausa/Stop). La regla
    // por estado vive en el orquestador (funcion de marco de la arquitectura,
    // igual que Escape); aca solo se reenvia el evento y se refleja la decision
    // sobre GameScene::start (el mismo flag que maneja el boton Activar/Detener
    // del menu de escena, asi ambas puertas comparten estado). F5 y F7 salen
    // entran de nav libre, por eso se recalcula tambien el modo del cursor.
    if ((key == GLFW_KEY_F5 || key == GLFW_KEY_F6 || key == GLFW_KEY_F7) &&
        action == GLFW_PRESS) {
        if (orquestador) {
            const auto tecla = key == GLFW_KEY_F5
                                   ? OrquestadorEstadoGUI::TeclaSimulacion::Play
                                   : key == GLFW_KEY_F6
                                         ? OrquestadorEstadoGUI::TeclaSimulacion::Pausa
                                         : OrquestadorEstadoGUI::TeclaSimulacion::Stop;
            orquestador->manejarTeclaSimulacion(tecla);
            // Refleja Playing/Editing sobre la simulacion de la escena (F5
            // arranca, F7 corta; F5 con la maquina ya en Playing re-asegura el
            // arranque despues de un "Detener" con el boton del menu de escena).
            if (scene) scene->setStart(orquestador->enSimulacion());
            // La pausa (F6) tambien se refleja: congela fisica/scripts sin
            // tocar `start` (asi no se dispara la limpieza de play->editor).
            if (scene)
                scene->setSimulacionPausada(orquestador->simulacionPausada());
            aplicarModoCursor(window);
        }
        return;
    }

    // Maquina de estado de movimiento: las teclas NO mueven la camara aca;
    // solo registran si estan apretadas/sueltas y aplicarMovimiento() las
    // combina por frame. GLFW_REPEAT no cambia el estado.
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_W: teclaAdelante = true; break;
            case GLFW_KEY_S: teclaAtras = true; break;
            case GLFW_KEY_A: teclaIzquierda = true; break;
            case GLFW_KEY_D: teclaDerecha = true; break;
            case GLFW_KEY_SPACE: teclaArriba = true; break;
            case GLFW_KEY_LEFT_SHIFT: teclaAbajo = true; break;
            default: break;
        }
    } else if (action == GLFW_RELEASE) {
        switch (key) {
            case GLFW_KEY_W: teclaAdelante = false; break;
            case GLFW_KEY_S: teclaAtras = false; break;
            case GLFW_KEY_A: teclaIzquierda = false; break;
            case GLFW_KEY_D: teclaDerecha = false; break;
            case GLFW_KEY_SPACE: teclaArriba = false; break;
            case GLFW_KEY_LEFT_SHIFT: teclaAbajo = false; break;
            default: break;
        }
        return;
    }

    if (key == GLFW_KEY_E && action == GLFW_PRESS) {
        // Durante orbita: bloquear toggle de editor (E).
        if (orbitando) return;
        // Solo en el estado de edicion: desde el menu de inicio la E no debe
        // "activar el editor" mostrando sus interfaces sobre el menu (fallo de
        // la maquina de estados). Con un InputText de ImGui activo, E tampoco
        // toca las interfaces.
        if (appState && appState->is(ApplicationState::Editing) &&
            !ImGui::GetIO().WantCaptureKeyboard) {
            if (scene) scene->toggleEditorInterfaces();
            // Entrar/salir de navegacion libre: captura y oculta el cursor.
            aplicarModoCursor(window);
        }
    }

    // G: alterna el sistema de coordenadas del gizmo entre LOCAL (ejes que
    // rotan con el objeto) y GLOBAL (ejes del mundo, el gizmo no rota).
    if (key == GLFW_KEY_G && action == GLFW_PRESS) {
        if (scene) scene->setGizmoGlobal(!scene->isGizmoGlobal());
    }

    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_1 || key == GLFW_KEY_T) {
            if (scene) scene->setGizmoOperation(ImGuizmo::TRANSLATE);
        } else if (key == GLFW_KEY_2 || key == GLFW_KEY_R) {
            if (scene) scene->setGizmoOperation(ImGuizmo::ROTATE);
        } else if (key == GLFW_KEY_3 || key == GLFW_KEY_Y) {
            if (scene) scene->setGizmoOperation(ImGuizmo::SCALE);
        }
    }
}

void EditorInput::onMouseButton(GLFWwindow* window, int button, int action,
                                int mods) {
    (void)mods;
    if (button != GLFW_MOUSE_BUTTON_RIGHT) return;
    if (action == GLFW_PRESS) {
        ImGuiIO& io = ImGui::GetIO();
        const bool gizmoCapturing = scene && scene->isGizmoCapturingInput();
        const bool editorActivo = scene && scene->isEditorActivo();

        if (!editorActivo && !io.WantCaptureMouse && !gizmoCapturing) {
// Editor oculto (E presionado) + clic derecho sobre la escena:
            // entrar en modo orbita. El origen esta en la recta de la direccion
            // de la camara; el radio es la distancia actual camara-origen.
            if (CameraComponent* camara = scene ? scene->getActiveCamera() : nullptr) {
                camara->refreshFromTransform();
                const float* pos = camara->getPosition();
                const float* dir = camara->getDirection();
                // origen = pos + dir * radio. La camara queda sobre la
                // circunferencia y el origen sobre la recta en direccion y
                // sentido a donde mira la camara.
                float radio = 10.0f; // radio fijo para la orbita
                origenOrbita[0] = pos[0] + dir[0] * radio;
                origenOrbita[1] = pos[1] + dir[1] * radio;
                origenOrbita[2] = pos[2] + dir[2] * radio;
                orbitando = true;
            }
        } else {
            mouseDerechoParaNavegar = !io.WantCaptureMouse && !gizmoCapturing;
        }
    } else {
        mouseDerechoParaNavegar = false;
        orbitando = false;
    }
    aplicarModoCursor(window);
}

void EditorInput::onMouse(GLFWwindow* window, double xpos, double ypos) {
    (void)window;
    float dx;
    float dy;
    if (firstTimeMouseX) {
        dx = 0;
        dy = 0;
        lastMousePosX = xpos;
        firstTimeMouseX = false;
    }
    if (firstTimeMouseY) {
        dx = 0;
        dy = 0;
        lastMousePosY = ypos;
        firstTimeMouseY = false;
    }

    dx = static_cast<float>(xpos - lastMousePosX);
    dy = static_cast<float>(ypos - lastMousePosY);

    lastMousePosX = xpos;
    lastMousePosY = ypos;

    ImGuiIO& io = ImGui::GetIO();
    const bool gizmoCapturing = scene && scene->isGizmoCapturingInput();
    const bool editorActivo = scene && scene->isEditorActivo();
    // Tres caminos para rotar la camara:
    //  (1) modo editor libre (sin E y sin objeto seleccionado), mirada con el
    //      mouse suelto y sin ImGui capturando la escena;
    //  (2) CLIC DERECHO sostenido sobre la escena desde el editor: permite
    //      mira-se (y con WASD trasladarse) sin apretar E y SIN esconder las
    //      interfaces. Una vez enganchado, se mantiene aunque el cursor pase
    //      sobre un panel (los popups de ImGui necesitan clic nuevo).
    //  (3) ORBITA: editor oculto (E) + clic derecho sostenido sobre la escena.
    //      La camara gira alrededor de un punto origen en la direccion de mirada,
    //      manteniendo radio fijo y mirando siempre hacia el origen.
    const bool navegandoLibre =
        !editorActivo && !io.WantCaptureMouse && !gizmoCapturing && !orbitando;
    const bool navegandoConDerecho =
        mouseDerechoParaNavegar && !gizmoCapturing && !orbitando;

    if (orbitando && !gizmoCapturing) {
        if (CameraComponent* camara = scene ? scene->getActiveCamera() : nullptr) {
            const float sensibilidad =
                scene ? scene->getSensibilidadCamara() : 1.0f;
            camara->orbitAround(origenOrbita, dx * sensibilidad, dy * sensibilidad);
        }
    } else if (navegandoLibre || navegandoConDerecho) {
        if (CameraComponent* camara = scene ? scene->getActiveCamera() : nullptr) {
            const float sensibilidad =
                scene ? scene->getSensibilidadCamara() : 1.0f;
            camara->updateYaw(dx * sensibilidad, dy * sensibilidad);
        }
    }
}