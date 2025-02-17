import ctypes
import codecs
import os
import shutil
import sys

class Exporter:
    def __init__(self):
        os.add_dll_directory(os.getcwd())
        self.assimp = ctypes.CDLL("assimp-vc143-mt.dll")
        self.exp = ctypes.CDLL("ModelExporter.dll")
        self.exp.initialize.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
        self.exp.addModel.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_char_p, ctypes.c_int]
        self.exp.addModelGeometry.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_char_p, ctypes.c_int]
        self.exp.load.argtypes = [ctypes.c_char_p]
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
    def addModelGeometry(self, filename, importMesh, importAnimation, useTransforms, animationName):
        self.exp.addModelGeometry(self.to_bytes(filename), importMesh, importAnimation, useTransforms, self.to_bytes(animationName), 0)
    def load(self, filename):
        self.exp.load(self.to_bytes(filename))
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
# inputfilename path outputfilename
inputfilename = sys.argv[1]
path = sys.argv[2]
outputfilename = sys.argv[3]
savePath = path
texturePath = path

exporter.initialize(outputfilename, savePath, texturePath)
exporter.addModelGeometry(inputfilename, 1, 0, 1, "")
exporter.save(outputfilename)
