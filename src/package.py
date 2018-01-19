import sys
import pyparsing as pp
import json
import os.path
import struct

NUL = '\00'

class folder():
    def __init__(self):
        pass
        
class item():
    def __init__(self, file_path):
        with open(file_path, 'rb') as file:
            self.data = file.read()
            self.name = os.path.basename(file_path)
            
    def __len__(self):
        return len(self.data)


def org(matchlist):
    base = {'.':[]}
    while len(matchlist) > 0:
        if len(matchlist) >=2 and type(matchlist[0]) == str and type(matchlist[1]) != str:
            key = matchlist.pop(0)
            li = matchlist.pop(0)
            sub = org(li)
            base[key] = sub
        else:
            base['.'].append(matchlist.pop(0))
    return base
    
def arx(archive, tree):
    archive.write('ARX' + NUL * 3)
    length = len(tree)-1 + len(tree['.']) # Length of archive base directory
    archive.write(struct.pack('II', 0x10, length * 64 + 3))
    archive.seek(0x10)
    package(archive, tree)
    
def get_end(f):
    p = f.tell()
    f.seek(0,2)
    end = f.tell()
    f.seek(p)
    return end
    
def expand(f, amount):
    p = f.tell()
    f.seek(amount-1, 1)
    f.write('\00')
    f.seek(p)
    
    
def package(archive, tree):
    archive.write('DIR')
    length = len(tree)-1 + len(tree['.'])
    p1 = archive.tell()
    expand(archive,  64 * length + 1)

    for path in tree['.']:
        f = item(path)
        line = struct.pack('II', get_end(archive), len(f) + 3) + ('{:\00<56.56}').format(f.name)
        archive.write(line)
        p1 = archive.tell()
        archive.seek(0,2)
        archive.write('FIL')
        archive.write(f.data)
        archive.seek(p1)
        # raw_input()
        # print p1
    for key, value in tree.items():
        if key != '.':
            line = struct.pack('II', get_end(archive), (len(value)-1 + len(value['.'])) * 64 + 3) + ('{0:\00<56.56}').format(key)
            archive.write(line)
            p1 = archive.tell()
            archive.seek(0,2)
            package(archive, value)
            archive.seek(p1)
        else:
            continue
        # raw_input()
        # print p1


folder_id = pp.Word(pp.alphanums+'_')
file_id = pp.Combine(pp.Word(pp.alphanums+'_') + '.' + pp.Word(pp.alphanums))
path = pp.Combine(pp.ZeroOrMore(folder_id+ '/') + file_id)
TAB,NL,LBR,RBR = map(pp.Suppress, "\t\n{}")
file = pp.ZeroOrMore(TAB) + path
#file_body = pp.nestedExpr(LBR, RBR)

folder = pp.Forward()
block = pp.Group(pp.OneOrMore(folder | file))
folder << folder_id + LBR + block('block') + RBR

with open(sys.argv[2], 'wb') as pack_file, open(sys.argv[1]) as manifest:
    man = manifest.read()
    match = block.searchString(man)[0][0]
    # print match
    # for a,b in zip(match, match[1:]):
        # print a,b
    tree = org(match)
    # print json.dumps(tree, indent=4)
    arx(pack_file, tree)