#ifndef SETTINGSTRANSFORM_H
#define SETTINGSTRANSFORM_H
#include "../SettingsComponent.h"
#include "../../../Objetos/Componentes/Transform.h"

class SettingsTransform : public SettingsComponent {
	Transform* componentTransform;
public:
	SettingsTransform(Transform* componentTransform);
	virtual void showDataComponent() override;
	virtual Component* getComponent();
};
#endif
