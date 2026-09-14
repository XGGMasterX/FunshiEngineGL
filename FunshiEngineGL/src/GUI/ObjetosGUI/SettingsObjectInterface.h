#ifndef SETTINGSOBJECTINTERFACE_H
#define SETTINGSOBJECTINTERFACE_H

#include "../GeneralUserInterface.h"
#include "../../Estructuras/ListasEnlazadas/ListasDoblementeEnlazada/ListaDE.h"

class GameObject;
class SettingsComponent;
class EditorController;

class SettingsObjectInterface : public GeneralUserInterface {
private:
	GameObject* object;
	ListaDE<SettingsComponent*>* listaDESettingsComponent;
	EditorController* editor = nullptr;
	int momentaneantID = 0;

public:
	SettingsObjectInterface(GameObject* object, bool stateGUI);
	~SettingsObjectInterface();

	void setEditor(EditorController* editor);
	void loadComponents();

	// Cambia el objeto inspeccionado sin recrear la ventana: limpia y recarga
	// solo el contenido (mismo patron que ContentFolderInterface).
	void setTargetObject(GameObject* newObject);

	virtual GameObject* getObjectInInspector();

	virtual void initGUI() override;
	virtual void contentGUI() override;
	virtual void endGUI() override;
	virtual void printGUI() override;
};
#endif