#ifndef SETTINGSCOLLIDERMALLA_H
#define SETTINGSCOLLIDERMALLA_H
#include "../SettingsComponent.h"
class GameObject;
class SettingsTransform;
class MallaCollider;

class SettingsColliderMalla : public SettingsComponent {
protected:
	SettingsTransform* settingsTransform;
	MallaCollider* myCollider;
	float newRadio;
public:
	SettingsColliderMalla(GameObject* objeto);
	~SettingsColliderMalla();

	void showDataComponent() override;
	Component* getComponent() override;
};
#endif