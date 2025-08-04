#ifndef SETTINGSOBJECTINTERFACE_H
#define SETTINGSOBJECTINTERFACE_H
#include "../GeneralUserInterface.h"
#include "SettingsComponent.h"
#include "../../Objetos/GameObject.h"
#include "../../Estructuras/ListasEnlazadas/ListasDoblementeEnlazada/ListaDE.h"

class SettingsObjectInterface : public GeneralUserInterface {
private:
	GameObject* object;
	ListaDE<SettingsComponent*>* listaDESettingsComponent;

public:
	// Constructor que recibe la ventana GLFW
	SettingsObjectInterface(GameObject* object, bool stateGUI) :
		GeneralUserInterface("Settings", stateGUI,
			ImGuiWindowFlags_MenuBar) {
		this->object = object;
	}

	virtual void initGUI() override  {
		ImGui::Begin(getNameGui().c_str(), &stateGUI, getFlagGui());
		ImGui::PushID(object);
		ImGui::Begin("Settings");
	}

	virtual void contentGUI() override {
		ImGui::InputInt("Id", &object->inputId);
		object->setId(object->inputId);

		ImGui::InputText("Nombre", object->inputName, IM_ARRAYSIZE(object->inputName));


		//llamar los component


		ImGui::Text("===Texture===");
		//TODO
		ImGui::Button("Texture");
		//crear componente
		//quitar luego :
		ImGui::Text("===ObjectTypeFormat===");
		ImGui::Checkbox("Lines", &object->lines);
		if (object->lines) {
			object->points = false;
			object->dense = false;
		}
		ImGui::Checkbox("Points", &object->points);
		if (object->points) {
			object->lines = false;
			object->dense = false;
		}
		ImGui::Checkbox("Dense", &object->dense);
		if (object->dense) {
			object->points = false;
			object->lines = false;
		}


	}

	virtual void endGUI() override {
		ImGui::PopID();
		ImGui::End();
	}
	
	virtual void printGUI() override {
		//Testing 
		if (object->getState() && object->activeComponentSettingsObject) {
			initGUI();
			contentGUI();
			endGUI();
		}
	}
};
#endif












