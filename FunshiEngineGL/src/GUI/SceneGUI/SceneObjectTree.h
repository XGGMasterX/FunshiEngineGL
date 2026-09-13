#ifndef SCENE_OBJECT_TREE_H
#define SCENE_OBJECT_TREE_H

#include "../../Herramientas/TreeGUI/TreeGUI.h"

class GameObject;
class SceneRegistry;
class EditorController;
class EventBus;
class IconosGUI;

/**
 * Widget reutilizable que dibuja la jerarquia de la escena: una fila por
 * GameObject con icono, nombre, colapso/expansion con estado persistente,
 * renombrado en linea, menu contextual y drag & drop.
 *
 * Es un componente puro de contenido: se dibuja dentro de la ventana activa
 * de ImGui (no abre/cierra ventanas). Se enlaza a la escena con bindScene(),
 * igual que los paneles de GUI.
 *
 * El delete y el reparent se aplican de forma diferida (tras el recorrido)
 * para no invalidar los iteradores del arbol mientras se dibuja.
 */
class SceneObjectTree {
public:
    SceneObjectTree() = default;
    ~SceneObjectTree();

    // No se copia: posee una suscripcion al EventBus.
    SceneObjectTree(const SceneObjectTree&) = delete;
    SceneObjectTree& operator=(const SceneObjectTree&) = delete;

    void bindScene(SceneRegistry* value, EditorController* controller,
                   EventBus* bus);
    void setIconosGUI(IconosGUI* iconos);
    void draw();

private:
    TreeIG::RowResult drawRow(GameObject* object, bool wasOpen);
    void applyDeferredOperations();
    void resetState();
    void unbind();

    SceneRegistry* scene = nullptr;
    EditorController* editor = nullptr;
    EventBus* events = nullptr;
    size_t eventSubscription = 0;
    IconosGUI* iconosGUI = nullptr;

    // Nodos abiertos en la jerarquia (estado de colapso persistente).
    TreeIG::OpenState openNodes;
    // Estado de renombrado en linea.
    GameObject* renombrando = nullptr;
    // Operaciones diferidas: esperan al final del recorrido para mutar el
    // arbol con seguridad.
    GameObject* objetoAEliminar = nullptr;
    GameObject* objetoAReParentar = nullptr;
    GameObject* objetoPadreNuevo = nullptr;
};

#endif