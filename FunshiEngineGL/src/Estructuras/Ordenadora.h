#ifndef ORDENADORA_H
#define ORDENADORA_H
#include "Comparable.h"

using namespace std;

template<typename E>
class Ordenadora {
public:
	virtual bool ordenBy(E p1, E p2) const = 0;
};
#endif
