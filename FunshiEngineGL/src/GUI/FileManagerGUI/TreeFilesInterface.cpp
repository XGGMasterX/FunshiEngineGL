#include "TreeFilesInterface.h"

#include "../../Herramientas/PathUtils.h"
#include "../../FileManager/FileManager.h"
#include "../../GestorDeArchivos/Carpeta.h"
#include "../../Herramientas/IconosGUI/IconosGUI.h"
#include <imgui.h>

namespace {
std::string rutaDe(Carpeta* c) {
    return c->getPathRoot() + PATH_SEP + c->getPathName();
}

// Elimina un nodo y TODO su subarbol del ArbolEnlazado (que nunca es dueno de
// sus File*): limpia openNodes, libera cada elemento y desvincula los nodos.
// Sin esto, deleteNodeInternalNode() promovia el primer hijo al lugar del padre
// y quedaban "carpetas fantasma" inexistentes en disco (B3).
void limpiarYLiberarSubarbol(ArbolEnlazado<File*>* arbol,
                             Position<File*>* p,
                             TreeIG::OpenState& openNodes) {
    if (!arbol || !p) return;
    if (p->getElement())
        openNodes.erase(static_cast<const void*>(p->getElement()));
    if (arbol->isInternal(p)) {
        auto* hijos = arbol->childsOf(p);
        auto* it = hijos->first();
        while (it) {
            Position<File*>* hijo = it->getElement();
            limpiarYLiberarSubarbol(arbol, hijo, openNodes);
            it = (it != hijos->last()) ? hijos->next(it) : nullptr;
        }
        delete hijos;
    }
    // Ya sin hijos, el nodo es hoja: deleteNode la desvincula y devuelve el
    // File*, que liberamos.
    delete arbol->deleteNode(p);
}
} // namespace

TreeFilesInterface::TreeFilesInterface(bool stateGUI, FileManager* fileManager)
    : GeneralUserInterface(WindowNames::BrowseFile, stateGUI, ImGuiWindowFlags_MenuBar),
      fileManager(fileManager),
      arbolDeArchivos(fileManager->getArbol()) {}

void TreeFilesInterface::setIconosGUI(IconosGUI* iconosG) { iconosGUI = iconosG; }

TreeIG::RowResult TreeFilesInterface::drawFolderRow(File* element, bool wasOpen) {
    Carpeta* folderRoot = dynamic_cast<Carpeta*>(element);
    if (!folderRoot) return {};
    FileSelection* sel = fileManager->getSelection();

    const bool isSelected = (sel->carpetaActual == folderRoot);
    ImGuiTreeNodeFlags nodeFlags =
        ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (isSelected) nodeFlags |= ImGuiTreeNodeFlags_Selected;
    if (wasOpen) nodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;

    if (iconosGUI && iconosGUI->getIconoCarpeta() != ImTextureID_Invalid) {
        ImGui::Image(iconosGUI->getIconoCarpeta(), ImVec2(22, 22));
        ImGui::SameLine();
    }

    const bool nodeOpen = ImGui::TreeNodeEx(
        folderRoot->getPathName().c_str(), nodeFlags, "%s",
        folderRoot->getPathName().c_str());
    const bool toggled = ImGui::IsItemToggledOpen();

    if (ImGui::IsItemClicked()) {
        sel->carpetaActual = folderRoot;
        sel->rutaVisible = rutaDe(folderRoot);
        sel->navegacionPendiente.clear();
    }

    if (ImGui::BeginPopupContextItem("MenuContextualCarpeta")) {
        ImGui::Text("Carpeta: %s", folderRoot->getPathName().c_str());
        ImGui::Separator();
        if (ImGui::MenuItem("Nueva Carpeta")) {
            const std::string rutaNuevaCarpeta =
                rutaDe(folderRoot) + PATH_SEP + "Nueva Carpeta";
            if (fileManager->crearCarpeta(rutaNuevaCarpeta)) {
                // No mutamos el arbol durante el recorrido (invalidaba
                // iteradores, B4): el rescaneo del proximo frame lo agrega.
                sel->contadorCambios++;
            }
        }
        if (ImGui::MenuItem("Eliminar Carpeta")) {
            carpetaAConfirmar = folderRoot;
            confirmarEliminar = true;
        }
        ImGui::EndPopup();
    }
    return {nodeOpen, toggled};
}

void TreeFilesInterface::initGUI() {
    ImGui::Begin(getNameGui().c_str(), &stateGUI, getFlagGui());

    if (confirmarEliminar && carpetaAConfirmar) {
        ImGui::OpenPopup("ConfirmarEliminar");
        confirmarEliminar = false;
    }
    if (ImGui::BeginPopupModal("ConfirmarEliminar", nullptr,
                               ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Eliminar \"%s\" y todo su contenido?",
                    carpetaAConfirmar ? carpetaAConfirmar->getPathName().c_str() : "");
        if (ImGui::Button("Eliminar", ImVec2(120, 0))) {
            carpetaAEliminar = carpetaAConfirmar;
            carpetaAConfirmar = nullptr;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancelar", ImVec2(120, 0))) {
            carpetaAConfirmar = nullptr;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void TreeFilesInterface::refrescarArbol() {
    fileManager->refrescar();
    arbolDeArchivos = fileManager->getArbol();
    openNodes.clear();
    carpetaAEliminar = nullptr;
    ultimoContadorVisto = fileManager->getSelection()->contadorCambios;
    // navegacionPendiente NO se limpia: es una ruta y debe aplicarse (FASE 2)
    // contra el arbol recien reconstruido; limpiarla aqui perderia el doble
    // clic que coincidio con un rescaneo (B6).
}

void TreeFilesInterface::aplicarNavegacionPendiente() {
    FileSelection* sel = fileManager->getSelection();
    if (sel->navegacionPendiente.empty()) return;
    const std::string ruta = sel->navegacionPendiente;
    Carpeta* objetivo = fileManager->buscarCarpetaPorRuta(ruta);
    sel->navegacionPendiente.clear();
    if (!objetivo) return;
    sel->carpetaActual = objetivo;
    sel->rutaVisible = ruta;

    // Expandir la jerarquia de padres para que sea visible.
    Position<File*>* currentPos = arbolDeArchivos->whatIsPositionOf(objetivo);
    while (currentPos) {
        File* currentElement = currentPos->getElement();
        openNodes.insert(static_cast<const void*>(currentElement));
        if (arbolDeArchivos->isRoot(currentPos)) break;
        currentPos = arbolDeArchivos->dadOf(currentPos);
    }
}

void TreeFilesInterface::contentGUI() {
    FileSelection* sel = fileManager->getSelection();

    // Refresco programado (actualizar) o por cambio de FS detectado (el panel
    // de contenido creo/copio una carpeta y subio el contador, R3).
    if (!arbolDeArchivos || actualizar ||
        sel->contadorCambios != ultimoContadorVisto) {
        refrescarArbol();
        actualizar = false;
    }

    // FASE 2 se aplica DESPUÉS de cualquier refresco y contra el arbol
    // vigente: asi el doble clic sobrevive a un rescaneo programado (B6).
    aplicarNavegacionPendiente();

    if (!arbolDeArchivos->isEmpty()) {
        TreeIG::drawTree(
            arbolDeArchivos, arbolDeArchivos->rootOfTree(), openNodes,
            [this](File* element, bool wasOpen) {
                return drawFolderRow(element, wasOpen);
            });
    }

    // Borrado diferido (fuera del recorrido del arbol, B4).
    if (carpetaAEliminar) {
        Carpeta* doomed = carpetaAEliminar;
        carpetaAEliminar = nullptr;
        if (sel->carpetaActual == doomed) {
            sel->carpetaActual = nullptr;
            sel->rutaVisible.clear();
        }
        Position<File*>* posicion = arbolDeArchivos->whatIsPositionOf(doomed);
        if (posicion && posicion != arbolDeArchivos->rootOfTree() &&
            fileManager->eliminarCarpeta(rutaDe(doomed))) {
            limpiarYLiberarSubarbol(arbolDeArchivos, posicion, openNodes);
        }
    }
}

void TreeFilesInterface::endGUI() { ImGui::End(); }

void TreeFilesInterface::printGUI() {
    if (stateGUI) {
        initGUI();
        contentGUI();
        endGUI();
    }
}

void TreeFilesInterface::solicitarActualizacion() { actualizar = true; }