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

			for (GameObject* obj : objects)
			{
				ApplyGravity(obj);
				obj->OnUpdate(deltaTime);
				obj->isColliding = false;
			}

			std::vector<std::pair<GameObject*, GameObject*>> potentialCollisions = SweepAndPrune();
			const int maxIteration = 10;

#ifndef NDEBUG
#endif
			//for (GameObject* obj : objects) //only for debug remove this later
			//{
			//	Debug::DrawBox(obj->boundingbox.GetPosition(), glm::vec3(), obj->boundingbox.GetExtents(), obj->debugC, 1.0f);
			//}

				for(const auto& pair : potentialCollisions)
				{
					GameObject& objA = *pair.first;
					GameObject& objB = *pair.second;
					Simplex simplexPs;	


					bool collision = GJK(objA,objB,simplexPs);
					if(collision)
					{
						//collisionResolved = true;


						objA.debugC = glm::vec4(1, 0, 1, 1);
						objB.debugC = glm::vec4(1, 0, 1, 1);
#ifndef NDEBUG
						//// Predefined tetrahedron faces
						//std::vector<std::array<int, 3>> indices =
						//{
						//	{0,1,2},
						//	{0,3,1},
						//	{0,2,3},
						//	{1,3,2}
						//};
						//for(const auto& i : indices)
						//{
						//	Debug::DrawLine(SupportA[i[0]], SupportA[i[1]], glm::vec4(1, 0, 1, 0), 1);
						//	Debug::DrawLine(SupportA[i[1]], SupportA[i[2]], glm::vec4(1, 0, 1, 0), 1);
						//	Debug::DrawLine(SupportA[i[2]], SupportA[i[0]], glm::vec4(1, 0, 1, 0), 1);

						//	Debug::DrawLine(SupportB[i[0]], SupportB[i[1]], glm::vec4(1, 0.7f, 0, 0), 1);
						//	Debug::DrawLine(SupportB[i[1]], SupportB[i[2]], glm::vec4(1, 0.7f, 0, 0), 1);
						//	Debug::DrawLine(SupportB[i[2]], SupportB[i[0]], glm::vec4(1, 0.7f, 0, 0), 1);

						//	Debug::DrawLine(simplexPs[i[0]] + glm::vec3(1), simplexPs[i[1]] + glm::vec3(1), glm::vec4(1, 0, 0, 0), 1);
						//	Debug::DrawLine(simplexPs[i[1]] + glm::vec3(1), simplexPs[i[2]] + glm::vec3(1), glm::vec4(1, 0, 0, 0), 1);
						//	Debug::DrawLine(simplexPs[i[2]] + glm::vec3(1), simplexPs[i[0]] + glm::vec3(1), glm::vec4(1, 0, 0, 0), 1);
						//}
#endif
						objA.isColliding = true;
						objB.isColliding = true;

						//EPA 
						glm::vec3 collisionNormal = glm::vec3(0);
						float penetrationDepth = 0;
						std::pair<glm::vec3, glm::vec3> collisionPoints;

						//ERROR:PROBLEM WITH ROTATED OBJECT, CLOSEST FACE IS NOT FINDING THE BEST ONE (POTENTIAL ERROR IN EXPANDING POLYTOPE)
						EPAV2(objA,objB,simplexPs, collisionNormal, penetrationDepth, collisionPoints);
						SolveCollision(objA, objB, collisionNormal, penetrationDepth,collisionPoints);

						////JUST TO TEST
						//objA.OnUpdate(deltaTime);
						//objB.OnUpdate(deltaTime);


					}
				}
		}

		//Apply gravity upon all the object and update object position and velocity
		void ApplyGravity(GameObject* obj)
		{
			if (!obj->mass > 0) //FOr NOW LETS LIMIT (SO NOT OCCURINGG ANY ZERO DIVISION) (SOLUTION IN OBJ CONSTRUCT (0 MASS Manually set it to 1))
				return;
				
			//glm::vec3 gravityForce = obj->mass * gravity;
			if (!obj->isColliding)
				obj->acceleration = gravity; //gravityForce / obj->mass;
			else obj->acceleration = glm::vec3(0.0f);

		}

		void ApplyForce( GameObject* obj, const glm::vec3& forceDirection, float forceMagnitude, const glm::vec3& hitP)
		{
			if (obj->mass <= 0) return;

			glm::vec3 force = forceDirection * forceMagnitude;
			obj->velocity += force / obj->mass;

			////Calculate the vector from the center of mass to the hit point (r)
			//glm::vec3 r = hitP - obj->boundingbox.GetPosition() ;

			//// Calculate the torque: τ = r × F
			//glm::vec3 torque = glm::cross(r, force);
			//obj->angularVelocity += obj->inertiaTensorInWorld * torque;
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
					std::cout << dotP << '\n';
					return false; //no collision
				}

				simplex.push_front(support);

				if (HandleSimplex(simplex, direction))
				{
					return true; // Collision Detect
				}
			}
		}

		void EPAV2(const GameObject& objA, const GameObject& objB,const Simplex& simplex, glm::vec3& collisionNormal, float& penetrationDepth, std::pair<glm::vec3, glm::vec3>& collisionPoints)
		{
			PolytopeData polytopeData = InitalizePolytopeV2(simplex);
			int iterationCount = 100;
			int closestFaceIndex = 0;
			// WHILE LOOP 
			while(iterationCount--)
			{
				Debug::ClearQueue();
				////----------- FIND CLOSEST FACE --------------------------------

				float minDistance = -FLT_MAX; //polytopeData.face[0].distance;

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
#endif

				VisualizePolytopeWithClosestFace(polytopeData, closestFaceIndex);
				glm::vec3 closestFaceNormal = polytopeData.face[closestFaceIndex].normal;
				float closestFaceDistance = polytopeData.face[closestFaceIndex].distance;

				// --------- FIND THE NEW SUPPORT POINT -----------------------------
				// Calculate the support point along the closest face normal
				SupportPoint supportPoint = Support(objA, objB, closestFaceNormal);
				float newDistance = glm::dot(closestFaceNormal,supportPoint.minkowDiff);
				float con = newDistance - closestFaceDistance;
				if ( con < 1e-6f)
				{
					//ERROR when handlesimplex get normalized direction leading to not getting in here but fix gjk rotation problem
					collisionNormal = closestFaceNormal;

					// ----------------- CALCULATE COLLISION POINTS -------------------

					// Support point is the Minkowski difference between objA and objB
					const Face& closestFace = polytopeData.face[closestFaceIndex];

					// Retrieve the vertices of the triangle from the polytope (Minkowski difference)
					SupportPoint A = polytopeData.polytope[closestFace.polytopeIndices[0]];
					SupportPoint B = polytopeData.polytope[closestFace.polytopeIndices[1]];
					SupportPoint C = polytopeData.polytope[closestFace.polytopeIndices[2]];

					// Project the support point onto the triangle (M1, M2, M3) to get the barycentric coordinates
					glm::vec3 closestBary = ProjectToTriangle(glm::vec3(), A.minkowDiff, B.minkowDiff, C.minkowDiff);

					// ----------------------- Barycentric interpolation for A and B -----------------------

					// Calculate the collision points on object A and B using barycentric interpolation
					glm::vec3 worldCollisionPointA = Cartesian(closestBary, A.Asupport, B.Asupport, C.Asupport);
					glm::vec3 worldCollisionPointB = Cartesian(closestBary, A.Bsupport, B.Bsupport, C.Bsupport);
					
					// Store the calculated collision points
					collisionPoints = std::make_pair(worldCollisionPointA, worldCollisionPointB);

					// Debug: Draw the exact collision points
					Debug::DrawBox(worldCollisionPointA, objA.GetRotation(), glm::vec3(0.1f), glm::vec4(1, 0, 0, 1), 0.02f); // Red box for objA
					Debug::DrawBox(worldCollisionPointB, objB.GetRotation(), glm::vec3(0.1f), glm::vec4(0, 1, 0, 1), 0.02f); // Green box for objB
					Debug::DrawLine(worldCollisionPointA, worldCollisionPointB, glm::vec4(1, 0, 0, 0), 2);

					// Calculate the penetration depth
					penetrationDepth = closestFaceDistance + 0.001f;//glm::length(worldCollisionPointA - worldCollisionPointB);
					//penetrationDepth = glm::distance(worldCollisionPointA ,worldCollisionPointB); //adding a bias value
					//std::cout << penetrationDepth << '\n';
					return;
				}
				
				//-------------- EXPANDING POLYTOPE EDGE BASED -------------------------
				ExpandPolytope(polytopeData, closestFaceIndex, supportPoint);

			}
		}

		void SolveCollision(GameObject& objA, GameObject& objB, glm::vec3& collisionNormal, const float& penetrationDepth, std::pair<glm::vec3, glm::vec3> collisionPoints)
		{
			/*
			* NOTE: WORKS PERFECT WHEN THE OBJECT HAS NO ANGULAR VELOCITY (CAUSE EPA RETURNING FAULTY CORRECT PEN AND NORMAL)
			*/

			//If both objects are immovable, skip
			if (objA.mass + objB.mass <= 0.0f) return;

			collisionNormal = normalize(collisionNormal);
			const float contactConstraint = dot(collisionNormal, objA.GetPosition() - objB.GetPosition());
			const glm::vec3 contactPA = collisionPoints.first;
			const glm::vec3 contactPB = collisionPoints.second;
			const glm::vec3 rA = contactPA - objA.GetPosition(); //radius vector / relation vector
			const glm::vec3 rB = contactPB - objB.GetPosition();

			//Contact velocity: linear velocity + cross angular , radius vector
			const glm::vec3 contactAVelocity = collisionNormal * (objA.velocity + cross(objA.angularVelocity, rA)); //Satisfy the contact constraint stopping vector
			const glm::vec3 contactBVelocity = collisionNormal * (objB.velocity + cross(objB.angularVelocity, rB));

			////TESTING THE VELOCITY CONSTRAINT 
			//glm::vec3 velocityConstraint = dot(collisionNormal, contactAVelocity - contactBVelocity) + cross(rA - rB, collisionNormal) * (objA.angularVelocity - objB.angularVelocity);
			

			const float relativeVelocity = dot(collisionNormal, contactAVelocity - contactBVelocity);
			if (relativeVelocity == 0) return;
			//std::cout << "relativeVelocity " << relativeVelocity << '\n'; //TESTING THE EPA AND PENETRATION RESOLUTION //REENABLE WHEN WORKING WITH VELOCITY

			//if (relativeVelocity < 0.1f) //Seperation  A-B = negative velocity seperated, positive = colliding
			//{
			//	std::cout << "seperated " << '\n';
			//	objA.isColliding = false;
			//	objB.isColliding = false;
			//	return;
			//}
			//if (relativeVelocity - 0.02f == 0) //at rest
			//{
			//	std::cout << "Resting " << '\n';
			//	objA.isColliding = true;
			//	objB.isColliding = true;
			//	objA.velocity.y = 0;
			//	objB.velocity.y = 0;
			//	return;
			//}

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

			const float restitutionCoefficient = 0.90f;
				
			//CALCULATE FINAL IMPULSE (brute force)
			glm::vec3 crossA = cross(rA, collisionNormal);
			glm::vec3 crossAngularA = cross(inertiaTensorA * crossA, rA);
			float angularA = dot(collisionNormal, crossAngularA);

			glm::vec3 crossB = cross(rB, collisionNormal);
			glm::vec3 crossAngularB = cross(inertiaTensorB * crossB, rB);
			float angularB = dot(collisionNormal, crossAngularB);
			
			float denominator = invMassA + invMassB + angularA + angularB;
			if (denominator == 0) return;
			const float impulse = -(1.0f + restitutionCoefficient) * relativeVelocity / denominator; //it became nan 0 division when colliding with ground
			//std::cout << "impulse FORCE " << impulse << '\n';
			
			//APPLY THE IMPULS
			ApplyForce(&objA, collisionNormal, impulse);
			ApplyForce(&objB, -collisionNormal, impulse);
			
			//RESOLVE THE PENETRATION
			const float percent = 0.99f;
			const float slop = 0.09f;

			glm::vec3 correction = collisionNormal * percent
				* std::max(penetrationDepth - slop, 0.0f)
				/ (objA.mass + objB.mass);


			if (objA.mass > 0.0f ) {
				objA.GetPosition() -= correction * invMassA; // Move A proportional to A's mass
				//objA.angularVelocity = crossAngularA * invMassA;

			}
			if (objB.mass > 0.0f) {
				objB.GetPosition() += correction * invMassB; // Move B proportional to B's mass
				//objB.angularVelocity = crossAngularB * invMassB;

			}


		//	// ------------- FRICTION HANDLING (REALISTIC SLIDING) -------------

		//	// Calculate tangential velocity (velocity not along the normal)
		//	glm::vec3 tangentVelocity = relativeVelocity - velAlongNormal * collisionNormal;
		//	glm::vec3 tangent = glm::normalize(tangentVelocity);

		//	// Apply friction only if there is a tangential velocity
		//	if (glm::length(tangentVelocity) > velocityThreshold) {
		//		float frictionCoefficient = 0.5f; // Adjust as needed for friction amount
		//		float frictionImpulseScalar = glm::length(tangentVelocity) / (invMassA + invMassB);

		//		// Apply friction impulse in the opposite direction of the tangent
		//		glm::vec3 frictionImpulse = -frictionCoefficient * frictionImpulseScalar * tangent;

		//		// Apply friction impulse
		//		if (objA.mass > 0.0f) objA.velocity -= invMassA * frictionImpulse;
		//		if (objB.mass > 0.0f) objB.velocity += invMassB * frictionImpulse;
		//	}
		}
};