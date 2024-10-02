#pragma once
//#include "config.h"

//#define EPSILON 1e-8

#pragma region RAY


inline bool RayIntersectAABB(const Ray& ray, const AABB& aabb)
{
	glm::vec3 rayDir = ray.direction;

	// Normalize the ray direction if necessary
	if (glm::length(rayDir) != 1.0f)
		rayDir = glm::normalize(ray.direction);

	// Compute tmin and tmax for each component (x, y, z)
	glm::vec3 tmin = (aabb.min - ray.origin) / rayDir;
	glm::vec3 tmax = (aabb.max - ray.origin) / rayDir;

	// Ensure tmin < tmax for each component by swapping them if needed
	glm::vec3 t1 = glm::min(tmin, tmax);
	glm::vec3 t2 = glm::max(tmin, tmax);

	// Calculate the near and far intersection distances
	float tNear = std::max(std::max(t1.x, t1.y), t1.z);  // The furthest entry point
	float tFar = std::min(std::min(t2.x, t2.y), t2.z);    // The closest exit point

	// If tNear is less than tFar and tFar is positive, there is an intersection
	return tNear <= tFar && tFar >= 0;
}

//inline bool RayIntersectAABB(const Ray& ray, const AABB& aabb)
//{
//	glm::vec3 rayDir = ray.direction;
//
//	if (glm::length(rayDir) != 1.0f)
//		rayDir = glm::normalize(ray.direction);
//
//	//brute force implementation
//	float tmin = (aabb.min.x - ray.origin.x) / rayDir.x;
//	float tmax = (aabb.max.x - ray.origin.x) / rayDir.x;
//	if (tmin > tmax) std::swap(tmin, tmax);
//	float tymin = (aabb.min.y - ray.origin.y) / rayDir.y;
//	float tymax = (aabb.max.y - ray.origin.y) / rayDir.y;
//	if (tymin > tymax) std::swap(tymin, tymax);
//	if ((tmin > tymax) || (tymin > tmax))
//		return false;
//
//	if (tymin > tmin)
//		tmin = tymin;
//
//	if (tymax < tmax)
//		tmax = tymax;
//
//	float tzmin = (aabb.min.z - ray.origin.z) / rayDir.z;
//	float tzmax = (aabb.max.z - ray.origin.z) / rayDir.z;
//	if (tzmin > tzmax) std::swap(tzmin, tzmax);
//	if ((tmin > tzmax) || (tzmin > tmax)) return false;
//
//	// Check if the ray starts inside the AABB
//	if (ray.origin.x >= aabb.min.x && ray.origin.x <= aabb.max.x &&
//		ray.origin.y >= aabb.min.y && ray.origin.y <= aabb.max.y &&
//		ray.origin.z >= aabb.min.z && ray.origin.z <= aabb.max.z)
//	{
//		return true; // Ray starts inside the AABB, so it's a hit
//	}
//
//	return true;
//}

// EXHANGE THE LOGIC INSIDE OBJ MESH INTERSECTION OF TRIANGLE INTERSECTION AND BUT IT INSIDE HERE LATER
//inline bool RayTriangleIntersection(const Ray& ray, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t, glm::vec3& hitPoint)
//{
    //WORKING CODE INSIDE THE MESH INTERSECTION
    
//}
#pragma endregion


#pragma region GJK ( SIMPLEX + SUPPORT)

// HELPER FUNCTIONS
inline glm::vec3 GetFurthestPointInDirection(const GameObject* obj, const glm::vec3& direction) 
{
    glm::vec3 furthestPoint = obj->boundingbox.min; // Start with a arbitrary point
    float maxDotProduct = glm::dot(furthestPoint, direction);

    //Loop through each triangle group and vertices to find the furthest point along the direction
    for(const auto& triangle : obj->GetTriangles())
    {
        for(const auto& vertex : triangle.vertices)
        {
            float dotProduct = glm::dot(vertex, direction);
            if(dotProduct > maxDotProduct)
            {
                maxDotProduct = dotProduct;
                furthestPoint = vertex;
            }
        }
    }

    return furthestPoint;
}

inline glm::vec3 Support(const GameObject* objA, const GameObject* objB, const glm::vec3& direction)
{
    const float MAX_DISTANCE_THRESHOLD = 10.0f; // Adjust this based on your object scale
    glm::vec3 normalizedDir = glm::normalize(direction);
    glm::vec3 furthestPointA = GetFurthestPointInDirection(objA, normalizedDir);
    glm::vec3 furthestPointB = GetFurthestPointInDirection(objB, -normalizedDir);
    glm::vec3 newPoint = furthestPointA - furthestPointB; //Within Minkowski sum
    // Clamp the distance to prevent the new point from being too far away
    float distance = glm::length(newPoint);
    if (distance > MAX_DISTANCE_THRESHOLD) {
        newPoint = glm::normalize(newPoint);
    }

    return  newPoint;
}

inline bool LineCase(std::vector<glm::vec3>& simplex, glm::vec3& direction)
{
    glm::vec3 A = simplex[1]; // Last added point to the simplex (most recent point)
    glm::vec3 B = simplex[0]; // Second point in the line simplex

    glm::vec3 AB = B - A; // Vector from A to B (forming the edge of the line)
    glm::vec3 A0 = -A; // Vector from A to the origin (find if origin is in this direction)

    // if the origin is in the direction of the AB, move towards it
    if (glm::dot(AB, A0) > 0) 
    { 
        // Project the search direction towards the origin, in the direction of AB
        direction = glm::cross(glm::cross(AB, A0), A0); 
    }
    else 
    {
        //otherwise, the simplex is just the point A
        simplex = { A };
        direction = A0; // Update the direction to point from A to the origin (AO)

    }
    return false;
}

inline bool TriangleCase(std::vector<glm::vec3>& simplex, glm::vec3& direction) 
{
    glm::vec3 A = simplex[2];
    glm::vec3 B = simplex[1];
    glm::vec3 C = simplex[0];

    glm::vec3 AB = B - A;
    glm::vec3 AC = C - A;
    glm::vec3 A0 = -A;

    //Compute the normal of the triangle
    glm::vec3 ABC_normal = glm::cross(AB, AC);
    //Determine which side of the triangle the origin is on
    glm::vec3 ABC_normal_toward_origin = glm::cross(ABC_normal, AC);

    //Check if the origin is on the AC side of the triangle
    if(glm::dot(ABC_normal_toward_origin,A0) > 0)
    {
        //Origin is outside AC
        simplex = { C,A };
        direction = glm::cross(glm::cross(AC, A0), AC); //New direction is perpendicular to AC
    }
    else
    {
        //Check if the origin is on the AB side of the triangle
        glm::vec3 ABC_normal_other_side = glm::cross(AB, ABC_normal);
        if(glm::dot(ABC_normal_other_side,A0) > 0)
        {
            //Origin is outside AB
            simplex = { B,A };
            direction = glm::cross(glm::cross(AB, A0), AB);
        }
        else
        {
            //Origin is above or below triangle, check which side the triangle's plane faces
            if(glm::dot(ABC_normal,A0) > 0)
            {
                //Origin is above, keep all three point
                direction = ABC_normal;
            }
            else
            {
                //Origin is below the triangle (flips the triangle winding order)
                simplex = { B,A,C };
                direction = -ABC_normal;
            }
        }
    }

    return false;
}

inline bool TetrahedronCase(std::vector<glm::vec3>& simplex, glm::vec3& direction) 
{
    glm::vec3 A = simplex[3];  //Last added point
    glm::vec3 B = simplex[2];
    glm::vec3 C = simplex[1];
    glm::vec3 D = simplex[0];

    glm::vec3 AB = B - A;
    glm::vec3 AC = C - A;
    glm::vec3 AD = D - A;
    glm::vec3 AO = -A;

    //Compute normals for each face of the tetrahedron
    glm::vec3 ABC_normal = glm::cross(AB, AC);  //Triangle ABC
    glm::vec3 ACD_normal = glm::cross(AC, AD);  //Triangle ACD
    glm::vec3 ABD_normal = glm::cross(AD, AB);  //Triangle ABD

    //Check if the origin is outside the ABC face
    if (glm::dot(ABC_normal, AO) > 0) {
        //Origin is outside ABC, handle it like a triangle case
        simplex = { C, B, A };
        direction = ABC_normal;
        return false;
    }

    //Check if the origin is outside the ACD face
    if (glm::dot(ACD_normal, AO) > 0) {
        //Origin is outside ACD, handle it like a triangle case
        simplex = { D, C, A };
        direction = ACD_normal;
        return false;
    }

    //Check if the origin is outside the ABD face
    if (glm::dot(ABD_normal, AO) > 0) {
        //Origin is outside ABD, handle it like a triangle case
        simplex = { D, B, A };
        direction = ABD_normal;
        return false;
    }

    //If the origin is not outside any face, it must be inside the tetrahedron
    return true;  // Collision detected (origin inside the tetrahedron)
}

inline bool HandleSimplex(std::vector<glm::vec3>& simplex, glm::vec3& direction)
{
    switch (simplex.size())
    {
    case 2: return LineCase(simplex, direction);
    case 3: return TriangleCase(simplex, direction);
    case 4: return TetrahedronCase(simplex, direction);
    }
    return false;
}

#pragma endregion

inline glm::vec3 ComputeNormal(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) 
{
    glm::vec3 AB = b - a;
    glm::vec3 AC = c - a;

    glm::vec3 normal = glm::cross(AB, AC);  // Calculate cross product
    float length = glm::length(normal);     // Magnitude of the cross product

    // Check for near-zero length (degenerate case)
    if (length < 1e-6f)
    {
        return glm::vec3(0, 0, 0);
    }

    // Handle precision issues by clamping very small values to zero
    glm::vec3 normalizedNormal = glm::normalize(normal);

    if (std::abs(normalizedNormal.x) < 1e-6f) normalizedNormal.x = 0.0f;
    if (std::abs(normalizedNormal.y) < 1e-6f) normalizedNormal.y = 0.0f;
    if (std::abs(normalizedNormal.z) < 1e-6f) normalizedNormal.z = 0.0f;

    return normalizedNormal;
}

inline Face findClosestFace(const std::vector<Face>& polytope)
{
    Face closestFace;
    float minDistance = FLT_MAX;
    // Loop through all faces and find the closest face to the origin
    for(const Face& face : polytope)
    {
        if(face.distance < minDistance)
        {
            closestFace = face;
            minDistance = face.distance;
            closestFace.distance = face.distance;
        }
    }
    return closestFace;
}

inline void ExpandPolytope(std::vector<Face>& polytope, const std::vector<glm::vec3>& simplex,const glm::vec3& newPoint)
{
    int newPointIndex = simplex.size();
    //simplex.push_back(newPoint);
    std::vector<glm::vec3> tempSimplex = simplex;
    tempSimplex.push_back(newPoint);

    std::vector<Face> newFaces;
    // Iterate through all the faces and determine which ones are facing the new point
    std::vector<int> faceToRemove;
    for(int i = 0; i < polytope.size(); i++)
    {
        Face& face = polytope[i];
        // Check if the face is visible from the new point
        glm::vec3 faceToNewPoint = newPoint - tempSimplex[face.polytopeIndices[0]]; // Vector from face to the new point
        if(glm::dot(face.normal,faceToNewPoint) > 0) // If the face normal points toward the new point
        {
            faceToRemove.push_back(i); // Mark the current face for removal

            // Create new faces by connecting the new point to the edge of the old face 
            for(int j = 0; j < 3; j++)
            {
                // Create new face using one edge of the old face and the new point
                std::array<int, 3> newIndices =
                {
                    face.polytopeIndices[j],
                    face.polytopeIndices[(j + 1) % 3],
                    newPointIndex
                };

                // Compute normal for the new face
                glm::vec3 normal = ComputeNormal(tempSimplex[newIndices[0]], tempSimplex[newIndices[1]], tempSimplex[newIndices[2]]);
                // Skip degenerate faces
                if (glm::length(normal) < 1e-6f) {
                    continue;
                }
                float distance = glm::dot(normal, tempSimplex[newIndices[0]]);

                newFaces.push_back(Face{ normal, distance,newIndices });
            }
        }
    }

    //Remove the old faces that are visible from the new point
    for (int i = faceToRemove.size() - 1; i >= 0; i--)
        polytope.erase(polytope.begin() + faceToRemove[i]);

    polytope.insert(polytope.end(), newFaces.begin(), newFaces.end());
}

#pragma region EPA
inline std::vector<Face> InitalizePolytope(const std::vector<glm::vec3> simplex)
{
   std::vector<Face> polytope;
   // Create faces for the tetrahedron (4 faces)
   std::vector<std::array<int, 3>> indices = {
       {0, 1, 2}, {0, 1, 3}, {0, 2, 3}, {1, 2, 3}
   };

   for(const auto& indexSet : indices)
   {
       glm::vec3 normal = ComputeNormal(simplex[indexSet[0]], simplex[indexSet[1]], simplex[indexSet[2]]);
       //only add non degenerate faces
       if(glm::length(normal) > 1e-6f)
       {
           //PRECISION FIX FOR DISTANCE, IF DEGENERATE RESULT SET IT TO 0
           float distance = glm::dot(normal, simplex[indexSet[0]]); // Calculate distance to origin
           polytope.push_back({ normal,distance,{indexSet[0],indexSet[1],indexSet[2]} });
       }

   }
   return polytope;
}

#pragma endregion
