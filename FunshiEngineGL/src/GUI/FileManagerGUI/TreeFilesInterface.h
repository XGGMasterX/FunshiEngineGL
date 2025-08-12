#ifndef TREEFILESINTERFACE_H
#define TREEFILESINTERFACE_H
#include <iostream>
#include "../GeneralUserInterface.h"
#include "../../Estructuras/Trees/ArbolesEnlazados/ArbolEnlazado.h"
#include "../../GestorDeArchivos/GestorDeArchivos.h"
#include "../../GUI/FileManagerGUI/ContentFolderInterface.h"

using namespace std;

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
        lastSelectedFolder(nullptr) // Inicializar
    {
        this->pathProyect = pathProyect;
        arbolDeArchivos = new ArbolEnlazado<File*>();
        gestorDeArchivos = new GestorDeArchivos(pathProyect);
        arbolDeArchivos = gestorDeArchivos->getTreeFilePath();
    }

    virtual void preOrdenOfTreeFile(Position<File*>* root) {
        if (Carpeta* folderRoot = dynamic_cast<Carpeta*>(root->getElement())) {
            std::string idStr = folderRoot->getPathRoot() + "\\" + folderRoot->getPathName();
            ImGui::PushID(idStr.c_str());

            bool isSelected = (lastSelectedFolder == folderRoot);
            bool open = ImGui::TreeNodeEx(folderRoot->getPathName().c_str(),
                isSelected ? (flags | ImGuiTreeNodeFlags_Selected) : flags,
                "%s", folderRoot->getPathName().c_str());

            // Manejar clic izquierdo y derecho
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
                lastSelectedFolder = folderRoot;
                thisFolderContent = folderRoot;
            }

            // Abrir menú contextual con clic derecho
            if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
                lastSelectedFolder = folderRoot;
                thisFolderContent = folderRoot;
                ImGui::OpenPopup("MenuContextualCarpeta");
            }

            // IMPORTANTE: El popup debe declararse en el mismo ID donde se abre
            if (ImGui::BeginPopup("MenuContextualCarpeta")) {
                ImGui::Text("Carpeta: %s", folderRoot->getPathName().c_str());
                ImGui::Separator();

                if (ImGui::MenuItem("Nueva Carpeta")) {
                    //TERMINAR PARA ABRIR VENTANA DONDE PONER NAME
                    string nombreNuevaCarpeta = "Nueva Carpeta";
                    string rutaNuevaCarpeta = folderRoot->getPathRoot() + "\\" +
                        folderRoot->getPathName() + "\\" + nombreNuevaCarpeta;

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
                    string rutaCarpeta = folderRoot->getPathRoot() + "\\" + folderRoot->getPathName();
                    if (gestorDeArchivos->eliminarCarpeta(rutaCarpeta)) {
                        Position<File*>* posicionAEliminar = arbolDeArchivos->whatIsPositionOf(folderRoot);
                        if (posicionAEliminar) {
                            arbolDeArchivos->deleteNode(posicionAEliminar);
                            actualizar = true;
                            if (lastSelectedFolder == folderRoot) {
                                lastSelectedFolder = nullptr;
                                thisFolderContent = nullptr;
                            }
                        }
                    }
                }

                ImGui::EndPopup();
            }

            if (open) {
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
    }

    virtual void initGUI() override {
        ImGui::Begin(getNameGui().c_str(), &stateGUI, getFlagGui());
    }

    virtual void contentGUI() override {
        actualizar = gestorDeArchivos->setTreeFilePath(pathProyect); // Carga un árbol por cambios
        if (actualizar) {
            arbolDeArchivos = gestorDeArchivos->getTreeFilePath(); // Obtiene el árbol
        }
        Position<File*>* root = arbolDeArchivos->rootOfTree();
        preOrdenOfTreeFile(root);
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
        return thisFolderContent;
    }

    void setFolderContent(Carpeta* thisFolderContent) {
        this->thisFolderContent = thisFolderContent;
    }
};
#endif