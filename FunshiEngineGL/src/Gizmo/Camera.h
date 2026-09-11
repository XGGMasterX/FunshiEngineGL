#ifndef CAMERA_H
#define CAMERA_H

#if defined(_WIN32)
#include <glfw3.h>
#elif defined(__linux__)
#include <GLFW/glfw3.h>
#endif

#include "../Matematicas/StructVec3.h"
#include "../Ventana.h"
#include "Gizmo.h"

class Camera : public Gizmo {
private:
    vec3 m_pos;
    vec3 m_dir;
    vec3 m_left;
    vec3 m_up;
    float m_speed;
    float m_yawX;
    float m_yawY;
    const float m_PI = 3.14159265f;

    float toRadians(float degrees);
    void move(vec3 direction, float velocity);

public:
    Camera(vec3 pos = vec3(0, 0, 0), float speed = 5.0f);
    void activate();
    void forward(float deltaTime);
    void back(float deltaTime);
    void left(float deltaTime);
    void right(float deltaTime);
    void up(float deltaTime);
    void down(float deltaTime);
    void forwardRight(float deltaTime);
    void forwardLeft(float deltaTime);
    void backRight(float deltaTime);
    void backLeft(float deltaTime);
    void updateYaw(float dYawX, float dYawY);
    void update();
    vec3 getPosition() const;
    vec3 getDirection() const;
    void getViewMatrix(float* outMatrix) const;
    void getProjectionMatrix(float* outMatrix, float fov, float aspect,
                             float nearPlane = 0.1f, float farPlane = 1000.0f) const;
};

#endif
