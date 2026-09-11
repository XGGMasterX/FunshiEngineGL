#include "SettingsTransform.h"
#include <imgui.h>

SettingsTransform::SettingsTransform(Transform* componentTransform) {
	this->componentTransform = componentTransform;
}

void SettingsTransform::showDataComponent() {
	ImGui::Text("Translate");
	if (ImGui::DragFloat("Xt", &componentTransform->getTranslatef()[0], 0.05f, 0.0f, 0.0f, "%.2f") |
	    ImGui::DragFloat("Yt", &componentTransform->getTranslatef()[1], 0.05f, 0.0f, 0.0f, "%.2f") |
	    ImGui::DragFloat("Zt", &componentTransform->getTranslatef()[2], 0.05f, 0.0f, 0.0f, "%.2f")) {
		componentTransform->setTranslatef(componentTransform->getTranslatef()[0],
		                                  componentTransform->getTranslatef()[1],
		                                  componentTransform->getTranslatef()[2]);
	}

	ImGui::Text("Scale");
	if (ImGui::DragFloat("Xs", &componentTransform->getScalef()[0], 0.01f, 0.001f, 100.0f, "%.3f") |
	    ImGui::DragFloat("Ys", &componentTransform->getScalef()[1], 0.01f, 0.001f, 100.0f, "%.3f") |
	    ImGui::DragFloat("Zs", &componentTransform->getScalef()[2], 0.01f, 0.001f, 100.0f, "%.3f")) {
		componentTransform->setScalef(componentTransform->getScalef()[0],
		                              componentTransform->getScalef()[1],
		                              componentTransform->getScalef()[2]);
	}

	ImGui::Text("Rotate");
	if (ImGui::DragFloat("Angulo", &componentTransform->getRotatef()[0], 0.5f, -360.0f, 360.0f, "%.1f deg") |
	    ImGui::DragFloat("Xr", &componentTransform->getRotatef()[1], 0.01f, -1.0f, 1.0f, "%.2f") |
	    ImGui::DragFloat("Yr", &componentTransform->getRotatef()[2], 0.01f, -1.0f, 1.0f, "%.2f") |
	    ImGui::DragFloat("Zr", &componentTransform->getRotatef()[3], 0.01f, -1.0f, 1.0f, "%.2f")) {
		componentTransform->setRotatef(componentTransform->getRotatef()[0],
		                               componentTransform->getRotatef()[1],
		                               componentTransform->getRotatef()[2],
		                               componentTransform->getRotatef()[3]);
	}
}

Component* SettingsTransform::getComponent() {
	return componentTransform;
}
