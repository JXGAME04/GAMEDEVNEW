# -*- coding: utf-8 -*-
"""(21/08) Quy uoc JX2 cho script item port tu Linux: main() tra != 1 -> engine tu tru 1 vat pham.
+ GetItemParam(idx,1) = o tham so 1 (Linux) thay vi so luong chong (khong script nao dung)."""
import io, sys
T = "\t"
SF = r"D:\GAMEDEVNEW\Sources\Core\Src\ScriptFuns.cpp"
IL = r"D:\GAMEDEVNEW\Sources\Core\Src\KItemList.cpp"
KP = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayer.cpp"
KH = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayer.h"
def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hi(s): return sum(1 for c in s if ord(c) > 127)
def rep1(s, old, new, crlf=False):
    if crlf:   # tep CRLF: old/new viet bang \n -> doi sang \r\n
        old = old.replace("\r\n", "\n").replace("\n", "\r\n")
        new = new.replace("\r\n", "\n").replace("\n", "\r\n")
    assert s.count(old) == 1, (s.count(old), old[:70])
    return s.replace(old, new, 1)

# ---------- KPlayer.h: khai bao ----------
s = rd(KH); h = hi(s)
old = "\tBOOL\t\t\tExecuteScript(char * ScriptFileName, char * szFunName, int nParam = 0, bool bGlobal = true);\n"
new = old + "#ifdef _SERVER\n\tBOOL\t\t\tExecuteItemScriptJX2(char * ScriptFileName, int nItemIdx, int* pnRet);\t// [JX2 ITEM 21/08] main(idx) -> gia tri tra\n#endif\n"
s = rep1(s, old, new, True); assert hi(s) == h; wr(KH, s); print("KPlayer.h ok")

# ---------- KPlayer.cpp: than ham (canh ExecuteScript(char*,char*,int,bool)) ----------
s = rd(KP); h = hi(s)
old = "BOOL\tKPlayer::ExecuteScript(DWORD dwScriptId,  char * szFunName, int nParam, bool bGlobal)\n{\n"
new = r"""#ifdef _SERVER
// [JX2 ITEM 21/08] Goi main(nItemIdx) cua script item va DOC GIA TRI TRA (ExecuteScript goc goi
// CallFunction voi 0 ket qua nen bo qua). Engine JX2/Linux: main tra != 1 -> tu tru 1 vat pham;
// script port tu Linux (bosscharm.lua tra 0, che*fu.lua `return`, honnguyenchandon tra 0) dua
// vao quy uoc do. *pnRet = 1 neu khong doc duoc (an toan: khong tru).
BOOL	KPlayer::ExecuteItemScriptJX2(char * ScriptFileName, int nItemIdx, int* pnRet)
{
	if (pnRet)
		*pnRet = 1;
	try
	{
		DWORD dwScriptId = g_FileName2Id(ScriptFileName);
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
		if (!pScript)
			return FALSE;
		Npc[m_nIndex].m_ActionScriptID = dwScriptId;
		Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);
		pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
		Lua_PushNumber(pScript->m_LuaState, m_dwID);
		pScript->SetGlobalName(SCRIPT_PLAYERID);
		Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
		pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
		int nTopIndex = 0;
		BOOL bOk = FALSE;
		pScript->SafeCallBegin(&nTopIndex);
		if (pScript->CallFunction("main", 1, "d", nItemIdx))
		{
			bOk = TRUE;
			if (pnRet)
			{
				int nTop = Lua_GetTopIndex(pScript->m_LuaState);
				if (nTop > nTopIndex && Lua_IsNumber(pScript->m_LuaState, nTop))
					*pnRet = (int)Lua_ValueToNumber(pScript->m_LuaState, nTop);
				else
					*pnRet = 0;	// tra nil / khong tra -> JX2: tru vat pham
			}
		}
		pScript->SafeCallEnd(nTopIndex);
		if (!bOk)
		{
			m_bWaitingPlayerFeedBack = false;
			m_btTryExecuteScriptTimes = 0;
			Npc[m_nIndex].m_ActionScriptID = 0;
			if (pnRet)
				*pnRet = 1;
			return FALSE;
		}
		return TRUE;
	}
	catch(...)
	{
		printf("-->Error ExecuteItemScriptJX2: [%s]\n", ScriptFileName);
		m_bWaitingPlayerFeedBack = false;
		m_btTryExecuteScriptTimes = 0;
		Npc[m_nIndex].m_ActionScriptID = 0;
		if (pnRet)
			*pnRet = 1;
		return FALSE;
	}
}
#endif

""" + old
s = rep1(s, old, new, True); assert hi(s) == h; wr(KP, s); print("KPlayer.cpp ok")

# ---------- KItemList.cpp: nhanh item_magicscript ----------
s = rd(IL); h = hi(s)
old = ("\t\tPlayer[m_PlayerIdx].ExecuteScript(Item[nIdx].GetScript(),\"main\", nIdx, bGlobal);\n"
       "\t}\n"
       "\t\n"
       "\telse if (nGenre == item_townportal)\n")
new = ("#ifdef _SERVER\n"
       "\t\t// [JX2 ITEM 21/08] script item port nguyen ban tu Linux (JX2) dua vao quy uoc engine JX2:\n"
       "\t\t// main() tra != 1 -> engine tu tru 1 vat pham. Engine JX1 bo qua gia tri tra nen\n"
       "\t\t// bosscharm.lua (Lenh bai boss bang hoi, tra 0) / che*fu.lua / honnguyenchandon.lua\n"
       "\t\t// khong bao gio mat item. CHI ap dung cho danh sach duong dan JX2 (sIsJx2ItemScript)\n"
       "\t\t// de khong doi hanh vi script JX1 (tu RemoveItem roi return nil).\n"
       "\t\tif (sIsJx2ItemScript(ScriptFileName))\n"
       "\t\t{\n"
       "\t\t\tDWORD dwItemID = Item[nIdx].GetID();\n"
       "\t\t\tint nRet = 1;\n"
       "\t\t\tPlayer[m_PlayerIdx].ExecuteItemScriptJX2(ScriptFileName, nIdx, &nRet);\n"
       "\t\t\tif (nRet != 1 && SearchID((int)dwItemID) == nIdx)\t// script co the da tu tru (ConsumeItem) -> kiem lai\n"
       "\t\t\t{\n"
       "\t\t\t\tif (Item[nIdx].IsStack() && Item[nIdx].GetStackNum() > 1)\n"
       "\t\t\t\t{\n"
       "\t\t\t\t\tItem[nIdx].SetStackNum(Item[nIdx].GetStackNum() - 1);\n"
       "\t\t\t\t\tthis->SyncItem(nIdx);\n"
       "\t\t\t\t}\n"
       "\t\t\t\telse\n"
       "\t\t\t\t{\n"
       "\t\t\t\t\tthis->Remove(nIdx);\n"
       "\t\t\t\t\tItemSet.Remove(nIdx);\n"
       "\t\t\t\t}\n"
       "\t\t\t}\n"
       "\t\t}\n"
       "\t\telse\n"
       "#endif\n"
       "\t\tPlayer[m_PlayerIdx].ExecuteScript(Item[nIdx].GetScript(),\"main\", nIdx, bGlobal);\n"
       "\t}\n"
       "\t\n"
       "\telse if (nGenre == item_townportal)\n")
s = rep1(s, old, new, True)
# ham tinh: dat truoc BOOL KItemList::EatMecidine
old = "BOOL KItemList::EatMecidine(int nIdx)\n{\n"
new = r"""#ifdef _SERVER
// [JX2 ITEM 21/08] danh sach script item port tu Linux dung quy uoc "tra != 1 -> tru vat pham"
static BOOL sIsJx2ItemScript(const char* szScript)
{
	static const char* szJx2[] = {
		"\\script\\item\\messenger\\",					// Tin Su: 5 yeu bai
		"\\script\\item\\xinshirenwu\\",				// Tin Su: Ngu Hanh Phu, Triet X Phu, Bao Ruong, Thien Bao Kho Lenh
		"\\script\\item\\bosscharm.lua",				// Lenh bai boss bang hoi (tra 0 sau khi goi boss)
		"\\script\\item\\event\\kinhmach\\honnguyenchandon.lua",	// Hon nguyen chan don (tra 0)
	};
	if (!szScript || !szScript[0])
		return FALSE;
	for (int i = 0; i < (int)(sizeof(szJx2) / sizeof(szJx2[0])); i++)
	{
		int n = (int)strlen(szJx2[i]);
		if (_strnicmp(szScript, szJx2[i], n) == 0)
			return TRUE;
	}
	return FALSE;
}
#endif

""" + old
s = rep1(s, old, new, True)
assert hi(s) == h; wr(IL, s); print("KItemList.cpp ok")

# ---------- ScriptFuns.cpp: GetItemParam kind 1 -> o tham so 1 ----------
s = rd(SF); h = hi(s)
old = (T + "case 1:\n"
       + T*2 + "if (Item[nItemIndex].CanStack())\n"
       + T*3 + "nResult = (int)Item[nItemIndex].GetStackNum();\n"
       + T*2 + "else\n"
       + T*3 + "nResult = 1;\n"
       + T*2 + "break;\n"
       + T + "case 2: case 3: case 4: case 5: case 6:\n")
new = (T + "case 1: case 2: case 3: case 4: case 5: case 6:\n"
       + T*2 + "// [TIN SU 21/08] kind 1 truoc tra so luong chong - KHONG script nao trong cay dung\n"
       + T*2 + "// (quet 21/08: chi item\\messenger\\toll_*.lua + seasonnpc_expitem.lua goi (idx,1) va\n"
       + T*2 + "// ca hai deu theo nghia Linux = o tham so 1 - bo dem so lan dung yeu bai Tin Su).\n")
s = rep1(s, old, new)
assert hi(s) == h; wr(SF, s); print("ScriptFuns.cpp GetItemParam kind 1 ok")
