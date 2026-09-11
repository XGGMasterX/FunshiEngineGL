#include "Camera.h"

#include <GL/glu.h>
#include <cmath>
#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Camera::Camera(vec3 pos, float speed) : m_pos(pos), m_speed(speed), m_yawX(0), m_yawY(0) {
    m_dir = vec3(0, 0, -1);
    m_left = vec3(-1, 0, 0);
    m_up = vec3(0, 1, 0);
}

void Camera::activate() {
    const vec3 lookAt(m_pos.x + m_dir.x, m_pos.y + m_dir.y, m_pos.z + m_dir.z);
    gluLookAt(m_pos.x, m_pos.y, m_pos.z, lookAt.x, lookAt.y, lookAt.z,
              m_up.x, m_up.y, m_up.z);
}

void Camera::move(vec3 direction, float velocity) {
    m_pos.x += direction.x * velocity;
    m_pos.y += direction.y * velocity;
    m_pos.z += direction.z * velocity;
}
void Camera::forward(float dt) { move(m_dir, m_speed * dt); }
void Camera::back(float dt) { move(m_dir, -m_speed * dt); }
void Camera::left(float dt) { move(m_left, m_speed * dt); }
void Camera::right(float dt) { move(m_left, -m_speed * dt); }
void Camera::up(float dt) { move(m_up, m_speed * dt); }
void Camera::down(float dt) { move(m_up, -m_speed * dt); }

void Camera::forwardRight(float dt) {
    const float v = m_speed * dt;
    m_pos.x += (m_dir.x - m_left.x) * v; m_pos.y += (m_dir.y - m_left.y) * v; m_pos.z += (m_dir.z - m_left.z) * v;
}
void Camera::forwardLeft(float dt) {
    const float v = m_speed * dt;
    m_pos.x += (m_dir.x + m_left.x) * v; m_pos.y += (m_dir.y + m_left.y) * v; m_pos.z += (m_dir.z + m_left.z) * v;
}
void Camera::backRight(float dt) {
    const float v = m_speed * dt;
    m_pos.x += (-m_dir.x - m_left.x) * v; m_pos.y += (-m_dir.y - m_left.y) * v; m_pos.z += (-m_dir.z - m_left.z) * v;
}
void Camera::backLeft(float dt) {
    const float v = m_speed * dt;
    m_pos.x += (-m_dir.x + m_left.x) * v; m_pos.y += (-m_dir.y + m_left.y) * v; m_pos.z += (-m_dir.z + m_left.z) * v;
}

void Camera::updateYaw(float dYawX, float dYawY) {
    m_yawX += dYawX;
    m_yawY += dYawY;
    if (m_yawY > 89.0f) m_yawY = 89.0f;
    if (m_yawY < -89.0f) m_yawY = -89.0f;
}

void Camera::update() {
    const float radX = toRadians(m_yawX), radY = toRadians(m_yawY);
    m_dir.x = std::sinf(radX) * std::cosf(radY);
    m_dir.y = -std::sinf(radY);
    m_dir.z = -std::cosf(radX) * std::cosf(radY);
    float length = std::sqrt(m_dir.x*m_dir.x + m_dir.y*m_dir.y + m_dir.z*m_dir.z);
    if (length > 0) { m_dir.x /= length; m_dir.y /= length; m_dir.z /= length; }
    m_left.x = m_up.y*m_dir.z - m_up.z*m_dir.y;
    m_left.y = m_up.z*m_dir.x - m_up.x*m_dir.z;
    m_left.z = m_up.x*m_dir.y - m_up.y*m_dir.x;
    length = std::sqrt(m_left.x*m_left.x + m_left.y*m_left.y + m_left.z*m_left.z);
    if (length > 0) { m_left.x /= length; m_left.y /= length; m_left.z /= length; }
}

float Camera::toRadians(float degrees) { return degrees * (m_PI / 180.0f); }
vec3 Camera::getPosition() const { return m_pos; }
vec3 Camera::getDirection() const { return m_dir; }

void Camera::getViewMatrix(float* outMatrix) const {
    glm::vec3 eye(m_pos.x, m_pos.y, m_pos.z);
    glm::vec3 center(m_pos.x + m_dir.x, m_pos.y + m_dir.y, m_pos.z + m_dir.z);
    glm::vec3 up(m_up.x, m_up.y, m_up.z);
    glm::mat4 v = glm::lookAt(eye, center, up);
    const float* ptr = glm::value_ptr(v);
    std::memcpy(outMatrix, ptr, sizeof(float) * 16);
}

void Camera::getProjectionMatrix(float* outMatrix, float fov, float aspect,
                                 float nearPlane, float farPlane) const {
    glm::mat4 p = glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
    const float* ptr = glm::value_ptr(p);
    std::memcpy(outMatrix, ptr, sizeof(float) * 16);
}
