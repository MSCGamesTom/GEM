#pragma once

#include <vector>
#include <string>
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/pbrmaterial.h"
#include <iostream>
#include <fstream>

#define TYPE_MESH 1
#define TYPE_ANIMATION 2

#define SQ(x) ((x) * (x))

class Vec2
{
public:
	float u;
	float v;
	Vec2()
	{
		u = 0;
		v = 0;
	}
	void write(FILE* file)
	{
		fwrite(&u, sizeof(float), 1, file);
		fwrite(&v, sizeof(float), 1, file);
	}
};

class Vec3
{
public:
	float x;
	float y;
	float z;
	Vec3()
	{
		x = 0;
		y = 0;
		z = 0;
	}
	Vec3(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}
	Vec3 operator+(Vec3 p) { return Vec3(x + p.x, y + p.y, z + p.z); }
	Vec3 operator-(Vec3 p) { return Vec3(x - p.x, y - p.y, z - p.z); }
	Vec3 operator*(float v) { return Vec3(x * v, y * v, z * v); }
	void write(FILE* file)
	{
		fwrite(&x, sizeof(float), 1, file);
		fwrite(&y, sizeof(float), 1, file);
		fwrite(&z, sizeof(float), 1, file);
	}
};

class Matrix
{
public:
	float m[16];
	Matrix()
	{
		identity();
	}
	void identity()
	{
		memset(m, 0, 16 * sizeof(float));
		m[0] = 1.0f;
		m[5] = 1.0f;
		m[10] = 1.0f;
		m[15] = 1.0f;
	}
	void transpose()
	{
		aiMatrix4x4 mat(m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10], m[11], m[12], m[13], m[14], m[15]);
		mat.Transpose();
		fromAssimp(mat);
	}
	void fromAssimp(aiMatrix4x4 mat)
	{
		memcpy(m, &mat.a1, 16 * sizeof(float));
	}
	float det()
	{
		aiMatrix4x4 mat(m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10], m[11], m[12], m[13], m[14], m[15]);
		return mat.Determinant();
	}
	Matrix inverse()
	{
		aiMatrix4x4 mat(m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10], m[11], m[12], m[13], m[14], m[15]);
		mat.Inverse();
		Matrix m1;
		m1.fromAssimp(mat);
		return m1;
	}
	float& operator[](int index)
	{
		return m[index];
	}
	void write(FILE* file)
	{
		fwrite(m, sizeof(float), 16, file);
	}
};

class Quaternion
{
public:
	float q[4];
	Quaternion()
	{
		q[0] = 0;
		q[1] = 0;
		q[2] = 0;
		q[3] = 0;
	}
	Quaternion(float _x, float _y, float _z, float _w)
	{
		q[0] = _x;
		q[1] = _y;
		q[2] = _z;
		q[3] = _w;
	}
	float norm()
	{
		return sqrtf(SQ(q[0]) + SQ(q[1]) + SQ(q[2]) + SQ(q[3]));
	}
	void Normalize()
	{
		float n = 1.0f / sqrtf(SQ(q[0]) + SQ(q[1]) + SQ(q[2]) + SQ(q[3]));
		q[0] *= n;
		q[1] *= n;
		q[2] *= n;
		q[3] *= n;
	}
	void Conjugate()
	{
		q[0] = -q[0];
		q[1] = -q[1];
		q[2] = -q[2];
	}
	void invert()
	{
		Conjugate();
		Normalize();
	}
	Quaternion operator*(Quaternion q1)
	{
		Quaternion v;
		v.q[0] = ((q[3] * q1.q[0]) + (q[0] * q1.q[3]) + (q[1] * q1.q[2]) - (q[2] * q1.q[1]));
		v.q[1] = ((q[3] * q1.q[1]) - (q[0] * q1.q[2]) + (q[1] * q1.q[3]) + (q[2] * q1.q[0]));
		v.q[2] = ((q[3] * q1.q[2]) + (q[0] * q1.q[1]) - (q[1] * q1.q[0]) + (q[2] * q1.q[3]));
		v.q[3] = ((q[3] * q1.q[3]) - (q[0] * q1.q[0]) - (q[1] * q1.q[1]) - (q[2] * q1.q[2]));
		return v;
	}
	Matrix toMatrix()
	{
		float wx, wy, wz, xx, yy, yz, xy, xz, zz;
		// adapted from Shoemake
		xx = q[0] * q[0];
		xy = q[0] * q[1];
		xz = q[0] * q[2];
		yy = q[1] * q[1];
		zz = q[2] * q[2];
		yz = q[1] * q[2];

		wx = q[3] * q[0];
		wy = q[3] * q[1];
		wz = q[3] * q[2];

		Matrix matrix;

		matrix[0] = 1.0f - 2.0f * (yy + zz);
		matrix[4] = 2.0f * (xy - wz);
		matrix[8] = 2.0f * (xz + wy);
		matrix[12] = 0.0;

		matrix[1] = 2.0f * (xy + wz);
		matrix[5] = 1.0f - 2.0f * (xx + zz);
		matrix[9] = 2.0f * (yz - wx);
		matrix[13] = 0.0;

		matrix[2] = 2.0f * (xz - wy);
		matrix[6] = 2.0f * (yz + wx);
		matrix[10] = 1.0f - 2.0f * (xx + yy);
		matrix[14] = 0.0;

		matrix[3] = 0;
		matrix[7] = 0;
		matrix[11] = 0;
		matrix[15] = 1;
	}
	void rotateAboutAxis(Vec3 pt, float angle, Vec3 axis)
	{
		Quaternion q1, p, qinv;

		q1.q[0] = sinf(0.5f * angle) * axis.x;
		q1.q[1] = sinf(0.5f * angle) * axis.y;
		q1.q[2] = sinf(0.5f * angle) * axis.z;
		q1.q[3] = cosf(0.5f * angle);

		p.q[0] = pt.x;
		p.q[1] = pt.y;
		p.q[2] = pt.z;
		p.q[3] = 0;

		qinv = q1;
		qinv.invert();

		q1 = q1 * p;
		q1 = q1 * qinv;
		q[0] = q1.q[0];
		q[1] = q1.q[1];
		q[2] = q1.q[2];
		q[3] = q1.q[3];
	}
	static Quaternion slerp(Quaternion q1, Quaternion q2, float lambda)
	{
		Quaternion qr;
		float dp = q1.q[0] * q2.q[0] + q1.q[1] * q2.q[1] + q1.q[2] * q2.q[2] + q1.q[3] * q2.q[3];
		float theta, st, sut, sout, coeff1, coeff2;

		// algorithm adapted from Shoemake's paper
		lambda = lambda / 2.0f;

		theta = acosf(dp);
		if (theta < 0.0) theta = -theta;

		st = sinf(theta);
		sut = sinf(lambda * theta);
		sout = sinf((1 - lambda) * theta);
		coeff1 = sout / st;
		coeff2 = sut / st;

		qr.q[0] = coeff1 * q1.q[0] + coeff2 * q2.q[0];
		qr.q[1] = coeff1 * q1.q[1] + coeff2 * q2.q[1];
		qr.q[2] = coeff1 * q1.q[2] + coeff2 * q2.q[2];
		qr.q[3] = coeff1 * q1.q[3] + coeff2 * q2.q[3];

		qr.Normalize();
		return qr;
	}
	void write(FILE* file)
	{
		fwrite(q, sizeof(float), 4, file);
	}
};

class Vertex
{
public:
	Vec3 p;
	Vec3 n;
	Vec3 tangent;
	Vec2 uv;
	unsigned int bones[4];
	float boneWeights[4];
	Vertex()
	{
		memset(bones, 0, 4 * sizeof(unsigned int));
		memset(boneWeights, 0, 4 * sizeof(float));
	}
	void addBone(int ID, float weight)
	{
		for (int i = 0; i < 4; i++)
		{
			if (boneWeights[i] == 0)
			{
				bones[i] = ID;
				boneWeights[i] = weight;
				break;
			}
		}
	}
	void writeStatic(FILE* file)
	{
		p.write(file);
		n.write(file);
		tangent.write(file);
		uv.write(file);
	}
	void writeAnimated(FILE* file)
	{
		p.write(file);
		n.write(file);
		tangent.write(file);
		uv.write(file);
		fwrite(bones, sizeof(unsigned int), 4, file);
		fwrite(boneWeights, sizeof(float), 4, file);
	}
};

static void writeString(std::string str, FILE* file)
{
	int l = str.length();
	fwrite(&l, sizeof(int), 1, file);
	fwrite(str.c_str(), sizeof(char), l, file);
}

static std::string loadS(std::ifstream& file)
{
	int l = 0;
	file.read(reinterpret_cast<char*>(&l), sizeof(int));
	char* buffer = new char[l + 1];
	memset(buffer, 0, l * sizeof(char));
	file.read(buffer, l * sizeof(char));
	buffer[l] = 0;
	std::string str(buffer);
	delete[] buffer;
	return str;
}

Vec3 loadV(std::ifstream& file)
{
	Vec3 v;
	file.read(reinterpret_cast<char*>(&v), sizeof(Vec3));
	return v;
}

Matrix loadM(std::ifstream& file)
{
	Matrix mat;
	file.read(reinterpret_cast<char*>(&mat.m), sizeof(float) * 16);
	return mat;
}

Quaternion loadQ(std::ifstream& file)
{
	Quaternion q;
	file.read(reinterpret_cast<char*>(&q.q), sizeof(float) * 4);
	return q;
}

struct Bone
{
	std::string name;
	Matrix offset;
	int parentIndex;
	void write(FILE* file)
	{
		writeString(name, file);
		offset.write(file);
		fwrite(&parentIndex, sizeof(int), 1, file);
	}
};

struct BoneHelper
{
	std::string name;
	std::string upName;
};

struct Skeleton
{
	std::vector<Bone> bones;
	Matrix rootTransform;
	Matrix globalInverse;
	int findBone(std::string name)
	{
		for (int i = 0; i < bones.size(); i++)
		{
			if (bones[i].name == name)
			{
				return i;
			}
		}
		return -1;
	}
	void write(FILE* file)
	{
		int n = bones.size();
		fwrite(&n, sizeof(int), 1, file);
		for (int i = 0; i < bones.size(); i++)
		{
			bones[i].write(file);
		}
		//rootTransform.write(file);
		globalInverse.write(file);
	}
};

struct Frame
{
	std::vector<Vec3> positions;
	std::vector<Quaternion> rotations;
	std::vector<Vec3> scales;
	std::vector<int> validp;
	std::vector<int> validr;
	std::vector<int> valids;
	void write(FILE* file)
	{
		for (int i = 0; i < positions.size(); i++)
		{
			positions[i].write(file);
		}
		for (int i = 0; i < rotations.size(); i++)
		{
			rotations[i].write(file);
		}
		for (int i = 0; i < scales.size(); i++)
		{
			scales[i].write(file);
		}
	}
};

struct Animation // This holds rescaled times
{
	std::string name;
	std::vector<Frame> frames;
	std::vector<float> times;
	float ticksPerSecond;
	Vec3 interpolate(Vec3 p1, Vec3 p2, float v, float s, float l)
	{
		float t = (v - s) / (l - s);
		return ((p1 * (1.0f - t)) + (p2 * t));
	}
	Quaternion interpolate(Quaternion q1, Quaternion q2, float v, float s, float l)
	{
		float t = (v - s) / (l - s);
		return Quaternion::slerp(q1, q2, t);
	}
	int findNextValid(int vindex, int index, int bone)
	{
		for (int i = index + 1; i < frames.size(); i++)
		{
			if (vindex == 0 && frames[i].validp[bone] == 1)
			{
				return i;
			}
			if (vindex == 1 && frames[i].validr[bone] == 1)
			{
				return i;
			}
			if (vindex == 2 && frames[i].valids[bone] == 1)
			{
				return i;
			}
		}
		return -1;
	}
	void fix()
	{
		for (int i = 1; i < frames.size(); i++)
		{
			for (int n = 0; n < frames[0].positions.size(); n++)
			{
				if (frames[i].validp[n] == 0)
				{
					int v = findNextValid(0, i, n);
					if (v > -1)
					{
						frames[i].positions[n] = interpolate(frames[i - 1].positions[n], frames[v].positions[n], times[i], times[i - 1], times[v]);
					} else
					{
						frames[i].positions[n] = frames[i - 1].positions[n];
					}
				}
			}
		}
		for (int i = 1; i < frames.size(); i++)
		{
			for (int n = 0; n < frames[0].rotations.size(); n++)
			{
				if (frames[i].validr[n] == 0)
				{
					int v = findNextValid(1, i, n);
					if (v > -1)
					{
						frames[i].rotations[n] = interpolate(frames[i - 1].rotations[n], frames[v].rotations[n], times[i], times[i - 1], times[v]);
					} else
					{
						frames[i].rotations[n] = frames[i - 1].rotations[n];
					}
				}
			}
		}
		for (int i = 1; i < frames.size(); i++)
		{
			for (int n = 0; n < frames[0].scales.size(); n++)
			{
				if (frames[i].valids[n] == 0)
				{
					int v = findNextValid(2, i, n);
					if (v > -1)
					{
						frames[i].scales[n] = interpolate(frames[i - 1].scales[n], frames[v].scales[n], times[i], times[i - 1], times[v]);
					} else
					{
						frames[i].scales[n] = frames[i - 1].scales[n];
					}
				}
			}
		}
	}
	float duration()
	{
		return (ticksPerSecond * (float)frames.size());
	}
	void write(FILE* file)
	{
		writeString(name, file);
		int l = frames.size();
		fwrite(&l, sizeof(int), 1, file);
		fwrite(&ticksPerSecond, sizeof(float), 1, file);
		for (int i = 0; i < frames.size(); i++)
		{
			frames[i].write(file);
		}
		for (int i = 0; i < frames.size(); i++)
		{
			//fwrite(&times[i], sizeof(float), 1, file);
		}
	}
};

#define MATERIALDESC_STRING 0
#define MATERIALDESC_COLOUR 1
#define MATERIALDESC_FLOAT 2

void writeStringWithPath(std::string path, std::string name, std::string filename, FILE* file)
{
	if (path != "")
	{
		std::string fullFilename = path + filename;
		writeString(name, file);
		writeString(fullFilename, file);
		return;
	}
	writeString(name, file);
	writeString(filename, file);
}

std::string stripPath(std::string fullFilename)
{
	if (fullFilename.rfind("\\") != std::string::npos)
	{
		std::string filename(fullFilename.substr(fullFilename.rfind("\\") + 1));
		return filename;
	}
	if (fullFilename.rfind("/") != std::string::npos)
	{
		std::string filename(fullFilename.substr(fullFilename.rfind("/") + 1));
		return filename;
	}
	return fullFilename;
}

class Paths
{
public:
	std::string modelSavePath;
	std::string texturesPath;
	std::string modelName;
};

template<typename T>
T& use()
{
	static T t;
	return t;
}

class BitmapWriter
{
public:
	unsigned char* createBitmapFileHeader(int height, int stride)
	{
		int fileSize = 14 + 40 + (stride * height);
		static unsigned char fileHeader[] = {
			0,0,     /// signature
			0,0,0,0, /// image file size in bytes
			0,0,0,0, /// reserved
			0,0,0,0, /// start of pixel array
		};
		fileHeader[0] = (unsigned char)('B');
		fileHeader[1] = (unsigned char)('M');
		fileHeader[2] = (unsigned char)(fileSize);
		fileHeader[3] = (unsigned char)(fileSize >> 8);
		fileHeader[4] = (unsigned char)(fileSize >> 16);
		fileHeader[5] = (unsigned char)(fileSize >> 24);
		fileHeader[10] = (unsigned char)(14 + 40);
		return fileHeader;
	}
	unsigned char* createBitmapInfoHeader(int width, int height)
	{
		static unsigned char infoHeader[] = {
			0,0,0,0, /// header size
			0,0,0,0, /// image width
			0,0,0,0, /// image height
			0,0,     /// number of color planes
			0,0,     /// bits per pixel
			0,0,0,0, /// compression
			0,0,0,0, /// image size
			0,0,0,0, /// horizontal resolution
			0,0,0,0, /// vertical resolution
			0,0,0,0, /// colors in color table
			0,0,0,0, /// important color count
		};
		infoHeader[0] = (unsigned char)(40);
		infoHeader[4] = (unsigned char)(width);
		infoHeader[5] = (unsigned char)(width >> 8);
		infoHeader[6] = (unsigned char)(width >> 16);
		infoHeader[7] = (unsigned char)(width >> 24);
		infoHeader[8] = (unsigned char)(height);
		infoHeader[9] = (unsigned char)(height >> 8);
		infoHeader[10] = (unsigned char)(height >> 16);
		infoHeader[11] = (unsigned char)(height >> 24);
		infoHeader[12] = (unsigned char)(1);
		infoHeader[14] = (unsigned char)(3 * 8);
		return infoHeader;
	}
	void write(unsigned char* image, int width, int height, const char* filename)
	{
		int widthInBytes = width * 3;
		unsigned char padding[3] = { 0, 0, 0 };
		int paddingSize = (4 - (widthInBytes) % 4) % 4;
		int stride = (widthInBytes)+paddingSize;
		FILE* file = fopen(filename, "wb");
		unsigned char* fileHeader = createBitmapFileHeader(height, stride);
		fwrite(fileHeader, 1, 14, file);
		unsigned char* infoHeader = createBitmapInfoHeader(height, width);
		fwrite(infoHeader, 1, 40, file);
		for (int i = 0; i < height; i++)
		{
			fwrite(image + (i * widthInBytes), 3, width, file);
			fwrite(padding, 1, paddingSize, file);
		}
		fclose(file);
	}
};

class MaterialFilename
{
public:
	std::string filename{ "default" };
	aiColor3D colour{ aiColor3D(0, 0, 0) };
	void updateFilename(std::string _filename)
	{
		filename = stripPath(_filename);
		colour = aiColor3D(0, 0, 0);
	}
	void updateColour(aiColor3D _colour, std::string _filename)
	{
		colour = _colour;
		filename = stripPath(_filename);
	}
	void updateColour(float v, std::string _filename)
	{
		colour = aiColor3D(v, v, v);
		filename = stripPath(_filename);
	}
	void makeTexture(std::string filename)
	{
		unsigned char pixel[3];
		pixel[2] = colour.r * 255;
		pixel[1] = colour.g * 255;
		pixel[0] = colour.b * 255;
		BitmapWriter bitmap;
		bitmap.write(pixel, 1, 1, filename.c_str());
	}
	void write(std::string name, FILE* file)
	{
		if (colour.IsBlack() == false)
		{
			std::string fullFilename = use<Paths>().texturesPath + filename;
			makeTexture(fullFilename);
		}
		if (filename != "default")
		{
			writeStringWithPath(use<Paths>().texturesPath, name, filename, file);
			return;
		}
		writeString(name, file);
		writeString(filename, file);
	}
};

class Material
{
public:
	MaterialFilename diffuse;
	MaterialFilename normals;
	MaterialFilename metallic;
	MaterialFilename roughness;
	int isLoaded;
	Material()
	{
		isLoaded = 0;
	}
	void load(aiMaterial* material, std::string meshName)
	{
		/*for (int i = 0; i < material->mNumProperties; i++)
		{
			printf("%s\n", material->mProperties[i]->mKey.C_Str());
		}*/
		aiString textureName;
		if (material->GetTextureCount(aiTextureType_BASE_COLOR) > 0)
		{
			material->Get(AI_MATKEY_TEXTURE(aiTextureType_BASE_COLOR, 0), textureName);
			diffuse.updateFilename(std::string(textureName.C_Str()));
		} else
		{
			if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			{
				material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), textureName);
				diffuse.updateFilename(std::string(textureName.C_Str()));
			} else
			{
				aiColor3D colour(0, 0, 0);
				material->Get(AI_MATKEY_COLOR_DIFFUSE, colour);
				std::string filename = use<Paths>().texturesPath + use<Paths>().modelName + "_" + meshName + "_diffuse.bmp";
				diffuse.updateColour(colour, filename);
			}
		}
		if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
		{
			material->Get(AI_MATKEY_TEXTURE(aiTextureType_NORMALS, 0), textureName);
			normals.updateFilename(std::string(textureName.C_Str()));
		} else
		{
			std::string filename = use<Paths>().texturesPath + use<Paths>().modelName + "_" + meshName + "_normals.bmp";
			normals.updateColour(aiColor3D(0.0f, 0.0f, 1.0f), filename);
		}
		if (material->GetTextureCount(aiTextureType_METALNESS) > 0)
		{
			material->Get(AI_MATKEY_TEXTURE(aiTextureType_METALNESS, 0), textureName);
			metallic.updateFilename(std::string(textureName.C_Str()));
		} else
		{
			ai_real value;
			if (material->Get(AI_MATKEY_METALLIC_FACTOR, value) == aiReturn_FAILURE)
			{
				value = 0.1f;
			}
			std::string filename = use<Paths>().texturesPath + use<Paths>().modelName + "_" + meshName + "_metallic.bmp";
			metallic.updateColour(value, filename);
		}
		if (material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) > 0)
		{
			material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE_ROUGHNESS, 0), textureName);
			roughness.updateFilename(std::string(textureName.C_Str()));
		} else
		{
			ai_real value;
			material->Get(AI_MATKEY_ROUGHNESS_FACTOR, value);
			std::string filename = use<Paths>().texturesPath + use<Paths>().modelName + "_" + meshName + "_roughness.bmp";
			roughness.updateColour(value, filename);
		}
	}
	void updateMaterialName(std::string material, std::string filename)
	{
		if (material == "diffuse")
		{
			diffuse.updateFilename(filename);
		}
		if (material == "normals")
		{
			normals.updateFilename(filename);
		}
		if (material == "metallic")
		{
			metallic.updateFilename(filename);
		}
		if (material == "diffuse")
		{
			roughness.updateFilename(filename);
		}
	}
	void write(FILE* file)
	{
		int i = 0;
		i = 4;
		fwrite(&i, sizeof(int), 1, file);
		diffuse.write("diffuse", file);
		normals.write("normals", file);
		metallic.write("metallic", file);
		roughness.write("roughness", file);
	}
	int loaded()
	{
		return isLoaded;
	}
};

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<int> indices;
	Material material;
	std::string name;
	int loadMesh(aiMesh* mesh, aiMatrix4x4 transform, Skeleton &skeleton)
	{
		int vertexOffset = (int)vertices.size();
		int indexOffset = (int)indices.size();
		name = mesh->mName.C_Str();
		aiMatrix3x3 rotation;
		rotation.a1 = transform.a1;
		rotation.a2 = transform.a2;
		rotation.a3 = transform.a3;
		rotation.b1 = transform.b1;
		rotation.b2 = transform.b2;
		rotation.b3 = transform.b3;
		rotation.c1 = transform.c1;
		rotation.c2 = transform.c2;
		rotation.c3 = transform.c3;
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex v;
			aiVector3D p = mesh->mVertices[i];
			p = transform * p;
			v.p.x = p.x;
			v.p.y = p.y;
			v.p.z = p.z;
			if (mesh->HasTextureCoords(0))
			{
				v.uv.u = mesh->mTextureCoords[0][i].x;
				v.uv.v = mesh->mTextureCoords[0][i].y;
			}
			p = mesh->mNormals[i];
			p = rotation * p;
			v.n.x = p.x;
			v.n.y = p.y;
			v.n.z = p.z;
			if (mesh->HasTextureCoords(0))
			{
				p = mesh->mNormals[i];
				p = rotation * mesh->mTangents[i];
				v.tangent.x = p.x;
				v.tangent.y = p.y;
				v.tangent.z = p.z;
			}
			vertices.push_back(v);
		}
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			if (face.mNumIndices != 3)
			{
				return 0;
			}
			for (int n = 0; n < 3; n++)
			{
				indices.push_back(face.mIndices[n] + vertexOffset);
			}
		}
		for (unsigned int i = 0; i < mesh->mNumBones; i++)
		{
			Bone b;
			aiBone* bone = mesh->mBones[i];
			int boneID = skeleton.findBone(bone->mName.C_Str());
			if (boneID == -1)
			{
				b.name = std::string(bone->mName.C_Str());
				b.offset.fromAssimp(bone->mOffsetMatrix);
				//b.offset.transpose();
				skeleton.bones.push_back(b);
				boneID = skeleton.bones.size() - 1;
			}
			for (unsigned int n = 0; n < bone->mNumWeights; n++)
			{
				vertices[vertexOffset + bone->mWeights[n].mVertexId].addBone(boneID, bone->mWeights[n].mWeight);
			}
		}
		return 1;
	}
	void write(FILE* file, Skeleton skeleton)
	{
		material.write(file);
		int n = vertices.size();
		fwrite(&n, sizeof(unsigned int), 1, file);
		for (int i = 0; i < vertices.size(); i++)
		{
			if (skeleton.bones.size() > 0)
			{
				vertices[i].writeAnimated(file);
			} else
			{
				vertices[i].writeStatic(file);
			}
		}
		n = indices.size();
		fwrite(&n, sizeof(unsigned int), 1, file);
		fwrite(&indices[0], sizeof(unsigned int), n, file);
	}
};

class Model
{
public:
	std::vector<Mesh> meshes;
	Skeleton skeleton;
	std::vector<Animation> animations;
	int isMeshValid(aiMesh* mesh)
	{
		if (mesh->HasNormals() == false)
		{
			return 0;
		}
		return 1;
	}
	int findMeshTransformation(const aiScene *scene, aiNode* node, aiString name, aiMatrix4x4 &transformation)
	{
		for (int i = 0; i < node->mNumMeshes; i++)
		{
			if (strcmp(scene->mMeshes[node->mMeshes[i]]->mName.C_Str(), name.C_Str()) == 0)
			{
				transformation = node->mTransformation;
				return 1;
			}
		}
		for (int i = 0; i < node->mNumChildren; i++)
		{
			if (findMeshTransformation(scene, node->mChildren[i], name, transformation) == 1)
			{
				return 1;
			}
		}
		return 0;
	}
	void loadSkeleton(aiNode* node, std::vector<BoneHelper>& nodes, std::string pname = "")
	{
		if (skeleton.findBone(node->mName.C_Str()) != -1)
		{
			BoneHelper h;
			h.name = node->mName.C_Str();
			h.upName = pname;//node->mParent->mName.C_Str();
			nodes.push_back(h);
			pname = h.name;
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			loadSkeleton(node->mChildren[i], nodes, pname);
		}
	}
	void loadAnimation(aiAnimation* anim, std::string useName = "")
	{
		Animation a;
		a.name = useName == "" ? anim->mName.C_Str() : useName;
		// Find number of frames
		unsigned int frames = 0;
		int maxF = 0;
		int maxK = 0;
		for (unsigned int i = 0; i < anim->mNumChannels; i++)
		{
			unsigned int f = std::max(std::max(anim->mChannels[i]->mNumPositionKeys, anim->mChannels[i]->mNumRotationKeys), anim->mChannels[i]->mNumScalingKeys);
			if (f > frames)
			{
				frames = f;
				maxF = i;
				if (anim->mChannels[i]->mNumPositionKeys > anim->mChannels[i]->mNumRotationKeys && anim->mChannels[i]->mNumPositionKeys > anim->mChannels[i]->mNumScalingKeys)
				{
					maxK = 0;
				} else
				{
					if (anim->mChannels[i]->mNumScalingKeys > anim->mChannels[i]->mNumPositionKeys && anim->mChannels[i]->mNumScalingKeys > anim->mChannels[i]->mNumRotationKeys)
					{
						maxK = 2;
					} else
					{
						maxK = 1;
					}
				}
			}
		}
		for (unsigned int i = 0; i < frames; i++)
		{
			Frame f;
			a.frames.push_back(f);
			float t;
			if (maxK == 0)
			{
				t = (float)anim->mChannels[maxF]->mPositionKeys[i].mTime;
			}
			if (maxK == 1)
			{
				t = (float)anim->mChannels[maxF]->mRotationKeys[i].mTime;
			}
			if (maxK == 2)
			{
				t = (float)anim->mChannels[maxF]->mScalingKeys[i].mTime;
			}
			a.times.push_back(t);
		}
		for (unsigned int i = 0; i < frames; i++)
		{
			for (int n = 0; n < skeleton.bones.size(); n++)
			{
				a.frames[i].positions.push_back(Vec3());
				a.frames[i].rotations.push_back(Quaternion(0, 0, 0, 1));
				a.frames[i].scales.push_back(Vec3());
				a.frames[i].validp.push_back(0);
				a.frames[i].validr.push_back(0);
				a.frames[i].valids.push_back(0);
			}
		}
		// Load init
		for (unsigned int i = 0; i < anim->mNumChannels; i++)
		{
			int ID = skeleton.findBone(anim->mChannels[i]->mNodeName.C_Str());
			if (ID != -1)
			{
				for (unsigned int n = 0; n < anim->mChannels[i]->mNumPositionKeys; n++)
				{
					a.frames[n].positions[ID] = Vec3(anim->mChannels[i]->mPositionKeys[n].mValue.x, anim->mChannels[i]->mPositionKeys[n].mValue.y, anim->mChannels[i]->mPositionKeys[n].mValue.z);
					a.frames[n].validp[ID] = 1;
				}
				for (unsigned int n = 0; n < anim->mChannels[i]->mNumRotationKeys; n++)
				{
					a.frames[n].rotations[ID] = Quaternion(anim->mChannels[i]->mRotationKeys[n].mValue.x, anim->mChannels[i]->mRotationKeys[n].mValue.y, anim->mChannels[i]->mRotationKeys[n].mValue.z, anim->mChannels[i]->mRotationKeys[n].mValue.w);
					a.frames[n].validr[ID] = 1;
				}
				for (unsigned int n = 0; n < anim->mChannels[i]->mNumScalingKeys; n++)
				{
					a.frames[n].scales[ID] = Vec3(anim->mChannels[i]->mScalingKeys[n].mValue.x, anim->mChannels[i]->mScalingKeys[n].mValue.y, anim->mChannels[i]->mScalingKeys[n].mValue.z);
					a.frames[n].valids[ID] = 1;
				}
			}
		}
		// Resample
		a.fix();
		a.ticksPerSecond = anim->mTicksPerSecond;
		animations.push_back(a);
	}
	void initialize(std::string modelName, std::string savePath, std::string texturePath)
	{
		use<Paths>().modelName = modelName;
		use<Paths>().modelSavePath = savePath;
		use<Paths>().texturesPath = texturePath;
	}
	/*void parseMaterialDesc(std::ifstream& file)
	{
		int n = 0;
		file.read(reinterpret_cast<char*>(&n), sizeof(int));
		for (int i = 0; i < n; i++)
		{
			int v = 0;
			file.read(reinterpret_cast<char*>(&v), sizeof(int));
		}
	}*/
	void loadMesh(std::ifstream& file, Mesh& mesh, int isAnimated)
	{
		int n = 0;
		/*mesh.material.diffuse.filename = loadS(file);
		mesh.material.normals.filename = loadS(file);
		mesh.material.metallic.filename = loadS(file);
		mesh.material.roughness.filename = loadS(file);*/
		file.read(reinterpret_cast<char*>(&n), sizeof(unsigned int));
		loadS(file);
		mesh.material.diffuse.filename = loadS(file);
		loadS(file);
		mesh.material.normals.filename = loadS(file);
		loadS(file);
		mesh.material.metallic.filename = loadS(file);
		loadS(file);
		mesh.material.roughness.filename = loadS(file);
		if (isAnimated == 0)
		{
			file.read(reinterpret_cast<char*>(&n), sizeof(unsigned int));
			for (int i = 0; i < n; i++)
			{
				Vertex v;
				file.read(reinterpret_cast<char*>(&v.p), sizeof(Vec3));
				file.read(reinterpret_cast<char*>(&v.n), sizeof(Vec3));
				file.read(reinterpret_cast<char*>(&v.tangent), sizeof(Vec3));
				file.read(reinterpret_cast<char*>(&v.uv), sizeof(Vec2));
				mesh.vertices.push_back(v);
			}
			file.read(reinterpret_cast<char*>(&n), sizeof(unsigned int));
			for (int i = 0; i < n; i++)
			{
				unsigned int index = 0;
				file.read(reinterpret_cast<char*>(&index), sizeof(unsigned int));
				mesh.indices.push_back(index);
			}
		} else
		{
			file.read(reinterpret_cast<char*>(&n), sizeof(unsigned int));
			for (int i = 0; i < n; i++)
			{
				Vertex v;
				file.read(reinterpret_cast<char*>(&v.p), sizeof(Vec3));
				file.read(reinterpret_cast<char*>(&v.n), sizeof(Vec3));
				file.read(reinterpret_cast<char*>(&v.tangent), sizeof(Vec3));
				file.read(reinterpret_cast<char*>(&v.uv), sizeof(Vec2));
				file.read(reinterpret_cast<char*>(&v.bones), 4 * sizeof(unsigned int));
				file.read(reinterpret_cast<char*>(&v.boneWeights), 4 * sizeof(float));
				mesh.vertices.push_back(v);
			}
			file.read(reinterpret_cast<char*>(&n), sizeof(unsigned int));
			for (int i = 0; i < n; i++)
			{
				unsigned int index = 0;
				file.read(reinterpret_cast<char*>(&index), sizeof(unsigned int));
				mesh.indices.push_back(index);
			}
		}
	}
	void loadFrame(Animation& aseq, std::ifstream& file, int bonesN)
	{
		Frame frame;
		for (int i = 0; i < bonesN; i++)
		{
			frame.positions.push_back(loadV(file));
		}
		for (int i = 0; i < bonesN; i++)
		{
			frame.rotations.push_back(loadQ(file));
		}
		for (int i = 0; i < bonesN; i++)
		{
			frame.scales.push_back(loadV(file));
		}
		aseq.frames.push_back(frame);
	}
	void loadFrames(Animation& aseq, std::ifstream& file, int bonesN, int frames)
	{
		for (int i = 0; i < frames; i++)
		{
			loadFrame(aseq, file, bonesN);
		}
	}
	void load(std::string filename)
	{
		std::ifstream file(filename, ::std::ios::binary);
		unsigned int n = 0;
		file.read(reinterpret_cast<char*>(&n), sizeof(unsigned int));
		if (n != 4058972161)
		{
			std::cout << filename << " is not a GE Model File" << std::endl;
			exit(0);
		}
		//parseMaterialDesc(file);
		unsigned int isAnimated = 0;
		file.read(reinterpret_cast<char*>(&isAnimated), sizeof(unsigned int));
		file.read(reinterpret_cast<char*>(&n), sizeof(unsigned int));
		for (unsigned int i = 0; i < n; i++)
		{
			Mesh mesh;
			loadMesh(file, mesh, isAnimated);
			meshes.push_back(mesh);
		}
		if (isAnimated == 1)
		{
			// Read skeleton
			unsigned int bonesN = 0;
			file.read(reinterpret_cast<char*>(&bonesN), sizeof(unsigned int));
			for (unsigned int i = 0; i < bonesN; i++)
			{
				Bone bone;
				bone.name = loadS(file);
				bone.offset = loadM(file);
				file.read(reinterpret_cast<char*>(&bone.parentIndex), sizeof(int));
				skeleton.bones.push_back(bone);
			}
			skeleton.globalInverse = loadM(file);
			// Read animation sequence
			file.read(reinterpret_cast<char*>(&n), sizeof(unsigned int));
			for (unsigned int i = 0; i < n; i++)
			{
				Animation aseq;
				aseq.name = loadS(file);
				int frames = 0;
				file.read(reinterpret_cast<char*>(&frames), sizeof(int));
				file.read(reinterpret_cast<char*>(&aseq.ticksPerSecond), sizeof(float));
				loadFrames(aseq, file, bonesN, frames);
				animations.push_back(aseq);
			}
		}
		file.close();
	}
	int load(std::string filename, unsigned int type, int centreToOrigin, int useTransforms = 0, std::string useName = "")
	{
		Assimp::Importer importer;
		importer.SetPropertyInteger(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0); // Check this works
		const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_FindInvalidData | aiProcess_GenUVCoords | aiProcess_TransformUVCoords);
		if (scene == NULL)
		{
			std::cout << filename << " did not load: " << importer.GetErrorString() << std::endl;
			return 0;
		}
		if ((type & TYPE_MESH) > 0)
		{
			for (unsigned int i = 0; i < scene->mNumMeshes; i++)
			{
				if (isMeshValid(scene->mMeshes[i]) == 1)
				{
					aiMatrix4x4 transformation;
					if (useTransforms == 1)
					{
						findMeshTransformation(scene, scene->mRootNode, scene->mMeshes[i]->mName, transformation);
					}
					while (meshes.size() <= scene->mMeshes[i]->mMaterialIndex)
					{
						meshes.push_back(Mesh());
					}
					if (meshes[scene->mMeshes[i]->mMaterialIndex].material.loaded() == 0)
					{
						meshes[scene->mMeshes[i]->mMaterialIndex].material.load(scene->mMaterials[scene->mMeshes[i]->mMaterialIndex], scene->mMeshes[i]->mName.C_Str());
					}
					meshes[scene->mMeshes[i]->mMaterialIndex].loadMesh(scene->mMeshes[i], transformation, skeleton);
				}
			}
			if (skeleton.bones.size() > 0)
			{
				skeleton.rootTransform.fromAssimp(scene->mRootNode->mTransformation);
				//skeleton.rootTransform.transpose();
				skeleton.globalInverse = skeleton.rootTransform;
				skeleton.globalInverse.inverse();
				std::vector<BoneHelper> nodes;
				loadSkeleton(scene->mRootNode, nodes);
				std::vector<Bone> bones;
				for (int i = 0; i < nodes.size(); i++)
				{
					bones.push_back(skeleton.bones[skeleton.findBone(nodes[i].name)]);
				}
				skeleton.bones.clear();
				for (int i = 0; i < bones.size(); i++)
				{
					skeleton.bones.push_back(bones[i]);
				}
				for (int i = 0; i < nodes.size(); i++)
				{
					skeleton.bones[skeleton.findBone(nodes[i].name)].parentIndex = skeleton.findBone(nodes[i].upName);
				}
			}
			if (centreToOrigin == 1 && skeleton.bones.size() == 0)
			{
				Vec3 centre;
				float base = FLT_MAX;
				int total = 0;
				for (int i = 0; i < meshes.size(); i++)
				{
					for (int n = 0; n < meshes[i].vertices.size(); n++)
					{
						centre = centre + meshes[i].vertices[n].p;
						base = std::min(base, meshes[i].vertices[n].p.y);
						total++;
					}
				}
				centre = centre * (1.0f / (float)total);
				centre.y = base;
				for (int i = 0; i < meshes.size(); i++)
				{
					for (int n = 0; n < meshes[i].vertices.size(); n++)
					{
						meshes[i].vertices[n].p = meshes[i].vertices[n].p - centre;
					}
				}
			}
		}
		if ((type & TYPE_ANIMATION) > 0)
		{
			for (unsigned int i = 0; i < scene->mNumAnimations; i++)
			{
				loadAnimation(scene->mAnimations[i], useName);
			}
		}
		return 1;
	}
	int findAnimationFrames(std::string name)
	{
		for (int i = 0; i < animations.size(); i++)
		{
			if (animations[i].name == name)
			{
				return animations[i].frames.size();
			}
		}
		return 0;
	}
	void cropAnimation(std::string name, int start, int end)
	{
		for (int i = 0; i < animations.size(); i++)
		{
			if (animations[i].name == name)
			{
				animations[i].frames.erase(animations[i].frames.begin(), animations[i].frames.begin() + start);
				animations[i].times.erase(animations[i].times.begin(), animations[i].times.begin() + start);
				for (int n = 0; n < end; n++)
				{
					animations[i].frames.pop_back();
					animations[i].times.pop_back();
				}
				return;
			}
		}
	}
	void cropAnimations(int start, int end)
	{
		for (int i = 0; i < animations.size(); i++)
		{
			cropAnimation(animations[i].name, start, end);
		}
	}
	void removeMesh(int index)
	{
		meshes.erase(meshes.begin() + index);
	}
	void removeFirstFrameAllAnimations()
	{
		cropAnimations(1, 0);
	}
	void save(std::string filename)
	{
		FILE *file;
		std::string fullFilename = use<Paths>().modelSavePath + filename;
		file = fopen(fullFilename.c_str(), "wb");
		unsigned int n = 4058972161;
		fwrite(&n, sizeof(unsigned int), 1, file);
		n = 0;
		if (skeleton.bones.size() > 0)
		{
			n = 1;
		}
		fwrite(&n, sizeof(unsigned int), 1, file);
		n = meshes.size();
		fwrite(&n, sizeof(unsigned int), 1, file);
		for (int i = 0; i < n; i++)
		{
			meshes[i].write(file, skeleton);
		}
		if (skeleton.bones.size() > 0)
		{
			skeleton.write(file);
			n = animations.size();
			fwrite(&n, sizeof(unsigned int), 1, file);
			for (int i = 0; i < n; i++)
			{
				animations[i].write(file);
			}
		}
		fclose(file);
	}
};