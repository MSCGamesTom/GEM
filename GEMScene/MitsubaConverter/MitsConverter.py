

from GEMScene import GEMScene, GEMSceneItem
import xml.etree.ElementTree as parser
import os
import shutil
from PIL import Image
import sys

scene = GEMScene()

def parseFilmData(sensor):
    global scene
    film = sensor.find('film')
    width = int(film.find("integer[@name='width']").get("value"))
    height = int(film.find("integer[@name='height']").get("value"))
    scene.addRendererItem("width", f"{width}")
    scene.addRendererItem("height", f"{height}")
    fov_elem = sensor.find("float[@name='fov']")
    fov = float(fov_elem.get("value"))
    scene.addRendererItem("fov", f"{fov}")
    transform = sensor.find("transform[@name='toWorld']")
    matrix_elem = transform.find('matrix')
    if matrix_elem is not None:
        matrix_str = matrix_elem.get("value").replace(",", "")
        matrix_str = matrix_str.replace("  ", " ")
        view = [float(v.strip()) for v in matrix_str.split(" ") if len(v) > 0]
        up = [view[1], view[5], view[9]]
        viewdir = [view[2], view[6], view[10]]
        viewfrom = [view[3], view[7], view[11]]
        to = [x + y for x, y in zip(viewfrom, viewdir)]
        scene.addRendererItem("from", ' '.join([str(x) for x in viewfrom]))
        scene.addRendererItem("to", ' '.join([str(x) for x in to]))
        scene.addRendererItem("up", ' '.join([str(x) for x in up]))
    else:
        lookat = transform.find('lookat')
        scene.addRendererItem("from", lookat.get("origin"))
        scene.addRendererItem("to", lookat.get("target"))
        scene.addRendererItem("up", lookat.get("up"))
    #scene.addRendererItem("view", view)

def parseEmitter(emitter, path, savepath):
    global scene
    if emitter is None:
        return
    emitter_type = emitter.attrib['type']
    if emitter_type == "envmap":
        filenameinit = emitter.find(".//string").attrib['value']
        filename = filenameinit
        if "/" in filenameinit:
            filename = filenameinit.split("/")[-1]
        shutil.copy(f"{path}/{filenameinit}", f"{savepath}/{filename}")
        scene.addRendererItem("envmap", filename)
        return
    if emitter_type == "sunsky":
        filename = "Sky.hdr"
        turbidity = emitter.find("float[@name='turbidity']").get("value")
        skyScale = emitter.find("float[@name='skyScale']").get("value")
        sunScale = emitter.find("float[@name='sunScale']").get("value")
        sunRadiusScale = emitter.find("float[@name='sunRadiusScale']").get("value")
        dirx = emitter.find("vector[@name='sunDirection']").get("x")
        diry = emitter.find("vector[@name='sunDirection']").get("y")
        dirz = emitter.find("vector[@name='sunDirection']").get("z")
        command = f"-turbidity {turbidity} -skyScale {skyScale} -sunScale {sunScale} -sunRadiusScale {sunRadiusScale} -sunDirectionX {dirx} -sunDirectionY {diry} -sunDirectionZ {dirz} -filename {savepath}/{filename}"
        os.system(f"SkyCreator.exe {command}")
        scene.addRendererItem("envmap", filename)
        return
    if emitter_type == "sun":
        filename = "Sky.hdr"
        turbidity = emitter.find("float[@name='turbidity']").get("value")
        skyScale = "0"
        sunScale = emitter.find("float[@name='sunScale']").get("value")
        sunRadiusScale = emitter.find("float[@name='sunRadiusScale']").get("value")
        dirx = emitter.find("vector[@name='sunDirection']").get("x")
        diry = emitter.find("vector[@name='sunDirection']").get("y")
        dirz = emitter.find("vector[@name='sunDirection']").get("z")
        command = f"-turbidity {turbidity} -skyScale {skyScale} -sunScale {sunScale} -sunRadiusScale {sunRadiusScale} -sunDirectionX {dirx} -sunDirectionY {diry} -sunDirectionZ {dirz} -filename {savepath}/{filename}"
        os.system(f"SkyCreator.exe {command}")
        scene.addRendererItem("envmap", filename)
        return
    

def attr(attrList, name):
    for item in attrList:
        if item[0] == name:
            return item[1]
    return None

def convertDist(mitsMaterial, dist):
    if dist == None:
        return "0"
    if dist == "ggx":
        if attr(mitsMaterial, "alpha") is None:
            return "0"
        alpha = float(attr(mitsMaterial, "alpha"))
        roughness = (alpha / 1.62142)
        roughness = roughness * roughness
        return str(roughness)
    if dist == "beckmann":
        if attr(mitsMaterial, "alpha") is None:
            return "0"
        alpha = float(attr(mitsMaterial, "alpha"))
        roughness = (alpha / 1.62142)
        roughness = roughness * roughness
        return str(roughness)
    print(dist)
    exit(0)

def isFilename(str):
    exts = [".png", ".tga", ".jpg", ".bmp", ".jpeg"]
    for ext in exts:
        if ext in str:
            return True
    return False

def makeReflectanceTexture(savepath, strData):
    if strData is None:
        vals = [255, 255, 255]
        img = Image.new("RGB", (1, 1), (vals[0], vals[1], vals[2]))
        filename = savepath + "/1_1_1.png"
        img.save(filename)
        return "1_1_1.png"
    if isFilename(strData):
        return strData
    vals = strData.replace(",", " ")
    vals = vals.replace("  ", " ")
    vals = [int(float(v) * 255) for v in vals.split(" ")]
    img = Image.new("RGB", (1, 1), (vals[0], vals[1], vals[2]))
    vals = strData.replace(",", " ")
    vals = vals.replace("  ", "_")
    filename = savepath + "/" + vals + ".png"
    img.save(filename)
    return vals + ".png"

def fixList(data):
    if data is None:
        print("Attribute not found")
        return "1"
    data = data.replace(",", " ")
    data = data.replace("  ", " ")
    return data

def materialConverter(name, item, mitsMaterial, path, savepath):
    # Find material
    material = attr(mitsMaterial, "bsdf")
    if name == "Mirror":
        item.addAttribute("bsdf", "mirror")
        item.addAttribute("reflectance", makeReflectanceTexture(savepath, attr(mitsMaterial, "specularReflectance")))
        return
    # Convert attributes for each
    if material == "roughconductor":
        item.addAttribute("bsdf", "conductor")
        item.addAttribute("roughness", convertDist(mitsMaterial, attr(mitsMaterial, "distribution")))
        item.addAttribute("extEta", attr(mitsMaterial, "extEta"))
        item.addAttribute("reflectance", makeReflectanceTexture(savepath, attr(mitsMaterial, "specularReflectance")))
        item.addAttribute("eta", fixList(attr(mitsMaterial, "eta")))
        item.addAttribute("k", fixList(attr(mitsMaterial, "k")))
        return
    if material == "conductor":
        item.addAttribute("bsdf", material)
        item.addAttribute("roughness", "0")
        item.addAttribute("extEta", attr(mitsMaterial, "extEta"))
        item.addAttribute("reflectance", makeReflectanceTexture(savepath, attr(mitsMaterial, "specularReflectance")))
        item.addAttribute("eta", fixList(attr(mitsMaterial, "eta")))
        item.addAttribute("k", fixList(attr(mitsMaterial, "k")))
        return
    if material == "diffuse":
        item.addAttribute("bsdf", material)
        item.addAttribute("reflectance", makeReflectanceTexture(savepath, attr(mitsMaterial, "reflectance")))
        return
    if material == "roughplastic":
        item.addAttribute("bsdf", "plastic")
        item.addAttribute("roughness", convertDist(mitsMaterial, attr(mitsMaterial, "distribution")))
        item.addAttribute("intIOR", attr(mitsMaterial, "intIOR"))
        item.addAttribute("extIOR", attr(mitsMaterial, "extIOR"))
        item.addAttribute("reflectance", makeReflectanceTexture(savepath, attr(mitsMaterial, "diffuseReflectance")))
        return
    if material == "plastic":
        item.addAttribute("bsdf", "plastic")
        item.addAttribute("roughness", convertDist(mitsMaterial, attr(mitsMaterial, "distribution")))
        item.addAttribute("intIOR", attr(mitsMaterial, "intIOR"))
        item.addAttribute("extIOR", attr(mitsMaterial, "extIOR"))
        item.addAttribute("reflectance", makeReflectanceTexture(savepath, attr(mitsMaterial, "diffuseReflectance")))
        return
    if material == "dielectric":
        item.addAttribute("bsdf", "glass")
        item.addAttribute("reflectance", makeReflectanceTexture(savepath, attr(mitsMaterial, "specularReflectance")))
        return
    if material == "thindielectric":
        item.addAttribute("bsdf", "glass")
        item.addAttribute("intIOR", attr(mitsMaterial, "intIOR"))
        item.addAttribute("extIOR", attr(mitsMaterial, "extIOR"))
        item.addAttribute("reflectance", makeReflectanceTexture(savepath, attr(mitsMaterial, "specularReflectance")))
        return
    if material == "mask":
        item.addAttribute("bsdf", material)
        item.addAttribute("reflectance", makeReflectanceTexture(savepath, attr(mitsMaterial, "specularReflectance")))
        return
    if material == "roughdiffuse":
        item.addAttribute("bsdf", "orennayar")
        item.addAttribute("reflectance", makeReflectanceTexture(savepath, attr(mitsMaterial, "reflectance")))
        item.addAttribute("alpha", attr(mitsMaterial, "alpha"))
        return
    if material == "roughdielectric":
        item.addAttribute("bsdf", "dielectric")
        item.addAttribute("reflectance", makeReflectanceTexture(savepath, attr(mitsMaterial, "reflectance")))
        item.addAttribute("roughness", convertDist(mitsMaterial, attr(mitsMaterial, "distribution")))
        item.addAttribute("intIOR", attr(mitsMaterial, "intIOR"))
        item.addAttribute("extIOR", attr(mitsMaterial, "extIOR"))
    if material == "coating":
        item.addAttribute("coatingBsdfRoughness", "0")
        item.addAttribute("coatingThickness", attr(mitsMaterial, "thickness"))
        item.addAttribute("coatingIntIOR", attr(mitsMaterial, "intIOR"))
        item.addAttribute("coatingExtIOR", attr(mitsMaterial, "extIOR"))
        item.addAttribute("coatingSigmaA", fixList(attr(mitsMaterial, "sigmaA")))
        subIndex = -1
        for index, element in enumerate(mitsMaterial):
            if element[0] == "bsdf":
                if subIndex > -1:
                    subIndex = index
                    break
                else:
                    subIndex = index
        mitsMaterial1 = mitsMaterial[subIndex:]
        materialConverter(name, item, mitsMaterial1, path, savepath)
        return
    print(material)
    print(mitsMaterial)
        

def parseMaterial(mat, path, savepath, matList):
    if mat.tag == "bsdf":
        if mat.attrib['type'] == 'twosided':
            for c in mat:
                parseMaterial(c, path, savepath, matList)
        else:
            matList.append(["bsdf", mat.attrib['type']])
            for c in mat:
                parseMaterial(c, path, savepath, matList)
        return
    name = mat.attrib.get('name')
    value = mat.attrib.get('value')
    if mat.tag == "rgb":
        matList.append([name, value])
    if mat.tag == "float":
        matList.append([name, value])
    if mat.tag == "string":
        matList.append([name, value])
    if mat.tag == "texture":
        filenameinit = ""
        for c in mat:
            if c.attrib.get('name') == "filename":
                filenameinit = c.attrib.get('value')
                break
        if filenameinit != "":
            filename = filenameinit
            if "/" in filenameinit:
                filename = filenameinit.split("/")[-1]
            shutil.copy(f"{path}/{filenameinit}", f"{savepath}/{filename}")
            matList.append([name, filename])

def convertMits(path, savepath):
    global scene
    tree = parser.parse(f"{path}/scene.xml")
    root = tree.getroot()
    sensor = root.find('sensor')
    parseFilmData(sensor)
    parseEmitter(root.find('emitter'), path, savepath)
    bsdf_elements = root.findall(".//bsdf[@id]")
    bsdf_dict = {elem.get("id"): elem for elem in bsdf_elements}
    shapes = root.findall("shape")
    nShapes = len(shapes)
    for index, shape in enumerate(shapes, start=1):
        item = GEMSceneItem()
        shapeType = shape.get("type")
        if shapeType == "obj":
            filename = shape.find("string[@name='filename']").get("value")
            print("obj"+ ": " + filename)
            if len(filename) > 0:
                gemfilename = filename.replace(".obj", ".gem")
                if "/" in gemfilename:
                    gemfilename = gemfilename.split("/")[-1]
                os.system(f"python ModelExporter.py {path}/{filename} {savepath}/ {gemfilename}")
                item.addFilename(gemfilename)
        else:
            if shapeType == "rectangle":
                os.system(f"python ModelExporter.py Rectangle.obj {savepath}/ Rectangle.gem")
                item.addFilename("Rectangle.gem")
            if shapeType == "cube":
                os.system(f"python ModelExporter.py Cube.obj {savepath}/ Cube.gem")
                item.addFilename("Cube.gem")
            if shapeType == "disk":
                os.system(f"python ModelExporter.py Disk.obj {savepath}/ Disk.gem")
                item.addFilename("Disk.gem")
            if shapeType == "sphere":
                os.system(f"python ModelExporter.py Sphere.obj {savepath}/ Sphere.gem")
                item.addFilename("Sphere.gem")
            print(shapeType)
        transform_elem = shape.find("transform[@name='toWorld']")
        if transform_elem is not None:
            matrix_elem = transform_elem.find("matrix")
            if matrix_elem is not None:
                matrix_str = matrix_elem.get("value")
                item.addWorld([float(x) for x in matrix_str.split()])
        ref_elem = shape.find("ref")
        if ref_elem is not None:
            name = ref_elem.get("id")
            matList = []
            parseMaterial(bsdf_dict[name], path, savepath, matList)
            materialConverter(name, item, matList, path, savepath)
        else:
            item.addAttribute("bsdf", "diffuse")
            item.addAttribute("reflectance", makeReflectanceTexture(savepath, "0 0 0"))
        if shape.find("emitter"):
            emitter = shape.find("emitter")
            emission = emitter.find("rgb[@name='radiance']").get("value")
            emission = emission.replace(",", " ")
            emission = emission.replace("  ", " ")
            item.addAttribute("emission", emission)
        scene.addSceneItem(item)
        print(f"{index} of {nShapes}")
    scene.save(f"{savepath}/scene.json")

convertMits(sys.argv[1], sys.argv[2])
#convertMits("Mits/bathroom", "bathroom")
