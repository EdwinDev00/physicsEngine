#include "dataCollection.h"
#include "meshObjectManager.h"
#include "vertexBufferLayout.h"

#include "material.h"
#include "gl/glew.h"

std::vector<Vertex> GLTFData::ConstructMeshData()
{
	std::vector<Vertex> vertices;

	for (int i = 0; i < positions.size(); i++)
	{
		vertices.push_back(
			Vertex{ positions[i],texUVs[i],
			normals[i],tangents[i] }
		);
	}
	return vertices;
}

std::vector<Vertex> ObjData::ConstructMeshData()
{
	std::vector<Vertex> temp;

	for (int i = 0; i < Vertexindices.size(); i++)
	{
		Vertex vertex;
		vertex.position = positions[Vertexindices[i]];
		vertex.UV = texUVs[Textureindices[i]];
		vertex.normal = normals[Normalindices[i]];
		temp.push_back(vertex);
	}

	for (int i = 0; i < Vertexindices.size(); i++)
	{
		bool found = false;
		for (int j = 0; j < vertices.size(); j++)
			if (temp[i] == vertices[j])
			{
				indices.push_back(j);
				found = true;
			}
		if (!found)
		{
			vertices.push_back(temp[i]);
			indices.push_back(vertices.size() - 1);
		}
	}
	return vertices;
}

Primitive::Primitive( PrimitiveData* data)
{
	vertices = data->ConstructMeshData();
	indices = data->indices;

	baseMaterial = data->baseMat;
	for (auto& texture : data->textures)
		baseMaterial->AddTexture(texture);

	vao.Bind();
	vbo.SetData(vertices);
	ibo.SetData(indices);
}

void Primitive::Upload()
{
	VertexBufferLayout layout;
	layout.Push<float>(3); //pos
	layout.Push<float>(2); //Uv
	layout.Push<float>(3); //normal
	layout.Push<float>(4); //tangent
	vao.AddBuffer(vbo, layout);
}

void Primitive::Clear()
{
	if(baseMaterial != nullptr)
		for (auto& tex : baseMaterial->GetTextures())
			tex.Delete();
}


void Model::UploadToGPU() {
	for (auto& mesh : this->meshes)
		for (auto& prim : mesh.primitives)
			prim.Upload();
}

void Model::SimpleDraw()
{
	for (auto& mesh : meshes)
		for (auto& prim : mesh.primitives)
		{
			prim.vao.Bind();
			prim.ibo.Bind();

			glDrawElements(GL_TRIANGLES, prim.ibo.GetCount(), GL_UNSIGNED_INT, nullptr);
		}
}
