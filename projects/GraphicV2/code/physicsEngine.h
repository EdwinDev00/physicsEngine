#pragma once

#include "dataCollection.h"
#include <vector>
#include "meshObjectManager.h"
#include "inlineFunc.h"

#include <map>

#define EPA_TOLERANCE 0.00001f

class PhysicsEngine
{
	public:
		std::vector<GameObject*> objects;
		bool enableGravity = false;
		glm::vec3 gravity;

		PhysicsEngine() : gravity(0.0f,-0.98f,0.0f){} //default gravity

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

			for (GameObject* obj : objects)
			{
				if(enableGravity)
					ApplyGravity(obj);
				obj->OnUpdate(deltaTime);
				obj->debugC = glm::vec4(0, 0, 1, 0);
			}

			std::vector<std::pair<GameObject*, GameObject*>> potentialCollisions = SweepAndPrune();

			for(const auto& pair : potentialCollisions)
			{
				GameObject& objA = *pair.first;
				GameObject& objB = *pair.second;
				Simplex simplexPs;	


				bool collision = GJK(objA,objB,simplexPs);
				if(collision)
				{
					objA.debugC = glm::vec4(1, 0, 1, 1);
					objB.debugC = glm::vec4(1, 0, 1, 1);

					//EPA 
					glm::vec3 collisionNormal = glm::vec3(0);
					float penetrationDepth = 0;
					std::pair<glm::vec3, glm::vec3> collisionPoints;

					if (EPA(objA, objB, simplexPs, collisionNormal, penetrationDepth, collisionPoints))
						SolveCollisionV2(objA, objB, collisionNormal, penetrationDepth,collisionPoints);
				}
			}
		}

		void ApplyGravity(GameObject* obj)
		{
			if (!obj->mass > 0)
				return;
			obj->acceleration = gravity;
		}

		void ApplyForce( GameObject* obj, const glm::vec3& forceDirection, float forceMagnitude, const glm::vec3& hitP)
		{
			if (!obj->mass > 0) return;

			glm::vec3 force = forceDirection * forceMagnitude;
			obj->velocity += force / obj->mass;

			//Calculate the vector from the center of mass to the hit point (r)
			glm::vec3 r = hitP - obj->GetPosition() ;

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

		std::vector<std::pair<GameObject* , GameObject*>> SweepAndPrune()
		{
			//Detect overlapping AABBS (Sweep algorithm)
			struct AABBEntry
			{
				float minX, maxX;
				GameObject* obj;
			};

			std::vector<AABBEntry> entries;
			for(GameObject* obj : objects)
			{
				//Get the latest update on the bounding box before checking
				obj->boundingbox.UpdateBounds(obj->GetTransform());
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

		bool GJK( const GameObject& objA,  const GameObject& objB, Simplex& simplex)
		{
			glm::vec3 direction(1, 0, 0); // arbitrary
			SupportPoint support = Support(objA, objB, direction);
			simplex.push_front(support);
			direction = -support.minkowDiff;
			while(true)
			{
				// Get the new support point in the current direction
				support = Support(objA, objB, direction);
				
				//Check if the new point past the origin
				if (glm::dot(support.minkowDiff, direction) < 0)
					return false; //no collision
	
				simplex.push_front(support);

				if (HandleSimplex(simplex, direction))
				{
					return true; // Collision Detect
				}
			}
		}

		bool EPA(const GameObject& objA, const GameObject& objB, const Simplex& simplex, glm::vec3& collisionNormal, float& penetrationDepth, std::pair<glm::vec3, glm::vec3>& collisionPoints)
		{
			// Initialize the polytope and faces from the simplex
			std::vector<SupportPoint> polytope(simplex.begin(), simplex.end());
			std::vector<int> faces = {	0, 1, 2,
										0, 3, 1,
										0, 2, 3,
										1, 3, 2	 };

			// Get initial face normals and find the minimum face
			auto faceNormals = GetFaceNormals(polytope, faces);
			std::vector<glm::vec4> normals = faceNormals.first;
			int minFaceIndex = faceNormals.second;

			glm::vec3 minNormal;
			float minDistance = FLT_MAX;
			std::vector<std::pair<int, int> > uniqueEdges;

			int interationCount = 65;
			while (interationCount--) {
				minNormal = glm::vec3(normals[minFaceIndex]);
				minDistance = normals[minFaceIndex].w;

				// Compute the support point in the direction of the closest face normal
				SupportPoint support = Support(objA, objB, minNormal);
				float sDistance = dot(minNormal, support.minkowDiff);

				if (sDistance - minDistance < 0.001f)
				{
					//Proceed Calculate the collision point 
					const SupportPoint closestFace[] =
					{
						polytope[faces[minFaceIndex * 3]],
						polytope[faces[minFaceIndex * 3 + 1]],
						polytope[faces[minFaceIndex * 3 + 2]]
					};


					std::pair<glm::vec3, glm::vec3> colPoints = CalcCollisionPoint(closestFace);

					Debug::DrawBox(colPoints.first, glm::vec3(), glm::vec3(0.1, 0.1, 0.1), glm::vec4(1, 0, 0, 1), 2);
					Debug::DrawBox(colPoints.second, glm::vec3(), glm::vec3(0.1, 0.1, 0.1), glm::vec4(0, 0, 1, 1), 2);
					Debug::DrawLine(colPoints.first, colPoints.first - normalize(minNormal) * minDistance, glm::vec4(0, 1, 0, 1), 3);
#ifndef NDEBUG
					for (int i = 0; i < faces.size(); i += 3) {
						Debug::DrawLine(polytope[faces[i]].minkowDiff, polytope[faces[i + 1]].minkowDiff, glm::vec4(1, 1, 1, 1), 2);
						Debug::DrawLine(polytope[faces[i + 1]].minkowDiff, polytope[faces[i + 2]].minkowDiff, glm::vec4(1, 1, 0, 1), 2);
						Debug::DrawLine(polytope[faces[i + 2]].minkowDiff, polytope[faces[i]].minkowDiff, glm::vec4(1, 1, 0, 1), 2);
					}
#endif
					collisionPoints = colPoints;
					collisionNormal = normalize(minNormal);
					penetrationDepth = minDistance;

					return true;
				}

				//Otherwise expand the polytope
				minDistance = FLT_MAX;
				uniqueEdges.clear();

				// Find unique edges of the faces visible to the new support point
				for (int i = 0; i < normals.size(); i++) {
					if (glm::dot(glm::vec3(normals[i]), support.minkowDiff) > 0) {
						int faceStart = i * 3;

						AddIfUniqueEdge(uniqueEdges, faces, faceStart, faceStart + 1);
						AddIfUniqueEdge(uniqueEdges, faces, faceStart + 1, faceStart + 2);
						AddIfUniqueEdge(uniqueEdges, faces, faceStart + 2, faceStart);

						faces.erase(faces.begin() + faceStart, faces.begin() + faceStart + 3);
						normals.erase(normals.begin() + i);
						i--; //adjust the index since we removed current face
					}
				}

				if (uniqueEdges.empty())
					return false;

				std::vector<int> newFaces;
				for (auto& edge : uniqueEdges) {
					newFaces.push_back(edge.first);
					newFaces.push_back(edge.second);
					newFaces.push_back(polytope.size());
				}

				polytope.push_back(support);

				auto newFaceNormals = GetFaceNormals(polytope, newFaces);
				std::vector<glm::vec4> newNormals = newFaceNormals.first;
				uint32 newMinFace = newFaceNormals.second;

				float currentMinDistance = FLT_MAX;
				for (int i = 0; i < normals.size(); i++) {
					if (normals[i].w < currentMinDistance) {
						currentMinDistance = normals[i].w;
						minFaceIndex = i;
					}
				}

				// Update the minimum face index if the new face is closer
				if (newNormals[newMinFace].w < currentMinDistance) {
					minFaceIndex = newMinFace + normals.size();
				}

				// Add the new faces to the existing list
				faces.insert(faces.end(), newFaces.begin(), newFaces.end());
				normals.insert(normals.end(), newNormals.begin(), newNormals.end());			
			}
		}

		void SolveCollisionV2(GameObject& objA, GameObject& objB, glm::vec3& collisionNormal, const float& penetrationDepth, std::pair<glm::vec3, glm::vec3> collisionPoints)
		{
			// Early exit if objects are immovable or massless 
			if (objA.mass + objB.mass <= 0.0f) return;

			const glm::vec3& pointA = collisionPoints.first;
			const glm::vec3& pointB = collisionPoints.second;

			glm::vec3 rA = pointA - objA.GetPosition();
			glm::vec3 rB = pointB - objB.GetPosition();

			// Calculate the velocity at the contact points
			glm::vec3 velocityA = objA.velocity + glm::cross(objA.angularVelocity, rA);
			glm::vec3 velocityB = objB.velocity + glm::cross(objB.angularVelocity, rB);

			const float invMassA = (objA.mass > 0) ? 1.0f / objA.mass : 0.0f;
			const float invMassB = (objB.mass > 0) ? 1.0f / objB.mass : 0.0f;
			glm::mat3 inertiaA = (invMassA > 0.0f) ? objA.inertiaTensorInWorld : glm::mat3();
			glm::mat3 inertiaB = (invMassB > 0.0f) ? objB.inertiaTensorInWorld : glm::mat3();

			//Calculate effective mass along the collision normal
			float effectiveMassA = invMassA + glm::dot(glm::cross(inertiaA * glm::cross(rA, collisionNormal), rA), collisionNormal);
			float effectiveMassB = invMassB + glm::dot(glm::cross(inertiaB * glm::cross(rB, collisionNormal), rB), collisionNormal);
			float relativeVelocity = glm::dot(collisionNormal, velocityA - velocityB);

			//Positional correction resolve
			const float correctionPercentage = 0.9f; // 90% of the penetration
			const float penetrationSlop = 0.01f; // Acceptable penetration 

			glm::vec3 correction = collisionNormal * correctionPercentage *
								   std::max(penetrationDepth - penetrationSlop, 0.0f) 
								   / (invMassA + invMassB);

			if(objA.mass > 0) objA.GetPosition() -= correction * invMassA;
			if (objB.mass > 0) objB.GetPosition() += correction * invMassB;

			const float seperationThreshold = 0.001f;
			if (relativeVelocity < seperationThreshold) return;

			// Calculate impulse scalar (force) based on the restitution coefficient (bounciness)
			const float restitution = 0.2f; // Low restitution for inelastic collision
			float impulse = -(1.0f + restitution) * relativeVelocity / (effectiveMassA + effectiveMassB);
			ApplyForce(&objA, collisionNormal, impulse, pointA);
			ApplyForce(&objB, -collisionNormal, impulse, pointB);
		}

};