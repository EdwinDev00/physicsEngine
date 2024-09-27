#pragma once
//#include "config.h"

//#define EPSILON 1e-8

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

inline bool RayTriangleIntersection(const Ray& ray, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t, glm::vec3& hitPoint)
{
    // Normalize ray direction to avoid calculation errors
    glm::vec3 rayDir = ray.direction;
    if (glm::length(rayDir) != 1.0f) {
        rayDir = glm::normalize(ray.direction);
    }

    // Edge vectors of the triangle
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;

    // Compute perpendicular vector between ray direction and edge2
    glm::vec3 h = glm::cross(rayDir, edge2);
    float a = glm::dot(edge1, h);

    // Dynamic EPSILON to handle precision
    const float EPSILON = 0.000001f;

    // If 'a' is close to zero, the ray is parallel to the triangle plane
    if (fabs(a) < EPSILON) {
        std::cout << "FAILED: Ray is parallel to the triangle\n";
        return false;  
    }

    float invA = 1.0f / a;
    glm::vec3 s = ray.origin - v0;

    // Calculate u parameter and test if it's within the triangle bounds
    float u = invA * glm::dot(s, h);
    if (u < -EPSILON || u > 1.0f + EPSILON) {
        std::cout << "FAILED: u out of bounds. u = " << u << "\n";
        return false;
    }

    // Calculate v parameter and test if it's within bounds
    glm::vec3 qvec = glm::cross(s, edge1);
    float v = invA * glm::dot(rayDir, qvec);
    if (v < -EPSILON || (u + v) > 1.0f + EPSILON) {
        std::cout << "FAILED: v out of bounds. v = " << v << ", u + v = " << (u + v) << "\n";
        return false;
    }

    // Calculate the intersection distance along the ray (t)
    t = invA * glm::dot(edge2, qvec);

    // Ensure the intersection is in front of the ray origin (t > 0)
    if (t > EPSILON) {
        // Compute the intersection point in world coordinates
        hitPoint = ray.origin + rayDir * t;
        std::cout << "SUCCESS: Intersection found at t = " << t << ", u = " << u << ", v = " << v << "\n";
        return true;
    }

    std::cout << "FAILED: Intersection behind the ray origin\n";
    return false;
}
