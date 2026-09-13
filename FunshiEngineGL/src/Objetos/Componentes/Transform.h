#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "../Componentes/Component.h"

// Las funciones libres de matrices se declaran aqui y se definen en
// Transform.cpp (evitan exponer glm en el header).
void buildMatrixFromTransform(class Transform* t, float outMatrix[16]);
void decomposeMatrixToTransform(float inMatrix[16], class Transform* t);

class Transform : public Component {
private:
	float objectTranslatef[3] = { 0.0f, 0.0f, 0.0f };
	float objectScalef[3] = { 1.0f, 1.0f, 1.0f };
	float objectRotatef[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	void serializeComponent(std::ofstream* fileNamePathContentObject) override;
	void deserializeComponent(std::ifstream* fileNamePathContentObject) override;

public:
	Transform();

	float arrTranslatef[3], arrScalef[3], arrRotatef[4];
	bool childsFreeze = false;

	void setTranslatef(float x, float y, float z);
	void setRotatef(float angle, float x, float y, float z);
	void setScalef(float x, float y, float z);

	float* getTranslatef() { return arrTranslatef; }
	float* getScalef() { return arrScalef; }
	float* getRotatef() { return arrRotatef; }

	void saveComponent(std::ofstream* fileNamePathContentObject) override;
	void loadComponent(std::ifstream* fileNamePathContentObject) override;

	void position();
};
#endif