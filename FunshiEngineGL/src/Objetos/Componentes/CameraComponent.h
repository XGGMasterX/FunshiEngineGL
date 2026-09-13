#ifndef CAMERACOMPONENT_H
#define CAMERACOMPONENT_H

#include "Component.h"

class GameObject;
class Transform;

// Camara como Component (mismo patron que Light): se adjunta a un GameObject,
// normalmente un objeto vacio con su Transform. La posicion/orientacion de la
// vista se deriva del Transform global del duenio; la navegacion (WASD + mouse
// FPS) mantiene el estado interno y lo sincroniza de vuelta al Transform, de
// modo que el gizmo y el inspector pueden editar la camara como a cualquier
// otro objeto.
class CameraComponent : public Component {
private:
    GameObject* owner = nullptr;
    float m_pos[3] = {0.f, 0.f, 0.f};
    float m_dir[3] = {0.f, 0.f, -1.f};
    float m_left[3] = {-1.f, 0.f, 0.f};
    float m_up[3] = {0.f, 1.f, 0.f};
    float speed = 5.0f;
    float yawX = 0.0f;
    float yawY = 0.0f;
    float fov = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    bool pintar = false;

    Transform* getLocalTransform() const;
    void leerDesdeTransform();
    void escribirATransform();
    void calculardireccion();
    void move(const float direction[3], float velocity);

protected:
    void serializeComponent(std::ofstream* file) override;
    void deserializeComponent(std::ifstream* file) override;

public:
    CameraComponent();

    void setUp(GameObject* objeto) { owner = objeto; }

    void saveComponent(std::ofstream* file) override;
    void loadComponent(std::ifstream* file) override;

    // Vista actual segun el Transform del duenio (nunca cachead) : el gizmo,
    // el picking y el render comparten la misma fuente de verdad.
    void activar();
    void getViewMatrix(float* outMatrix) const;
    void getProjectionMatrix(float* outMatrix, float aspect) const;

    // Navegacion FPS (misma API que el viejo Camera).
    void forward(float dt);
    void back(float dt);
    void left(float dt);
    void right(float dt);
    void up(float dt);
    void down(float dt);
    void forwardRight(float dt);
    void forwardLeft(float dt);
    void backRight(float dt);
    void backLeft(float dt);
    void updateYaw(float dYawX, float dYawY);

    const float* getPosition() const { return m_pos; }

    float getFov() const { return fov; }
    void setFov(float v) { fov = v; }
    float getNearPlane() const { return nearPlane; }
    void setNearPlane(float v) { nearPlane = v; }
    float getFarPlane() const { return farPlane; }
    void setFarPlane(float v) { farPlane = v; }
    float getSpeed() const { return speed; }
    void setSpeed(float v) { speed = v; }

    // Vista previa viva: el motor pinta la escena desde esta camara a una
    // textura y la muestra en una ventana ImGui (Fase 2).
    bool getPintar() const { return pintar; }
    void setPintar(bool v) { pintar = v; }
};
#endif