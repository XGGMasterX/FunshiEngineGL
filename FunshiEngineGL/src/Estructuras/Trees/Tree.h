#ifndef TREE_H
#define TREE_H
#include "../ListasEnlazadas/ListasDoblementeEnlazada/ListaDE.h"

template<typename E>
class Tree {
public:
    //consultas de la estructura
	virtual int tam() const = 0;
	virtual bool isEmpty() const = 0;
	virtual Position<E>* root() const = 0;

	//consultas de una posicion en la estructura
	virtual ListaDE<Position<E>*>* childsOf(Position<E>* p) const = 0;
	virtual Position<E>* dadOf(Position<E>* p) const = 0;

	//consultas tipo de nodo
	virtual bool isRoot(Position<E>* p) const = 0;
	virtual bool isInternal(Position<E>* p) const = 0;
	virtual bool isExternal(Position<E>* p) const = 0;

	//modificar de la esturctura
	//=>agregar
	virtual void createRoot(E e) = 0;
	virtual void addNodeChildOf(Position<E>* p,E e) = 0; //por defecto addLast()
	virtual void addNodeChildAfterOf(Position<E>* dad, Position<E>* plChild, E e) = 0;
	virtual void addNodeChildBeforeOf(Position<E>* dad, Position<E>* prChild, E e) = 0;

	//=>eliminar
	virtual E deleteRoot() = 0;
	virtual E deleteInternalNode(Position<E>* p) = 0;
	virtual E deleteExternalNode(Position<E>* p) = 0;
};
#endif