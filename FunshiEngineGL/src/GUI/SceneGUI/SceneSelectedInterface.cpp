#include "SceneSelectedInterface.h"

#include "../../Objetos/GameObject.h"
#include "../../Objetos/Modelos3D.h"
#include "../../Objetos/GameObjectFactory.h"
#include "../../Scenes/EditorController.h"
#include "../../Scenes/SceneRegistry.h"
#include "../../Fisicas/PhysicsEngine.h"
#include "../../Events/EventBus.h"
#include <cstring>
#include <memory>
#include <imgui.h>

SceneSelectedInterface::SceneSelectedInterface(bool state)
    : GeneralUserInterface("SelectedObjects", state, ImGuiWindowFlags_MenuBar) {}

SceneSelectedInterface::~SceneSelectedInterface() {
    if (events && eventSubscription) events->unsubscribe(eventSubscription);
}

void SceneSelectedInterface::bindScene(SceneRegistry* value,
                                       EditorController* controller,
                                       EventBus* bus) {
    if (events && eventSubscription) events->unsubscribe(eventSubscription);
    scene = value;
    editor = controller;
    events = bus;
    eventSubscription = 0;
    returneableObject = nullptr;
    if (events) {
        eventSubscription = events->subscribe([this](const SceneEvent& event) {
            if (event.type == SceneEventType::ObjectDeleted ||
                event.type == SceneEventType::SceneCleared) {
                returneableObject = nullptr;
            }
        });
    }
}

ArbolEnlazado<GameObject*>* SceneSelectedInterface::getEntitysTree() {
    return scene ? scene->getEntitysTree() : nullptr;
}
void SceneSelectedInterface::setEntitys(ListaDE<GameObject*>* value) {
    // Kept for source compatibility. The GUI no longer imports or copies
    // scene state from an external list.
    (void)value;
}
GameObject* SceneSelectedInterface::getReturnableEntity() { return returneableObject; }
void SceneSelectedInterface::setPhysics(PhysicsEngine* physics) {
    if (editor) editor->setPhysics(physics);
}

void SceneSelectedInterface::initGUI() {
    ImGui::Begin(getNameGui().c_str(), &stateGUI, getFlagGui());
    ImGui::PushID(this);
}

void SceneSelectedInterface::drawPreOrder(Position<GameObject*>* pos) {
    auto* entitys = getEntitysTree();
    if (!entitys || !pos) return;
    if (pos != entitys->rootOfTree()) {
        const std::string label = "Object:" + std::to_string(pos->getElement()->getId());
        if (ImGui::Selectable(label.c_str(), returneableObject == pos->getElement())) {
            returneableObject = pos->getElement();
            if (events)
                events->publish({SceneEventType::ObjectSelected,
                                 returneableObject, nullptr});
        }
        if (ImGui::BeginDragDropSource()) {
            Position<GameObject*>* dragged = pos;
            ImGui::SetDragDropPayload("ENTITY_NODE", &dragged, sizeof(dragged));
            ImGui::Text("Moviendo %s", label.c_str());
            ImGui::EndDragDropSource();
        }
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_NODE")) {
                Position<GameObject*>* dragged = nullptr;
                std::memcpy(&dragged, payload->Data, sizeof(dragged));
                if (dragged != pos) {
                    if (editor)
                        editor->reparentGameObject(dragged->getElement(),
                                                    pos->getElement());
                }
            }
            ImGui::EndDragDropTarget();
        }
    }
    if (entitys->isInternal(pos)) {
        ImGui::Indent();
        auto* children = entitys->childsOf(pos);
        auto* child = children->first();
        while (child) {
            drawPreOrder(child->getElement());
            child = (child != children->last()) ? children->next(child) : nullptr;
        }
        ImGui::Unindent();
    }
}

void SceneSelectedInterface::contentGUI() {
    auto* entitys = getEntitysTree();
    if (!entitys) return;
    if (!entitys->isEmpty()) {
        if (deleteObject) {
            ImGui::InputInt("Id", &inputImGuiID);
            if (ImGui::Button("Delete")) {
                deleteObjectByID(inputImGuiID);
                deleteObject = false;
            }
        }
        drawPreOrder(entitys->rootOfTree());
    }
    if (ImGui::BeginPopupContextWindow("SelectedEntitysPopup", ImGuiPopupFlags_MouseButtonRight)) {
        if (ImGui::MenuItem("New Object")) {}
        if (ImGui::MenuItem("New RenderObject")) {
            auto object = GameObjectFactory::createModelObject();
            ImGui::InputText("Path", inputImGuiString, IM_ARRAYSIZE(inputImGuiString),
                             ImGuiInputTextFlags_EnterReturnsTrue);
            createGameObject(std::move(object));
        }
        if (ImGui::MenuItem("Delete By ID")) deleteObject = true;
        ImGui::EndPopup();
    }
}

void SceneSelectedInterface::createGameObject(GameObject* object) {
    if (!object) return;
    std::unique_ptr<GameObject> owned(object);
    createGameObject(std::move(owned));
}

void SceneSelectedInterface::createGameObject(
    std::unique_ptr<GameObject> object) {
    if (!object) return;
    if (!editor) return;
    editor->createGameObject(std::move(object),
                             scene ? scene->getRoot() : nullptr);
}

void SceneSelectedInterface::addChildGameObject(GameObject* parent, GameObject* object) {
    if (!object) return;
    std::unique_ptr<GameObject> owned(object);
    if (!parent || !editor) return;
    editor->createGameObject(std::move(owned), parent);
}

void SceneSelectedInterface::replaceRootGameObject(GameObject* newRoot) {
    if (!newRoot) return;
    std::unique_ptr<GameObject> owned(newRoot);
    if (!scene) return;
    if (editor) editor->clearScene();
    scene->replaceRoot(std::move(owned));
    returneableObject = nullptr;
}

void SceneSelectedInterface::clearGameObjects() {
    returneableObject = nullptr;
    if (editor) editor->clearScene();
    else if (scene) scene->clear();
}

void SceneSelectedInterface::refreshGameObjectView() {
    if (scene) scene->refreshGameObjectView();
}

bool SceneSelectedInterface::deleteObjectByID(int id) {
    const bool deleted = editor && editor->deleteObjectByID(id);
    if (deleted) returneableObject = nullptr;
    return deleted;
}

ListaDE<GameObject*>* SceneSelectedInterface::getGameObjects() {
    return scene ? scene->getGameObjects() : nullptr;
}
void SceneSelectedInterface::endGUI() { ImGui::PopID(); ImGui::End(); }
void SceneSelectedInterface::printGUI() {
    if (stateGUI) { initGUI(); contentGUI(); endGUI(); }
}
