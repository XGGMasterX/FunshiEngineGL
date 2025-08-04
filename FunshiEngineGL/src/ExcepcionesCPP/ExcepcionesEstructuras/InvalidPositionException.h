#ifndef INVALIDPOSITIONEXCEPTION_H
#define INVALIDPOSITIONEXCEPTION_H
#include "../RuntimeException.h"
using namespace std;

class InvalidPositionException : public RuntimeException {
   public:
	   InvalidPositionException(string msg) : RuntimeException(msg + " Posicion invalida"){}
};
#endif