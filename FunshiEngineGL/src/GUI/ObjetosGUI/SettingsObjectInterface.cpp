#include "SettingsObjectInterface.h"

#include "SettingsComponent.h"
#include "Transform/SettingsTransform.h"
#include "Color/SettingsColor.h"
#include "Colliders/SettingsColliderEsfera.h"
#include "Colliders/SettingsColliderCubo.h"
#include "Colliders/SettingsColliderMalla.h"
#include "RigidBody/SettingsRigidBody.h"
#include "Script/SettingsScript.h"
#include "Model/SettingsModel.h"
#include "Material/SettingsMaterial.h"
#include "Light/SettingsLight.h"
#include "../../Objetos/GameObject.h"
#include "../../Objetos/Componentes/Light.h"
#include "../../Objetos/Componentes/Material.h"
#include "../../Fisicas/PhysicsEngine.h"
#include "../../Herramientas/TypeUtils.h"
#include <imgui.h>
#include <typeinfo>

SettingsObjectInterface::SettingsObjectInterface(GameObject* object,
                                                 bool stateGUI)
	: GeneralUserInterface("Settings", stateGUI, ImGuiWindowFlags_MenuBar) {
	this->object = object;
	momentaneantID = object ? object->getId() : 0;
	listaDESettingsComponent = new ListaDE<SettingsComponent*>();
	if (!object->getComponents()->isEmpty()) {
		loadComponents();
	}
}

SettingsObjectInterface::~SettingsObjectInterface() {
	while (!listaDESettingsComponent->isEmpty()) {
		Position<SettingsComponent*>* pos = listaDESettingsComponent->first();
		delete pos->getElement();
		listaDESettingsComponent->remove(pos);
	}
	delete listaDESettingsComponent;
}

void SettingsObjectInterface::setPhysics(PhysicsEngine* phisics) {
	this->phisics = phisics;
}

void SettingsObjectInterface::loadComponents() {
	Transform* transformComponent = object->getComponent<Transform>();
	if (transformComponent != nullptr) {
		listaDESettingsComponent->addLast(
		    new SettingsTransform(transformComponent, object));
	}
	Color* colorComponent = object->getComponent<Color>();
	if (colorComponent != nullptr) {
		listaDESettingsComponent->addLast(new SettingsColor(object));
	}
	Material* materialComponent = object->getComponent<Material>();
	if (materialComponent != nullptr) {
		listaDESettingsComponent->addLast(new SettingsMaterial(object));
	}
	Light* lightComponent = object->getComponent<Light>();
	if (lightComponent != nullptr) {
		listaDESettingsComponent->addLast(new SettingsLight(object));
	}
	// SOPORTE PARA AMBOS COLLIDER
	Collider* collider = object->getComponent<EsfereCollider>();
	if (collider != nullptr) {
		listaDESettingsComponent->addLast(new SettingsColliderEsfera(object));
	}
	collider = object->getComponent<CubeCollider>();
	if (collider != nullptr) {
		listaDESettingsComponent->addLast(new SettingsColliderCubo(object));
	}
	collider = object->getComponent<MallaCollider>();
	if (collider != nullptr) {
		listaDESettingsComponent->addLast(new SettingsColliderMalla(object));
	}
	RigidBody* rigidBody = object->getComponent<RigidBody>();
	if (rigidBody != nullptr) {
		listaDESettingsComponent->addLast(new SettingsRigidBody(object));
	}
	Script* script = object->getComponent<Script>();
	if (script != nullptr) {
		listaDESettingsComponent->addLast(new SettingsScript(object));
	}
	Model* model = object->getComponent<Model>();
	if (model != nullptr) {
		listaDESettingsComponent->addLast(new SettingsModel(object));
	}
}

GameObject* SettingsObjectInterface::getObjectInInspector() { return object; }

void SettingsObjectInterface::setTargetObject(GameObject* newObject) {
    if (object == newObject || newObject == nullptr) return;
    object = newObject;
    while (!listaDESettingsComponent->isEmpty()) {
        Position<SettingsComponent*>* pos = listaDESettingsComponent->first();
        delete pos->getElement();
        listaDESettingsComponent->remove(pos);
    }
    momentaneantID = object->getId();
    loadComponents();
}

void SettingsObjectInterface::initGUI() {
	ImGui::Begin(getNameGui().c_str(), &stateGUI, getFlagGui());
	ImGui::PushID(object);
}

void SettingsObjectInterface::contentGUI() {
	ImGui::InputInt("Id", &momentaneantID);
	if (ImGui::Button("Confirmar")) {
		object->setId(momentaneantID);
		momentaneantID = 0;
	}

	ImGui::InputText("Nombre", object->inputName, IM_ARRAYSIZE(object->inputName));

	ImGui::Separator();

	if (ImGui::BeginPopupContextWindow("AddComponentPopup",
	                                   ImGuiPopupFlags_MouseButtonRight)) {
		if (ImGui::MenuItem("Agregar Transform") &&
		    object->getComponent<Transform>() == nullptr) {
			object->addComponent(new Transform());
			listaDESettingsComponent->addLast(
			    new SettingsTransform(object->getComponent<Transform>(),
			                          object));
		}
		if (ImGui::MenuItem("Agregar Color") &&
		    object->getComponent<Color>() == nullptr) {
			object->addComponent(new Color());
			listaDESettingsComponent->addLast(new SettingsColor(object));
		}
		if (ImGui::MenuItem("Agregar Material") &&
		    object->getComponent<Material>() == nullptr) {
			object->addComponent(new Material());
			listaDESettingsComponent->addLast(new SettingsMaterial(object));
		}
		if (ImGui::MenuItem("Agregar Luz") &&
		    object->getComponent<Light>() == nullptr) {
			object->addComponent(new Light());
			listaDESettingsComponent->addLast(new SettingsLight(object));
		}
		if (ImGui::TreeNodeEx("Agregar Collider",
		                      ImGuiTreeNodeFlags_OpenOnArrow |
		                          ImGuiTreeNodeFlags_SpanAvailWidth)) {
			if (ImGui::Selectable("EsfereCollider")) {
				if (object->getComponent<Collider>() == nullptr &&
				    object->getComponent<Transform>() != nullptr) {
					object->addComponent(
					    new EsfereCollider(5.0f,
					                       object->getComponent<Transform>()));
					listaDESettingsComponent->addLast(
					    new SettingsColliderEsfera(object));
				}
			}
			if (ImGui::Selectable("CubeCollider")) {
				if (object->getComponent<Collider>() == nullptr &&
				    object->getComponent<Transform>() != nullptr) {
					object->addComponent(
					    new CubeCollider(5.0f,
					                     object->getComponent<Transform>()));
					listaDESettingsComponent->addLast(
					    new SettingsColliderCubo(object));
				}
			}
			if (ImGui::Selectable("MallaCollider")) {
				if (object->getComponent<Collider>() == nullptr &&
				    object->getComponent<Transform>() != nullptr) {
					object->addComponent(
					    new MallaCollider(5.0f,
					                      object->getComponent<Transform>()));
					listaDESettingsComponent->addLast(
					    new SettingsColliderMalla(object));
				}
			}
			ImGui::TreePop();
		}
		if (ImGui::MenuItem("RigidBody")) {
			if (object->getComponent<RigidBody>() == nullptr &&
			    object->getComponent<Collider>() != nullptr) {
				RigidBody* rb =
				    new RigidBody(object->getComponent<Collider>(), 1.0f);
				object->addComponent(rb);
				phisics->getWorld()->addRigidBody(rb->getRigidBody());
				listaDESettingsComponent->addLast(new SettingsRigidBody(object));
			}
		}
		if (ImGui::MenuItem("Script")) {
			if (object->getComponent<Script>() == nullptr) {
				object->addComponent(new Script());
				listaDESettingsComponent->addLast(new SettingsScript(object));
			}
		}
		if (ImGui::MenuItem("Model")) {
			if (object->getComponent<Model>() == nullptr) {
				object->addComponent(new Model());
				listaDESettingsComponent->addLast(new SettingsModel(object));
			}
		}
		ImGui::EndPopup();
	}

	// MOSTRAMOS COMPONENTES
	if (!listaDESettingsComponent->isEmpty()) {
		Position<SettingsComponent*>* position =
		    listaDESettingsComponent->first();

		while (position != nullptr && position->getElement() != nullptr) {
			SettingsComponent* comp = position->getElement();
			ImGui::PushID(comp);

			// Usamos demangle para obtener un nombre legible
			std::string compName = demangle(typeid(*comp).name());

			bool open = ImGui::CollapsingHeader(
			    compName.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

			if (ImGui::BeginDragDropSource(
			        ImGuiDragDropFlags_SourceNoHoldToOpenOthers)) {
				ImGui::SetDragDropPayload("COMPONENT_DRAG", &comp,
				                          sizeof(SettingsComponent*));
				ImGui::Text("%s", compName.c_str());
				ImGui::EndDragDropSource();
			}

			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload =
				        ImGui::AcceptDragDropPayload("COMPONENT_DRAG")) {
					SettingsComponent* draggedComp =
					    *(SettingsComponent**)payload->Data;
					auto posA =
					    listaDESettingsComponent->whatElementPosition(
					        draggedComp);
					auto posB = listaDESettingsComponent->whatElementPosition(
					    comp);
					if (posA && posB && posA != posB) {
						listaDESettingsComponent->swapPositions(posA, posB);
					}
				}
				ImGui::EndDragDropTarget();
			}

			if (ImGui::BeginPopupContextItem("DeleteComponent",
			                                 ImGuiPopupFlags_MouseButtonRight)) {
				if (ImGui::MenuItem("Eliminar Componente")) {
					object->deleteComponent(comp->getComponent());
					listaDESettingsComponent->remove(position);
					RigidBody* rb = object->getComponent<RigidBody>();
					// SI ESTA RELACIONADO QUITAR SINO SEGUIR
					if ((rb != nullptr && rb == comp->getComponent())) {
						phisics->getWorld()->removeRigidBody(
						    rb->getRigidBody());
					}
					delete comp;
					ImGui::EndPopup();
					ImGui::PopID();
					break;
				}
				ImGui::EndPopup();
			}

			if (open) {
				comp->showDataComponent();
			}

			ImGui::PopID();
			position = (position != listaDESettingsComponent->last())
			               ? listaDESettingsComponent->next(position)
			               : nullptr;
		}
	}
}

void SettingsObjectInterface::endGUI() {
	ImGui::PopID();
	ImGui::End();
}

void SettingsObjectInterface::printGUI() {
	if (stateGUI && phisics != nullptr) {
		initGUI();
		contentGUI();
		endGUI();
	}
}