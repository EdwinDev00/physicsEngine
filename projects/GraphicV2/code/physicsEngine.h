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
			const int maxIteration = 10;

#ifndef NDEBUG
			for (GameObject* obj : objects) //only for debug remove this later
				obj->debugC = glm::vec4(0, 0, 1, 1); //reset color
#endif
			//Resolving collision respond
			/*for(int i = 0; i < maxIteration; i++)
			{
				bool collisionResolved = false;*/
				for(const auto& pair : potentialCollisions)
				{
					GameObject& objA = *pair.first;
					GameObject& objB = *pair.second;
					Simplex simplexPs;	
					std::vector<glm::vec3> SupportA; //these point is local space (add the transform when returning)
					std::vector<glm::vec3> SupportB;

					bool collision = GJK(objA,objB,simplexPs,SupportA,SupportB);
					if(collision)
					{
						//collisionResolved = true;
						objA.debugC = glm::vec4(1, 0, 1, 1); 
						objB.debugC = glm::vec4(1, 0, 1, 1);

						//EPA 
						glm::vec3 collisionNormal = glm::vec3(0);
						float penetrationDepth = 0;
						std::pair<glm::vec3, glm::vec3> collisionPoints;
						EPAV2(objA,objB,simplexPs, collisionNormal, penetrationDepth, collisionPoints,SupportA,SupportB);

						//Collision response logic
						//Debug::DrawLine(objA.GetPosition(), objA.GetPosition() + collisionNormal * penetrationDepth, glm::vec4(1, 0, 0, 1), 2);
						ApplyImpulse(objA, objB, collisionNormal, penetrationDepth,collisionPoints);

					}
				}

			/*	if (!collisionResolved) break;
			}*/

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


		bool GJK(const GameObject& objA, const GameObject& objB, Simplex& simplex, std::vector<glm::vec3>& SupportA, std::vector<glm::vec3>& SupportB)
		{
			glm::vec3 direction(1, 0, 0); // arbitrary
			glm::vec3 support = Support(objA, objB, direction);
			simplex.push_front(support);
			SupportA.push_back(objA.TransformLocalPointToWorldSpace(GetFurthestPointInDirection(objA, direction))); //stores (not regarding the objects rotation and scale)
			SupportB.push_back(objB.TransformLocalPointToWorldSpace(GetFurthestPointInDirection(objB, -direction)));

			//if the first simplex passsed the origin, return no collision
			if (glm::dot(support, direction) <= 0) 
				return false;
			direction = -support; //new direction towards origin

			int iterationCount = 50;
			//Main loop: simplex construction
			while(true)
			{
				// Get the new support point  in the current direction
				support = Support(objA, objB, glm::normalize(direction)); // get another point in current direction
				SupportA.push_back(objA.TransformLocalPointToWorldSpace(GetFurthestPointInDirection(objA, direction)));
				SupportB.push_back(objB.TransformLocalPointToWorldSpace(GetFurthestPointInDirection(objB, -direction)));
				
				//Check if the new point past the origin
				if (glm::dot(support, direction) <= 0)
					return false; //no collision

				simplex.push_front(support);
				if (HandleSimplex(simplex, direction)) //ERROR the cross product becomes zero (due to simplex inside one of the component is zero)
				{
					return true; // Collision Detect
				}
			}
		}

		void EPAV2(const GameObject& objA, const GameObject& objB,const Simplex& simplex, glm::vec3& collisionNormal, float& penetrationDepth, std::pair<glm::vec3, glm::vec3>& collisionPoints, std::vector<glm::vec3>& SupportA, std::vector<glm::vec3>& SupportB)
		{
			glm::mat3 modelRotationA = glm::transpose(objA.GetRotationMat());
			glm::mat3 modelRotationB = glm::transpose(objB.GetRotationMat());

			PolytopeData polytopeData = InitalizePolytopeV2(simplex);
			//Reverse the order of the supportA and B to have right widning order
			std::reverse(SupportA.begin(), SupportA.end());
			std::reverse(SupportB.begin(), SupportB.end());

			int iterationCount = 50;
			// WHILE LOOP 
			while(iterationCount--)
			{
				Debug::ClearQueue();
				//----------- FIND CLOSEST FACE --------------------------------
				int closestFaceIndex = 0;

				float minDistance = polytopeData.face[0].distance;

				for(int i = 1; i < polytopeData.face.size(); i++)
				{
					if(polytopeData.face[i].distance < minDistance)
					{
						minDistance = polytopeData.face[i].distance;
						closestFaceIndex = i;
					}
				}
#ifndef NDEBUG
				//Visualize the whole polytope
				VisualizePolytopeWithClosestFace(polytopeData, closestFaceIndex);
#endif
				glm::vec3 closestFaceNormal = glm::normalize(polytopeData.face[closestFaceIndex].normal);
				float closestFaceDistance = polytopeData.face[closestFaceIndex].distance;

				// --------- FIND THE NEW SUPPORT POINT -----------------------------
				// Calculate the support point along the closest face normal
				glm::vec3 supportPoint = Support(objA, objB, closestFaceNormal);
				float newDistance = glm::dot(closestFaceNormal,supportPoint);
				if (newDistance - closestFaceDistance < 1e-6f)
				{
					collisionNormal = closestFaceNormal;
					// ----------------- CALCULATE COLLISION POINTS -------------------

					// Support point is the Minkowski difference between objA and objB
					const Face& closestFace = polytopeData.face[closestFaceIndex];
					glm::vec3 differenceAB = supportPoint;  // Minkowski difference between objA and B

					// Retrieve the vertices of the triangle from the polytope, forming the Minkowski difference
					glm::vec3 M1 = polytopeData.polytope[closestFace.polytopeIndices[0]];
					glm::vec3 M2 = polytopeData.polytope[closestFace.polytopeIndices[1]];
					glm::vec3 M3 = polytopeData.polytope[closestFace.polytopeIndices[2]];

					// Project the support point onto the triangle (M1, M2, M3) to get the barycentric coordinates
					glm::vec3 closestBary = ProjectToTriangle(glm::vec3(), M1, M2, M3);

					// Retrieve the corresponding support points on objects A and B
					glm::vec3 A1 = SupportA[closestFace.polytopeIndices[0]];
					glm::vec3 A2 = SupportA[closestFace.polytopeIndices[1]];
					glm::vec3 A3 = SupportA[closestFace.polytopeIndices[2]];

					glm::vec3 B1 = SupportB[closestFace.polytopeIndices[0]];
					glm::vec3 B2 = SupportB[closestFace.polytopeIndices[1]];
					glm::vec3 B3 = SupportB[closestFace.polytopeIndices[2]];

					// Calculate the collision points on object A and B using barycentric interpolation
					glm::vec3 localCollisionPointA = Cartesian(closestBary, A1, A2, A3);
					glm::vec3 localCollisionPointB = Cartesian(closestBary, B1, B2, B3);

					// Transform the local collision points to world space
					glm::vec3 worldCollisionPointA = localCollisionPointA;//objA.TransformLocalPointToWorldSpace(localCollisionPointA);
					glm::vec3 worldCollisionPointB = localCollisionPointB;//objB.TransformLocalPointToWorldSpace(localCollisionPointB);

					// Store the calculated collision points
					collisionPoints = std::make_pair(worldCollisionPointA, worldCollisionPointB);

					// Debug: Draw the exact collision points
					Debug::DrawBox(worldCollisionPointA, objA.GetRotation(), glm::vec3(0.1f), glm::vec4(1, 0, 0, 1), 0.02f); // Red box for objA
					Debug::DrawBox(worldCollisionPointB, objB.GetRotation(), glm::vec3(0.1f), glm::vec4(0, 1, 0, 1), 0.02f); // Green box for objB
					Debug::DrawLine(worldCollisionPointA, worldCollisionPointB, glm::vec4(1, 0, 0, 0), 2);

					penetrationDepth = glm::length(worldCollisionPointA - worldCollisionPointB) * closestFaceDistance;
					Debug::DrawLine(worldCollisionPointA, worldCollisionPointB, glm::vec4(1, 0, 0, 0), 2);

					return;
				}
				
				//-------------- EXPANDING POLYTOPE EDGE BASED -------------------------
				ExpandPolytope(polytopeData, closestFaceIndex, supportPoint,SupportA,SupportB,objA,objB);
			}
		}

		void ApplyImpulse(GameObject& objA,  GameObject& objB, const glm::vec3& collisionNormal, const float penetrationDepth, std::pair<glm::vec3,glm::vec3> collisionPoints)
		{
			// First resolve the penetration of the objects
			float totalMass = objA.mass + objB.mass;
			if (totalMass == 0.0f) return; // Avoid division by zero for zero mass

			const float velocityThreshold = 1e-4f;
			const float angularVelocityThreshold = 1e-4f;

			//bool objAAtRest = glm::length(objA.velocity) < velocityThreshold && glm::length(objA.angularVelocity) < angularVelocityThreshold;
			//bool objBAtRest = glm::length(objB.velocity) < velocityThreshold && glm::length(objB.angularVelocity) < angularVelocityThreshold;

			//if (objAAtRest && objBAtRest) return; // Both objects are at rest

			// ----------------- PENETRATION RESOLUTION --------------------

			// Use a bias factor to smooth out the penetration resolution over multiple frames
			const float correctionBias = 0.2f;  // Factor between 0.0 (no correction) and 1.0 (full correction)
			float correctedPenetrationDepth = correctionBias * penetrationDepth;

			// Move the objects out of penetration
			glm::vec3 correctionA = (correctedPenetrationDepth * (objA.mass / totalMass)) * collisionNormal;
			glm::vec3 correctionB = (correctedPenetrationDepth * (objB.mass / totalMass)) * collisionNormal;

			// Apply corrections only to objects with mass
			if (objA.mass > 0.0f) objA.GetPosition() -= correctionA;
			if (objB.mass > 0.0f) objB.GetPosition() += correctionB;

			// Apply small rotational correction based on how the object is penetrated
			glm::vec3 rA = collisionPoints.first - objA.GetPosition();
			glm::vec3 rB = collisionPoints.second - objB.GetPosition();

			glm::vec3 angularCorrectionA = glm::cross(rA, correctionA);
			glm::vec3 angularCorrectionB = glm::cross(rB, correctionB);

			// Apply the angular correction (small) to simulate rotation response during penetration
			if (objA.mass > 0.0f) objA.angularVelocity += objA.inertiaTensorInWorld * angularCorrectionA * 0.05f;
			if (objB.mass > 0.0f) objB.angularVelocity += objB.inertiaTensorInWorld * angularCorrectionB * 0.05f;

			// -------------- IMPULSE APPLICATION (ELASTIC RESPONSE) ---------------

			// Coefficient of restitution (how bouncy the objects are)
			float restitution = 0.8f; // You can adjust this value for more or less bounce

			// Relative velocity
			glm::vec3 relativeVelocity = objB.velocity - objA.velocity;

			// Velocity along the collision normal
			float velAlongNormal = glm::dot(relativeVelocity, collisionNormal);

			// If the velocities are separating (no collision response needed)
			if (velAlongNormal > 0)
				return;

			// Calculate the impulse scalar: j = -(1.0f + restitution) * Vrelative / (invMA + invMB)
			float invMassA = (objA.mass > 0.0f) ? (1.0f / objA.mass) : 0.0f;
			float invMassB = (objB.mass > 0.0f) ? (1.0f / objB.mass) : 0.0f;

			if (invMassA == 0.0f && invMassB == 0.0f) return; // If both have no mass, do not apply impulse

			float impulseScalar = -(1.0f + restitution) * velAlongNormal / (invMassA + invMassB);

			// Apply impulse to both objects
			glm::vec3 impulse = impulseScalar * collisionNormal;

			// Apply the impulse to objects
			if (objA.mass > 0.0f) objA.velocity -= invMassA * impulse;
			if (objB.mass > 0.0f) objB.velocity += invMassB * impulse;

			// ------------- FRICTION HANDLING (REALISTIC SLIDING) -------------

			// Calculate tangential velocity (velocity not along the normal)
			glm::vec3 tangentVelocity = relativeVelocity - velAlongNormal * collisionNormal;
			glm::vec3 tangent = glm::normalize(tangentVelocity);

			// Apply friction only if there is a tangential velocity
			if (glm::length(tangentVelocity) > velocityThreshold) {
				float frictionCoefficient = 0.5f; // Adjust as needed for friction amount
				float frictionImpulseScalar = glm::length(tangentVelocity) / (invMassA + invMassB);

				// Apply friction impulse in the opposite direction of the tangent
				glm::vec3 frictionImpulse = -frictionCoefficient * frictionImpulseScalar * tangent;

				// Apply friction impulse
				if (objA.mass > 0.0f) objA.velocity -= invMassA * frictionImpulse;
				if (objB.mass > 0.0f) objB.velocity += invMassB * frictionImpulse;
			}

			// ------------- ROTATIONAL RESPONSE TO COLLISION -------------

			// Calculate torque for angular velocity
			glm::vec3 torqueA = glm::cross(rA, impulse);
			glm::vec3 torqueB = glm::cross(rB, impulse);

			// Apply torque scaling to prevent excessive rotation
			const float torqueScaleFactor = 0.1f;  // Scale down torque to prevent excessive angular velocity changes
			if (objA.mass > 0.0f) objA.angularVelocity += torqueScaleFactor * objA.inertiaTensorInWorld * torqueA;
			if (objB.mass > 0.0f) objB.angularVelocity += torqueScaleFactor * objB.inertiaTensorInWorld * torqueB;

			// Add angular friction to simulate rotational slowing down over time
			const float angularFrictionCoefficient = 0.02f; // A small amount to slow down rotation
			if (glm::length(objA.angularVelocity) > angularVelocityThreshold)
				objA.angularVelocity *= (1.0f - angularFrictionCoefficient);

			if (glm::length(objB.angularVelocity) > angularVelocityThreshold)
				objB.angularVelocity *= (1.0f - angularFrictionCoefficient);
		}
};