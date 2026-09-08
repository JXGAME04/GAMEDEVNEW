# -*- coding: utf-8 -*-
"""Nhan ban cau hinh '<Cfg>|Win32' thanh '<Cfg>|x64' trong mot .vcxproj, chi THEM, khong dong vao Win32.
Cach dung: python them_x64.py <vcxproj> <Cfg> [--replace-existing] [--sub A=B ...] [--link-libs a.lib;b.lib]
                              [--no-vcpkg] [--exclude-libraries] [--extra-cl "/we4311 ..."]
- Moi phan tu co Condition ...=='<Cfg>|Win32' duoc chep ngay sau no voi ...=='<Cfg>|x64'.
- Trong ban sao: doi duong dan dau ra sang thu muc x64\, Lib\release -> Lib\release64, bin\client -> bin\client64,
  them TargetMachine=MachineX64, bo EnableEnhancedInstructionSet / /SAFESEH / /arch:.
- --replace-existing: xoa moi phan tu dang co dieu kien '<Cfg>|x64' truoc khi nhan ban (dung khi cau hinh x64 cu nua voi).
- --exclude-libraries: moi <Library Include=...> hien co bi loai khoi x64 (vi la .lib x86).
Tep vcxproj la XML ASCII/UTF-8 -> xu ly bang text, giu nguyen BOM va CRLF.
"""
import io, re, sys

args = sys.argv[1:]
path, cfg = args[0], args[1]
opts = {"replace": False, "subs": [], "libs": None, "novcpkg": False, "excl_lib": False, "extra_cl": None, "defs": None}
i = 2
while i < len(args):
    a = args[i]
    if a == "--replace-existing": opts["replace"] = True
    elif a == "--sub": opts["subs"].append(args[i + 1].split("=", 1)); i += 1
    elif a == "--link-libs": opts["libs"] = args[i + 1]; i += 1
    elif a == "--no-vcpkg": opts["novcpkg"] = True
    elif a == "--exclude-libraries": opts["excl_lib"] = True
    elif a == "--extra-cl": opts["extra_cl"] = args[i + 1]; i += 1
    elif a == "--defs": opts["defs"] = args[i + 1]; i += 1
    i += 1

raw = io.open(path, "rb").read()
bom = b""
if raw.startswith(b"\xef\xbb\xbf"):
    bom = raw[:3]; raw = raw[3:]
t = raw.decode("utf-8")
nl = "\r\n" if "\r\n" in t else "\n"
cond32 = "'$(Configuration)|$(Platform)'=='%s|Win32'" % cfg
cond64 = "'$(Configuration)|$(Platform)'=='%s|x64'" % cfg

def remove_existing_x64(t):
    # xoa cac khoi lon (PropertyGroup/ItemDefinitionGroup/ImportGroup) co dieu kien x64
    for tag in ("PropertyGroup", "ItemDefinitionGroup", "ImportGroup"):
        t = re.sub(r"[ \t]*<%s[^>]*Condition=\"%s\"[^>]*>.*?</%s>[ \t]*\r?\n" % (tag, re.escape(cond64), tag), "", t, flags=re.S)
    # xoa cac dong con (per-file) co dieu kien x64
    t = re.sub(r"[ \t]*<(\w+)[^>]*Condition=\"%s\"[^>]*>.*?</\1>[ \t]*\r?\n" % re.escape(cond64), "", t, flags=re.S)
    t = re.sub(r"[ \t]*<(\w+)[^>]*Condition=\"%s\"[^>]*/>[ \t]*\r?\n" % re.escape(cond64), "", t, flags=re.S)
    # xoa ProjectConfiguration x64 cua cfg
    t = re.sub(r"[ \t]*<ProjectConfiguration Include=\"%s\|x64\">.*?</ProjectConfiguration>[ \t]*\r?\n" % re.escape(cfg), "", t, flags=re.S)
    return t

if opts["replace"]:
    t = remove_existing_x64(t)

if ("<ProjectConfiguration Include=\"%s|x64\">" % cfg) in t:
    print("da co ProjectConfiguration x64 cho", cfg, "- dung"); sys.exit(2)

def fix_clone(s):
    s = s.replace(cond32, cond64)
    # thu muc dau ra: .\Release\ -> .\x64\Release\ ; Release/Game.exe ; ClientRelease -> x64\ClientRelease
    s = re.sub(r"(?i)(?<![\\\w])\.\\(Release|Debug|ClientRelease|ClientDebug)\\", r".\\x64\\\1\\", s)
    s = re.sub(r"(?i)(?<![\\\w.])(Release|ClientRelease)\\(Game\.\w+|Engine\.\w+|CoreClient\.\w+|Represent3\.\w+|Core\.\w+|S3Client\.\w+|FilterText_StaticLib\.\w+)", r"x64\\\1\\\2", s)
    s = s.replace("Release/Game.exe", "x64/Release/Game.exe")
    s = re.sub(r"\\[Ll]ib\\[Rr]elease\\", r"\\Lib\\release64\\", s)
    s = re.sub(r"/lib/release/", "/lib/release64/", s)
    s = re.sub(r"\\[Ll]ib\\[Dd]ebug\\", r"\\Lib\\debug64\\", s)
    s = re.sub(r"\\bin\\[Cc]lient\\", r"\\bin\\client64\\", s)
    s = re.sub(r"\\bin\\server\\", r"\\bin\\client64\\", s)
    s = re.sub(r"<EnableEnhancedInstructionSet>[^<]*</EnableEnhancedInstructionSet>\s*", "", s)
    s = re.sub(r"\s*/SAFESEH(:NO)?", "", s)
    s = re.sub(r"\s*/arch:\w+", "", s)
    for a, b in opts["subs"]:
        s = s.replace(a, b)
    if "<Link>" in s:
        if "<TargetMachine>" in s:
            s = re.sub(r"<TargetMachine>[^<]*</TargetMachine>", "<TargetMachine>MachineX64</TargetMachine>", s)
        else:
            s = s.replace("<Link>", "<Link>" + nl + "      <TargetMachine>MachineX64</TargetMachine>", 1)
        if opts["libs"] is not None:
            newdep = "<AdditionalDependencies>%s;%%(AdditionalDependencies)</AdditionalDependencies>" % opts["libs"]
            if "<AdditionalDependencies>" in s:
                s = re.sub(r"<AdditionalDependencies>[^<]*</AdditionalDependencies>", lambda m: newdep, s)
            else:
                s = s.replace("<Link>", "<Link>" + nl + "      " + newdep, 1)
    if "<ClCompile>" in s and opts["extra_cl"]:
        if "<AdditionalOptions>" in s:
            s = re.sub(r"<AdditionalOptions>([^<]*)</AdditionalOptions>", lambda m: "<AdditionalOptions>%s %s</AdditionalOptions>" % (opts["extra_cl"], m.group(1)), s, count=1)
        else:
            s = s.replace("<ClCompile>", "<ClCompile>" + nl + "      <AdditionalOptions>%s %%(AdditionalOptions)</AdditionalOptions>" % opts["extra_cl"], 1)
    if "<ClCompile>" in s and opts["defs"]:
        s = re.sub(r"(<ClCompile>.*?<PreprocessorDefinitions>)([^<]*)(</PreprocessorDefinitions>)", lambda m: m.group(1) + opts["defs"] + ";" + m.group(2) + m.group(3), s, count=1, flags=re.S)
    return s

out = []
pos = 0
count = 0
# 1) ProjectConfiguration
pc = re.search(r"([ \t]*)<ProjectConfiguration Include=\"%s\|Win32\">.*?</ProjectConfiguration>[ \t]*\r?\n" % re.escape(cfg), t, flags=re.S)
if not pc:
    print("khong thay ProjectConfiguration", cfg, "|Win32"); sys.exit(1)
clone = pc.group(0).replace("%s|Win32" % cfg, "%s|x64" % cfg).replace("<Platform>Win32</Platform>", "<Platform>x64</Platform>")
t = t[:pc.end()] + clone + t[pc.end():]
count += 1
# 2) block-level groups
for tag in ("PropertyGroup", "ImportGroup", "ItemDefinitionGroup"):
    pat = re.compile(r"[ \t]*<%s[^>]*Condition=\"%s\"[^>]*>.*?</%s>[ \t]*\r?\n" % (tag, re.escape(cond32), tag), re.S)
    pieces = []; last = 0
    for m in pat.finditer(t):
        pieces.append(t[last:m.end()]); pieces.append(fix_clone(m.group(0))); last = m.end(); count += 1
    pieces.append(t[last:]); t = "".join(pieces)
# 2b) self-closing / one-line PropertyGroup with Label (e.g. Vcpkg) handled above if multi-line; also single-line variants:
pat = re.compile(r"[ \t]*<(\w+)[^>]*Condition=\"%s\"[^>]*/>[ \t]*\r?\n" % re.escape(cond32))
pieces = []; last = 0
for m in pat.finditer(t):
    if m.group(1) in ("PropertyGroup", "ImportGroup", "ItemDefinitionGroup", "Import"):
        pieces.append(t[last:m.end()]); pieces.append(fix_clone(m.group(0))); last = m.end(); count += 1
pieces.append(t[last:]); t = "".join(pieces)
# 3) per-file children: <Tag Condition=...>value</Tag> on one line (khong phai group)
pat = re.compile(r"[ \t]*<(?!PropertyGroup|ImportGroup|ItemDefinitionGroup)(\w+)([^>]*)Condition=\"%s\"([^>]*)>(.*?)</\1>[ \t]*\r?\n" % re.escape(cond32), re.S)
pieces = []; last = 0
for m in pat.finditer(t):
    pieces.append(t[last:m.end()]); pieces.append(fix_clone(m.group(0))); last = m.end(); count += 1
pieces.append(t[last:]); t = "".join(pieces)
# 4) Library items x86 -> loai khoi x64
if opts["excl_lib"]:
    def excl(m):
        body = m.group(0)
        if body.endswith("/>" + nl) or body.rstrip().endswith("/>"):
            return body.rstrip()[:-2] + ">" + nl + "      <ExcludedFromBuild Condition=\"%s\">true</ExcludedFromBuild>" % cond64 + nl + "    </Library>" + nl
        return body.replace("</Library>", "  <ExcludedFromBuild Condition=\"%s\">true</ExcludedFromBuild>" % cond64 + nl + "    </Library>", 1)
    t = re.sub(r"[ \t]*<Library Include=\"[^\"]+\"\s*/>[ \t]*\r?\n", excl, t)
    t = re.sub(r"[ \t]*<Library Include=\"[^\"]+\">.*?</Library>[ \t]*\r?\n", excl, t, flags=re.S)
# 5) vcpkg tat cho x64
if opts["novcpkg"]:
    t = t.replace("</Project>", "  <PropertyGroup Label=\"Vcpkg\" Condition=\"%s\">%s    <VcpkgEnableManifest>false</VcpkgEnableManifest>%s    <VcpkgEnabled>false</VcpkgEnabled>%s  </PropertyGroup>%s</Project>" % (cond64, nl, nl, nl, nl), 1)
io.open(path, "wb").write(bom + t.encode("utf-8"))
print("OK", path, "cfg", cfg, "nhan ban", count, "phan tu")
