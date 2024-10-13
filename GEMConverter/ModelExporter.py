
import ctypes
import codecs
import os
import shutil

import tkinter as tk
from tkinter import ttk
from tkinter import filedialog as fd
from tkinter import simpledialog

class Exporter:
    def __init__(self):
        os.add_dll_directory(os.getcwd())
        self.assimp = ctypes.CDLL("assimp-vc143-mt.dll")
        self.exp = ctypes.CDLL("ModelExporter.dll")
        self.exp.initialize.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
        self.exp.addModel.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_char_p, ctypes.c_int]
        self.exp.save.argtypes = [ctypes.c_char_p]
        self.exp.cropAnimations.argtypes = [ctypes.c_int, ctypes.c_int]
        self.exp.cropAnimation.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int]
        self.exp.numMeshes.restype = ctypes.c_int
        self.exp.meshName.argtypes = [ctypes.c_int]
        self.exp.meshName.restype = ctypes.c_char_p
        self.exp.findMaterialFilename.argtypes = [ctypes.c_int, ctypes.c_char_p]
        self.exp.findMaterialFilename.restype = ctypes.c_char_p
        self.exp.updateMaterialFilename.argtypes = [ctypes.c_int, ctypes.c_char_p, ctypes.c_char_p]
        self.exp.numAnimations.restype = ctypes.c_int
        self.exp.animationName.argtypes = [ctypes.c_int]
        self.exp.animationName.restype = ctypes.c_char_p
        self.exp.renameAnimation.argtypes = [ctypes.c_int, ctypes.c_char_p]
        self.exp.removeMesh.argtypes = [ctypes.c_int]
    def to_bytes(self, s):
        if type(s) is bytes:
            return s
        elif type(s) is str or (sys.version_info[0] < 3 and type(s) is unicode):
            return codecs.encode(s, 'utf-8')
        else:
            raise TypeError("Expected bytes or string, but got %s." % type(s))
    def initialize(self, modelName, savePath, texturePath):
        self.exp.initialize(self.to_bytes(modelName), self.to_bytes(savePath), self.to_bytes(texturePath))
    def addModel(self, filename, importMesh, importAnimation, useTransforms, animationName):
        self.exp.addModel(self.to_bytes(filename), importMesh, importAnimation, useTransforms, self.to_bytes(animationName), 1)
    def save(self, filename):
        self.exp.save(self.to_bytes(filename))
    def removeFirstFrameAllAnimations(self):
        self.exp.removeFirstFrameAllAnimations()
    def cropAnimations(self, start, end):
        self.exp.cropAnimations(start, end)
    def cropAnimation(self, name, start, end):
        self.exp.cropAnimation(self.to_bytes(name), start, end)
    def numMeshes(self):
        return self.exp.numMeshes()
    def meshName(self, index):
        return self.exp.meshName(index).decode(encoding="utf-8")
    def findMaterialFilename(self, index, filename):
        return self.exp.findMaterialFilename(index, self.to_bytes(filename)).decode(encoding="utf-8")
    def updateMaterialFilename(self, index, name, filename):
        self.exp.updateMaterialFilename(index, self.to_bytes(name), self.to_bytes(filename))
    def numAnimations(self):
        return self.exp.numAnimations()
    def animationName(self, index):
        return self.exp.animationName(index).decode(encoding="utf-8")
    def renameAnimation(self, index, name):
        self.exp.renameAnimation(index, self.to_bytes(name))
    def removeMesh(self, index):
        self.exp.removeMesh(index)

exporter = Exporter()
savePath = "Models/"
texturePath = "Textures/"
baseModelDir = ""
modelname = ""
dialogDir = os.getcwd()

root = tk.Tk()
root.title("ModelExporter UI")
root.minsize(width=800, height=800)

meshNames = []

treeviewmodel = ttk.Treeview(columns=('Description'))
treeviewanimation = ttk.Treeview()

# Set up headings for the treeviewmodel
treeviewmodel.heading('#0', text='Name')
treeviewmodel.heading('Description', text='Description')

# Configure column widths
treeviewmodel.column('#0', width=200, anchor='w')
treeviewmodel.column('Description', width=150, anchor='w')

status = []

def copyTexture(src, dst):
    shutil.copyfile(src, dst)

def copyFileFromFolderSearch(filename, path, dirsl = 3):
    dirs = path.split('/')
    for l in range(1, dirsl):
        if len(dirs[:-l]) > 1:
            path1 = '/'.join(dirs[:-l])
            for root, dirs, files in os.walk(path1):
                for file in files:
                    if file == filename:
                        copyTexture(os.path.join(root, name), texturePath + filename)
                        return
    status.append(filename)

def copyTextures():
    # Iterate through textures
    for mesh in treeviewmodel.get_children():
        textures = treeviewmodel.get_children(mesh)
        for texture in textures:
            filename = treeviewmodel.item(texture)["text"]
            if not os.path.isfile(texturePath + filename):
                copyFileFromFolderSearch(filename, baseModelDir)
    # Check if in folder
    # If not, check base path, any up folders, any folders down three levels

def updateMeshList(treeview):
    treeview.delete(*treeview.get_children())
    numMeshes = exporter.numMeshes()
    meshNames.clear()
    for i in range(numMeshes):
        item = treeview.insert("", tk.END, text=exporter.meshName(i), open=True)
        meshNames.append(exporter.meshName(i))
        treeview.insert(item, tk.END, text=exporter.findMaterialFilename(i, "diffuse"), values=('Diffuse Map'))
        treeview.insert(item, tk.END, text=exporter.findMaterialFilename(i, "normals"), values=('Normal Map'))
        treeview.insert(item, tk.END, text=exporter.findMaterialFilename(i, "metallic"), values=('Metallic Map'))
        treeview.insert(item, tk.END, text=exporter.findMaterialFilename(i, "roughness"), values=('Roughness Map'))

def updateAnimationList(treeview):
    treeview.delete(*treeview.get_children())
    numAnimations = exporter.numAnimations()
    for i in range(numAnimations):
        item = treeview.insert("", tk.END, text=exporter.animationName(i))

def load_mesh():
    global dialogDir
    filetypes = (
        ('3D Model Files', '*.obj *.fbx *.dae *.gltf *.glb *.stl *.3ds *.blend '
                       '*.ifc *.ifczip *.ply *.dxf *.lwo *.lws *.x *.ac *.ms3d '
                       '*.bvh *.mdl *.md2 *.md3 *.smd *.vta *.xml *.cob *.scn'),
        ('Wavefront OBJ', '*.obj'),
        ('Autodesk FBX', '*.fbx'),
        ('COLLADA', '*.dae'),
        ('glTF', '*.gltf *.glb'),
        ('Stereolithography (STL)', '*.stl'),
        ('3D Studio Max (3DS)', '*.3ds'),
        ('Blender', '*.blend'),
        ('Industry Foundation Classes (IFC/STEP)', '*.ifc *.ifczip'),
        ('Stanford PLY', '*.ply'),
        ('AutoCAD DXF', '*.dxf'),
        ('All Files', '*.*'),
    )
    filename = fd.askopenfilename(title='Open a model', initialdir=dialogDir, filetypes=filetypes)
    if filename != "":
        if exporter.numMeshes() == 0:
            baseModelDir = os.path.dirname(os.path.abspath(filename))
            modelname = os.path.basename(filename).split(".")[0]
            exporter.initialize(modelname, savePath, texturePath)
        exporter.addModel(filename, 1, 1, 1, "")
        updateMeshList(treeviewmodel)
        updateAnimationList(treeviewanimation)
        dialogDir = os.path.dirname(os.path.abspath(filename))
        
def save_mesh():
    filetypes = (
        ('GEM Files', '*.gem'),
        ('All files', '*.*')
    )
    filename = fd.asksaveasfilename(title='Save', initialfile = modelname, initialdir=savePath, filetypes=filetypes)
    if filename != "":
        filename = os.path.basename(filename)
        root, ext = os.path.splitext(filename)
        if not ext:
            filename = filename + ".gem"
        exporter.save(filename)
    copyTextures()

def load_animations():
    global dialogDir
    filetypes = (
        ('FBX Files', '*.fbx'),
        ('GLTF Files', '*.glb'),
        ('All files', '*.*')
    )
    filenames = fd.askopenfilenames(title='Open animation files', initialdir=dialogDir, filetypes=filetypes)
    if len(filenames) > 0:
        for filename in filenames:
            name = os.path.basename(filename).split(".")[0]
            exporter.addModel(filename, 0, 1, 1, name)
        updateAnimationList(treeviewanimation)
        dialogDir = os.path.dirname(os.path.abspath(filename))

def model_doubleclick(event):
    global dialogDir
    meshIndex = treeviewmodel.index(treeviewmodel.parent(treeviewmodel.selection()[0]))
    indexList = treeviewmodel.index(treeviewmodel.selection()[0])
    tmap = ""
    if indexList == 0:
        tmap = "diffuse"
    if indexList == 1:
        tmap = "normals"
    if indexList == 2:
        tmap = "metallic"
    if indexList == 3:
        tmap = "roughness"
    filename = fd.askopenfilename(title='Open a texture', initialdir=dialogDir)
    if filename != "":
        exporter.updateMaterialFilename(meshIndex, tmap, os.path.basename(filename))
        updateMeshList(treeviewmodel)
        copyTexture(filename, texturePath + os.path.basename(filename))
        dialogDir = os.path.dirname(os.path.abspath(filename))

def animation_doubleclick(event):
    name = simpledialog.askstring("Rename Animation", "New animation name:", parent=root)
    if name != "":
        index = treeviewanimation.index(treeviewanimation.selection()[0])
        exporter.renameAnimation(index, name)
        updateAnimationList(treeviewanimation)

def animation_removeFirstFrame():
    exporter.removeFirstFrameAllAnimations()

def model_key(event):
    if event.keycode == 46:
        if treeviewmodel.parent(treeviewmodel.selection()[0]) != "":
            meshIndex = treeviewmodel.index(treeviewmodel.parent(treeviewmodel.selection()[0]))
        else:
            meshIndex = treeviewmodel.index(treeviewmodel.selection()[0])
        exporter.removeMesh(meshIndex)
        updateMeshList(treeviewmodel)

def set_paths():
    def on_ok():
        global exporter, modelname, savePath, texturePath
        savePath = save_entry.get()
        exporter.initialize(modelname, savePath, texturePath)
        texturePath = texture_entry.get()
        path_window.destroy()
    def on_cancel():
        path_window.destroy()
    path_window = tk.Toplevel(root)
    path_window.title("Set Paths")
    path_window.resizable(False, False)
    # Center the window
    path_window.transient(root)
    save_label = tk.Label(path_window, text="Save Path:")
    save_entry = tk.Entry(path_window, width=50)
    save_entry.insert(0, savePath)
    texture_label = tk.Label(path_window, text="Texture Path:")
    texture_entry = tk.Entry(path_window, width=50)
    texture_entry.insert(0, texturePath)
    save_label.grid(row=0, column=0, sticky="e", padx=5, pady=5)
    save_entry.grid(row=0, column=1, padx=5, pady=5)
    texture_label.grid(row=1, column=0, sticky="e", padx=5, pady=5)
    texture_entry.grid(row=1, column=1, padx=5, pady=5)
    button_frame = tk.Frame(path_window)
    button_frame.grid(row=2, column=0, columnspan=2, pady=10)
    ok_button = tk.Button(button_frame, text="OK", command=on_ok)
    cancel_button = tk.Button(button_frame, text="Cancel", command=on_cancel)
    ok_button.pack(side="left", padx=5)
    cancel_button.pack(side="left", padx=5)
    path_window.grab_set()
    root.wait_window(path_window)
        
    
loadMesh = ttk.Button(root, text="Load Mesh", command=load_mesh)
loadAnimations = ttk.Button(root, text="Load Animations", command=load_animations)
saveMesh = ttk.Button(root, text="Save", command=save_mesh)

# Place buttons in the first row
loadMesh.grid(row=0, column=0, sticky="nsew")
loadAnimations.grid(row=0, column=1, sticky="nsew")

# Place Treeviews
treeviewmodel.grid(row=1, column=0, sticky="nsew")
treeviewanimation.grid(row=1, column=1, sticky="nsew")

row_frame = tk.Frame(root)
row_frame.grid(row=1, column=2, sticky="nsew")
row_frame.grid_columnconfigure(0, weight=1)
removeFirstFrame = ttk.Button(row_frame, text="Remove First Frame", command=animation_removeFirstFrame)
setPathsButton = ttk.Button(row_frame, text="Set Paths", command=set_paths)
removeFirstFrame.grid(row=1, column=0, sticky="nsew")
setPathsButton.grid(row=0, column=0, sticky="nsew")

# Place other buttons appropriately
saveMesh.grid(row=2, column=0, columnspan=2, sticky="nsew")
# You can add more widgets or buttons to the third column as needed

# Configure the grid to adjust column and row weights
root.grid_rowconfigure(1, weight=1)
for i in range(3):
    root.grid_columnconfigure(i, weight=1)

# Bind events
treeviewmodel.bind("<Double-1>", model_doubleclick)
treeviewmodel.bind("<KeyRelease>", model_key)
treeviewanimation.bind("<Double-1>", animation_doubleclick)

root.mainloop()
