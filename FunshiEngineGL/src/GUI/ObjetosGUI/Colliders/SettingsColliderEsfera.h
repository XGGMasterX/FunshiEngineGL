#ifndef SETTINGSCOLLIDERESFERA_H
#define SETTINGSCOLLIDERESFERA_H
#include "../SettingsComponent.h"
class GameObject;
class SettingsTransform;
class EsfereCollider;
class EditorController;

class SettingsColliderEsfera : public SettingsComponent {
protected:
	SettingsTransform* settingsTransform;
	EsfereCollider* myCollider;
	float newRadio;
	EditorController* editor = nullptr;
public:
	SettingsColliderEsfera(GameObject* objeto);
	~SettingsColliderEsfera();

	void setEditor(EditorController* editor);
	void showDataComponent() override;
	Component* getComponent() override;
};
#endif