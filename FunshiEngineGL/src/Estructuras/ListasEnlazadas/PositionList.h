#ifndef POSITIONLIST_H
#define POSITIONLIST_H

#include <iostream>
#include "../../Estructuras/Position/Position.h"

using namespace std;

template<typename E>
class PositionList {
public:
	virtual bool isEmpty() = 0;
	virtual int tam() = 0;
	virtual bool isElement(E p) = 0;
	virtual Position<E>* whatElementPosition(E p) = 0;

	virtual Position<E>* first() = 0;
	virtual Position<E>* last() = 0;

	virtual void addFirst(E e) = 0;
	virtual void addLast(E e) = 0;
	virtual void addAfter(Position<E>* pl, E e) = 0;
	virtual void addBefore(Position<E>* pr, E e) = 0;

	virtual Position<E>* next(Position<E>* p) = 0;
	virtual Position<E>* prev(Position<E>* p) = 0;
	virtual E remove(Position<E>* p) = 0;
	virtual E remplace(Position<E>* p, E e) = 0;
	virtual void clear() = 0;
};
#endif