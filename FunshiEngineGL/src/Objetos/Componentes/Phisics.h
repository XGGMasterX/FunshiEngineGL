#ifndef PHISICS_H
#define PHISICS_H

#include "Component.h"
#include "Transform.h"
#include <array>


class Phisics : public Component {

private:
	float gravity;
public:
	Phisics() {

	}

	void inercia(/*DATOS FISICOS NO DE OBJETO */ float x, float y, float z, Transform* trObj) {
		 
	}

	void reaccion(/*DATOS FISICOS NO DE OBJETO */ float x, float y, float z, Transform* trObj) {

	}

	void dinamica(/*DATOS FISICOS NO DE OBJETO */ float x, float y, float z, Transform* trObj) {

	}

	void fuerza(float x, float y, float z, Transform* trObj) {
	}



};

#endif