
import json

class InlineListEncoder(json.JSONEncoder):
    def iterencode(self, o, _one_shot=False):
        if isinstance(o, list):
            # Inline encode the list.
            return iter(['[' + ', '.join(self.encode(e) for e in o) + ']'])
        elif isinstance(o, dict):
            # For dictionaries, mimic pretty printing with inline lists.
            if not o:
                return iter(['{}'])
            # inner_indent is used for the key-value pairs.
            inner_indent = ' ' * (self.indent) if self.indent is not None else ''
            items = []
            for key, value in o.items():
                if isinstance(value, list):
                    encoded_value = '[' + ', '.join(self.encode(e) for e in value) + ']'
                else:
                    encoded_value = self.encode(value)
                items.append(f'{json.dumps(key)}: {encoded_value}')
            # Join the items with commas and newlines.
            joined_items = (',\n' + inner_indent).join(items)
            # The final closing brace has no indent.
            return iter([f'{{\n{inner_indent}{joined_items}\n}}'])
        else:
            return super().iterencode(o, _one_shot)

class GEMSceneItem:
    def __init__(self):
        self.base = {}
        self.base["filename"] = ""
        self.base["world"] = [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1]
    def addWorld(self, w):
        self.base["world"] = w
    def addFilename(self, filename):
        self.base["filename"] = filename
    def addAttribute(self, name, attribute):
        self.base[name] = attribute

class GEMScene:
    def __init__(self):
        self.base = {}
        self.items = []
    def addSceneItem(self, item):
        self.items.append(item.base)
    def addRendererItem(self, name, attribute):
        self.base[name] = attribute
    def save(self, filename):
        self.base["instances"] = self.items
        data = json.dumps(self.base, cls=InlineListEncoder, indent=4)
        with open(filename, "w") as file:
            file.write(data)

##item = GEMSceneItem()
##item.addFilename("a.gem")
##item.addAttribute("bsdf", "diffuse")
##item.addAttribute("diffuse", "albedo.png")
##
##scene = GEMScene()
##scene.addSceneItem(item)
##item.addFilename("b.gem")
##scene.addSceneItem(item)
##scene.addRendererItem("width", "1920")
##scene.addRendererItem("height", "1080")
##scene.save("scene1.json")
