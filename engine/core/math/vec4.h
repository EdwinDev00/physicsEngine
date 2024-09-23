#pragma once

#include <math.h>

class vec4 {
public:
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 0;

	//Constructors
	vec4()
	{
		this->x = 0;
		this->y = 0;
		this->z = 0;
		this->w = 0;
	}

	vec4(const float x, const float y, const float z, const float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	vec4(vec4 const& v)
	{
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
		this->w = v.w;
	}

	//Operator overloads
	vec4& operator=(vec4 const& rhs)
	{
		this->x = rhs.x;
		this->y = rhs.y;
		this->z = rhs.z;
		this->w = rhs.w;

		return *this;
	}

	vec4 operator-()
	{
		return vec4(-this->x, -this->y, -this->z, -this->w);
	}

	vec4 operator+(vec4 const& rhs)
	{
		return vec4(this->x + rhs.x,
			this->y + rhs.y,
			this->z + rhs.z,
			this->w + rhs.w);
	}

	vec4& operator+=(vec4 const& rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;
		this->z += rhs.z;
		this->w += rhs.w;

		return *this;
	}

	vec4 operator-(vec4 const& rhs)
	{
		vec4 result = vec4();
		result.x = this->x - rhs.x;
		result.y = this->y - rhs.y;
		result.z = this->z - rhs.z;
		result.w = this->w - rhs.w;

		return result;
	}

	vec4& operator-=(vec4 const& rhs)
	{
		this->x -= rhs.x;
		this->y -= rhs.y;
		this->z -= rhs.z;
		this->w -= rhs.w;

		return *this;
	}

	vec4 operator*(const float scalar)
	{
		vec4 result = vec4();
		result.x = this->x * scalar;
		result.y = this->y * scalar;
		result.z = this->z * scalar;
		result.w = this->w * scalar;

		return result;
	}
	vec4& operator*=(const float scalar)
	{
		this->x *= scalar;
		this->y *= scalar;
		this->z *= scalar;
		this->w *= scalar;

		return *this;
	}

	bool operator==(vec4 const& rhs)
	{
		return (this->x == rhs.x 
			&& this->y == rhs.y 
			&& this->z == rhs.z
			&& this->w ==rhs.w) ? true : false;
	}

	bool operator!=(vec4 const& rhs)
	{
		return (this->x == rhs.x
			&& this->y == rhs.y
			&& this->z == rhs.z
			&& this->w == rhs.w) ? false : true;
	}

	float& operator[](const unsigned int i)
	{
		switch (i)
		{
		case 0:
			return this->x;
		case 1:
			return this->y;
		case 2:
			return this->z;
		case 3:
			return this->w;
		}
	}

	const float& operator[](const unsigned int i) const
	{
		switch (i)
		{
		case 0:
			return this->x;
		case 1:
			return this->y;
		case 2:
			return this->z;
		case 3:
			return this->w;
		}
	}
};

inline float dot(vec4 const& a, vec4 const& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

inline float length(vec4 const& v)
{
	return (float)sqrt(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
}

inline vec4 normalize(vec4 const& v)
{
	vec4 result = vec4();
	result.x = v.x/length(v);
	result.y = v.y/length(v);
	result.z = v.z/length(v);
	result.w = v.w/length(v);

	return result;
}