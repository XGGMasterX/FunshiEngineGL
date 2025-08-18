#ifndef ENTITY_H
#define ENTITY_H
#include "../Estructuras/ListasEnlazadas/ListasDoblementeEnlazada/ListaDE.h"
#include "../Objetos/Componentes/Component.h"
#include "../GestorDeArchivos/Binario.h"
#include "../Objetos/Componentes/Transform.h"


//TODA ENTIDAD ES UBICABLE Y ANIDABLE
//TODA ATRIBUCION DEBE TENER SU METODO DE SERIALIZACION VIRTUAL SI SE DESEA
//GUARDAR
class Entity {

 protected:
  ListaDE<Component*>* components;
  Binario* myBinario;
  Transform* transformOrigin = nullptr;

 public:
	Entity() {
		components = new ListaDE<Component*>();
  components->addLast(new Transform());
	}
 Entity(Entity* origin){
  components = new ListaDE<Component*>();
  components->addLast(new Transform());
  transformOrigin = origin->getComponent<Transform>();
 }

	Binario* getMyBinario() {
		return myBinario;
	}

 void setOriginTransform(Transform* newOriginTransform){
  transformOrigin = newOriginTransform;
 }

 //SOLO SI NO EXISTA YA PREVIAMENTE
	virtual void addComponent(Component* component) = 0;

	//ASUMO QUE NO SE REPITEN AL DELETEAR PARA NO EXAUSTIVO
	virtual void deleteComponent(Component* component) = 0;

	template <typename T>
	T* getComponent() {
		T* component = nullptr;
		if (!components->isEmpty()) {
			Position<Component*>* position = components->first();
			while (position != nullptr) {
				if (dynamic_cast<T*>(position->getElement())) {
					component = dynamic_cast<T*>(position->getElement());
					position = nullptr;
				}
				else {
					position = (position != components->last()) ? components->next(position) : nullptr;
				}
			}
		}
		return component;
	}

	virtual ListaDE<Component*>* getComponents() = 0;
protected:
 //SERIALIZACION ATRIBUTOS
 
 //GUARDADO
 virtual void serializeGlobalAtributes() = 0;
 virtual void serializeLocalAtributes() = 0;
 virtual void serializeExternalAtributes() = 0;

 //ATRIBUTOS LOCALES
 virtual void serializeTransformOrigin() = 0;
 
 //CARGADO
 virtual void deserializeGlobalAtributes() = 0;
 virtual void deserializeLocalAtributes() = 0;
 virtual void deserializeExternalAtributes() = 0;
 
 //ATRIBUTOS LOCALES
 virtual void deserializeTransformOrigin() = 0;

 //SERIALIZACION ENTIDAD
 virtual void serializeEntityComponents() = 0;
	virtual void deserializeEntityComponents() = 0;
	virtual void serializeEntity() = 0;
	virtual void deserializeEntity() = 0;
public:
	virtual void saveEntity(string filename) = 0;
	virtual void loadEntity(string filename) = 0;
 
 //RETORNA EL PUNTO GLOBAL DEL TRANSFORM SI HAY PADREORIGEN
 virtual Transform* getGlobalTransform() = 0;
};
#endif
