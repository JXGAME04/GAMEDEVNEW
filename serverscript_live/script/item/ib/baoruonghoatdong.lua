--Author: Fong KiÒu
--Function: Thæ §Þa phï
--Date: 21/07/2021


Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\lib\\lib_task.lua")

FREECELL_GETITEMBLUE = 10

tbDoXanh =
{
	[1]=
	{
		szName = "Vò khÝ cËn chiÕn",
		tbEquip =
		{
			{"KiÕm",0,0,0},
			{"§ao",0,0,1},
			{"Bæng",0,0,2},
			{"KÝch",0,0,3},
			{"Chïy",0,0,4},
			{"Song ®ao",0,0,5},
		}
	},
	[2]=
	{
		szName = "Vò khÝ tÇm xa",
		tbEquip =
		{
			{"Tiªu",0,1,0},
			{"Phi §ao",0,1,1},
			{"Ná",0,1,2},
		}
	},
}
nItemTempIdx = nil



function main(nItemIdx)
	dofile("script/item/ib/baoruonghoatdong.lua")
	nItemTempIdx = nItemIdx
	OpenGetNumber("NhËp Sè L­îng", "main_callback")
end

function main_callback()
local nItemIdx = nItemTempIdx
local nSoLuongV = GetNumberFromUI()
	if nSoLuongV <= 0 then
        Talk(1,"","Sè l­îng kh«ng hîp lÖ!")
        return
    end
	if nSoLuongV > 10 then
        Talk(1,"","Sè l­îng kh«ng thÓ lín h¬n 10!")
        return
    end

	if CalcFreeItemCellCount() < FREECELL_GETITEMBLUE then
		Talk(1,"","Kho¶ng trèng hµnh trang kh«ng ®ñ "..FREECELL_GETITEMBLUE.." « .")
		return
	end	

	local tbOpt = {"<npc> Xin mêi <sex> lùa chän trang bÞ:"}
	for i=1, getn(tbDoXanh) do
		tinsert(tbOpt, format("%s/#laydoxanh1(%d,%d)", tbDoXanh[i].szName, i, nItemIdx))
	end
	tinsert(tbOpt, "Kh«ng nhËn/no")
	SayEx(tbOpt)
end


function laydoxanh1(nType, nItemIndex)
	local tbEquip = tbDoXanh[nType]["tbEquip"]
	local tbOpt = {"<npc> Xin mêi <sex> lùa chän trang bÞ:"}
	for i=1, getn(tbEquip) do
		tinsert(tbOpt, format("%s/#laydoxanh2(%d,%d,%d)", tbEquip[i][1], i, nType, nItemIndex))
	end
	tinsert(tbOpt, "Kh«ng nhËn/no")
	SayEx(tbOpt)
end


function laydoxanh2(nIndex, nType, nItemIndex)
	local tbOpt = {"<npc> Xin mêi <sex> chän hÖ:"}
	tinsert(tbOpt, format("Kim/#laydoxanh3(%d,%d,%d,%d)", nIndex, nType, 0, nItemIndex))
	tinsert(tbOpt, format("Méc/#laydoxanh3(%d,%d,%d,%d)", nIndex, nType, 1, nItemIndex))
	tinsert(tbOpt, format("Thuû/#laydoxanh3(%d,%d,%d,%d)", nIndex, nType, 2, nItemIndex))
	tinsert(tbOpt, format("Ho¶/#laydoxanh3(%d,%d,%d,%d)", nIndex, nType, 3, nItemIndex))
	tinsert(tbOpt, format("Thæ/#laydoxanh3(%d,%d,%d,%d)", nIndex, nType, 4, nItemIndex))
	tinsert(tbOpt, "Kh«ng nhËn/no")
	SayEx(tbOpt)
end

function laydoxanh3(nIndex, nType, nSeries, nItemIndex)

	if CalcFreeItemCellCount() < FREECELL_GETITEMBLUE then
		Talk(1,"","Kho¶ng trèng hµnh trang kh«ng ®ñ "..FREECELL_GETITEMBLUE.." « .")
		return
	end	
	local nSoLuongV = GetNumberFromUI()
	local tbEquipSelect = tbDoXanh[nType]["tbEquip"][nIndex]


	local nLevel = 10
	local nLucky = 200
	local nMagicLevel = 10
	local nNum = nSoLuongV

	for i = 1, nNum do
		AddItem(
			tbEquipSelect[2],
			tbEquipSelect[3],
			tbEquipSelect[4],
			nLevel,
			nSeries,
			nLucky,
			nMagicLevel
		)
	end
	for i = 1, nNum do
		RemoveItem(nItemIndex)
	end

end



function no()
	
end
