# GEM
GEM: A minimal file format for geometry and skeletal animated data.

## Overview

The GEM file loader provides classes and methods for loading 3D models and animations from GEM model files. It supports both static and animated models, handling materials, meshes, vertices, skeletal bones, and animation sequences.

These are packaged in the GEMLoader namespace.

## Classes and Structures

### GEMMaterial

Represents the material properties of a mesh, including texture file paths.

- **Members:**
  - `std::string diffuse`: Path to the diffuse (albedo) texture map.
  - `std::string normals`: Path to the normal map texture.
  - `std::string metallic`: Path to the metallic texture map.
  - `std::string roughness`: Path to the roughness texture map.

### GEMVec3

A simple 3D vector class representing positions, normals, and scales.

- **Members:**
  - `float x`: X-component of the vector.
  - `float y`: Y-component of the vector.
  - `float z`: Z-component of the vector.

### GEMStaticVertex

Represents a vertex in a static (non-animated) mesh.

- **Members:**
  - `GEMVec3 position`: The position of the vertex.
  - `GEMVec3 normal`: The normal at the vertex.
  - `GEMVec3 tangent`: The tangent vector at the vertex.
  - `float u`: Texture coordinate U.
  - `float v`: Texture coordinate V.

### GEMAnimatedVertex

Represents a vertex in an animated mesh, including bone influences.

- **Members:**
  - `GEMVec3 position`: The position of the vertex.
  - `GEMVec3 normal`: The normal at the vertex.
  - `GEMVec3 tangent`: The tangent vector at the vertex.
  - `float u`: Texture coordinate U.
  - `float v`: Texture coordinate V.
  - `unsigned int bonesIDs[4]`: IDs of up to four bones influencing the vertex.
  - `float boneWeights[4]`: Weights of the bone influences.

### GEMMesh

Represents a mesh within the model, which can be either static or animated.

- **Members:**
  - `GEMMaterial material`: The material properties of the mesh.
  - `std::vector<GEMStaticVertex> verticesStatic`: List of static vertices.
  - `std::vector<GEMAnimatedVertex> verticesAnimated`: List of animated vertices.
  - `std::vector<unsigned int> indices`: Indices for indexed drawing.

- **Methods:**
  - `bool isAnimated()`: Returns `true` if the mesh contains animated vertices.

### GEMMatrix

Represents a 4x4 transformation matrix used for bone transformations and global transformations.

- **Members:**
  - `float m[16]`: Elements of the matrix in row-major order.

### GEMQuaternion

Represents a quaternion for rotations in animations.

- **Members:**
  - `float q[4]`: Components of the quaternion.

### GEMBone

Represents a bone in the skeletal hierarchy.

- **Members:**
  - `std::string name`: Name of the bone.
  - `GEMMatrix offset`: Offset matrix for transforming the bone.
  - `int parentIndex`: Index of the parent bone (-1 if root bone).

### GEMAnimationFrame

Represents a single frame in an animation sequence.

- **Members:**
  - `std::vector<GEMVec3> positions`: Positions of bones at this frame.
  - `std::vector<GEMQuaternion> rotations`: Rotations of bones at this frame.
  - `std::vector<GEMVec3> scales`: Scales of bones at this frame.

### GEMAnimationSequence

Represents an animation sequence containing multiple frames.

- **Members:**
  - `std::string name`: Name of the animation sequence.
  - `std::vector<GEMAnimationFrame> frames`: List of frames in the sequence.
  - `float ticksPerSecond`: Playback speed of the animation.

### GEMAnimation

Contains all skeletal animation data for an animated model.

- **Members:**
  - `std::vector<GEMBone> bones`: List of bones in the skeleton.
  - `std::vector<GEMAnimationSequence> animations`: List of animation sequences.
  - `GEMMatrix globalInverse`: Global inverse transformation matrix.

### GEMModelLoader

Responsible for loading models and animations from GEM files.

#### Public Methods

- `int isAnimatedModel(std::string filename)`: Checks if a model file contains animation data.

- `void load(std::string filename, std::vector<GEMMesh>& meshes)`: Loads a static model.

- `void load(std::string filename, std::vector<GEMMesh>& meshes, GEMAnimation& animation)`: Loads an animated model along with its animations.

#### Private Methods

- `void parseMaterialDesc(std::ifstream& file)`: Parses the material description section of the file.

- `void loadMesh(std::ifstream& file, GEMMesh& mesh, int isAnimated)`: Loads a mesh from the file.

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