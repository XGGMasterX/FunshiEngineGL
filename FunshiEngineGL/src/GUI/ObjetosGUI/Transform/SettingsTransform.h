#ifndef SETTINGSTRANSFORM_H
#define SETTINGSTRANSFORM_H
#include "../SettingsComponent.h"
#include "../../../Objetos/Componentes/Transform.h"

class SettingsTransform : public SettingsComponent {
	Transform* componentTransform;
public:
	SettingsTransform(Transform* componentTransform){
		this->componentTransform = componentTransform;
	}

	virtual void showDataComponent() override {
		ImGui::Text("Translate");
		ImGui::SliderFloat("Xt", &componentTransform->getTranslatef()[0], -100.0f, 100.0f);
		ImGui::SliderFloat("Yt", &componentTransform->getTranslatef()[1], -100.0f, 100.0f);
		ImGui::SliderFloat("Zt", &componentTransform->getTranslatef()[2], -100.0f, 100.0f);
		componentTransform->setTranslatef(componentTransform->getTranslatef()[0], componentTransform->getTranslatef()[1], componentTransform->getTranslatef()[2]);

		ImGui::Text("Scale");
		ImGui::SliderFloat("Xs", &componentTransform->getScalef()[0], -50.0f, 50.0f);
		ImGui::SliderFloat("Ys", &componentTransform->getScalef()[1], -50.0f, 50.0f);
		ImGui::SliderFloat("Zs", &componentTransform->getScalef()[2], -50.0f, 50.0f);
		componentTransform->setScalef(componentTransform->getScalef()[0], componentTransform->getScalef()[1], componentTransform->getScalef()[2]);

		ImGui::Text("Rotate");
		ImGui::SliderFloat("Angulo", &componentTransform->getRotatef()[0], -100.0f, 100.0f);
		ImGui::SliderFloat("Xr", &componentTransform->getRotatef()[1], -100.0f, 100.0f);
		ImGui::SliderFloat("Yr", &componentTransform->getRotatef()[2], -100.0f, 100.0f);
		ImGui::SliderFloat("Zr", &componentTransform->getRotatef()[3], -100.0f, 100.0f);
		componentTransform->setRotatef(componentTransform->getRotatef()[0], componentTransform->getRotatef()[1], componentTransform->getRotatef()[2], componentTransform->getRotatef()[3]);
	}

	virtual Component* getComponent() {
		return componentTransform;
	}
};
#endif
