#ifndef SCENESELECTEDINTERFACE_H
#define SCENESELECTEDINTERFACE_H
#include <iostream>
#include <string>
#include "../ObjetosGUI/SettingsObjectInterface.h"
char inputImGuiString[128] = "";
int inputImGuiID;

using namespace std;

//ADAPTAR DE GAMEOBJECT A PRIORITY DE ENTITY
class SceneSelectedInterface : public GeneralUserInterface {
protected:
	ListaDE<GameObject*>* Entitys;
	GameObject* returneableObject;
	//CREAR LOS PRE FABRICADOS
public:
	SceneSelectedInterface(bool stateGUI) :
		GeneralUserInterface("SelectedObjects", stateGUI , ImGuiWindowFlags_MenuBar) {
		Entitys = new ListaDE<GameObject*>();
		returneableObject = nullptr;
	}
	

	virtual void setEntitys(ListaDE<GameObject*>* Entitys) {
		this->Entitys = Entitys;
	}

	
	virtual GameObject* getReturnableEntity() {
		return returneableObject;
	}

	virtual void initGUI() override {
		ImGui::Begin(getNameGui().c_str(), &stateGUI, getFlagGui());
		ImGui::PushID(this);
	}

	virtual void contentGUI() override {
		if (!Entitys->isEmpty()) {
			Position<GameObject*>* position = Entitys->first();
			while (position != nullptr) {
				GameObject* object = position->getElement();

				
				string label = "Object " + to_string(object->getId());
				ImGui::PushID(object);
				ImGui::Selectable(label.c_str());

				
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
					ImGui::SetDragDropPayload("GAMEOBJECT_DRAG", &position, sizeof(Position<GameObject*>*));
					ImGui::Text("Mover %s", label.c_str());
					ImGui::EndDragDropSource();
				}

				
				if (ImGui::BeginDragDropTarget()) {
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_DRAG")) {
						Position<GameObject*>* draggedPos = *(Position<GameObject*>**)payload->Data;

						if (draggedPos != position) {
							GameObject* aux = position->getElement();
							Entitys->swapPositions(draggedPos, position);
						}
					}
					ImGui::EndDragDropTarget();
				}

				
				if (ImGui::IsItemClicked()) {
					returneableObject = object;
				}

				ImGui::PopID();
				position = (position != Entitys->last()) ? Entitys->next(position) : nullptr;
			}
		}

		if (ImGui::BeginPopupContextWindow("SelectedEntitysPopup", ImGuiPopupFlags_MouseButtonRight)) {
			if (ImGui::MenuItem("New Object")) {
				//MOSTRAR LISTA PRE DEFINIDA DE OBJETOS
			}
			if (ImGui::MenuItem("New RenderObject")) {
				//MOSTRAR ENTRADA PARA PONER PATH DEL RENDER
				//PERMITIR ARRASTRAR DESDE OTRA GUI (CONTENT GUI)
				Modelos3D* newModelos1 = new Modelos3D("C:/MotorGraficoArchivos/Binarios/BlenderModelos/cubo.obj");//AUTOMATIZAR
				Modelos3D* newModelos2 = new Modelos3D("C:/MotorGraficoArchivos/Binarios/BlenderModelos/monkey.obj");//AUTOMATIZAR
				Modelos3D* newModelos3 = new Modelos3D("C:/MotorGraficoArchivos/Binarios/BlenderModelos/head_fixed.obj");//AUTOMATIZAR
				cout << "Objeto Creado" << endl;
				ImGui::InputText("Path", inputImGuiString, IM_ARRAYSIZE(inputImGuiString), ImGuiInputTextFlags_EnterReturnsTrue);
				createGameObject(newModelos1);
				createGameObject(newModelos2);
				createGameObject(newModelos3);
			}
			if (ImGui::MenuItem("Delete By ID")) {
				ImGui::InputInt("Id", &inputImGuiID);
				if (ImGui::Button("Delete"))
				{
					deleteObjectByID(inputImGuiID);
				}
			}
			ImGui::EndPopup();
		}
	}

	void createGameObject(GameObject* newGameObject) {
		if (Entitys->isEmpty()) {
			newGameObject->setId(0);
		}
		else {
			newGameObject->setId(Entitys->last()->getElement()->getId() + 1);
		}
		Entitys->addLast(newGameObject);
	}

	bool deleteObjectByID(int id) {
		bool encontre = false;
		if (!Entitys->isEmpty()) {
			Position<GameObject*>* pos = Entitys->first();
			while (pos != nullptr && !encontre) {
				if (pos->getElement()->getId() == id) {
					Entitys->remove(pos);
					encontre = true;
				}
				else {
					pos = (pos != Entitys->last()) ? Entitys->next(pos) : nullptr;
				}
			}
		}
		return encontre;
	}

	virtual ListaDE<GameObject*>* getGameObjects() {
		return Entitys;
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