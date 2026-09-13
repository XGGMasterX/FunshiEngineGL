#ifndef SETTINGSCOLOR_H
#define SETTINGSCOLOR_H
#include "../SettingsComponent.h"
class GameObject;
class Color;

class SettingsColor : public SettingsComponent {
	GameObject* gameObject;
public:
	SettingsColor(GameObject* gameObject);

	void showDataComponent() override;
	Component* getComponent() override;
};
#endif