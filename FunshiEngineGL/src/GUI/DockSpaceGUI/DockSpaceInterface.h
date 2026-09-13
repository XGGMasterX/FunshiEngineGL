#ifndef DOCKSPACEINTERFACE_H
#define DOCKSPACEINTERFACE_H

#include "../GeneralUserInterface.h"
#include <imgui.h>

class DockSpaceInterface : public GeneralUserInterface {
protected:
	ImGuiID dockspaceId = 0;
public:
	DockSpaceInterface(bool stateGUI);
	virtual void initGUI() override;
	virtual void contentGUI() override;
	virtual void endGUI() override;
	virtual void printGUI() override;
};
#endif