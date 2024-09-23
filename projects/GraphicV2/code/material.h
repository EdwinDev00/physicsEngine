#pragma once

#include "config.h"
#include <memory>
#include <vector>

class ShaderResource;
class Texture;

namespace material
{
	class BaseMaterial 
	{
	public:
		BaseMaterial(){}
		~BaseMaterial(){}
		virtual void Apply(ShaderResource& program){}
		inline void AddTexture(const Texture& texture) { textures.push_back(texture); }
		inline std::vector<Texture>& GetTextures() { return textures; }
	
	protected:
		std::vector<Texture> textures;
	};

	class BlinnPhongMat : public BaseMaterial
	{
	protected:
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float shininess;

	public:
		BlinnPhongMat(){}
		BlinnPhongMat(glm::vec3 ambientV, glm::vec3 diffuseV, glm::vec3 specularV, float shininess = 32);
		void Apply(ShaderResource& program) override;
	};
}
