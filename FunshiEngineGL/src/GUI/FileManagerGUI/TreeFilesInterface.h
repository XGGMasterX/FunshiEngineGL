#ifndef TREEFILESINTERFACE_H
#define TREEFILESINTERFACE_H
#include <iostream>
#include "../GeneralUserInterface.h"
#include "../../Estructuras/Trees/ArbolesEnlazados/ArbolEnlazado.h"
#include "../../GestorDeArchivos/Folder.h"
#include "../../GestorDeArchivos/File.h"

using namespace std;

class TreeFilesInterface : public GeneralUserInterface {
protected:
	ArbolEnlazado<File*>* arbolDeArchivos;

public:
	TreeFilesInterface(bool stateGUI) :
		GeneralUserInterface("BrowseFile", stateGUI, ImGuiWindowFlags_MenuBar) {
	}

	virtual void initGUI() override {
		ImGui::Begin(getNameGui().c_str(), &stateGUI, getFlagGui());
	}

	virtual void contentGUI() override {
	}


	virtual void endGUI() override {
	}

	virtual void printGUI() override {
	}
};
#endif