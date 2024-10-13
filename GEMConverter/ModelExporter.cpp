

#include <iostream>
#include "Model.h"


/*int main()
{
	int i = 0;
	Model model;
	std::string modelName = "TRex1";
	model.initialize(modelName, "Models/", "Textures/");
	model.load(modelName + ".glb", TYPE_MESH | TYPE_ANIMATION, 1);
	//model.load("run forward.fbx", TYPE_ANIMATION, 0, "run forward");
	model.save(modelName + ".gem");
	return 0;
}*/

Model model;

extern "C" __declspec(dllexport) void initialize(char *modelName, char *savePath, char *texturePath)
{
	model.initialize(modelName, savePath, texturePath);
}

extern "C" __declspec(dllexport) void addModel(char *filename, int importMesh, int importAnimation, int useTransforms, char *animationName, int centreToOrigin)
{
	int type = importMesh | importAnimation << 1;
	model.load(filename, type, centreToOrigin, useTransforms, animationName);
}

extern "C" __declspec(dllexport) void save(char* filename)
{
	model.save(filename);
}

extern "C" __declspec(dllexport) void removeMesh(int index)
{
	model.removeMesh(index);
}

extern "C" __declspec(dllexport) void removeFirstFrameAllAnimations()
{
	model.removeFirstFrameAllAnimations();
}

extern "C" __declspec(dllexport) int findAnimationFrames(char* name)
{
	return model.findAnimationFrames(name);
}

extern "C" __declspec(dllexport) void cropAnimations(int start, int end)
{
	model.cropAnimations(start, end);
}

extern "C" __declspec(dllexport) void cropAnimation(char *name, int start, int end)
{
	model.cropAnimation(name, start, end);
}

extern "C" __declspec(dllexport) int numMeshes()
{
	return model.meshes.size();
}

char* returnStr(const std::string str) // This leaks memory but this is a tool. Needs to be fixed.
{
	const char* name = str.c_str();
	int l = strlen(name);
	char* buffer = new char[l + 1];
	memset(buffer, 0, (l + 1) * sizeof(char));
	memcpy(buffer, name, l);
	return buffer;
}

extern "C" __declspec(dllexport) char *meshName(int index)
{
	return returnStr(model.meshes[index].name);
}

extern "C" __declspec(dllexport) char *findMaterialFilename(int index, char *_name)
{
	std::string name = _name;
	if (name == "diffuse")
	{
		return returnStr(model.meshes[index].material.diffuse.filename);
	}
	if (name == "normals")
	{
		return returnStr(model.meshes[index].material.normals.filename);
	}
	if (name == "metallic")
	{
		return returnStr(model.meshes[index].material.metallic.filename);
	}
	if (name == "roughness")
	{
		return returnStr(model.meshes[index].material.roughness.filename);
	}
	return returnStr("");
}

extern "C" __declspec(dllexport) void updateMaterialFilename(int index, char *_name, char *filename)
{
	std::string name = _name;
	if (name == "diffuse")
	{
		model.meshes[index].material.diffuse.updateFilename(filename);
	}
	if (name == "normals")
	{
		model.meshes[index].material.normals.updateFilename(filename);
	}
	if (name == "metallic")
	{
		model.meshes[index].material.metallic.updateFilename(filename);
	}
	if (name == "roughness")
	{
		model.meshes[index].material.roughness.updateFilename(filename);
	}
}

extern "C" __declspec(dllexport) int numAnimations()
{
	return model.animations.size();
}

extern "C" __declspec(dllexport) char* animationName(int index)
{
	return returnStr(model.animations[index].name);
}

extern "C" __declspec(dllexport) void renameAnimation(int index, char *name)
{
	model.animations[index].name = name;
}

/*extern "C" __declspec(dllexport) void update(char* filename)
{
	model.initialize("", "", "");
	model.load(filename);
	model.save(filename);
}*/