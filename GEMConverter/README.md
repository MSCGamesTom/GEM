# ModelExporter UI User Guide

## Introduction

The **ModelExporter UI** is a graphical user interface (GUI) application designed to simplify the process of importing, editing, and exporting 3D models and animations. It leverages the functionality of the **Assimp** library and a custom **ModelExporter** to provide users with an intuitive platform for managing 3D assets, particularly in game development and 3D modeling workflows.

This user guide will walk you through the installation process, provide an overview of the user interface, and explain how to use the application's features effectively.

---

## Table of Contents

1. [System Requirements](#system-requirements)
2. [Installation](#installation)
3. [Launching the Application](#launching-the-application)
4. [Using ModelExporter UI](#using-modelexporter-ui)
   - [Loading a Mesh](#loading-a-mesh)
   - [Loading Animations](#loading-animations)
   - [Editing Mesh Textures](#editing-mesh-textures)
   - [Renaming Animations](#renaming-animations)
   - [Removing a Mesh](#removing-a-mesh)
   - [Removing the First Frame from All Animations](#removing-the-first-frame-from-all-animations)
   - [Setting Paths](#setting-paths)
   - [Saving the Exported Model](#saving-the-exported-model)

5. [C++ DLL Notes](#dll-Notes)

---

## System Requirements

- **Operating System**: Windows (due to the use of Windows-specific DLLs)
- **Python Version**: Python 3.x
- **Libraries and Dependencies**:
  - `ctypes`
  - `tkinter`
  - `os`
  - `shutil`
  - `codecs`
- **Additional Files**:
  - `assimp-vc143-mt.dll` (Assimp library DLL)
  - `ModelExporter.dll` (Custom exporter DLL)

Ensure that the DLL files (`assimp-vc143-mt.dll` and `ModelExporter.dll`) are located in the same directory as the Python script or in a directory that's added to the DLL search path.

---

## Installation

1. **Install Python 3.x**: If not already installed, download and install Python 3.x from the [official website](https://www.python.org/downloads/).

2. **Install Required Libraries**: Most required libraries are standard with Python. If any are missing, you can install them using `pip`:
   ```bash
   pip install tkinter
   ```

3. **Prepare DLL Files**:
   - Place `assimp-vc143-mt.dll` and `ModelExporter.dll` in the same directory as the Python script.
   - Alternatively, you can place them in a directory that's added to the DLL search path within the script.

4. **Set Up Directory Structure**:
   - Create a `Models` directory where exported models will be saved.
   - Create a `Textures` directory where texture files will be stored.

---

## Launching the Application

Run the Python script using the command prompt or an IDE:

```bash
python model_exporter.py
```

---

## Using ModelExporter UI

### Loading a Mesh

1. **Click on "Load Mesh"**:
   - A file dialog will open.
   - Supported file types include `.obj`, `.fbx`, `.dae`, `.gltf`, `.glb`, `.stl`, and more.

2. **Select a Mesh File**:
   - Navigate to the directory containing your mesh file.
   - Select the desired file and click **Open**.

3. **Mesh Loading**:
   - The mesh will be loaded and displayed in the **Mesh Tree View**.
   - The application initializes with the first mesh loaded, setting the base model directory and model name.

### Loading Animations

1. **Click on "Load Animations"**:
   - A file dialog will open.
   - Supported file types include `.fbx` and `.glb`.

2. **Select Animation Files**:
   - You can select multiple files by holding down the **Ctrl** key while clicking.
   - Click **Open** after selecting the files.

3. **Animations Loading**:
   - Each animation will be loaded and displayed in the **Animation Tree View**.
   - Animations are named based on their file names.

### Editing Mesh Textures

You can edit the textures associated with a mesh:

1. **Select a Texture**:
   - In the **Mesh Tree View**, expand the mesh to see its textures (Diffuse Map, Normal Map, Metallic Map, Roughness Map).
   - Click on the texture you wish to change.

2. **Double-Click the Texture**:
   - A file dialog will open.

3. **Select a New Texture File**:
   - Navigate to the desired texture file.
   - Click **Open** to update the texture.

4. **Texture Copying**:
   - The new texture file will be copied to the `Textures` directory.
   - The texture path in the project will be updated accordingly.

### Renaming Animations

1. **Select an Animation**:
   - In the **Animation Tree View**, click on the animation you wish to rename.

2. **Double-Click the Animation**:
   - A dialog will appear prompting for a new animation name.

3. **Enter New Name**:
   - Type the desired name and click **OK**.

4. **Update**:
   - The animation will be renamed in the project and displayed with the new name.

### Removing a Mesh

You can remove unwanted meshes from the project:

1. **Select the Mesh**:
   - In the **Mesh Tree View**, click on the mesh you wish to remove.

2. **Click "Remove Mesh"**:
   - Alternatively, you can press the **Delete** key after selecting the mesh.

3. **Confirm Removal**:
   - The mesh will be removed from the project.
   - The tree view will update to reflect the change.

**Note**: Only meshes can be removed using this method. Textures and other items cannot be deleted via the UI.

### Removing the First Frame from All Animations

Sometimes, the first frame of an animation may be unnecessary or cause issues. To remove it:

1. **Click on "Remove First Frame"**:
   - Located in the control buttons area.

2. **Automatic Removal**:
   - The first frame will be removed from all loaded animations.

### Setting Paths

You can customize the paths where models and textures are saved:

1. **Click on "Set Paths"**:
   - A dialog window will open.

2. **Edit Paths**:
   - **Save Path**: Directory where the exported model files will be saved.
   - **Texture Path**: Directory where texture files are stored.

3. **Click "OK"**:
   - The exporter will reinitialize with the new paths.
   - Click **Cancel** to discard changes.

### Saving the Exported Model

1. **Click on "Save"**:
   - A save dialog will open.

2. **Enter File Name**:
   - Provide a name for your exported model.
   - The default extension is `.gem`.

3. **Select Save Location**:
   - By default, it saves to the `Models` directory.
   - You can change the location if desired.

4. **Click "Save"**:
   - The model, along with its meshes and animations, will be saved.
   - Textures used by the meshes will be copied to the `Textures` directory if they are not already present.


---

## DLL Notes

The C++ code in this folder is to build a DLL to be used with the python interface to convert models. Note this code is currently at a prototype stage and is not production ready.