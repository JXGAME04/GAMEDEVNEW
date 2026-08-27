# -*- coding: utf-8 -*-
"""v27 - dau noi he xuc xac vao may chu:
   1. 8 extern + 8 dong dang ky ham Lua trong ScriptFuns.cpp (khoi #ifdef _SERVER)
   2. moc KItemDice_Breathe() vao CoreServerShell::Breathe()
   3. them KItemDice.cpp/.h vao Core.vcxproj (loai khoi cac cau hinh Client,
      y het KJx2League.cpp)

Moi tep deu CRLF; Core.vcxproj co BOM UTF-8 - ban va giu nguyen ca hai.
"""
import io
import os
import shutil
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

CAY = r"D:\GAMEDEVNEW"
HAU_TO = ".truoc_xucxac_2608"

F_SF = os.path.join(CAY, "Sources", "Core", "Src", "ScriptFuns.cpp")
F_SH = os.path.join(CAY, "Sources", "Core", "Src", "CoreServerShell.cpp")
F_VP = os.path.join(CAY, "Sources", "Core", "Core.vcxproj")

EXTERN = (
    'extern int LuaOB_LoadShareData(Lua_State* L);\t\t// [PORT5 23/08] KJx2SharedStore.cpp\r\n'
    '// --- he XUC XAC chia do (DICEITEM) 26/08 - KItemDice.cpp ---\r\n'
    'extern int LuaApplyItemDice(Lua_State* L);\t\t\t// Linux 0x81c1c40\r\n'
    'extern int LuaAddDiceItemInfo(Lua_State* L);\t\t// Linux 0x81c1a10\r\n'
    'extern int LuaRollItem(Lua_State* L);\t\t\t\t// Linux 0x81c16d0\r\n'
    'extern int LuaGetItemDiceState(Lua_State* L);\t\t// Linux 0x81c05b0\r\n'
    'extern int LuaDiceLootItem(Lua_State* L);\t\t\t// Linux 0x81c14f0\r\n'
    'extern int LuaGetItemDiceRollInfo(Lua_State* L);\t// Linux 0x81c0b80\r\n'
    'extern int LuaGetItemDiceItemInfo(Lua_State* L);\t// Linux 0x81c04a0\r\n'
    'extern int LuaGetItemDicePlayerList(Lua_State* L);\t// Linux 0x81c0380\r\n'
)
NEO_EXTERN = 'extern int LuaOB_LoadShareData(Lua_State* L);\t\t// [PORT5 23/08] KJx2SharedStore.cpp\r\n'

NEO_REG = ('\t\t{ "NPCINFO_SetNpcCurrentLife",\tLuaNPCINFO_SetNpcCurrentLife },'
           '\t// (0x081C0070)\r\n')
REG = NEO_REG + (
    '\t\t//--- he XUC XAC chia do (DICEITEM) 26/08 - KItemDice.cpp ---\r\n'
    '\t\t{ "ApplyItemDice",\tLuaApplyItemDice },\t// yandibaozang/tong_guta (0x081C1C40)\r\n'
    '\t\t{ "AddDiceItemInfo",\tLuaAddDiceItemInfo },\t// (0x081C1A10)\r\n'
    '\t\t{ "RollItem",\tLuaRollItem },\t// (0x081C16D0)\r\n'
    '\t\t{ "GetItemDiceState",\tLuaGetItemDiceState },\t// (0x081C05B0)\r\n'
    '\t\t{ "DiceLootItem",\tLuaDiceLootItem },\t// (0x081C14F0) - o ta chi TRA THONG TIN\r\n'
    '\t\t{ "GetItemDiceRollInfo",\tLuaGetItemDiceRollInfo },\t// (0x081C0B80) tra 3 gia tri\r\n'
    '\t\t{ "GetItemDiceItemInfo",\tLuaGetItemDiceItemInfo },\t// (0x081C04A0) tra 8 gia tri\r\n'
    '\t\t{ "GetItemDicePlayerList",\tLuaGetItemDicePlayerList },\t// (0x081C0380)\r\n'
)

NEO_BREATHE = ('\t\tKJx2CityWar_Breathe();\t// DOT E (E3): mot lan sau boot - '
               'ghi chu thanh/thue vao KSubWorld\r\n')
BREATHE = NEO_BREATHE + (
    '\t\tKItemDice_Breathe();\t// 26/08: he xuc xac chia do (DICEITEM) - '
    'tu do nhip 1 giay ben trong\r\n')

VP_CPP_NEO = '    <ClCompile Include="Src\\KJx2League.cpp">\r\n'
VP_CPP = (
    '    <ClCompile Include="Src\\KItemDice.cpp">\r\n'
    "      <ExcludedFromBuild Condition=\"'$(Configuration)|$(Platform)'=='Client Debug|Win32'\">true</ExcludedFromBuild>\r\n"
    "      <ExcludedFromBuild Condition=\"'$(Configuration)|$(Platform)'=='Client Debug|x64'\">true</ExcludedFromBuild>\r\n"
    "      <ExcludedFromBuild Condition=\"'$(Configuration)|$(Platform)'=='Client Release|Win32'\">true</ExcludedFromBuild>\r\n"
    "      <ExcludedFromBuild Condition=\"'$(Configuration)|$(Platform)'=='Client Release|x64'\">true</ExcludedFromBuild>\r\n"
    '    </ClCompile>\r\n') + VP_CPP_NEO

VP_H_NEO = '    <ClInclude Include="Src\\KJx2League.h">\r\n'
VP_H = ('    <ClInclude Include="Src\\KItemDice.h">\r\n'
        '    </ClInclude>\r\n') + VP_H_NEO


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def kieu_eol(p):
    """Cay nay TRON kieu xuong dong: ScriptFuns.cpp la LF (15.516 dong, chi 4 CRLF)
    trong khi CoreServerShell.cpp va Core.vcxproj la CRLF. Doan sai la hong tep."""
    d = io.open(p, "rb").read()
    return "\r\n" if d.count(b"\r\n") > (d.count(b"\n") - d.count(b"\r\n")) else "\n"


def theo_eol(s, eol):
    """chuoi trong ban va viet bang \\r\\n -> doi cho khop tep dich"""
    return s.replace("\r\n", "\n") if eol == "\n" else s


def ghi(p, s):
    bak = p + HAU_TO
    if not os.path.isfile(bak):
        shutil.copy2(p, bak)
    b = s.encode("latin-1")
    goc = io.open(bak, "rb").read()

    # Kiem "kieu THIEU SO khong duoc tang": tep CRLF thi khong duoc sinh them dong
    # LF le; tep LF thi khong duoc sinh them dong CRLF. (Ban truoc kiem cung nhac
    # LF le nen bao dong gia voi ScriptFuns.cpp - von la tep LF.)
    def dem(x):
        crlf = x.count(b"\r\n")
        lf = x.count(b"\n") - crlf
        return crlf, lf

    c_goc, l_goc = dem(goc)
    c_moi, l_moi = dem(b)
    if c_goc > l_goc:                       # tep CRLF
        if l_moi > l_goc:
            raise SystemExit("!! %s: tep CRLF ma dong LF le tang %d -> %d"
                             % (p, l_goc, l_moi))
    else:                                   # tep LF
        if c_moi > c_goc:
            raise SystemExit("!! %s: tep LF ma dong CRLF tang %d -> %d"
                             % (p, c_goc, c_moi))
    tmp = p + ".tmp"
    io.open(tmp, "wb").write(b)
    os.replace(tmp, p)


VA = [
    (F_SF, NEO_EXTERN, EXTERN, "ScriptFuns.cpp: 8 extern ham Lua xuc xac"),
    (F_SF, NEO_REG, REG, "ScriptFuns.cpp: 8 dong dang ky (trong khoi #ifdef _SERVER)"),
    (F_SH, NEO_BREATHE, BREATHE, "CoreServerShell.cpp: moc KItemDice_Breathe()"),
    (F_VP, VP_CPP_NEO, VP_CPP, "Core.vcxproj: them KItemDice.cpp (loai khoi Client)"),
    (F_VP, VP_H_NEO, VP_H, "Core.vcxproj: them KItemDice.h"),
]


def main():
    theo_tep = {}
    for p, cu, moi, nhan in VA:
        theo_tep.setdefault(p, []).append((cu, moi, nhan))

    noidung = {}
    eol = {}
    for p, ds in theo_tep.items():
        if not os.path.isfile(p):
            print("!! khong co tep:", p)
            return 2
        eol[p] = kieu_eol(p)
        d = doc(p)
        print("   %-22s kieu xuong dong: %s"
              % (os.path.basename(p), "CRLF" if eol[p] == "\r\n" else "LF"))
        for cu, moi, nhan in ds:
            if theo_eol(moi, eol[p]) in d:
                print("   = da co san:", nhan)
                continue
            n = d.count(theo_eol(cu, eol[p]))
            if n != 1:
                print("!! MO NEO khong duy nhat (%d lan): %s" % (n, nhan))
                return 2
        noidung[p] = d
    print("\nmoi mo neo deu duy nhat - bat dau sua\n")

    for p, ds in theo_tep.items():
        d = noidung[p]
        for cu, moi, nhan in ds:
            cu2 = theo_eol(cu, eol[p])
            moi2 = theo_eol(moi, eol[p])
            if moi2 in d:
                continue
            d = d.replace(cu2, moi2, 1)
            print("   > " + nhan)
        ghi(p, d)

    # kiem lai
    print()
    d = doc(F_SF)
    print("   ScriptFuns.cpp: %d/8 ham da dang ky"
          % sum(1 for t in ("ApplyItemDice", "AddDiceItemInfo", "RollItem",
                            "GetItemDiceState", "DiceLootItem", "GetItemDiceRollInfo",
                            "GetItemDiceItemInfo", "GetItemDicePlayerList")
                if ('{ "%s",' % t) in d))
    print("   CoreServerShell.cpp: moc Breathe: %s"
          % ("CO" if "KItemDice_Breathe();" in doc(F_SH) else "KHONG"))
    d = doc(F_VP)
    print("   Core.vcxproj: KItemDice.cpp %s, KItemDice.h %s"
          % ("CO" if "Src\\KItemDice.cpp" in d else "KHONG",
             "CO" if "Src\\KItemDice.h" in d else "KHONG"))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
