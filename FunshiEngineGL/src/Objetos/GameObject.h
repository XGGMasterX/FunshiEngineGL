#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <cfloat>
#include <memory>
#include <string>

#include "../Herramientas/TypeUtils.h"
#include "../Objetos/Componentes/Phisics.h"
#include "../Objetos/Componentes/Transform.h"
#include "../Objetos/Componentes/Color.h"
#include "../GestorDeArchivos/Binario.h"
#include "../Estructuras/Comparable.h"
#include "../Estructuras/ListasEnlazadas/ListasDoblementeEnlazada/ListaDE.h"
#include "../Objetos/Componentes/Colliders/EsfereCollider.h"
#include "../Objetos/Componentes/Colliders/CubeCollider.h"
#include "../Objetos/Componentes/Colliders/MallaCollider.h"
#include "../Objetos/Componentes/RigidBody/RigidBody.h"
#include "../Objetos/Componentes/Script.h"
#include "../Objetos/Componentes/Model.h"
#include "../Entity/Entity.h"

class GameObject : public Comparable<GameObject>, public Entity {
protected:
    bool state = true;
    int id = 0;
    int tam = 1;
    // Buffer reutilizable para el global transform. NO puede ser un
    // unique_ptr recreado por llamada: getGlobalTransform() devuelve puntero a
    // este miembro y otros sistemas (gizmo, colliders, settings) lo guardan
    // entre frames; si se reasignara, quedarian punteros a memoria liberada
    // (heap-use-after-free). El valor se sobrescribe en cada llamada, pero la
    // direccion es estable mientras el GameObject viva.
    Transform globalTransformCache;

public:
    explicit GameObject(Entity* origin);
    GameObject();
    ~GameObject() override;

    char inputName[25] = "";
    color auxColor = {0.0f, 0.0f, 0.0f, 1.0f};

    void addComponent(Component* component) override;
    void addComponent(std::unique_ptr<Component> component);
    void deleteComponent(Component* component) override;
    ListaDE<Component*>* getComponents() override;
    int compareTo(GameObject* other) override;
    float distanciaA(GameObject* other);
    void setId(int id);
    void setState(bool state);
    void setTam(int tam);
    void setColor(color cor);
    const float* getColor(float*& arr, int tam);
    Color* getColor();
    int getId();
    bool getState();
    int getTam();

    virtual void dibujar(float deltaTime) = 0;
    virtual void update(float deltaTime);

protected:
    void serializeGlobalAtributes() override;
    void serializeLocalAtributes() override;
    void serializeExternalAtributes() override;
    void serializeTransformOrigin() override;
    void deserializeGlobalAtributes() override;
    void deserializeLocalAtributes() override;
    void deserializeExternalAtributes() override;
    void deserializeTransformOrigin() override;
    void serializeEntityComponents() override;
    void deserializeEntityComponents() override;
    void serializeEntity() override;
    void deserializeEntity() override;

public:
    void saveEntity(std::string filename) override;
    void loadEntity(std::string filename) override;
    Transform* getGlobalTransform() override;
};

#endif
