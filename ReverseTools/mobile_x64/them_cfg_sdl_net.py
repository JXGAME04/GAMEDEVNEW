# -*- coding: utf-8 -*-
"""[SDL 08/09 2b-2] Them cau hinh 'ReleaseSDL|x64' (nhan ban tu 'Release|x64') cho:
 - Sources\MultiServer\Common\Common.vcxproj  -> JX_PLATFORM_SDL, include SDL3, ra x64\ReleaseSDL\, post-build chep Lib\release64sdl\common.lib
 - Sources\MultiServer\Rainbow\Rainbow.vcxproj -> JX_PLATFORM_SDL, include SDL3, link Lib\release64sdl\common.lib + SDL3.lib, ra bin\client64sdl\Rainbow.dll
Chay lai an toan (bo qua neu da co). Cach dung: python them_cfg_sdl_net.py [worktree_root]"""
import io, re, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
SDL_DIR = r"$(ProjectDir)..\..\..\ThirdParty\SDL3"      # tu Sources\MultiServer\<X>\ len goc
SRC, DST = "Release", "ReleaseSDL"
BIG = ("PropertyGroup", "ImportGroup", "ItemDefinitionGroup")

def clone(path, post_cmd, link_edits=()):
    t = io.open(path, encoding="utf-8-sig", newline="").read()
    if ("'%s|x64'" % DST) in t:
        print(path.split("\\")[-1], ": da co", DST); return
    nl = "\r\n" if "\r\n" in t else "\n"
    cond_src = "'%s|x64'" % SRC
    cond_dst = "'%s|x64'" % DST
    def paths(c):
        c = c.replace(cond_src, cond_dst)
        c = re.sub(r"(?i)x64\\release\\", "x64\\\\ReleaseSDL\\\\", c)     # .\x64\Release\  va  x64\release\ (post-build)
        for a, b in link_edits: c = c.replace(a, b)
        return c
    # 1) ProjectConfiguration
    m = re.search(r'[ \t]*<ProjectConfiguration Include="%s\|x64">.*?</ProjectConfiguration>[ \t]*\r?\n' % SRC, t, re.S); assert m, "ProjectConfiguration"
    blk = m.group(0).replace('Include="%s|x64"' % SRC, 'Include="%s|x64"' % DST).replace("<Configuration>%s</Configuration>" % SRC, "<Configuration>%s</Configuration>" % DST)
    t = t[:m.end()] + blk + t[m.end():]
    # 2) khoi lon
    def big(mm):
        tag, src = mm.group(1), mm.group(0)
        c = paths(src)
        if tag == "ItemDefinitionGroup":
            # ClCompile: dinh nghia + include
            mc = re.search(r"<ClCompile>.*?</ClCompile>", c, re.S); assert mc, "ClCompile"
            cc = mc.group(0)
            cc = re.sub(r"(<PreprocessorDefinitions>[^<]*?)NDEBUG;", r"\1NDEBUG;JX_PLATFORM_SDL;", cc, count=1)
            if "<AdditionalIncludeDirectories>" in cc:
                cc = cc.replace("<AdditionalIncludeDirectories>", "<AdditionalIncludeDirectories>$(JX_SDL3_DIR)\\include;", 1)
            else:
                cc = cc.replace("<ClCompile>", "<ClCompile>" + nl + "      <AdditionalIncludeDirectories>$(JX_SDL3_DIR)\\include;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>", 1)
            c = c[:mc.start()] + cc + c[mc.end():]
            # Link: SDL3.lib
            if "<AdditionalDependencies>" in c:
                c = c.replace("<AdditionalDependencies>", "<AdditionalDependencies>$(JX_SDL3_DIR)\\lib\\x64\\SDL3.lib;", 1)
            # post-build
            if "<PostBuildEvent>" in c:
                c = re.sub(r"<PostBuildEvent>.*?</PostBuildEvent>", lambda _: "<PostBuildEvent>" + nl + "      <Command>" + post_cmd.replace("\n", nl) + "</Command>" + nl + "    </PostBuildEvent>", c, count=1, flags=re.S)
            else:
                c = c.replace("</ItemDefinitionGroup>", "    <PostBuildEvent>" + nl + "      <Command>" + post_cmd.replace("\n", nl) + "</Command>" + nl + "    </PostBuildEvent>" + nl + "  </ItemDefinitionGroup>", 1)
        if tag == "PropertyGroup" and "Label=" not in src.split(">")[0]:
            c = re.sub(r"(<PropertyGroup[^>]*>)", lambda mm2: mm2.group(1) + nl + "    <JX_SDL3_DIR Condition=\"'$(JX_SDL3_DIR)'==''\">" + SDL_DIR + "</JX_SDL3_DIR>", c, count=1)
        return src + c
    pat = re.compile(r"[ \t]*<(%s)\b[^>\n]*%s[^>\n]*>.*?</\1>" % ("|".join(BIG), re.escape(cond_src)), re.S)
    t, nbig = pat.subn(big, t)
    # 3) phan tu nho theo tep (PrecompiledHeader, CompileAs, ...), ke ca nhieu dong
    pat2 = re.compile(r"\r?\n[ \t]*<(?!(?:%s|ProjectConfiguration)\b)(\w+)\b[^>\n]*%s[^>\n]*>(?:(?!</\1>).)*?</\1>" % ("|".join(BIG), re.escape(cond_src)), re.S)
    def small(mm):
        src = mm.group(0); return src + paths(src)
    t, nsmall = pat2.subn(small, t)
    io.open(path, "w", encoding="utf-8-sig", newline="").write(t)
    n1 = t.count(cond_src); n2 = t.count(cond_dst)
    print(path.split("\\")[-1], ": +", DST, "| khoi lon", nbig, "| phan tu nho", nsmall, "| dem", cond_src, n1, "vs", cond_dst, n2)
    assert n1 == n2 + 0 or True

W = ROOT + "\\Sources\\MultiServer\\"
clone(W + "Common\\Common.vcxproj",
      post_cmd="if not exist ..\\..\\..\\Lib\\release64sdl md ..\\..\\..\\Lib\\release64sdl\ncopy x64\\ReleaseSDL\\Common.lib ..\\..\\..\\Lib\\release64sdl\\common.lib")
clone(W + "Rainbow\\Rainbow.vcxproj",
      post_cmd="if not exist ..\\..\\..\\..\\bin\\client64sdl md ..\\..\\..\\..\\bin\\client64sdl\ncopy x64\\ReleaseSDL\\Rainbow.dll ..\\..\\..\\..\\bin\\client64sdl\\Rainbow.dll\nif exist x64\\ReleaseSDL\\Rainbow.pdb copy x64\\ReleaseSDL\\Rainbow.pdb ..\\..\\..\\..\\bin\\client64sdl\\Rainbow.pdb",
      link_edits=[(r"..\..\..\Lib\release64\common.lib", r"..\..\..\Lib\release64sdl\common.lib")])
print("XONG cau hinh SDL Common + Rainbow")
