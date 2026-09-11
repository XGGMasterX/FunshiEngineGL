#ifndef FILE_H
#define FILE_H

#include <string>

class File {
protected:
    std::string pathRoot;
    std::string pathName;

public:
    explicit File(std::string pathName);
    virtual ~File() = default;

    virtual std::string getPathRoot();
    virtual std::string getPathName();
    virtual void setPathRoot(std::string pathRoot);
};

#endif
