#ifndef SCENESELECTEDINTERFACE_H
#define SCENESELECTEDINTERFACE_H
#include <iostream>
#include <string>
#include <memory>
#include "../ObjetosGUI/SettingsObjectInterface.h"
#include "../../Estructuras/Trees/ArbolesEnlazados/ArbolEnlazado.h"
#include "../../Objetos/Modelos3D.h"
#include "SceneObjectTree.h"

using namespace std;

// Forward declarations
class SceneRegistry;
class EditorController;
class EventBus;
class IconosGUI;

// Ventana "SelectedObjects": jerarquia de la escena.
//
// Es una ventana fina: la logica del arbol vive en el widget reutilizable
// SceneObjectTree y la seleccion en el EditorController (unica fuente de
// verdad, publicada via EventBus).
class SceneSelectedInterface : public GeneralUserInterface {
protected:
	// Scene integration
	SceneRegistry* scene = nullptr;
	EditorController* editor = nullptr;

	// Widget reutilizable de jerarquia (contenido de la ventana)
	SceneObjectTree sceneTree;

public:
	SceneSelectedInterface(bool stateGUI);

	void bindScene(SceneRegistry* value, EditorController* controller, EventBus* bus);
	void setIconosGUI(IconosGUI* iconosG) { sceneTree.setIconosGUI(iconosG); }

	virtual ArbolEnlazado<GameObject*>* getEntitysTree();
	virtual void setEntitys(ListaDE<GameObject*>* gameObjects);
	virtual GameObject* getReturnableEntity();
	virtual void setReturnableEntity(GameObject* object);
	virtual ListaDE<GameObject*>* getGameObjects();

	virtual void initGUI() override;
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