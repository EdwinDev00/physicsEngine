#pragma once

#include "core/math/vec3.h"
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
		vec3 ambient;
		vec3 diffuse;
		vec3 specular;
		float shininess;

	public:
		BlinnPhongMat(){}
		BlinnPhongMat(vec3 ambientV, vec3 diffuseV, vec3 specularV, float shininess = 32);
		void Apply(ShaderResource& program) override;
	};
}
