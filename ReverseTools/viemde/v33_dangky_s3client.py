# -*- coding: utf-8 -*-
r"""v33 - dang ky UiDiceItem.cpp/.h vao S3Client.vcxproj + .filters.

.vcxproj  : CRLF, KHONG BOM (co san 3 dong LF le tu truoc)
.filters  : CRLF, CO BOM UTF-8
Filter "BASE_ELEMENT_CASE\UICase" da duoc khai bao san (dong 10) nen dung luon.
"""
import io
import os
import shutil
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

D = r"D:\GAMEDEVNEW\Sources\S3Client"
HAU_TO = ".truoc_xucxac_2608"
F_P = os.path.join(D, "S3Client.vcxproj")
F_F = os.path.join(D, "S3Client.vcxproj.filters")

VA = [
    (F_P,
     '    <ClCompile Include="ui\\uicase\\UiTrembleItem.cpp" />\r\n',
     '    <ClCompile Include="ui\\uicase\\UiDiceItem.cpp" />\r\n'
     '    <ClCompile Include="ui\\uicase\\UiTrembleItem.cpp" />\r\n',
     "S3Client.vcxproj: ClCompile UiDiceItem.cpp"),
    (F_P,
     '    <ClInclude Include="ui\\uicase\\UiTrembleItem.h" />\r\n',
     '    <ClInclude Include="ui\\uicase\\UiDiceItem.h" />\r\n'
     '    <ClInclude Include="ui\\uicase\\UiTrembleItem.h" />\r\n',
     "S3Client.vcxproj: ClInclude UiDiceItem.h"),
    (F_F,
     '    <ClCompile Include="ui\\uicase\\UiTrembleItem.cpp">\r\n',
     '    <ClCompile Include="ui\\uicase\\UiDiceItem.cpp">\r\n'
     '      <Filter>BASE_ELEMENT_CASE\\UICase</Filter>\r\n'
     '    </ClCompile>\r\n'
     '    <ClCompile Include="ui\\uicase\\UiTrembleItem.cpp">\r\n',
     "S3Client.vcxproj.filters: UiDiceItem.cpp"),
    (F_F,
     '    <ClInclude Include="ui\\uicase\\UiTrembleItem.h">\r\n',
     '    <ClInclude Include="ui\\uicase\\UiDiceItem.h">\r\n'
     '      <Filter>BASE_ELEMENT_CASE\\UICase</Filter>\r\n'
     '    </ClInclude>\r\n'
     '    <ClInclude Include="ui\\uicase\\UiTrembleItem.h">\r\n',
     "S3Client.vcxproj.filters: UiDiceItem.h"),
]


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def ghi(p, s):
    bak = p + HAU_TO
    if not os.path.isfile(bak):
        shutil.copy2(p, bak)
    b = s.encode("latin-1")
    goc = io.open(bak, "rb").read()
    if (b.count(b"\n") - b.count(b"\r\n")) > (goc.count(b"\n") - goc.count(b"\r\n")):
        raise SystemExit("!! %s: sinh them dong LF le" % p)
    tmp = p + ".tmp"
    io.open(tmp, "wb").write(b)
    os.replace(tmp, p)


def main():
    theo = {}
    for p, cu, moi, nhan in VA:
        theo.setdefault(p, []).append((cu, moi, nhan))
    noi = {}
    for p, ds in theo.items():
        d = doc(p)
        for cu, moi, nhan in ds:
            if moi in d:
                print("   = da co san:", nhan)
                continue
            if d.count(cu) != 1:
                print("!! MO NEO khong duy nhat (%d): %s" % (d.count(cu), nhan))
                return 2
        noi[p] = d
    print("moi mo neo deu duy nhat - bat dau sua\n")
    for p, ds in theo.items():
        d = noi[p]
        for cu, moi, nhan in ds:
            if moi in d:
                continue
            d = d.replace(cu, moi, 1)
            print("   > " + nhan)
        ghi(p, d)
    print()
    for p in (F_P, F_F):
        print("   %-26s UiDiceItem: %s"
              % (os.path.basename(p), "CO" if "UiDiceItem" in doc(p) else "!! KHONG"))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
