#ifndef TREE_H
#define TREE_H
#include "../ListasEnlazadas/ListasDoblementeEnlazada/ListaDE.h"

template<typename E>
class Tree {
public:
    //consultas de la estructura
	virtual int tam() const = 0;
	virtual bool isEmpty() const = 0;
	virtual Position<E>* rootOfTree() = 0;

	//consultas de una posicion en la estructura
	virtual ListaDE<Position<E>*>* childsOf(Position<E>* p) = 0;
	virtual Position<E>* dadOf(Position<E>* p) = 0;
	virtual Position<E>* whatIsPositionOf(E e) = 0;

	//consultas tipo de nodo
	virtual bool isRoot(Position<E>* p) = 0;
	virtual bool isInternal(Position<E>* p) = 0;
	virtual bool isExternal(Position<E>* p) = 0;

	//modificar de la esturctura
	//=>agregar
	virtual Position<E>* createRoot(E e) = 0;
	virtual Position<E>* addNodeChildOf(Position<E>* p,E e) = 0; //por defecto addLast()
	virtual Position<E>* addNodeChildAfterOf(Position<E>* dad, Position<E>* plChild, E e) = 0;
	virtual Position<E>* addNodeChildBeforeOf(Position<E>* dad, Position<E>* prChild, E e) = 0;
	virtual void positionToChildOf(Position<E>* dad, Position<E>* pChild) = 0; //si no hay relacion parental (pre orden)

	//=>eliminar
	virtual E deleteRoot() = 0;
	virtual E deleteInternalNode(Position<E>* p) = 0;
	virtual E deleteExternalNode(Position<E>* p) = 0;
	virtual E deleteNode(Position<E>* p) = 0;
};
#endif