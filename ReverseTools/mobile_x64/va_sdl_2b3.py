# -*- coding: utf-8 -*-
"""[SDL 08/09 2b-3] Am thanh: chan #ifndef JX_PLATFORM_SDL quanh than 4 tep DirectSound (KDSound/KWavSound/KMusic/KMp3Music .cpp) va them
Sources/Engine/Src/KSoundMa.cpp (miniaudio) vao Engine.vcxproj (moi cau hinh; tep rong khi khong co JX_PLATFORM_SDL). Byte-safe."""
import io, re, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
E = ROOT + "\\Sources\\Engine\\Src\\"
TAG = b"[SDL 08/09 2b-3]"
for f, last_inc in (("KDSound.cpp", b'#include "KDSound.h"'), ("KWavSound.cpp", b'#include "KWavSound.h"'), ("KMusic.cpp", b'#include "KMusic.h"'), ("KMp3Music.cpp", b'#include "KMp3Music.h"')):
    p = E + f; b = io.open(p, "rb").read()
    if TAG in b:
        print(f, ": da chan"); continue
    nl = b"\r\n" if b"\r\n" in b else b"\n"
    i = b.find(last_inc + nl); assert i >= 0, (f, "khong thay include cuoi")
    j = i + len(last_inc + nl)
    nb = b[:j] + b"#ifndef JX_PLATFORM_SDL\t// " + TAG + b" ban SDL dung miniaudio o KSoundMa.cpp; than DirectSound duoi day chi cho Win32 thuong" + nl + b[j:]
    if not nb.endswith(nl): nb += nl
    nb += b"#endif // JX_PLATFORM_SDL " + TAG + nl
    assert sum(1 for c in b if c > 127) == sum(1 for c in nb if c > 127)
    io.open(p, "wb").write(nb); print(f, ": chan OK")
# Engine.vcxproj: them KSoundMa.cpp
p = ROOT + "\\Sources\\Engine\\Engine.vcxproj"
t = io.open(p, encoding="utf-8-sig", newline="").read()
if "KSoundMa.cpp" not in t:
    a = '<ClCompile Include="Src\\KSoundCache.cpp" />'
    if a not in t:
        m = re.search(r'<ClCompile Include="Src\\KSoundCache\.cpp"[^>]*>', t); assert m, "khong thay KSoundCache.cpp trong vcxproj"
        a = m.group(0)
    nl = "\r\n" if "\r\n" in t else "\n"
    t = t.replace(a, a + nl + '    <ClCompile Include="Src\\KSoundMa.cpp" />', 1)
    io.open(p, "w", encoding="utf-8-sig", newline="").write(t); print("Engine.vcxproj: + KSoundMa.cpp")
else: print("Engine.vcxproj: da co KSoundMa.cpp")
print("XONG 2b-3")
