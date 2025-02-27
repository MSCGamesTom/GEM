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
#include <map>

// Stop warnings about members not being initialized
#pragma warning( disable : 26495)

namespace GEMLoader
{

	class GEMProperty
	{
	public:
		std::string name;
		std::string value;
		GEMProperty() = default;
		GEMProperty(std::string initialName)
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
					v = std::stof(word);
				}
				catch (...)
				{
					v = _default;
				}
				values.push_back(v);
			}
		}
		void getValuesAsVector3(float &x, float &y, float &z, char seperator = ' ', float _default = 0)
		{
			std::vector<float> values;
			getValuesAsArray(values, seperator, _default);
			for (int i = (int)values.size(); i < 3; i++)
			{
				values.push_back(_default);
			}
			x = values[0];
			y = values[1];
			z = values[2];
		}
	};

	class GEMMaterial
	{
	public:
		std::vector<GEMProperty> properties;
		GEMProperty find(std::string name)
		{
			for (int i = 0; i < properties.size(); i++)
			{
				if (properties[i].name == name)
				{
					return properties[i];
				}
			}
			return GEMProperty(name);
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
		GEMProperty loadProperty(std::ifstream& file)
		{
			GEMProperty prop;
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

	#define GEM_JSON_NULL 0
	#define GEM_JSON_BOOLEAN 1
	#define GEM_JSON_NUMBER 2
	#define GEM_JSON_STRING 3
	#define GEM_JSON_ARRAY 4
	#define GEM_JSON_DICT 5

	class GEMJson
	{
	public:
		int type;
		bool vBool;
		float vFloat;
		std::string vStr;
		std::vector<GEMJson> vArr;
		std::map<std::string, GEMJson> vDict;

		GEMJson()
		{
			type = GEM_JSON_NULL;
		}
		GEMJson(bool v)
		{
			type = GEM_JSON_BOOLEAN;
			vBool = v;
		}
		GEMJson(float v)
		{
			type = GEM_JSON_NUMBER;
			vFloat = v;
		}
		GEMJson(const std::string v)
		{
			type = GEM_JSON_STRING;
			vStr = v;
		}
		GEMJson(const std::vector<GEMJson>& v)
		{
			type = GEM_JSON_ARRAY;
			vArr = v;
		}
		GEMJson(const std::map<std::string, GEMJson>& v)
		{
			type = GEM_JSON_DICT;
			vDict = v;
		}
		std::string asStr() const
		{
			switch (type)
			{
			case GEM_JSON_BOOLEAN:
			{
				return std::to_string(vBool);
			}
			case GEM_JSON_NUMBER:
			{
				return std::to_string(vFloat);
			}
			case GEM_JSON_STRING:
			{
				return vStr;
			}
			default:
			{
				return "";
			}
			}
		}
	};

	class GEMJsonParser
	{
	public:
		std::string s;
		unsigned int pos;
		GEMJson parse(const std::string& str)
		{
			s = str;
			pos = 0;
			skipWhitespace();
			GEMJson value = parseValue();
			skipWhitespace();
			return value;
		}

	private:
		void skipWhitespace()
		{
			while (pos < s.size() && std::isspace(s[pos]))
			{
				pos++;
			}
		}
		char peek() const
		{
			return (pos < s.size() ? s[pos] : 0);
		}
		char get()
		{
			pos++;
			return s[pos - 1];
		}
		GEMJson parseValue()
		{
			skipWhitespace();
			char c = peek();
			if (c == 'n')
			{
				return parseNull();
			}
			if (c == 't' || c == 'f')
			{
				return parseBool();
			}
			if (c == '-' || std::isdigit(c))
			{
				return parseNum();
			}
			if (c == '"')
			{
				return parseStr();
			}
			if (c == '[')
			{
				return parseArr();
			}
			if (c == '{')
			{
				return parseDict();
			}
			return GEMJson();
		}
		GEMJson parseNull()
		{
			pos += 4;
			return GEMJson();
		}
		GEMJson parseBool()
		{
			if (s[pos] == 't')
			{
				pos += 4;
				return GEMJson(true);
			} else
			{
				pos += 5;
				return GEMJson(false);
			}
		}
		GEMJson parseNum()
		{
			size_t start = pos;
			if (peek() == '-')
			{
				get();
			}
			if (peek() == '0')
			{
				get();
			} else
			{
				while (std::isdigit(peek()) != 0)
				{
					get();
				}
			}
			if (peek() == '.')
			{
				get();
				while (std::isdigit(peek()) != 0)
				{
					get();
				}
			}
			if (peek() == 'e' || peek() == 'E')
			{
				get();
				if (peek() == '+' || peek() == '-')
				{
					get();
				}
				while (std::isdigit(peek()) != 0)
				{
					get();
				}
			}
			float v = std::stof(s.substr(start, pos - start));
			return GEMJson(v);
		}
		GEMJson parseStr()
		{
			get();
			std::string result;
			while (true)
			{
				char c = get();
				if (c == '"')
				{
					break;
				}
				result.push_back(c); // There shouldn't be any escape characters
			}
			return GEMJson(result);
		}
		GEMJson parseArr()
		{
			get();
			skipWhitespace();
			std::vector<GEMJson> elements;
			if (peek() == ']')
			{
				get();
				return GEMJson(elements);
			}
			while (1)
			{
				elements.push_back(parseValue());
				skipWhitespace();
				char c = get();
				if (c == ']')
				{
					break;
				}
				skipWhitespace();
			}
			return GEMJson(elements);
		}
		GEMJson parseDict()
		{
			get();
			skipWhitespace();
			std::map<std::string, GEMJson> obj;
			if (peek() == '}')
			{
				get();
				return GEMJson(obj);
			}
			while (true)
			{
				skipWhitespace();
				std::string key = parseStr().vStr;
				skipWhitespace();
				get();
				skipWhitespace();
				GEMJson value = parseValue();
				obj[key] = value;
				skipWhitespace();
				char c = get();
				if (c == '}')
				{
					break;
				}
				skipWhitespace();
			}
			return GEMJson(obj);
		}
	};

	class GEMInstance
	{
	public:
		GEMMatrix w;
		std::string meshFilename;
		GEMMaterial material;
	};

	class GEMScene
	{
	public:
		std::vector<GEMInstance> instances;
		std::vector<GEMProperty> sceneProperties;
	public:
		void parseInstance(const GEMJson& inst)
		{
			GEMInstance instance;
			for (const auto& item : inst.vDict)
			{
				int isCore = 0;
				if (item.first == "filename")
				{
					instance.meshFilename = item.second.asStr();
					isCore = 1;
				}
				if (item.first == "world")
				{
					for (int i = 0; i < 16; i++)
					{
						instance.w.m[i] = item.second.vArr[i].vFloat;
					}
					isCore = 1;
				}
				if (isCore == 0)
				{
					GEMProperty property;
					property.name = item.first;
					property.value = item.second.asStr();
					instance.material.properties.push_back(property);
				}
			}
			instances.push_back(instance);
		}
		void load(std::string filename)
		{
			std::ifstream file(filename);
			std::stringstream buffer;
			buffer << file.rdbuf();
			std::string content = buffer.str();
			GEMJsonParser parser;
			GEMJson data = parser.parse(content);
			for (const auto& item : data.vDict)
			{
				if (item.second.type != GEM_JSON_ARRAY)
				{
					GEMProperty property;
					property.name = item.first;
					property.value = item.second.asStr();
					sceneProperties.push_back(property);
				} else
				{
					for (const auto& inst : item.second.vArr)
					{
						parseInstance(inst);
					}
				}
			}
		}
		GEMProperty findProperty(std::string name)
		{
			for (const auto& item : sceneProperties)
			{
				if (item.name == name)
				{
					return item;
				}
			}
			return GEMProperty(name);
		}
	};

};