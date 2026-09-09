# -*- coding: utf-8 -*-
"""[GPU 08/09] Represent3: (1) them cau hinh 'ReleaseSDL|x64' (nhan ban tu 'Release|x64': JX_PLATFORM_SDL, include SDL3, link SDL3.lib +
Lib\\release64sdl\\engine.lib, ra x64\\ReleaseSDL\\, post-build chep bin\\client64sdl\\Represent3.dll); (2) them D3D9onGPU*.cpp vao ClCompile
(moi cau hinh, than tep rong khi khong co JX_PLATFORM_SDL); (3) D3D_Shell.cpp: nhanh Rep3Api=100 -> Rep3_CreateD3D9onGPU() trong #ifdef.
Chay lai an toan. Cach dung: python them_cfg_sdl_rep3.py [worktree_root]"""
import io, re, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
P = ROOT + "\\Sources\\Represent\\Represent3\\"
SDL_DIR = r"$(ProjectDir)..\..\..\ThirdParty\SDL3"
SRC, DST = "Release", "ReleaseSDL"
BIG = ("PropertyGroup", "ImportGroup", "ItemDefinitionGroup")

def clone(path, post_cmd, link_edits=()):
    t = io.open(path, encoding="utf-8-sig", newline="").read()
    if ("'%s|x64'" % DST) in t:
        print(path.split("\\")[-1], ": da co", DST); return
    nl = "\r\n" if "\r\n" in t else "\n"
    cond_src = "'%s|x64'" % SRC; cond_dst = "'%s|x64'" % DST
    def paths(c):
        c = c.replace(cond_src, cond_dst)
        c = re.sub(r"(?i)x64\\release\\", "x64\\\\ReleaseSDL\\\\", c)
        for a, b in link_edits: c = c.replace(a, b)
        return c
    m = re.search(r'[ \t]*<ProjectConfiguration Include="%s\|x64">.*?</ProjectConfiguration>[ \t]*\r?\n' % SRC, t, re.S); assert m
    blk = m.group(0).replace('Include="%s|x64"' % SRC, 'Include="%s|x64"' % DST).replace("<Configuration>%s</Configuration>" % SRC, "<Configuration>%s</Configuration>" % DST)
    t = t[:m.end()] + blk + t[m.end():]
    def big(mm):
        tag, src = mm.group(1), mm.group(0)
        c = paths(src)
        if tag == "ItemDefinitionGroup":
            mc = re.search(r"<ClCompile>.*?</ClCompile>", c, re.S); assert mc
            cc = mc.group(0)
            cc = re.sub(r"(<PreprocessorDefinitions>[^<]*?)NDEBUG;", r"\1NDEBUG;JX_PLATFORM_SDL;", cc, count=1)
            if "<AdditionalIncludeDirectories>" in cc: cc = cc.replace("<AdditionalIncludeDirectories>", "<AdditionalIncludeDirectories>$(JX_SDL3_DIR)\\include;", 1)
            else: cc = cc.replace("<ClCompile>", "<ClCompile>" + nl + "      <AdditionalIncludeDirectories>$(JX_SDL3_DIR)\\include;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>", 1)
            c = c[:mc.start()] + cc + c[mc.end():]
            if "<AdditionalDependencies>" in c: c = c.replace("<AdditionalDependencies>", "<AdditionalDependencies>$(JX_SDL3_DIR)\\lib\\x64\\SDL3.lib;", 1)
            c = re.sub(r"<PostBuildEvent>.*?</PostBuildEvent>", lambda _: "<PostBuildEvent>" + nl + "      <Command>" + post_cmd.replace("\n", nl) + "</Command>" + nl + "    </PostBuildEvent>", c, count=1, flags=re.S)
        if tag == "PropertyGroup" and "Label=" not in src.split(">")[0]:
            c = re.sub(r"(<PropertyGroup[^>]*>)", lambda mm2: mm2.group(1) + nl + "    <JX_SDL3_DIR Condition=\"'$(JX_SDL3_DIR)'==''\">" + SDL_DIR + "</JX_SDL3_DIR>", c, count=1)
        return src + c
    pat = re.compile(r"[ \t]*<(%s)\b[^>\n]*%s[^>\n]*>.*?</\1>" % ("|".join(BIG), re.escape(cond_src)), re.S)
    t, nbig = pat.subn(big, t)
    pat2 = re.compile(r"\r?\n[ \t]*<(?!(?:%s|ProjectConfiguration)\b)(\w+)\b[^>\n]*%s[^>\n]*>(?:(?!</\1>).)*?</\1>" % ("|".join(BIG), re.escape(cond_src)), re.S)
    t, nsmall = pat2.subn(lambda mm: mm.group(0) + paths(mm.group(0)), t)
    io.open(path, "w", encoding="utf-8-sig", newline="").write(t)
    print(path.split("\\")[-1], ": +", DST, "| khoi lon", nbig, "| phan tu nho", nsmall, "| dem", t.count(cond_src), "vs", t.count(cond_dst))

p = P + "Represent3.vcxproj"
clone(p, post_cmd="if not exist ..\\..\\..\\bin\\client64sdl md ..\\..\\..\\bin\\client64sdl\ncopy x64\\ReleaseSDL\\Represent3.dll ..\\..\\..\\bin\\client64sdl\\Represent3.dll\nif exist x64\\ReleaseSDL\\Represent3.pdb copy x64\\ReleaseSDL\\Represent3.pdb ..\\..\\..\\bin\\client64sdl\\Represent3.pdb",
      link_edits=[(r"..\..\..\Lib\release64\engine.lib", r"..\..\..\Lib\release64sdl\engine.lib")])
# ClCompile cho tep moi
t = io.open(p, encoding="utf-8-sig", newline="").read(); nl = "\r\n" if "\r\n" in t else "\n"
if "D3D9onGPU.cpp" not in t:
    a = '<ClCompile Include="D3D9on11Pal.cpp" />'; assert a in t
    t = t.replace(a, a + nl + '    <ClCompile Include="D3D9onGPU.cpp" />' + nl + '    <ClCompile Include="D3D9onGPUDev.cpp" />' + nl + '    <ClCompile Include="D3D9onGPURes.cpp" />', 1)
    io.open(p, "w", encoding="utf-8-sig", newline="").write(t); print("Represent3.vcxproj: + 3 ClCompile D3D9onGPU")
else: print("Represent3.vcxproj: da co D3D9onGPU.cpp")
# D3D_Shell.cpp: nhanh Rep3Api = 100 (byte-safe)
p2 = P + "D3D_Shell.cpp"; b = io.open(p2, "rb").read(); s = b.decode("latin-1"); o = s
TAG = "[GPU 08/09]"
if TAG not in s:
    nl2 = "\r\n" if "\r\n" in s else "\n"
    # include
    m = re.search(r'#include "D3D9on11.h"[^\n]*\n', s); assert m, "include D3D9on11.h"
    s = s[:m.end()] + "#ifdef JX_PLATFORM_SDL" + nl2 + '#include "D3D9onGPU.h"\t// ' + TAG + " D3D9 tren SDL_GPU (Rep3Api=100)" + nl2 + "#endif" + nl2 + s[m.end():]
    # nhanh: truoc 'if (g_nRep3Api == 11)'
    m = re.search(r"([ \t]*)if \(g_nRep3Api == 11\)", s); assert m, "if Rep3Api == 11"
    ind = m.group(1)
    blk = ("#ifdef JX_PLATFORM_SDL" + nl2 + ind + "if (g_nRep3Api == 100)\t// " + TAG + " lop D3D9 tren SDL_GPU (Vulkan): bo ve cho mobile, thu tren PC" + nl2
           + ind + "{" + nl2 + ind + "\tm_pD3D = Rep3_CreateD3D9onGPU();" + nl2 + ind + "\tif (m_pD3D)" + nl2 + ind + "\t\tg_nRep3ApiOn = 100;" + nl2
           + ind + "\telse" + nl2 + ind + "\t\tRep3Log(\"[REP3] Rep3Api=100 nhung khong tao duoc SDL_GPU -> lui ve D3D9\");" + nl2 + ind + "}" + nl2 + "#endif" + nl2)
    s = s[:m.start()] + blk + s[m.start():]
    assert sum(1 for c in o if ord(c) > 127) == sum(1 for c in s if ord(c) > 127)
    io.open(p2, "wb").write(s.encode("latin-1")); print("D3D_Shell.cpp: + nhanh Rep3Api=100")
else: print("D3D_Shell.cpp: da co")
print("XONG cau hinh Represent3 SDL")
