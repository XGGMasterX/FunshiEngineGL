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
    Folder* thisFolderContent;
    string pathProyect;
    ArbolEnlazado<File*>* arbolDeArchivos;
    GestorDeArchivos* gestorDeArchivos;
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    Folder* lastSelectedFolder; // Nuevo: Para manejar la selección

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
        if (Folder* folderRoot = dynamic_cast<Folder*>(root->getElement())) {
            std::string idStr = folderRoot->getPathRoot() + "\\" + folderRoot->getPathName();
            ImGui::PushID(idStr.c_str());

            // Modificado: Usar flags de selección si es el folder seleccionado
            bool isSelected = (lastSelectedFolder == folderRoot);
            bool open = ImGui::TreeNodeEx(folderRoot->getPathName().c_str(),
                isSelected ? (flags | ImGuiTreeNodeFlags_Selected) : flags,
                "%s", folderRoot->getPathName().c_str());

            // Modificado: Manejar clic para selección
            if (ImGui::IsItemClicked()) {
                lastSelectedFolder = folderRoot;
                thisFolderContent = folderRoot;
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
		actualizar = gestorDeArchivos->setTreeFilePath(pathProyect); //carga un arbol por cambios
		if (actualizar) {
			arbolDeArchivos = gestorDeArchivos->getTreeFilePath(); //obtiene el arbol
		}
		Position<File*>* root = arbolDeArchivos->rootOfTree();
		preOrdenOfTreeFile(root); //lee el arbol
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

	Folder* getFolderContent() {
		return thisFolderContent;
	}

	void setFolderContent(Folder* thisFolderContent) {
		this->thisFolderContent = thisFolderContent;
	}
};
#endif