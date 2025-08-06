#ifndef SCENESELECTEDINTERFACE_H
#define SCENESELECTEDINTERFACE_H
#include <iostream>
#include <string>
#include "../ObjetosGUI/SettingsObjectInterface.h"

using namespace std;

//ADAPTAR DE GAMEOBJECT A PRIORITY DE ENTITY
class SceneSelectedInterface : public GeneralUserInterface {
protected:
	PriorityListaDE<GameObject*>* Entitys;
	//otras listas de otras entidades
	GameObject* returneableObject;

public:
	SceneSelectedInterface(bool stateGUI) :
		GeneralUserInterface("SelectedObjects", stateGUI , ImGuiWindowFlags_MenuBar) {
		Entitys = new PriorityListaDE<GameObject*>(nullptr);
		returneableObject = nullptr;
	}

	virtual void setEntitys(PriorityListaDE<GameObject*>* Entitys) {
		this->Entitys = Entitys;
	}

	//ARMAR TODO EN UNO
	virtual GameObject* getReturnableEntity() {
		return returneableObject;
	}

	virtual void initGUI() override {
		ImGui::Begin(getNameGui().c_str(), &stateGUI, getFlagGui());
		ImGui::PushID(this);
	}

	virtual void contentGUI() override {
		//Muestro un botton para cada objeto de la Scene
		//Si se preciona uno entonces ese objeto se obtiene y se da al GUIManager
		//obtengo el Settings y lo pinto en Scene
		if (!Entitys->isEmpty()) {
			Position<GameObject*>* position = Entitys->first();
			GameObject* object;
			while (position != nullptr) {
				object = position->getElement();
				if (ImGui::Selectable(("Object" + to_string(object->getId())).c_str())) {
					returneableObject = object;
				}
				position = (position != Entitys->last()) ? Entitys->next(position) : nullptr;
			}
		}
	}


	virtual void endGUI() override {
		ImGui::PopID();
		ImGui::End();
	}

	virtual void printGUI() override {
		//Testing 
		if (stateGUI) {
			initGUI();
			contentGUI();
			endGUI();
		}
	}
};
#endif