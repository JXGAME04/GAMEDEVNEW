# -*- coding: utf-8 -*-
"""[SDL 08/09] Them cau hinh 'ReleaseSDL|x64' vao S3Client.vcxproj = ban sao 'Release|x64' + JX_PLATFORM_SDL;SDL_MAIN_HANDLED,
include/lib SDL3 ($(JX_SDL3_DIR), mac dinh E:\\vcpkg-master\\installed\\x64-windows), ra GameSDL.exe o .\\x64\\ReleaseSDL\\,
post-build chep GameSDL.exe + SDL3.dll vao ..\\..\\bin\\client64\\. Them Platform\\KSdlApp.cpp/.h (chi bien dich o ReleaseSDL|x64).
Khong dung den cau hinh khac (Win32/x64 thuong y nguyen). Chay lai an toan (co 'ReleaseSDL|x64' thi bo qua).
Cach dung: python them_sdl_cfg.py <worktree_root>"""
import io, re, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
p = ROOT + r"\Sources\S3Client\S3Client.vcxproj"
t = io.open(p, encoding="utf-8-sig").read()
if "ReleaseSDL|x64" in t:
    print("S3Client.vcxproj: da co ReleaseSDL|x64"); sys.exit(0)
nl = "\r\n" if "\r\n" in t else "\n"
SDL_DIR_DEF = r"$(ProjectDir)..\..\ThirdParty\SDL3"   # goi SDL3-devel-3.2.14-VC (include + lib\x64), khong can vcpkg

# a) ProjectConfiguration
m = re.search(r'(\s*<ProjectConfiguration Include="Release\|x64">.*?</ProjectConfiguration>)', t, re.S)
assert m, "khong thay ProjectConfiguration Release|x64"
blk = m.group(1).replace('Include="Release|x64"', 'Include="ReleaseSDL|x64"').replace("<Configuration>Release</Configuration>", "<Configuration>ReleaseSDL</Configuration>")
t = t[:m.end()] + blk + t[m.end():]

# b) clone cac khoi lon co dieu kien Release|x64
def clone_block(mm):
    tag = mm.group(1); src = mm.group(0)
    c = src.replace("'Release|x64'", "'ReleaseSDL|x64'")
    c = c.replace(r".\x64\Release\\", r".\x64\ReleaseSDL\\").replace(r".\x64\Release\ ", r".\x64\ReleaseSDL\ ")
    c = c.replace(r".\x64\Release\<", r".\x64\ReleaseSDL\<").replace(r".\x64\Release\S3Client", r".\x64\ReleaseSDL\S3Client")
    c = c.replace("x64/Release/Game.exe", "x64/ReleaseSDL/GameSDL.exe").replace(r"x64\Release\Game", r"x64\ReleaseSDL\GameSDL")
    c = c.replace(r"<ObjectFileName>.\x64\Release\</ObjectFileName>", r"<ObjectFileName>.\x64\ReleaseSDL\</ObjectFileName>")
    c = c.replace(r"<ProgramDataBaseFileName>.\x64\Release\</ProgramDataBaseFileName>", r"<ProgramDataBaseFileName>.\x64\ReleaseSDL\</ProgramDataBaseFileName>")
    c = c.replace(r"<AssemblerListingLocation>.\x64\Release\</AssemblerListingLocation>", r"<AssemblerListingLocation>.\x64\ReleaseSDL\</AssemblerListingLocation>")
    c = c.replace(r"<OutDir>.\x64\Release\</OutDir>", r"<OutDir>.\x64\ReleaseSDL\</OutDir>").replace(r"<IntDir>.\x64\Release\</IntDir>", r"<IntDir>.\x64\ReleaseSDL\</IntDir>")
    if tag == "ItemDefinitionGroup":
        c = c.replace("<PreprocessorDefinitions>S3_CLIENT;", "<PreprocessorDefinitions>S3_CLIENT;JX_PLATFORM_SDL;SDL_MAIN_HANDLED;", 1)
        c = c.replace("<AdditionalIncludeDirectories>", "<AdditionalIncludeDirectories>$(JX_SDL3_DIR)\\include;", 1)
        c = c.replace("<AdditionalDependencies>", "<AdditionalDependencies>$(JX_SDL3_DIR)\\lib\\x64\\SDL3.lib;", 1)
        c = re.sub(r"<Command>.*?</Command>",
                   "<Command>if not exist ..\\\\..\\\\bin\\\\client64\\\\ md ..\\\\..\\\\bin\\\\client64\\\\" + nl +
                   "copy x64\\\\ReleaseSDL\\\\GameSDL.exe ..\\\\..\\\\bin\\\\client64\\\\GameSDL.exe" + nl +
                   "copy x64\\\\ReleaseSDL\\\\GameSDL.pdb ..\\\\..\\\\bin\\\\client64\\\\GameSDL.pdb" + nl +
                   "copy \"$(JX_SDL3_DIR)\\\\lib\\\\x64\\\\SDL3.dll\" ..\\\\..\\\\bin\\\\client64\\\\SDL3.dll</Command>", c, count=1, flags=re.S)
    if tag == "PropertyGroup" and "<OutDir>" in c and "Label=" not in c.split(">")[0]:
        c = c.replace("<LinkIncremental>", "<TargetName>GameSDL</TargetName>" + nl + "    <JX_SDL3_DIR Condition=\"'$(JX_SDL3_DIR)'==''\">" + SDL_DIR_DEF + "</JX_SDL3_DIR>" + nl + "    <LinkIncremental>", 1)
    return src + c
pat = re.compile(r"[ \t]*<(PropertyGroup|ImportGroup|ItemDefinitionGroup)\b[^>\n]*'Release\|x64'[^>\n]*>.*?</\1>", re.S)
t, nblk = pat.subn(clone_block, t)
print("khoi lon nhan ban:", nblk)

# c) phan tu theo tep (PrecompiledHeader / ExcludedFromBuild / ... ) co dieu kien Release|x64 -> nhan ban voi ReleaseSDL
def clone_small(mm):
    src = mm.group(0)
    return src + src.replace("'Release|x64'", "'ReleaseSDL|x64'")
pat2 = re.compile(r"\r?\n[ \t]*<(PrecompiledHeader|PrecompiledHeaderFile|ExcludedFromBuild|ObjectFileName|CompileAs|AdditionalOptions|PreprocessorDefinitions|Optimization)\b[^>\n]*'Release\|x64'[^>\n]*>(?:(?!</\1>).)*?</\1>", re.S)
t, nsmall = pat2.subn(clone_small, t)
print("phan tu theo tep nhan ban:", nsmall)

# d) them KSdlApp.cpp/.h
a = '<ClCompile Include="S3Client.cpp" />'
assert t.count(a) == 1
t = t.replace(a, a + nl + '    <ClCompile Include="Platform\\KSdlApp.cpp">' + nl +
              "      <ExcludedFromBuild Condition=\"'$(Configuration)|$(Platform)'!='ReleaseSDL|x64'\">true</ExcludedFromBuild>" + nl +
              "    </ClCompile>", 1)
m = re.search(r"[ \t]*<ClInclude Include=", t)
assert m
t = t[:m.start()] + '    <ClInclude Include="Platform\\KSdlApp.h" />' + nl + t[m.start():]
io.open(p, "w", encoding="utf-8-sig", newline="").write(t)
print("S3Client.vcxproj: them ReleaseSDL|x64 OK")
