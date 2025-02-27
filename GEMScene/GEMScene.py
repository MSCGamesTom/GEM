
import json

class InlineListEncoder(json.JSONEncoder):
    def iterencode(self, o, _one_shot=False):
        if isinstance(o, list):
            return iter(['[' + ', '.join(self.encode(e) for e in o) + ']'])
        elif isinstance(o, dict):
            if not o:
                return iter(['{}'])
            inner_indent = ' ' * (self.indent) if self.indent is not None else ''
            items = []
            for key, value in o.items():
                if isinstance(value, list):
                    encoded_value = '[' + ', '.join(self.encode(e) for e in value) + ']'
                else:
                    encoded_value = self.encode(value)
                items.append(f'{json.dumps(key)}: {encoded_value}')
            joined_items = (',\n' + inner_indent).join(items)
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
