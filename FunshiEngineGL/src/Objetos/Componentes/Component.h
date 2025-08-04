#ifndef COMPONENT_H
#define COMPONENT_H

using namespace std;

class Component {
private:
	bool terminalSelectScript = false;
	char name[25] = "";
protected:
	virtual void serializeComponent(std::ofstream* fileNamePathContentObject) = 0;

	virtual void deserializeComponent(std::ifstream* fileNamePathContentObject) = 0;
public:
	bool settingsObjectComponent = false;
	virtual ~Component() {} // Destructor virtual para hacer que la clase sea polimórfica.

	virtual void saveComponent(std::ofstream* fileNamePathContentObject) = 0;

	virtual void loadComponent(std::ifstream* fileNamePathContentObject) = 0;

};
#endif
