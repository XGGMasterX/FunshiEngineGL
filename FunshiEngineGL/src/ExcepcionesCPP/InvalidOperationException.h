#ifndef INVALIDOPERATIONEXCEPTION_H
#define INVALIDOPERATIONEXCEPTION_H
#include "RuntimeException.h"
using namespace std;

class InvalidOperationException : public RuntimeException {
public:
	InvalidOperationException(string msg) : RuntimeException(msg + " No es posible realizar la Operacion") {}
};
#endif