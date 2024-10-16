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
    for (auto& mesh : obj.modelObject->meshes)
    {
        for (auto& data : mesh.primitives)
        {
            for (int i = 0; i < data.vertices.size(); i += 3)
            {
                float distance = glm::dot(data.vertices[i].position, direction);
                if (distance > maxDistance)
                {
                    maxDistance = distance;
                    furthestPoint = data.vertices[i].position;
                }
            }
        }
    }

   /* for(const Triangles& triangle : obj.GetTriangles())
        for(const glm::vec3& vertex : triangle.vertices)
        {
            float distance = glm::dot(vertex, direction);
            if(distance > maxDistance)
            {
                maxDistance = distance;
                furthestPoint = vertex;
            }
        }*/
    return furthestPoint;
}

inline SupportPoint Support(const GameObject& objA, const GameObject& objB,  const glm::vec3& direction)
{
        
        //glm::vec3 ndirection = glm::normalize(direction);
        glm::mat3 modelRotationA = glm::transpose(objA.GetRotationMat());
        glm::mat3 modelRotationB = glm::transpose(objB.GetRotationMat());

        // Get the furthest point on objA in the given direction
        glm::vec3 furthestPointA = modelRotationA * GetFurthestPointInDirection(objA, direction * modelRotationA  ) * objA.GetScale() + objA.GetPosition();
    
        // Get the furthest point on objB in the opposite direction
        glm::vec3 furthestPointB = modelRotationB * GetFurthestPointInDirection(objB, -direction * modelRotationB ) * objB.GetScale() + objB.GetPosition();

        // Minkowski difference: objA - objB
        glm::vec3 minkowskiDifference = furthestPointA - furthestPointB;

        return { furthestPointA,furthestPointB, minkowskiDifference }; // Return the point on the Minkowski difference
}

inline bool LineCase(Simplex& simplex, glm::vec3& direction)
{
    SupportPoint a = simplex[0];
    SupportPoint b = simplex[1];

    glm::vec3 ab = b.minkowDiff - a.minkowDiff;
    glm::vec3 ao = -a.minkowDiff;

    if (dot(ab, ao) > 0) {
        direction = cross(cross(ab, ao), ab); //testing normalize
    }

    else {
        simplex = { a };
        direction = ao;
    }

    return false;
}

//inline bool LineCase(Simplex& simplex, glm::vec3& direction)
//{
//    glm::vec3 A = simplex[0]; // Last added point to the simplex (most recent point)
//    glm::vec3 B = simplex[1]; // Second point in the line simplex
//
//    glm::vec3 AB = B - A; // Vector from A to B (forming the edge of the line)
//    glm::vec3 A0 = -A; // Vector from A to the origin (find if origin is in this direction)
//
//    // if the origin is in the direction of the AB, move towards it
//    if (glm::dot(AB, A0) > 0) 
//    { 
//        // Project the search direction towards the origin, in the direction of AB
//        glm::vec3 crossProd = glm::cross(AB, A0);
//        //if (glm::length(crossProd) < 1e-6f) {
//        //    // Handle case where the cross product is nearly zero
//          //  direction = A0;
//        //}
//        //else {
//            direction = glm::cross(crossProd, AB);
//        //}
//    }
//    else 
//    {
//        //otherwise, the simplex is just the point A
//        simplex = { A };
//        direction = A0; // Update the direction to point from A to the origin (AO)
//
//    }
//    return false;
//}

inline bool TriangleCase(Simplex& simplex, glm::vec3& direction)
{
    SupportPoint a = simplex[0];
    SupportPoint b = simplex[1];
    SupportPoint c = simplex[2];

    glm::vec3 ab = b.minkowDiff - a.minkowDiff;
    glm::vec3 ac = c.minkowDiff - a.minkowDiff;
    glm::vec3 ao = -a.minkowDiff;

    glm::vec3 abc = cross(ab, ac); //testing normalize

    if (dot(cross(abc, ac), ao) > 0) {
        if (dot(ac, ao) > 0) {
            simplex = { a, c };
            direction = normalize(cross(cross(ac, ao), ac));
        }

        else {
            return LineCase(simplex = { a, b }, direction);
        }
    }

    else {
        if (dot(cross(ab, abc), ao) > 0) {
            return LineCase(simplex = { a, b }, direction);
        }

        else {
            if (dot(abc, ao) > 0) {
                direction = abc;
            }

            else {
                simplex = { a, c, b };
                direction = -abc;
            }
        }
    }

    return false;
}

//inline bool
//TriangleCase(Simplex& simplex, glm::vec3& dir) {
//    glm::vec3 a = simplex[0];
//    glm::vec3 b = simplex[1];
//    glm::vec3 c = simplex[2];
//
//    glm::vec3 ab = b - a;
//    glm::vec3 ac = c - a;
//    glm::vec3 ao = -a;
//
//   glm::vec3 abc = cross(ab, ac);
//
//    if (dot(cross(abc, ac), ao) > 0) {
//        if (dot(ac, ao) > 0) {
//            simplex = { a, c };
//            dir = cross(cross(ac, ao), ac);
//        }
//        else
//        {
//            return LineCase(simplex = { a, b }, dir);
//        }
//    }
//    else {
//        if (dot(cross(ab, abc), ao) > 0)
//            return LineCase(simplex = { a, b }, dir);
//
//        if (dot(abc, ao))
//            dir = abc;
//
//        else {
//            simplex = { a, c, b };
//            dir = -abc;
//        }
//    }
//
//    return false;
//}

//inline bool TriangleCase(Simplex& simplex, glm::vec3& direction) 
//{
//    glm::vec3 A = simplex[0];
//    glm::vec3 B = simplex[1];
//    glm::vec3 C = simplex[2];
//
//    glm::vec3 AB = B - A;
//    glm::vec3 AC = C - A;
//    glm::vec3 A0 = -A;
//
//    //Compute the normal of the triangle
//    glm::vec3 ABC_normal = glm::cross(AB, AC);
//    glm::vec3 ACB_normal = glm::cross(AB, AC);
//    glm::vec3 ABC_normal = glm::cross(AB, AC);
//
//    //Origin is above or below triangle, check which side the triangle's plane faces
//    if(glm::dot(ABC_normal,A0) > 0)
//    {
//        //Origin is above, keep all three point
//        direction = ABC_normal;
//    }
//    else
//    {
//        //Origin is below the triangle (flips the triangle winding order)
//        simplex = { A,C,B };
//        direction = -ABC_normal;
//    }
//
//
//    ////Determine which side of the triangle the origin is on
//    //glm::vec3 ABC_normal_toward_origin = glm::cross(ABC_normal, AC);
//
//    ////Check if the origin is on the AC side of the triangle
//    //if(glm::dot(ABC_normal_toward_origin,A0) > 0)
//    //{
//    //    //Origin is outside AC
//    //    simplex = { A,C };
//    //    direction = glm::cross(glm::cross(AC, A0), AC); //New direction is perpendicular to AC
//    //}
//    //else
//    //{
//        ////Check if the origin is on the AB side of the triangle
//        //glm::vec3 ABC_normal_other_side = glm::cross(AB, ABC_normal);
//        //if(glm::dot(ABC_normal_other_side,A0) > 0)
//        //{
//        //    //Origin is outside AB
//        //    simplex = { A,B };
//        //    direction = glm::cross(glm::cross(AB, A0), AB);
//        //}
//        //else
//        //{
//
//
//       // }
//    //}
//
//    return false;
//}


inline bool TetrahedronCase(Simplex& simplex, glm::vec3& direction)
{
    SupportPoint a = simplex[0];
    SupportPoint b = simplex[1];
    SupportPoint c = simplex[2];
    SupportPoint d = simplex[3];

    glm::vec3 ab = b.minkowDiff - a.minkowDiff;
    glm::vec3 ac = c.minkowDiff - a.minkowDiff;
    glm::vec3 ad = d.minkowDiff - a.minkowDiff;
    glm::vec3 ao = -a.minkowDiff;

    glm::vec3 abc = cross(ab, ac);
    glm::vec3 acd = cross(ac, ad);
    glm::vec3 adb = cross(ad, ab);

    if (dot(abc, ao) > 0) {
        return TriangleCase(simplex = { a, b, c }, direction);
    }

    if (dot(acd, ao) > 0) {
        return TriangleCase(simplex = { a, c, d }, direction);
    }

    if (dot(adb, ao) > 0) {
        return TriangleCase(simplex = { a, d, b }, direction);
    }

    return true;
}

//inline bool TetrahedronCase(Simplex& simplex, glm::vec3& direction) 
//{
//    glm::vec3 A = simplex[0];  //Last added point
//    glm::vec3 B = simplex[1];
//    glm::vec3 C = simplex[2];
//    glm::vec3 D = simplex[3];
//
//    glm::vec3 AB = B - A;
//    glm::vec3 AC = C - A;
//    glm::vec3 AD = D - A;
//    glm::vec3 AO = -A;
//
//    //Compute normals for each face of the tetrahedron
//    glm::vec3 ABC_normal = glm::cross(AB, AC);  //Triangle ABC
//    glm::vec3 ACD_normal = glm::cross(AC, AD);  //Triangle ACD
//    glm::vec3 ADB_normal = glm::cross(AD, AB);  //Triangle ABD
//
//    //Check if the origin is outside the ABC face
//    if (glm::dot(ABC_normal, AO) > 0) {
//        //Origin is outside ABC, handle it like a triangle case
//        simplex = { A, B, C };
//        direction = ABC_normal;
//        return false;
//    }
//
//    //Check if the origin is outside the ACD face
//    if (glm::dot(ACD_normal, AO) > 0) {
//        //Origin is outside ACD, handle it like a triangle case
//        simplex = { A, C, D };
//        direction = ACD_normal;
//        return false;
//    }
//
//    //Check if the origin is outside the ABD face
//    if (glm::dot(ADB_normal, AO) > 0) {
//        //Origin is outside ABD, handle it like a triangle case
//        simplex = { A, D, B };
//        direction = ADB_normal;
//        return false;
//    }
//
//    //If the origin is not outside any face, it must be inside the tetrahedron
//    return true;  // Collision detected (origin inside the tetrahedron)
//}

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
inline Face CreateFace(int v1,int v2,int v3, const std::vector<SupportPoint>& polytope)
{
    Face newFace;
    newFace.polytopeIndices = { v1,v2,v3 };
    glm::vec3 A = polytope[v1].minkowDiff;
    glm::vec3 B = polytope[v2].minkowDiff;
    glm::vec3 C = polytope[v3].minkowDiff;

    //normal calculation became nan on one of the component when normalized
    glm::vec3 test = glm::cross(B - A, C - A);//glm::normalize(glm::cross(B - A, C - A)); 
    newFace.normal = test;
    if (glm::dot(newFace.normal, A) < 0) newFace.normal = -newFace.normal;
    newFace.distance = glm::dot(newFace.normal, A);
    return newFace;
}

inline void AddIfUniqueEdges(std::vector<std::pair<int,int>>& edge, const std::array<int,3>& face, const int a, const int b)
{
    // Check if the edge exists in reverse order (shared by another face)
    auto reverseEdgeIt = std::find(edge.begin(), edge.end(), std::make_pair(face[b], face[a]));
    if (reverseEdgeIt != edge.end())
        edge.erase(reverseEdgeIt);
    else
        edge.emplace_back(face[a], face[b]);
}

inline const PolytopeData InitalizePolytopeV2(const Simplex& simplex) 
{
    std::vector<SupportPoint> polytope(simplex.begin(), simplex.end());
    //std::reverse(polytope.begin(), polytope.end()); // reverse the order to follow push back (lastest one is at back)
    std::vector<Face> faces;

    std::vector<std::array<int, 3>> indices =
    {
        {0, 1, 2}, // Reversed order for face {0, 1, 2}
        {0, 3, 1}, // Reversed order for face {0, 3, 1}
        {0, 2, 3}, // Reversed order for face {0, 2, 3}
        {1, 3, 2}  // Reversed order for face {1, 3, 2}
    };

    for(const auto& faceIndexSet :indices)
    {
        faces.push_back(CreateFace(faceIndexSet[0], faceIndexSet[1], faceIndexSet[2], polytope));
    }

    return { polytope,faces };
}


//EXPANDING LOGIC
inline void ExpandPolytope(PolytopeData& data, int& closestFaceIndex, const SupportPoint& supportPoint)
{
    const auto& closestFace = data.face[closestFaceIndex];
    //Add the new vertex to the polytope
    int newVertexIndex = data.polytope.size();
    data.polytope.push_back(supportPoint);

    //EPA Edge method
    
    std::vector<std::pair<int, int>> uniqueEdge;
    AddIfUniqueEdges(uniqueEdge, closestFace.polytopeIndices, 0, 1);
    AddIfUniqueEdges(uniqueEdge, closestFace.polytopeIndices, 1, 2);
    AddIfUniqueEdges(uniqueEdge, closestFace.polytopeIndices, 2, 0);

    data.face.erase(data.face.begin() + closestFaceIndex);

    //IT SHOULD CREATE ONLY ONE NEW FACE WHICH CONNECTS THE UNIQUE ONE TO THIS FACE
    std::vector<Face> newFaces;
    for(const auto& edge : uniqueEdge)
    {
        Face newFace;
        //CREATE ONE NEW FACE WHICH CONNECT SHARES THE SAME EDGE AS THIS NEW POINT
        newFace.polytopeIndices = { edge.first, edge.second, newVertexIndex };  // Create a new triangle face
        newFace.normal = cross(data.polytope[edge.second].minkowDiff - data.polytope[edge.first].minkowDiff, supportPoint.minkowDiff - data.polytope[edge.first].minkowDiff);
        if (glm::dot(newFace.normal, data.polytope[edge.first].minkowDiff) < 0) newFace.normal = -newFace.normal;
        //newFace.normal = CalculateFaceNormal(data.polytope[edge.first].minkowDiff, data.polytope[edge.second].minkowDiff, supportPoint.minkowDiff); // Calculate new face normal
        newFace.distance = glm::dot(newFace.normal, supportPoint.minkowDiff); // Calculate the distance of the face to the origin
        newFaces.push_back(newFace);
        //newFaces.push_back(CreateFace(edge.first, edge.second, newVertexIndex, data.polytope));
    }


    // Add the new faces to the polytope's face list
    data.face.insert(data.face.end(), newFaces.begin(), newFaces.end());

    //// Recompute the closest face
    //float minDistance = FLT_MAX;
    //int newClosestFaceIndex = -1;
    //for (int i = 0; i < data.face.size(); ++i) {
    //    if (data.face[i].distance < minDistance) {
    //        minDistance = data.face[i].distance;
    //        newClosestFaceIndex = i;
    //    }
    //}

    //// Update the closest face index (this face will be expanded next)
    //closestFaceIndex = newClosestFaceIndex;
}

//VISUALIZECLOSEST FACE
inline void VisualizePolytopeWithClosestFace(const PolytopeData& data, const int closestFaceIndex)
{
    //Visualize the whole polytope
    for (const Face& face : data.face)
    {
        glm::vec3 A = data.polytope[face.polytopeIndices[0]].minkowDiff;
        glm::vec3 B = data.polytope[face.polytopeIndices[1]].minkowDiff;
        glm::vec3 C = data.polytope[face.polytopeIndices[2]].minkowDiff;

        // Draw the edges of the new face
        Debug::DrawLine(-A, -B, glm::vec4(0, 1, 1, 1), 3); // Cyan for the edges
        Debug::DrawLine(-B, -C, glm::vec4(0, 1, 1, 1), 3);
        Debug::DrawLine(-C, -A, glm::vec4(0, 1, 1, 1), 3);


    }

    //visualize the closest face and its normal
    const auto& closestFace = data.face[closestFaceIndex];
    const auto& faceIndices = closestFace.polytopeIndices;
    glm::vec3 A = data.polytope[faceIndices[0]].minkowDiff; //First vertex of the face
    glm::vec3 B = data.polytope[faceIndices[1]].minkowDiff; //second vertex of the face
    glm::vec3 C = data.polytope[faceIndices[2]].minkowDiff; //third vertex of the face
    glm::vec3 normal = normalize(data.face[closestFaceIndex].normal);

    // Highlight the edges of the closest face in green
    Debug::DrawLine(-A, -B, glm::vec4(0, 1, 0, 1), 3); // Green for edges
    Debug::DrawLine(-B, -C, glm::vec4(0, 1, 0, 1), 3); // Green for edges
    Debug::DrawLine(-C, -A, glm::vec4(0, 1, 0, 1), 3); // Green for edges

    // Highlight the normal of the closest face in green
    glm::vec3 centroid = (A + B + C) / 3.0f; // Calculate the centroid of the face
    Debug::DrawLine(centroid, centroid + normal, glm::vec4(0, 1, 0, 1), 1); // Green for the normal
}

inline glm::vec3
Cartesian(glm::vec3 const& b, glm::vec3 const& p0, glm::vec3 const& p1, glm::vec3 const& p2) {
    return p0 * b.x + p1 * b.y + p2 * b.z;
}

inline glm::vec3
ProjectToTriangle(glm::vec3 const& p, glm::vec3 const& a, glm::vec3 const& b, glm::vec3 const& c) {
    glm::vec3 n, q, r, t;
    q = b - a;
    r = c - a;
    n = glm::cross(q, r);

    q = a - p;
    r = b - p;
    const float wc = glm::dot(n, glm::cross(q, r));

    t = c - p;
    const float wa = glm::dot(n, glm::cross(r, t));

    const float wb = glm::dot(n, glm::cross(t, q));

    const float denom = wa + wb + wc;

    return glm::vec3(wa / denom, wb / denom, wc / denom);
}
#pragma endregion
