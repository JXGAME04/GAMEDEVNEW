-- ================================================================================================
-- [HE THONG] script/log_game/log_giaodich.lua
-- Muc dich  : Log giao dich.
-- Duoc nap  : engine nap luc boot (moi tep .lua = 1 lua_State rieng) + C++ goi truc tiep theo ten tep
-- Include   : save_log.lua, lib_map.lua
-- Ham (dong): linebegin (9), trademoney (33), tradeitem (39)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
--Author Fong KiÒu
--Date 21/01/2021
--Script luu lich su giao dich cua player

Include("\\script\\log_game\\save_log.lua")
Include("\\script\\lib\\lib_map.lua")

-- Buoc 1 : Luu lai thong tin ban dau
function linebegin(playerIndexK)
	
	--dofile("script/log_game/log_giaodich.lua")

	local szAccount = GetAccount()
	local szName = GetName()
	local szIP = GetIP()
	
	PlayerIndex = playerIndexK
	local szTarAccount = GetAccount()
	local szTarName = GetName()
	local szTarIP = GetIP()
	local nW,nX,nY = GetWorldPos()
	nX = floor(nX / 8)
	nY = floor(nY / 16)
	local log = "**************Start Log Trade***********************\n"
	log = log..date("%H:%M:%S").."\tAccount chñ: "..szAccount.."\tPlayerName chñ: "..szName.."\tIP chñ: "..szIP.."\n"
	log = log.."\tAccount kh¸ch: "..szTarAccount.."\tPlayerName Kh¸ch: "..szTarName.."\tIP kh¸ch: "..szTarIP.."\n"
	log = log.."\tVÞ trÝ giao dÞch ("..nX.."/"..nY..") "..GetMapName(nW).."\n"
	
	logWriteTrade(log)
end

-- Buoc 2: Luu lai so tien da giao dich
function trademoney(nmoneyC,nmoneyK)
	local log = "\tTiÒn giao dÞch [chñ nhËn: "..nmoneyC.." kh¸ch nhËn: "..nmoneyK.."]\n"
	logWriteTrade(log)
end

-- Buoc 3: Luu lai item da giao dich
function tradeitem(itemidx, nowner)

	local szItemName = GetNameItem(itemidx)
	local nGoldId,nGen,nDetail,nParti,nLevel,nSeries = GetCBItem(itemidx)
	
	local Rmatype1,RmatypeV1,Rmatype2,RmatypeV2,Rmatype3,RmatypeV3,
		  Rmatype4,RmatypeV4,Rmatype5,RmatypeV5,Rmatype6,RmatypeV6 = GetOTItem(itemidx)
	
	local sowner = "" 
	if(nowner == 1) then 
		sowner = "Item kh¸ch nhËn:"
	else
		sowner = "Item chñ nhËn:"
	end
	local log = "\t["..sowner.."]\t["..szItemName.."]"
	log = log.."\t[nGoldId: "..nGoldId.."]\t[nGen: "..nGen.."]\t[nDetail: "..nDetail.."]\t[nParti: "..nParti.."]\t[nLevel: "..nLevel.."]\t[nSeries: "..nSeries.."]"
	log = log.."\t[Option:]\t["..Rmatype1.."|"..RmatypeV1.."]\t["..Rmatype2.."|"..RmatypeV2.."]\t["..Rmatype3.."|"..RmatypeV3.."]\t["..Rmatype4.."|"..RmatypeV4.."]\t["..Rmatype5.."|"..RmatypeV5.."]\t["..Rmatype6.."|"..RmatypeV6.."]\n"
	logWriteTrade(log)
end