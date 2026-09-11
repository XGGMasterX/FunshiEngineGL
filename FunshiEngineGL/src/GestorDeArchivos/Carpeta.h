#ifndef FOLDER_H
#define FOLDER_H

#include "File.h"

class Carpeta : public File {
protected:
    bool open;

public:
    explicit Carpeta(std::string pathName);
    bool isOpen();
    void setStateOpenOrClose(bool open);
};

#endif
