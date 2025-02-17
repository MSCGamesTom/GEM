#ifndef H_VEC3
#define H_VEC3

#include <cstdio>
#include <limits>
#include <cmath>
#include "Colour.h"

struct Vec3
{
	union
	{
		float v[4];
		struct { float x, y, z, w; };
	};

	Vec3() { v[0] = -1.0f; v[1] = -1.0f; v[2] = -1.0f; v[3] = -1.0f; }
	Vec3(float x, float y, float z) { v[0] = x; v[1] = y; v[2] = z; v[3] = 0.0f; }
	Vec3(const Vec3 &vec) { v[0] = vec.v[0]; v[1] = vec.v[1]; v[2] = vec.v[2]; v[3] = 0.0f; }
	Vec3(const float f) { v[0] = f; v[1] = f; v[2] = f; v[3] = 0.0f; }

    Vec3 operator =(const Vec3 &pVec) { return Vec3(v[0] = pVec.v[0], v[1] = pVec.v[1], v[2] = pVec.v[2]); }
	Vec3 operator =(const Colour &pVec) { return Vec3(v[0] = pVec.c[0], v[1] = pVec.c[1], v[2] = pVec.c[2]); }

	Vec3 operator-(const Vec3 &pVec) const { return Vec3(v[0] - pVec.v[0], v[1] - pVec.v[1], v[2] - pVec.v[2]); }
    Vec3 operator+(const Vec3 &pVec) const { return Vec3(v[0] + pVec.v[0], v[1] + pVec.v[1], v[2] + pVec.v[2]); }
	Vec3 operator*(const Vec3 &pVec) const { return Vec3(v[0] * pVec.v[0], v[1] * pVec.v[1], v[2] * pVec.v[2]); }
	Vec3 operator/(const Vec3 &pVec) const { return Vec3(v[0] / pVec.v[0], v[1] / pVec.v[1], v[2] / pVec.v[2]); }

	Vec3 operator+(const Vec3 *pVec) const { return Vec3(v[0] + pVec->v[0], v[1] + pVec->v[1], v[2] + pVec->v[2]); }

    Vec3 operator-(const float val) const { return Vec3(v[0] - val, v[1] - val, v[2] - val); }
    Vec3 operator+(const float val) const { return Vec3(v[0] + val, v[1] + val, v[2] + val); }
    Vec3 operator*(const float val) const { return Vec3(v[0] * val, v[1] * val, v[2] * val); }
    Vec3 operator/(const float val) const { return Vec3(v[0] / val, v[1] / val, v[2] / val); }

	Vec3 &operator-=(const Vec3 &pVec) { v[0] -= pVec.v[0]; v[1] -= pVec.v[1]; v[2] -= pVec.v[2]; return *this; }
	Vec3 &operator+=(const Vec3 &pVec) { v[0] += pVec.v[0]; v[1] += pVec.v[1]; v[2] += pVec.v[2]; return *this; }
	Vec3 &operator*=(const Vec3 &pVec) { v[0] *= pVec.v[0]; v[1] *= pVec.v[1]; v[2] *= pVec.v[2]; return *this; }
	Vec3 &operator/=(const Vec3 &pVec) { v[0] /= pVec.v[0]; v[1] /= pVec.v[1]; v[2] /= pVec.v[2]; return *this; }

	Vec3 &operator*=(const float val) { v[0] *= val; v[1] *= val; v[2] *= val; return *this; }

	Vec3 operator-() const { return Vec3(-v[0], -v[1], -v[2]); }

	FINLINE float Dot(const Vec3 &pVec) const { return v[0] * pVec.v[0] + v[1] * pVec.v[1] + v[2] * pVec.v[2]; }

	FINLINE void Cross(const Vec3 &p, const Vec3 &q)
	{
		v[0] = p.v[1] * q.v[2] - p.v[2] * q.v[1];
		v[1] = p.v[2] * q.v[0] - p.v[0] * q.v[2];
		v[2] = p.v[0] * q.v[1] - p.v[1] * q.v[0];
	}

	FINLINE Vec3& CrossVec(const Vec3 &q)
	{
		v[0] = v[1] * q.v[2] - v[2] * q.v[1];
		v[1] = v[2] * q.v[0] - v[0] * q.v[2];
		v[2] = v[0] * q.v[1] - v[1] * q.v[0];
		return *this;
	}

	FINLINE void Normalize(void)
	{
		const float len = 1.0f / sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
		v[0] = v[0] * len;
		v[1] = v[1] * len;
		v[2] = v[2] * len;
	}

	FINLINE Vec3& NormalizeVec(void)
	{
		const float len = 1.0f / sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
		v[0] = v[0] * len;
		v[1] = v[1] * len;
		v[2] = v[2] * len;
		return *this;
	}

	FINLINE float Magnitude() const
	{
		return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	}

	FINLINE float MagnitudeSquare() const
	{
		return (v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	}

	FINLINE const float Normalize_GetLength()
	{
		const float length = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]),
					len = 1.0f / length;

		v[0] *= len;
		v[1] *= len;
		v[2] *= len;

		return length;
	}

	FINLINE void RotateX(const float amnt)
	{
		const float s = sinf(amnt),
				    c = cosf(amnt),
					y = v[1],
					z = v[2];

		v[1] = (y * c) - (z * s);
		v[2] = (y * s) + (z * c);
	}

	FINLINE void RotateY(const float amnt)
	{
		const float s = sinf(amnt),
				    c = cosf(amnt),
					x = v[0],
					z = v[2];

		v[0] = (x * c) + (z * s);
		v[2] = (z * c) - (x * s);
	}

	FINLINE void RotateZ(const float amnt)
	{
		const float s = sinf(amnt),
				    c = cosf(amnt),
					x = v[0],
					y = v[1];

		v[0] = (x * c) - (y * s);
		v[1] = (x * s) + (y * c);
	}

	FINLINE Vec3 Pow(const Vec3 &e) const
	{
		return Vec3(v[0] > 0.0f ? powf(v[0], e[0]) : 0.0f,
				    v[1] > 0.0f ? powf(v[1], e[1]) : 0.0f,
					v[2] > 0.0f ? powf(v[2], e[2]) : 0.0f);
	}

	FINLINE Vec3 Pow(const float e) const
	{
		return Vec3(v[0] > 0.0f ? powf(v[0], e) : 0.0f,
				    v[1] > 0.0f ? powf(v[1], e) : 0.0f,
					v[2] > 0.0f ? powf(v[2], e) : 0.0f);
	}

	const float &operator[](int ndx) const { return v[ndx]; }
	float &operator[](int ndx) { return v[ndx]; }
	operator float*(void) {return v;}

	void Print() const { printf("%.6f %.6f %.6f", v[0], v[1], v[2]); }
};

FINLINE Vec3 Min(const Vec3 &v1, const Vec3 &v2)
{
	return Vec3(fminf(v1.v[0], v2.v[0]),
		        fminf(v1.v[1], v2.v[1]),
			    fminf(v1.v[2], v2.v[2]));
}

FINLINE Vec3 Max(const Vec3 &v1, const Vec3 &v2)
{
	return Vec3(fmaxf(v1.v[0], v2.v[0]),
	            fmaxf(v1.v[1], v2.v[1]),
			    fmaxf(v1.v[2], v2.v[2]));
}

FINLINE Vec3 Cross(const Vec3 &v1, const Vec3 &v2)
{
	return Vec3(v1.v[1] * v2.v[2] - v1.v[2] * v2.v[1],
				v1.v[2] * v2.v[0] - v1.v[0] * v2.v[2],
				v1.v[0] * v2.v[1] - v1.v[1] * v2.v[0]);
}

FINLINE float Dot(const Vec3 &v1, const Vec3 &v2)
{
	return v1.v[0] * v2.v[0] + v1.v[1] * v2.v[1] + v1.v[2] * v2.v[2];
}

class VecIndex
{
public:

	VecIndex() {}
	VecIndex(int x, int y, int z) { Set(x,y,z); }
	inline void Set(int x, int y, int z)
	{
		index[0] = x; index[1] = y; index[2] = z;
	}

	int index[3];
};

#endif
