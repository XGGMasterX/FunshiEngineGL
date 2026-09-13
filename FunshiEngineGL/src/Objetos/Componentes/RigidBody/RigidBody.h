#ifndef RIGIDBODY_H
#define RIGIDBODY_H
#include "../Colliders/Collider.h"

// El cuerpo fisico de bullet se usa por puntero; el include pesado de bullet
// solo lo necesita RigidBody.cpp.
class btRigidBody;

class RigidBody : public Component {
private:
    btRigidBody* rigidBody = nullptr;
    Collider* collider = nullptr;
    float mass = 1.0f;
    // Guarda la posicion y rotacion para serializar
    float pos[3];
    float rot[4]; // quaternion x,y,z,w

    void serializeComponent(std::ofstream* fileNamePathContentObject) override;
    void deserializeComponent(std::ifstream* fileNamePathContentObject) override;

public:
    RigidBody(Collider* collider, float mass);
    ~RigidBody();

    void createRigidBody();

    // ALTERAR EL DAD TRANSFORM
    void syncPhysicsToGameObject();

    void saveComponent(std::ofstream* fileNamePathContentObject) override;
    void loadComponent(std::ifstream* fileNamePathContentObject) override;

    btRigidBody* getRigidBody() { return rigidBody; }
};
#endif