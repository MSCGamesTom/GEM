/*
MIT License

Copyright (c) 2024 MSc Games Engineering Team

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

namespace GEMLoader
{

	class GEMMaterialProperty
	{
	public:
		std::string name;
		std::string value;
		GEMMaterialProperty() = default;
		GEMMaterialProperty(std::string initialName)
		{
			name = initialName;
		}
		std::string getValue(std::string _default = "")
		{
			return value;
		}
		float getValue(float _default)
		{
			float v;
			try
			{
				v = std::stof(value);
			}
			catch (...)
			{
				v = _default;
			}
			return v;
		}
		int getValue(int _default)
		{
			int v;
			try
			{
				v = std::stoi(value);
			}
			catch (...)
			{
				v = _default;
			}
			return v;
		}
		unsigned int getValue(unsigned int _default)
		{
			int v = getValue(static_cast<int>(_default));
			return static_cast<unsigned int>(v);
		}
		void getValuesAsArray(std::vector<float>& values, char seperator = ' ', float _default = 0)
		{
			std::stringstream ss(value);
			std::string word;
			while (std::getline(ss, word, seperator))
			{
				float v;
				try
				{
					v = std::stof(value);
				}
				catch (...)
				{
					v = _default;
				}
				values.push_back(v);
			}
		}
	};

	class GEMMaterial
	{
	public:
		std::vector<GEMMaterialProperty> properties;
		GEMMaterialProperty find(std::string name)
		{
			for (int i = 0; i < properties.size(); i++)
			{
				if (properties[i].name == name)
				{
					return properties[i];
				}
			}
			return GEMMaterialProperty(name);
		}
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
	private:
		GEMMaterialProperty loadProperty(std::ifstream& file)
		{
			GEMMaterialProperty prop;
			prop.name = loadString(file);
			prop.value = loadString(file);
			return prop;
		}
		void loadMesh(std::ifstream& file, GEMMesh& mesh, int isAnimated)
		{
			unsigned int n = 0;
			file.read(reinterpret_cast<char*>(&n), sizeof(unsigned int));
			for (unsigned int i = 0; i < n; i++)
			{
				mesh.material.properties.push_back(loadProperty(file));
			}
			if (isAnimated == 0)
			{
				file.read(reinterpret_cast<char*>(&n), sizeof(unsigned int));
				for (unsigned int i = 0; i < n; i++)
				{
					GEMStaticVertex v;
					file.read(reinterpret_cast<char*>(&v), sizeof(GEMStaticVertex));
					mesh.verticesStatic.push_back(v);
				}
				file.read(reinterpret_cast<char*>(&n), sizeof(unsigned int));
				for (unsigned int i = 0; i < n; i++)
				{
					unsigned int index = 0;
					file.read(reinterpret_cast<char*>(&index), sizeof(unsigned int));
					mesh.indices.push_back(index);
				}
			} else
			{
				file.read(reinterpret_cast<char*>(&n), sizeof(unsigned int));
				for (unsigned int i = 0; i < n; i++)
				{
					GEMAnimatedVertex v;
					file.read(reinterpret_cast<char*>(&v), sizeof(GEMAnimatedVertex));
					mesh.verticesAnimated.push_back(v);
				}
				file.read(reinterpret_cast<char*>(&n), sizeof(unsigned int));
				for (unsigned int i = 0; i < n; i++)
				{
					unsigned int index = 0;
					file.read(reinterpret_cast<char*>(&index), sizeof(unsigned int));
					mesh.indices.push_back(index);
				}
			}
		}
		std::string loadString(std::ifstream& file)
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
		GEMVec3 loadVec3(std::ifstream& file)
		{
			GEMVec3 v;
			file.read(reinterpret_cast<char*>(&v), sizeof(GEMVec3));
			return v;
		}
		GEMMatrix loadMatrix(std::ifstream& file)
		{
			GEMMatrix mat;
			file.read(reinterpret_cast<char*>(&mat.m), sizeof(float) * 16);
			return mat;
		}
		GEMQuaternion loadQuaternion(std::ifstream& file)
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
				GEMVec3 p = loadVec3(file);
				frame.positions.push_back(p);
			}
			for (int i = 0; i < bonesN; i++)
			{
				GEMQuaternion q = loadQuaternion(file);
				frame.rotations.push_back(q);
			}
			for (int i = 0; i < bonesN; i++)
			{
				GEMVec3 s = loadVec3(file);
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
	public:
		bool isAnimatedModel(std::string filename)
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
			unsigned int isAnimated = 0;
			file.read(reinterpret_cast<char*>(&isAnimated), sizeof(unsigned int));
			file.read(reinterpret_cast<char*>(&n), sizeof(unsigned int));
			for (unsigned int i = 0; i < n; i++)
			{
				GEMMesh mesh;
				loadMesh(file, mesh, isAnimated);
				meshes.push_back(mesh);
			}
			file.close();
		}
		void load(std::string filename, std::vector<GEMMesh>& meshes, GEMAnimation& animation)
		{
			std::ifstream file(filename, ::std::ios::binary);
			unsigned int n = 0;
			file.read(reinterpret_cast<char*>(&n), sizeof(unsigned int));
			if (n != 4058972161)
			{
				std::cout << filename << " is not a GE Model File" << std::endl;
				exit(0);
			}
			unsigned int isAnimated = 0;
			file.read(reinterpret_cast<char*>(&isAnimated), sizeof(unsigned int));
			file.read(reinterpret_cast<char*>(&n), sizeof(unsigned int));
			for (unsigned int i = 0; i < n; i++)
			{
				GEMMesh mesh;
				loadMesh(file, mesh, isAnimated);
				meshes.push_back(mesh);
			}
			// Read skeleton
			unsigned int bonesN = 0;
			file.read(reinterpret_cast<char*>(&bonesN), sizeof(unsigned int));
			for (unsigned int i = 0; i < bonesN; i++)
			{
				GEMBone bone;
				bone.name = loadString(file);
				bone.offset = loadMatrix(file);
				file.read(reinterpret_cast<char*>(&bone.parentIndex), sizeof(int));
				animation.bones.push_back(bone);
			}
			animation.globalInverse = loadMatrix(file);
			// Read animation sequence
			file.read(reinterpret_cast<char*>(&n), sizeof(unsigned int));
			for (unsigned int i = 0; i < n; i++)
			{
				GEMAnimationSequence aseq;
				aseq.name = loadString(file);
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