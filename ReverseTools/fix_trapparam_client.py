# -*- coding: utf-8 -*-
r"""[24/08] VA loi PORT5 chi lo khi BUILD CLIENT:
KRegion::GetTrapParam nam trong khoi #ifdef _SERVER (KRegion.cpp:1341) nen ban client
KHONG co ham, trong khi KNpc::CheckTrap goi no MA KHONG co guard
=> LNK2019 unresolved external, CoreClient.dll khong build duoc.
Hai cho DUNG bien nCellTrapParam deu da co #ifdef _SERVER, chi rieng dong khai bao thieu.
"""
import io, shutil

P = r"D:\GAMEDEVNEW\Sources\Core\Src\KNpc.cpp"
s = io.open(P, "r", encoding="latin-1", newline="").read()
NL = "\r\n" if "\r\n" in s else "\n"

if "nCellTrapParam = 0" in s:
    print("da va truoc do"); raise SystemExit(0)

old = "\tint nCellTrapParam = SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].GetTrapParam(m_MapX, m_MapY);"
assert s.count(old) == 1, "anchor = %d" % s.count(old)

new = ("\t// [FIX 24/08] GetTrapParam CHI ton tai o ban _SERVER (KRegion.cpp:1341 nam trong" + NL +
       "\t// #ifdef _SERVER) - khong guard thi CoreClient.dll loi LNK2019." + NL +
       "\tint nCellTrapParam = 0;" + NL +
       "#ifdef _SERVER" + NL +
       "\tnCellTrapParam = SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].GetTrapParam(m_MapX, m_MapY);" + NL +
       "#endif")
s = s.replace(old, new, 1)
shutil.copyfile(P, P + ".truoc_fix_trapparam_2408")
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("DA VA KNpc.cpp (guard GetTrapParam cho ban client)")
