import random

CodeObfuscationOn = False


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
    keyWords = ['asm', 'else', 'new', 'this', 'auto', 'enum', 'malloc', 'free',
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
    builtinObjs = ["swap", "cin", "cout", "rand", "srand", "Json", "sqrt", "log", "nullptr", "main", "RAND_MAX",
                   "string", "getline", "Reader", "size", "FastWriter", "sprintf", "write", "endl",
                   "Value", "clock", "CLOCKS_PER_SEC", "parse", "asInt", "minInt", "branchNum", "tanh", "max", "min"]
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
filenames = ["gameRule.cpp", "bot.cpp", "io.cpp"]
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
aio.write("//你接下来看到的代码是经过了混淆的整活代码\n" if CodeObfuscationOn else "")
aio.write("//如果你要看真正的源码的话，请访问github\n//https://github.com/lwh9346/NoGoBot\n")
aio.write("#pragma GCC optimize(3)\n")
includes = list(includes)
defines = list(defines)
includes.sort()
defines.sort()
for include in includes:
    aio.write(include)
for define in defines:
    aio.write(define)
aio.write("using namespace std;\n")
for codeParts in codeLineParts:
    temp = ""
    for p in codeParts:
        if not CodeObfuscationOn:
            temp += p
            continue
        if not codeNames.__contains__(p):
            temp += p
            continue
        i = codeNames.index(p)
        temp += "I" if i % 2 == 0 else "l"
        while i != 0:
            if i % 3 == 0:
                temp += "1"
            if i % 3 == 1:
                temp += "l"
            if i % 3 == 2:
                temp += "I"
            i = i//3
        # temp += "/*"+p+"*/"  # 调试模式开关
    temp += "\n"
    aio.write(temp)
aio.close()
