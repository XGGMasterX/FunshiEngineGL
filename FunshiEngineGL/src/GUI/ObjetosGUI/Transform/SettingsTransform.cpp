#include "SettingsTransform.h"
#include "../../../Objetos/GameObject.h"
#include "../../../Objetos/Componentes/RigidBody/RigidBody.h"
#include <imgui.h>
#include <vector>
#include <utility>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

SettingsTransform::SettingsTransform(Transform* componentTransform, GameObject* ownerObject) {
	this->componentTransform = componentTransform;
	this->ownerObject = ownerObject;
}

void SettingsTransform::showDataComponent() {
	float* trans = componentTransform->getTranslatef();
	float* scale = componentTransform->getScalef();
	float* rot = componentTransform->getRotatef();

	bool freeze = componentTransform->childsFreeze;
	std::vector<std::pair<Entity*, glm::mat4>> childSnapshots;

	auto snapshotChildren = [&]() {
		childSnapshots.clear();
		if (ownerObject && freeze) {
			for (auto* child : ownerObject->getChildEntities()) {
				if (child && child->getComponent<Transform>()) {
					float m[16];
					buildMatrixFromTransform(child->getGlobalTransform(), m);
					childSnapshots.push_back({child, glm::make_mat4(m)});
				}
			}
		}
	};

	auto applyFreeze = [&]() {
		if (!childSnapshots.empty() && ownerObject) {
			float pM[16];
			buildMatrixFromTransform(ownerObject->getGlobalTransform(), pM);
			glm::mat4 invParent = glm::inverse(glm::make_mat4(pM));
			for (auto& snap : childSnapshots) {
				glm::mat4 newLocal = invParent * snap.second;
				float localArr[16];
				const float* ptr = glm::value_ptr(newLocal);
				for (int i = 0; i < 16; ++i) localArr[i] = ptr[i];
				decomposeMatrixToTransform(localArr, snap.first->getComponent<Transform>());
			}
		}
	};

	// Sincronizar el resultado al RigidBody: si no, la fisica queda con la
	// pose vieja y (con la simulacion corriendo) re-escribe el transform del
	// objeto cada frame, haciendo que los sliders 'no hagan caso'.
	auto syncFisica = [&]() {
		if (ownerObject) {
			if (RigidBody* body = ownerObject->getComponent<RigidBody>())
				body->syncGameObjectToPhysics();
		}
	};

	ImGui::Text("Translate");
	if (ImGui::DragFloat("Xt", &trans[0], 0.05f, 0.0f, 0.0f, "%.2f") |
	    ImGui::DragFloat("Yt", &trans[1], 0.05f, 0.0f, 0.0f, "%.2f") |
	    ImGui::DragFloat("Zt", &trans[2], 0.05f, 0.0f, 0.0f, "%.2f")) {
		snapshotChildren();
		componentTransform->setTranslatef(trans[0], trans[1], trans[2]);
		applyFreeze();
		syncFisica();
	}

	ImGui::Text("Scale");
	if (ImGui::DragFloat("Xs", &scale[0], 0.01f, 0.001f, 100.0f, "%.3f") |
	    ImGui::DragFloat("Ys", &scale[1], 0.01f, 0.001f, 100.0f, "%.3f") |
	    ImGui::DragFloat("Zs", &scale[2], 0.01f, 0.001f, 100.0f, "%.3f")) {
		snapshotChildren();
		componentTransform->setScalef(scale[0], scale[1], scale[2]);
		applyFreeze();
		syncFisica();
	}

	ImGui::Text("Rotate");
	if (ImGui::DragFloat("Angulo", &rot[0], 0.5f, -360.0f, 360.0f, "%.1f deg") |
	    ImGui::DragFloat("Xr", &rot[1], 0.01f, -1.0f, 1.0f, "%.2f") |
	    ImGui::DragFloat("Yr", &rot[2], 0.01f, -1.0f, 1.0f, "%.2f") |
	    ImGui::DragFloat("Zr", &rot[3], 0.01f, -1.0f, 1.0f, "%.2f")) {
		snapshotChildren();
		componentTransform->setRotatef(rot[0], rot[1], rot[2], rot[3]);
		applyFreeze();
		syncFisica();
	}

	ImGui::Separator();
	ImGui::Checkbox("Childs Freeze", &componentTransform->childsFreeze);
}

Component* SettingsTransform::getComponent() {
	return componentTransform;
}
