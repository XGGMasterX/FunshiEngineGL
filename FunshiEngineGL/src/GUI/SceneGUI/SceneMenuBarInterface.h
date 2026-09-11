#ifndef SCENEMENUBARINTERFACE_H
#define SCENEMENUBARINTERFACE_H
#include <iostream>
#include <string>
#include "../ObjetosGUI/SettingsObjectInterface.h"
#include <imgui.h>

using namespace std;

class SceneMenuBarInterface : public GeneralUserInterface {
protected:
    bool* toggleBool = nullptr;
    bool cargarScripts = false;

public:
    SceneMenuBarInterface(bool stateGUI);
    void setActivador(bool* targetBool);
    bool* getActivador();
    bool getCargarScripts();
    void setCargarScripts(bool value);
    virtual void initGUI() override;
    virtual void contentGUI() override;
    virtual void endGUI() override;
    virtual void printGUI() override;
};
#endif
