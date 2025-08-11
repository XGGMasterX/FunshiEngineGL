#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include <iostream>
#include <stdlib.h>
#include "../GUI/MenusGUI/MenuInterface.h"
#include "../GUI/SceneGUI/SceneSelectedInterface.h"
#include "../GUI/FileManagerGUI/TreeFilesInterface.h"
#include "../GUI/SceneGUI/SceneMenuBarInterface.h"

using namespace std;


//Crea las GUI encapsulando la generacion de instancias
//Retorana una GUI generica pero con acceso a los metodos genericos
//uno llama a esos metodos y sabe lo que recibe por el nombre del metodo

//Crear las instancias mediante los metodos
//una vez echo esto asignarlos asi se mantiene unicicidad y coherencia
//metodos getters para leer lass GUI
class GUIManager {
private:
	
	
	//LosMenuDelMotor
	MenuInterface* menuGUI;
	//UltimoSettingsObjectInterface
	SettingsObjectInterface* settingGUI;

	//SelecteableDeGameObjects,GizmosYScenes
	//adaptar luego para todos las ENTIDADES
	SceneSelectedInterface* selecteableGUI;
	SceneMenuBarInterface* menuBarGUI;


	TreeFilesInterface* treeFilesGUI;
	//Armar GUI de paths de proyecto
	//Armar GUI para opciones
	//Armar GUI para Manejo de Objetos
	//Actualizar para admitir desplazamiento de archivos y carga dinamica
	PhysicsEngine* phisics;
	ContentFolderInterface* contentOfThisFolder;
public:
	GUIManager(GLFWwindow* window){
		menuGUI = new MenuInterface(window,true);
		selecteableGUI = new SceneSelectedInterface(true);
		settingGUI = new SettingsObjectInterface(new Modelos3D(),false);
		treeFilesGUI = new TreeFilesInterface(true, "C:/MotorGraficoArchivos");
		contentOfThisFolder = new ContentFolderInterface(false);
		menuBarGUI = new SceneMenuBarInterface(true);
	}

	void setPhysics(PhysicsEngine* phisics) {
		this->phisics = phisics;
	}

	MenuInterface* getMenuGUI() {
		return menuGUI;
	}

	TreeFilesInterface* getTreeFilesGUI() {
		return treeFilesGUI;
	}

	
	SettingsObjectInterface* getSettingGUI(GameObject* gameObject) {
		if (gameObject != nullptr && settingGUI->getObjectInInspector() == gameObject) {
			settingGUI->setPhysics(phisics);
			settingGUI->setStateGui(true);
			return settingGUI;
		}
		else if(gameObject != nullptr){
			settingGUI->setPhysics(phisics);
			settingGUI->setStateGui(false);
			settingGUI = new SettingsObjectInterface(gameObject, true);
			return settingGUI;
		}
	}

	void removeSettingsGUI() {
		settingGUI->setStateGui(false);
	}

	//metodos para agregar con todas las estructuras de entidades
	void setSelecteableGUI(PriorityListaDE<GameObject*>* gameObjects) {
		selecteableGUI->setEntitys(gameObjects);
	}
	
	SceneMenuBarInterface* getMenuBarGUI(bool* targetBool) {
		menuBarGUI->setActivador(targetBool);
		return menuBarGUI;
	}

	SceneSelectedInterface* getSelecteableGUI() {
		return selecteableGUI;
	}

	void setContentFolderGUI() {
		if (treeFilesGUI->getFolderContent() == nullptr) {
			contentOfThisFolder->setStateGui(false);
		}
		else {
			contentOfThisFolder->setFolderRoot(treeFilesGUI->getFolderContent());
			contentOfThisFolder->setStateGui(true);
		}

	}
	
	ContentFolderInterface* getContentFolderGUI(){
		return contentOfThisFolder;
	}


};
#endif

