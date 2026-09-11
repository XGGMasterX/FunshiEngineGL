#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include <iostream>
#include <memory>
#include <cstdlib>
#include "../GUI/MenusGUI/MenuInterface.h"
#include "../GUI/SceneGUI/SceneSelectedInterface.h"
#include "../GUI/FileManagerGUI/TreeFilesInterface.h"
#include "../GUI/SceneGUI/SceneMenuBarInterface.h"
#include "../Estructuras/ListasEnlazadas/ListasConPrioridad/PriorityListaDE.h"

// Forward declarations
class SceneRegistry;
class EditorController;
class EventBus;
class PhysicsEngine;

using namespace std;

class GUIManager {
private:
	// Use unique_ptr for owned resources
	std::unique_ptr<MenuInterface> menuGUI;
	std::unique_ptr<SettingsObjectInterface> settingGUI;
	std::unique_ptr<SceneSelectedInterface> selecteableGUI;
	std::unique_ptr<SceneMenuBarInterface> menuBarGUI;
	std::unique_ptr<TreeFilesInterface> treeFilesGUI;
	PhysicsEngine* phisics = nullptr;
	EditorController* editor = nullptr;
	std::unique_ptr<ContentFolderInterface> contentOfThisFolder;

public:
	GUIManager(GLFWwindow* window);
	~GUIManager();

	void setPhysics(PhysicsEngine* phisics);
	void bindScene(SceneRegistry* scene, EditorController* editor, EventBus* events);

	MenuInterface* getMenuGUI();
	TreeFilesInterface* getTreeFilesGUI();
	SettingsObjectInterface* getSettingGUI(GameObject* gameObject);
	void removeSettingsGUI();
	void setSelecteableGUI(PriorityListaDE<GameObject*>* gameObjects);
	SceneMenuBarInterface* getMenuBarGUI(bool* targetBool);
	SceneSelectedInterface* getSelecteableGUI();
	void setContentFolderGUI();
	ContentFolderInterface* getContentFolderGUI();
};
#endif
