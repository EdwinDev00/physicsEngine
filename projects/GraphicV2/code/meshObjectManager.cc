#include "meshObjectManager.h"
#include "dataCollection.h"
#include "gltfLoader.h"

#include "shaderResource.h"
#include "material.h"
#include "camera.h"
#include "texture.h"
#include "inlineFunc.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <gl/glew.h>

MeshObject* MeshObject::instance = nullptr;

#pragma region LOAD MODEL DATA 


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
#pragma endregion

#pragma region GAMEOBJECT

GameObject::GameObject(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale,std::string modelPath, bool isStatic, std::string texPath)
	: position(position), rotation(rotation), scale(scale), velocity(glm::vec3(0.0f)), acceleration(glm::vec3(0.0f)), mass(0), angularVelocity(glm::vec3(0.0f)),
	  modelRotation(glm::identity<glm::mat4>())
{
	static int count;
	name = modelPath + std::to_string(count++);
	modelObject = MeshObject::Get()->LoadModel(modelPath, texPath);

	//Store each mesh triangle group
	std::vector<glm::vec3> triangleGroup(3);
	glm::vec3 triangleNormal; 
	for(auto& mesh : modelObject->meshes)
	{
		for(auto& data : mesh.primitives)
		{
			for(int i = 0; i < data.vertices.size(); i+=3)
			{
				//Loop through each 3 vertices to create a group of triangle
				triangleGroup[0] = data.vertices[data.indices[i]].position;
				triangleGroup[1] = data.vertices[data.indices[i + 1]].position;
				triangleGroup[2] = data.vertices[data.indices[i + 2]].position;

				//has precomputed normal (smooth shading) pick thhe first vertex normal
				if (data.vertices[data.indices[i]].normal != glm::vec3(0)) { triangleNormal = data.vertices[data.indices[i]].normal; }
				else
				{
					//Compute the normal manually (flat shading)
					glm::vec3 edge1 = triangleGroup[1] - triangleGroup[0];
					glm::vec3 edge2 = triangleGroup[2] - triangleGroup[0];
					triangleNormal = glm::normalize(glm::cross(edge1, edge2));
				}

				triangles.push_back(Triangles(triangleGroup, triangleNormal));
			}
		}
	}

	//TESTING NEW FETCH 
	for (auto& mesh : modelObject->meshes)
		for (const auto& data : mesh.primitives)
		{
			for (int i = 0; i < data.vertices.size(); i ++)
			{
				if (colliderVertices.end() == std::find(colliderVertices.begin(), colliderVertices.end(), data.vertices[i].position))
				{
					colliderVertices.push_back(data.vertices[i].position);
				}
			}
		}

	//for(const auto& triangle : triangles)
	//{
	//	for(const auto& vertex : triangle.vertices)
	//	{
	//		if(  colliderVertices.end() == std::find(colliderVertices.begin(), colliderVertices.end(), vertex))
	//		{
	//			colliderVertices.push_back(vertex);
	//		}
	//	}
	//}


	//set the aabb bounds exactly like the modeled size (include matching in scale)
	boundingbox = AABB(modelObject);

	if (!isStatic)
	{
		//Calculate the object's volume from the obj AABB
		glm::vec3 extent = boundingbox.GetExtents();
		float volume = extent.x * extent.y * extent.z; //AABB Volume

		// Estimate the mass based on the volume
		mass = volume * 5.0f; //Assuming constant density for simplicity (density set to 5); 
	}
	else mass = 0;
}

void GameObject::ApplyRotation(const glm::vec3& angularDisplacement)
{
	inertiaTensorInWorld = modelRotation * CalculateInertiaTensorMat() * glm::transpose(modelRotation);
	glm::quat orientation(modelRotation);
	glm::quat spin(0, angularDisplacement);
	orientation += spin * orientation * 0.5f;
	orientation = glm::normalize(orientation);
	modelRotation = glm::mat4(orientation);
	transform = modelTranslation * modelRotation * modelScale;
}

glm::mat4 GameObject::CalculateInertiaTensorMat() const 
{
	glm::vec3 extends = boundingbox.getOriginalExtend();
	float w = extends.x; float h = extends.y; float d = extends.z;

	//Inertia tensor (moment of inertia for cuboid)
	//OPTIMIZE: COMPUTE IN CONSTRUCTOR (PROBLEM: IF RUNTIME SCALE WRONG NEED UPDATE + MASS UPDATE (IMPORTANT!!))
	return
		glm::inverse(glm::mat4
		(
			(1.0f / 12.0f) * mass * (h * h + d * d), 0, 0, 0, // Ix
			0, (1.0f / 12.0f) * mass * (w * w + d * d), 0, 0, // Iy
			0, 0, (1.0f / 12.0f) * mass * (w * w + h * h), 0, // Iz
			0, 0, 0, 1
		));
}

glm::vec3 GameObject::CalculateInertiaTensor() const
{
	glm::vec3 extends = boundingbox.GetExtents();
	float w = extends.x; float h = extends.y; float d = extends.z;

	//Inertia tensor (moment of inertia for cuboid)
	return glm::vec3
	(
		(1.0f / 12.0f) * mass * (h * h + d * d), // Ix
		(1.0f / 12.0f) * mass * (w * w + d * d), // Iy
		(1.0f / 12.0f) * mass * (w * w + h * h)	 // Iz
	);
}

void GameObject::Draw(ShaderResource& program, Object::Camera& cam) 
{
	/*modelRotation = glm::rotationx(rotation.x) *
		rotationy(rotation.y) *
		rotationz(rotation.z);*/
	//modelRotation = SetRotation(rotation);

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

	transform = modelTranslation * modelRotation * modelScale;
	glm::mat4 MVP = cam.GetProjView() * transform;
	
	program.SetUniformMat4f("u_Model", transform);
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

bool GameObject::RayMeshIntersection(const Ray& ray, glm::vec3& hitpoint)
{
	float closestHit = FLT_MAX; // closest position of the ray hitting the mesh
	bool hit = false;

	//Invert the ray using the mesh transform matrix
	glm::mat4 inverseMeshTransform = glm::inverse(this->transform);
	glm::vec3 inverseOrigin = inverseMeshTransform * glm::vec4(ray.origin, 1);
	glm::vec3 inverseDirection = inverseMeshTransform * glm::vec4(ray.direction,0);

	// Iterate through all the triangle in the mesh
	for(const Triangles& tri : this->triangles)
	{
		// Access the trianlge vertices of the triangle
		glm::vec3 const& p1 = tri.vertices[0]; // Vertex A
		glm::vec3 const& p2 = tri.vertices[1]; // Vertex B
		glm::vec3 const& p3 = tri.vertices[2]; // Vertex C

		//Calculate intersection with the triangle's plane
		float denom = glm::dot(tri.normal, inverseDirection);
		float dividend = -(glm::dot(tri.normal, inverseOrigin) - glm::dot(p1, tri.normal));
		float t = dividend / denom;

		// If t < 0, the intersection is behind te ray, continue with another triangle
		if (t < 0) continue;

		// Calculate the exact intersetion point on the triangle's plane
		glm::vec3 intersectionP = inverseOrigin + inverseDirection * t;

		// Check if the intersectionP is inside the triangle using cross product 
		glm::vec3 A = p1 - intersectionP;
		glm::vec3 B = p2 - intersectionP;
		glm::vec3 C = p3 - intersectionP;

		// Boundary checks (inside the triangle)
		glm::vec3 u = glm::cross(B,C);
		glm::vec3 v = glm::cross(C,A);
		glm::vec3 w = glm::cross(A,B);

		// If any dot product result in negative value, the intersectionP is outside the triangle
		if (glm::dot(u, v) < 0.0f || glm::dot(u, w) < 0.0f)
			continue;

		// IntersectionP is inside the triangle
		if(t < closestHit)
		{
			closestHit = t;
			hit = true;
		}
	}
	
	hitpoint = ray.origin + ray.direction * closestHit; // store the closest hit point
	return hit;
}
#pragma endregion