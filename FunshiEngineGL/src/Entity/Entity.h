#ifndef ENTITY_H
#define ENTITY_H
#include "../Estructuras/ListasEnlazadas/ListasDoblementeEnlazada/ListaDE.h"
#include "../Objetos/Componentes/Component.h"
#include "../GestorDeArchivos/Binario.h"


class Entity {
public:
	Entity() {
		components = new ListaDE<Component*>();
	}

protected:
	ListaDE<Component*>* components;
	Binario* myBinario;
public:

	Binario* getMyBinario() {
		return myBinario;
	}

	virtual void addComponent(Component* component) = 0;

	//ASUMO QUE NO SE REPITEN
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


	virtual void serializeEntityComponents() = 0;


	virtual void deserializeEntityComponents() = 0;


	virtual void serializeEntity() = 0;


	virtual void deserializeEntity() = 0;


public:


	virtual void saveEntity(string filename) = 0;



	virtual void loadEntity(string filename) = 0;
};
#endif