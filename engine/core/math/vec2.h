#pragma once

#include <math.h>

class vec2 {
public:
	float x = 0;
	float y = 0;

	//Constructors
	vec2()
	{
		this->x = 0;
		this->y = 0;
	}

	vec2(const float x, const float y)
	{
		this->x = x;
		this->y = y;
	}

	vec2(vec2 const& v)
	{
		this->x = v.x;
		this->y = v.y;
	}

	//Operator overloads
	vec2& operator=(vec2 const& rhs)
	{
		this->x = rhs.x;
		this->y = rhs.y;

		return *this;
	}

	vec2 operator-()
	{
		return vec2(-this->x, -this->y);
	}

	vec2 operator+(vec2 const& rhs)
	{
		return vec2(this->x + rhs.x, this->y + rhs.y);
	}
	vec2& operator+=(vec2 const& rhs) 
	{
		this->x += rhs.x;
		this->y += rhs.y;

		return *this;
	}

	vec2 operator-(vec2 const& rhs)
	{
		vec2 result = vec2();
		result.x = this->x - rhs.x;
		result.y = this->y - rhs.y;

		return result;
	}

	const vec2 operator-(vec2 const& rhs) const
	{
		vec2 result = vec2();
		result.x = this->x - rhs.x;
		result.y = this->y - rhs.y;

		return result;
	}

	vec2& operator-=(vec2 const& rhs) 
	{
		this->x -= rhs.x;
		this->y -= rhs.y;

		return *this;
	}

	vec2 operator*(const float scalar)
	{
		vec2 result = vec2();
		result.x = this->x * scalar;
		result.y = this->y * scalar;

		return result;
	}

	vec2& operator*=(const float scalar)
	{
		this->x *= scalar;
		this->y *= scalar;

		return *this;
	}

	bool operator==(vec2 const& rhs)
	{
		return (this->x == rhs.x && this->y == rhs.y);
	}

	bool operator!=(vec2 const& rhs)
	{
		return !(this->x == rhs.x
			&& this->y == rhs.y);
	}

	float& operator[](const unsigned int i)
	{
		switch (i)
		{
		case 0:
			return this->x;
		case 1:
			return this->y;
		}
	}
};

inline float dot(vec2 const& a, vec2 const& b)
{
	return a.x * b.x + a.y * b.y;
}

inline float length(vec2 const& v)
{
	return sqrt(v.x*v.x + v.y*v.y);
}

inline vec2 normalize(vec2 const& v)
{
	vec2 result = vec2();
	float len = length(v);
	result.x = v.x / len;
	result.y = v.y / len;

	return result;
}