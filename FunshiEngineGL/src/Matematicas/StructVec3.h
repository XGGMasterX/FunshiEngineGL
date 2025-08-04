#ifndef STRUCTVEC3_H
#define STRUCTVEC3_H


#include<cmath>
#include<iostream>

struct vec3 {
	float x, y, z;
	vec3();
	vec3(float x_, float y_, float z_);
	vec3 operator+(const vec3& v)const;
	vec3 operator-(const vec3& v)const;
	vec3 operator*(float k)const;
	float mag();
	void normaliza();
	float magnitude() const;
	float distance(const vec3& v) const;
	vec3 prodVetorial(vec3 v);
	float dot(const vec3& v) const;
	void print();
};


struct vec2 {
	float x, y;
	vec2() :x(0.0), y(0.0) {}
	vec2(float x_, float y_) :x(x_), y(y_) {}
};


#endif