#ifndef SETTINGSCOLLIDERCUBO_H
#define SETTINGSCOLLIDERCUBO_H
#include "../SettingsComponent.h"
class GameObject;
class SettingsTransform;
class CubeCollider;
class EditorController;

class SettingsColliderCubo : public SettingsComponent {
protected:
	SettingsTransform* settingsTransform;
	CubeCollider* myCollider;
	float newRadio;
	EditorController* editor = nullptr;
public:
	SettingsColliderCubo(GameObject* objeto);
	~SettingsColliderCubo();

	void setEditor(EditorController* editor);
	void showDataComponent() override;
	Component* getComponent() override;
};
#endif