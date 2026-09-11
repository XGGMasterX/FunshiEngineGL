#include "Camera.h"

#include <GL/glu.h>
#include <cmath>
#include <cstring>

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
    const vec3 f = m_dir;
    const vec3 s = m_left;
    const vec3 u(f.y*s.z - f.z*s.y, f.z*s.x - f.x*s.z, f.x*s.y - f.y*s.x);
    std::memset(outMatrix, 0, sizeof(float) * 16);
    outMatrix[0]=s.x; outMatrix[4]=s.y; outMatrix[8]=s.z;
    outMatrix[1]=u.x; outMatrix[5]=u.y; outMatrix[9]=u.z;
    outMatrix[2]=-f.x; outMatrix[6]=-f.y; outMatrix[10]=-f.z;
    outMatrix[12]=-(s.x*m_pos.x+s.y*m_pos.y+s.z*m_pos.z);
    outMatrix[13]=-(u.x*m_pos.x+u.y*m_pos.y+u.z*m_pos.z);
    outMatrix[14]=f.x*m_pos.x+f.y*m_pos.y+f.z*m_pos.z;
    outMatrix[15]=1.0f;
}

void Camera::getProjectionMatrix(float* outMatrix, float fov, float aspect,
                                 float nearPlane, float farPlane) const {
    const float f = 1.0f / std::tanf(fov * 0.5f * (m_PI / 180.0f));
    std::memset(outMatrix, 0, sizeof(float) * 16);
    outMatrix[0]=f/aspect; outMatrix[5]=f;
    outMatrix[10]=(farPlane+nearPlane)/(nearPlane-farPlane);
    outMatrix[11]=-1.0f;
    outMatrix[14]=(2.0f*farPlane*nearPlane)/(nearPlane-farPlane);
}
