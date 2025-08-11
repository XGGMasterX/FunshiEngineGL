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

class Camera : public Gizmo{
private:
    vec3 m_pos;
    vec3 m_dir;
    vec3 m_left;
    vec3 m_up;
    float m_speed;
    float m_yawX;
    float m_yawY;
    const float m_PI = 3.14159265f;

public:
    Camera(vec3 pos = vec3(0, 0, 0), float speed = 5.0f);
    void activate();

    // Movimientos básicos
    void forward(float deltaTime);
    void back(float deltaTime);
    void left(float deltaTime);
    void right(float deltaTime);
    void up(float deltaTime);
    void down(float deltaTime);

    // Movimientos combinados
    void forwardRight(float deltaTime);
    void forwardLeft(float deltaTime);
    void backRight(float deltaTime);
    void backLeft(float deltaTime);

    void updateYaw(float dYawX, float dYawY);
    void update();

    vec3 getPosition() const { return m_pos; }
    vec3 getDirection() const { return m_dir; }

private:
    float toRadians(float degrees);
    void move(vec3 direction, float velocity);
};

// Implementación
Camera::Camera(vec3 pos, float speed) : m_speed(speed), m_yawX(0), m_yawY(0) {
    m_pos = pos;
    m_dir = vec3(0, 0, -1);
    m_left = vec3(-1, 0, 0);
    m_up = vec3(0, 1, 0);
}

void Camera::activate() {
    vec3 lookAt;
    lookAt.x = m_pos.x + m_dir.x;
    lookAt.y = m_pos.y + m_dir.y;
    lookAt.z = m_pos.z + m_dir.z;
    gluLookAt(m_pos.x, m_pos.y, m_pos.z,
        lookAt.x, lookAt.y, lookAt.z,
        m_up.x, m_up.y, m_up.z);
}

void Camera::move(vec3 direction, float velocity) {
    m_pos.x += direction.x * velocity;
    m_pos.y += direction.y * velocity;
    m_pos.z += direction.z * velocity;
}

void Camera::forward(float deltaTime) {
    move(m_dir, m_speed * deltaTime);
}

void Camera::back(float deltaTime) {
    move(m_dir, -m_speed * deltaTime);
}

void Camera::left(float deltaTime) {
    move(m_left, m_speed * deltaTime);
}

void Camera::right(float deltaTime) {
    move(m_left, -m_speed * deltaTime);
}

void Camera::up(float deltaTime) {
    move(m_up, m_speed * deltaTime);
}

void Camera::down(float deltaTime) {
    move(m_up, -m_speed * deltaTime);
}

// Movimientos combinados
void Camera::forwardRight(float deltaTime) {
    float velocity = m_speed * deltaTime;
    m_pos.x += (m_dir.x - m_left.x) * velocity;
    m_pos.y += (m_dir.y - m_left.y) * velocity;
    m_pos.z += (m_dir.z - m_left.z) * velocity;
}

void Camera::forwardLeft(float deltaTime) {
    float velocity = m_speed * deltaTime;
    m_pos.x += (m_dir.x + m_left.x) * velocity;
    m_pos.y += (m_dir.y + m_left.y) * velocity;
    m_pos.z += (m_dir.z + m_left.z) * velocity;
}

void Camera::backRight(float deltaTime) {
    float velocity = m_speed * deltaTime;
    m_pos.x += (-m_dir.x - m_left.x) * velocity;
    m_pos.y += (-m_dir.y - m_left.y) * velocity;
    m_pos.z += (-m_dir.z - m_left.z) * velocity;
}

void Camera::backLeft(float deltaTime) {
    float velocity = m_speed * deltaTime;
    m_pos.x += (-m_dir.x + m_left.x) * velocity;
    m_pos.y += (-m_dir.y + m_left.y) * velocity;
    m_pos.z += (-m_dir.z + m_left.z) * velocity;
}

void Camera::updateYaw(float dYawX, float dYawY) {
    m_yawX += dYawX;  // Rotación horizontal (siempre libre)

    // Limitar ángulo vertical (entre -89° y +89° para evitar inversión)
    const float maxPitch = 89.0f;
    m_yawY += dYawY;
    if (m_yawY > maxPitch) m_yawY = maxPitch;
    if (m_yawY < -maxPitch) m_yawY = -maxPitch;
}

void Camera::update() {
    float radX = toRadians(m_yawX);  // Ángulo horizontal (yaw)
    float radY = toRadians(m_yawY);  // Ángulo vertical (pitch, limitado)

    // Calcular nueva dirección
    m_dir.x = sinf(radX) * cosf(radY);
    m_dir.y = -sinf(radY);  // Negativo porque en OpenGL Y+ es arriba
    m_dir.z = -cosf(radX) * cosf(radY);

    // Normalizar dirección
    float length = sqrtf(m_dir.x * m_dir.x + m_dir.y * m_dir.y + m_dir.z * m_dir.z);
    if (length > 0) {
        m_dir.x /= length;
        m_dir.y /= length;
        m_dir.z /= length;
    }

    // Recalcular vector izquierda (producto cruz entre up y dirección)
    m_left.x = m_up.y * m_dir.z - m_up.z * m_dir.y;
    m_left.y = m_up.z * m_dir.x - m_up.x * m_dir.z;
    m_left.z = m_up.x * m_dir.y - m_up.y * m_dir.x;

    // Normalizar izquierda
    length = sqrtf(m_left.x * m_left.x + m_left.y * m_left.y + m_left.z * m_left.z);
    if (length > 0) {
        m_left.x /= length;
        m_left.y /= length;
        m_left.z /= length;
    }
}

float Camera::toRadians(float degrees) {
    return degrees * (m_PI / 180.0f);
}

#endif