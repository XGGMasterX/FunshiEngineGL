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

	virtual string getPathRoot() { return pathRoot; }
	virtual string getPathName() { return pathName; }
	virtual void setPathRoot(string pathRoot) {
		this->pathRoot = pathRoot;
	}
	//generar archivo
	//metodo virtual
	//re escribir metodo para hijos
};
#endif