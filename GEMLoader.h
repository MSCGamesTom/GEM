#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

namespace GEMLoader
{

	class GEMMaterial
	{
	public:
		std::string diffuse;
		std::string normals;
		std::string metallic;
		std::string roughness;
	};

	class GEMVec3
	{
	public:
		float x;
		float y;
		float z;
	};

	class GEMStaticVertex
	{
	public:
		GEMVec3 position;
		GEMVec3 normal;
		GEMVec3 tangent;
		float u;
		float v;
	};

	class GEMAnimatedVertex
	{
	public:
		GEMVec3 position;
		GEMVec3 normal;
		GEMVec3 tangent;
		float u;
		float v;
		unsigned int bonesIDs[4];
		float boneWeights[4];
	};

	class GEMMesh
	{
	public:
		GEMMaterial material;
		std::vector<GEMStaticVertex> verticesStatic;
		std::vector<GEMAnimatedVertex> verticesAnimated;
		std::vector<unsigned int> indices;
		bool isAnimated()
		{
			return verticesAnimated.size() > 0;
		}
	};

	class GEMMatrix
	{
	public:
		float m[16];
	};

	class GEMQuaternion
	{
	public:
		float q[4];
	};

	struct GEMBone
	{
		std::string name;
		GEMMatrix offset;
		int parentIndex;
	};

	struct GEMAnimationFrame
	{
		std::vector<GEMVec3> positions;
		std::vector<GEMQuaternion> rotations;
		std::vector<GEMVec3> scales;
	};

	struct GEMAnimationSequence // This holds rescaled times
	{
		std::string name;
		std::vector<GEMAnimationFrame> frames;
		float ticksPerSecond;
	};

	class GEMAnimation
	{
	public:
		std::vector<GEMBone> bones;
		std::vector<GEMAnimationSequence> animations;
		GEMMatrix globalInverse;
	};

	class GEMModelLoader
	{
	public:
		int isAnimatedModel(std::string filename)
		{
			std::ifstream file(filename, ::std::ios::binary);
			unsigned int n = 0;
			file.read(reinterpret_cast<char*>(&n), sizeof(unsigned int));
			if (n != 4058972161)
			{
				std::cout << filename << " is not a GE Model File" << std::endl;
				file.close();
				exit(0);
			}
			unsigned int isAnimated = 0;
			file.read(reinterpret_cast<char*>(&isAnimated), sizeof(unsigned int));
			file.close();
			return isAnimated;
		}
		void parseMaterialDesc(std::ifstream& file)
		{
			int n = 0;
			file.read(reinterpret_cast<char*>(&n), sizeof(int));
			for (int i = 0; i < n; i++)
			{
				int v = 0;
				file.read(reinterpret_cast<char*>(&v), sizeof(int));
			}
		}
		void loadMesh(std::ifstream& file, GEMMesh &mesh, int isAnimated)
		{
			int n = 0;
			mesh.material.diffuse = loadS(file);
			mesh.material.normals = loadS(file);
			mesh.material.metallic = loadS(file);
			mesh.material.roughness = loadS(file);
			if (isAnimated == 0)
			{
				file.read(reinterpret_cast<char*>(&n), sizeof(unsigned int));
				for (int i = 0; i < n; i++)
				{
					GEMStaticVertex v;
					file.read(reinterpret_cast<char*>(&v), sizeof(GEMStaticVertex));
					mesh.verticesStatic.push_back(v);
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
					GEMAnimatedVertex v;
					file.read(reinterpret_cast<char*>(&v), sizeof(GEMAnimatedVertex));
					mesh.verticesAnimated.push_back(v);
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
		void load(std::string filename, std::vector<GEMMesh>& meshes)
		{
			std::ifstream file(filename, ::std::ios::binary);
			unsigned int n = 0;
			file.read(reinterpret_cast<char*>(&n), sizeof(unsigned int));
			if (n != 4058972161)
			{
				std::cout << filename << " is not a GE Model File" << std::endl;
				file.close();
				exit(0);
			}
			parseMaterialDesc(file);
			unsigned int isAnimated = 0;
			file.read(reinterpret_cast<char*>(&isAnimated), sizeof(unsigned int));
			file.read(reinterpret_cast<char*>(&n), sizeof(unsigned int));
			for (int i = 0; i < n; i++)
			{
				GEMMesh mesh;
				loadMesh(file, mesh, isAnimated);
				meshes.push_back(mesh);
			}
			file.close();
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
		GEMVec3 loadV(std::ifstream& file)
		{
			GEMVec3 v;
			file.read(reinterpret_cast<char*>(&v), sizeof(GEMVec3));
			return v;
		}
		GEMMatrix loadM(std::ifstream& file)
		{
			GEMMatrix mat;
			file.read(reinterpret_cast<char*>(&mat.m), sizeof(float) * 16);
			return mat;
		}
		GEMQuaternion loadQ(std::ifstream& file)
		{
			GEMQuaternion q;
			file.read(reinterpret_cast<char*>(&q.q), sizeof(float) * 4);
			return q;
		}
		void loadFrame(GEMAnimationSequence& aseq, std::ifstream& file, int bonesN)
		{
			GEMAnimationFrame frame;
			for (int i = 0; i < bonesN; i++)
			{
				GEMVec3 p = loadV(file);
				frame.positions.push_back(p);
			}
			for (int i = 0; i < bonesN; i++)
			{
				GEMQuaternion q = loadQ(file);
				frame.rotations.push_back(q);
			}
			for (int i = 0; i < bonesN; i++)
			{
				GEMVec3 s = loadV(file);
				frame.scales.push_back(s);
			}
			aseq.frames.push_back(frame);
		}
		void loadFrames(GEMAnimationSequence& aseq, std::ifstream& file, int bonesN, int frames)
		{
			for (int i = 0; i < frames; i++)
			{
				loadFrame(aseq, file, bonesN);
			}
		}
		void load(std::string filename, std::vector<GEMMesh>& meshes, GEMAnimation& animation)
		{
			std::ifstream file(filename, ::std::ios::binary);
			unsigned int n = 0;
			file.read(reinterpret_cast<char*>(&n), sizeof(unsigned int));
			if (n != 4058972161)
			{
				printf("%s is not a GE Model File", filename);
				exit(0);
			}
			parseMaterialDesc(file);
			unsigned int isAnimated = 0;
			file.read(reinterpret_cast<char*>(&isAnimated), sizeof(unsigned int));
			file.read(reinterpret_cast<char*>(&n), sizeof(unsigned int));
			for (int i = 0; i < n; i++)
			{
				GEMMesh mesh;
				loadMesh(file, mesh, isAnimated);
				meshes.push_back(mesh);
			}
			// Read skeleton
			int bonesN = 0;
			file.read(reinterpret_cast<char*>(&bonesN), sizeof(unsigned int));
			for (int i = 0; i < bonesN; i++)
			{
				GEMBone bone;
				bone.name = loadS(file);
				bone.offset = loadM(file);
				file.read(reinterpret_cast<char*>(&bone.parentIndex), sizeof(int));
				animation.bones.push_back(bone);
			}
			animation.globalInverse = loadM(file);
			// Read animation sequence
			file.read(reinterpret_cast<char*>(&n), sizeof(unsigned int));
			for (int i = 0; i < n; i++)
			{
				GEMAnimationSequence aseq;
				aseq.name = loadS(file);
				int frames = 0;
				file.read(reinterpret_cast<char*>(&frames), sizeof(int));
				file.read(reinterpret_cast<char*>(&aseq.ticksPerSecond), sizeof(float));
				loadFrames(aseq, file, bonesN, frames);
				animation.animations.push_back(aseq);
			}
			file.close();
		}
	};

};