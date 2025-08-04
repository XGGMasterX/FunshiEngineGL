#ifndef SCENESELECTEDINTERFACE_H
#define SCENESELECTEDINTERFACE_H
#include <iostream>
#include <string>
#include "../GeneralUserInterface.h"
#include "../../Objetos/GameObject.h"

using namespace std;

class SceneSelectedInterface : public GeneralUserInterface {
protected:
	GameObject* object;

public:
	SceneSelectedInterface(GameObject* object, bool stateGUI) : 
		GeneralUserInterface("SelectedObjects", stateGUI , ImGuiWindowFlags_MenuBar) {
		this->object = object;
	}

	virtual void initGUI() override {
		ImGui::Begin(getNameGui().c_str(), &stateGUI, getFlagGui());
		ImGui::PushID(object);


	}

	virtual void contentGUI() override {
		bool isSelected; //veo si ya fue seleccionado
		if (ImGui::Selectable(("Object" + to_string(object->getId())).c_str(), isSelected)) {
				// Lógica de selección/desactivación
		}
		
	}


	virtual void endGUI() override {
		ImGui::PopID();
		ImGui::End();
	}

	virtual void printGUI() override {
		//Testing 
		if (object->getState()) {
			initGUI();
			contentGUI();
			endGUI();
		}
	}
};
#endif