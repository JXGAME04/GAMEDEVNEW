# -*- coding: utf-8 -*-
r"""v01 - VA NEN TANG ENGINE cho he lo ren (doc KItemCompound.h/.cpp truoc).

Ba viec, deu la BO SUNG, khong sua logic cu:
  1. KItem.h/.cpp : them SetMaxDurability (ban Linux co, JX1 chua co).
                    GetMaxDurability doc m_aryBaseAttrib[i] co
                    nAttribType == magic_durability_v -> Set ghi vao dung o do.
  2. ScriptFuns.cpp : mo mot cua cho KItemCompound.cpp lay bang KTabFile theo
                    ten (s_mapTabFiles + sGetTabFileByName dang la static).
  3. ScriptFuns.cpp : TabFile_GetCell nhan them tham so THU TU (gia tri mac
                    dinh) - ban Linux goi 4 doi:
                       TabFile_GetCell( path, row, "COL", -1 )
                    JX1 dang bo qua doi thu 4 va tra "" -> tonumber("") = nil
                    -> moi phep tinh dung o do deu no.

MOI TEP DEU DOC/GHI BANG latin-1 (byte-nguyen) - trong cay nay co tep GBK va
TCVN3, doc bang utf-8 la hong byte.
"""
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

SRC = r"D:\GAMEDEVNEW\Sources\Core\Src"


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def ghi(p, t, sao_luu=True):
    if sao_luu and not os.path.isfile(p + ".truoc_loren"):
        io.open(p + ".truoc_loren", "wb").write(io.open(p, "rb").read())
    io.open(p, "wb").write(t.encode("latin-1"))


def dem_dong(t):
    crlf = t.count("\r\n")
    lf = t.count("\n") - crlf
    return crlf, lf


CR = chr(13)
NL = chr(10)


def theo_eol(t, s):
    """doi xuong dong cua chuoi vao/ra cho khop tep dich"""
    crlf, lf = dem_dong(t)
    s = s.replace(CR + NL, NL)
    if crlf > lf:
        s = s.replace(NL, CR + NL)
    return s


def va(ten, tim, thay, mot_lan=True):
    """tra ve (ok, thong bao)"""
    p = os.path.join(SRC, ten)
    t = doc(p)
    tim = theo_eol(t, tim)
    thay = theo_eol(t, thay)
    if thay in t:
        return True, "  BO QUA %-18s da co san" % ten
    n = t.count(tim)
    if n == 0:
        return False, "  *** LOI  %-18s KHONG TIM THAY moc neo" % ten
    if mot_lan and n > 1:
        return False, "  *** LOI  %-18s moc neo xuat hien %d lan" % (ten, n)
    c0, l0 = dem_dong(t)
    t2 = t.replace(tim, thay, 1)
    c1, l1 = dem_dong(t2)
    # canh gac xuong dong: loai thieu so KHONG duoc tang them
    if (c0 <= l0 and l1 > l0 + (l1 - l0)) or (c0 > l0 and l1 > l0):
        return False, "  *** LOI  %-18s lam tang so dong LF le loi" % ten
    ghi(p, t2)
    return True, "  OK       %-18s (CRLF %d->%d, LF %d->%d)" % (ten, c0, c1, l0, l1)


# ---------------------------------------------------------------------------
# 1. KItem.h : khai bao SetMaxDurability ngay canh GetMaxDurability
# ---------------------------------------------------------------------------
KITEM_H_TIM = "\tint\t\tGetMaxDurability();\n"
KITEM_H_THAY = (
    "\tint\t\tGetMaxDurability();\n"
    "\t// [LOREN] ban Linux co SetMaxDurability (script\\item\\compound), JX1 chua.\n"
    "\t// GetMaxDurability doc m_aryBaseAttrib co nAttribType == magic_durability_v\n"
    "\t// nen Set ghi vao DUNG o do; khong co o thi khong lam gi (tra FALSE).\n"
    "\tBOOL\tSetMaxDurability(IN const int nDur);\n"
)

# ---------------------------------------------------------------------------
# 2. KItem.cpp : than ham, dat ngay sau GetMaxDurability
# ---------------------------------------------------------------------------
KITEM_CPP_TIM = """int KItem::GetTotalMagicLevel()
{"""
KITEM_CPP_THAY = """// [LOREN] doi ung cua GetMaxDurability - ghi vao chinh o base attrib do.
BOOL KItem::SetMaxDurability(IN const int nDur)
{
	for (int i = 0; i < 7; i++)
	{
		if (m_aryBaseAttrib[i].nAttribType == magic_durability_v)
		{
			m_aryBaseAttrib[i].nValue[0] = nDur;
			if (m_nCurrentDur > nDur)
				m_nCurrentDur = nDur;
			return TRUE;
		}
	}
	return FALSE;
}

int KItem::GetTotalMagicLevel()
{"""

# ---------------------------------------------------------------------------
# 3. ScriptFuns.cpp : cua ra cho KItemCompound.cpp + doi mac dinh cho GetCell
# ---------------------------------------------------------------------------
SF_TIM_1 = "int LuaTabFile_Load(Lua_State* L)\n"
SF_THAY_1 = (
    "// [LOREN] KItemCompound.cpp can chinh bang nay de lam TabFile_Search.\n"
    "// s_mapTabFiles/sGetTabFileByName la static nen mo mot cua duy nhat.\n"
    "KTabFile* g_GetLuaTabFile(Lua_State* L, int nArg)\n"
    "{\n"
    "\treturn sGetTabFileByName(L, nArg);\n"
    "}\n"
    "\n"
    "int LuaTabFile_Load(Lua_State* L)\n"
)

# GetCell: chen doi mac dinh (doi thu 4) thay cho "" o CA BA nhanh
SF_TIM_2 = """	KTabFile* pTabC = sGetTabFileByName(L, 1);
	if (nParamNum >= 3 && pTabC->GetHeight())
	{
		if (Lua_IsNumber(L, 2) && Lua_IsNumber(L, 3))
		{
			int nRow = (int)Lua_ValueToNumber(L, 2);
			int nColumn = (int)Lua_ValueToNumber(L, 3);
			pTabC->GetString(nRow, nColumn, "", szString, dwCellMax);
		}
		else if (Lua_IsNumber(L, 2) && Lua_IsString(L, 3))
		{
			int nRow = (int)Lua_ValueToNumber(L, 2);
			char szColumn[64];
			strncpy(szColumn, Lua_ValueToString(L, 3), sizeof(szColumn) - 1);
			szColumn[sizeof(szColumn) - 1] = 0;
			pTabC->GetString(nRow, szColumn, "", szString, dwCellMax);
		}
		else if (Lua_IsString(L, 2) && Lua_IsString(L, 3))
		{
			char szRow[64];
			char szColumn[64];
			strncpy(szRow, Lua_ValueToString(L, 2), sizeof(szRow) - 1);
			szRow[sizeof(szRow) - 1] = 0;
			strncpy(szColumn, Lua_ValueToString(L, 3), sizeof(szColumn) - 1);
			szColumn[sizeof(szColumn) - 1] = 0;
			pTabC->GetString(szRow, szColumn, "", szString, dwCellMax);
		}
		else
			return 0;
"""

SF_THAY_2 = """	KTabFile* pTabC = sGetTabFileByName(L, 1);
	if (nParamNum >= 3 && pTabC->GetHeight())
	{
		// [LOREN] doi thu 4 = GIA TRI MAC DINH khi o trong / khong co cot.
		// Ban Linux goi TabFile_GetCell( path, row, "COL", -1 ) o khap noi;
		// truoc day JX1 bo qua doi nay va luon tra "" -> tonumber("") = nil.
		char szDefault[64] = "";
		if (nParamNum >= 4)
		{
			if (Lua_IsNumber(L, 4))
			{
				double dDef = Lua_ValueToNumber(L, 4);
				if (dDef == (double)(int)dDef)
					_snprintf(szDefault, sizeof(szDefault) - 1, "%d", (int)dDef);
				else
					_snprintf(szDefault, sizeof(szDefault) - 1, "%g", dDef);
				szDefault[sizeof(szDefault) - 1] = 0;
			}
			else if (Lua_IsString(L, 4))
			{
				strncpy(szDefault, Lua_ValueToString(L, 4), sizeof(szDefault) - 1);
				szDefault[sizeof(szDefault) - 1] = 0;
			}
		}
		if (Lua_IsNumber(L, 2) && Lua_IsNumber(L, 3))
		{
			int nRow = (int)Lua_ValueToNumber(L, 2);
			int nColumn = (int)Lua_ValueToNumber(L, 3);
			pTabC->GetString(nRow, nColumn, szDefault, szString, dwCellMax);
		}
		else if (Lua_IsNumber(L, 2) && Lua_IsString(L, 3))
		{
			int nRow = (int)Lua_ValueToNumber(L, 2);
			char szColumn[64];
			strncpy(szColumn, Lua_ValueToString(L, 3), sizeof(szColumn) - 1);
			szColumn[sizeof(szColumn) - 1] = 0;
			pTabC->GetString(nRow, szColumn, szDefault, szString, dwCellMax);
		}
		else if (Lua_IsString(L, 2) && Lua_IsString(L, 3))
		{
			char szRow[64];
			char szColumn[64];
			strncpy(szRow, Lua_ValueToString(L, 2), sizeof(szRow) - 1);
			szRow[sizeof(szRow) - 1] = 0;
			strncpy(szColumn, Lua_ValueToString(L, 3), sizeof(szColumn) - 1);
			szColumn[sizeof(szColumn) - 1] = 0;
			pTabC->GetString(szRow, szColumn, szDefault, szString, dwCellMax);
		}
		else
			return 0;
		// o trong -> KTabFile van tra chuoi rong; ep ve mac dinh
		if (szString[0] == 0 && szDefault[0] != 0)
		{
			strncpy(szString, szDefault, dwCellMax);
			szString[dwCellMax] = 0;
		}
"""


def main():
    print("VA NEN TANG ENGINE CHO HE LO REN")
    print("=" * 78)
    ket = []
    ket.append(va("KItem.h", KITEM_H_TIM, KITEM_H_THAY))
    ket.append(va("KItem.cpp", KITEM_CPP_TIM, KITEM_CPP_THAY))
    ket.append(va("ScriptFuns.cpp", SF_TIM_1, SF_THAY_1))
    ket.append(va("ScriptFuns.cpp", SF_TIM_2, SF_THAY_2))
    loi = 0
    for ok, msg in ket:
        print(msg)
        if not ok:
            loi += 1
    print("=" * 78)
    print("  %s" % ("XONG - khong loi" if loi == 0 else "CO %d MUC LOI" % loi))
    return 1 if loi else 0


if __name__ == "__main__":
    raise SystemExit(main())
