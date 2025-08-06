#ifndef FOLDER_H
#define FOLDER_H
#include <iostream>
#include "../Estructuras/ListasEnlazadas/ListasDoblementeEnlazada/ListaDE.h"
#include "File.h"
using namespace std;

class Folder : public File {
protected:
	bool open;
public:
	Folder(string pathName) : File(pathName) {}

	bool isOpen() {
		return open;
	}
	void setStateOpenOrClose(bool open) {
		this->open = open;
	}
	//crear folder
};
#endif