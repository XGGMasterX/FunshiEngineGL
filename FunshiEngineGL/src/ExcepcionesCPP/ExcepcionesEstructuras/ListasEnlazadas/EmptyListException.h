#ifndef EMPTYLISTEXCEPTION_H
#define EMPTYLISTEXCEPTION_H
using namespace std;
#include "../../RuntimeException.h"
class EmptyListException : public RuntimeException {

public:
	EmptyListException(string msg) : RuntimeException(msg + " Lista Vacia") {}
};

#endif