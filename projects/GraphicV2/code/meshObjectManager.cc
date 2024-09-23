#include "meshObjectManager.h"
#include "dataCollection.h"
#include "gltfLoader.h"

#include "shaderResource.h"
#include "material.h"
#include "camera.h"
#include "texture.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <gl/glew.h>

MeshObject* MeshObject::instance = nullptr;

std::shared_ptr<Model> MeshObject::LoadObj(std::string modelPath, std::string texPath)
{
	return std::make_shared<Model>(Mesh(&ParseObj(modelPath, texPath)));
}

ObjData MeshObject::ParseObj(std::string source, std::string texSource)
{
	ObjData data;

	//Read the file
	std::ifstream file(source);
	std::string line;

	while (std::getline(file, line))
	{
		if (line[0] == '#') continue;

		if (line.find("v") != std::string::npos)
		{
			if (line.find("t") != std::string::npos) { ReadDataV2(line.c_str(), data.texUVs); }
			else if (line.find("n") != std::string::npos) { ReadDataV3(line.c_str(), data.normals); }
			else { ReadDataV3(line.c_str(), data.positions); }
		}
		else if (line[0] == 'f')
		{
			std::replace(line.begin(), line.end(), '/', ' ');

			unsigned int face[9];
			sscanf(line.c_str(), "%*s %i %i %i %i %i %i %i %i %i", &face[0], &face[1], &face[2]
				, &face[3], &face[4], &face[5], &face[6], &face[7], &face[8]);
			data.Vertexindices.insert(data.Vertexindices.end(), { face[0] - 1, face[3] - 1, face[6] - 1 });
			data.Textureindices.insert(data.Textureindices.end(), { face[1] - 1, face[4] - 1, face[7] - 1 });
			data.Normalindices.insert(data.Normalindices.end(), { face[2] - 1, face[5] - 1, face[8] - 1 });
		}
	}
	data.baseMat =
		new material::BlinnPhongMat(glm::vec3(1.0f, 0.5f, 0.31f),
									glm::vec3(1.0f, 0.5f, 0.31f),
									glm::vec3(0.5f, 0.5f, 0.5f));

	data.textures.push_back(Texture(texSource));
	Texture defaultNormal;
	Texture defaultMetallic;
	defaultNormal.SetType(Normal);
	defaultMetallic.SetType(MetallicRoughness);
	data.textures.push_back(defaultNormal);
	data.textures.push_back(defaultMetallic);

	return data;
}


std::shared_ptr<Model> MeshObject::LoadModel(std::string modelPath, std::string texPath)
{
	if (modelList.find(modelPath) != modelList.end())
		return modelList[modelPath];

	std::ifstream file(modelPath);

	if (!file.good())
	{
		std::cout << "ENGINE ERROR: BAD MODEL FILE PATH! Returned empty" << std::endl;
		return std::shared_ptr<Model>();
	}

	std::stringstream operation(modelPath);
	std::string out;
	while (std::getline(operation, out, '.'))
	{
		if (out == "obj") modelList[modelPath] = LoadObj(modelPath, texPath);

		else if (out == "gltf") modelList[modelPath] = GLTFLoader::Get()->CreateGLTF(modelPath);
	}

	return modelList[modelPath];
}

void MeshObject::ReadDataV3(const std::string& line, std::vector<glm::vec3>& out)
{
	float vertex[3];
	sscanf(line.c_str(), "%*s %f %f %f", &vertex[0], &vertex[1], &vertex[2]);
	out.push_back(glm::vec3(vertex[0], vertex[1], vertex[2]));
}

void MeshObject::ReadDataV2(const std::string& line, std::vector<glm::vec2>& out)
{
	float vertex[2];
	sscanf(line.c_str(), "%*s %f %f", &vertex[0], &vertex[1]);
	out.push_back(glm::vec2(vertex[0], vertex[1]));
}

GameObject::GameObject(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, std::string modelPath, std::string texPath)
	: position(position), rotation(rotation), scale(scale)
{
	name = modelPath;
	modelObject = MeshObject::Get()->LoadModel(modelPath, texPath);
}

void GameObject::Draw(ShaderResource& program, Object::Camera& cam) 
{
	/*modelRotation = glm::rotationx(rotation.x) *
		rotationy(rotation.y) *
		rotationz(rotation.z);*/
	modelRotation = SetRotation(rotation);

	modelScale = glm::mat4
	   (glm::vec4(scale.x, 0, 0, 0),
		glm::vec4(0, scale.y, 0, 0),
		glm::vec4(0, 0, scale.z, 0),
		glm::vec4(0, 0, 0, 1)
	   );

	modelTranslation = glm::mat4
	(glm::vec4(1, 0, 0, 0),
		glm::vec4(0, 1, 0, 0),
		glm::vec4(0, 0, 1, 0),
		glm::vec4(position.x, position.y, position.z, 1)
	);

	glm::mat4 modelMat = modelTranslation * modelRotation * modelScale;
	glm::mat4 MVP = cam.GetProjView() * modelMat;
	
	program.SetUniformMat4f("u_Model", modelMat);
	program.SetUniformMat4f("u_MVP", MVP);

	//Draw function
	for (auto& mesh : modelObject->meshes)
	{
		for (auto& prim : mesh.primitives)
		{
			prim.vao.Bind();
			prim.ibo.Bind();
			prim.baseMaterial->Apply(program);

			glDrawElements(GL_TRIANGLES, prim.ibo.GetCount(), GL_UNSIGNED_INT, nullptr);
			for (auto& texture : prim.baseMaterial->GetTextures())
				texture.UnBind();
		}
	}
}