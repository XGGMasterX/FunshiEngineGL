#ifndef SETTINGSMATERIAL_H
#define SETTINGSMATERIAL_H
#include "../SettingsComponent.h"
class GameObject;
class Material;

class SettingsMaterial : public SettingsComponent {
	GameObject* gameObject;
public:
	SettingsMaterial(GameObject* gameObject);

	void showDataComponent() override;
	Component* getComponent() override;
};
#endif