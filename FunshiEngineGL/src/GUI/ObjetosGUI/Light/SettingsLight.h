#ifndef SETTINGSLIGHT_H
#define SETTINGSLIGHT_H
#include "../SettingsComponent.h"
class GameObject;
class Light;

class SettingsLight : public SettingsComponent {
	GameObject* gameObject;
public:
	SettingsLight(GameObject* gameObject);

	void showDataComponent() override;
	Component* getComponent() override;
};
#endif