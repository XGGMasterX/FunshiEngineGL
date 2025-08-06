#ifndef SETTINGSOBJECTINTERFACE_H
#define SETTINGSOBJECTINTERFACE_H

#include "../GeneralUserInterface.h"
#include "SettingsComponent.h"
#include "../../Objetos/GameObject.h"
#include "../../Estructuras/ListasEnlazadas/ListasDoblementeEnlazada/ListaDE.h"
#include "SettingsTransform.h"
#include "SettingsColor.h"

class SettingsObjectInterface : public GeneralUserInterface {
private:
	GameObject* object;
	ListaDE<SettingsComponent*>* listaDESettingsComponent;

public:
	// Constructor que recibe la ventana GLFW
	SettingsObjectInterface(GameObject* object,bool stateGUI) :
		GeneralUserInterface("Settings", stateGUI,
			ImGuiWindowFlags_MenuBar) {
		this->object = object;
		listaDESettingsComponent = new ListaDE<SettingsComponent*>();


		//agregando el componente transform , mejorar arquitectura para recorrer getComponents
		SettingsComponent* settingsTransform = new SettingsTransform(object->getComponent<Transform>());
		listaDESettingsComponent->addLast(settingsTransform);

		SettingsComponent* settingsColor = new SettingsColor(object);
		listaDESettingsComponent->addLast(settingsColor);
	}

	virtual GameObject* getObjectInInspector() {
		return object;
	}

	virtual void initGUI() override  {
		ImGui::Begin(getNameGui().c_str(), &stateGUI, getFlagGui());
		ImGui::PushID(object);
	}

	virtual void contentGUI() override {

		ImGui::InputInt("Id", &object->inputId);
		if (ImGui::Button("Confirmar")) {
			object->setId(object->inputId);
		}
		

		ImGui::InputText("Nombre", object->inputName, IM_ARRAYSIZE(object->inputName));
        
		//pintar los settingsComponent
		Position<SettingsComponent*>* position = listaDESettingsComponent->first();
		while (position != nullptr && position->getElement() != nullptr) {
			position->getElement()->showDataComponent();
			position = (position != listaDESettingsComponent->last()) ? listaDESettingsComponent->next(position) : nullptr;
		}

	}

	virtual void endGUI() override {
		ImGui::PopID();
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












