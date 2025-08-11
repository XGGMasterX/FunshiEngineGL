#ifndef SCENESELECTEDINTERFACE_H
#define SCENESELECTEDINTERFACE_H
#include <iostream>
#include <string>
#include "../ObjetosGUI/SettingsObjectInterface.h"
#include "../../Estructuras/Trees/ArbolesEnlazados/ArbolEnlazado.h"
#include "../../Objetos/Modelos3D.h"



using namespace std;

//ADAPTAR DE GAMEOBJECT A PRIORITY DE ENTITY
class SceneSelectedInterface : public GeneralUserInterface {
protected:
	ArbolEnlazado<GameObject*>* entitys;
	ListaDE<GameObject*>* gameObjects;
	GameObject* returneableObject;
	char inputImGuiString[128] = "";
	int inputImGuiID;
	bool deleteObject = false;
	//CREAR LOS PRE FABRICADOS
public:
	SceneSelectedInterface(bool stateGUI) :
		GeneralUserInterface("SelectedObjects", stateGUI , ImGuiWindowFlags_MenuBar) {
		entitys = new ArbolEnlazado<GameObject*>();
		entitys->createRoot(new Modelos3D());
		gameObjects = new ListaDE<GameObject*>();
		returneableObject = nullptr;
	}
	

	virtual void setEntitys(ListaDE<GameObject*>* gameObjects) {
		this->gameObjects = gameObjects;
	}

	
	virtual GameObject* getReturnableEntity() {
		return returneableObject;
	}

	virtual void initGUI() override {
		ImGui::Begin(getNameGui().c_str(), &stateGUI, getFlagGui());
		ImGui::PushID(this);
	}

	void drawPreOrder(Position<GameObject*>* pos) {
		if (pos != entitys->rootOfTree()) {
			// Mostrar el nombre o ID del GameObject
			std::string label = "Object:" + std::to_string(pos->getElement()->getId());

			bool clicked = ImGui::Selectable(label.c_str(), returneableObject == pos->getElement());
			if (clicked) {
				returneableObject = pos->getElement();
			}

			// Drag source
			if (ImGui::BeginDragDropSource()) {
				Position<GameObject*>* dragged = pos;
				ImGui::SetDragDropPayload("ENTITY_NODE", &dragged, sizeof(dragged));
				ImGui::Text("Moviendo %s", label.c_str());
				ImGui::EndDragDropSource();
			}

			// Drop target
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_NODE")) {
					Position<GameObject*>* dragged;
					memcpy(&dragged, payload->Data, sizeof(dragged));
					if (dragged != pos) {
						entitys->positionToChildOf(pos, dragged);
					}
				}
				ImGui::EndDragDropTarget();
			}
		}

		// Si el nodo tiene hijos, los dibujamos con indentación
		if (entitys->isInternal(pos)) {
			ImGui::Indent(); // Aumenta la indentación visual
			ListaDE<Position<GameObject*>*>* hijos = entitys->childsOf(pos);
			Position<Position<GameObject*>*>* position = hijos->first();
			while (position != nullptr) {
				drawPreOrder(position->getElement());
				position = (position != hijos->last()) ? hijos->next(position) : nullptr;
			}
			ImGui::Unindent(); // Vuelve la indentación
		}
	}



	virtual void contentGUI() override {
		if (!entitys->isEmpty()) {
			if (deleteObject) {
				ImGui::InputInt("Id", &inputImGuiID);
				if (ImGui::Button("Delete"))
				{
					deleteObjectByID(inputImGuiID);
					deleteObject = false;
				}
			}
			drawPreOrder(entitys->rootOfTree());
		}

		if (ImGui::BeginPopupContextWindow("SelectedEntitysPopup", ImGuiPopupFlags_MouseButtonRight)) {
			if (ImGui::MenuItem("New Object")) {
               
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
				deleteObject = true;
			}
			ImGui::EndPopup();
		}
	}

	void createGameObject(GameObject* newGameObject) {
		if (gameObjects->isEmpty()) {
			newGameObject->setId(0);
		}
		else {
			newGameObject->setId(gameObjects->last()->getElement()->getId() + 1);
		}
		gameObjects->addLast(newGameObject);
		entitys->addNodeChildOf(entitys->rootOfTree(), newGameObject);
	}

	//EMULAR PARA ENTIDADES Y OTROS

	bool deleteObjectByID(int id) {
		bool encontre = false;
		if (!gameObjects->isEmpty() && !entitys->isEmpty()) {
			Position<GameObject*>* pos = gameObjects->first();
			while (pos != nullptr && !encontre) {
				if (pos->getElement()->getId() == id) {
					gameObjects->remove(pos);
					encontre = true;
				}
				else {
					pos = (pos != gameObjects->last()) ? gameObjects->next(pos) : nullptr;
				}
			}
			if (encontre) {
				//QUITAR SI ESTA EN ENTITY , SINO VARIAR ENCONTRE A FALSO
			}
		}
		return encontre;
	}

	virtual ListaDE<GameObject*>* getGameObjects() {
		return gameObjects;
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