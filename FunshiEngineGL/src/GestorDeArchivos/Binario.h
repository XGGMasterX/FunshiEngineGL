#ifndef BINARIO_H
#define BINARIO_H

#include <fstream>
#include <string>

class Binario {
private:
    std::string path;
    std::ofstream* ofBin;
    std::ifstream* ifBin;

public:
    explicit Binario(std::string path);
    ~Binario();

    Binario(const Binario&) = delete;
    Binario& operator=(const Binario&) = delete;

    std::string getPath();
    void ofOpenBinary();
    void ifOpenBinary();
    void ofCloseBinary();
    void ifCloseBinary();
    std::ofstream* getOfBinariFile();
    std::ifstream* getIfBinariFile();
};

#endif
