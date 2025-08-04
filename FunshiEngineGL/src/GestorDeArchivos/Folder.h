#ifndef FOLDER_H
#define FOLDER_H
#include <iostream>
#include "../Estructuras/ListasEnlazadas/ListasDoblementeEnlazada/ListaDE.h"
#include "File.h"
using namespace std;

class Folder : public File {
protected:

public:
	Folder(string pathName) : File(pathName) {}

	//crear folder
};
#endif