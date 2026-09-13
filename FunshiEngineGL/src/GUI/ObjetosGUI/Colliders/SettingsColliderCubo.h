#ifndef SETTINGSCOLLIDERCUBO_H
#define SETTINGSCOLLIDERCUBO_H
#include "../SettingsComponent.h"
class GameObject;
class SettingsTransform;
class CubeCollider;

class SettingsColliderCubo : public SettingsComponent {
protected:
	SettingsTransform* settingsTransform;
	CubeCollider* myCollider;
	float newRadio;
public:
	SettingsColliderCubo(GameObject* objeto);
	~SettingsColliderCubo();

	void showDataComponent() override;
	Component* getComponent() override;
};
#endif