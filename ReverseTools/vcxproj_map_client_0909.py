# -*- coding: utf-8 -*-
"""vcxproj_map_client_0909.py - them GenerateMapFile cho 'Client Release|Win32' cua Core.vcxproj (nhu Server Release da co).
Giu nguyen ket thuc dong cua tep (CRLF hay LF)."""
import io
import sys

P = "D:/GAMEDEVNEW_wt_delta/Sources/Core/Core.vcxproj"
BS = chr(92)
s = io.open(P, encoding="utf-8", newline="").read()
NL = "\r\n" if "\r\n" in s else "\n"
moc = "<ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='Client Release|Win32'\">"
if s.count(moc) != 1:
    print("FAIL moc:", s.count(moc)); sys.exit(1)
a = s.find(moc)
k = s.find("</ItemDefinitionGroup>", a)
old = "      <GenerateDebugInformation>true</GenerateDebugInformation>" + NL
b = s.find(old, a)
if b < 0 or b > k:
    print("FAIL: khong thay GenerateDebugInformation trong khoi Client Release"); sys.exit(1)
if "GenerateMapFile" in s[a:k]:
    print("da co"); sys.exit(0)
new = old + "      <GenerateMapFile>true</GenerateMapFile>" + NL + "      <MapFileName>." + BS + "ClientRelease" + BS + "CoreClient.map</MapFileName>" + NL
s = s[:b] + new + s[b + len(old):]
io.open(P, "w", encoding="utf-8", newline="").write(s)
print("OK Core.vcxproj: GenerateMapFile Client Release (NL=%s)" % ("CRLF" if NL == "\r\n" else "LF"))
