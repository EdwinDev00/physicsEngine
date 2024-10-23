#pragma once

#pragma region RAY

inline bool RayIntersectAABB(const Ray& ray, const AABB& aabb)
{
	glm::vec3 rayDir = ray.direction;

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

#pragma endregion

#pragma region GJK ( SIMPLEX + SUPPORT)

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
    return furthestPoint;
}

inline SupportPoint Support(const GameObject& objA, const GameObject& objB,  const glm::vec3& direction)
{     
        glm::mat3 modelRotationA = objA.GetRotationMat();
        glm::mat3 modelRotationB = objB.GetRotationMat();

        // Get the furthest point on objA in the given direction (world space)
        glm::vec3 furthestPointA = modelRotationA * GetFurthestPointInDirection(objA, transpose(modelRotationA) * direction) * objA.GetScale() + objA.GetPosition();
    
        // Get the furthest point on objB in the opposite direction (world space)
        glm::vec3 furthestPointB = modelRotationB * GetFurthestPointInDirection(objB, transpose(modelRotationB) * -direction) * objB.GetScale() + objB.GetPosition();

        // Minkowski difference: objA - objB
        glm::vec3 minkowskiDifference = furthestPointA - furthestPointB;

        return { furthestPointA,furthestPointB, minkowskiDifference };
}

inline bool LineCase(Simplex& simplex, glm::vec3& direction)
{
    SupportPoint a = simplex[0];
    SupportPoint b = simplex[1];

    glm::vec3 ab = b.minkowDiff - a.minkowDiff;
    glm::vec3 ao = -a.minkowDiff;

    if (dot(ab, ao) > 0) {
        direction = cross(cross(ab, ao), ab);
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
            direction = cross(cross(ac, ao), ac);
        }

        else {
            return LineCase(simplex = { a, b }, direction);
        }
    }

    else {
        if (dot(cross(ab, abc), ao) > 0) {
            return LineCase(simplex = { a, b }, direction);
        }

        if (dot(abc, ao) > 0) {
              direction = abc;
        }

        else {
            simplex = { a, c, b };
            direction = -abc;
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
        default: return false;
    } 
}

#pragma endregion

#pragma region EPA
inline std::pair<std::vector<glm::vec4>, int>
GetFaceNormals(std::vector<SupportPoint> const& polytope, std::vector<int> const& faces) {
    std::vector<glm::vec4> normals;
    uint32 minTriangle = 0;
    float minDistance = FLT_MAX;

    for (int i = 0; i < faces.size(); i += 3) {
        glm::vec3 a = polytope[faces[i]].minkowDiff;
        glm::vec3 b = polytope[faces[i + 1]].minkowDiff;
        glm::vec3 c = polytope[faces[i + 2]].minkowDiff;

       glm::vec3 normal = glm::normalize(cross(b - a, c - a));
        float distance = dot(normal, a);

        if (distance < 0) {
            normal *= -1;
            distance *= -1;
        }

        normals.emplace_back(normal, distance);

        if (distance < minDistance) {
            minTriangle = i / 3;
            minDistance = distance;
        }
    }

    return { normals, minTriangle };
}

inline void
AddIfUniqueEdge(
    std::vector<std::pair<int, int> >& edges,
    const std::vector<int>& faces,
    uint32 a,
    uint32 b) {
    // Check if the edge (b, a) already exists in reverse order
    auto reverse = std::find(      
        edges.begin(),                              //     / \ B /   A: 2-0
        edges.end(),                                //     / A \ /    B: 0-2
        std::make_pair(faces[b], faces[a])
    );
    // If reverse edge exists, remove it; otherwise, add the edge (a, b)
    if (reverse != edges.end()) {
        edges.erase(reverse);
    }
    else {
        edges.emplace_back(faces[a], faces[b]);
    }
}

inline glm::vec3 Barycentric(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& p)
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
        return glm::vec3(0);
    }
#endif

   float v = (d11 * d20 - d01 * d21) / denom;
   float w = (d00 * d21 - d01 * d20) / denom;
   float u = 1.0f - v - w;

    return glm::vec3(u,v,w);
}

inline glm::vec3
Cartesian(glm::vec3 const& b, glm::vec3 const& p0, glm::vec3 const& p1, glm::vec3 const& p2) {
    return p0 * b.x + p1 * b.y + p2 * b.z;
}

inline std::pair<glm::vec3, glm::vec3>
CalcCollisionPoint(const SupportPoint face[]) {
    // Project the origin onto the triangle in Minkowski space to get barycentric coordinates
    glm::vec3 closestBary = Barycentric(
        face[0].minkowDiff,
        face[1].minkowDiff,
        face[2].minkowDiff,
        glm::vec3());

    glm::vec3 colPointA = Cartesian(
        closestBary,
        face[0].Asupport,
        face[1].Asupport,
        face[2].Asupport);

   glm::vec3 colPointB = Cartesian(
        closestBary,
        face[0].Bsupport,
        face[1].Bsupport,
        face[2].Bsupport);

    return { colPointA, colPointB };
}
#pragma endregion
