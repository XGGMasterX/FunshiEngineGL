#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include <iostream>
#include <conio.h>
#include <stdlib.h>

//#define GLFW_INCLUDE_VULKAN
//#include <glfw3.h>
//HERRAMIENTAS ADICIONALES
//#include "ListaDE.h"
//TODAS LAS GUI
//#include "ContentFolderInterface.h"
#include "../GUI/MenusGUI/MenuInterface.h"
//#include "SceneSelectedInterface.h"
//#include "SettingsObjetInterface.h"
//#include "TreeFilesInterface.h"

using namespace std;


//Crea las GUI encapsulando la generacion de instancias
//Retorana una GUI generica pero con acceso a los metodos genericos
//uno llama a esos metodos y sabe lo que recibe por el nombre del metodo
class GUIManager {
private:
	//ContentFolderInterface* contentGUI;
	MenuInterface* menuGUI;
	//SceneSelectedInterface* sceneGUI;
	//ListaDE<SettingsObjectInterface*>* settingsGUI;
	//TreeFilesInterface* treeFilesGUI;
	//Armar GUI de paths de proyecto
	//Armar GUI para opciones
	//Armar GUI para Manejo de Objetos
	//Actualizar para admitir desplazamiento de archivos y carga dinamica

public:
	GUIManager(GLFWwindow* window){
		menuGUI = new MenuInterface(window,true);
	}

	MenuInterface* getMenuInterface() {
		return menuGUI;
	}
	//Crear las instancias mediante los metodos
	//una vez echo esto asignarlos asi se mantiene unicicidad y coherencia
	//metodos getters para leer lass GUI
};
#endif

