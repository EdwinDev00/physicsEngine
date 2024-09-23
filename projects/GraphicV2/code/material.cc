#include "material.h"
#include "shaderResource.h"
#include "texture.h"


material::BlinnPhongMat::BlinnPhongMat(glm::vec3 ambientV, glm::vec3 diffuseV, glm::vec3 specularV, float shininess)
	: ambient(ambientV), diffuse(diffuseV), specular(specularV), shininess(shininess)
{
}

void material::BlinnPhongMat::Apply(ShaderResource& program)
{
	for (auto& tex : textures)
		tex.Bind();
}
