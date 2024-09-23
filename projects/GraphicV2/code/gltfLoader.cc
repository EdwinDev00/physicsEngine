#include "gltfLoader.h"

#include <iostream>
#include <sstream>

#include "dataCollection.h"
#include "material.h"
#include "texture.h"

GLTFLoader* GLTFLoader::instance = nullptr;

std::string get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	
	throw(errno);
}

bool IsEmbedded(std::string uri)
{
	return uri.find("data:application/octet-stream;base64") == 0 || uri.find("data:image/jpeg;base64") == 0 || uri.find("data:image/png;base64") == 0;
}
 
void GLTFLoader::ParseData(const std::string& source)
{
	file = source;
	std::string content = get_file_contents(source.c_str());
	JSON = json::parse(content);
	GetSourceData();
}

std::shared_ptr<Model> GLTFLoader::LoadGLTFMeshDataPrimitive()
{
	std::vector<Mesh> meshes;
	
	unsigned int meshesCount = JSON["meshes"].size();

	for (int i = 0; i <= meshesCount - 1; i++)
	{
		GLTFData data;
		Mesh mesh;
		for(int j = 0; j < JSON["meshes"][i]["primitives"].size(); j++)
		{
			json primAccIndex =JSON["meshes"][i]["primitives"][j];
			unsigned int posAccIndex = primAccIndex["attributes"]["POSITION"];
			unsigned int texAccIndex = primAccIndex["attributes"]["TEXCOORD_0"];
			unsigned int norAccIndex = primAccIndex["attributes"]["NORMAL"];
			unsigned int indAccIndex = primAccIndex["indices"];
			unsigned int tanAccIndex = 0;
			if (primAccIndex["attributes"].find("TANGENT") != primAccIndex["attributes"].end())
				tanAccIndex = primAccIndex["attributes"]["TANGENT"];

			std::vector<float> posVec = GetFloats(JSON["accessors"][posAccIndex]);
			data.positions = groupFloatsVec3(posVec);
			std::vector<float> texUV = GetFloats(JSON["accessors"][texAccIndex]);
			data.texUVs = groupFloatsVec2(texUV);
			std::vector<float> norVec = GetFloats(JSON["accessors"][norAccIndex]);
			data.normals = groupFloatsVec3(norVec);

			data.indices = GetIndices(JSON["accessors"][indAccIndex]);

			if (tanAccIndex != 0)
			{
				std::vector<float> tanVec = GetFloats(JSON["accessors"][tanAccIndex]);
				data.tangents = groupFloatsVec4(tanVec);
			}
			else data.tangents = CalcTangents(data.positions, data.indices, data.texUVs);

			data.textures = GetTexture(primAccIndex);
			data.baseMat = new material::BlinnPhongMat(
				glm::vec3(1.0f, 1.0f, 1.0f),
				glm::vec3(1.0f, 1.0f, 1.0f),
				glm::vec3(1.0f, 1.0f, 1.0f));

			mesh.primitives.push_back(Primitive(&data));
		}
		meshes.push_back(mesh);
	}
	data.clear();
	return std::make_shared<Model>(meshes);
}

std::shared_ptr<Model> GLTFLoader::CreateGLTF(const std::string& source)
{
	ParseData(source);	
	return LoadGLTFMeshDataPrimitive();
}


void GLTFLoader::GetSourceData()
{
	//search through the dictionary key
	std::string uri = JSON["buffers"][0]["uri"]; //Uniform Resource Identifier

	std::string fileStr = std::string(file);
	std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1); //Remove the /n
	
	if(IsEmbedded(uri))
	{
		fx::gltf::Document embeddedDataContent = fx::gltf::LoadFromText(fileStr);
		for (int i = 0; i < embeddedDataContent.bufferViews.size(); i++)
		{
			auto& dataContent = embeddedDataContent.buffers[embeddedDataContent.bufferViews[i].buffer];
			data.insert(data.end(), dataContent.data.begin(),dataContent.data.end());
		}
	}
	else
	{
		std::string bytesText; //Holds the raw text
		bytesText = get_file_contents((fileDirectory + uri).c_str());
		data = std::vector<unsigned char>(bytesText.begin(), bytesText.end());
	}
	
//	return data;
}

std::vector<float> GLTFLoader::GetFloats(json accessor)
{
	//First get the bufferview (we want bufferview, count, byteoffset and type )
	std::vector<float> floatVec;

	unsigned int bufferviewIndex = accessor.value("bufferView", 0); //points to the correct bufferview
	unsigned int count = accessor["count"]; //how many floats we have to get
	unsigned int accByteOffset = accessor.value("byteOffset", 0); //tells what index to look at the data with respect to the beginning if the bufferview
	std::string type = accessor["type"]; // how we should group the floats (it can either be 'SCALAR','VEC2','VEC3' or 'VEC4')

	json bufferView = JSON["bufferViews"][bufferviewIndex];
	unsigned int byteOffset = 0;
	if(bufferView.find("byteOffset") != bufferView.end()) 
		byteOffset = bufferView["byteOffset"]; //tells us where that bufferview starts


	unsigned int numPerVert;
	if (type == "SCALAR") numPerVert = 1;
	else if (type == "VEC2") numPerVert = 2;
	else if (type == "VEC3") numPerVert = 3;
	else if (type == "VEC4") numPerVert = 4;
	else throw std::invalid_argument("Type is invalid (NOT SCALAR, VEC2, VEC3, or VEC4)");

	unsigned int beginningOfData = byteOffset + accByteOffset;
	unsigned int lengthOfData = count * 4 * numPerVert;
	for(unsigned int i = beginningOfData; i < beginningOfData + lengthOfData; i += 4)
	{
		unsigned char bytes[] = { data[i],data[i+1],data[i+2] ,data[i+3] };
		float value;
		std::memcpy(&value, bytes, sizeof(float)); //transform bytes into a float
		floatVec.push_back(value);
	}

	return floatVec;
}

std::vector<unsigned int> GLTFLoader::GetIndices(json accessor)
{
	std::vector<unsigned int> indices;

	//Get properties from the accessor
	unsigned int bufferviewIndex = accessor.value("bufferView", 0);
	unsigned int count = accessor["count"];
	unsigned int accByteOffset = accessor.value("byteOffset", 0);
	unsigned int componentType = accessor["componentType"];

	//Get properties form the bufferview
	json bufferView = JSON["bufferViews"][bufferviewIndex];
	unsigned int byteOffset = 0;
	if (bufferView.find("byteOffset") != bufferView.end())
		byteOffset = bufferView["byteOffset"];

	unsigned int beginningOfData = byteOffset + accByteOffset;
	
	if(componentType == 5126)
	{
		for (unsigned int i = beginningOfData; i < beginningOfData + count * 4; i +=4)
		{
			unsigned char bytes[] = { data[i],data[i+1] ,data[i+2] ,data[i+3] };
			float value;
			std::memcpy(&value, bytes, sizeof(float));
			indices.push_back((unsigned int) value);
		}
	}
	else if(componentType == 5125)
	{
		for (unsigned int i = beginningOfData; i < beginningOfData + count * 4; i += 4)
		{
			unsigned char bytes[] = { data[i],data[i+1] ,data[i+2] ,data[i+3] };
			unsigned int value;
			std::memcpy(&value, bytes, sizeof(unsigned int));
			indices.push_back(value);
		}
	}
	else if (componentType == 5123)
	{
		for (unsigned int i = beginningOfData; i < beginningOfData + count * 2; i += 2)
		{
			unsigned char bytes[] = { data[i], data[i+1] };
			unsigned short value;
			std::memcpy(&value, bytes, sizeof(unsigned short));
			indices.push_back((unsigned int) value);
		}
	}
	else if (componentType == 5122)
	{
		for (unsigned int i = beginningOfData; i < beginningOfData +  count * 2; i += 2)
		{
			unsigned char bytes[] = { data[i],data[i+1] };
			short value;
			std::memcpy(&value, bytes, sizeof(short));
			indices.push_back((unsigned int) value);
		}
	}

	return indices;
}

std::vector<glm::vec2> GLTFLoader::groupFloatsVec2(std::vector<float> floatVec)
{
	std::vector<glm::vec2> vectors;
	for (int i = 0; i < floatVec.size(); i += 2)
	{
		vectors.push_back(glm::vec2(floatVec[i], floatVec[i + 1]));
	}

	return vectors;
}

std::vector<glm::vec3> GLTFLoader::groupFloatsVec3(std::vector<float>& floatVec)
{
	std::vector<glm::vec3> vectors;
	for (int i = 0; i < floatVec.size(); i += 3)
		vectors.push_back(glm::vec3(floatVec[i], floatVec[i+1], floatVec[i+2]));
	return vectors;
}

std::vector<glm::vec4> GLTFLoader::groupFloatsVec4(std::vector<float> floatVec)
{
	std::vector<glm::vec4> vectors;
	for (int i = 0; i < floatVec.size(); i += 4)
		vectors.push_back(glm::vec4(floatVec[i], floatVec[i+1], floatVec[i+2], floatVec[i+3]));
	return vectors;
}

std::vector<glm::vec4> GLTFLoader::CalcTangents(const std::vector<glm::vec3>& positions, const std::vector<unsigned int>& indices, const std::vector<glm::vec2>& UVs)
{
	std::vector<glm::vec4> tangents(positions.size());

	for(int i = 0; i < indices.size(); i+=3)
	{
		glm::vec3 pos0 = positions[indices[i]];
		glm::vec3 pos1 = positions[indices[i + 1]];
		glm::vec3 pos2 = positions[indices[i + 2]];

		glm::vec2 deltaUV1 = UVs[indices[i + 1]] - UVs[indices[i]];
		glm::vec2 deltaUV2 = UVs[indices[i + 2]] - UVs[indices[i]];
	
		glm::vec3 edge1 = pos1 - pos0;
		glm::vec3 edge2 = pos2 - pos0;
		
		
		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		glm::vec4 tangent =
		glm::vec4(f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
				 f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
				 f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z),1);
	
		tangents[indices[i]] += tangent;
		tangents[indices[i+1]] += tangent;
		tangents[indices[i+2]] += tangent;
	}

	return tangents;
}

std::vector<Texture> GLTFLoader::GetTexture(json primitiveAccessor)
{
	std::vector<Texture> textures;

	std::string fileStr = std::string(file);
	std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);

	fx::gltf::Document document = fx::gltf::LoadFromText(fileStr);

	for(auto& image : document.images)
	{
		if(loadedTexture.find(image.uri) == loadedTexture.end())
		{
			if(image.IsEmbeddedResource()) //is embedded
			{
				std::vector<unsigned char> imageData;
				image.MaterializeData(imageData);
				textures.push_back(Texture(imageData));
			}
			else
			{
				Texture tex(fileDirectory + image.uri, false);
				textures.push_back(tex);
				loadedTexture[image.uri] = tex;
			}
		}
		else
			textures.push_back(loadedTexture[image.uri]);
	}

	//TEXTURE

	//Fetch and apply to the right texture slot 
	std::vector<Texture> primTexs;
	auto GLTFmaterial = JSON["materials"][primitiveAccessor.value("material",0)];
	if (GLTFmaterial["pbrMetallicRoughness"].contains("baseColorTexture"))
	{
		int textureIndex = GLTFmaterial["pbrMetallicRoughness"]["baseColorTexture"]["index"];
		int TextureSource = JSON["textures"][textureIndex].value("source",0);
		textures[TextureSource].SetType(BaseColor);
		primTexs.push_back(textures[TextureSource]);
	}
	else
	{
		Texture defaultColor = Texture::GetDefualtWhite();
		defaultColor.SetType(BaseColor);
		primTexs.push_back(defaultColor);
	}

	if(GLTFmaterial["pbrMetallicRoughness"].contains("metallicRoughnessTexture"))
	{
		int textureIndex = GLTFmaterial["pbrMetallicRoughness"]["metallicRoughnessTexture"]["index"];
		int TextureSource = JSON["textures"][textureIndex].value("source", 0);
		textures[TextureSource].SetType(MetallicRoughness);
		primTexs.push_back(textures[TextureSource]);
	}
	else
	{
		Texture defaultColor = Texture::GetDefualtBlack();
		defaultColor.SetType(MetallicRoughness);
		primTexs.push_back(defaultColor);
	}

	if (GLTFmaterial.contains("normalTexture")) {

		int textureIndex = GLTFmaterial["normalTexture"]["index"];
		int TextureSource = JSON["textures"][textureIndex].value("source", 0);
		textures[TextureSource].SetType(Normal);
		primTexs.push_back(textures[TextureSource]);
	}
	else
	{
		Texture defaultColor = Texture::GetDefualtNormal();
		defaultColor.SetType(Normal);
		primTexs.push_back(defaultColor);
	}

	//if (GLTFmaterial.contains("occlusionTexture")){
	//	textures[GLTFmaterial["occlusionTexture"]["index"]].SetType(Occlusion);
	//	primTexs.push_back(textures[GLTFmaterial["occlusionTexture"]["index"]]);
	//}
	//if (GLTFmaterial.contains("emissiveTexture")){
	//	textures[GLTFmaterial["emissiveTexture"]["index"]].SetType(Emissive);
	//	primTexs.push_back(textures[GLTFmaterial["emissiveTexture"]["index"]]);
	//}
	
	return primTexs;
}