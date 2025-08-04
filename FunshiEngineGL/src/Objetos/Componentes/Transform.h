#ifndef TRANSFORM_H
#define TRANSFORM_H


#include "../Componentes/Component.h"
using namespace std;



class Transform : public Component {
private:
	float objectTranslatef[3] = { 0.0f, 0.0f, 0.0f };
	float objectScalef[3] = { 1.0f, 1.0f, 1.0f };
	float objectRotatef[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	void serializeComponent(std::ofstream* fileNamePathContentObject) {
		fileNamePathContentObject->write(reinterpret_cast<const char*>(&objectTranslatef), sizeof(float) * 3);
		fileNamePathContentObject->write(reinterpret_cast<const char*>(&objectScalef), sizeof(float) * 3);
		fileNamePathContentObject->write(reinterpret_cast<const char*>(&objectRotatef), sizeof(float) * 4);
	}

	void deserializeComponent(std::ifstream* fileNamePathContentObject) {
		fileNamePathContentObject->read(reinterpret_cast<char*>(&objectTranslatef), sizeof(float) * 3);
		fileNamePathContentObject->read(reinterpret_cast<char*>(&objectScalef), sizeof(float) * 3);
		fileNamePathContentObject->read(reinterpret_cast<char*>(&objectRotatef), sizeof(float) * 4);

		// Actualizar los arrays auxiliares
		for (int i = 0; i < 3; i++) {
			arrTranslatef[i] = objectTranslatef[i];
			arrScalef[i] = objectScalef[i];
		}
		for (int i = 0; i < 4; i++) {
			arrRotatef[i] = objectRotatef[i];
		}
	}


public:
	Transform() {
		// Inicializar arrays auxiliares
		for (int i = 0; i < 3; i++) {
			arrTranslatef[i] = objectTranslatef[i];
			arrScalef[i] = objectScalef[i];
		}
		for (int i = 0; i < 4; i++) {
			arrRotatef[i] = objectRotatef[i];
		}
	}

	float arrTranslatef[3], arrScalef[3], arrRotatef[4];



	void setTranslatef(float x, float y, float z) {
		objectTranslatef[0] = x;
		objectTranslatef[1] = y;
		objectTranslatef[2] = z;

		// Actualizar el array auxiliar
		for (int i = 0; i < 3; i++) {
			arrTranslatef[i] = objectTranslatef[i];
		}

	}

	void setRotatef(float angle, float x, float y, float z) {
		objectRotatef[0] = angle;
		objectRotatef[1] = x;
		objectRotatef[2] = y;
		objectRotatef[3] = z;

		// Actualizar el array auxiliar
		for (int i = 0; i < 4; i++) {
			arrRotatef[i] = objectRotatef[i];
		}
	}

	void setScalef(float x, float y, float z) {
		objectScalef[0] = x;
		objectScalef[1] = y;
		objectScalef[2] = z;

		// Actualizar el array auxiliar
		for (int i = 0; i < 3; i++) {
			arrScalef[i] = objectScalef[i];
		}
	}

	float* getTranslatef() { return arrTranslatef; }
	float* getScalef() { return arrScalef; }
	float* getRotatef() { return arrRotatef; }

	vec3 getPosition() const {
		return vec3(objectTranslatef[0], objectTranslatef[1], objectTranslatef[2]);
	}

	void setPosition(const vec3& pos) {
		setTranslatef(pos.x, pos.y, pos.z);
	}


	void saveComponent(std::ofstream* fileNamePathContentObject) {
		serializeComponent(fileNamePathContentObject);
	}

	void loadComponent(std::ifstream* fileNamePathContentObject) {
		deserializeComponent(fileNamePathContentObject);
	}

	void position() {
		glPushMatrix();
		//Reposicionamiento del Objeto En Cuestion
		glTranslatef(getTranslatef()[0], getTranslatef()[1], getTranslatef()[2]);

		//Escalacion De Objeto En Cuestion
		glScalef(getScalef()[0], getScalef()[1], getScalef()[2]);

		//Rotacion de Objeto En Cuestion
		glRotatef(getRotatef()[0], getRotatef()[1], getRotatef()[2], getRotatef()[3]);

	}

};
#endif
