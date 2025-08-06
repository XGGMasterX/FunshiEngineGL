#ifndef TREEFILESINTERFACE_H
#define TREEFILESINTERFACE_H
#include <iostream>
#include "../GeneralUserInterface.h"
#include "../../Estructuras/Trees/ArbolesEnlazados/ArbolEnlazado.h"
#include "../../GestorDeArchivos/GestorDeArchivos.h"
#include "../../GUI/FileManagerGUI/ContentFolderInterface.h"

using namespace std;

//Usa un gestorDeArchivos con pre orden para visualizar el arbol de carpetas que tiene
class TreeFilesInterface : public GeneralUserInterface {
protected:
	bool actualizar;
	string pathProyect;
	ArbolEnlazado<File*>* arbolDeArchivos;
	GestorDeArchivos* gestorDeArchivos;
	ContentFolderInterface* contentOfThisFolder;
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
public:
	TreeFilesInterface(bool stateGUI, string pathProyect) :
		GeneralUserInterface("BrowseFile", stateGUI, ImGuiWindowFlags_MenuBar) {
		this->pathProyect = pathProyect;
		arbolDeArchivos = new ArbolEnlazado<File*>();
		gestorDeArchivos = new GestorDeArchivos(pathProyect);
		arbolDeArchivos = gestorDeArchivos->getTreeFilePath();
		contentOfThisFolder = new ContentFolderInterface(new Folder(""), false);
	}

	virtual void preOrdenOfTreeFile(Position<File*>* root) {
		
		if (Folder* folderRoot = dynamic_cast<Folder*>(root->getElement())) {

			//ME VISITO
			std::string idStr = folderRoot->getPathRoot() + "\\" + folderRoot->getPathName(); // o solo getPathName()
			ImGui::PushID(idStr.c_str());
			bool open = ImGui::TreeNodeEx(folderRoot->getPathName().c_str(), flags, "%s", folderRoot->getPathName().c_str());

			if (open) {
			    //CONTENIDO DEL FOLDER ACTUAL
			    if (contentOfThisFolder->getFolderContent() == folderRoot) {
					contentOfThisFolder->printGUI();
				}
				else {
					contentOfThisFolder->setStateGui(false);
					contentOfThisFolder = new ContentFolderInterface(folderRoot,true);
					contentOfThisFolder->printGUI();
				}


			  //PINTANDO EL ARBOL DE FOLDERS
			  if(arbolDeArchivos->isInternal(root)){
				ListaDE<Position<File*>*>* childsOfFolder = arbolDeArchivos->childsOf(root);
				Position<Position<File*>*>* position = childsOfFolder->first();
				//VISITO MIS HIJOS
				while (position != nullptr) {
					preOrdenOfTreeFile(position->getElement());
					position = (position != childsOfFolder->last()) ? childsOfFolder->next(position) : nullptr;
				}
				
			  }
			  ImGui::TreePop();

		    }
			ImGui::PopID();
		}
		//SI NO ES FOLDER SIGO NOMAS
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
};
#endif