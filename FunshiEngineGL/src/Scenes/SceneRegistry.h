#ifndef SCENE_REGISTRY_H
#define SCENE_REGISTRY_H

#include <memory>
#include <vector>

#include "../Estructuras/ListasEnlazadas/ListasDoblementeEnlazada/ListaDE.h"
#include "../Estructuras/Trees/ArbolesEnlazados/ArbolEnlazado.h"

class GameObject;
class Modelos3D;
template <typename E> class Position;

/**
 * Owns all scene GameObjects and the structural hierarchy.
 *
 * The tree and linear list expose raw pointers only as non-owning views. A
 * caller must never delete an object returned by this class.
 */
class SceneRegistry {
private:
    std::vector<std::unique_ptr<GameObject>> ownedGameObjects;
    ArbolEnlazado<GameObject*> entitys;
    ListaDE<GameObject*> gameObjects;

    void createDefaultRoot();
    void refreshTransformOrigins(Position<GameObject*>* parent);

public:
    SceneRegistry();
    ~SceneRegistry();

    // Non-owning compatibility views. The registry remains the sole owner.
    ArbolEnlazado<GameObject*>* getEntitysTree() noexcept { return &entitys; }
    ListaDE<GameObject*>* getGameObjects() noexcept { return &gameObjects; }

    void refreshGameObjectView();
    // Non-owning pointer into ownedGameObjects.
    GameObject* getRoot() const noexcept;
    bool contains(GameObject* object) const noexcept;

    // Ownership of object is transferred to the registry.
    GameObject* createObject(std::unique_ptr<GameObject> object,
                             GameObject* parent = nullptr);
    bool replaceRoot(std::unique_ptr<GameObject> root);
    bool deleteObject(GameObject* object);
    bool deleteObjectByID(int id);
    bool reparent(GameObject* object, GameObject* parent);
    void clear();
};

#endif
