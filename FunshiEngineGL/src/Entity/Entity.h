#ifndef ENTITY_H
#define ENTITY_H

#include <memory>
#include <vector>

#include "../Estructuras/ListasEnlazadas/ListasDoblementeEnlazada/ListaDE.h"
#include "../Objetos/Componentes/Component.h"
#include "../GestorDeArchivos/Binario.h"
#include "../Objetos/Componentes/Transform.h"

class Entity {
protected:
    // The list is a non-owning compatibility view. componentOwners owns every
    // component stored in it.
    std::unique_ptr<ListaDE<Component*>> components;
    std::vector<std::unique_ptr<Component>> componentOwners;
    std::unique_ptr<Binario> myBinario;
    Transform* transformOrigin = nullptr;
    std::unique_ptr<Transform> ownedTransformOrigin;

    void clearComponents();

public:
    Entity();
    Entity(Entity* origin);
    virtual ~Entity();

    Binario* getMyBinario();
    void setOriginTransform(Transform* newOriginTransform);

    virtual void addComponent(Component* component) = 0;
    virtual void deleteComponent(Component* component) = 0;

    template <typename T>
    T* getComponent() {
        if (!components || components->isEmpty()) return nullptr;
        Position<Component*>* position = components->first();
        while (position != nullptr) {
            if (auto* component = dynamic_cast<T*>(position->getElement())) return component;
            position = (position != components->last()) ? components->next(position) : nullptr;
        }
        return nullptr;
    }

    virtual ListaDE<Component*>* getComponents() = 0;

protected:
    virtual void serializeGlobalAtributes() = 0;
    virtual void serializeLocalAtributes() = 0;
    virtual void serializeExternalAtributes() = 0;
    virtual void serializeTransformOrigin() = 0;
    virtual void deserializeGlobalAtributes() = 0;
    virtual void deserializeLocalAtributes() = 0;
    virtual void deserializeExternalAtributes() = 0;
    virtual void deserializeTransformOrigin() = 0;
    virtual void serializeEntityComponents() = 0;
    virtual void deserializeEntityComponents() = 0;
    virtual void serializeEntity() = 0;
    virtual void deserializeEntity() = 0;

public:
    virtual void saveEntity(std::string filename) = 0;
    virtual void loadEntity(std::string filename) = 0;
    virtual Transform* getGlobalTransform() = 0;
};

#endif
