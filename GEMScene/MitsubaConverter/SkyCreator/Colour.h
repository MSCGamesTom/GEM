#ifndef H_COLOUR
#define H_COLOUR

#include <cstdio>
#include <cmath>
#include <memory.h>
using namespace std;

#define FINLINE inline

class Colour
{
public:

	union
	{
		float c[3];
		struct { float r, g, b; };
	};

    Colour() { c[0] = 0; c[1] = 0; c[2] = 0; }
    Colour(float px, float py, float pz) { c[0] = px; c[1] = py; c[2] = pz; }
	Colour(float a[3]) { c[0] = a[0]; c[1] = a[1]; c[2] = a[2]; }
    Colour(float px, float py) { c[0] = px; c[1] = py; c[2] = 0; }
    Colour(const Colour &pVec) { c[0] = pVec.c[0]; c[1] = pVec.c[1]; c[2] = pVec.c[2]; }
	Colour(const float col) { c[0] = col; c[1] = col; c[2] = col; }
	Colour(int R, int G, int B) { c[0] = float(R) / 255.0f; c[1] = float(G) / 255.0f; c[2] = float(B) / 255.0f; }

    Colour operator = (const Colour pVec) { return Colour(c[0] = pVec.c[0], c[1] = pVec.c[1], c[2] = pVec.c[2]); }
    Colour operator = (float *ptr) { return Colour(c[0] = ptr[0], c[1] = ptr[1], c[2] = ptr[2]); }

    bool operator==(const Colour pVec) const { return (c[0] == pVec[0] && c[1] == pVec[1] && c[2] == pVec[2]); }
    bool operator==(float *pVec) { return (c[0] == pVec[0] && c[1] == pVec[1] && c[2] == pVec[2]); }

    bool operator!=(Colour &pVec) { return (c[0] != pVec[0] || c[1] != pVec[1] || c[2] != pVec[2]); }
    bool operator!=(float *pVec)  { return (c[0] != pVec[0] || c[1] != pVec[1] || c[2] != pVec[2]); }

	Colour Exp() { return Colour(expf(c[0]), expf(c[1]), expf(c[2]));  }
	Colour Ln() { return Colour(logf(c[0]), logf(c[1]), logf(c[2])); }

	Colour operator+=(Colour pVec);
    Colour operator+=(float val);
	Colour operator-=(Colour pVec);
	Colour operator-=(float val);
	Colour operator*=(Colour pVec);
	Colour operator*=(float val);
	Colour operator/=(Colour pVec);
	Colour operator/=(float val);

    Colour operator+(Colour const pVec) const { return Colour(c[0] + pVec.c[0], c[1] + pVec.c[1], c[2] + pVec.c[2]); }
    Colour operator+(float const val) const { return Colour(c[0] + val, c[1] + val, c[2] + val); }
    Colour operator-(Colour const pVec) const { return Colour(c[0] - pVec.c[0], c[1] - pVec.c[1], c[2] - pVec.c[2]); }
    Colour operator-(float const val) const { return Colour(c[0] - val, c[1] - val, c[2] - val); }
	Colour operator-() const { return Colour(-c[0], -c[1], -c[2]); }
    Colour operator*(Colour const pVec) const { return Colour(c[0] * pVec.c[0], c[1] * pVec.c[1], c[2] * pVec.c[2]); }
    Colour operator*(float const val) const { return Colour(c[0] * val, c[1] * val, c[2] * val); }
    Colour operator/(Colour const pVec) const { return Colour(c[0] / pVec.c[0], c[1] / pVec.c[1], c[2] / pVec.c[2]); }
    Colour operator/(float const val) const { return Colour(c[0] / val, c[1] / val, c[2] / val); }

	void Clear(void) {c[0] = c[1] = c[2] = 0;}
	float Normalize(void);

	float Lum() const { return 0.212671f * c[0] + 0.715160f * c[1] + 0.072169f * c[2]; }

    void Abs()
    {
        c[0] = abs(c[0]); c[1] = abs(c[1]); c[2] = abs(c[2]);
    }

	void Invert() { c[0] = -c[0]; c[1] = -c[1]; c[2] = -c[2]; }

	void Set(float x, float y, float z) {c[0] = x; c[1] = y; c[2] = z;}

	float X() {return c[0];}
	float Y() {return c[1];}
	float Z() {return c[2];}

	void X(float nx) {c[0] = nx;}
	void Y(float ny) {c[1] = ny;}
	void Z(float nz) {c[2] = nz;}

	const float &operator[](int ndx) const {return c[ndx];}
	float &operator[](int ndx) {return c[ndx];}
	operator float*(void) {return c;}

	void Clamp(float min, float max);
	void maxZero()
	{
		r = r > 0 ? r : 0;
		g = g > 0 ? g : 0;
		b = b > 0 ? b : 0;
	}

	float *Get(void) {return c;}

	void Print() const { printf("%.6f %.6f %.6f\n", c[0], c[1], c[2]); }

	bool isBlack() { return (c[0] <= 0.0f && c[1] <= 0.0f && c[2] <= 0.0f); }
	bool isNan() { return (c[0] != c[0] || c[1] != c[1] || c[2] != c[2]); }

	Colour Pow(const Colour e) const
	{
		return Colour(c[0] > 0.0f ? powf(c[0], e[0]) : 0.0f,
					  c[1] > 0.0f ? powf(c[1], e[1]) : 0.0f,
					  c[2] > 0.0f ? powf(c[2], e[2]) : 0.0f);
	}

	Colour Pow(const float e) const
	{
		return Colour(c[0] > 0.0f ? powf(c[0], e) : 0.0f,
					  c[1] > 0.0f ? powf(c[1], e) : 0.0f,
					  c[2] > 0.0f ? powf(c[2], e) : 0.0f);
	}

	float Avg() const
	{
		return ((c[0] + c[1] + c[2]) * 0.3333333f);
	}

	float Max()
	{
		return ((r > g && r > b) ? r : (g > b ? g : b));
	}

	Colour Max(Colour &col)
	{
		return Colour(r > col.r ? r : col.r, g > col.g ? g : col.g, b > col.b ? b : col.b);
	}

	int allPositive()
	{
		if (r < 0 || g < 0 || b < 0)
		{
			return 0;
		}
		return 1;
	}

	Colour sqrt()
	{
		Colour col;
		col.r = sqrtf(r);
		col.g = sqrtf(g);
		col.b = sqrtf(b);
		return col;
	}
};

// -------------------------------------------------------------------------------------- //
// ---------------------------- FUNCTIONS ----------------------------------------------- //
// -------------------------------------------------------------------------------------- //

FINLINE Colour Colour::operator+=(Colour pVec)
{
   c[0] += pVec.c[0];
   c[1] += pVec.c[1];
   c[2] += pVec.c[2];
   return *this;
}


FINLINE Colour Colour::operator+=(float val)
{
   c[0] += val;
   c[1] += val;
   c[2] += val;
   return *this;
}


FINLINE Colour Colour::operator-=(Colour pVec)
{
   c[0] -= pVec.c[0];
   c[1] -= pVec.c[1];
   c[2] -= pVec.c[2];
   return *this;
}


FINLINE Colour Colour::operator-=(float val)
{
   c[0] -= val;
   c[1] -= val;
   c[2] -= val;
   return *this;
}


FINLINE Colour Colour::operator*=(Colour pVec)
{
   c[0] *= pVec.c[0];
   c[1] *= pVec.c[1];
   c[2] *= pVec.c[2];
   return *this;
}


FINLINE Colour Colour::operator*=(float val)
{
   c[0] *= val;
   c[1] *= val;
   c[2] *= val;
   return *this;
}


FINLINE Colour Colour::operator/=(Colour pVec)
{
   c[0] /= pVec.c[0];
   c[1] /= pVec.c[1];
   c[2] /= pVec.c[2];
   return *this;
}


FINLINE Colour Colour::operator/=(float val)
{
   c[0] /= val;
   c[1] /= val;
   c[2] /= val;
   return *this;
}


FINLINE float Colour::Normalize(void)
{
	float length, len = 0;

	length = sqrtf(c[0] * c[0] + c[1] * c[1] + c[2] * c[2]);

	if (length == 0) return -1.0;

	len = 1.0f / length;

	c[0] *= len;
	c[1] *= len;
	c[2] *= len;

	return length;
}

FINLINE void Colour::Clamp(float min, float max)
{
   if (c[0] > max || c[0] < min)
      c[0] = 0;

   if (c[1] > max || c[1] < min)
      c[1] = 0;

   if (c[2] > max || c[2] < min)
      c[2] = 0;
}

class Spectrum11
{
public:
	float v[11];
	Spectrum11()
	{
		memset(v, 0, 11 * sizeof(float));
	}
	static float wavelength(int ID)
	{
		float spectralStep;
		spectralStep = (700.0f - 390.0f) / 11.0f;
		return (390.0f + ((float)ID * spectralStep));
	}
	float xFit_1931( float wave )
	{
		float t1 = (wave-442.0f)*((wave<442.0f)?0.0624f:0.0374f);
		float t2 = (wave-599.8f)*((wave<599.8f)?0.0264f:0.0323f);
		float t3 = (wave-501.1f)*((wave<501.1f)?0.0490f:0.0382f);
		return 0.362f*expf(-0.5f*t1*t1) + 1.056f*expf(-0.5f*t2*t2) - 0.065f*expf(-0.5f*t3*t3);
	}
	float yFit_1931( float wave )
	{
		float t1 = (wave-568.8f)*((wave<568.8f)?0.0213f:0.0247f);
		float t2 = (wave-530.9f)*((wave<530.9f)?0.0613f:0.0322f);
		return 0.821f*exp(-0.5f*t1*t1) + 0.286f*expf(-0.5f*t2*t2);
	}
	float zFit_1931( float wave )
	{
		float t1 = (wave-437.0f)*((wave<437.0f)?0.0845f:0.0278f);
		float t2 = (wave-459.0f)*((wave<459.0f)?0.0385f:0.0725f);
		return 1.217f*exp(-0.5f*t1*t1) + 0.681f*expf(-0.5f*t2*t2);
	}
	Colour toColour(int ensureVisible = 1)
	{
		int i = 0;
		float X;
		float Y;
		float Z;
		X = 0;
		Y = 0;
		Z = 0;
		float scale;
		scale = 1.0f / 106.85665f; // Integral of Y
		for (i = 0; i < 11; i++)
		{
			X = X + (xFit_1931(wavelength(i)) * v[i]);
			Y = Y + (yFit_1931(wavelength(i)) * v[i]);
			Z = Z + (zFit_1931(wavelength(i)) * v[i]);
		}
		X = X * scale;
		Y = Y * scale;
		Z = Z * scale;
		Colour col;
		col.r = (X * 3.2404542f) + (Y * -1.5371385f) + (Z * -0.4985314f);
		col.g = (X * -0.9692660f) + (Y * 1.8760108f) + (Z * 0.0415560f);
		col.b = (X * 0.0556434f) + (Y * -0.2040259f) + (Z * 1.0572252f);
		if (ensureVisible == 1)
		{
			float w;
			w = 0 < col.r ? 0 : col.r;
			w = w < col.g ? w : col.g;
			w = w < col.b ? w : col.b;
			col.r = col.r - w;
			col.g = col.g - w;
			col.b = col.b - w;
		}
		return col;
	}
};

class Colours
{
public:

	static const Colour black,
					    white,
						red,
						green,
						blue,
						grey;
};

#endif
