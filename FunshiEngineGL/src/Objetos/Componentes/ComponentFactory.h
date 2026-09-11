#ifndef COMPONENT_FACTORY_H
#define COMPONENT_FACTORY_H

#include <memory>
#include <string>

class Component;
class GameObject;

class ComponentFactory {
public:
    static std::unique_ptr<Component> create(const std::string& typeName,
                                             GameObject& owner);
};

#endif
