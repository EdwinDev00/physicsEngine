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

    //glm::vec3 furthestPoint = obj->boundingbox.GetExtents();

    //float maxDotProduct = glm::dot(furthestPoint, direction);

    ////Loop through each triangle group and vertices to find the furthest point along the direction
    //for(const auto& triangle : obj->GetTriangles())
    //{
    //    for(const auto& vertex : triangle.vertices)
    //    {
    //        float dotProduct = glm::dot(vertex, direction);
    //        if(dotProduct > maxDotProduct)
    //        {
    //            maxDotProduct = dotProduct;
    //            furthestPoint = vertex;
    //        }
    //    }
    //}

    //return furthestPoint;
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

    Debug::DrawLine(A, B, glm::vec4(1, 0, 0, 1), 4.0f); // first to second
    Debug::DrawLine(A, C, glm::vec4(0, 1, 0, 1), 4.0f); // first to third
    Debug::DrawLine(A, D, glm::vec4(0, 0, 1, 1), 4.0f); // first to forth
    Debug::DrawLine(B, C, glm::vec4(1, 1, 0, 1), 4.0f); // second to third
    Debug::DrawLine(C, D, glm::vec4(1, 0, 1, 1), 4.0f); // third to forth
    Debug::DrawLine(D, B, glm::vec4(1, 1, 1, 1), 4.0f); // forth to second


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
            //closestFace.distance = face.distance;
            //closestFace.normal = face.normal;
        }
    }
    return closestFace;
}

//// Helper function to check and add unique edges
//bool IfAddUniqueEdge(std::vector<std::pair<int, int>>& edges, int index1, int index2)
//{
//    for (auto it = edges.begin(); it != edges.end(); ++it) {
//        // Check if the edge exists in reverse order, indicating it's shared and not unique
//        if ((it->first == index2 && it->second == index1)) {
//            edges.erase(it); // Remove the shared edge, it's not unique
//            return false;
//        }
//    }
//    return true; // The edge is unique
//}

inline void ExpandPolytope(std::vector<Face>& polytope, std::vector<glm::vec3>& simplex,const glm::vec3& newPoint)
{
    ////NEED TO TEST THIS UNIQUE EDGE FUNCTION 

    //int newPointIndex = simplex.size();
    //simplex.push_back(newPoint);

    //std::vector<Face> newFaces;
    //std::vector<int> faceToRemove;

    //// keep track of pair indices for representing the edge of the polytope
    //std::vector<std::pair<int, int>> uniqueEdges;

    //for(int i = 0; i < polytope.size(); i++)
    //{
    //    Face& face = polytope[i];
    //    glm::vec3 faceToNewPoint = newPoint - simplex[face.polytopeIndices[0]]; // Vector from face to the new point
    //    if(glm::dot(face.normal,faceToNewPoint) > 0) // face is visible from the new point
    //    {
    //        faceToRemove.push_back(i);

    //        //Collect the unique edges from this face
    //        for(int j = 0; j < 3; j++)
    //        {
    //            int index1 = face.polytopeIndices[j];
    //            int index2 = face.polytopeIndices[(j + 1) % 3];

    //            //Add unique edge (order matters, so check both direction)
    //            if(!IfAddUniqueEdge(uniqueEdges,index1,index2))
    //            {
    //                uniqueEdges.push_back({ index1,index2 });
    //            }

    //        }
    //    }
    //}

    ////remove the visible face
    //for (int i = faceToRemove.size() - 1; i >= 0; i--) polytope.erase(polytope.begin() + faceToRemove[i]);

    ////Create the new face from the unique edges and the new point
    //for(const auto& edge : uniqueEdges)
    //{
    //    std::array<int, 3> newIndices = { edge.first, edge.second, newPointIndex };
    //    glm::vec3 a = simplex[newIndices[0]];
    //    glm::vec3 b = simplex[newIndices[1]];
    //    glm::vec3 c = simplex[newIndices[2]];
    //    glm::vec3 normal = glm::normalize(glm::cross(b-a, c-a));
    //    if (glm::length(normal) < 1e-6f) continue;

    //    float distance = glm::dot(normal, simplex[newIndices[0]]);
    //    if (glm::dot(normal, simplex[newIndices[0]]) > 0)
    //    {
    //        normal = -normal;
    //        distance = -distance;
    //    }

    //    newFaces.push_back(Face{ normal,distance, newIndices });
    //}
    //polytope.insert(polytope.end(), newFaces.begin(), newFaces.end());

    //int newPointIndex = simplex.size();
    //simplex.push_back(newPoint);
 
    //std::vector<Face> newFaces;
    //// Iterate through all the faces and determine which ones are facing the new point
    //std::vector<int> faceToRemove;
    //for(int i = 0; i < polytope.size(); i++)
    //{
    //    Face& face = polytope[i];
    //    // Check if the face is visible from the new point
    //    glm::vec3 faceToNewPoint = newPoint - simplex[face.polytopeIndices[0]]; // Vector from face to the new point
    //    if(glm::dot(face.normal,faceToNewPoint) > 0) // If the face normal points toward the new point
    //    {
    //        faceToRemove.push_back(i); // Mark the current face for removal

    //        // Create new faces by connecting the new point to the edge of the old face 
    //        for(int j = 0; j < 3; j++)
    //        {
    //            // Create new face using one edge of the old face and the new point
    //            std::array<int, 3> newIndices =
    //            {
    //                face.polytopeIndices[j],
    //                face.polytopeIndices[(j + 1) % 3],
    //                newPointIndex
    //            };

    //            // Compute normal for the new face
    //            glm::vec3 normal = ComputeNormal(simplex[newIndices[0]], simplex[newIndices[1]], simplex[newIndices[2]]);
    //            // Skip degenerate faces
    //            if (glm::length(normal) < 1e-6f) {
    //                continue;
    //            }
    //            // Ensure the normal is outward facing
    //           if (glm::dot(normal, simplex[newIndices[0]]) > 0)
    //           {
    //                normal = -normal;
    //           }

    //            float distance = glm::dot(normal, simplex[newIndices[0]]);

    //            newFaces.push_back(Face{ normal, distance,newIndices });
    //        }
    //    }
    //}

    ////Remove the old faces that are visible from the new point
    //for (int i = faceToRemove.size() - 1; i >= 0; i--)
    //    polytope.erase(polytope.begin() + faceToRemove[i]);

    //polytope.insert(polytope.end(), newFaces.begin(), newFaces.end());
}

#pragma region EPA
//inline std::vector<Face> InitalizePolytope(const std::vector<glm::vec3>& simplex)
//{
//   std::vector<Face> polytope;
//   // Create faces for the tetrahedron (4 faces)
//   std::vector<std::array<int, 3>> indices = {
//       {0, 1, 2}, {0, 1, 3}, {0, 2, 3}, {1, 2, 3}
//   };
//   
//   for(const auto& indexSet : indices)
//   {
//       glm::vec3 a = simplex[indexSet[0]];
//       glm::vec3 b = simplex[indexSet[1]];
//       glm::vec3 c = simplex[indexSet[2]];
//
//       float ABx = b.x - a.x; 
//       float ABy = b.y - a.y; 
//       float ABz = b.z - a.z;
//       float ACx = c.x - a.x;
//       float ACy = c.y - a.y;
//       float ACz = c.z - a.z;
//       glm::vec3 AB = glm::vec3(ABx, ABy, ABz);
//       glm::vec3 AC = glm::vec3(ACx, ACy, ACz);
//
//       // Compute the normal of the triangle face
//       glm::vec3 normal = glm::normalize(glm::cross(AB, AC));
//       if (glm::length(normal) <= 0) continue;
//       // Compute the signed distance to the origin
//       float distance = glm::dot(normal, a);
//
//       // Ensure the normal is pointing outward by checking the distance
//       if(distance < 0.0f)
//       {
//           normal = -normal; // Flip the normal
//           distance = -distance; // Make the distance positiv;
//       }
//
//       // CHECK For degenerate triangle by comparing the length of the normal is greather than 1e-6f
//       polytope.push_back(Face{ {indexSet[0],indexSet[1],indexSet[2]} }, normal, distance);
//   }
//
//   return polytope;
//}
#pragma endregion

#pragma region V2 EPA

inline const PolytopeData InitalizePolytopeV2(const Simplex& simplex) 
{
    std::vector<glm::vec3> polytope(simplex.begin(), simplex.end());
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
        glm::vec3 A = polytope[faceIndexSet[0]]; //First vertex of the face
        glm::vec3 B = polytope[faceIndexSet[1]]; //second vertex of the face
        glm::vec3 C = polytope[faceIndexSet[2]]; //third vertex of the face

        //Compute the normal of the face
        glm::vec3 normal = glm::normalize(glm::cross(B - A, C - A));
        // ensure the normal is facing outward by checking if its pointing away from the origin
        if (glm::dot(normal, A) < 0) normal = -normal; //reverse the normal direction if it faces inward

        ////visualize the edges of the current polytope
        //Debug::DrawLine(A, B, glm::vec4(0, 1, 1, 1), 3);
        //Debug::DrawLine(B, C, glm::vec4(0, 1, 1, 1), 3);
        //Debug::DrawLine(C, A, glm::vec4(0, 1, 1, 1), 3);
        ////Draw the face normal
        //glm::vec3 centroid = (A + B + C) / 3.0f;
        //Debug::DrawLine(centroid, centroid + normal, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), 3); //orange for the normal

        float distance = glm::dot(normal, A); // distance from the origin to the face along the normal
        
        // Add this face's data (indices, normal, distance) to the polytope data
        faces.push_back({ faceIndexSet,normal,distance });
    }

    return { polytope,faces };
}

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

//GET CLOSEST FACE

//VISUALIZECLOSEST FACE
inline void VisualizeClosestFace(const PolytopeData& data, const int closestFaceIndex)
{
    //visualize the closest face and its normal
    const auto& faceIndices = data.face[closestFaceIndex].polytopeIndices;
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

//SIMPLE ONE INDEX
inline bool EdgeSharesVertex(const Face& face, int newVertexIndex)
{
    // Check if the new vertex is already part of the face
    // Compare the vertex index with the indices in the face
    return (face.polytopeIndices[0] == newVertexIndex ||
        face.polytopeIndices[1] == newVertexIndex ||
        face.polytopeIndices[2] == newVertexIndex);
}

//EXTENDED VERSION CHECKING SHARED VERTICES
inline bool EdgeSharesVertex(const Face& face, int newVertexIndex, const PolytopeData& polytopeData)
{
    // Retrieve the vertices of the current face
    glm::vec3 A = polytopeData.polytope[face.polytopeIndices[0]];
    glm::vec3 B = polytopeData.polytope[face.polytopeIndices[1]];
    glm::vec3 C = polytopeData.polytope[face.polytopeIndices[2]];

    // Retrieve the new vertex
    glm::vec3 newVertex = polytopeData.polytope[newVertexIndex];

    // Check if the new vertex is part of any edge in the face
    // We are comparing the new vertex to see if it lies along the edges of the face
    return (glm::length(newVertex - A) < 1e-6f || glm::length(newVertex - B) < 1e-6f || glm::length(newVertex - C) < 1e-6f);
}


#pragma endregion
