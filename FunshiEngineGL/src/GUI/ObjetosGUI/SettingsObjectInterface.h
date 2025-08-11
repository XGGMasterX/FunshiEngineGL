#ifndef SETTINGSOBJECTINTERFACE_H
#define SETTINGSOBJECTINTERFACE_H

#include "../GeneralUserInterface.h"
#include "SettingsComponent.h"
#include "../../Objetos/GameObject.h"
#include "../../Estructuras/ListasEnlazadas/ListasDoblementeEnlazada/ListaDE.h"
#include "Transform/SettingsTransform.h"
#include "Color/SettingsColor.h"
#include "Colliders/SettingsColliderEsfera.h"
#include "Colliders/SettingsColliderCubo.h"
#include "Colliders/SettingsColliderMalla.h"
#include "RigidBody/SettingsRigidBody.h"
#include "Script/SettingsScript.h"
#include "../../Fisicas/PhysicsEngine.h"

class SettingsObjectInterface : public GeneralUserInterface {
private:
	GameObject* object;
	ListaDE<SettingsComponent*>* listaDESettingsComponent;
    PhysicsEngine* phisics;

public:
	
	SettingsObjectInterface(GameObject* object,bool stateGUI) :
		GeneralUserInterface("Settings", stateGUI,
			ImGuiWindowFlags_MenuBar) {
		this->object = object;
		listaDESettingsComponent = new ListaDE<SettingsComponent*>();
        if (!object->getComponents()->isEmpty()) {
            loadComponents();
        }
	}

    void setPhysics(PhysicsEngine* phisics) {
        this->phisics = phisics;
    }

    void loadComponents() {
        Transform* transformComponent = object->getComponent<Transform>();
        if (transformComponent != nullptr) {
            listaDESettingsComponent->addLast(new SettingsTransform(transformComponent));
        }
        Color* colorComponent = object->getComponent<Color>();
        if (colorComponent != nullptr) {
            listaDESettingsComponent->addLast(new SettingsColor(object));

        }
        //SOPORTE PARA AMBOS COLLIDER
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
    }

	virtual GameObject* getObjectInInspector() {
		return object;
	}

	virtual void initGUI() override  {
		ImGui::Begin(getNameGui().c_str(), &stateGUI, getFlagGui());
		ImGui::PushID(object);
	}

    virtual void contentGUI() override {

        static int momentaneantID = 0;
        ImGui::InputInt("Id", &momentaneantID);
        if (ImGui::Button("Confirmar")) {
            object->setId(momentaneantID);
            momentaneantID = 0;
        }

        ImGui::InputText("Nombre", object->inputName, IM_ARRAYSIZE(object->inputName));

        ImGui::Separator();

        //AGREGAR SOPORTE PARA SCRIPTS (Y SI NO TIENE , SI QUITA , SI AGREGA ?)
		//SOPORTE PARA COLLIDERS

		//agregando el componente transform , mejorar arquitectura para recorrer getComponents

        if (ImGui::BeginPopupContextWindow("AddComponentPopup", ImGuiPopupFlags_MouseButtonRight)) {
            if (ImGui::MenuItem("Agregar Transform") && object->getComponent<Transform>() == nullptr) {
                object->addComponent(new Transform());
                listaDESettingsComponent->addLast(new SettingsTransform(object->getComponent<Transform>()));
            }
            if (ImGui::MenuItem("Agregar Color") && object->getComponent<Color>() == nullptr){
                object->addComponent(new Color());
                listaDESettingsComponent->addLast(new SettingsColor(object));
            }
            if (ImGui::TreeNodeEx("Agregar Collider", ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth)) {

                if (ImGui::Selectable("EsfereCollider")) {
                    if (object->getComponent<Collider>() == nullptr && object->getComponent<Transform>() != nullptr) {
                        object->addComponent(new EsfereCollider(5.0f, object->getComponent<Transform>()));
                        listaDESettingsComponent->addLast(new SettingsColliderEsfera(object));
                    }
                }

                if (ImGui::Selectable("CubeCollider")) {
                    if (object->getComponent<Collider>() == nullptr && object->getComponent<Transform>() != nullptr) {
                        object->addComponent(new CubeCollider(5.0f, object->getComponent<Transform>()));
                        listaDESettingsComponent->addLast(new SettingsColliderCubo(object));
                    }
                }

                if (ImGui::Selectable("MallaCollider")) {
                    if (object->getComponent<Collider>() == nullptr && object->getComponent<Transform>() != nullptr) {
                        object->addComponent(new MallaCollider(5.0f, object->getComponent<Transform>()));
                        listaDESettingsComponent->addLast(new SettingsColliderMalla(object));
                    }
                }
                ImGui::TreePop();
            }
            if (ImGui::MenuItem("RigidBody")) {
                if (object->getComponent<RigidBody>() == nullptr && object->getComponent<Collider>() != nullptr) {
                    RigidBody* rb = new RigidBody(object->getComponent<Collider>(), 1.0f);
                    object->addComponent(rb);
                    phisics->getWorld()->addRigidBody(rb->getRigidBody());
                    //AGREGAR EL RIGID AL MUNDO
                    listaDESettingsComponent->addLast(new SettingsRigidBody(object));
                }
            }
            if (ImGui::MenuItem("Script")) {
                if (object->getComponent<Script>() == nullptr) {
                    object->addComponent(new Script());
                    listaDESettingsComponent->addLast(new SettingsScript(object));
                }
            }
            ImGui::EndPopup();
        }

        //MOSTRAMOS COMPONENTES
        if (!listaDESettingsComponent->isEmpty()) {
            Position<SettingsComponent*>* position = listaDESettingsComponent->first();
            int index = 0;

            while (position != nullptr && position->getElement() != nullptr) {
                SettingsComponent* comp = position->getElement();
                ImGui::PushID(index);

                bool open = ImGui::CollapsingHeader(
                    typeid(*comp).name(),
                    ImGuiTreeNodeFlags_DefaultOpen
                );

                
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoHoldToOpenOthers)) {
                    ImGui::SetDragDropPayload("COMPONENT_DRAG", &comp, sizeof(SettingsComponent*));
                    ImGui::Text("%s", typeid(*comp).name());
                    ImGui::EndDragDropSource();
                }

                
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("COMPONENT_DRAG")) {
                        SettingsComponent* draggedComp = *(SettingsComponent**)payload->Data;
                        auto posA = listaDESettingsComponent->whatElementPosition(draggedComp);
                        auto posB = listaDESettingsComponent->whatElementPosition(comp);
                        if (posA && posB && posA != posB) {
                            listaDESettingsComponent->swapPositions(posA, posB);
                        }
                    }
                    ImGui::EndDragDropTarget();
                }


                
                if (ImGui::BeginPopupContextItem("DeleteComponent", ImGuiPopupFlags_MouseButtonRight)) {
                    if (ImGui::MenuItem("Eliminar Componente")) {
                        object->deleteComponent(comp->getComponent());
                        listaDESettingsComponent->remove(position);
                        RigidBody* rb = object->getComponent<RigidBody>();
                        //SI ESTA RELACIONADO QUITAR SINO SEGUIR
                        if ((rb != nullptr && rb == comp->getComponent())) {
                            phisics->getWorld()->removeRigidBody(rb->getRigidBody());
                        }
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
                position = (position != listaDESettingsComponent->last()) ?
                    listaDESettingsComponent->next(position) : nullptr;
                index++;
            }
        }
    }


	virtual void endGUI() override {
		ImGui::PopID();
		ImGui::End();
	}
	
	virtual void printGUI() override {
		if (stateGUI && phisics != nullptr) {
			initGUI();
			contentGUI();
			endGUI();
		}
	}
};
#endif