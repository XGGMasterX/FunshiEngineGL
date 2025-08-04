#include "Material.h"

Material::Material(float ambi_r, float ambi_g, float ambi_b,
	float diff_r, float diff_g, float diff_b,
	float spec_r, float spec_g, float spec_b,
	float shin,
	float emiss_r, float emiss_g, float emiss_b)
{
	ambient[0] = ambi_r; ambient[1] = ambi_g; ambient[2] = ambi_b; ambient[3] = 1.f;
	diffuse[0] = diff_r; diffuse[1] = diff_g; diffuse[2] = diff_b; diffuse[3] = 1.f;
	specular[0] = spec_r; specular[1] = spec_g; specular[2] = spec_b; specular[3] = 1.f;
	shininess = shin;
	emission[0] = emiss_r; emission[1] = emiss_g; emission[2] = emiss_b;
}

Material::~Material()
{
}

void Material::ativar()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, emission);
}