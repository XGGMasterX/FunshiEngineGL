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
#include "MenuView.h"

#include <cstring>

namespace {
// Constantes de layout del menu (solo lectura): tamanio de boton y desvios
// verticales desde el centro de la ventana para cada fila.
constexpr float kBotonAncho = 200.0f;
constexpr float kBotonAlto = 50.0f;
constexpr float kSeparacionVertical = 60.0f;

// Posiciona el cursor para el boton de la fila `fila` (0 = "Iniciar Estudio",
// 1 = "Config Proyect", 2 = "Opciones", 3 = "Exit"), centrado horizontalmente.
void cursorFila(int fila) {
    ImVec2 c = ImGui::GetWindowSize();
    c.x *= 0.5f;
    c.y *= 0.5f;
    ImGui::SetCursorPos(ImVec2(c.x - kBotonAncho * 0.5f,
                               c.y - 2.0f * kSeparacionVertical
                                   + fila * kSeparacionVertical));
}

// Alinea verticalmente una etiqueta a la derecha del boton recien dibujado.
void etiquetaDerecha(ImVec2 posBoton, const char* texto) {
    ImGui::SameLine();
    ImGui::SetCursorScreenPos(ImVec2(
        posBoton.x + kBotonAncho + 10.0f,
        posBoton.y + (kBotonAlto - ImGui::GetTextLineHeight()) * 0.5f));
    ImGui::Text("%s", texto);
}
} // namespace

MenuView::MenuView(MenuModel* model, StartMenuPresenter* presenter, GLFWwindow* window)
    : GeneralUserInterface("Menu", true, ImGuiWindowFlags_NoTitleBar |
                                             ImGuiWindowFlags_NoResize |
                                             ImGuiWindowFlags_NoMove |
                                             ImGuiWindowFlags_NoCollapse |
                                             ImGuiWindowFlags_NoDocking),
      model(model),
      presenter(presenter),
      window(window),
      nombreProyectoBuffer{0} {}

void MenuView::initGUI() {
    // Pantalla completa sin decoracion, fondo opaco propio del menu. El color
    // se deriva del tema activo (TemaEditor) en lugar de un azul fijo, asi el
    // menu acompana al modo claro/oscuro y al modo blanco y negro.
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImVec4 fondoMenu = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
    fondoMenu.w = 1.0f;
    ImGui::PushStyleColor(ImGuiCol_WindowBg, fondoMenu);
    ImGui::Begin(getNameGui().c_str(), &stateGUI, getFlagGui());
}

void MenuView::contentGUI() {
    // Un solo Begin por frame: switch sobre la vista activa del modelo.
    switch (model->getVista()) {
    case MenuModel::Vista::Principal:
        renderizarPrincipal();
        break;
    case MenuModel::Vista::Opciones:
        renderizarOpciones();
        break;
    case MenuModel::Vista::ConfigProyecto:
        renderizarConfigProyecto();
        break;
    case MenuModel::Vista::Ninguna:
        break;
    }
}

void MenuView::endGUI() {
    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

void MenuView::printGUI() {
    // El menu (todas sus vistas) se dibuja solo si el modelo lo declara visible.
    if (!model->estaVisible()) return;
    initGUI();
    contentGUI();
    endGUI();
}

void MenuView::renderizarPrincipal() {
    cursorFila(0);
    if (ImGui::Button("Iniciar Estudio", ImVec2(kBotonAncho, kBotonAlto))) {
        // Cerrar el menu afecta al resto del motor: se notifica por el
        // presenter (DebeCerrar -> ConsultarCierre en main), que hace la
        // transicion real. La vista solo informa la intencion.
        presenter->PedirCierre();
    }

    cursorFila(1);
    ImVec2 posBoton = ImGui::GetCursorScreenPos();
    if (ImGui::Button("Config Proyect", ImVec2(kBotonAncho, kBotonAlto))) {
        model->abrirConfigProyecto();
    }
    etiquetaDerecha(posBoton, model->getNombreProyecto().c_str());

    cursorFila(2);
    posBoton = ImGui::GetCursorScreenPos();
    if (ImGui::Button("Opciones", ImVec2(kBotonAncho, kBotonAlto))) {
        model->abrirOpciones();
    }
    etiquetaDerecha(posBoton, model->getIdioma().c_str());

    cursorFila(3);
    if (ImGui::Button("Exit", ImVec2(kBotonAncho, kBotonAlto))) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void MenuView::renderizarOpciones() {
    // Columna centrada con scroll: la vista crecio con las opciones de
    // apariencia y ya no entra en las filas fijas del menu principal.
    const ImVec2 win = ImGui::GetWindowSize();
    constexpr float kAncho = 460.0f;
    constexpr float kAltoInferior = 90.0f; // deja lugar al boton "Volver"

    ImGui::SetCursorPos(ImVec2((win.x - kAncho) * 0.5f, 50.0f));
    ImGui::BeginChild("##opciones", ImVec2(kAncho, win.y - 50.0f - kAltoInferior),
                      false, ImGuiWindowFlags_AlwaysVerticalScrollbar);

    ImGui::TextUnformatted("Opciones");
    ImGui::Separator();

    ImGui::SeparatorText("Juego");
    ImGui::TextUnformatted("Idioma");
    const std::vector<std::string>& idiomas = model->getIdiomas();
    const std::string& actual = model->getIdioma();
    ImGui::SetNextItemWidth(-1.0f);
    if (ImGui::BeginCombo("##idioma", actual.c_str())) {
        for (const std::string& opcion : idiomas) {
            const bool seleccionada = (opcion == actual);
            if (ImGui::Selectable(opcion.c_str(), seleccionada)) {
                model->setIdioma(opcion);
            }
            if (seleccionada) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::TextUnformatted("Sensibilidad de camara");
    float sensibilidad = model->getSensibilidadCamara();
    ImGui::SetNextItemWidth(-1.0f);
    if (ImGui::SliderFloat("##sensibilidad", &sensibilidad, 0.02f, 5.0f,
                           "%.2f")) {
        model->setSensibilidadCamara(sensibilidad);
    }

    ImGui::SeparatorText("Apariencia");
    // Se edita una copia y se delega al modelo UNA vez si hubo cambios; asi
    // el modelo sigue siendo la unica fuente de verdad (patron MVP).
    Apariencia ap = model->getApariencia();
    bool cambio = false;

    cambio |= ImGui::Checkbox("Tema claro de la interfaz", &ap.temaClaro);
    cambio |= ImGui::Checkbox("Modo blanco y negro (fondo y grilla)",
                              &ap.blancoYNegro);
    ImGui::TextUnformatted("Color de acento de la interfaz");
    cambio |= ImGui::ColorEdit4("##acento", ap.acento,
                                ImGuiColorEditFlags_AlphaBar);
    ImGui::TextUnformatted("Color de fondo de la escena");
    cambio |= ImGui::ColorEdit3("##fondo", ap.fondo);
    ImGui::TextDisabled(
        "El modo blanco y negro ignora estos colores y usa\n"
        "blanco/negro segun el tema claro u oscuro.");

    if (ImGui::Button("Restablecer apariencia", ImVec2(-1.0f, 0.0f))) {
        ap.restablecer();
        cambio = true;
    }

    if (cambio) model->setApariencia(ap);

    ImGui::EndChild();

    // Boton "Volver" fijo abajo, centrado, fuera del area con scroll.
    ImGui::SetCursorPos(ImVec2((win.x - kBotonAncho) * 0.5f,
                               win.y - kAltoInferior + 20.0f));
    if (ImGui::Button("Volver", ImVec2(kBotonAncho, kBotonAlto))) {
        model->volver();
    }
}

void MenuView::renderizarConfigProyecto() {
    cursorFila(0);
    ImGui::Text("Config Proyect");

    cursorFila(1);
    ImGui::Text("Nombre");
    ImGui::SameLine();

    // Estado de UI: el buffer arranca vacio y se refresca desde el modelo la
    // primera vez; cada cambio validado se delega al modelo.
    if (nombreProyectoBuffer[0] == '\0') {
        std::strncpy(nombreProyectoBuffer,
                     model->getNombreProyecto().c_str(),
                     sizeof(nombreProyectoBuffer) - 1);
        nombreProyectoBuffer[sizeof(nombreProyectoBuffer) - 1] = '\0';
    }
    ImGui::SetNextItemWidth(kBotonAncho);
    if (ImGui::InputText("##nombreProyecto", nombreProyectoBuffer,
                         sizeof(nombreProyectoBuffer))) {
        model->setNombreProyecto(nombreProyectoBuffer);
    }

    cursorFila(2);
    if (ImGui::Button("Volver", ImVec2(kBotonAncho, kBotonAlto))) {
        model->volver();
    }
}