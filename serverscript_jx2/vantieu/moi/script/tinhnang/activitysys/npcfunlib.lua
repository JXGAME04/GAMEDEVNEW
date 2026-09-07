-- =========================================================================
-- npcfunlib.lua   [LMBC 06/09]
--
-- CHI port phuong thuc NpcFunLib:AddObjNpc cua ban Linux
--   D:\ServerLinux\server1\script\activitysys\npcfunlib.lua:237-247 (11 dong)
--
-- KHONG be ca 271 dong ban goc: cac phuong thuc con lai (Hide, CheckInMap,
-- CheckNpcSeries, DropAward, AddFightNpc...) can NPCINFO_GetSeries, HideNpc,
-- tbDropTemplet:GiveAwardByList va lib:CheckInList - JX1 khong co.
-- Da ra soat CA HAI nhanh van tieu, chi goi DUNG phuong thuc nay:
--   script\activitysys\config\129\config.lua:18  NpcFunLib:AddObjNpc
--   script\activitysys\config\129\config.lua:19  NpcFunLib:AddObjNpc
-- (hai dong AddDialogNpc trong config.lua da bi chu thich san tu ban goc).
--
-- Can them phuong thuc ve sau thi THEM VAO DAY, dung ghi de tep.
-- =========================================================================

if not NpcFunLib then
	NpcFunLib = {};
end

-- AddNpc cua JX1: AddNpc(nNpcId, nLevel, nSubWorldIdx, nX32, nY32, nSeries, szName)
--   Sources\Core\Src\ScriptFuns.cpp:7034 - DUNG THU TU tham so voi ban Linux
--   (khac AddNpcEx: ban Linux dao nSeries/nSubWorldIdx, xem chu thich
--    ScriptFuns.cpp truoc LuaAddNpcEx) nen chep nguyen 11 dong goc.
-- SubWorldID2Idx tra ve -1 khi ban do chua nap (ScriptFuns.cpp:542) nen
-- phep so sanh nMapIndex >= 0 luon hop le.
function NpcFunLib:AddObjNpc(szNpcName, nNpcId, tbNpcPos, szLuaFile)
	for i = 1, getn(tbNpcPos) do
		local pPos = tbNpcPos[i];

		local nMapIndex = SubWorldID2Idx(pPos[1]);
		if nMapIndex >= 0 then
			local nNpcIndex = AddNpc(nNpcId, 1, nMapIndex, pPos[2] * 32, pPos[3] * 32, 0, szNpcName);
			SetNpcScript(nNpcIndex, szLuaFile);
		end
	end
end
