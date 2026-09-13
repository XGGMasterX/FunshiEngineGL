#ifndef SCRIPT_H
#define SCRIPT_H

#include "Component.h"
#include <string>

class Script : public Component {
private:
    std::string dllPath;
    std::string nameClass;
protected:
    void serializeComponent(std::ofstream* fileNamePathContentObject) override;
    void deserializeComponent(std::ifstream* fileNamePathContentObject) override;

public:
    void setDllPath(std::string dllPath);

    std::string getPath() { return dllPath; }

    std::string getNameClass() { return nameClass; }

    void saveComponent(std::ofstream* fileNamePathContentObject) override;
    void loadComponent(std::ifstream* fileNamePathContentObject) override;
};
#endif