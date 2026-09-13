#ifndef SETTINGSCAMERA_H
#define SETTINGSCAMERA_H
#include "../SettingsComponent.h"
class GameObject;
class CameraComponent;

class SettingsCamera : public SettingsComponent {
	GameObject* gameObject;
public:
	SettingsCamera(GameObject* gameObject);

	void showDataComponent() override;
	Component* getComponent() override;
};
#endif