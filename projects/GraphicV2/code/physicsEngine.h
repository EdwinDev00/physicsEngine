#pragma once

#include "dataCollection.h"
#include <vector>
#include "meshObjectManager.h"
#include "inlineFunc.h"

#define EPA_TOLERANCE 0.00001f

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

#if DEBUG
			for (GameObject* obj : objects) //only for debug remove this later
				obj->debugC = glm::vec4(0, 0, 1, 1); //reset color
#endif
			for(const auto& pair : potentialCollisions)
			{
				GameObject& objA = *pair.first;
				GameObject& objB = *pair.second;
				Simplex simplexPs;	

				bool collision = GJK(objA,objB,simplexPs); //GJK WORKS (WITH THE OFFSET IN THE SUPPORT)
				if(collision)
				{
					//IF WORKS EXCHANGE IT INTO EPA Collision resolution
					objA.debugC = glm::vec4(1, 0, 1, 1); 
					objB.debugC = glm::vec4(1, 0, 1, 1);

					//TODO: SEEMS THERE ARE SOME ISSUE OF DETECTING PENETRATIONDEPTH AND COLLLISION NORMAL
					// CURRENTLY IT ONLY DETECTS IF THE OBJECT HAS PENETRATED THE OTHER ALMOST 80% FOR EPA TO MET THE REQUIREMENT
					//EPA 
					glm::vec3 collisionNormal = glm::vec3(0);
					float penetrationDepth = 0;
					EPAV2(objA,objB,simplexPs, collisionNormal, penetrationDepth);
					//EPA(objA, objB, simplexPs,collisionNormal, penetrationDepth);

					//std::cout << "EPA PenetrationDepth: " << penetrationDepth << "\n";
					//std::cout << "EPA Collision Normal x: " << collisionNormal.x << "\n";
					//std::cout << "EPA Collision Normal y: " << collisionNormal.y << "\n";
					//std::cout << "EPA Collision Normal z: " << collisionNormal.z << "\n";

					//Apply collision response using the collision normal and penetrationDepth
					//Collision response logic
					//ApplyImpulse(objA, objB, collisionNormal, penetrationDepth);

				}
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


		bool GJK(const GameObject& objA, const GameObject& objB, Simplex& simplex)
		{
			glm::vec3 direction(1, 0, 0); // arbitrary
			glm::vec3 support = Support(objA, objB, direction);
			simplex.push_front(support);

			//if the first simplex passsed the origin, return no collision
			if (glm::dot(support, direction) <= 0) 
				return false;
			direction = -support; //new direction towards origin

			//Main loop: simplex construction
			while(true)
			{
				// Get the new support point  in the current direction
				support = Support(objA, objB, glm::normalize(direction)); // get another point in current direction
				
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

		void EPAV2(const GameObject& objA, const GameObject& objB,const Simplex& simplex, glm::vec3& collisionNormal, float& penetrationDepth )
		{
			PolytopeData polytopeData = InitalizePolytopeV2(simplex);
			int iterationCount = 50; //prevent from infinite loop
			// WHILE LOOP 
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

			glm::vec3 closestFaceNormal = polytopeData.face[closestFaceIndex].normal;
			float closestFaceDistance = polytopeData.face[closestFaceIndex].distance;
			VisualizeClosestFace(polytopeData, closestFaceIndex);
			// -----------------------------------------------------------------------------
			// --------- FIND THE NEW SUPPORT POINT -----------------------------
			// Calculate the support point along the closest face normal
			glm::vec3 supportPoint = Support(objA, objB, closestFaceNormal);
			float newDistance = glm::dot(closestFaceNormal,supportPoint);
			if(newDistance - closestFaceDistance < 1e-6f)
			{
				std::cout << "SUCCESSFUL EPA RETURN \n";
				collisionNormal = closestFaceNormal;
				penetrationDepth = closestFaceDistance;
				return;
			}
			// ------------------------------------------------------------------

#pragma region BRUTE FORCE VERSION
			//WHile loop (EPA operation)
			// FINDING THE CLOSEST FACE TO THE ORIGIN
			// COMPUTE A NEW SUPPORT POINT AND ADD IT TO THE POLYTOPEDATA POLYTOPE FOR NEW POINT TO CONSTRUCT NEW FACE IN THE EXPANDING POLYTOPE PROCESS
			// CHECK IF THE NEW SUPPORT POINT LIES BEYOND THE CLOSEST FACE (WITHIN A TOLERANCE 1E-6F)

			//FIRST Build a polytope out of the simplex from the gjk
			//std::vector<glm::vec3> polytope(simplex.begin(), simplex.end()); //EXPAND THIS //remember the order is push front
			////INITALIZE THE POLYTOPE BASED ON THE FINAL SIMPLEX FROM GJK
			//std::vector<std::array<int,3>> faces =
			//{
			//	{0,1,2},
			//	{0,3,1},
			//	{0,2,3},
			//	{1,3,2}
			//};

			//// Store the normals for each face and the distance to the origin (used for penetration depth)
			//std::vector<glm::vec3> faceNormals; 
			//std::vector<float> faceDistances; // Dot product of the normal with a vertex to determine penetration

			////Compute the normal and distance for each face
			//for(const auto& face : faces)
			//{
			//	glm::vec3 A = polytope[face[0]]; //First vertex of the face
			//	glm::vec3 B = polytope[face[1]]; //second vertex of the face
			//	glm::vec3 C = polytope[face[2]]; //third vertex of the face

			//	//Compute the normal of the face
			//	glm::vec3 normal = glm::normalize(glm::cross(B - A, C - A));
			//	// ensure the normal is facing outward by checking if its pointing away from the origin
			//	if (glm::dot(normal, A) < 0) normal = -normal; //reverse the normal direction if it faces inward

			//	//visualize the edges of the current polytope
			//	Debug::DrawLine(A, B , glm::vec4(0, 1, 1, 1), 3);
			//	Debug::DrawLine(B, C, glm::vec4(0, 1, 1, 1), 3);
			//	Debug::DrawLine(C, A, glm::vec4(0, 1, 1, 1), 3);

			//	//Draw the face normal
			//	glm::vec3 centroid = (A + B + C) / 3.0f;
			//	Debug::DrawLine(centroid, centroid + normal, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), 3); //orange for the normal


			//	faceNormals.push_back(normal);
			//	float distance = glm::dot(normal, A); // distance from the origin to the face along the normal
			//	faceDistances.push_back(distance);
			//}

			//// Find the closest face to the origin
			//int closestFaceIndex = 0;
			//float minDistance = faceDistances[0];

			//for (int i = 1; i < faceDistances.size(); i++)
			//{
			//	if (faceDistances[i] < minDistance)
			//	{
			//		minDistance = faceDistances[i];
			//		closestFaceIndex = i;
			//	}
			//}

			////visualize the closest face and its normal
			//{
			//	const auto& face = faces[closestFaceIndex];
			//	glm::vec3 A = polytope[face[0]]; //First vertex of the face
			//	glm::vec3 B = polytope[face[1]]; //second vertex of the face
			//	glm::vec3 C = polytope[face[2]]; //third vertex of the face

			//	glm::vec3 normal = faceNormals[closestFaceIndex];
			//	// Highlight the edges of the closest face in green
			//	Debug::DrawLine(A, B, glm::vec4(0, 1, 0, 1), 3); // Green for edges
			//	Debug::DrawLine(B, C, glm::vec4(0, 1, 0, 1), 3); // Green for edges
			//	Debug::DrawLine(C, A, glm::vec4(0, 1, 0, 1), 3); // Green for edges

			//	// Highlight the normal of the closest face in green
			//	glm::vec3 centroid = (A + B + C) / 3.0f; // Calculate the centroid of the face
			//	Debug::DrawLine(centroid, centroid + normal, glm::vec4(0, 1, 0, 1), 3); // Green for the normal

			//	//Visualize the origin point of the closest polytope to the centroid
			//	glm::vec3 origin(0.0f);
			//	Debug::DrawLine(origin, normal, glm::vec4(1, 0, 0, 1), 3);
			//}
#pragma endregion
		}

		//void EPA(const GameObject* objA, const GameObject* objB, std::vector<glm::vec3>& simplex, glm::vec3& collisionNormal, float& penetrationDepth)
		//{
		//	//TODO: REDO THE EPA FUNCTION FROM START
		//	//TODO: COMPUTING NORMAL OF THE TRIANGLE FACE of those simplex point (there are some normals from initializePolytope)
		//	// CHECK WHEN FINDING THE CLOSEST FACE 
		//	//EDGE CONSTRUCTION WITH THE POLYTOPE FOR DETERMINE THE FINAL POLYTOPE FIGURE COVERING THE INTERSECTION

		//	// Build the inital polytope from gjk simplex point
		//	std::vector<Face> polytope = InitalizePolytope(simplex); 
		//	int maxIteration = 50;
		//	while(maxIteration--)
		//	{
		//		Face closestFace = findClosestFace(polytope);
		//		// Get a new point in the direction of the closest face normal
		//		glm::vec3 newPoint = Support(objA, objB, closestFace.normal); //Potential error where the support function getting a point to far away
		//		// Check if the new point is far enough from the origin
		//		float distanceToOrigin = glm::dot( newPoint, closestFace.normal);

		//		if (distanceToOrigin - closestFace.distance < EPA_TOLERANCE) 
		//		{
		//			collisionNormal = closestFace.normal;
		//			penetrationDepth = closestFace.distance;
		//			break;
		//		}
		//		
		//		ExpandPolytope(polytope, simplex, newPoint);
		//	}
		//	std::cout << polytope.size() << "\n";
		//}

		void ApplyImpulse( GameObject* objA,  GameObject* objB, const glm::vec3& collisionNormal, const float penetrationDepth)
		{
			//FIRST APPLY ONLY TO velocity
			glm::vec3 relativeVelocity = objB->velocity - objA->velocity;
			float velocityAlongNormal = glm::dot(relativeVelocity, collisionNormal);
			if (velocityAlongNormal > 0) return; //Do not resolve collision if object are moving away

			float invMassA = (objA->mass > 0) ? 1.0f / objA->mass : 0.0f;
			float invMassB = (objB->mass > 0) ? 1.0f / objB->mass : 0.0f;

			float impulseScalar = -(1.0f) * velocityAlongNormal;
			impulseScalar /= invMassA + invMassB;

			glm::vec3 impulseVec = impulseScalar * collisionNormal;
			if (invMassA > 0.0f)
				objA->velocity -= impulseVec * invMassA; //Apply opposite impuls to object
			if (invMassB > 0.0f)
				objB->velocity -= impulseVec * invMassB; //Apply opposite impuls to object

			//UPDATE LATER TO EFFECT ANGULAR VELOCITY 

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