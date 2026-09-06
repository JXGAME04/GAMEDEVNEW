-- Cuon Mat Chi (6/1/212) - cong task 1025 hoac manh SHXT 1027
-- [PB 19/08] SUA LOI CO SAN: ban cu bao nham la "Dia Do Chi" trong file Mat Chi.
-- [PB 19/08/2026] Them MAU + TEN MAP + so tam hien tai vao thong bao.
-- File nay THUAN ASCII: tieng Viet viet bang escape thap phan \\ddd cua Lua 4.
--
-- KHONG DUOC DOI LOI VAN: auto Da Tau (CoreShell.cpp:4658) do bang strstr cac cum
--   "Ban nhan duoc mot tam" / "tong cong" / " tam."
--   (viet khong dau o day cho de doc; chuoi THAT nam trong ma ben duoi)
-- khai trong KDaTauTables.h. Doi chu = auto khong dem duoc cuon nua.
-- Chi duoc BOC THE MAU quanh chung (DT_NumAfter bo qua ky tu khong phai so).
--
-- Dau cach ASCII truoc the <color...> la BAT BUOC khi phia truoc la chu Viet
-- (Text.cpp:468 nuot dau '<' sau day LE byte >0x80).

IncludeLib("BATTLE");
Include("\\script\\task\\newtask\\newtask_head.lua");
Include("\\script\\task\\newtask\\map_index.lua");
Include("\\script\\task\\newtask\\lib_setmembertask.lua");

function AddMapValues()

local myMapID, myMapName, myMapX, myMapY
local myTaskType = nt_getTask(1021)
local nWorldMaps = nt_getTask(1027)
local myMapNum = nt_getTask(1025)

myMapID = SubWorldIdx2ID( SubWorld )

	if (myTaskType == 4) then

		myMapName, myMapX, myMapY = tl_getMapInfo(myMapID)

		-- [PB 19/08] tl_getMapInfo chi phu 204 map, ngoai bang tra 0/nil. Ban cu de ten
		-- RONG lam cau cut; nay lui ve so hieu map cho con doc duoc.
		if (myMapName == 0) or (myMapName == nil) or (myMapName == "") then
			myMapName = "map "..myMapID
		end

		if (nt_getTask(1031) == myMapID) then

			if (GetByte(nt_getTask(1032),1) == 2) then

				myMapNum = myMapNum + 1
				nt_setTask(1025,myMapNum)
				Msg2Player("B¹n nhËn ®­îc mét tÊm".." <color=Yellow>".."MËt ChØ".." <color>".."t¹i"..
					" <color=Cyan>"..myMapName.." <color>".."! HiÖn t¹i b¹n cã tæng céng"..
					" <color=AYellow>"..myMapNum.."<color>".." tÊm.");

				return 0
			end
		end

		nWorldMaps = nWorldMaps + 1
		nt_setTask(1027,nWorldMaps)
		Msg2Player("B¹n nhËn ®­îc mét m¶nh b¶n ®å S¬n Hµ X· T¾c! HiÖn t¹i b¹n cã tæng céng".." <color=AYellow>"..nWorldMaps.."<color>".." m¶nh b¶n ®å S¬n Hµ X· T¾c.");

	else
		nWorldMaps = nWorldMaps + 1
		nt_setTask(1027,nWorldMaps)
		Msg2Player("B¹n nhËn ®­îc mét m¶nh b¶n ®å S¬n Hµ X· T¾c! HiÖn t¹i b¹n cã tæng céng".." <color=AYellow>"..nWorldMaps.."<color>".." m¶nh b¶n ®å S¬n Hµ X· T¾c.");
	end

end

-- [PB 17/08/2026] Vong chia to doi theo binding JX1: GetTeamMember(0) = doi truong.
-- Luu y: bien SubWorld KHONG doi trong vong lap, nen ten map trong thong bao cua
-- dong doi la map cua NGUOI NHAT (thuc te ca doi thuong cung map).
function TLG_ChiaToDoi()
	local nMe = PlayerIndex
	AddMapValues()
	local nMemCount = GetTeamSize()
	if (nMemCount and nMemCount > 1) then
		local i
		for i = 0, nMemCount do
			local nMem = GetTeamMember(i)
			if (nMem and nMem > 0 and nMem ~= nMe) then
				PlayerIndex = nMem
				AddMapValues()
			end
		end
		PlayerIndex = nMe
	end
end

function PickUp( nItemIndex, nPlayerIndex )
	TLG_ChiaToDoi()
	return 0
end

-- click-phai cuon trong tui: TRU CUON TRUOC roi moi chia (chong farm neu co loi
-- giua chung - PB 17/08 phat hien exploit khi remove nam sau vong chia)
function main(nItemIndex)
	RemoveItemByIndex(nItemIndex)
	TLG_ChiaToDoi()
	return 0
end
