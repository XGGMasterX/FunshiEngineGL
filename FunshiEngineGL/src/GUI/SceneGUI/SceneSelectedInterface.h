#ifndef SCENESELECTEDINTERFACE_H
#define SCENESELECTEDINTERFACE_H
#include <iostream>
#include <string>
#include <memory>
#include "../ObjetosGUI/SettingsObjectInterface.h"
#include "../../Estructuras/Trees/ArbolesEnlazados/ArbolEnlazado.h"
#include "../../Objetos/Modelos3D.h"
#include "../../Events/EventBus.h"

using namespace std;

// Forward declarations
class SceneRegistry;
class EditorController;
class PhysicsEngine;

//ADAPTAR DE GAMEOBJECT A PRIORITY DE ENTITY
class SceneSelectedInterface : public GeneralUserInterface {
protected:
	GameObject* returneableObject = nullptr;
	char inputImGuiString[128] = "";
	int inputImGuiID = 0;
	bool deleteObject = false;

	// Scene integration
	SceneRegistry* scene = nullptr;
	EditorController* editor = nullptr;
	EventBus* events = nullptr;
	size_t eventSubscription = 0;

public:
	SceneSelectedInterface(bool stateGUI);
	virtual ~SceneSelectedInterface();

	void bindScene(SceneRegistry* value, EditorController* controller, EventBus* bus);
	void setPhysics(PhysicsEngine* physics);

	virtual ArbolEnlazado<GameObject*>* getEntitysTree();
	virtual void setEntitys(ListaDE<GameObject*>* gameObjects);
	virtual GameObject* getReturnableEntity();
	virtual void setReturnableEntity(GameObject* object);
	virtual ListaDE<GameObject*>* getGameObjects();

	virtual void initGUI() override;
	void drawPreOrder(Position<GameObject*>* pos);
	virtual void contentGUI() override;
	virtual void endGUI() override;
	virtual void printGUI() override;

	void createGameObject(GameObject* newGameObject);
	void createGameObject(std::unique_ptr<GameObject> object);
	void addChildGameObject(GameObject* parent, GameObject* object);
	void replaceRootGameObject(GameObject* newRoot);
	void clearGameObjects();
	void refreshGameObjectView();
	bool deleteObjectByID(int id);
};
#endif
