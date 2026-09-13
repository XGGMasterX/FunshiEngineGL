#ifndef SETTINGSCOLLIDERESFERA_H
#define SETTINGSCOLLIDERESFERA_H
#include "../SettingsComponent.h"
class GameObject;
class SettingsTransform;
class EsfereCollider;

class SettingsColliderEsfera : public SettingsComponent {
protected:
	SettingsTransform* settingsTransform;
	EsfereCollider* myCollider;
	float newRadio;
public:
	SettingsColliderEsfera(GameObject* objeto);
	~SettingsColliderEsfera();

	void showDataComponent() override;
	Component* getComponent() override;
};
#endif