# -*- coding: utf-8 -*-
"""[SDL 08/09 2b] Khung build "ban SDL" rieng, khong dung den cau hinh Win32/x64 thuong:
 - Engine.vcxproj : 'Release|x64'        -> 'ReleaseSDL|x64'        (JX_PLATFORM_SDL, include/lib SDL3, ra x64\\ReleaseSDL\\, lib -> Lib\\release64sdl, dll -> bin\\client64sdl)
 - Core.vcxproj   : 'Client Release|x64' -> 'Client ReleaseSDL|x64' (nhu tren, link Lib\\release64sdl\\engine.lib)
 - S3Client.vcxproj: 'ReleaseSDL|x64' (da co) -> link Lib\\release64sdl (CoreClient/engine), ra bin\\client64sdl\\GameSDL.exe (+ SDL3.dll)
Chay lai an toan. Cach dung: python them_cfg_sdl.py <worktree_root>"""
import io, re, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
SDL_DIR = r"$(ProjectDir)..\..\ThirdParty\SDL3"

def clone_project(path, src_cfg, dst_cfg, edits, post_cmd, extra_prop=""):
    t = io.open(path, encoding="utf-8-sig", newline="").read()
    if ("'%s|x64'" % dst_cfg) in t:
        print(path.split("\\")[-1], ": da co", dst_cfg); return t
    nl = "\r\n" if "\r\n" in t else "\n"
    # ProjectConfiguration
    m = re.search(r'([ \t]*<ProjectConfiguration Include="%s\|x64">.*?</ProjectConfiguration>[ \t]*\r?\n)' % re.escape(src_cfg), t, re.S); assert m, "ProjectConfiguration"
    blk = m.group(1).replace('Include="%s|x64"' % src_cfg, 'Include="%s|x64"' % dst_cfg).replace("<Configuration>%s</Configuration>" % src_cfg, "<Configuration>%s</Configuration>" % dst_cfg)
    t = t[:m.end()] + blk + t[m.end():]
    def clone_block(mm):
        tag = mm.group(1); src = mm.group(0)
        c = src.replace("'%s|x64'" % src_cfg, "'%s|x64'" % dst_cfg)
        for a, b in edits: c = c.replace(a, b)
        if tag == "ItemDefinitionGroup":
            c = re.sub(r"<PreprocessorDefinitions>NDEBUG;", "<PreprocessorDefinitions>NDEBUG;JX_PLATFORM_SDL;", c, count=1)
            c = c.replace("<AdditionalIncludeDirectories>", "<AdditionalIncludeDirectories>$(JX_SDL3_DIR)\\include;", 1)
            c = c.replace("<AdditionalDependencies>", "<AdditionalDependencies>$(JX_SDL3_DIR)\\lib\\x64\\SDL3.lib;", 1)
            c = re.sub(r"<Command>.*?</Command>", "<Command>" + post_cmd.replace("\\", "\\\\") + "</Command>", c, count=1, flags=re.S)
        if tag == "PropertyGroup" and "<OutDir>" in c and "Label=" not in c.split(">")[0]:
            c = c.replace("<LinkIncremental>", extra_prop + "<JX_SDL3_DIR Condition=\"'$(JX_SDL3_DIR)'==''\">" + SDL_DIR + "</JX_SDL3_DIR>" + nl + "    <LinkIncremental>", 1)
        return src + c
    pat = re.compile(r"[ \t]*<(PropertyGroup|ImportGroup|ItemDefinitionGroup)\b[^>\n]*'%s\|x64'[^>\n]*>.*?</\1>" % re.escape(src_cfg), re.S)
    t, nblk = pat.subn(clone_block, t)
    def clone_small(mm):
        src = mm.group(0); return src + src.replace("'%s|x64'" % src_cfg, "'%s|x64'" % dst_cfg)
    pat2 = re.compile(r"\r?\n[ \t]*<(PrecompiledHeader|PrecompiledHeaderFile|ExcludedFromBuild|ObjectFileName|CompileAs|AdditionalOptions|PreprocessorDefinitions|Optimization|AssemblerOutput|BasicRuntimeChecks|RuntimeLibrary)\b[^>\n]*'%s\|x64'[^>\n]*>(?:(?!</\1>).)*?</\1>" % re.escape(src_cfg), re.S)
    t, nsmall = pat2.subn(clone_small, t)
    io.open(path, "w", encoding="utf-8-sig", newline="").write(t)
    print(path.split("\\")[-1], ": +", dst_cfg, "| khoi", nblk, "| phan tu tep", nsmall)
    return t

W = ROOT + "\\Sources\\"
# Engine
clone_project(W + "Engine\\Engine.vcxproj", "Release", "ReleaseSDL",
    edits=[(r".\x64\Release\<", r".\x64\ReleaseSDL\<"), (r".\x64\Release\Engine", r".\x64\ReleaseSDL\Engine")],
    post_cmd="if not exist ..\\..\\Lib\\release64sdl md ..\\..\\Lib\\release64sdl\r\nif not exist ..\\..\\bin\\client64sdl md ..\\..\\bin\\client64sdl\r\ncopy x64\\ReleaseSDL\\Engine.lib ..\\..\\Lib\\release64sdl\\engine.lib\r\ncopy x64\\ReleaseSDL\\Engine.dll ..\\..\\bin\\client64sdl\\Engine.dll\r\ncopy x64\\ReleaseSDL\\Engine.pdb ..\\..\\bin\\client64sdl\\Engine.pdb")
# Core
clone_project(W + "Core\\Core.vcxproj", "Client Release", "Client ReleaseSDL",
    edits=[(r".\x64\ClientRelease\<", r".\x64\ClientReleaseSDL\<"), (r".\x64\ClientRelease\Core", r".\x64\ClientReleaseSDL\Core"),
           (r"x64\ClientRelease\CoreClient.dll", r"x64\ClientReleaseSDL\CoreClient.dll"), (r".\x64\ClientRelease\CoreClient.lib", r".\x64\ClientReleaseSDL\CoreClient.lib"),
           ("../../lib/release64/;", "../../lib/release64sdl/;../../lib/release64/;"), (r"..\..\Lib\release64\engine.lib", r"..\..\Lib\release64sdl\engine.lib")],
    post_cmd="if not exist ..\\..\\Lib\\release64sdl md ..\\..\\Lib\\release64sdl\r\nif not exist ..\\..\\bin\\client64sdl md ..\\..\\bin\\client64sdl\r\ncopy x64\\ClientReleaseSDL\\CoreClient.lib ..\\..\\Lib\\release64sdl\\CoreClient.lib\r\ncopy x64\\ClientReleaseSDL\\CoreClient.dll ..\\..\\bin\\client64sdl\\CoreClient.dll\r\ncopy x64\\ClientReleaseSDL\\CoreClient.pdb ..\\..\\bin\\client64sdl\\CoreClient.pdb")
# S3Client ReleaseSDL: link release64sdl + ra bin\client64sdl
p = W + "S3Client\\S3Client.vcxproj"
t = io.open(p, encoding="utf-8-sig", newline="").read()
if "release64sdl" not in t:
    m = re.search(r"<ItemDefinitionGroup Condition=\"'\$\(Configuration\)\|\$\(Platform\)'=='ReleaseSDL\|x64'\">.*?</ItemDefinitionGroup>", t, re.S); assert m
    c = m.group(0)
    c = c.replace(r"..\..\Lib\release64\CoreClient.lib;..\..\Lib\release64\engine.lib;", r"..\..\Lib\release64sdl\CoreClient.lib;..\..\Lib\release64sdl\engine.lib;")
    c = c.replace(r"<AdditionalLibraryDirectories>..\..\Lib\release64;", r"<AdditionalLibraryDirectories>..\..\Lib\release64sdl;..\..\Lib\release64;")
    c = re.sub(r"<Command>.*?</Command>", "<Command>if not exist ..\\\\..\\\\bin\\\\client64sdl md ..\\\\..\\\\bin\\\\client64sdl\r\ncopy x64\\\\ReleaseSDL\\\\GameSDL.exe ..\\\\..\\\\bin\\\\client64sdl\\\\GameSDL.exe\r\ncopy x64\\\\ReleaseSDL\\\\GameSDL.pdb ..\\\\..\\\\bin\\\\client64sdl\\\\GameSDL.pdb\r\ncopy \"$(JX_SDL3_DIR)\\\\lib\\\\x64\\\\SDL3.dll\" ..\\\\..\\\\bin\\\\client64sdl\\\\SDL3.dll</Command>", c, count=1, flags=re.S)
    t = t[:m.start()] + c + t[m.end():]
    io.open(p, "w", encoding="utf-8-sig", newline="").write(t); print("S3Client.vcxproj: ReleaseSDL -> release64sdl / bin\\client64sdl")
else:
    print("S3Client.vcxproj: da tro release64sdl")
