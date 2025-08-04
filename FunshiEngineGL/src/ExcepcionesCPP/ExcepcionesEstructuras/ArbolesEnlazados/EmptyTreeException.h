#ifndef EMPTYTREEEXCEPTION_H
#define EMPTYTREEXCEPTION_H
using namespace std;
#include "../../RuntimeException.h"
class EmptyTreeException : public RuntimeException {

public:
	EmptyTreeException(string msg) : RuntimeException(msg + " Arbol Vacio") {}
};
#endif