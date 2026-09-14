#include "TreeFilesInterface.h"
#include "../../Herramientas/IconosGUI/IconosGUI.h"
#include "../../GestorDeArchivos/GestorDeArchivos.h"
#include "../../GestorDeArchivos/Carpeta.h"
#include <imgui.h>

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

// --- FASE 1: REGISTRAR LA RUTA PENDIENTE (sin tocar estado del árbol) ---
void TreeFilesInterface::requestOpenFolder(Carpeta* parent, const std::string& childName) {
    if (!parent || childName.empty()) return;
    // La ruta completa del hijo (carpetas reales: pathRoot = directorio
    // padre). Guardamos la ruta (no el puntero) para que sobreviva a un
    // refrescarArbol() intermedio, aplicándose en FASE 2 contra el árbol vigente.
    pendingFolderPath = parent->getPathRoot() + PATH_SEP +
                        parent->getPathName() + PATH_SEP + childName;
}

TreeIG::RowResult TreeFilesInterface::drawFolderRow(File* element, bool wasOpen) {
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
                // No mutamos el árbol durante el recorrido (invalidaba
                // iteradores, B4): el rescaneo del próximo frame lo agrega.
                actualizar = true;
            }
        }
        if (ImGui::MenuItem("Eliminar Carpeta")) {
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
    gestorDeArchivos->setTreeFilePath(pathProyect, "MotorGrafico");
    arbolDeArchivos = gestorDeArchivos->getTreeFilePath();
    openNodes.clear();
    lastSelectedFolder = nullptr;
    thisFolderContent = nullptr;
    carpetaAEliminar = nullptr;
    // pendingFolderPath NO se limpia: es una ruta y debe aplicarse (FASE 2)
    // contra el árbol recién reconstruido; limpiarla aquí perdería el doble
    // clic que coincidió con un rescaneo (B6).
}

// Busca pre-orden la primera Carpeta cuya ruta completa coincida. La raiz se
// trata como contenedor (su pathRoot+pathName no es una ruta real).
static Carpeta* buscarCarpetaPorRuta(ArbolEnlazado<File*>* arbol,
                                     Position<File*>* current,
                                     const std::string& ruta) {
    if (!arbol || !current) return nullptr;
    File* elemento = current->getElement();
    if (elemento && current != arbol->rootOfTree() &&
        (elemento->getPathRoot() + PATH_SEP + elemento->getPathName()) == ruta) {
        return dynamic_cast<Carpeta*>(elemento);
    }
    if (arbol->isInternal(current)) {
        Carpeta* encontrado = nullptr;
        TreeIG::forEachChild((TNodo<File*>*)current, [&](Position<File*>* hijo) {
            if (!encontrado) encontrado = buscarCarpetaPorRuta(arbol, hijo, ruta);
        });
        return encontrado;
    }
    return nullptr;
}

// Elimina un nodo y TODO su subarbol del ArbolEnlazado (que nunca es dueno de
// sus File*): limpia openNodes, libera cada elemento y desvincula los nodos.
// Sin esto, deleteNodeInternalNode() promovia el primer hijo al lugar del padre
// y quedaban "carpetas fantasma" inexistentes en disco (B3).
static void limpiarYLiberarSubarbol(ArbolEnlazado<File*>* arbol,
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

void TreeFilesInterface::contentGUI() {
    // FASE 2 se aplica DESPUÉS de cualquier refresco y contra el árbol vigente:
    // refrescarArbol() no invalida pendingFolderPath (es una ruta), así el
    // doble clic sobrevive a un rescaneo programado (B6).
    if (!arbolDeArchivos || actualizar) {
        refrescarArbol();
        actualizar = false;
    }

    if (!pendingFolderPath.empty()) {
        Carpeta* objetivo = buscarCarpetaPorRuta(
            arbolDeArchivos, arbolDeArchivos->rootOfTree(), pendingFolderPath);
        pendingFolderPath.clear();
        if (objetivo) {
            lastSelectedFolder = objetivo;
            thisFolderContent = objetivo;

            // Expandir la jerarquía de padres para que sea visible
            Position<File*>* currentPos = arbolDeArchivos->whatIsPositionOf(objetivo);
            while (currentPos) {
                File* currentElement = currentPos->getElement();
                openNodes.insert(static_cast<const void*>(currentElement));
                if (arbolDeArchivos->isRoot(currentPos)) break;
                currentPos = arbolDeArchivos->dadOf(currentPos);
            }
        }
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
        Position<File*>* posicion = arbolDeArchivos->whatIsPositionOf(doomed);
        const std::string ruta =
            doomed->getPathRoot() + PATH_SEP + doomed->getPathName();
        if (posicion && posicion != arbolDeArchivos->rootOfTree() &&
            gestorDeArchivos->eliminarCarpeta(ruta)) {
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

Carpeta* TreeFilesInterface::getFolderContent() { return lastSelectedFolder; }

// Ambas referencias deben apuntar a lo mismo: getFolderContent() lee
// lastSelectedFolder, por eso setFolderContent también lo actualiza (B5).
void TreeFilesInterface::setFolderContent(Carpeta* folder) {
    thisFolderContent = folder;
    lastSelectedFolder = folder;
}

void TreeFilesInterface::solicitarActualizacion() { actualizar = true; }
