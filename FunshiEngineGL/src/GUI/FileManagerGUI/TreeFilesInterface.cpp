#include "TreeFilesInterface.h"

#include "../../Herramientas/IconosGUI/IconosGUI.h"
#include "../../GestorDeArchivos/GestorDeArchivos.h"
#include "../../GestorDeArchivos/Carpeta.h"
#include <imgui.h>

// Separador de rutas segun plataforma.
#ifdef _WIN32
const std::string PATH_SEP = "\\";
#else
const std::string PATH_SEP = "/";
#endif

TreeFilesInterface::TreeFilesInterface(bool stateGUI, const std::string& pathProyect)
    : GeneralUserInterface("BrowseFile", stateGUI, ImGuiWindowFlags_MenuBar),
      pathProyect(pathProyect) {
    gestorDeArchivos = new GestorDeArchivos(pathProyect);
}

TreeFilesInterface::~TreeFilesInterface() {
    delete gestorDeArchivos;
    gestorDeArchivos = nullptr;
}

void TreeFilesInterface::setIconosGUI(IconosGUI* iconosG) { iconosGUI = iconosG; }

TreeIG::RowResult TreeFilesInterface::drawFolderRow(File* element, bool wasOpen) {
    // Solo las carpetas se muestran en el arbol; los archivos viven en el
    // panel de contenido de la carpeta actual.
    Carpeta* folderRoot = dynamic_cast<Carpeta*>(element);
    if (!folderRoot) return {};

    const bool isSelected = (lastSelectedFolder == folderRoot);
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
        lastSelectedFolder = folderRoot;
        thisFolderContent = folderRoot;
    }

    if (ImGui::BeginPopupContextItem("MenuContextualCarpeta")) {
        ImGui::Text("Carpeta: %s", folderRoot->getPathName().c_str());
        ImGui::Separator();

        if (ImGui::MenuItem("Nueva Carpeta")) {
            const std::string nombreNuevaCarpeta = "Nueva Carpeta";
            const std::string rutaNuevaCarpeta =
                folderRoot->getPathRoot() + PATH_SEP +
                folderRoot->getPathName() + PATH_SEP + nombreNuevaCarpeta;

            if (gestorDeArchivos->crearCarpeta(rutaNuevaCarpeta)) {
                Carpeta* nuevaCarpeta = new Carpeta(nombreNuevaCarpeta);
                nuevaCarpeta->setPathRoot(rutaNuevaCarpeta);
                Position<File*>* posicionPadre =
                    arbolDeArchivos->whatIsPositionOf(folderRoot);
                if (posicionPadre) {
                    arbolDeArchivos->addNodeChildOf(posicionPadre, nuevaCarpeta);
                    // Dejar la carpeta padre abierta para ver la nueva.
                    openNodes.insert(static_cast<const void*>(folderRoot));
                    actualizar = true;
                } else {
                    delete nuevaCarpeta;
                }
            }
        }

        if (ImGui::MenuItem("Eliminar Carpeta")) {
            // Diferido: deleteNode durante el recorrido invalidaria iteradores.
            if (lastSelectedFolder == folderRoot) lastSelectedFolder = nullptr;
            if (thisFolderContent == folderRoot) thisFolderContent = nullptr;
            carpetaAEliminar = folderRoot;
        }

        ImGui::EndPopup();
    }

    return {nodeOpen, toggled};
}

void TreeFilesInterface::initGUI() {
    ImGui::Begin(getNameGui().c_str(), &stateGUI, getFlagGui());
}

void TreeFilesInterface::refrescarArbol() {
    // Reconstruir desde disco. GestorDeArchivos libera el arbol previo
    // (nodos y File*), por eso se descartan seleccion/estado colapsado:
    // apuntaban a objetos que ya no existen.
    gestorDeArchivos->setTreeFilePath(pathProyect, "MotorGrafico");
    arbolDeArchivos = gestorDeArchivos->getTreeFilePath();
    openNodes.clear();
    lastSelectedFolder = nullptr;
    thisFolderContent = nullptr;
    carpetaAEliminar = nullptr;
}

void TreeFilesInterface::contentGUI() {
    // El arbol solo se (re)construye cuando hace falta: la primera vez o
    // tras una operacion de archivos del explorador. Antes se reconstruia
    // cada frame descartando el arbol anterior sin liberar memoria
    // (fuga masiva -> "error en memoria").
    if (!arbolDeArchivos || actualizar) {
        refrescarArbol();
        actualizar = false;
    }

    if (!arbolDeArchivos->isEmpty()) {
        TreeIG::drawTree(
            arbolDeArchivos, arbolDeArchivos->rootOfTree(), openNodes,
            [this](File* element, bool wasOpen) {
                return drawFolderRow(element, wasOpen);
            });
    }

    if (carpetaAEliminar) {
        Carpeta* doomed = carpetaAEliminar;
        carpetaAEliminar = nullptr;
        openNodes.erase(static_cast<const void*>(doomed));
        Position<File*>* posicion = arbolDeArchivos->whatIsPositionOf(doomed);
        const std::string ruta =
            doomed->getPathRoot() + PATH_SEP + doomed->getPathName();
        if (posicion && posicion != arbolDeArchivos->rootOfTree() &&
            gestorDeArchivos->eliminarCarpeta(ruta)) {
            // deleteNode libera el nodo pero NO el File*; lo hacemos aqui.
            delete arbolDeArchivos->deleteNode(posicion);
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

Carpeta* TreeFilesInterface::getFolderContent() { return lastSelectedFolder; }

void TreeFilesInterface::setFolderContent(Carpeta* folder) {
    thisFolderContent = folder;
}