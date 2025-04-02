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

namespace GEMLoader
{

	// This class represents a generic property with a name and value
	class GEMProperty
	{
	public:
		std::string name;  // Name of the property
		std::string value; // String value of the property

		// Default constructor
		GEMProperty() = default;

		// Constructor that initializes the property name
		GEMProperty(std::string initialName)
		{
			name = initialName;
		}

		// Retrieves the property value as a string; returns a default string if empty
		std::string getValue(std::string _default = "")
		{
			return value;
		}

		// Retrieves the property value as a float; returns a default float if conversion fails or is empty
		float getValue(float _default)
		{
			float v;
			if (value == "")
			{
				return _default;
			}
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

		// Retrieves the property value as an int; returns a default int if conversion fails or is empty
		int getValue(int _default)
		{
			int v;
			if (value == "")
			{
				return _default;
			}
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

		// Retrieves the property value as an unsigned int; uses int conversion internally
		unsigned int getValue(unsigned int _default)
		{
			int v = getValue(static_cast<int>(_default));
			return static_cast<unsigned int>(v);
		}

		// Splits the string value by a separator and stores each part as a float in the provided vector
		void getValuesAsArray(std::vector<float>& values, char seperator = ' ', float _default = 0)
		{
			std::stringstream ss(value);
			std::string word;
			while (std::getline(ss, word, seperator))
			{
				float v;
				if (word != "")
				{
					try
					{
						v = std::stof(word);
					}
					catch (...)
					{
						v = _default;
					}
				} else
				{
					v = _default;
				}
				values.push_back(v);
			}
		}

		// Retrieves three floats (x, y, z) from the property value by splitting it
		void getValuesAsVector3(float& x, float& y, float& z, char seperator = ' ', float _default = 0)
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

	// Represents a material that holds a list of GEMProperty items
	class GEMMaterial
	{
	public:
		std::vector<GEMProperty> properties; // A list of key-value properties for the material

		// Searches for a property by name and returns the found property or a default if not found
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

	// A simple 3D vector structure for storing x, y, z coordinates
	class GEMVec3
	{
	public:
		float x;
		float y;
		float z;
	};

	// Represents a vertex for a static (non-animated) mesh,
	// including position, normal, tangent, and texture coordinates (u, v)
	class GEMStaticVertex
	{
	public:
		GEMVec3 position;
		GEMVec3 normal;
		GEMVec3 tangent;
		float u;
		float v;
	};

	// Represents a vertex for an animated mesh, including:
	// position, normal, tangent, texture coords, bone IDs, and bone weights
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

	// A mesh contains either static or animated vertices,
	// an associated material, and index data for rendering
	class GEMMesh
	{
	public:
		GEMMaterial material;
		std::vector<GEMStaticVertex> verticesStatic;
		std::vector<GEMAnimatedVertex> verticesAnimated;
		std::vector<unsigned int> indices;

		// Returns true if the mesh has animated vertices
		bool isAnimated()
		{
			return verticesAnimated.size() > 0;
		}
	};

	// Represents a 4x4 matrix stored in a single array of 16 floats
	class GEMMatrix
	{
	public:
		float m[16];
	};

	// Represents a quaternion (x, y, z, w) used for rotations
	class GEMQuaternion
	{
	public:
		float q[4];
	};

	// A bone in a skeleton with a name, offset matrix, and a parent index
	struct GEMBone
	{
		std::string name;
		GEMMatrix offset;
		int parentIndex;
	};

	// A single animation frame containing position, rotation, and scale data for each bone
	struct GEMAnimationFrame
	{
		std::vector<GEMVec3> positions;
		std::vector<GEMQuaternion> rotations;
		std::vector<GEMVec3> scales;
	};

	// A collection of frames forming a named animation sequence, including playback rate
	struct GEMAnimationSequence
	{
		std::string name;
		std::vector<GEMAnimationFrame> frames;
		float ticksPerSecond;
	};

	// Holds all bones and animation sequences for a model, as well as a global inverse matrix
	class GEMAnimation
	{
	public:
		std::vector<GEMBone> bones;
		std::vector<GEMAnimationSequence> animations;
		GEMMatrix globalInverse;
	};

	// This class handles loading GEM model files (both animated and static)
	// It reads the file header, mesh data, bone data, and animation data as needed
	class GEMModelLoader
	{
	private:
		// Reads a GEMProperty (name-value) from the file
		GEMProperty loadProperty(std::ifstream& file)
		{
			GEMProperty prop;
			prop.name = loadString(file);
			prop.value = loadString(file);
			return prop;
		}

		// Loads a single mesh from the file (either static or animated)
		void loadMesh(std::ifstream& file, GEMMesh& mesh, int isAnimated)
		{
			unsigned int n = 0;

			// Load the material properties for this mesh
			file.read(reinterpret_cast<char*>(&n), sizeof(unsigned int));
			for (unsigned int i = 0; i < n; i++)
			{
				mesh.material.properties.push_back(loadProperty(file));
			}

			// If it's static
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
			}
			// If it's animated
			else
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

		// Reads a string from the file, which starts with an int length,
		// followed by that many characters
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

		// Reads a GEMVec3 structure from the file
		GEMVec3 loadVec3(std::ifstream& file)
		{
			GEMVec3 v;
			file.read(reinterpret_cast<char*>(&v), sizeof(GEMVec3));
			return v;
		}

		// Reads a GEMMatrix structure (16 floats) from the file
		GEMMatrix loadMatrix(std::ifstream& file)
		{
			GEMMatrix mat;
			file.read(reinterpret_cast<char*>(&mat.m), sizeof(float) * 16);
			return mat;
		}

		// Reads a GEMQuaternion structure (4 floats) from the file
		GEMQuaternion loadQuaternion(std::ifstream& file)
		{
			GEMQuaternion q;
			file.read(reinterpret_cast<char*>(&q.q), sizeof(float) * 4);
			return q;
		}

		// Loads data for a single animation frame, including position, rotation, and scale for each bone
		void loadFrame(GEMAnimationSequence& aseq, std::ifstream& file, int bonesN)
		{
			GEMAnimationFrame frame;

			// Load positions
			for (int i = 0; i < bonesN; i++)
			{
				GEMVec3 p = loadVec3(file);
				frame.positions.push_back(p);
			}

			// Load rotations
			for (int i = 0; i < bonesN; i++)
			{
				GEMQuaternion q = loadQuaternion(file);
				frame.rotations.push_back(q);
			}

			// Load scales
			for (int i = 0; i < bonesN; i++)
			{
				GEMVec3 s = loadVec3(file);
				frame.scales.push_back(s);
			}

			aseq.frames.push_back(frame);
		}

		// Loads multiple frames for an animation sequence
		void loadFrames(GEMAnimationSequence& aseq, std::ifstream& file, int bonesN, int frames)
		{
			for (int i = 0; i < frames; i++)
			{
				loadFrame(aseq, file, bonesN);
			}
		}

	public:
		// Checks if the model file is flagged as an animated model
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

		// Load a model file that may only contain static meshes or non-bone data
		// Populates the provided 'meshes' vector with the loaded data
		void load(std::string filename, std::vector<GEMMesh>& meshes)
		{
			std::ifstream file(filename, ::std::ios::binary);
			unsigned int n = 0;
			file.read(reinterpret_cast<char*>(&n), sizeof(unsigned int));

			// Check file signature
			if (n != 4058972161)
			{
				std::cout << filename << " is not a GE Model File" << std::endl;
				file.close();
				exit(0);
			}

			unsigned int isAnimated = 0;
			file.read(reinterpret_cast<char*>(&isAnimated), sizeof(unsigned int));
			file.read(reinterpret_cast<char*>(&n), sizeof(unsigned int));

			// Load each mesh
			for (unsigned int i = 0; i < n; i++)
			{
				GEMMesh mesh;
				loadMesh(file, mesh, isAnimated);
				meshes.push_back(mesh);
			}
			file.close();
		}

		// Load a model file that may contain meshes plus animation data (bones, frames)
		// Populates both 'meshes' and the 'animation' structure
		void load(std::string filename, std::vector<GEMMesh>& meshes, GEMAnimation& animation)
		{
			std::ifstream file(filename, ::std::ios::binary);
			unsigned int n = 0;
			file.read(reinterpret_cast<char*>(&n), sizeof(unsigned int));

			// Check file signature
			if (n != 4058972161)
			{
				std::cout << filename << " is not a GE Model File" << std::endl;
				exit(0);
			}

			unsigned int isAnimated = 0;
			file.read(reinterpret_cast<char*>(&isAnimated), sizeof(unsigned int));
			file.read(reinterpret_cast<char*>(&n), sizeof(unsigned int));

			// Load each mesh
			for (unsigned int i = 0; i < n; i++)
			{
				GEMMesh mesh;
				loadMesh(file, mesh, isAnimated);
				meshes.push_back(mesh);
			}

			// Read skeleton (bone) data
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

			// Read the global inverse matrix
			animation.globalInverse = loadMatrix(file);

			// Read animation sequences
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

	// Defines various JSON value types for a simple JSON parser
#define GEM_JSON_NULL 0
#define GEM_JSON_BOOLEAN 1
#define GEM_JSON_NUMBER 2
#define GEM_JSON_STRING 3
#define GEM_JSON_ARRAY 4
#define GEM_JSON_DICT 5

	// A lightweight JSON structure that can store booleans, floats, strings, arrays, or dictionaries
	class GEMJson
	{
	public:
		int type;
		bool vBool;
		float vFloat;
		std::string vStr;
		std::vector<GEMJson> vArr;
		std::map<std::string, GEMJson> vDict;

		// Default constructor (null JSON)
		GEMJson()
		{
			type = GEM_JSON_NULL;
		}

		// Boolean JSON constructor
		GEMJson(bool v)
		{
			type = GEM_JSON_BOOLEAN;
			vBool = v;
		}

		// Number JSON constructor
		GEMJson(float v)
		{
			type = GEM_JSON_NUMBER;
			vFloat = v;
		}

		// String JSON constructor
		GEMJson(const std::string v)
		{
			type = GEM_JSON_STRING;
			vStr = v;
		}

		// Array JSON constructor
		GEMJson(const std::vector<GEMJson>& v)
		{
			type = GEM_JSON_ARRAY;
			vArr = v;
		}

		// Dictionary JSON constructor
		GEMJson(const std::map<std::string, GEMJson>& v)
		{
			type = GEM_JSON_DICT;
			vDict = v;
		}

		// Converts this JSON value to a string representation
		std::string asStr() const
		{
			switch (type)
			{
			case GEM_JSON_BOOLEAN:
				return std::to_string(vBool);
			case GEM_JSON_NUMBER:
				return std::to_string(vFloat);
			case GEM_JSON_STRING:
				return vStr;
			default:
				return "";
			}
		}
	};

	// A simple JSON parser class that parses a JSON string into a GEMJson object
	class GEMJsonParser
	{
	public:
		std::string s;
		unsigned int pos;

		// Main entry point for parsing the provided string into a GEMJson object
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
		// Skips whitespace characters in the string
		void skipWhitespace()
		{
			while (pos < s.size() && std::isspace(s[pos]))
			{
				pos++;
			}
		}

		// Returns the current character without advancing 'pos'
		char peek() const
		{
			return (pos < s.size() ? s[pos] : 0);
		}

		// Returns the current character and advances 'pos'
		char get()
		{
			pos++;
			return s[pos - 1];
		}

		// Parses any JSON value (null, bool, number, string, array, object)
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

		// Parses the 'null' literal
		GEMJson parseNull()
		{
			pos += 4;
			return GEMJson();
		}

		// Parses a boolean value: 'true' or 'false'
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

		// Parses a numeric value, including sign, decimals, and exponent part
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

		// Parses a string, which is within double quotes
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
				result.push_back(c);
			}
			return GEMJson(result);
		}

		// Parses a JSON array: [ val1, val2, ... ]
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

		// Parses a JSON object/dictionary: { "key": value, ... }
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

	// Represents an instance of a mesh in a scene, storing a transformation matrix (w),
	// the mesh file name, and material overrides (if any)
	class GEMInstance
	{
	public:
		GEMMatrix w;
		std::string meshFilename;
		GEMMaterial material;
	};

	// Represents a full scene containing multiple mesh instances and top-level properties
	class GEMScene
	{
	public:
		std::vector<GEMInstance> instances;
		std::vector<GEMProperty> sceneProperties;

	public:
		// Parses a single instance (dictionary in JSON) into a GEMInstance object
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

		// Loads and parses a JSON scene file into GEMScene, storing instances and top-level properties
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
				// If it's not an array, treat it as a scene property
				if (item.second.type != GEM_JSON_ARRAY)
				{
					GEMProperty property;
					property.name = item.first;
					property.value = item.second.asStr();
					sceneProperties.push_back(property);
				} else
				{
					// Otherwise, parse it as an array of instances
					for (const auto& inst : item.second.vArr)
					{
						parseInstance(inst);
					}
				}
			}
		}

		// Searches for a top-level property by name and returns it if found
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