#pragma once

#include <math.h>
#include "vec4.h"
#include "vec3.h"
#include "vec2.h"

class mat4 {
public:
	vec4 m[4] = {
		vec4(),
		vec4(),
		vec4(),
		vec4()
	};

	//Constructors
	mat4()
	{
		this->m[0] = vec4(1, 0, 0, 0);
		this->m[1] = vec4(0, 1, 0, 0);
		this->m[2] = vec4(0, 0, 1, 0);
		this->m[3] = vec4(0, 0, 0, 1);
	}

	mat4(vec4 const& r0, vec4 const& r1, vec4 const& r2, vec4 const& r3)
	{
		this->m[0] = r0;
		this->m[1] = r1;
		this->m[2] = r2;
		this->m[3] = r3;
	}

	mat4(float a)
	{
		this->m[0] = vec4(a, a, a, a);
		this->m[1] = vec4(a, a, a, a);
		this->m[2] = vec4(a, a, a, a);
		this->m[3] = vec4(a, a, a, a);
	}

	mat4(mat4 const& m)
	{
		this->m[0] = m.m[0];
		this->m[1] = m.m[1];
		this->m[2] = m.m[2];
		this->m[3] = m.m[3];
	}

	//Operator overloads
	mat4& operator=(mat4 const& rhs)
	{
		this->m[0] = rhs.m[0];
		this->m[1] = rhs.m[1];
		this->m[2] = rhs.m[2];
		this->m[3] = rhs.m[3];

		return *this;
	}

	const mat4 operator*(mat4 const& rhs) const
	{
		mat4 result = mat4(0);

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				float num = 0;
				for (int k = 0; k < 4; k++)
				{
					num += this->m[k][i] * rhs.m[j][k];
				}

				result[j][i] = num;
			}
		}

		return result;
	}

	const vec4 operator*(vec4 const& rhs) const
	{
		vec4 result = vec4();

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				result[i] += this->m[j][i] * rhs[j];
			}
		}

		return result;
	}

	bool operator==(mat4 const& rhs)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (this->m[i][j] != rhs.m[i][j])
					return false;
			}
		}

		return true;
	}

	bool operator!=(mat4 const& rhs)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (this->m[i][j] != rhs.m[i][j])
					return true;
			}
		}

		return false;
	}
	vec4& operator[](const unsigned int i)
	{
		return m[i];
	}
};

inline float determinant(mat4 const& m)
{
	float D = m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] -
		m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2] -
		m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] +
		m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] +
		m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2] -
		m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] -

		m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] +
		m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2] +
		m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] -
		m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] -
		m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2] +
		m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0] +

		m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] -
		m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] -
		m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] +
		m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] +
		m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] -
		m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] -

		m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2] +
		m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] +
		m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2] -
		m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0] -
		m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] +
		m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0];

	return D;
}

inline mat4 inverse(mat4 const& mat)
{
	//
// Inversion by Cramer's rule.  Code taken from an Intel publication
//
	float Result[4][4] =
	{
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}
	};

	float tmp[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 }; /* temp array for pairs */
	float src[16]
	{
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
		0,0,0,0
	}; /* array of transpose source matrix */
	float det = 0; /* determinant */

	/* transpose matrix */
	for (int i = 0; i < 4; i++)
	{
		src[i + 0] = mat.m[i][0];
		src[i + 4] = mat.m[i][1];
		src[i + 8] = mat.m[i][2];
		src[i + 12] = mat.m[i][3];
	}
	/* calculate pairs for first 8 elements (cofactors) */
	tmp[0] = src[10] * src[15];
	tmp[1] = src[11] * src[14];
	tmp[2] = src[9] * src[15];
	tmp[3] = src[11] * src[13];
	tmp[4] = src[9] * src[14];
	tmp[5] = src[10] * src[13];
	tmp[6] = src[8] * src[15];
	tmp[7] = src[11] * src[12];
	tmp[8] = src[8] * src[14];
	tmp[9] = src[10] * src[12];
	tmp[10] = src[8] * src[13];
	tmp[11] = src[9] * src[12];
	/* calculate first 8 elements (cofactors) */
	Result[0][0] = tmp[0] * src[5] + tmp[3] * src[6] + tmp[4] * src[7];
	Result[0][0] -= tmp[1] * src[5] + tmp[2] * src[6] + tmp[5] * src[7];
	Result[0][1] = tmp[1] * src[4] + tmp[6] * src[6] + tmp[9] * src[7];
	Result[0][1] -= tmp[0] * src[4] + tmp[7] * src[6] + tmp[8] * src[7];
	Result[0][2] = tmp[2] * src[4] + tmp[7] * src[5] + tmp[10] * src[7];
	Result[0][2] -= tmp[3] * src[4] + tmp[6] * src[5] + tmp[11] * src[7];
	Result[0][3] = tmp[5] * src[4] + tmp[8] * src[5] + tmp[11] * src[6];
	Result[0][3] -= tmp[4] * src[4] + tmp[9] * src[5] + tmp[10] * src[6];
	Result[1][0] = tmp[1] * src[1] + tmp[2] * src[2] + tmp[5] * src[3];
	Result[1][0] -= tmp[0] * src[1] + tmp[3] * src[2] + tmp[4] * src[3];
	Result[1][1] = tmp[0] * src[0] + tmp[7] * src[2] + tmp[8] * src[3];
	Result[1][1] -= tmp[1] * src[0] + tmp[6] * src[2] + tmp[9] * src[3];
	Result[1][2] = tmp[3] * src[0] + tmp[6] * src[1] + tmp[11] * src[3];
	Result[1][2] -= tmp[2] * src[0] + tmp[7] * src[1] + tmp[10] * src[3];
	Result[1][3] = tmp[4] * src[0] + tmp[9] * src[1] + tmp[10] * src[2];
	Result[1][3] -= tmp[5] * src[0] + tmp[8] * src[1] + tmp[11] * src[2];
	/* calculate pairs for second 8 elements (cofactors) */
	tmp[0] = src[2] * src[7];
	tmp[1] = src[3] * src[6];
	tmp[2] = src[1] * src[7];
	tmp[3] = src[3] * src[5];
	tmp[4] = src[1] * src[6];
	tmp[5] = src[2] * src[5];

	tmp[6] = src[0] * src[7];
	tmp[7] = src[3] * src[4];
	tmp[8] = src[0] * src[6];
	tmp[9] = src[2] * src[4];
	tmp[10] = src[0] * src[5];
	tmp[11] = src[1] * src[4];
	/* calculate second 8 elements (cofactors) */
	Result[2][0] = tmp[0] * src[13] + tmp[3] * src[14] + tmp[4] * src[15];
	Result[2][0] -= tmp[1] * src[13] + tmp[2] * src[14] + tmp[5] * src[15];
	Result[2][1] = tmp[1] * src[12] + tmp[6] * src[14] + tmp[9] * src[15];
	Result[2][1] -= tmp[0] * src[12] + tmp[7] * src[14] + tmp[8] * src[15];
	Result[2][2] = tmp[2] * src[12] + tmp[7] * src[13] + tmp[10] * src[15];
	Result[2][2] -= tmp[3] * src[12] + tmp[6] * src[13] + tmp[11] * src[15];
	Result[2][3] = tmp[5] * src[12] + tmp[8] * src[13] + tmp[11] * src[14];
	Result[2][3] -= tmp[4] * src[12] + tmp[9] * src[13] + tmp[10] * src[14];
	Result[3][0] = tmp[2] * src[10] + tmp[5] * src[11] + tmp[1] * src[9];
	Result[3][0] -= tmp[4] * src[11] + tmp[0] * src[9] + tmp[3] * src[10];
	Result[3][1] = tmp[8] * src[11] + tmp[0] * src[8] + tmp[7] * src[10];
	Result[3][1] -= tmp[6] * src[10] + tmp[9] * src[11] + tmp[1] * src[8];
	Result[3][2] = tmp[6] * src[9] + tmp[11] * src[11] + tmp[3] * src[8];
	Result[3][2] -= tmp[10] * src[11] + tmp[2] * src[8] + tmp[7] * src[9];
	Result[3][3] = tmp[10] * src[10] + tmp[4] * src[8] + tmp[9] * src[9];
	Result[3][3] -= tmp[8] * src[9] + tmp[11] * src[10] + tmp[5] * src[8];
	/* calculate determinant */
	det = src[0] * Result[0][0] + src[1] * Result[0][1] + src[2] * Result[0][2] + src[3] * Result[0][3];
	/* calculate matrix inverse */
	det = 1.0f / det;

	mat4 FloatResult;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			FloatResult[i][j] = float(Result[i][j] * det);
		}
	}
	return FloatResult;
}

inline mat4 
transpose(mat4 const& m)
{
	mat4 result = mat4(0);

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result[j][i] = m.m[i][j];
		}
	}
	return result;
}

inline mat4 
rotationx(const float rad)
{
	mat4 result =
		mat4(
			vec4(1, 0, 0, 0),
			vec4(0, cos(rad), sin(rad), 0),
			vec4(0, -sin(rad), cos(rad), 0),
			vec4(0, 0, 0, 1));

	return result;
}

inline mat4 
rotationy(const float rad)
{
	mat4 result =
		mat4(
			vec4(cos(rad), 0, -sin(rad), 0),
			vec4(0, 1, 0, 0),
			vec4(sin(rad), 0, cos(rad), 0),
			vec4(0, 0, 0, 1));

	return result;
}

inline mat4 
rotationz(const float rad)
{
	mat4 result =
		mat4(
			vec4(cos(rad), -sin(rad), 0, 0),
			vec4(sin(rad), cos(rad), 0, 0),
			vec4(0, 0, 1, 0),
			vec4(0, 0, 0, 1));

	return transpose(result);
}

inline mat4 
rotationaxis(const vec3& v, const float rad)
{
	mat4 result =
		mat4(vec4(cos(rad) + v.x * v.x * (1 - cos(rad)), v.x * v.y * (1 - cos(rad)) - v.z * sin(rad), v.x * v.z * (1 - cos(rad)) + v.y * sin(rad), 0),
			vec4(v.y * v.x * (1 - cos(rad)) + v.z * sin(rad), cos(rad) + v.y * v.y * (1 - cos(rad)), v.y * v.z * (1 - cos(rad)) - v.x * sin(rad), 0),
			vec4(v.z * v.x * (1 - cos(rad)) - v.y * sin(rad), v.z * v.y * (1 - cos(rad)) + v.x * sin(rad), cos(rad) + v.z * v.z * (1 - cos(rad)), 0),
			vec4(0, 0, 0, 1)
		);

	return transpose(result);
}

inline mat4 
translate(const vec3& v)
{
	mat4 result =
		mat4(vec4(1, 0, 0, 0),
			vec4(0, 1, 0, 0),
			vec4(0, 0, 1, 0),
			vec4(v.x, v.y, v.z, 1)
		);

	return result;
}

inline mat4
scale(const vec3& v)
{
	return mat4(
		vec4(v.x, 0, 0, 0),
		vec4(0, v.y, 0, 0),
		vec4(0, 0, v.z, 0),
		vec4(0, 0, 0, 1)
	);
}

inline mat4
orthographic(const float left, const float right, const float top, const float bottom, const float near, const float far)
{
	mat4 result =
		mat4(vec4(2 / (right - left), 0, 0, 0),
			vec4(0, 2 / (top - bottom), 0, 0),
			vec4(0, 0, -2 / (far - near), 0),
			vec4(-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(far + near) / (far - near), 1));
	
	return result;
}

inline mat4 
perspective(const float FOVy, const float aspect, const float near, const float far)
{
	float top = tan(FOVy / 2) * near;
	float bottom = -top;
	float right = top * aspect;
	float left = bottom * aspect;
	mat4 result = mat4(
		vec4((2 * near) / (right - left), 0, 0, 0),
		vec4(0, (2 * near) / (top - bottom), 0, 0),
		vec4((right + left) / (right - left), (top + bottom) / (top - bottom), -(far + near) / (far - near), -1),
		vec4(0, 0, -(2 * far * near) / (far - near), 0)
	);

	return result;
}

inline mat4 
lookat(const vec3& eye, const vec3& at, const vec3& up)
{
	vec3 zaxis = normalize(at - eye);
	vec3 xaxis = normalize(cross(zaxis, up));
	vec3 yaxis = cross(xaxis, zaxis);
	zaxis = -zaxis;


	mat4 result = mat4(
		vec4(xaxis.x, yaxis.x, zaxis.x, 0),
		vec4(xaxis.y, yaxis.y, zaxis.y, 0),
		vec4(xaxis.z, yaxis.z, zaxis.z, 0),
		vec4(-dot(xaxis, eye), -dot(yaxis, eye), -dot(zaxis, eye), 1)
	);
	return result;
}

inline mat4
lookto(const vec3& eye, const vec3& dir, const vec3& up)
{
	vec3 zaxis = normalize(dir);
	vec3 xaxis = normalize(cross(zaxis, up));
	vec3 yaxis = cross(xaxis, zaxis);
	zaxis = -zaxis;


	mat4 result = mat4(
		vec4(xaxis.x, yaxis.x, zaxis.x, 0),
		vec4(xaxis.y, yaxis.y, zaxis.y, 0),
		vec4(xaxis.z, yaxis.z, zaxis.z, 0),
		vec4(-dot(xaxis, eye), -dot(yaxis, eye), -dot(zaxis, eye), 1)
	);
	return result;
}

inline float* 
value_ptr(mat4 m)
{
	return &(m[0].x);
}

const float PI = 3.14159265359f;