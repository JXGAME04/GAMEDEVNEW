# -*- coding: utf-8 -*-
r"""v02 - NOI KItemCompound VAO DU AN.

  1. Core.vcxproj  : them KItemCompound.cpp/.h (loai khoi 4 cau hinh Client,
                     giong khuon KItemDice.cpp).
  2. ScriptFuns.cpp: khai bao ngoai + dang ky 12 ten ham Lua moi;
                     doi GetItemQuality tu ban vo (luon tra 0) sang ban that.
  3. KCore.cpp     : goi g_ItemCompound.Init() sau g_IniScriptEngine().

Moi tep doc/ghi byte-nguyen (latin-1) va tu bam theo xuong dong cua chinh no.
"""
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

CR = chr(13)
NL = chr(10)

SRC = r"D:\GAMEDEVNEW\Sources\Core\Src"
PROJ = r"D:\GAMEDEVNEW\Sources\Core\Core.vcxproj"


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def ghi(p, t):
    if not os.path.isfile(p + ".truoc_loren"):
        io.open(p + ".truoc_loren", "wb").write(io.open(p, "rb").read())
    io.open(p, "wb").write(t.encode("latin-1"))


def dem_dong(t):
    crlf = t.count(CR + NL)
    lf = t.count(NL) - crlf
    return crlf, lf


def theo_eol(t, s):
    crlf, lf = dem_dong(t)
    s = s.replace(CR + NL, NL)
    if crlf > lf:
        s = s.replace(NL, CR + NL)
    return s


KQ = []


def va(duong, tim, thay, nhan=None):
    ten = nhan or os.path.basename(duong)
    t = doc(duong)
    tim = theo_eol(t, tim)
    thay = theo_eol(t, thay)
    if thay in t:
        KQ.append((True, "  BO QUA   %-28s da co san" % ten))
        return
    n = t.count(tim)
    if n != 1:
        KQ.append((False, "  *** LOI  %-28s moc neo xuat hien %d lan" % (ten, n)))
        return
    c0, l0 = dem_dong(t)
    t2 = t.replace(tim, thay, 1)
    c1, l1 = dem_dong(t2)
    it0 = min(c0, l0)
    it1 = min(c1, l1)
    if it1 > it0:
        KQ.append((False, "  *** LOI  %-28s tang loai xuong dong thieu so" % ten))
        return
    ghi(duong, t2)
    KQ.append((True, "  OK       %-28s (CRLF %d->%d, LF %d->%d)" % (ten, c0, c1, l0, l1)))


# ---------------------------------------------------------------------------
# 1. Core.vcxproj
# ---------------------------------------------------------------------------
VCX_CPP_TIM = """    <ClCompile Include="Src\\KItemDice.cpp">"""
VCX_CPP_THAY = """    <ClCompile Include="Src\\KItemCompound.cpp">
      <ExcludedFromBuild Condition="'$(Configuration)|$(Platform)'=='Client Debug|Win32'">true</ExcludedFromBuild>
      <ExcludedFromBuild Condition="'$(Configuration)|$(Platform)'=='Client Debug|x64'">true</ExcludedFromBuild>
      <ExcludedFromBuild Condition="'$(Configuration)|$(Platform)'=='Client Release|Win32'">true</ExcludedFromBuild>
      <ExcludedFromBuild Condition="'$(Configuration)|$(Platform)'=='Client Release|x64'">true</ExcludedFromBuild>
    </ClCompile>
    <ClCompile Include="Src\\KItemDice.cpp">"""

VCX_H_TIM = """    <ClInclude Include="Src\\KItemDice.h">"""
VCX_H_THAY = """    <ClInclude Include="Src\\KItemCompound.h">
    </ClInclude>
    <ClInclude Include="Src\\KItemDice.h">"""

# ---------------------------------------------------------------------------
# 2. ScriptFuns.cpp - khai bao ngoai
# ---------------------------------------------------------------------------
SF_EXT_TIM = "extern int LuaHD3_GetItemRandSeed(Lua_State* L);\n"
SF_EXT_THAY = (
    "extern int LuaHD3_GetItemRandSeed(Lua_State* L);\n"
    "// [LOREN] he lo ren - KItemCompound.cpp (viet moi theo ban Linux)\n"
    "extern int LuaCmp_ITEM_CalcItemValue(Lua_State* L);\n"
    "extern int LuaCmp_ITEM_GetItemVersion(Lua_State* L);\n"
    "extern int LuaCmp_ITEM_GetLatestItemVersion(Lua_State* L);\n"
    "extern int LuaCmp_AddItemEx(Lua_State* L);\n"
    "extern int LuaCmp_TabFile_Search(Lua_State* L);\n"
    "extern int LuaCmp_WriteCompoundLog(Lua_State* L);\n"
    "extern int LuaCmp_GetMaxDurability(Lua_State* L);\n"
    "extern int LuaCmp_GetCurDurability(Lua_State* L);\n"
    "extern int LuaCmp_SetMaxDurability(Lua_State* L);\n"
    "extern int LuaCmp_SetCurDurability(Lua_State* L);\n"
    "extern int LuaCmp_GetItemQuality(Lua_State* L);\n"
    "extern int LuaCmp_GetItemBindState(Lua_State* L);\n"
)

# ---------------------------------------------------------------------------
# 2b. ScriptFuns.cpp - bang dang ky
#     Doi luon GetItemQuality sang ban that (ban cu luon tra 0).
# ---------------------------------------------------------------------------
SF_REG_TIM = (
    '\t\t{"GetItemGenTime",\tLuaHD3_GetItemGenTime},\n'
    '\t\t{"GetItemQuality",\tLuaHD3_GetItemQuality},\n'
)
SF_REG_THAY = (
    '\t\t{"GetItemGenTime",\tLuaHD3_GetItemGenTime},\n'
    "\t\t// [LOREN] GetItemQuality ban VO cu luon tra 0; he lo ren song bang\n"
    "\t\t// gia tri nay nen doi sang ban doc that tu m_CommonAttrib.nItemNature.\n"
    '\t\t{"GetItemQuality",\tLuaCmp_GetItemQuality},\n'
    "\t\t// [LOREN] 11 ham con lai cua he lo ren\n"
    '\t\t{"ITEM_CalcItemValue",\tLuaCmp_ITEM_CalcItemValue},\n'
    '\t\t{"ITEM_GetItemVersion",\tLuaCmp_ITEM_GetItemVersion},\n'
    '\t\t{"ITEM_GetLatestItemVersion",\tLuaCmp_ITEM_GetLatestItemVersion},\n'
    '\t\t{"AddItemEx",\tLuaCmp_AddItemEx},\n'
    '\t\t{"TabFile_Search",\tLuaCmp_TabFile_Search},\n'
    '\t\t{"WriteCompoundLog",\tLuaCmp_WriteCompoundLog},\n'
    '\t\t{"GetMaxDurability",\tLuaCmp_GetMaxDurability},\n'
    '\t\t{"GetCurDurability",\tLuaCmp_GetCurDurability},\n'
    '\t\t{"SetMaxDurability",\tLuaCmp_SetMaxDurability},\n'
    '\t\t{"SetCurDurability",\tLuaCmp_SetCurDurability},\n'
    '\t\t{"GetItemBindState",\tLuaCmp_GetItemBindState},\n'
)

# ---------------------------------------------------------------------------
# 3. KCore.cpp - khoi tao
# ---------------------------------------------------------------------------
KCORE_TIM = "\tg_IniScriptEngine();\n"
KCORE_THAY = (
    "\tg_IniScriptEngine();\n"
    "#ifdef _SERVER\n"
    "\t// [LOREN] he lo ren: nap compoundscript.txt + itemvaluescript.txt.\n"
    "\t// Phai dat SAU g_IniScriptEngine vi cac kich ban gia tri tu nap bang\n"
    "\t// rieng cua chung luc chay dong dau tien.\n"
    "\tg_ItemCompound.Init();\n"
    "#endif\n"
)

KCORE_INC_TIM = '#include "KItemDice.h"\t// 26/08: he xuc xac chia do (DICEITEM)\n'
KCORE_INC_THAY = (
    '#include "KItemDice.h"\t// 26/08: he xuc xac chia do (DICEITEM)\n'
    '#include "KItemCompound.h"\t// [LOREN] he lo ren (viet moi theo ban Linux)\n'
)


def main():
    print("NOI HE LO REN VAO DU AN")
    print("=" * 78)

    va(PROJ, VCX_CPP_TIM, VCX_CPP_THAY, "Core.vcxproj (ClCompile)")
    va(PROJ, VCX_H_TIM, VCX_H_THAY, "Core.vcxproj (ClInclude)")
    va(os.path.join(SRC, "ScriptFuns.cpp"), SF_EXT_TIM, SF_EXT_THAY, "ScriptFuns.cpp (extern)")
    va(os.path.join(SRC, "ScriptFuns.cpp"), SF_REG_TIM, SF_REG_THAY, "ScriptFuns.cpp (dang ky)")

    # KCore.cpp co the chua include KItemDice.h - kiem truoc
    kc = os.path.join(SRC, "KCore.cpp")
    if '#include "KItemDice.h"' in doc(kc):
        va(kc, KCORE_INC_TIM, KCORE_INC_THAY, "KCore.cpp (include)")
    else:
        KQ.append((False, "  *** LOI  %-28s khong thay include KItemDice.h" % "KCore.cpp"))
    va(kc, KCORE_TIM, KCORE_THAY, "KCore.cpp (Init)")

    loi = 0
    for ok, msg in KQ:
        print(msg)
        if not ok:
            loi += 1
    print("=" * 78)
    print("  %s" % ("XONG - khong loi" if loi == 0 else "CO %d MUC LOI" % loi))
    return 1 if loi else 0


if __name__ == "__main__":
    raise SystemExit(main())
