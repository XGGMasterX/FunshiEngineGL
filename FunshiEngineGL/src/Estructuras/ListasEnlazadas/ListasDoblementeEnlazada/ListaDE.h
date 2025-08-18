#ifndef LISTADE_H
#define LISTADE_H

#include "../../Nodos/DNodo.h"
#include "../PositionList.h"
#include "../../../ExcepcionesCPP/ExcepcionesEstructuras/ListasEnlazadas/EmptyListException.h"
#include "../../../ExcepcionesCPP/ExcepcionesEstructuras/InvalidPositionException.h"
#include "../../../ExcepcionesCPP/InvalidOperationException.h"

using namespace std;


template <typename E>
//El recorrido en la estructura es hacia la derecha conceptualmente
//Lista implementada mediante centinelas
class ListaDE : PositionList<E> {
protected:
	DNodo<E>* front;
	DNodo<E>* tail;
	int size;

	virtual DNodo<E>* checkPosition(Position<E>* p) {
		if (isEmpty()) {
			throw InvalidPositionException("ListaDE::checkPosition:LaListaEstaVacia");
		}
		if (p == nullptr) {
			throw InvalidPositionException("ListaDE::checkPosition:LaPosicionEsNullPtr");
		}
		if (p->getElement() == nullptr) {
			throw InvalidPositionException("ListaDE::checkPosition:LaPosicionTieneElementoNullPtr");
		}
		try {
			return (DNodo<E>*)p;
		}
		catch (exception e) {
			throw InvalidPositionException("ListaDE::checkPosition:LaPosicionNoEsDeLaLista");
		}
	}

public:
	ListaDE() {
		front = new DNodo<E>(nullptr);
		tail = new DNodo<E>(nullptr);
		front->setRight(tail);
		tail->setLeft(front);
		size = 0;
	}

	virtual bool isEmpty() override {
		return size == 0;
	}

	virtual int tam() override {
		return size;
	}

	virtual Position<E>* first() override {
		if (isEmpty()) {
			throw EmptyListException("ListaDE::first:NoHayPrimerNodo");
		}
		return front->getRight();
	}
	virtual Position<E>* last() override {
		if (isEmpty()) {
			throw EmptyListException("ListaDE::last:NoHayUltimoNodo");
		}
		return tail->getLeft();
	}
	virtual void addLast(E e) override {
		DNodo<E>* nuevo = new DNodo<E>(e);

		//ligar
		tail->getLeft()->setRight(nuevo);
		nuevo->setLeft(tail->getLeft());
		tail->setLeft(nuevo);
		nuevo->setRight(tail);

		size++;
	}

	virtual void addFirst(E e) override {
		DNodo<E>* nuevo = new DNodo<E>(e);

		//ligar
		front->getRight()->setLeft(nuevo);
		nuevo->setRight(tail->getRight());
		tail->setRight(nuevo);
		nuevo->setLeft(tail);

		size++;
	}

	virtual void addAfter(Position<E>* pl, E e) override {
		DNodo<E>* position = checkPosition(pl);
		DNodo<E>* nuevo = new DNodo<E>(e);

		position->getRight()->setLeft(nuevo);
		nuevo->setRight(position->getRight());
		position->setRight(nuevo);
		nuevo->setLeft(position);

		size++;
	}

	virtual void addBefore(Position<E>* pr, E e) override {
		DNodo<E>* position = checkPosition(pr);
		DNodo<E>* nuevo = new DNodo<E>(e);

		position->getLeft()->setRight(nuevo);
		nuevo->setLeft(position->getLeft());
		position->setLeft(nuevo);
		nuevo->setRight(position);

		size++;
	}

	virtual E remplace(Position<E>* p, E e) override {
		DNodo<E>* position = checkPosition(p);
		E resultado = position->getElement();
		position->setElement(e);
		return resultado;
	}

	virtual E remove(Position<E>* p) override {
		DNodo<E>* position = checkPosition(p);
		E resultado = position->getElement();

		position->getLeft()->setRight(position->getRight());
		position->getRight()->setLeft(position->getLeft());

		position->setElement(nullptr);
		size--;
		return resultado;
	}

	virtual Position<E>* next(Position<E>* p) override {
		DNodo<E>* position = checkPosition(p);
		if (position == last()) {
			throw InvalidOperationException("ListaDE::next:LaPosicionDadaPorParametroEsLast");
		}
		return position->getRight();
	}

	virtual Position<E>* prev(Position<E>* p) override {
		DNodo<E>* position = checkPosition(p);
		if (position == first()) {
			throw InvalidOperationException("ListaDE::next:LaPosicionDadaPorParametroEsLast");
		}
		return position->getLeft();
	}

	virtual bool isElement(E p) override {
		bool esDeLista = false;
		if (!isEmpty() && p != nullptr) {
			Position<E>* iterador = first();
			while (!esDeLista && iterador != nullptr) {
				if (iterador->getElement() == p) {
					esDeLista = true;
				}
				else {
					iterador = (iterador != last()) ? next(iterador) : nullptr;
				}
			}
		}
		return esDeLista;
	}

	virtual Position<E>* whatElementPosition(E p) override {
		Position<E>* iterador = nullptr;
		if (!isEmpty() && p != nullptr) {
			bool esDeLista = false;
			iterador = first();
			while (!esDeLista && iterador != nullptr) {
				if (iterador->getElement() == p) {
					esDeLista = true;
				}
				else {
					iterador = (iterador != last()) ? next(iterador) : nullptr;
				}
			}
		}
		return iterador;
	}

	virtual void clear() override {
		if (!isEmpty()) {
			Position<E>* position = first();
			while (position != nullptr) {
				Position<E>* aux = position;
				position = (position != last()) ? next(position) : nullptr;
				remove(aux);
			}
			this->front->setRight(this->tail);
			this->tail->setLeft(this->front);
			size = 0;
		}
	}

	virtual void deleteByElement(E e) {
		Position<E>* iterador = nullptr;
		if (!isEmpty() && e != nullptr) {
			bool esDeLista = false;
			iterador = first();
			while (!esDeLista && iterador != nullptr) {
				if (iterador->getElement() == e) {
					esDeLista = true;
					remove(iterador);
				}
				else {
					iterador = (iterador != last()) ? next(iterador) : nullptr;
				}
			}
		}
	}

	virtual void swapPositions(Position<E>* p1, Position<E>* p2) override {
		DNodo<E>* nodo1 = checkPosition(p1);
		DNodo<E>* nodo2 = checkPosition(p2);

		if (p1 == p2 || size <= 1) {
			throw InvalidOperationException("LosParametrosSonIgualesONoHaySuficientes");
		}

		if (nodo1->getRight() == nodo2) {
			// nodo1 está antes de nodo2
			DNodo<E>* prev = nodo1->getLeft();
			DNodo<E>* next = nodo2->getRight();

			prev->setRight(nodo2);
			nodo2->setLeft(prev);

			nodo2->setRight(nodo1);
			nodo1->setLeft(nodo2);

			nodo1->setRight(next);
			next->setLeft(nodo1);
		}
		else if (nodo2->getRight() == nodo1) {
			// nodo2 está antes de nodo1
			swapPositions(p2, p1);  // Reutilizamos el caso anterior
		}
		else {
			// Caso general: no son adyacentes
			// Guardamos referencias de vecinos
			DNodo<E>* nodo1Prev = nodo1->getLeft();
			DNodo<E>* nodo1Next = nodo1->getRight();
			DNodo<E>* nodo2Prev = nodo2->getLeft();
			DNodo<E>* nodo2Next = nodo2->getRight();

			// Reenlazar nodo1 en posición de nodo2
			nodo1Prev->setRight(nodo2);
			nodo1Next->setLeft(nodo2);
			nodo2->setLeft(nodo1Prev);
			nodo2->setRight(nodo1Next);

			// Reenlazar nodo2 en posición de nodo1
			nodo2Prev->setRight(nodo1);
			nodo2Next->setLeft(nodo1);
			nodo1->setLeft(nodo2Prev);
			nodo1->setRight(nodo2Next);
		}

	}  
};
#endif
