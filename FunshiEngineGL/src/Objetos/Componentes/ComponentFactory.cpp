#include "ComponentFactory.h"

#include "Colliders/CubeCollider.h"
#include "Colliders/EsfereCollider.h"
#include "Colliders/MallaCollider.h"
#include "CameraComponent.h"
#include "Color.h"
#include "Light.h"
#include "Material.h"
#include "Model.h"
#include "RigidBody/RigidBody.h"
#include "Script.h"
#include "Transform.h"
#include "../GameObject.h"

std::unique_ptr<Component> ComponentFactory::create(const std::string& typeName,
                                                    GameObject& owner) {
    Transform* transform = owner.getComponent<Transform>();

    if (typeName == "Transform") return std::make_unique<Transform>();
    if (typeName == "Color") return std::make_unique<Color>();
    if (typeName == "Material") return std::make_unique<Material>();
    if (typeName == "Light") return std::make_unique<Light>();
    // El nombre con el que se serializa es el demangle RTTI de la clase
    // ("CameraComponent"); "Camera" se conserva como alias hacia atras.
    if (typeName == "CameraComponent" || typeName == "Camera")
        return std::make_unique<CameraComponent>();
    if (typeName == "EsfereCollider" && transform)
        return std::make_unique<EsfereCollider>(5.0f, transform, &owner);
    if (typeName == "CubeCollider" && transform)
        return std::make_unique<CubeCollider>(5.0f, transform, &owner);
    if (typeName == "MallaCollider" && transform)
        return std::make_unique<MallaCollider>(5.0f, transform, &owner);
    if (typeName == "RigidBody") {
        Collider* collider = owner.getComponent<Collider>();
        if (collider) return std::make_unique<RigidBody>(collider, 1.0f);
    }
    if (typeName == "Script") return std::make_unique<Script>();
    if (typeName == "Model") return std::make_unique<Model>();

    return nullptr;
}
