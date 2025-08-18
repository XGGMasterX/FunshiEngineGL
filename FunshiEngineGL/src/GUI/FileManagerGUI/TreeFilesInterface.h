#ifndef TREEFILESINTERFACE_H
#define TREEFILESINTERFACE_H
#include <iostream>
#include "../GeneralUserInterface.h"
#include "../../Estructuras/Trees/ArbolesEnlazados/ArbolEnlazado.h"
#include "../../GestorDeArchivos/GestorDeArchivos.h"
#include "../../GUI/FileManagerGUI/ContentFolderInterface.h"
#include "../../GestorDeArchivos/Carpeta.h"

using namespace std;

// Definir separador de rutas según plataforma
#ifdef _WIN32
    const std::string PATH_SEP = "\\";
#else
    const std::string PATH_SEP = "/";
#endif

class TreeFilesInterface : public GeneralUserInterface {
protected:
    bool actualizar;
    Carpeta* thisFolderContent;
    string pathProyect;
    ArbolEnlazado<File*>* arbolDeArchivos;
    GestorDeArchivos* gestorDeArchivos;
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    Carpeta* lastSelectedFolder;

public:
    TreeFilesInterface(bool stateGUI, string pathProyect) :
        GeneralUserInterface("BrowseFile", stateGUI, ImGuiWindowFlags_MenuBar),
        actualizar(false),
        thisFolderContent(nullptr),
        pathProyect(pathProyect),
        arbolDeArchivos(nullptr),
        gestorDeArchivos(nullptr),
        lastSelectedFolder(nullptr)
    {
        gestorDeArchivos = new GestorDeArchivos(pathProyect);
        arbolDeArchivos = gestorDeArchivos->getTreeFilePath();
    }

    virtual void preOrdenOfTreeFile(Position<File*>* root) {
        if (!root) return;

        Carpeta* folderRoot = dynamic_cast<Carpeta*>(root->getElement());
        if (!folderRoot) return;

        std::string idStr = folderRoot->getPathRoot() + PATH_SEP + folderRoot->getPathName();
        ImGui::PushID(idStr.c_str());

        bool isSelected = (lastSelectedFolder == folderRoot);
        ImGuiTreeNodeFlags nodeFlags = isSelected ? (flags | ImGuiTreeNodeFlags_Selected) : flags;

        bool nodeOpen = ImGui::TreeNodeEx(folderRoot->getPathName().c_str(), nodeFlags, "%s", folderRoot->getPathName().c_str());

        // Manejo de clics
        if (ImGui::IsItemClicked()) {
            lastSelectedFolder = folderRoot;
            thisFolderContent = folderRoot;
        }

        // Menú contextual
        if (ImGui::BeginPopupContextItem("MenuContextualCarpeta")) {
            ImGui::Text("Carpeta: %s", folderRoot->getPathName().c_str());
            ImGui::Separator();

            if (ImGui::MenuItem("Nueva Carpeta")) {
                string nombreNuevaCarpeta = "Nueva Carpeta";
                string rutaNuevaCarpeta = folderRoot->getPathRoot() + PATH_SEP +
                                          folderRoot->getPathName() + PATH_SEP + nombreNuevaCarpeta;

                if (gestorDeArchivos->crearCarpeta(rutaNuevaCarpeta)) {
                    Carpeta* nuevaCarpeta = new Carpeta(nombreNuevaCarpeta);
                    nuevaCarpeta->setPathRoot(rutaNuevaCarpeta);
                    Position<File*>* posicionPadre = arbolDeArchivos->whatIsPositionOf(folderRoot);
                    if (posicionPadre) {
                        arbolDeArchivos->addNodeChildOf(posicionPadre, nuevaCarpeta);
                        actualizar = true;
                    }
                }
            }

            if (ImGui::MenuItem("Eliminar Carpeta")) {
                string rutaCarpeta = folderRoot->getPathRoot() + PATH_SEP + folderRoot->getPathName();
                Position<File*>* posicionAEliminar = arbolDeArchivos->whatIsPositionOf(folderRoot);

                if (posicionAEliminar && gestorDeArchivos->eliminarCarpeta(rutaCarpeta)) {
                    // Limpiar estados antes de eliminar
                    if (lastSelectedFolder == folderRoot) {
                        lastSelectedFolder = nullptr;
                        thisFolderContent = nullptr;
                    }

                    // Eliminar el nodo
                    File* deletedNode = arbolDeArchivos->deleteNode(posicionAEliminar);

                    // No usar folderRoot después de deleteNode!
                    actualizar = true;

                    // Cerrar el nodo si estaba abierto
                    if (nodeOpen) {
                        ImGui::TreePop();
                        nodeOpen = false;
                    }

                    // Salir temprano ya que el nodo fue eliminado
                    ImGui::EndPopup();
                    ImGui::PopID();
                    return;
                }
            }

            ImGui::EndPopup();
        }

        // Mostrar hijos si el nodo está abierto
        if (nodeOpen) {
            if (arbolDeArchivos->isInternal(root)) {
                ListaDE<Position<File*>*>* childsOfFolder = arbolDeArchivos->childsOf(root);
                Position<Position<File*>*>* position = childsOfFolder->first();
                while (position != nullptr) {
                    preOrdenOfTreeFile(position->getElement());
                    position = (position != childsOfFolder->last()) ? childsOfFolder->next(position) : nullptr;
                }
            }
            ImGui::TreePop();
        }

        ImGui::PopID();
    }

    virtual void initGUI() override {
        ImGui::Begin(getNameGui().c_str(), &stateGUI, getFlagGui());
    }

    virtual void contentGUI() override {
        actualizar = gestorDeArchivos->setTreeFilePath(pathProyect,"MotorGrafico"); // Carga un árbol por cambios
        if (actualizar) {
            arbolDeArchivos = gestorDeArchivos->getTreeFilePath(); // Obtiene el árbol
        }
        if(!arbolDeArchivos->isEmpty()){
            Position<File*>* root = arbolDeArchivos->rootOfTree();
            if(arbolDeArchivos->isInternal(root)){
                ListaDE<Position<File*>*>* childsOfRoot = arbolDeArchivos->childsOf(root);
                Position<Position<File*>*>* position = childsOfRoot->first();
                while(position != nullptr){
                    preOrdenOfTreeFile(position->getElement());
                    position = (position != childsOfRoot->last()) ? childsOfRoot->next(position) : nullptr;
                }
            }
        }
    }

    virtual void endGUI() override {
        ImGui::End();
    }

    virtual void printGUI() override {
        if (stateGUI) {
            initGUI();
            contentGUI();
            endGUI();
        }
    }

    Carpeta* getFolderContent() {
        return lastSelectedFolder;
    }

    void setFolderContent(Carpeta* thisFolderContent) {
        this->thisFolderContent = thisFolderContent;
    }
};
#endif
