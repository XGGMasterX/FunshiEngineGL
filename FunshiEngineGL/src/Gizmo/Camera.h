#ifndef CAMERA_H
#define CAMERA_H

#if defined(_WIN32)
#include <glfw3.h>
#elif defined(__linux__)
#include <GLFW/glfw3.h>
#endif
#include <iostream>
#include "../Matematicas/StructVec3.h"
#include "../Ventana.h"
#include "../Estructuras/Ordenadora.h"
#include "../Objetos/GameObject.h"

using namespace std;

class Camera : public GameObject {
protected:
	vec3 m_pos;
	vec3 m_dir;
	vec3 m_left;
	vec3 m_up;
	vec3 m_veloc;
	float m_scl = 0.25;
	float m_yawX;
	float m_yawY;
	float m_PI = 3.14159265;

public:
	Camera(vec3 pos);
	~Camera();
	void activar();

	void forward();
	void back();
	void left();
	void right();
	void up();
	void down();

	void forwardRight();
	void forwardLeft();

	void backRight();
	void backLeft();

	void updateYaw(float dYawX, float dYawY);
	void update(); 

	void dibujar() override;

private:
	float cvtToRad(float ang);
};

Camera::Camera(vec3 pos)
{
	m_pos = vec3(pos);

	m_dir = vec3(0, 0, -1);   //z
	m_left = vec3(-1, 0, 0);  //x
	m_up = vec3(0, 1, 0);     //y

	m_veloc = vec3(0, 0, 0);
}


Camera::~Camera()
{
}

void Camera::dibujar()
{
}

void Camera::activar()
{
	vec3 look = m_pos + m_dir;
	gluLookAt(m_pos.x, m_pos.y, m_pos.z, look.x, look.y, look.z, m_up.x, m_up.y, m_up.z);
}

void Camera::forward()
{
	m_veloc = m_dir * m_scl;
	m_pos = m_pos + m_veloc;
}

void Camera::back()
{
	m_veloc = m_dir * (-m_scl);
	m_pos = m_pos + m_veloc;
}

void Camera::left()
{
	m_veloc = m_left * m_scl;
	m_pos = m_pos + m_veloc;
}

void Camera::right()
{
	m_veloc = m_left * (-m_scl);
	m_pos = m_pos + m_veloc;
}

void Camera::down()
{
	m_veloc = m_up * (-m_scl);
	m_pos = m_pos + m_veloc;
}

void Camera::up()
{
	m_veloc = m_up * m_scl;
	m_pos = m_pos + m_veloc;
}

void Camera::forwardRight() {
	forward();
	right();
}

void Camera::forwardLeft() {
	forward();
	left();
}

void Camera::backRight() {
	back();
	right();
}

void Camera::backLeft() {
	back();
	left();
}

void Camera::updateYaw(float dYawX, float dYawY)
{
	m_yawX += dYawX;
	m_yawY += dYawY;
}

void Camera::update()
{
	float angX = cvtToRad(m_yawX);
	float angY = cvtToRad(m_yawY);
	m_dir.x = sin(angX);
	m_dir.z = -cos(angX);
	m_dir.y = (-sin(angY)) + cos(angY);
	m_dir.normaliza();
	m_left = m_up.prodVetorial(m_dir);
}

float Camera::cvtToRad(float ang)
{
	return (ang * m_PI) / 180.0;
}

#endif
