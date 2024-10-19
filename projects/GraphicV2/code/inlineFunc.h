#pragma once
#include <map>
#include <math.h>
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

    for (int i = 0; i < obj.colliderVertices.size(); i ++)
    {
        float distance = glm::dot(obj.colliderVertices[i], direction);
        if (distance > maxDistance)
        {
            maxDistance = distance;
            furthestPoint = obj.colliderVertices[i];
        }
    }

    //for(const Triangles& triangle : obj.GetTriangles())
    //    for(const glm::vec3& vertex : triangle.vertices)
    //    {
    //        float distance = glm::dot(vertex, direction);
    //        if(distance > maxDistance)
    //        {
    //            maxDistance = distance;
    //            furthestPoint = vertex;
    //        }
    //    }
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
    newFace.distance = glm::dot(newFace.normal, A);
    if (glm::dot(newFace.normal, A) < 0) newFace.normal = -newFace.normal; newFace.distance = -newFace.distance;
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
    std::vector<Face> faces;

    std::vector<std::array<int, 3>> indices =
    {
        {0, 1, 2},
        {0, 3, 1}, 
        {0, 2, 3}, 
        {1, 3, 2}
    };

    for(const auto& faceIndexSet :indices)
    {
        faces.push_back(CreateFace(faceIndexSet[0], faceIndexSet[1], faceIndexSet[2], polytope));
    }

    return { polytope,faces };
}


//EXPANDING LOGIC
inline bool ExpandPolytope(PolytopeData& data, int& closestFaceIndex, const SupportPoint& supportPoint)
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

    if (uniqueEdge.empty())
        return false;

    data.face.erase(data.face.begin() + closestFaceIndex);

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
        //newFaces.push_back(CreateFace(edge.first, edge.second, newVertexIndex, data.polytope));
        newFaces.push_back(newFace);
    }

    // Add the new faces to the polytope's face list
    data.face.insert(data.face.end(), newFaces.begin(), newFaces.end());
    return true;
    //// recompute the closest face
    //float mindistance = FLT_MAX;
    //int newclosestfaceindex = 0;
    //for (int i = 0; i < data.face.size(); ++i) {
    //    if (data.face[i].distance < mindistance) {
    //        mindistance = data.face[i].distance;
    //        newclosestfaceindex = i;
    //    }
    //}

    //// update the closest face index (this face will be expanded next)
    //closestFaceIndex = newclosestfaceindex;
    //return true;
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
        Debug::DrawLine(A, B, glm::vec4(0, 1, 1, 1), 3); // Cyan for the edges
        Debug::DrawLine(B, C, glm::vec4(0, 1, 1, 1), 3);
        Debug::DrawLine(C, A, glm::vec4(0, 1, 1, 1), 3);
    }



    //visualize the closest face and its normal
    const auto& closestFace = data.face[closestFaceIndex];
    const auto& faceIndices = closestFace.polytopeIndices;
    glm::vec3 A = data.polytope[faceIndices[0]].minkowDiff; //First vertex of the face
    glm::vec3 B = data.polytope[faceIndices[1]].minkowDiff; //second vertex of the face
    glm::vec3 C = data.polytope[faceIndices[2]].minkowDiff; //third vertex of the face
    glm::vec3 normal = normalize(data.face[closestFaceIndex].normal);

    // Highlight the edges of the closest face in green
    Debug::DrawLine(A, B, glm::vec4(0, 1, 0, 1), 3); // Green for edges
    Debug::DrawLine(B, C, glm::vec4(0, 1, 0, 1), 3); // Green for edges
    Debug::DrawLine(C, A, glm::vec4(0, 1, 0, 1), 3); // Green for edges

    // Highlight the normal of the closest face in green
    glm::vec3 centroid = (A + B + C) / 3.0f; // Calculate the centroid of the face
    Debug::DrawLine(centroid, centroid + normal, glm::vec4(0, 1, 0, 1), 1); // Green for the normal
}

inline glm::vec3
Cartesian(glm::vec3 const& b, glm::vec3 const& p0, glm::vec3 const& p1, glm::vec3 const& p2) {
    return p0 * b.x + p1 * b.y + p2 * b.z;
}

inline bool Barycentric(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& p, float& u, float& v, float& w)
{
    glm::vec3 v0 = b - a, v1 = c - a, v2 = p - a;
    float d00 = dot(v0, v0);
    float d01 = dot(v0, v1);
    float d11 = dot(v1, v1);
    float d20 = dot(v2, v0);
    float d21 = dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;

#ifndef NDEBUG
    if (denom == 0)
    {
        std::cout << "denom 0\n";
        return false;
    }
#endif

    v = (d11 * d20 - d01 * d21) / denom;
    w = (d00 * d21 - d01 * d20) / denom;
    u = 1.0f - v - w;

    return true;
}

inline glm::vec3
ProjectToTriangle(glm::vec3 const& p, glm::vec3 const& a, glm::vec3 const& b, glm::vec3 const& c) {
    // Check for degenerate triangle

    // Continue with the projection as before
    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;

    glm::vec3 crossProduct = glm::cross(ab, ac);

    float areaSquared = glm::dot(crossProduct, crossProduct);
    if( areaSquared < 1e-6f) return glm::vec3();

    glm::vec3 normal = glm::normalize(glm::cross(ab, ac)); // Normal of the triangle plane

    float distanceToPlane = glm::dot(normal, p - a);
    glm::vec3 projectedPoint = p - normal * distanceToPlane;

    return projectedPoint;
}


/*inline glm::vec3
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

    glm::vec3 test = glm::vec3(wa / denom, wb / denom, wc / denom);

    if (isinf(test.x) || isinf(test.y) || isinf(test.z))
        std::cout << "CUNT " << std::endl;

    return test;
}  */ 
#pragma endregion
