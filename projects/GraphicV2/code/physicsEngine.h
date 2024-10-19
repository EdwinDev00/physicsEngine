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
		std::vector<GameObject*> objects; // list of objects in the scene
		bool enableGravity = false;
		glm::vec3 gravity; // user defined gravity vector

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
				obj->isColliding = false;
				obj->debugC = glm::vec4(0, 0, 1, 0);
			}

			std::vector<std::pair<GameObject*, GameObject*>> potentialCollisions = SweepAndPrune();
			const int maxIteration = 10;

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

					objA.isColliding = true;
					objB.isColliding = true;

					//EPA 
					glm::vec3 collisionNormal = glm::vec3(0);
					float penetrationDepth = 0;
					std::pair<glm::vec3, glm::vec3> collisionPoints;

					if (EPAV2(objA, objB, simplexPs, collisionNormal, penetrationDepth, collisionPoints))
						SolveCollision(objA, objB, collisionNormal, penetrationDepth,collisionPoints);
				}
			}
		}

		//Apply gravity upon all the object and update object position and velocity
		void ApplyGravity(GameObject* obj)
		{
			if (!obj->mass > 0) //FOr NOW LETS LIMIT (SO NOT OCCURINGG ANY ZERO DIVISION) (SOLUTION IN OBJ CONSTRUCT (0 MASS Manually set it to 1))
				return;
				
			//if (obj->isColliding) return;
			//glm::vec3 gravityForce = obj->mass * gravity;
				obj->acceleration = gravity; //gravityForce / obj->mass;

		}

		void ApplyForce( GameObject* obj, const glm::vec3& forceDirection, float forceMagnitude, const glm::vec3& hitP)
		{
			if (obj->mass <= 0) return;

			glm::vec3 force = forceDirection * forceMagnitude;
			obj->velocity += force / obj->mass;

			//Calculate the vector from the center of mass to the hit point (r)
			glm::vec3 r = hitP - obj->boundingbox.GetPosition() ;

			// Calculate the torque: τ = r × F
			glm::vec3 torque = glm::cross(r, force);
			obj->angularVelocity += obj->inertiaTensorInWorld * torque;
		}

		void ApplyForce(GameObject* obj, const glm::vec3& forceDirection, float forceMagnitude)
		{
			if (obj->mass <= 0) return;

			glm::vec3 force = forceDirection * forceMagnitude;
			obj->velocity += force / obj->mass;
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
			glm::vec3 direction(0, 0, 1); // arbitrary
			SupportPoint support = Support(objA, objB, direction);
			simplex.push_front(support);

			direction = -support.minkowDiff;
			while(true)
			{
				// Get the new support point  in the current direction
				support = Support(objA, objB, direction); // get another point in current direction
				
				//Check if the new point past the origin
				float dotP = glm::dot(support.minkowDiff, direction);
				if ( dotP < 0)
				{
					return false; //no collision
				}

				simplex.push_front(support);

				if (HandleSimplex(simplex, direction))
				{
					return true; // Collision Detect
				}
			}
		}

		bool EPAV2(const GameObject& objA, const GameObject& objB,const Simplex& simplex, glm::vec3& collisionNormal, float& penetrationDepth, std::pair<glm::vec3, glm::vec3>& collisionPoints)
		{
			PolytopeData polytopeData = InitalizePolytopeV2(simplex);
			int iterationCount = 100;
			int closestFaceIndex = 0;
			// WHILE LOOP 
			while(iterationCount--)
			{
				//Debug::ClearQueue();
				//----------- FIND CLOSEST FACE --------------------------------

				float minDistance = dot(polytopeData.polytope[polytopeData.face[closestFaceIndex].polytopeIndices[0]].minkowDiff, normalize(polytopeData.face[closestFaceIndex].normal));//polytopeData.face[0].distance;

				for(int i = 0; i < polytopeData.face.size(); i++)
				{
					float distance = dot(polytopeData.polytope[polytopeData.face[i].polytopeIndices[0]].minkowDiff, normalize(polytopeData.face[i].normal));
					if(distance < minDistance)
					{
						minDistance = distance;
						closestFaceIndex = i;
					}
				}
#ifndef NDEBUG
				//Visualize the whole polytope
#endif
				VisualizePolytopeWithClosestFace(polytopeData, closestFaceIndex);

				glm::vec3 closestFaceNormal = normalize(polytopeData.face[closestFaceIndex].normal);
				float closestFaceDistance = polytopeData.face[closestFaceIndex].distance;

				// --------- FIND THE NEW SUPPORT POINT -----------------------------
				// Calculate the support point along the closest face normal
				SupportPoint supportPoint = Support(objA, objB, closestFaceNormal);
				float newDistance = glm::dot(closestFaceNormal,supportPoint.minkowDiff);
				if (newDistance - closestFaceDistance < 1e-6f)
				{

					collisionNormal = closestFaceNormal;

					// ----------------- CALCULATE COLLISION POINTS -------------------

					// Support point is the Minkowski difference between objA and objB
					const Face& closestFace = polytopeData.face[closestFaceIndex];

					// Retrieve the vertices of the triangle from the polytope (Minkowski difference)
					SupportPoint A = polytopeData.polytope[closestFace.polytopeIndices[0]];
					SupportPoint B = polytopeData.polytope[closestFace.polytopeIndices[1]];
					SupportPoint C = polytopeData.polytope[closestFace.polytopeIndices[2]];

					// Project the support point onto the triangle (M1, M2, M3) to get the barycentric coordinates
					glm::vec3 projectedPoint;
					float u, v, w;
					glm::vec3 closestBary = ProjectToTriangle(glm::vec3(0,0,0), A.minkowDiff, B.minkowDiff, C.minkowDiff); //MUST OF THE CONNECTION IS RIGHT THOSE WHO ARE FAULT IS DUE TO CLOSETBARY IS INF
					////IF NAN
					//if (closestBary.x != closestBary.x || closestBary.y != closestBary.y || closestBary.z != closestBary.z)
					//	return false;

					// ----------------------- Barycentric interpolation for A and B -----------------------
					if (!Barycentric(A.minkowDiff, B.minkowDiff, C.minkowDiff, closestBary, u, v, w)) return false;
					
					glm::vec3 worldCollisionPointA = Cartesian(glm::vec3(u,v,w), A.Asupport, B.Asupport, C.Asupport);
					glm::vec3 worldCollisionPointB = Cartesian(glm::vec3(u,v,w), A.Bsupport, B.Bsupport, C.Bsupport);
					
					// Store the calculated collision points
					collisionPoints = std::make_pair(worldCollisionPointA, worldCollisionPointB);

#ifndef NDEBUG
					// Debug: Draw the exact collision points
					Debug::DrawBox(worldCollisionPointA, objA.GetRotation(), glm::vec3(0.1f), glm::vec4(1, 0, 0, 1), 0.02f); // Red box for objA
					Debug::DrawBox(worldCollisionPointB, objB.GetRotation(), glm::vec3(0.1f), glm::vec4(0, 1, 0, 1), 0.02f); // Green box for objB
					Debug::DrawLine(worldCollisionPointA, worldCollisionPointB, glm::vec4(1, 0, 0, 0), 2);
#endif // !NDEBUG

					// Calculate the penetration depth
					penetrationDepth = closestFaceDistance;
					return true;
				}
				
				//-------------- EXPANDING POLYTOPE EDGE BASED -------------------------
				if (!ExpandPolytope(polytopeData, closestFaceIndex, supportPoint))
						return false;

			}
			return false;
		}

		void SolveCollision(GameObject& objA, GameObject& objB, glm::vec3& collisionNormal, const float& penetrationDepth, std::pair<glm::vec3, glm::vec3> collisionPoints)
		{
			//If both objects are immovable, skip
			if (objA.mass + objB.mass <= 0.0f) return;

			const glm::vec3 contactPA = collisionPoints.first;
			const glm::vec3 contactPB = collisionPoints.second;
		
			const glm::vec3 rA = contactPA - objA.GetPosition(); //radius vector / relation vector
			const glm::vec3 rB = contactPB - objB.GetPosition();

			//Contact velocity: linear velocity + cross angular , radius vector
			const glm::vec3 contactAVelocity = collisionNormal * (objA.velocity + cross(objA.angularVelocity, rA)); //Satisfy the contact constraint stopping vector
			const glm::vec3 contactBVelocity = collisionNormal * (objB.velocity + cross(objB.angularVelocity, rB));
			
			//CALCULATE PENETRATION CORRECTION
			const float percent = 0.9f;
			const float slop = 0.01f;

			glm::vec3 correction = collisionNormal * percent
				* std::max(penetrationDepth - slop, 0.0f)
				/ (objA.mass + objB.mass);

			const float relativeVelocity = dot(collisionNormal, contactAVelocity - contactBVelocity);

			//configure the seperation threshold
			if (relativeVelocity < 0.001f) //Seperation  A-B = negative velocity seperated, positive = colliding
			{
				return;
			}
			if (relativeVelocity == 0) //at rest
			{
				//objA.isColliding = true;
				//objB.isColliding = true;
				if (objA.mass > 0.0f)
					objA.GetPosition() -= correction * 1.0f/objA.mass; // Move A proportional to A's mass
				if (objB.mass > 0.0f)
					objB.GetPosition() += correction * 1.0f/objB.mass; // Move B proportional to B's mass
				return;
			}

			//Calculate the effective mass (brute force this for now)
			float effectiveMassA = 0; float effectiveMassB = 0;
			float invMassB = 0; float invMassA = 0;
			glm::mat3 inertiaTensorA(1.0f); glm::mat3 inertiaTensorB(1.0f);
			if(objA.mass > 0)
			{
				invMassA  = 1.0f / objA.mass;
				inertiaTensorA = objA.inertiaTensorInWorld;
				effectiveMassA = 1.0f / invMassA + dot(inertiaTensorA * cross(rA, collisionNormal), collisionNormal);
			}

			if (objB.mass > 0)
			{
				invMassB = 1.0f / objB.mass;
				inertiaTensorB = objB.inertiaTensorInWorld;
				effectiveMassB = 1.0f / invMassB + dot(inertiaTensorB * cross(rB, collisionNormal), collisionNormal);
			}

			const float restitutionCoefficient = 0.80f;
				
			//CALCULATE FINAL IMPULSE (brute force)
			glm::vec3 crossA = cross(rA, collisionNormal);
			glm::vec3 crossAngularA = cross(inertiaTensorA * crossA, rA);
			float angularA = dot(collisionNormal, crossAngularA);

			glm::vec3 crossB = cross(rB, collisionNormal);
			glm::vec3 crossAngularB = cross(inertiaTensorB * crossB, rB);
			float angularB = dot(collisionNormal, crossAngularB);
			
			float denominator = invMassA + invMassB + angularA + angularB;

			const float impulse = -(1.0f + restitutionCoefficient) * relativeVelocity / denominator;

			if (objA.mass > 0.0f ) {
				objA.GetPosition() -= correction * invMassA; // Move A proportional to A's mass
				/*glm::vec3 torque = glm::cross(contactPA - objA.GetPosition(), glm::vec3(impulse));
				objA.angularVelocity += objA.inertiaTensorInWorld * torque;*/
			}
			if (objB.mass > 0.0f) {
				objB.GetPosition() += correction * invMassB; // Move B proportional to B's mass
				//glm::vec3 torque = glm::cross(contactPB - objB.GetPosition(), glm::vec3(impulse));
				//objB.angularVelocity += objB.inertiaTensorInWorld * torque;
			}
			ApplyForce(&objA, collisionNormal, impulse,contactPA);
			ApplyForce(&objB, -collisionNormal, impulse, contactPB);
			//APPLY THE IMPULS
		}
};