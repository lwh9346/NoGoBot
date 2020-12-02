import random


def getParts(line: str):
    # 假定没有英文-数字混合类型的名称
    strs = []
    names = []
    namesTemp = []
    l = len(line)
    line += " "
    temp = ""
    isName = False
    inQuatation = False
    spilters = [" ", "+", "-", "*", "/", "%",
                "&", "|", "=", ";", "{", "}", ":",
                "[", "]", ".", ",", "(", ")", "<", ">", "!", "?",
                "1", "2", "3", "4", "5", "6", "7", "8", "9", "0"]
    keyWords = ['asm', 'else', 'new', 'this', 'auto', 'enum',
                'operator', 'throw', 'bool', 'explicit', 'private', 'true',
                'break', 'export', 'protected', 'try', 'case', 'extern',
                'public', 'typedef', 'catch', 'false', 'register', 'typeid',
                'char', 'float', 'reinterpret_cast', 'typename', 'class',
                'for', 'return', 'union', 'const', 'friend', 'short',
                'unsigned', 'const_cast', 'goto', 'signed', 'usingcontinue',
                'if', 'sizeof', 'virtual', 'default', 'inline', 'static',
                'void', 'delete', 'int', 'static_cast', 'volatile', 'do',
                'long', 'struct', 'wchar_t', 'double', 'mutable', 'switch',
                'while', 'dynamic_cast', 'namespace', 'template', 'continue']
    builtinObjs = ["swap", "cin", "cout", "rand", "srand", "Json", "sqrt", "log", "nullptr", "main",
                   "string", "getline", "Reader", "size", "FastWriter", "sprintf", "write", "endl",
                   "Value", "clock", "CLOCKS_PER_SEC", "parse", "asInt", "minInt", "branchNum"]
    for i in range(l):
        if line[i] == '"':
            inQuatation = not inQuatation
            temp += line[i]
            continue
        if spilters.__contains__(line[i]) or inQuatation:
            if not isName:
                temp += line[i]
            else:
                if not temp == "":
                    strs.append(temp)
                    names.append(temp)
                temp = line[i]
                isName = False
        else:
            if isName:
                temp += line[i]
            else:
                if not temp == "":
                    strs.append(temp)
                temp = line[i]
                isName = True
    if not temp.strip() == "":
        strs.append(temp.strip())
    for name in names:
        if not keyWords.__contains__(name) and not builtinObjs.__contains__(name) and not name[0] == "\"":
            namesTemp.append(name)
    names = namesTemp
    return strs, names


includes = set()
defines = set()
codeLineParts = []
codeNames = set()
filenames = ["./validPosition.cpp", "./mctNode.cpp", "./main.cpp"]
for filename in filenames:
    f = open(filename, encoding="utf-8")
    for i, l in enumerate(f):
        sp = l.split()
        if len(sp) == 0:
            continue
        if sp[0] == "#include":
            if not filenames.__contains__(sp[1][1:-1]):
                includes.add(l)
            continue
        if sp[0] == "#define":
            defines.add(l)
            continue
        if sp[0] == "using":
            continue
        for j in range(len(sp)):
            if len(sp[j]) < 2:
                continue
            if sp[j][:2] == "//":
                sp[j] = ""
        temp = " ".join(sp)+"\n"
        if not temp == "\n":
            parts, names = getParts(temp)
            codeLineParts.append(parts)
            for name in names:
                codeNames.add(name)
    f.close()
codeNames = list(codeNames)
random.seed(233)
random.shuffle(codeNames)
aio = open("./aio.cpp", mode="w", encoding="utf-8")
for include in includes:
    aio.write(include)
for define in defines:
    aio.write(define)
aio.write("using namespace std;\n")
for codeParts in codeLineParts:
    temp = ""
    for p in codeParts:
        if not codeNames.__contains__(p):
            temp += p
            continue
        i = codeNames.index(p)
        temp += "O"
        temp += "o" if i%6==0 else "0"
        temp += "o" if i%6==1 else "O"
        temp += "O" if i%6==2 else "0"
        temp += "o" if i%6==3 else "O"
        temp += "0" if i%6==4 else "O"
        temp += "o" if i%6==5 else "O"
        temp += str(i//6)
        # temp += "/*"+p+"*/"  # 调试模式开关
    temp += "\n"
    aio.write(temp)
aio.close()
