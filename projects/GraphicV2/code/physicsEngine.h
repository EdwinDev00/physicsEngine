#pragma once

#include "dataCollection.h"
#include <vector>
#include "meshObjectManager.h"
#include "inlineFunc.h"

#define EPA_TOLERANCE 0.001f

#define DEBUG 1

class PhysicsEngine
{
	public:
		std::vector<GameObject*> objects; // list of objects in the scene
		glm::vec3 gravity; // user defined gravity vector

		PhysicsEngine() : gravity(0.0f,-0.00f,0.0f){} //default gravity

		void DeleteObjectData()
		{
			if (objects.size() > 0)
			{
				for (auto& obj : objects) delete obj;
				objects.resize(0);
			}
		}

		void AddObject(GameObject* object) { objects.push_back(object); }

		void Update(float deltaTime)
		{
			std::vector<std::pair<GameObject*, GameObject*>> potentialCollisions = SweepAndPrune();
			std::vector<glm::vec3> simplexPs;

#if DEBUG
			for (GameObject* obj : objects) //only for debug remove this later
				obj->debugC = glm::vec4(0, 0, 1, 1); //reset color
#endif
			for(const auto& pair : potentialCollisions)
			{
				GameObject* objA = pair.first;
				GameObject* objB = pair.second;

				bool collision = GJK(objA,objB,simplexPs); //CHANGE THIS TO GJK COLLISION
				if(collision)
				{
					//IF WORKS EXCHANGE IT INTO EPA Collision resolution
					objA->debugC = glm::vec4(1, 0, 1, 1); 
					objB->debugC = glm::vec4(1, 0, 1, 1);

					//EPA 
					glm::vec3 collisionNormal = glm::vec3(0);
					float penetrationDepth = 0;
					EPA(objA, objB, simplexPs,collisionNormal, penetrationDepth);

					std::cout << "EPA PenetrationDepth: " << penetrationDepth << "\n";
					std::cout << "EPA Collision Normal x: " << collisionNormal.x << "\n";
					std::cout << "EPA Collision Normal y: " << collisionNormal.y << "\n";
					std::cout << "EPA Collision Normal z: " << collisionNormal.z << "\n";

					//Apply collision response using the collision normal and penetrationDepth
					//Collision response logic

				}
				simplexPs.clear();
			}

			for(GameObject* obj : objects)
			{
				ApplyGravity(obj);
				obj->OnUpdate(deltaTime);
			}

		}

		//Apply gravity upon all the object and update object position and velocity
		void ApplyGravity(GameObject* obj)
		{
			if (!obj->mass > 0) //FOr NOW LETS LIMIT (SO NOT OCCURINGG ANY ZERO DIVISION) (SOLUTION IN OBJ CONSTRUCT (0 MASS Manually set it to 1))
				return;
				
			glm::vec3 gravityForce = obj->mass * gravity;
			obj->acceleration += gravityForce / obj->mass;
		}

		void ApplyForce( GameObject* obj, const glm::vec3& forceDirection, float forceMagnitude, const glm::vec3& hitP)
		{
			glm::vec3 force = forceDirection * forceMagnitude;
			obj->velocity += force / obj->mass;

			//Calculate the vector from the center of mass to the hit point (r)
			glm::vec3 r = hitP - obj->boundingbox.GetPosition() ;

			// Calculate the torque: τ = r × F
			glm::vec3 torque = glm::cross(r, force);
			obj->angularVelocity += obj->inertiaTensorInWorld * torque;
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

		//Detect overlapping AABBS (Sweep algorithm)
		std::vector<std::pair<GameObject* , GameObject*>> SweepAndPrune()
		{
			struct AABBEntry
			{
				float minX, maxX;
				GameObject* obj;
			};

			std::vector<AABBEntry> entries;
			for(GameObject* obj : objects)
			{
				//Get the latest update on the bounding box before checking
				obj->boundingbox.UpdateBounds(obj->transform);
				AABB& aabb = obj->boundingbox;
				entries.push_back({ aabb.min.x,aabb.max.x,obj });
			}

			// Sort the entries based on the minimum x value
			std::sort(entries.begin(), entries.end(), [](const AABBEntry& a, const AABBEntry& b) { return a.minX < b.minX; });

			//Sweep and find overlapping pairs
			std::vector<std::pair<GameObject*, GameObject*>> overlappingPairs;
			for(int i = 0; i < entries.size(); i++)
			{
				for(int j = i + 1; j < entries.size() && entries[j].minX <= entries[i].maxX; j++ )
				{
					//Check for overlap in the (Y and Z axis)
					if (entries[i].obj->boundingbox.Intersects(entries[j].obj->boundingbox))
						overlappingPairs.push_back({ entries[i].obj, entries[j].obj });
				}
			}

			return overlappingPairs;
		}


		bool GJK(const GameObject* objA, const GameObject* objB, std::vector<glm::vec3>& simplex)
		{
			glm::vec3 direction(1, 0, 0); // arbitrary
			simplex.push_back(Support(objA, objB, direction));

			//if the first simplex is already towards the origin, return no collision
			if (glm::dot(simplex.back(), direction) <= 0) return false;

			//Main loop: simplex construction
			while(true)
			{
				direction = -simplex.back(); //new direction towards origin
				glm::vec3 newPoint = Support(objA, objB, direction); // get another point in current direction
				//Check if the new point past the origin
				if (glm::dot(newPoint, direction) <= 0)
					return false; //no collision

				simplex.push_back(newPoint);
				if (HandleSimplex(simplex, direction))
				{
					return true; // Collision Detect
				}
			}
		}

		void EPA(const GameObject* objA, const GameObject* objB, std::vector<glm::vec3>& simplex, glm::vec3& collisionNormal, float& penetrationDepth)
		{
			// Build the polytope from final simplex
			std::vector<Face> polytope = InitalizePolytope(simplex); 
			int maxIteration = 50;
			while(maxIteration--)
			{
				Face closestFace = findClosestFace(polytope);
				// Get a new point in the direction of the closest face normal
				glm::vec3 newPoint = Support(objA, objB, closestFace.normal); //Potential error where the support function getting a point to far away
				// Check if the new point is far enough from the origin
				float distanceToOrigin = glm::dot(closestFace.normal, newPoint);

				if (distanceToOrigin - closestFace.distance < EPA_TOLERANCE || glm::length(newPoint) < 1e-6f) 
				{
					collisionNormal = closestFace.normal;
					penetrationDepth = closestFace.distance;
					break;
				}
				
				ExpandPolytope(polytope, simplex, newPoint);
			}

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