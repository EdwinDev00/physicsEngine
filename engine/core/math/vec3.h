//#pragma once
//
//#include <math.h>
//
//class vec3 {
//public:
//	float x = 0;
//	float y = 0;
//	float z = 0;
//
//	//Constructors
//	vec3()
//	{
//		this->x = 0;
//		this->y = 0;
//		this->z = 0;
//	}
//
//	vec3(const float x, const float y, const float z)
//	{
//		this->x = x;
//		this->y = y;
//		this->z = z;
//	}
//
//	vec3(vec3 const& v)
//	{
//		this->x = v.x;
//		this->y = v.y;
//		this->z = v.z;
//	}
//
//	vec3(float& v)
//	{
//		this->x = v;
//		this->y = v;
//		this->z = v;
//	}
//
//	//Operator overloads
//	vec3& operator=(vec3 const& rhs)
//	{
//		this->x = rhs.x;
//		this->y = rhs.y;
//		this->z = rhs.z;
//
//		return *this;
//	}
//
//	vec3 operator-()
//	{
//		return vec3(-this->x, -this->y, -this->z);
//	}
//
//	vec3 operator+(vec3 const& rhs)
//	{
//		return vec3(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z);
//	}
//	vec3& operator+=(vec3 const& rhs) 
//	{
//		this->x += rhs.x;
//		this->y += rhs.y;
//		this->z += rhs.z;
//
//		return *this;
//	}
//
//	vec3 operator-(vec3 const& rhs)
//	{
//		vec3 result = vec3();
//		result.x = this->x - rhs.x;
//		result.y = this->y - rhs.y;
//		result.z = this->z - rhs.z;
//
//		return result;
//	}
//
//	const vec3 operator-(vec3 const& rhs) const
//	{
//		vec3 result = vec3();
//		result.x = this->x - rhs.x;
//		result.y = this->y - rhs.y;
//		result.z = this->z - rhs.z;
//
//		return result;
//	}
//
//	vec3& operator-=(vec3 const& rhs) 
//	{
//		this->x -= rhs.x;
//		this->y -= rhs.y;
//		this->z -= rhs.z;
//
//		return *this;
//	}
//
//	vec3 operator*(const float scalar)
//	{
//		vec3 result = vec3();
//		result.x = this->x * scalar;
//		result.y = this->y * scalar;
//		result.z = this->z * scalar;
//
//		return result;
//	}
//
//	vec3& operator*=(const float scalar)
//	{
//		this->x *= scalar;
//		this->y *= scalar;
//		this->z *= scalar;
//
//		return *this;
//	}
//
//	bool operator==(vec3 const& rhs)
//	{
//		return (this->x == rhs.x && this->y == rhs.y && this->z == rhs.z) ? true : false;
//	}
//
//	bool operator!=(vec3 const& rhs)
//	{
//		return (this->x == rhs.x
//			&& this->y == rhs.y 
//			&& this->z == rhs.z) ? false : true;
//	}
//
//	float& operator[](const unsigned int i)
//	{
//		switch (i)
//		{
//		case 0:
//			return this->x;
//		case 1:
//			return this->y;
//		case 2:
//			return this->z;
//		}
//	}
//};
//
//inline float dot(vec3 const& a, vec3 const& b)
//{
//	return a.x * b.x + a.y * b.y + a.z * b.z;
//}
//
//inline float length(vec3 const& v)
//{
//	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
//}
//
//inline vec3 cross(vec3 const& a, vec3 const& b)
//{
//	vec3 result = vec3();
//	result.x = a.y * b.z - a.z * b.y;
//	result.y = a.z * b.x - a.x * b.z;
//	result.z = a.x * b.y - a.y * b.x;
//
//	return result;
//}
//
//inline vec3 normalize(vec3 const& v)
//{
//	vec3 result = vec3();
//	float len = length(v);
//	result.x = v.x / len;
//	result.y = v.y / len;
//	result.z = v.z / len;
//
//	return result;
//}