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
			throw EmptyListException("ListaDE::first:NoHayPrimerNodo");
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
			while (!esDeLista) {
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
			while (!esDeLista) {
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
};
#endif