#ifndef TRANSFORM_H
#define TRANSFORM_H


#include "../Componentes/Component.h"
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;



class Transform : public Component {
private:
	float objectTranslatef[3] = { 0.0f, 0.0f, 0.0f };
	float objectScalef[3] = { 1.0f, 1.0f, 1.0f };
	float objectRotatef[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	void serializeComponent(std::ofstream* fileNamePathContentObject) override {
		fileNamePathContentObject->write(reinterpret_cast<const char*>(&objectTranslatef), sizeof(float) * 3);
		fileNamePathContentObject->write(reinterpret_cast<const char*>(&objectScalef), sizeof(float) * 3);
		fileNamePathContentObject->write(reinterpret_cast<const char*>(&objectRotatef), sizeof(float) * 4);
	}

	void deserializeComponent(std::ifstream* fileNamePathContentObject) override {
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
	bool childsFreeze = false;

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

	void saveComponent(std::ofstream* fileNamePathContentObject) override {
		serializeComponent(fileNamePathContentObject);
	}

	void loadComponent(std::ifstream* fileNamePathContentObject) override {
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

inline void buildMatrixFromTransform(Transform* t, float outMatrix[16]) {
    glm::mat4 mat(1.0f);
    mat = glm::translate(mat, glm::vec3(t->getTranslatef()[0], t->getTranslatef()[1], t->getTranslatef()[2]));

    float angle = t->getRotatef()[0];
    glm::vec3 axis(t->getRotatef()[1], t->getRotatef()[2], t->getRotatef()[3]);
    if (glm::length(axis) > 0.0001f)
        mat = glm::rotate(mat, glm::radians(angle), glm::normalize(axis));

    mat = glm::scale(mat, glm::vec3(t->getScalef()[0], t->getScalef()[1], t->getScalef()[2]));

    const float* ptr = glm::value_ptr(mat); // ✅ value_ptr
    for (int i = 0; i < 16; i++) outMatrix[i] = ptr[i];
}

inline void decomposeMatrixToTransform(float inMatrix[16], Transform* t) {
    glm::mat4 mat = glm::make_mat4(inMatrix); // ✅ make_mat4
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(mat, scale, rotation, translation, skew, perspective);

    t->setTranslatef(translation.x, translation.y, translation.z);

    rotation = glm::normalize(rotation);
    float angle = glm::degrees(glm::angle(rotation));
    glm::vec3 axis = glm::axis(rotation);
    if (std::isnan(angle) || glm::length(axis) < 0.0001f) {
        angle = 0.0f;
        axis = glm::vec3(0, 1, 0);
    }
    t->setRotatef(angle, axis.x, axis.y, axis.z);

    t->setScalef(scale.x, scale.y, scale.z);
}
#endif
