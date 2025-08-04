#ifndef FILE_H
#define FILE_H
#include <iostream>
using namespace std;

class File{
protected:
	string pathRoot;//sin barra final
	string pathName;//sin barra inicial
	//tam del archivo
	//extencion del arhivo
public:
	File(string pathName) {
		this->pathName = pathName;
	}

	string getPathRoot() { return pathRoot; }
	string getPathName() { return pathName; }
	string setPathRoot(string pathRoot) {
		this->pathRoot = pathRoot;
	}
	//generar archivo
	//metodo virtual
	//re escribir metodo para hijos
};
#endif