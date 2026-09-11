#ifndef SETTINGSTRANSFORM_H
#define SETTINGSTRANSFORM_H
#include "../SettingsComponent.h"
#include "../../../Objetos/Componentes/Transform.h"

class GameObject;

class SettingsTransform : public SettingsComponent {
	Transform* componentTransform;
	GameObject* ownerObject;
public:
	SettingsTransform(Transform* componentTransform, GameObject* ownerObject = nullptr);
	virtual void showDataComponent() override;
	virtual Component* getComponent();
};
#endif
