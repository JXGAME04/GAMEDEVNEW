--Author: X
--Date: 2025
--Function: Mission PUBG

Include("\\script\\tinhnang\\pubg\\pubgutils.lua")

function BeginMission()
	local nSubWorldId
	nSubWorldId = SubWorldID2Idx(BIENKINHPUBG)
	if nSubWorldId < 0 then	--chua mo map, ngung ham`
		return 
	end
	SubWorld = nSubWorldId
end

function EndMission()
	local nSubWorldId
	nSubWorldId = SubWorldID2Idx(BIENKINHPUBG)
	if nSubWorldId < 0 then	--chua mo map, ngung ham`
		return 
	end
	SubWorld = nSubWorldId
end

--loai bo player ra khoi mission thi se chay ham nay, thoat ra cung la loai bo
function OnLeave(nPlayerIndex)
	if(SubWorld == SubWorldID2Idx(BIENKINHPUBG)) then
		pubg_out(nPlayerIndex)
		return 
	end
end

function pubg_out(nPlayerIndex)
	PlayerIndex = nPlayerIndex
	KickOutSelf()
end
