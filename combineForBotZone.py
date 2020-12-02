includes = set()
defines = set()
codeLines = []
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
            codeLines.append(temp)
    f.close()
aio = open("./aio.cpp", mode="w", encoding="utf-8")
for include in includes:
    aio.write(include)
for define in defines:
    aio.write(define)
aio.write("using namespace std;\n")
for code in codeLines:
    aio.write(code)
aio.close()
