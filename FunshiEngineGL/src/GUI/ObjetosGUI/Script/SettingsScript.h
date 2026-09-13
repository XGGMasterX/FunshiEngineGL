#ifndef SETTINGSSCRIPT_H
#define SETTINGSSCRIPT_H
#include "../SettingsComponent.h"
class GameObject;
class Script;

class SettingsScript : public SettingsComponent {
protected:
	Script* myScript;
public:
	SettingsScript(GameObject* objeto);

	// CONFIGURAR EL SISTEMA PARA QUE AL ACTIVAR SEA UN GHOST BODY, FALSO UN COMUN BODY
	void showDataComponent() override;
	Component* getComponent() override;
};
#endif