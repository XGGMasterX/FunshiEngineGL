#ifndef SETTINGSCOLLIDERMALLA_H
#define SETTINGSCOLLIDERMALLA_H
#include "../SettingsComponent.h"
class GameObject;
class SettingsTransform;
class MallaCollider;
class EditorController;

class SettingsColliderMalla : public SettingsComponent {
protected:
	SettingsTransform* settingsTransform;
	MallaCollider* myCollider;
	float newRadio;
	EditorController* editor = nullptr;
public:
	SettingsColliderMalla(GameObject* objeto);
	~SettingsColliderMalla();

	void setEditor(EditorController* editor);
	void showDataComponent() override;
	Component* getComponent() override;
};
#endif