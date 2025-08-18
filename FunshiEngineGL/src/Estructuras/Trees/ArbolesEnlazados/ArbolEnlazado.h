#ifndef ARBOLENLAZADO_H
#define ARBOLENLAZADO_H
#include <iostream>
#include "../../../ExcepcionesCPP/InvalidOperationException.h"
#include "../../../ExcepcionesCPP/ExcepcionesEstructuras/ArbolesEnlazados/EmptyTreeException.h"
#include "../Tree.h"
#include "../../Nodos/TNodo.h"

using namespace std;

template<typename E>
class ArbolEnlazado : public Tree<E> {
private:
	TNodo<E>* root;
	int size;

	TNodo<E>* checkPosition(Position<E>* p) {
		if (isEmpty()) {
			throw InvalidPositionException("ArbolEnlazado::checkPosition:ElArbolEstaVacio");
		}
		if (p == nullptr) {
			throw InvalidPositionException("ArbolEnlazado::checkPosition:LaPosicionEsNullPtr");
		}
		if (p->getElement() == nullptr) {
			throw InvalidPositionException("ArbolEnlazado::checkPosition:ElElementoDeLaPosicionEsNullPtr");
		}
	    TNodo<E>* resultado = nullptr;
		try {
			resultado = (TNodo<E>*)p;
			if (p != root) {
				ListaDE<TNodo<E>*>* listOfChildsTheDad = resultado->getRootDad()->getChilds();
				if (!listOfChildsTheDad->isElement(resultado)) {
					throw InvalidPositionException("ArbolEnlazado::checkPosition:LaPosicionNoEsHijaDeSuPadre");
				}
			}
		}
		catch (exception e) {
			throw InvalidPositionException("ArbolEnlazado::checkPosition:LaPosicionNoEsDelArbol");
		}
		return resultado;
	}
public:
	ArbolEnlazado(){
		root = nullptr;
		size = 0;
	}
	ArbolEnlazado(E e){
		root = new TNodo<E>(e);
		size = 1;
	}

     ~ArbolEnlazado() {
        if (root) {
            delete root;  // Esto llama destructor de Nodo y borra todo recursivamente
            root = nullptr;
        }
    }

	//consultas de la estructura
	virtual int tam() const override { return size; }
	virtual bool isEmpty() const override { return size == 0; }
	virtual Position<E>* rootOfTree() override {
		if (isEmpty()) {
			throw EmptyTreeException("ArbolEnlazado::root:NoHayRoot");
		}
		return root;
	}

	//consultas de una posicion en la estructura
	virtual ListaDE<Position<E>*>* childsOf(Position<E>* p) override {
		TNodo<E>* position = checkPosition(p);
		if (isExternal(position)) {
			throw InvalidOperationException("ArbolEnlazado::childsOf:LaPosicionEsExternal");
		}
		ListaDE<Position<E>*>* resultado = new ListaDE<Position<E>*>();

		ListaDE<TNodo<E>*>* listaDeHijos = position->getChilds();
		Position<TNodo<E>*>* iterador = listaDeHijos->first();
		while (iterador != nullptr) {
			resultado->addLast(iterador->getElement());
			iterador = (iterador != listaDeHijos->last()) ? listaDeHijos->next(iterador) : nullptr;
		}
		return resultado;
	}
	virtual Position<E>* dadOf(Position<E>* p) override {
		TNodo<E>* position = checkPosition(p);
		if (isRoot(position)) {
			throw InvalidOperationException("ArbolEnlazado::childsOf:LaPosicionEsRoot");
		}
		return position->getRootDad();
	}

	//consultas tipo de nodo
	virtual bool isRoot(Position<E>* p) override {
		TNodo<E>* position = checkPosition(p);
		bool resultado = (position == root);
		return resultado;
	}
	virtual bool isInternal(Position<E>* p) override {
		TNodo<E>* position = checkPosition(p);
		bool resultado = !position->getChilds()->isEmpty();
		return resultado;
	}
	virtual bool isExternal(Position<E>* p) override {
		return !isInternal(p);
	}

	//modificar de la esturctura
	//=>agregar
	virtual Position<E>* createRoot(E e) override {
		if (root != nullptr) {
			throw InvalidOperationException("ArbolEnlazado::createRoot:YaExisteUnRoot");
		}
		root = new TNodo<E>(e);
		size++;
		return root;
	}
	virtual Position<E>* addNodeChildOf(Position<E>* p, E e) override {
		TNodo<E>* dad = checkPosition(p);
		TNodo<E>* hijoNuevo = new TNodo<E>(e,dad);
		dad->getChilds()->addLast(hijoNuevo);
		size++;
		return hijoNuevo;
	}
	virtual Position<E>* addNodeChildAfterOf(Position<E>* dad, Position<E>* plChild, E e) override {
		TNodo<E>* position = checkPosition(dad);
		TNodo<E>* positionLeftChild = checkPosition(position);
		if (position != positionLeftChild->getRootDad()) {
			throw InvalidPositionException("ArbolEnlazado::addNodeChildAfterOf:LaPosicionDeReferenciaIzquierdaNoEsHijaDeDad");
		}
		TNodo<E>* hijoNuevo = new TNodo<E>(e, position);
		ListaDE<TNodo<E>*>* listChildsOfDad = position->getChilds();
		Position<TNodo<E>*>* tlChild = listChildsOfDad->whatElementPosition(positionLeftChild);
		listChildsOfDad->addAfter(tlChild, hijoNuevo);
		size++;
		return hijoNuevo;
	}
	virtual Position<E>* addNodeChildBeforeOf(Position<E>* dad, Position<E>* prChild, E e) override {
		TNodo<E>* position = checkPosition(dad);
		TNodo<E>* positionRightChild = checkPosition(position);
		if (position != positionRightChild->getRootDad()) {
			throw InvalidPositionException("ArbolEnlazado::addNodeChildAfterOf:LaPosicionDeReferenciaDerechaNoEsHijaDeDad");
		}
		TNodo<E>* hijoNuevo = new TNodo<E>(e, position);
		ListaDE<TNodo<E>*>* listChildsOfDad = position->getChilds();
		Position<TNodo<E>*>* trChild = listChildsOfDad->whatElementPosition(positionRightChild);
		listChildsOfDad->addBefore(trChild, hijoNuevo);
		size++;
		return hijoNuevo;
	}

	//=>eliminar
	//siempre el primer hijo ocupa el lugar del padre
	//toma la lista del padre se quita el , la agrega a su lista
	//mientras la agrega a su lista se linkea como padre y listo
	//se retorna el valor del padre
	virtual E deleteRoot() override {
		E saveElement = nullptr;
		if (!isEmpty()) {
			saveElement = root->getElement();
   if(isInternal(root)){
   root->setElement(nullptr);
			ListaDE<TNodo<E>*>* listChildsRoot = root->getChilds();
			Position<TNodo<E>*>* firstChildPosition = listChildsRoot->first();
			listChildsRoot->remove(firstChildPosition);
   delete root;
			root = firstChildPosition->getElement();
			ListaDE<TNodo<E>*>* listChildNewRoot = root->getChilds();
			Position<TNodo<E>*>* iterador = listChildsRoot->first();
			while (iterador != nullptr) { //recorrido exaustivo
				listChildNewRoot->addLast(iterador->getElement());
				((TNodo<E>*)iterador->getElement())->setRootDad(root);
				iterador = (iterador != listChildsRoot->last()) ? listChildsRoot->next(iterador) : nullptr;
			}
   }else{
    root->setElement(nullptr);
    delete root;
    root = nullptr;
   }
			size--;
		}
  else{
   throw InvalidOperationException("ArbolEnlazado::deleteRoot::NoHayRoot");
  }
		return saveElement;
	}


	virtual E deleteInternalNode(Position<E>* p) override {
		TNodo<E>* theDeleteable = checkPosition(p);
		if (!isInternal(theDeleteable)) {
			throw InvalidOperationException("ArbolEnlazado::deleteExternalNode:LaPosicionNoEsInternal");
		}
		E saveElement = theDeleteable->getElement();
		theDeleteable->setElement(nullptr);

		//Tomo el nodo a eliminar , agarro su lista de hijos , saco a su primer hijo de ahi
		ListaDE<TNodo<E>*>* listChildsTheDeleteable = theDeleteable->getChilds();
		Position<TNodo<E>*>* firstChildTheDeleteablePosition = listChildsTheDeleteable->first();
		listChildsTheDeleteable->remove(firstChildTheDeleteablePosition);

		//Tomo el nodo a eliminar , agarro su padre , salvo la posicion de Position y hago el intercambio
		TNodo<E>* theDeleteableDad = theDeleteable->getRootDad();
		ListaDE<TNodo<E>*>* listBrosTheDeleteable = theDeleteableDad->getChilds();
		Position<TNodo<E>*>* theDeleteablePosition = listBrosTheDeleteable->whatElementPosition(theDeleteable);
	 delete theDeleteable;
  listBrosTheDeleteable->remplace(theDeleteablePosition, firstChildTheDeleteablePosition->getElement());
		TNodo<E>* firstChildTheDeleteable = firstChildTheDeleteablePosition->getElement();
		firstChildTheDeleteable->setRootDad(theDeleteableDad);
		

		Position<TNodo<E>*>* iterador = listChildsTheDeleteable->first();
		while (iterador != nullptr) { //recorrido exaustivo
			firstChildTheDeleteable->getChilds()->addLast(iterador->getElement());
			((TNodo<E>*)iterador->getElement())->setRootDad(firstChildTheDeleteable);
			iterador = (iterador != listChildsTheDeleteable->last()) ? listChildsTheDeleteable->next(iterador) : nullptr;
		}
		size--;
		return saveElement;
	}
	virtual E deleteExternalNode(Position<E>* p) override {
		TNodo<E>* theDeleteable = checkPosition(p);
		if (!isExternal(p)) {
			throw InvalidOperationException("ArbolEnlazado::deleteExternalNode:LaPosicionNoEsExternal");
		}
		E saveElement = theDeleteable->getElement();
		theDeleteable->setElement(nullptr);
		if (theDeleteable != root) {
			ListaDE<TNodo<E>*>* listChildsDad = theDeleteable->getRootDad()->getChilds();
			Position<TNodo<E>*>* positionTheDeleteable = listChildsDad->whatElementPosition(theDeleteable);
			delete theDeleteable;
   listChildsDad->remove(positionTheDeleteable);
		}
		else {
			root = nullptr;
		}
		size--;
		return saveElement;
	}

	virtual E deleteNode(Position<E>* p) override {
		E resultado = nullptr;
		if (isInternal(p)) {
			resultado = deleteInternalNode(p);
		}
		else if (isExternal(p)) {
			resultado = deleteExternalNode(p);
		}
		return resultado;
	}

	virtual Position<E>* whatIsPositionOf(E e) override {
		Position<E>* resultado = nullptr;
		if (!isEmpty() && e != nullptr) {
			resultado = busquedaDeElementoPreOrden(root, e);
		}
		return resultado;
	}

	virtual void positionToChildOf(Position<E>* dad, Position<E>* pChild) {
		TNodo<E>* nodeDad = checkPosition(dad);
		TNodo<E>* nodePChild = checkPosition(pChild);
		bool esHijo = false;
		esHijo = busquedaDePositionPreOrden(nodePChild, nodeDad);
		if (!esHijo) {
			//Me quito de mi padre
			ListaDE<TNodo<E>*>* hermanos = nodePChild->getRootDad()->getChilds();
			Position<TNodo<E>*>* position = hermanos->whatElementPosition(nodePChild);
			hermanos->remove(position);

			nodePChild->setRootDad(nodeDad);
			nodeDad->getChilds()->addLast(nodePChild);
		}
	}

private:
	bool busquedaDePositionPreOrden(TNodo<E>* root, TNodo<E>* e) {
		bool resultado = false;
		resultado = (root == e);

		if (resultado) {
			return resultado;
		}
		else if (isInternal(root)) {
			ListaDE<TNodo<E>*>* childs = root->getChilds();
			Position<TNodo<E>*>* child = childs->first();
			TNodo<E>* position = child->getElement();

			while (position != nullptr && !resultado) {
				if (position == e) {
					resultado = true;
				}
				else {
					resultado = busquedaDePositionPreOrden(position, e);
					child = (child != childs->last()) ? childs->next(child) : nullptr;
					if (child != nullptr) {
						position = child->getElement();
					}
					else {
						position = nullptr;
					}
				}
			}
		}
		return resultado;
	}



	Position<E>* busquedaDeElementoPreOrden(Position<E>* root,E e) {
		TNodo<E>* node = checkPosition(root);
		Position<E>* resultado = nullptr;
		if (node->getElement() == e) {
			resultado = node;
		}
		else if(isInternal(node)){
			ListaDE<TNodo<E>*>* childs = node->getChilds();
			Position<TNodo<E>*>* child = childs->first();
			TNodo<E>* position = child->getElement();
			while (position != nullptr && resultado == nullptr) {
				if (position->getElement() == e) {
					resultado = position;
				}
				else {
					resultado = busquedaDeElementoPreOrden(position, e);
					child = (child != childs->last()) ? childs->next(child) : nullptr;
					if (child != nullptr) {
						position = child->getElement();
					}
					else {
						position = nullptr;
					}
				}
			}
		}
		return resultado;
	}
};
#endif
