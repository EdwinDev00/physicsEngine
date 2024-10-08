#pragma once
#include <map>
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
inline glm::vec3 GetFurthestPointInDirection(const GameObject& obj, const glm::vec3& direction) 
{
    glm::vec3 furthestPoint = glm::vec3();
    float maxDistance = -FLT_MAX;
    for(const Triangles& triangle : obj.GetTriangles())
        for(const glm::vec3& vertex : triangle.vertices)
        {
            float distance = glm::dot(vertex, direction);
            if(distance > maxDistance)
            {
                maxDistance = distance;
                furthestPoint = vertex;
            }
        }
    return furthestPoint;
}

inline glm::vec3 Support(const GameObject& objA, const GameObject& objB,  glm::vec3& direction)
{
        direction = glm::normalize(direction);
        glm::mat3 modelRotationA = glm::transpose(objA.GetRotationMat());
        glm::mat3 modelRotationB = glm::transpose(objB.GetRotationMat());

        // Get the furthest point on objA in the given direction
        glm::vec3 furthestPointA = GetFurthestPointInDirection(objA, direction * modelRotationA) * objA.GetScale() + objA.GetPosition();
   
        // Get the furthest point on objB in the opposite direction
        glm::vec3 furthestPointB = GetFurthestPointInDirection(objB, -direction * modelRotationB) * objB.GetScale() + objB.GetPosition();

        // Minkowski difference: objA - objB
        glm::vec3 minkowskiDifference = furthestPointA - furthestPointB;
        
        // Check for near-zero components in the Minkowski difference and adjust if necessary
        if (glm::length(minkowskiDifference) < 1e-6f) {
            minkowskiDifference += direction * 1e-6f; // Apply small offset in the direction to avoid degenerate case
        }

        return minkowskiDifference; // Return the point on the Minkowski difference
}

inline bool LineCase(Simplex& simplex, glm::vec3& direction)
{
    glm::vec3 A = simplex[0]; // Last added point to the simplex (most recent point)
    glm::vec3 B = simplex[1]; // Second point in the line simplex

    glm::vec3 AB = B - A; // Vector from A to B (forming the edge of the line)
    glm::vec3 A0 = -A; // Vector from A to the origin (find if origin is in this direction)

    // if the origin is in the direction of the AB, move towards it
    if (glm::dot(AB, A0) > 0) 
    { 
        // Project the search direction towards the origin, in the direction of AB
        glm::vec3 crossProd = glm::cross(AB, A0);
        if (glm::length(crossProd) < 1e-6f) {
            // Handle case where the cross product is nearly zero
            direction = A0;
        }
        else {
            direction = glm::cross(crossProd, AB);
        }
    }
    else 
    {
        //otherwise, the simplex is just the point A
        simplex = { A };
        direction = A0; // Update the direction to point from A to the origin (AO)

    }
    return false;
}

inline bool TriangleCase(Simplex& simplex, glm::vec3& direction) 
{
    glm::vec3 A = simplex[0];
    glm::vec3 B = simplex[1];
    glm::vec3 C = simplex[2];

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

inline bool TetrahedronCase(Simplex& simplex, glm::vec3& direction) 
{
    glm::vec3 A = simplex[0];  //Last added point
    glm::vec3 B = simplex[1];
    glm::vec3 C = simplex[2];
    glm::vec3 D = simplex[3];

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

inline bool HandleSimplex(Simplex& simplex, glm::vec3& direction)
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

#pragma region V2 EPA
inline Face CreateFace(int v1,int v2,int v3, const std::vector<glm::vec3>& polytope)
{
    Face newFace;
    newFace.polytopeIndices = { v1,v2,v3 };
    glm::vec3 A = polytope[v1];
    glm::vec3 B = polytope[v2];
    glm::vec3 C = polytope[v3];
    newFace.normal = glm::normalize(glm::cross(B - A, C - A));
    if (glm::dot(newFace.normal, A) < 0) newFace.normal = -newFace.normal;
    newFace.distance = glm::dot(newFace.normal, A);
    return newFace;
}

inline const PolytopeData InitalizePolytopeV2(const Simplex& simplex) 
{
    //std::vector<glm::vec3> polytope(simplex.begin(), simplex.end()); //Some reason the size does not match with the amount of points
    std::vector<glm::vec3> polytope;
    for (const glm::vec3& points : simplex.getPoints())
        polytope.push_back(points);

    // Predefined tetrahedron faces
    std::vector<std::array<int, 3>> indices =
    {
        {0,1,2},
        {0,3,1},
        {0,2,3},
        {1,3,2}
    };

    std::vector<Face> faces;
    for(const auto& faceIndexSet :indices)
    {
        faces.push_back(CreateFace(faceIndexSet[0], faceIndexSet[1], faceIndexSet[2], polytope));
    }

    return { polytope,faces };
}


//EXPANDING LOGIC
inline void ExpandPolytope(PolytopeData& data, const int& closestFaceIndex, const glm::vec3& supportPoint)
{
    const auto& closestFace = data.face[closestFaceIndex];
    glm::vec3 A = data.polytope[closestFace.polytopeIndices[0]];
    glm::vec3 B = data.polytope[closestFace.polytopeIndices[1]];
    glm::vec3 C = data.polytope[closestFace.polytopeIndices[2]];

    //Add the new vertex to the polytope
    int newVertexIndex = data.polytope.size();
    data.polytope.push_back(supportPoint);

    //Remove the closest face (replace it by the new face)
    data.face.erase(data.face.begin() + closestFaceIndex);

    //Create three new faces connecting the support point to the edges of the old face
    Face newFace1 = CreateFace(closestFace.polytopeIndices[0], closestFace.polytopeIndices[1], newVertexIndex, data.polytope);
    Face newFace2 = CreateFace(closestFace.polytopeIndices[1], closestFace.polytopeIndices[2], newVertexIndex, data.polytope);
    Face newFace3 = CreateFace(closestFace.polytopeIndices[2], closestFace.polytopeIndices[0], newVertexIndex, data.polytope);

    data.face.push_back(newFace1);
    data.face.push_back(newFace2);
    data.face.push_back(newFace3);

    //EPA Edge method
                // Collect all the boundary edges
    std::map<std::pair<int, int>, int> edgeCount; // keep track of the edge occurence
    for (const Face& face : data.face)
    {
        //Loop through all the edges of the current face
        for (int i = 0; i < 3; i++)
        {
            int v1 = face.polytopeIndices[i];
            int v2 = face.polytopeIndices[(i + 1) % 3];

            //Store the edge in order (smallest first)
            std::pair<int, int> edge = (v1 < v2) ? std::make_pair(v1, v2) : std::make_pair(v2, v1);
            edgeCount[edge]++;
        }
    }

    //Find the unique edge
    std::vector<std::pair<int, int>> boundaryEdge;
    for (const auto& entry : edgeCount)
    {
        // This is a boundary edge because it only belongs to one face
        if (entry.second == 1) boundaryEdge.push_back(entry.first);
    }

    std::vector<Face> newFace;
    for (const auto& edge : boundaryEdge)
    {
        int v1 = edge.first;
        int v2 = edge.second;
        newFace.push_back(CreateFace(v1, v2, newVertexIndex, data.polytope));
    }
    for (const Face& face : newFace)
        data.face.push_back(face);
}

//VISUALIZECLOSEST FACE
inline void VisualizePolytopeWithClosestFace(const PolytopeData& data, const int closestFaceIndex)
{
    //Visualize the whole polytope
    for (const Face& face : data.face)
    {
        glm::vec3 A = data.polytope[face.polytopeIndices[0]];
        glm::vec3 B = data.polytope[face.polytopeIndices[1]];
        glm::vec3 C = data.polytope[face.polytopeIndices[2]];

        // Draw the edges of the new face
        Debug::DrawLine(A, B, glm::vec4(0, 1, 1, 1), 3); // Cyan for the edges
        Debug::DrawLine(B, C, glm::vec4(0, 1, 1, 1), 3);
        Debug::DrawLine(C, A, glm::vec4(0, 1, 1, 1), 3);
    }

    //visualize the closest face and its normal
    const auto& closestFace = data.face[closestFaceIndex];
    const auto& faceIndices = closestFace.polytopeIndices;
    glm::vec3 A = data.polytope[faceIndices[0]]; //First vertex of the face
    glm::vec3 B = data.polytope[faceIndices[1]]; //second vertex of the face
    glm::vec3 C = data.polytope[faceIndices[2]]; //third vertex of the face
    glm::vec3 normal = data.face[closestFaceIndex].normal;

    // Highlight the edges of the closest face in green
    Debug::DrawLine(A, B, glm::vec4(0, 1, 0, 1), 3); // Green for edges
    Debug::DrawLine(B, C, glm::vec4(0, 1, 0, 1), 3); // Green for edges
    Debug::DrawLine(C, A, glm::vec4(0, 1, 0, 1), 3); // Green for edges

    // Highlight the normal of the closest face in green
    glm::vec3 centroid = (A + B + C) / 3.0f; // Calculate the centroid of the face
    Debug::DrawLine(centroid, centroid + normal, glm::vec4(0, 1, 0, 1), 10); // Green for the normal
}

#pragma endregion
