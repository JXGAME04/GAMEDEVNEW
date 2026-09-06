-------------------------------------------------------------------
-- Filename	: S˘ Ki÷n Hπt Huy Hoµng
-- Author	: DzungDolby
-------------------------------------------------------------------

SCRIPT_HAT_HUYHOANG 				= "\\script\\event\\event_huyhoang_dungdb\\hathuyhoang.lua"
SCRIPT_QUA_HUYHOANG 			= "\\script\\event\\event_huyhoang_dungdb\\quahuyhoang.lua"
TIME_SWITCH 									= 5 * 60 * 18 -- ThÍi gian chuy”n ÆÊi hπt sang qu∂ 5p 5*60*18
TIME_DELETE_QHH 						= 5 * 60 * 18 --thÍi gian hu˚ qu∂ huy hoµng 5p 5*60*18
MAX_HUYHOANG_PER_DAY 		= 5 -- sˆ dÙng tËi Æa 5 qu∂ huy hoµng
MAX_QHK_PER_DAY 						= 5 -- sˆ dÙng tËi Æa 2 qu∂ hoµng kim
MAX_COLLECT_PER_DAY_HH	= 10 -- h∏i tËi Æa 10 qu∂ huy hoµng
MAX_COLLECT_PER_DAY_HK	= 3 -- h∏i tËi Æa 3 qu∂ hoµng kim
EXP_QHH_T										=	20000000
EXP_QHH_TR										=	30000000
EXP_QHH_C										=	50000000
EXP_QHK												= 100000000
TIENVAN_QHK												= 500000
TG_SD_QUAHH 									= 60*60*24*7
TG_SD_QUAHK 									= 60*60*24*7
ID_QHH_THAP 									= 905
ID_QHH_TR 										= 906
ID_QHH_C 											= 907
ID_QHK 												= 908
TIME_DELAY_TH 								= 5 -- 5 gi©y thu hoπch

TAB_TIME_HAT_HH = {
	{12,0},
	{12,10},
	{12,20},
	{19,0},
	{19,10},
	{19,20},	
}

greatseed_configtab = {
	-- {340,3,6,"\\settings\\maps\\great_night\\ƒ™∏ﬂøﬂ.txt","Mπc Cao QuÀt"},			--ƒ™∏ﬂøﬂ
	--{336,3,100,"\\settings\\maps\\great_night\\∑Á¡Í∂….txt","Phong L®ng ßÈ"},			--∑Á¡Í∂…
	--{999,3,30,"\\settings\\maps\\great_night\\bienkinhphu.txt","ß u Tr≠Íng Sinh Tˆ"},		-- ≥§∞◊…Ωƒœ
	--{37,2,20,"\\settings\\maps\\great_night\\bienkinhquaso.txt","Bi÷n Kinh"},		-- ≥§∞◊…Ωƒœ
	--{999,4,3,"\\settings\\maps\\great_night\\bienkinhquahk.txt","ß u Tr≠Íng Sinh Tˆ"},		-- ≥§∞◊…Ωƒœ
	--{322,2,100,"\\settings\\maps\\great_night\\≥§∞◊…Ω±±¢¥.txt","Tr≠Íng Bπch S¨n Bæc"},	-- ≥§∞◊…Ω±±¢¥
	--{321,2,100,"\\settings\\maps\\great_night\\≥§∞◊…Ωƒœ.txt","Tr≠Íng Bπch S¨n Nam"},		-- ≥§∞◊…Ωƒœ
	--{225,2,30,"\\settings\\maps\\great_night\\…≥ƒÆ…Ω∂¥1.txt","Sa mπc S¨n ßÈng 1"},		-- ≥§∞◊…Ωƒœ
	--{226,2,30,"\\settings\\maps\\great_night\\…≥ƒÆ…Ω∂¥2.txt","Sa mπc S¨n ßÈng 2"},		-- ≥§∞◊…Ωƒœ
	{995,2,10,"\\settings\\maps\\great_night\\…≥ƒÆ…Ω∂¥3.txt","Ki’m M´n Quan"},		-- ≥§∞◊…Ωƒœ
	--{182,1,25,"\\settings\\maps\\great_night\\ƒı¡˙∂¥√‘π¨.txt","Nghi÷t Long ßÈng"},	-- ≥§∞◊…Ωƒœ
	--{167,1,10,"\\settings\\maps\\great_night\\µ„≤‘…Ω.txt","ßi”m Th≠¨ng S¨n"},			-- ≥§∞◊…Ωƒœ
	--{200,1,25,"\\settings\\maps\\great_night\\π≈—Ù∂¥√‘π¨.txt","CÊ D≠¨ng ßÈng"},	-- ≥§∞◊…Ωƒœ
	--{92,1,25,"\\settings\\maps\\great_night\\ Ò∏‘…Ω.txt","ThÙc C≠¨ng S¨n"},				-- ≥§∞◊…Ωƒœ
	--{341, 4, 1, "\\settings\\maps\\great_night\\ƒÆ±±≤›‘≠.txt","Mπc Bæc Th∂o Nguy™n"},		-- ƒÆ±±≤›‘≠
	{995, 4, 3, "\\settings\\maps\\great_night\\ª™…Ω≈….txt","Ki’m M´n Quan"},			-- ª™…Ω≈… 
	--{319, 4, 3, "\\settings\\maps\\great_night\\¡Ÿ”Âπÿ.txt","L©m Du Quan"},			-- ¡Ÿ”Âπÿ
	--{959, 4, 3, "\\settings\\maps\\great_night\\À´¡˙∂¥.txt","Kh·a Lang ßÈng"},			-- À´¡˙∂¥
	--{181, 4, 1, "\\settings\\maps\\great_night\\¡ΩÀÆ∂¥.txt","L≠Ïng ThÒy ßÈng"},			-- ¡ΩÀÆ∂¥
}

function Add_HatHuyHoang()
	local nNowTime = tonumber(date("%H%M"))
	local nBatch = floor(mod(nNowTime,100)/5) + 1
	if nNowTime >= 1200 and nNowTime < 1205 then
		print(format("QUA HUY HOANG %d", nNowTime))
	end	
	for i = 1, getn(greatseed_configtab) do
		Global_GreatSeedExecute(greatseed_configtab[i][1], greatseed_configtab[i][2], greatseed_configtab[i][3], greatseed_configtab[i][4],greatseed_configtab[i][5],nBatch)
		local szMsg = ""
		if (mod(nBatch,2) == 1) and greatseed_configtab[i][2] ~= 4 then
			szMsg = "Hπt Huy Hoµng Cao"
		elseif greatseed_configtab[i][2] == 4 then
			szMsg = "Hπt Huy Hoµng Trung"
		elseif (mod(nBatch,2) == 0) and greatseed_configtab[i][2] ~= 4 then
			szMsg = "Hπt Huy Hoµng Ti”u"
		elseif greatseed_configtab[i][2] == 4 then
			szMsg = "Hπt Hoµng Kim"
		end
		if (mod(nBatch,2) == 1) then
			szMsg = format("Tr≠Ìc mæt lµ thÍi gian m≠a thuÀn gi„ hﬂa, <color=yellow>Administrator<color> Æ∑ gieo <color=green><%s><color> tπi %s, 5 phÛt sau c∏c ßπi Hi÷p c„ th” thu hoπch!!", szMsg, greatseed_configtab[i][5])
			AddLocalNews(szMsg,3)
			--Msg2SubWorld(szMsg)
			logHoatDong(szMsg)
		elseif (mod(nBatch,2) == 0) then
			szMsg = format("G∆p thÍi ti’t thuÀn lÓi <color=pink><%s><color> do <color=yellow>Administrator<color> gieo hπt Æ∑ k’t qu∂ tπi <color=green>%s<color>, C∏c ßπi Hi÷p h∑y nhanh ch„ng thu hoπch!!", szMsg, greatseed_configtab[i][5])
			AddLocalNews(szMsg,3)
			--Msg2SubWorld(szMsg)
			logHoatDong(szMsg)
		end		
	end
end

function Global_GreatSeedExecute(worldidx, seedlevel, count, SeedPosFile, szMapName, nBatch)
	--Msg2Player(format("%d %d %d %s %s %d", worldidx, seedlevel, count, SeedPosFile, szMapName, nBatch))
	local nRetCode = TabFile_Load(SeedPosFile, SeedPosFile)
	if (nRetCode == 0)then
		print("Read file"..SeedPosFile.."Failed in function 'Global_GreatSeedExecute()'")
		return
	end
	local nNpcIdx = 0
	local nTimeMinutesNow = GetTimeMinutesNow()
	local nPosX, nPosY
	local nNpcTmpl = 1110
	local szNpcName = "Hπt Huy Hoµng"
	local szNpcScriptFile = SCRIPT_HAT_HUYHOANG
	if mod(nBatch,2) == 0 then
		if seedlevel == 4  then
			nNpcTmpl = 1118
			szNpcScriptFile = SCRIPT_QUA_HUYHOANG
			szNpcName = "Qu∂ Hoµng Kim"
		else
			nNpcTmpl = 1111
			szNpcScriptFile = SCRIPT_QUA_HUYHOANG
			szNpcName = "Qu∂ Huy Hoµng"
		end
	elseif seedlevel ==  4 then
			nNpcTmpl = 1117
			szNpcName = "Hπt Hoµng Kim "
	end
	local nLineCount = TabFile_GetRowCount(SeedPosFile) - 1
	count = (nLineCount < count ) and nLineCount or  count
	for i = 1, count do
		worldidx = tonumber(TabFile_GetCell(SeedPosFile, i + 1, "map_ID"))
		nPosX = tonumber(TabFile_GetCell(SeedPosFile, i + 1, "nPosX"))
		nPosY = tonumber(TabFile_GetCell(SeedPosFile, i + 1, "nPosY"))
		if (nil ~= nPosX and nil ~= nPosY) then
			print(format("Add Qua huy hoang toa do:%d %d %d", worldidx, nPosX / 8, nPosY / 16))
			nNpcIdx = AddNpc(nNpcTmpl , 1, SubWorldID2Idx(worldidx), nPosX * 32, nPosY * 32, 0 , szNpcName)
			if (nNpcIdx > 0) then
				SetNpcScript(nNpcIdx, szNpcScriptFile)
				SetNpcParam(nNpcIdx, 1, seedlevel)
				SetNpcParam(nNpcIdx, 3, nTimeMinutesNow + TIME_SWITCH)
				SetNpcTimer(nNpcIdx, TIME_SWITCH)
				if seedlevel == 4  then
					Msg2SubWorld("<color=yellow>"..szNpcName.."<color>".." xu t hi÷n tπi "..szMapName.." ("..floor(nPosX / 8)..","..floor(nPosY / 16)..") . ")
				end				
			end			
		end
	end
	TabFile_UnLoad(SeedPosFile)
end

function Add_QuaHuyHoang(nLoaiHat,nx,ny,nw)
	local nNewNpcIdx = 0
	local szNpcName = "Qu∂ Huy Hoµng"
	if (nLoaiHat == 1) then	 --- Add Qua Huy Hoang Tieu
		nNewNpcIdx = AddNpc(1111, 1, nw, nx, ny, 0, szNpcName)
		SetNpcScript(nNewNpcIdx, SCRIPT_QUA_HUYHOANG)
		SetNpcParam(nNewNpcIdx,1,nLoaiHat)
		SetNpcTimer(nNewNpcIdx, TIME_DELETE_QHH)
	elseif (nLoaiHat == 2) then	--- Add Qua Huy Hoang Trung
		nNewNpcIdx = AddNpc(1111, 1, nw, nx, ny, 0, szNpcName)
		SetNpcScript(nNewNpcIdx, SCRIPT_QUA_HUYHOANG)
		SetNpcParam(nNewNpcIdx,1,nLoaiHat)
		SetNpcTimer(nNewNpcIdx, TIME_DELETE_QHH)
    elseif (nLoaiHat == 3) then		--- Add Qua Huy Hoang Dai
		nNewNpcIdx = AddNpc(1111, 1, nw, nx, ny, 0, szNpcName)
		SetNpcScript(nNewNpcIdx, SCRIPT_QUA_HUYHOANG)
		SetNpcParam(nNewNpcIdx,1,nLoaiHat)
		SetNpcTimer(nNewNpcIdx, TIME_DELETE_QHH)
    elseif (nLoaiHat == 4) then		--- Add Qua Hoang Kim
		szNpcName = "Qu∂ Hoµng Kim"
		nNewNpcIdx = AddNpc(1118, 1, nw, nx, ny, 0, szNpcName)
		SetNpcScript(nNewNpcIdx, SCRIPT_QUA_HUYHOANG)
		SetNpcParam(nNewNpcIdx,1,nLoaiHat)
		SetNpcTimer(nNewNpcIdx, TIME_DELETE_QHH)			
	end
end

function GetTimeMinutesNow()
	local nYr, nMth, nDay, nHr, nMi, nSec = GetTimeNow()
	local nTimeMinutesNow = (nMi * 60 + nSec) * 18
	return nTimeMinutesNow
end

