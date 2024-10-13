# GEM

GEM: A minimal file format for geometry and skeletal animated data.

## Overview

The GEM file loader provides classes and methods for loading 3D models and animations from GEM model files. It supports both static and animated models, handling materials, meshes, vertices, skeletal bones, and animation sequences.

These are packaged in the GEMLoader namespace.

## Overview

- **GEMLoader**: Main namespace containing all classes related to loading GEM Model Files.
- **GEMModelLoader**: Core class responsible for parsing model files and loading meshes and animations.
- **Mesh and Vertex Classes**: Represent the geometric data of models.
- **Animation Classes**: Represent skeletal animation data.

## Table of Contents

1. [Classes](#classes)
   - [GEMMaterialProperty](#gemmaterialproperty)
   - [GEMMaterial](#gemmaterial)
   - [GEMVec3](#gemvec3)
   - [GEMStaticVertex](#gemstaticvertex)
   - [GEMAnimatedVertex](#gemanimatedvertex)
   - [GEMMesh](#gemmesh)
   - [GEMMatrix](#gemmatrix)
   - [GEMQuaternion](#gemquaternion)
   - [GEMBone](#gembone)
   - [GEMAnimationFrame](#gemanimationframe)
   - [GEMAnimationSequence](#gemanimationsequence)
   - [GEMAnimation](#gemanimation)
   - [GEMModelLoader](#gemmodelloader)
2. [Usage](#usage)

## Classes

### GEMMaterialProperty

Represents a single property of a material.

**Members:**

- `std::string name`: Name of the material property.
- `std::string value`: Value of the material property.

**Constructors:**

- `GEMMaterialProperty()`: Default constructor.
- `GEMMaterialProperty(std::string initialName)`: Initializes the property with a name.

**Methods:**

- `std::string getValue(std::string _default = "")`: Returns the value as a string or `_default` if empty.
- `float getValue(float _default)`: Converts the value to a `float` or returns `_default` on failure.
- `int getValue(int _default)`: Converts the value to an `int` or returns `_default` on failure.
- `unsigned int getValue(unsigned int _default)`: Converts the value to an `unsigned int` or returns `_default` on failure.
- `void getValuesAsArray(std::vector<float>& values, char separator = ' ', float _default = 0)`: Splits the value string by `separator` and converts each segment to a `float`, adding them to `values`. Uses `_default` on conversion failure.

### GEMMaterial

Represents a material composed of multiple properties.

**Members:**

- `std::vector<GEMMaterialProperty> properties`: List of material properties.

**Methods:**

- `GEMMaterialProperty find(std::string name)`: Searches for a property by name. Returns a default `GEMMaterialProperty` with the given name if not found.

### GEMVec3

Simple 3D vector class.

**Members:**

- `float x`: X-coordinate.
- `float y`: Y-coordinate.
- `float z`: Z-coordinate.

### GEMStaticVertex

Represents a vertex in a static mesh.

**Members:**

- `GEMVec3 position`: Vertex position.
- `GEMVec3 normal`: Vertex normal.
- `GEMVec3 tangent`: Vertex tangent.
- `float u`: Texture coordinate U.
- `float v`: Texture coordinate V.

### GEMAnimatedVertex

Represents a vertex in an animated mesh, including bone data.

**Members:**

- `GEMVec3 position`: Vertex position.
- `GEMVec3 normal`: Vertex normal.
- `GEMVec3 tangent`: Vertex tangent.
- `float u`: Texture coordinate U.
- `float v`: Texture coordinate V.
- `unsigned int bonesIDs[4]`: IDs of influencing bones.
- `float boneWeights[4]`: Weights of the influencing bones.

### GEMMesh

Represents a mesh, which can be either static or animated.

**Members:**

- `GEMMaterial material`: Material applied to the mesh.
- `std::vector<GEMStaticVertex> verticesStatic`: Static vertices.
- `std::vector<GEMAnimatedVertex> verticesAnimated`: Animated vertices.
- `std::vector<unsigned int> indices`: Index buffer.

**Methods:**

- `bool isAnimated()`: Returns `true` if the mesh is animated.

### GEMMatrix

Represents a 4x4 transformation matrix.

**Members:**

- `float m[16]`: Matrix elements in column-major order.

### GEMQuaternion

Represents a quaternion for rotation.

**Members:**

- `float q[4]`: Quaternion components (x, y, z, w).

### GEMBone

Represents a bone in a skeletal hierarchy.

**Members:**

- `std::string name`: Bone name.
- `GEMMatrix offset`: Offset matrix.
- `int parentIndex`: Index of the parent bone.

### GEMAnimationFrame

Represents a single frame in an animation sequence.

**Members:**

- `std::vector<GEMVec3> positions`: Positions of bones at this frame.
- `std::vector<GEMQuaternion> rotations`: Rotations of bones at this frame.
- `std::vector<GEMVec3> scales`: Scales of bones at this frame.

### GEMAnimationSequence

Represents a sequence of animation frames.

**Members:**

- `std::string name`: Animation sequence name.
- `std::vector<GEMAnimationFrame> frames`: List of frames.
- `float ticksPerSecond`: Animation playback speed.

### GEMAnimation

Contains animation data for a model.

**Members:**

- `std::vector<GEMBone> bones`: List of bones.
- `std::vector<GEMAnimationSequence> animations`: List of animation sequences.
- `GEMMatrix globalInverse`: Global inverse transformation matrix.

### GEMModelLoader

Class responsible for loading GEM Model Files.

**Public Methods:**

- `int isAnimatedModel(std::string filename)`: Checks if the model is animated. Returns `1` if animated, `0` otherwise.
- `void load(std::string filename, std::vector<GEMMesh>& meshes)`: Loads meshes as a static model. Note animated geometry will be loaded, but the associated animation will not be loaded.
- `void load(std::string filename, std::vector<GEMMesh>& meshes, GEMAnimation& animation)`: Loads meshes and animation data from an animated model file.

**Private Methods:**

- `GEMMaterialProperty loadProperty(std::ifstream& file)`: Reads a material property from the file.
- `void loadMesh(std::ifstream& file, GEMMesh& mesh, int isAnimated)`: Loads a mesh (static or animated) from the file.
- `static std::string loadS(std::ifstream& file)`: Reads a string from the file.
- `GEMVec3 loadV(std::ifstream& file)`: Reads a `GEMVec3` from the file.
- `GEMMatrix loadM(std::ifstream& file)`: Reads a `GEMMatrix` from the file.
- `GEMQuaternion loadQ(std::ifstream& file)`: Reads a `GEMQuaternion` from the file.
- `void loadFrame(GEMAnimationSequence& aseq, std::ifstream& file, int bonesN)`: Loads a single animation frame.
- `void loadFrames(GEMAnimationSequence& aseq, std::ifstream& file, int bonesN, int frames)`: Loads multiple animation frames.

## Usage Examples

### Loading a Static Model

```cpp
#include "GEMLoader.h"

int main() {
    std::vector<GEMLoader::GEMMesh> meshes;
    GEMLoader::GEMModelLoader loader;
    loader.load("static_model.gem", meshes);

    // Use the meshes for rendering
    return 0;
}
```

Note this will also load animated models and will ognore any bone and animation data. However, vertex information will still be in the verticesAnimated vector.

### Loading an Animated Model

```cpp
#include "GEMLoader.h"

int main() {
    std::vector<GEMLoader::GEMMesh> meshes;
    GEMLoader::GEMAnimation animation;
    GEMLoader::GEMModelLoader loader;
    loader.load("animated_model.gem", meshes, animation);

    // Use the meshes and animation data for rendering
    return 0;
}
```

### Checking if a Model is Animated

```cpp
#include "GEMLoader.h"

int main() {
    GEMLoader::GEMModelLoader loader;
    int isAnimated = loader.isAnimatedModel("model.gem");
    if (isAnimated) {
        // Load as animated model
    } else {
        // Load as static model
    }
    return 0;
}
```
### Converting and unrolling vertices

The following code shows an example of converting from the GEM format structure to a custom structures in an engine, in this case a Vec3 class, i.e. ```Vec3(vec.x, vec.y, vec.z)```. This also shows flattening the vertex and index buffers into a list of vertices.

```cpp
#include "GEMLoader.h"

int main() {

		GEMLoader::GEMModelLoader loader;
		std::vector<GEMLoader::GEMMesh> gemmeshes;
		loader.load(filename, gemmeshes);
		std::vector<Vec3> vertexList;
		for (int i = 0; i < gemmeshes.size(); i++)
		{
			for (int j = 0; j < gemmeshes[i].indices.size(); j++)
			{
				GEMLoader::GEMVec3 vec = gemmeshes[i].verticesStatic[gemmeshes[i].indices[j]].position;
				vertexList.push_back(Vec3(vec.x, vec.y, vec.z));
			}
		}
    return 0;
}
```

## License

This code is provided under the MIT License: