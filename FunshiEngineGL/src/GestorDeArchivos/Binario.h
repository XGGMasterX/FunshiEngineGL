#ifndef BINARIO_H
#define BINARIO_H

#include <iostream>
#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#endif
#include <fstream>
using namespace std;


class Binario {

private:
	string path;
	ofstream* ofBin;
	ifstream* ifBin;

public:
	Binario(string path) {
		this->path = path;
	}

	string getPath() {
		return path;
	}

	void ofOpenBinary() {
		remove(path.c_str());
		ofBin = new ofstream(path, ios::binary);
	}

	void ifOpenBinary() {
		ifBin = new ifstream(path, ios::binary);
	}

	void ofCloseBinary() {
		ofBin->close();
	}

	void ifCloseBinary() {
		ifBin->close();
	}

	ofstream* getOfBinariFile() {
		return ofBin;
	}

	ifstream* getIfBinariFile() {
		return ifBin;
	}
};
#endif
