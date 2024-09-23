#pragma once
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include "core/gltf.h"

#include <vector>
#include "core/math/mat4.h"
#include "texture.h"

using json = nlohmann::json;
class Model;

struct Vertex;

class GLTFLoader
{
private:
	static GLTFLoader* instance;

	 json JSON;
	 std::string file; //Store the file path

	//Container for loaded texture resources
	 std::unordered_map<std::string,Texture> loadedTexture; 

	void GetSourceData();
	
	std::vector<float> GetFloats(json accessor); //Get the values from the file 
	std::vector<unsigned int> GetIndices(json accessor);
	std::vector<Texture> GetTexture(json primitiveAccessor);
	
	//Group the data from the source based on type
	std::vector<vec2> groupFloatsVec2(std::vector<float> floatVec);
	std::vector<vec3> groupFloatsVec3(std::vector<float>& floatVec);
	std::vector<vec4> groupFloatsVec4(std::vector<float> floatVec);
	
	 void ParseData(const std::string& source);
	 std::shared_ptr<Model> LoadGLTFMeshDataPrimitive();

protected:
	std::vector<unsigned char> data;

public:
	static GLTFLoader* Get()
	{
		if (instance == nullptr)
			instance = new GLTFLoader;
		return instance;
	}

	static void Delete()
	{
		delete instance;
		instance = NULL;	
	}
 
	std::shared_ptr<Model> CreateGLTF(const std::string& source);
	std::vector<vec4> CalcTangents(const std::vector<vec3>& positions, const std::vector<unsigned int>& indices, const std::vector<vec2>& UVs);
};