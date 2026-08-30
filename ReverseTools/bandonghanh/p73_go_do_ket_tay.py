# -*- coding: utf-8 -*-
r"""[PETSYS 30/08] Chu bi item ket TREN TAY (pos_hand = 1, GameDataDef.h:321)
-> con tro dinh mon do, khong bam duoc NPC. Item o pos_hand KHONG nam trong
hanh trang nen ConsumeEquiproomItem khong dong toi duoc.

Them ham Lua C: PET_ClearHand()
 - duyet m_ItemList cua nguoi choi (khuon DT_ThuHoiBox - CoreShell.cpp:3672)
 - moi item o pos_hand: neu hanh trang con cho thi DAT VAO TUI, khong con
   cho thi XOA HAN (mon ket nay von la ban ghi loi)
 - tra ve so mon da xu ly
Dang ky vao ScriptFuns + goi tu menu admin "Dọn item pet lỗi".
"""
import io

CR = chr(13)


def doc_lf(p):
    return io.open(p, "r", encoding="latin-1", newline="").read().replace(CR + "\n", "\n")


def ghi_crlf(p, lf):
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))


# ---------- 1. ham C ----------
p = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerPet.cpp"
lf = doc_lf(p)
if "LuaPET_ClearHand" not in lf:
    neo = "int LuaPET_IsCreate(Lua_State* L)"
    assert lf.count(neo) == 1
    them = """// [30/08] Go item KET TREN TAY (pos_hand) - con tro dinh mon do lam
// khong bam duoc NPC. Uu tien tra ve hanh trang; het cho thi xoa han.
int LuaPET_ClearHand(Lua_State* L)
{
	int nIdx = sPetCtx(L);
	int nDone = 0;
	if (nIdx <= 0 || nIdx >= MAX_PLAYER)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	for (int nVong = 0; nVong < 32; nVong++)
	{
		PlayerItem* pIt = Player[nIdx].m_ItemList.GetFirstItem();
		int nFound = 0;
		while (pIt)
		{
			if (pIt->nPlace == pos_hand && pIt->nIdx > 0)
			{
				nFound = pIt->nIdx;
				break;
			}
			pIt = Player[nIdx].m_ItemList.GetNextItem();
		}
		if (!nFound)
			break;
		int nX = 0, nY = 0;
		if (Player[nIdx].m_ItemList.CheckCanPlaceInEquipment(
			Item[nFound].GetWidth(), Item[nFound].GetHeight(), &nX, &nY))
			Player[nIdx].m_ItemList.Add(nFound, pos_equiproom, nX, nY);
		else
		{
			Player[nIdx].m_ItemList.Remove(nFound);
			ItemSet.Remove(nFound);
		}
		nDone++;
	}
	Lua_PushNumber(L, nDone);
	return 1;
}

int LuaPET_IsCreate(Lua_State* L)"""
    lf = lf.replace(neo, them, 1)
    ghi_crlf(p, lf)
    print("1. them LuaPET_ClearHand")
else:
    print("1. da co")

# ---------- 2. dang ky ----------
p = r"D:\GAMEDEVNEW\Sources\Core\Src\ScriptFuns.cpp"
lf = doc_lf(p)
if '"PET_ClearHand"' not in lf:
    neo = "extern int LuaPET_GetName(Lua_State* L);"
    assert lf.count(neo) == 1
    lf = lf.replace(neo, neo + "\nextern int LuaPET_ClearHand(Lua_State* L);", 1)
    neo2 = '\t\t{ "PET_GetName",\tLuaPET_GetName },'
    assert lf.count(neo2) == 1
    lf = lf.replace(neo2, neo2 + '\n\t\t{ "PET_ClearHand",\tLuaPET_ClearHand },\t// [30/08] go do ket tren tay', 1)
    ghi_crlf(p, lf)
    print("2. dang ky PET_ClearHand")
else:
    print("2. da co")

# ---------- 3. menu admin goi them ----------
import sys
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn  # noqa: E402

p = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\bdh_admin.lua"
lf = doc_lf(p)
if "PET_ClearHand" not in lf:
    i = lf.find("function BDH_P_DonLoi()")
    assert i > 0
    j = lf.find("\nend", i)
    moi = '''function BDH_P_DonLoi()
	local i
	local nDem = 0
	for i = 4874, 4932 do
		local nCo = CalcEquiproomItemCount(6, 1, i, -1)
		if nCo > 0 then
			ConsumeEquiproomItem(nCo, 6, 1, i)
			nDem = nDem + nCo
		end
	end
	local nTay = 0
	if PET_ClearHand ~= nil then
		nTay = PET_ClearHand()
	end
	Msg2Player(format("''' + vn("Đã dọn %d vật phẩm trong túi và %d món kẹt trên tay") + '''", nDem, nTay))'''
    lf = lf[:i] + moi + lf[j:]
    ghi_crlf(p, lf)
    print("3. menu admin goi PET_ClearHand")
else:
    print("3. da co")
print("XONG p73")
