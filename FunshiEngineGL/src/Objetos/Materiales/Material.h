#ifndef MATERIAL_H
#define MATERIAL_H

#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>

class Material
{
private:
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float emission[4];
	float shininess;
public:
	Material(
		float ambi_r, float ambi_g, float ambi_b,
		float diff_r, float diff_g, float diff_b,
		float spec_r, float spec_g, float spec_b,
		float shin,
		float emiss_r = 0.f, float emiss_g = 0.f, float emiss_b = 0.f
	);
	~Material();
	void ativar();
};
#endif