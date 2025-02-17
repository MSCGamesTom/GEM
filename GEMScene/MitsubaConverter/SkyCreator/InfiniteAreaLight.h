#ifndef H_INFAREALIGHT
#define H_INFAREALIGHT

#define _USE_MATH_DEFINES
#include <math.h>
#include "Colour.h"
#include "Vec3.h"
#include "HosekWilkie/ArHosekSkyModel.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define __STDC_LIB_EXT1__
#include "stb_image_write.h"

#define SUN_RADIUS_RAD (0.5358f / 360.0f) * (2.0f * M_PI)

class SkyParams
{
public:
	float turbidity;
	Vec3 sunDir;
	float sunElevation;
	float skyScale;
	float sunScale;
	float sunRadiusScale;
	float albedo;
	int width;
	int height;
	ArHosekSkyModelState *state[3];
	ArHosekSkyModelState *sunstate;
	SkyParams()
	{
		turbidity = 1.0f;
		sunDir = Vec3(0, 1, 0);
		skyScale = 1.0f;
		sunScale = 1.0f;
		sunRadiusScale = 1.0f;
		width = 1024;
		height = 512;
		albedo = 1.0f;
	}
	void init()
	{
		int i = 0;
		sunElevation = (M_PI * 0.5f) - acosf(sunDir.y);
		for (i = 0; i < 3; i++)
		{
			state[i] = arhosek_rgb_skymodelstate_alloc_init(turbidity, albedo, sunElevation);
		}
		sunstate = arhosekskymodelstate_alloc_init(sunElevation, turbidity, albedo);
	}
	int sunVisible(const Vec3 &wi)
	{
		if (acosf(Dot(wi, sunDir)) > (SUN_RADIUS_RAD * sunRadiusScale)) // Hack for now
		{
			return 0;
		}
		return 1;
	}
	inline void jenkinsMix(unsigned int &a, unsigned int &b, unsigned int &c)
	{
		a -= b; a -= c; a ^= (c>>13);
		b -= c; b -= a; b ^= (a<<8);
		c -= a; c -= b; c ^= (b>>13);
		a -= b; a -= c; a ^= (c>>12);
		b -= c; b -= a; b ^= (a<<16);
		c -= a; c -= b; c ^= (b>>5);
		a -= b; a -= c; a ^= (c>>3);
		b -= c; b -= a; b ^= (a<<10);
		c -= a; c -= b; c ^= (b>>15);
	}
	inline float rnd(float &v)
	{
		union FI
		{
			float f1;
			unsigned int u1;
		};
		FI fi;
		fi.f1 = v;
		unsigned int hashA = fi.u1;
		unsigned int hashB = 0x9e3779b9u;
		unsigned int hashC = 0x9e3779b9u;
		jenkinsMix(hashA, hashB, hashC);
		jenkinsMix(hashA, hashB, hashC);
		v = (float)hashC * 0.00000000023283064365386962890625f;//exp2(-32.0f);
		v = v - (int)v;
		return v;
	}
	Colour evalSun(float theta, float phi)
	{
		int i = 0;
		Vec3 wi;
		wi = Vec3(sinf(theta) * cosf(phi), cosf(theta), sinf(theta) * sinf(phi));
		if (sunVisible(wi) == 0)
		{
			return evalSky(theta, phi);
		}
		float gamma;
		gamma = acosf(fminf(Dot(wi, sunDir), 1.0f));
		Spectrum11 suns;
		for (i = 0; i < 11; i++)
		{
			suns.v[i] = arhosekskymodel_solar_radiance(sunstate, theta, gamma, Spectrum11::wavelength(i));
		}
		Colour col;
		col = suns.toColour();
		float L;
		L = col.Lum();
		col = Colour(L, L, L); // Hack to make the sun always white
		/*Colour col;
		int i = 0;
		float gamma;
		Vec3 wi;
		wi = Vec3(sinf(theta) * cosf(phi), cosf(theta), sinf(theta) * sinf(phi));
		if (sunVisible(wi) == 0)
		{
			return evalSky(theta, phi);
		}
		gamma = acosf(Dot(wi, sunDir));
		Spectrum11 suns;
		for (float wavelength = 320; wavelength <= 720; wavelength += 40)
		{
			suns.v[i] = arhosekskymodel_solar_radiance(sunstate, theta, gamma, wavelength);
			i++;
		}
		col = suns.toColour();*/
		return (col * sunScale / sunRadiusScale);
	}
	int nearSun(float theta, float phi)
	{
		Vec3 wi;
		wi = Vec3(sinf(theta) * cosf(phi), cosf(theta), sinf(theta) * sinf(phi));
		if (acosf(Dot(wi, sunDir)) > SUN_RADIUS_RAD * sunRadiusScale * 2.0f) // Hack for now
		{
			return 0;
		}
		return 1;
	}
	Colour evalSky(float theta, float phi)
	{
		float gamma;
		Vec3 wi;
		wi = Vec3(sinf(theta) * cosf(phi), cosf(theta), sinf(theta) * sinf(phi));
		//phi = sphericalPhi(wi);
		//gamma = cosf(theta) * cosf(sunElevation) + sinf(theta) * sinf(sunElevation) * cosf(phi - sunAzimuth);
		//gamma = acosf(gamma);
		gamma = acosf(Dot(wi, sunDir));
		Colour col;
		col.r = arhosek_tristim_skymodel_radiance(state[0], theta, gamma, 0) / 106.856980;
		col.g = arhosek_tristim_skymodel_radiance(state[1], theta, gamma, 1) / 106.856980;
		col.b = arhosek_tristim_skymodel_radiance(state[2], theta, gamma, 2) / 106.856980;
		col = col * skyScale;
		return col;
	}
};

class Sky
{
public:
	Colour* sky;
	int width;
	int height;
	void init(SkyParams params)
	{
		int i = 0;
		int n = 0;
		width = params.width;
		height = params.height;
		sky = new Colour[width * height];
		memset(sky, 0, width * height * sizeof(Colour));
		float theta;
		float phi;
		params.init();
		for (i = 0; i < (height / 2); i++)
		{
			theta = ((float)i / (float)height) * M_PI;
			for (n = 0; n < width; n++)
			{
				phi = ((float)n / (float)width) * (2.0f * M_PI);
				if (params.nearSun(theta, phi) == 1)
				{
					int v = 0;
					Colour sun;
					sun = Colour(0.0f, 0.0f, 0.0f);
					float ts;
					float ps;
					ts = (1.0f / (float)height) * M_PI;
					ps = (1.0f / (float)width) * (M_PI * 2.0f);
					float s1;
					s1 = theta;
					float s2;
					s2 = phi;
					for (v = 0; v < 100; v++)
					{
						s1 = params.rnd(s1);
						s2 = params.rnd(s2);
						sun = sun + params.evalSun(theta + (ts * s1), phi + (ps * s2));
					}
					sun = sun / (float)100;
					sky[(i * width) + n] = sky[(i * width) + n] + sun;
				} else
				{
					sky[(i * width) + n] = params.evalSky(theta, phi);
				}
			}
		}
		for (i = (height / 2); i < height; i++)
		{
			for (n = 0; n < width; n++)
			{
				sky[(i * width) + n] = sky[(((height / 2) - 1) * width) + n];
			}
		}
	}
	void save(const char* filename)
	{
		stbi_write_hdr(filename, width, height, 3, (float*)sky);
	}
};

#endif
