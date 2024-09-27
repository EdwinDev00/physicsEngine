#pragma once

#include "dataCollection.h"
#include <vector>
#include "meshObjectManager.h"

#include "inlineFunc.h" //testing for all the raycast inline func

class PhysicsEngine
{
	public:
		std::vector<GameObject*> objects; // list of objects in the scene
		glm::vec3 gravity; // user defined gravity vector


		PhysicsEngine() : gravity(0.0f,9.81f,0.0f){} //default gravity

		void DeleteObjectData()
		{
			if (objects.size() > 0)
			{
				for (auto& obj : objects) delete obj;
				objects.resize(0);
			}
		}

		void AddObject(GameObject* object) { objects.push_back(object); }

		void Update()
		{
			for(GameObject* obj : objects)
			{
				obj->OnUpdate();
			}

		}

		//Apply gravity upon all the object and update object position and velocity
		void ApplyGravity(float deltaTime)
		{
			/*
			* loop through all the object and apply gravity
			* and update their position and velocity
			*/
		}

		GameObject* Raycast(const Ray& ray, glm::vec3& hitPoint)
		{
			GameObject* closestObject = nullptr;
			glm::vec3 tempHitPoint; 

			for(GameObject* obj : objects)
			{
				if(RayIntersectAABB(ray,obj->boundingbox))
				{		
					if(obj->RayMeshIntersection(ray,tempHitPoint))
					{
						closestObject = obj;
						hitPoint = tempHitPoint;
					}
				}
			}

			return closestObject;
		}

		//Detect and resolve AABB-AABB collisions
		void DetectAndResolveCol()
		{
			for(int i = 0; i < objects.size(); i++)
			{
				for(int j = i + 1; j < objects.size(); j++)
				{
					if (objects[i]->boundingbox.Intersects(objects[j]->boundingbox))
						ResolveCollision(objects[i], objects[j]);
				}
			}
		}

		void ResolveCollision(GameObject* a, GameObject* b)
		{
			//Simple collision resolve: reverse velocities
			glm::vec3 normal = glm::normalize(b->GetPosition()- a->GetPosition());
			a->velocity = glm::reflect(a->velocity, normal);
			b->velocity = glm::reflect(b->velocity, normal);
		}
};