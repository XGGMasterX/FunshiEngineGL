#ifndef SETTINGSMODEL_H
#define SETTINGSMODEL_H
#include "../SettingsComponent.h"
class GameObject;
class Model;

class SettingsModel : public SettingsComponent {
protected:
	Model* myModel;
public:
	SettingsModel(GameObject* objeto);

	// CONFIGURAR EL SISTEMA PARA QUE AL ACTIVAR SEA UN GHOST BODY, FALSO UN COMUN BODY
	void showDataComponent() override;
	Component* getComponent() override;
};
#endif