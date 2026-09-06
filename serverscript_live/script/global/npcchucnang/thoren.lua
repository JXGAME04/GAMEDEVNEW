-- Author: Fong KiÒu
-- Date: 28/11/2020
-- Chuc nang: Thî rÌn ba l¨ng huyÖn

Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\log_game\\save_log.lua")
-- [PB 01/09] main() dofile lai tep nay moi lan bam NPC -> Include chay lai lam mat phien tay luyen dang mo
if not onMantleSystem then
	Include("\\script\\global\\mantlesystem\\mantleupgrade_npc.lua")	-- [PHI PHONG 29/08] he tang sao ban Linux
end
-- [DUNGLUYEN 01/09] he dung luyen Van Cuong (ban Linux) - Include MOT LAN (main() dofile lai tep nay moi lan bam NPC)
if not DoSmeltEquip then
	Include("\\script\\global\\smelt_system.lua")
end
if not onUpgradeWenJing then
	Include("\\script\\global\\smeltsystem\\smeltupgrade.lua")
end
-- [EP AN 01/09] main() dofile lai chinh tep nay moi lan noi chuyen; wuxingyin.lua khi nap se
-- tbWuxingyin:Register() (pEventType:Reg) -> chi Include MOT LAN de khong dang ky trung moi lan bam NPC.
if not WXY_MoBang then
	Include("\\script\\event\\equip_publish\\wuxingyin\\wuxingyin.lua")
end

OK_TRADE 												= "Giao dÞch/giaodich"
NOT_TRADE 											= "Kh«ng giao dÞch/no"
HOP_THANH 											= "Hîp thµnh trang bÞ/hopthanhtrangbi"
PAGE_2 													= "Trang 2/hopthanhtrangbi2"
NOW_END_SAY 									= "KÕt thóc ®èi tho¹i./no"
LUACHON 												= "H·y lùa chän trang bÞ cÇn hîp thµnh:"
DIEUKIENHOPTHANH 						= "- M¶nh ghÐp %s sè 1 - 4\n- PhÝ: %d l­îng\n- Cã thÓ ®Æt thªm TTT, LTT, LTT, THBT t¨ng tØ lÖ thµnh c«ng\n- MÆc ®Þnh 30 - 75 phÇn tr¨m\n- Trªn 70 phÇn tr¨m lµ Ðp thµnh c«ng "
NOTCHINHXAC 										= "Vui lßng ®Æt vµo chÝnh x¸c nguyªn liÖu."
MAX_STACK_NUM 								= 500
ID_XU_TIENDONG 								= 417

ITEMHOPTHANH =
{
	{1, "HiÖp Cèt ThiÕt HuyÕt Sam"	,185,903,906,3000000,20},
	{2, "HiÖp Cèt §a T×nh Hoµn"		,186,907,910,3000000,20},
	{3, "HiÖp Cèt T×nh ý KÕt"		,188,771,776,3000000,20},
	{4, "HiÖp Cèt §an T©m Giíi"		,187,911,914,3000000,20},
	{5, "Nhu T×nh Thôc N÷ H¹ng Liªn",190,915,918,3000000,20},
	{6, "Nhu T×nh Phông Nghi Giíi ChØ"	,191,919,922,3000000,20},
	{7, "Nhu T×nh TuÖ T©m Ngäc Béi"		,192,923,926,3000000,20},
	{8, "Nhu T×nh C©n Quèc Nghª Th­êng"	,189,777,782,150000,20},
	{9, "§Þnh Quèc Thanh Sa Tr­êng Sam"	,158,927,930,7000000,10},
	{10,"§Þnh Quèc XÝch Quyªn NhuyÔn Ngoa",160,931,934,5000000,10},
	{11,"§Þnh Quèc Tö §»ng Hé uyÓn"		,161,935,938,5000000,10},
	{12,"§Þnh Quèc Ng©n Tµm Yªu ®¸i"	,162,939,942,5000000,10},
	{13,"§Þnh Quèc ¤ Sa Ph¸t Qu¸n"		,159,783,788,7000000,10},
	{14,"An Bang Tinh Th¹ch H¹ng Liªn"	,163,747,752,10000000,4},
	{15,"An Bang Cóc Hoa Th¹ch ChØ Hoµn",164,753,758,7000000,10},
	{16,"An Bang §iÒn Hoµng Th¹ch Ngäc Béi",165,759,764,5000000,15},
	{17,"An Bang Kª HuyÕt Th¹ch Giíi ChØ"	,166,765,770,7000000,10},
}


function main(NpcIndex)
	
	 dofile("script/global/npcchucnang/thoren.lua")
	
	SayEx({"<color=yellow><npc><color>: <sex> cÇn g× cã ng©n l­îng ta ®©y s½n lßng",
	"Kh¶m n¹m/OpenTrembleItem",
	"Mua vò khÝ/giaodichvukhicap10",
	"Hñy vËt phÈm/tieuhuytrangbi",
	"T¨ng cÊp Phi Phong/onMantleSystem",
	"N©ng cÊp Hoµng kim Ên/onNangCapHoangKimAn",	-- [EP AN 01/09] 
	"Trang bÞ dung luyÖn/onSmeltSystem",	-- [DUNGLUYEN 01/09] ban Linux
	-- "N©ng cÊp b¹ch kim/openupgradeplatina",
	-- "N©ng cÊp Trang Søc /nangcapptrangsuc",
	-- "N©ng cÊp Phi Phong/nangcapphiphong",
	-- "N©ng cÊp Ngò Hµnh Ên /nangcappan",
	-- "N©ng cÊp Ngò Hµnh Ên VIP /nangcappanvip",
	-- "GhÐp Trang BÞ HKMP/ghephkmp",
	-- "GhÐp m¶nh ®æ phæ hkmp/ghepmanhdopho",
	-- "N©ng cÊp An Bang Hoµn Mü/nangcapanbang",
	-- "GhÐp Trang BÞ Hoµng Kim/hopthanhtrangbi",
	-- "TÈy LuyÖn Trang BÞ Hoµng Kim/openresetgolditem",
	-- "Kh«i phôc trang bÞ háng/khoiphuctrangbihong",
	-- "Kh¶m hµo quang Vò KhÝ/KhamHaoQuangVK",
--	"Kh¶m n¹m trang bÞ TÝm/OpenCompoundItem",
	NOW_END_SAY})
end
-- [EP AN 01/09] Nang cap Hoang Kim An theo ban Linux (wuxingyin.lua): 1 An cap N + 1 Vuong Thiet
-- Tuong Lenh Phu -> cap N+1; ti le 35/45/55/65/75%; that bai chi mat Lenh Phu, giu An; tran cap 6.
-- WXY_MoBang() phai goi tu CHINH state da Include wuxingyin.lua (callback GiveItemUI theo m_ActionScriptID).
function onNangCapHoangKimAn()
	if not WXY_MoBang then
		Talk(1, "", "HÖ n©ng cÊp Hoµng kim Ên ch­a ®­îc n¹p, h·y b¸o qu¶n trÞ.");
		return 0;
	end
	WXY_MoBang();
	return 1;
end

function openupgradeplatina()
	local xTD = 20
	local nKvan = 5000000  
szLoiNoi = "ChØ cÇn ®Æt trang bÞ hkmp cÇn n©ng cÊp vµo.\nC¸c nguyªn liÖu kh¸c ®Ó s¼n trong r­¬ng ®å bao gåm:\n- "..(nKvan/10000).." v¹n l­îng.\n".."- "..xTD.." tiÒn ®ång. \n TØ lÖ thµnh c«ng: 30%"
OpenGiveBox("§Æt Trang BÞ Muèn N©ng CÊp Vµo",szLoiNoi,"updateplatina")    
end
function updateplatina()
local tbValidIDs = {
    [15] = 1,
    [20] = 1,
    [25] = 1,
    [1] = 1,
    [5] = 1,
    [10] = 1,
    [75] = 1,
    [70] = 1,
    [80] = 1,
    [60] = 1,
    [65] = 1,
    [30] = 1,
    [38] = 1,
    [45] = 1,
    [50] = 1,
    [93] = 1,
    [95] = 1,
    [100] = 1,
    [110] = 1,
    [115] = 1,
    [120] = 1,
    [125] = 1,
    [130] = 1
}

	local nIndexEquip = 0
	local nIndex = 0
	local nCountEquip = 0
	local i = 0
	local checkUpgrade = 0
	local xTD = 20
	local nKvan = 5000000              
	
	local nTienDong = GetItemCount(0,6,1, 4835, -1, -1, pos_equiproom) -- ®Õm sè KNB trong hµnh trang
	if nTienDong < xTD then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..xTD.." TiÒn §ång.")
		return
	end
	if GetCash() < nKvan then
	Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..(nKvan/10000).." v¹n l­îng.")
		return
	end
	for i=0,5 do
		for j=0,3 do
			nIndex = GetROItem(ROOAFFAIR,i,j)
			if (nIndex > 0) then
				
				nIndexEquip = nIndex
				kind, genre, detail, parti, level, series, row = GetCBItem(nIndexEquip)
				
				local nItemLockState = GetPlayerItemIsLock(nIndexEquip)
				local nItemTimeLimit = GetPlayerItemIsTimeLimit(nIndexEquip)
				if nItemTimeLimit > 0 or nItemLockState == -2 then
					Talk(1,"","Trang bÞ cã thêi gian sö dông hoÆc bÞ khãa kh«ng thÓ n©ng cÊp ")
					EndGiveBox()
					return
				end
				if not tbValidIDs[row] then
					Talk(1,"","Trang bÞ n©ng cÊp kh«ng hîp lÖ !")
					EndGiveBox()
					return
				end
		
				nCountEquip = nCountEquip + 1			
		end
	end 
end		
				if nCountEquip == 0 then
					Msg2Player("Xin ®Æt trang bÞ hkmp cÇn n©ng cÊp vµo.")
					return
				elseif nCountEquip > 1 then
					Msg2Player("ChØ cÇn ®Æt vµo 1 trang bÞ hkmp cÇn n©ng cÊp.")
					return
				end
					local ntilerandom = random(1,9)
					if level > 4 and level <= 7 then
					ntilerandom = random(2,15)
					elseif level > 7 and level < 10 then
					ntilerandom = random(3,17)
					elseif level == 10 then
					ntilerandom = random(2,6)
					end
				if ntilerandom < 4 then
					local result = UpgradePlatinaItem(nIndexEquip)
					if result and type(result) == "number" and result > 0 then
					EndGiveBox()
					checkUpgrade = 1
					Pay(nKvan)
					DelItem(0, -1, 6,1 ,4835, -1, -1, ROOME, xTD) -- xo¸ sè knb trong hµnh trang
					local s_name = GetItemName(nIndexEquip)
					Msg2SubWorld("<color=yellow>Chóc mõng<color><color=green> " .. GetName() .. " <color><color=yellow>n©ng cÊp thµnh c«ng, nhËn ®­îc <color> "..s_name.." .")
					-- Msg2SubWorld("<color=yellow>Chóc mõng<color><color=green> " .. GetName() .. " <color><color=yellow>n©ng cÊp thµnh c«ng, nhËn ®­îc <color> B¹ch Kim.")
					end
				else
				Pay(nKvan)
				DelItem(0, -1, 6,1 ,4835, -1, -1, ROOME, xTD) -- xo¸ sè knb trong hµnh trang
				EndGiveBox()
				Msg2Player("Xin chia buån<color=green> " .. GetName() .. " <color>®· thÊt b¹i khi n©ng cÊp trang bÞ <color=yellow>B¹ch Kim .")
			end
				-- Msg2Player("nKindfk:"..kind)
				-- Msg2Player("nGenfk:"..genre)
				-- Msg2Player("nDetailfk:"..detail)
				-- Msg2Player("nParticur:"..parti)
				-- Msg2Player("nEQLevel:"..level)
				-- Msg2Player("nSerialfk:"..series)
				-- Msg2Player("row:"..row)
				-- Msg2Player("ntilethanhcong:"..ntilerandom)
				
	
end
function ghepmanhdopho()
		local nKvan = 500000              
		local nTienDong = 2   
		local nSoDSK = 100
szLoiNoi = "ChØ cÇn ®Æt BÝ KiÕp 90 vµo.\nC¸c nguyªn liÖu kh¸c ®Ó s¼n trong r­¬ng ®å bao gåm:\n- "..(nKvan/10000).." v¹n l­îng.\n".."- "..nTienDong.." tiÒn ®ång.\n".."- "..nSoDSK.." §iÓm sù kiÖn.\n".."- 1 Tö Lôc Lam Thñy Tinh.\n".."- 6 THBT. \n TØ lÖ thµnh c«ng: 100%"
OpenGiveBox("§Æt BÝ KiÕp 90 Vµo",szLoiNoi,"ghepmanhdophohkmp")

end
function ghepmanhdophohkmp()
	local i, j, nIndex, kind, genre, detail, parti, level, series, row;
	local nKvan = 50000              
	local nTienDong = 2               
	local TIENDONG_ID = 4835     
	local nDSK = GetTask(TASK_DSK)	    
	local nSoDSK = 100	 
	local nRealIndex = 0
	local nRealIndexX = 0
	local nBKID = 0
	local count = 0;
	local ntilecong = GetTask(TASK_FANCUNG_6) ;
	for i = 0, 5 do
		for j = 0, 3 do
			nIndex = GetROItem(ROOAFFAIR, i, j)
			if nIndex > 0 then
				nRealIndex = nIndex
				kind, genre, detail, parti, level, series, row = GetCBItem(nRealIndex)
				if kind == 0 and genre == 6 and detail == 1 and (parti >= 27 and  parti <= 59) then -- do pho hkmp
					if parti >= 27 and parti <= 59 then
						if (parti >= 28 and parti <= 32) or parti == 44 or parti == 49 or parti == 52 or parti == 53 or parti == 59   then
							Talk(1, "", "BÝ kiÕp nµy kh«ng hîp lÖ.")
							return
						end
						count = count + 1
						nBKID = parti
					end
				end
			end
		end
	end
	local naddtungphai = 0;
	if nBKID == 37 then -- kim thiªn v­¬ng
		naddtungphai = 253  --- chuy
	elseif nBKID == 38 then
		naddtungphai = 258 --- thuong 
	elseif nBKID == 39 then
		naddtungphai = 263 --- dao
	elseif nBKID == 56 then  -- kim thiÕu l©m
		naddtungphai = 239  -- quyen
	elseif nBKID == 57 then 
		naddtungphai = 243  -- con
	elseif nBKID == 58 then 
		naddtungphai = 248 -- dao
	elseif nBKID == 27 then  -- méc ®­êng m«n
		naddtungphai = 313  -- bao vu
	elseif nBKID == 45 then
		naddtungphai = 308  -- dao
	elseif nBKID == 46 then
		naddtungphai = 318  -- tieu
	elseif nBKID == 48 then  -- méc ngñ ®éc
		naddtungphai = 298  -- dao
	elseif nBKID == 47 then
		naddtungphai = 303  -- chuong
	elseif nBKID == 42 then  -- thñy nga my
		naddtungphai = 268  -- kiem
	elseif nBKID == 43 then
		naddtungphai = 276  -- chuong
	elseif nBKID == 40 then  -- thñy thóy yªn
		naddtungphai = 283  -- dao
	elseif nBKID == 41 then
		naddtungphai = 288  -- chuong
	elseif nBKID == 54 then  -- háa c¸i bang
		naddtungphai = 331  -- chuong
	elseif nBKID == 55 then
		naddtungphai = 333  -- bong
	elseif nBKID == 35 then  -- háa thiªn nhÉn
		naddtungphai = 338 -- thuong
	elseif nBKID == 36 then
		naddtungphai = 348 -- chuong
	elseif nBKID == 33 then  -- thæ vâ ®ang
		naddtungphai = 353  -- quyen
	elseif nBKID == 34 then
		naddtungphai = 358	-- kiem
	elseif nBKID == 50 then  -- thæ c«n l«n
		naddtungphai = 363  -- dao 
	elseif nBKID == 51 then
		naddtungphai = 368  -- kiem
	end

	if count == 0 then
		Msg2Player("Xin ®Æt bÝ kiÕp 90 vµo.")
		return
	elseif count > 1 then
		Msg2Player("ChØ cÇn ®Æt vµo 1 bÝ kiÕp 90 .")
		return
	end


	local nTD  = GetItemCount(0,6,1, TIENDONG_ID, -1, -1, pos_equiproom)
	local nTTT  = GetItemCount(0,4,238,0, -1, -1,pos_equiproom)
	local nLTT  = GetItemCount(0,4,239,0, -1, -1,pos_equiproom)
	local nLLT  = GetItemCount(0,4,240,0, -1, -1,pos_equiproom)
	local nTHBT = GetItemCount(0,4,353,0, -1, -1,pos_equiproom)
	if nTTT < 1 then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ 1 Tö Thñy Tinh.")
		return
	end
	if nLTT < 1 then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ 1 Lôc Thñy Tinh.")
		return
	end
	if nLLT < 1 then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ 1 Lam Thñy Tinh.")
		return
	end
	if nTHBT < 6 then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ 1 Tinh Hång B¶o Th¹ch.")
		return
	end
	if nDSK < nSoDSK then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nSoDSK.." §iÓm Sù KiÖn.")
		return
	end
	if nTD < nTienDong then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nTienDong.." TiÒn §ång.")
		return
	end

	if GetCash() >= nKvan then
				RemoveItem(nRealIndex, 1)
				DelItem(0, -1, 6,1 ,TIENDONG_ID, -1, -1, pos_equiproom, nTienDong)
				DelItem(0,-1,4,238,0, -1, -1,pos_equiproom,1)
				DelItem(0,-1,4,239,0, -1, -1,pos_equiproom,1)
				DelItem(0,-1,4,240,0, -1, -1,pos_equiproom,1)
				DelItem(0,-1,4,353,0, -1, -1,pos_equiproom,6)
				Pay(nKvan)
				local nIndexX = AddItemSL(naddtungphai,1,0)--27 --313
				local s_name = GetItemName(nIndexX)
				EndGiveBox()
				SetTask(TASK_DSK, GetTask(TASK_DSK) - nSoDSK)
				Msg2Player("<color=yellow>Chóc mõng<color><color=green> " .. GetName() .. " <color><color=yellow>ghÐp thµnh c«ng!")
				Msg2SubWorld("<color=yellow>Chóc mõng<color><color=green> " .. GetName() .. " <color><color=yellow>ghÐp thµnh c«ng, nhËn ®­îc <color> "..s_name.." .")
			else
			Msg2Player("Kh¸ch quan ch­a mang ®ñ "..(nKvan/10000).." v¹n l­îng.")
		end
end


function ghephkmp()
		local nKvan = 5000000              
		local nTienDong = 50   
		local nSoDSK = 200
szLoiNoi = "ChØ cÇn ®Æt §å Phæ Hoµng Kim cÇn n©ng cÊp vµo.\nC¸c nguyªn liÖu kh¸c ®Ó s¼n trong r­¬ng ®å bao gåm:\n- "..(nKvan/10000).." v¹n l­îng.\n".."- "..nTienDong.." tiÒn ®ång.\n".."- "..nSoDSK.." §iÓm sù kiÖn.\n".."- 1 Tö Lôc Lam Thñy Tinh.\n".."- 6 THBT. \n TØ lÖ thµnh c«ng: 20% \n ThÊt b¹i sÏ mÊt hÕt !"
OpenGiveBox("§Æt §å Phæ Muèn N©ng CÊp Vµo",szLoiNoi,"batdaunangcaphkmp")

end
function batdaunangcaphkmp()
	local i, j, nIndex, kind, genre, detail, parti, level, series, row;
	local nKvan = 5000000              
	local nTienDong = 50               
	local TIENDONG_ID = 4835     
	local nDSK = GetTask(TASK_DSK)	    
	local nSoDSK = 200	 
	local nRealIndex = 0
	local nRealIndexX = 0
	local nhkmpID = 0
	local count = 0;
	local ntilecong = GetTask(TASK_FANCUNG_5) ;
	for i = 0, 5 do
		for j = 0, 3 do
			nIndex = GetROItem(ROOAFFAIR, i, j)
			if nIndex > 0 then
				nRealIndex = nIndex
				kind, genre, detail, parti, level, series, row = GetCBItem(nRealIndex)
				if kind == 0 and genre == 6 and detail == 1 and (parti >= 238 and  parti <= 377) then -- do pho hkmp
					if parti >= 238 and parti <= 377 then
						if GetPlayerItemIsLock(nRealIndex) == -2 then
							Talk(1, "", "Trang bÞ ®ang ®­îc kho¸ b¶o hiÓm, kh«ng thÓ n©ng cÊp.")
							return
						end
						if GetPlayerItemIsTimeLimit(nRealIndex) > 0 then
							Talk(1, "", "Trang bÞ cã thêi gian sö dông, kh«ng thÓ n©ng cÊp.")
							return
						end
						count = count + 1
						nhkmpID = parti
					end
				end
			end
		end
	end
						-- Msg2Player("nKindfk:"..kind)
						-- Msg2Player("nGenfk:"..genre)
						-- Msg2Player("nDetailfk:"..detail)
						-- Msg2Player("nParticur:"..parti)
						-- Msg2Player("nEQLevel:"..level)
						-- Msg2Player("nSerialfk:"..series)
						-- Msg2Player("row:"..row)

	if count == 0 then
		Msg2Player("Xin ®Æt ®å phæ hkmp vµo.")
		return
	elseif count > 1 then
		Msg2Player("ChØ cÇn ®Æt vµo 1 ®å phæ hkmp .")
		return
	end


	local nTD  = GetItemCount(0,6,1, TIENDONG_ID, -1, -1, pos_equiproom)
	local nTTT  = GetItemCount(0,4,238,0, -1, -1,pos_equiproom)
	local nLTT  = GetItemCount(0,4,239,0, -1, -1,pos_equiproom)
	local nLLT  = GetItemCount(0,4,240,0, -1, -1,pos_equiproom)
	local nTHBT = GetItemCount(0,4,353,0, -1, -1,pos_equiproom)
	if nTTT < 1 then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ 1 Tö Thñy Tinh.")
		return
	end
	if nLTT < 1 then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ 1 Lôc Thñy Tinh.")
		return
	end
	if nLLT < 1 then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ 1 Lam Thñy Tinh.")
		return
	end
	if nTHBT < 6 then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ 1 Tinh Hång B¶o Th¹ch.")
		return
	end
	if nDSK < nSoDSK then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nSoDSK.." §iÓm Sù KiÖn.")
		return
	end
	if nTD < nTienDong then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nTienDong.." TiÒn §ång.")
		return
	end

	if GetCash() >= nKvan then
		
		local nRand = random(1, 7)
		local nSuccess =  nRand + ntilecong
		if nSuccess >= 30 then
			RemoveItem(nRealIndex, 1)
			DelItem(0, -1, 6,1 ,TIENDONG_ID, -1, -1, pos_equiproom, nTienDong)
			DelItem(0,-1,4,238,0, -1, -1,pos_equiproom,1)
			DelItem(0,-1,4,239,0, -1, -1,pos_equiproom,1)
			DelItem(0,-1,4,240,0, -1, -1,pos_equiproom,1)
			DelItem(0,-1,4,353,0, -1, -1,pos_equiproom,6)
			Pay(nKvan)
			local nIndexX = AddItem2(2, 0, nhkmpID - 238, 0, 0, 0)
			local s_name = GetItemName(nIndexX)
			EndGiveBox()
			SetTask(TASK_DSK, GetTask(TASK_DSK) - nSoDSK)
			SetTask(TASK_FANCUNG_5, 0)
			Msg2Player("<color=yellow>Chóc mõng<color><color=green> " .. GetName() .. " <color><color=yellow>ghÐp thµnh c«ng!")
			Msg2SubWorld("<color=yellow>Chóc mõng<color><color=green> " .. GetName() .. " <color><color=yellow>ghÐp thµnh c«ng, nhËn ®­îc <color> "..s_name.." .")
		else
			Msg2Player("Xin chia buån ghÐp trang bÞ hkmp thÊt b¹i !")
			RemoveItem(nRealIndex, 1)
			DelItem(0, -1, 6,1 ,TIENDONG_ID, -1, -1, pos_equiproom, nTienDong)
			DelItem(0,-1,4,238,0, -1, -1,pos_equiproom,1)
			DelItem(0,-1,4,239,0, -1, -1,pos_equiproom,1)
			DelItem(0,-1,4,240,0, -1, -1,pos_equiproom,1)
			DelItem(0,-1,4,353,0, -1, -1,pos_equiproom,6)
			Pay(nKvan)
			EndGiveBox()
			SetTask(TASK_DSK, GetTask(TASK_DSK) - nSoDSK)
			SetTask(TASK_FANCUNG_5, GetTask(TASK_FANCUNG_5) + 1)
		end
		-- Msg2Player("ntilecong:"..ntilecong)
		-- Msg2Player("tong:"..nSuccess)
	else
		Msg2Player("Kh¸ch quan ch­a mang ®ñ "..(nKvan/10000).." v¹n l­îng.")
	end
end
function nangcapanbang()
		local nKvan = 1000000              
		local nTienDong = 30   
		local nSoDSK = 100
szLoiNoi = "ChØ cÇn ®Æt An Bang Hoµn Mü cÇn n©ng cÊp vµo.\nC¸c nguyªn liÖu kh¸c ®Ó s¼n trong r­¬ng ®å bao gåm:\n- "..(nKvan/10000).." v¹n l­îng.\n".."- "..nTienDong.." tiÒn ®ång.\n".."- "..nSoDSK.." §iÓm sù kiÖn. \n TØ lÖ thµnh c«ng: 40%"
OpenGiveBox("§Æt Ngùa Muèn N©ng CÊp Vµo",szLoiNoi,"batdaunangcapab")

end
function batdaunangcapab()
	local i, j, nIndex, kind, genre, detail, parti, level, series, row;
	local nKvan = 1000000              
	local nTienDong = 30               
	local TIENDONG_ID = 4835     
	local nDSK = GetTask(TASK_DSK)	    
	local nSoDSK = 100	 
	local nRealIndex = 0
	local nRealIndexX = 0
	local nPhiPhongID = 0
	local count = 0;
	local ntilecong = GetTask(TASK_FANCUNG_6) ;
	for i = 0, 5 do
		for j = 0, 3 do
			nIndex = GetROItem(ROOAFFAIR, i, j)
			if nIndex > 0 then
				nRealIndex = nIndex
				kind, genre, detail, parti, level, series, row = GetCBItem(nRealIndex)
				if kind == 3 and genre == 0 and (detail == 4 or detail == 3 or detail == 9) and row >= 163 and  row <= 166 then -- Phi phong
					if row >= 163 and row <= 166 then
						if GetPlayerItemIsLock(nRealIndex) == -2 then
							Talk(1, "", "Phi phong ®ang ®­îc kho¸ b¶o hiÓm, kh«ng thÓ n©ng cÊp.")
							return
						end
						if GetPlayerItemIsTimeLimit(nRealIndex) > 0 then
							Talk(1, "", "Phi phong cã thêi gian sö dông, kh«ng thÓ n©ng cÊp.")
							return
						end
						count = count + 1
						nPhiPhongID = row
					end
				end
			end
		end
	end
						-- Msg2Player("nKindfk:"..kind)
						-- Msg2Player("nGenfk:"..genre)
						-- Msg2Player("nDetailfk:"..detail)
						-- Msg2Player("nParticur:"..parti)
						-- Msg2Player("nEQLevel:"..level)
						-- Msg2Player("nSerialfk:"..series)
						-- Msg2Player("row:"..row)

	if count == 0 then
		Msg2Player("Xin ®Æt An Bang cÇn n©ng cÊp vµo.")
		return
	elseif count > 1 then
		Msg2Player("ChØ cÇn ®Æt vµo 1 trang bÞ An Bang cÇn n©ng cÊp.")
		return
	end


	local nTD  = GetItemCount(0,6,1, TIENDONG_ID, -1, -1, pos_equiproom)
	if nDSK < nSoDSK then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nSoDSK.." §iÓm Sù KiÖn.")
		return
	end
	if nTD < nTienDong then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nTienDong.." TiÒn §ång.")
		return
	end

	if GetCash() >= nKvan then
		
		local nRand = random(1, 7)
		local nSuccess =  nRand + ntilecong
		if nSuccess >= 12 then
			RemoveItem(nRealIndex, 1)
			DelItem(0, -1, 6,1 ,TIENDONG_ID, -1, -1, pos_equiproom, nTienDong)
			Pay(nKvan)
			local nIndexX = AddItem2(2, 0, nPhiPhongID + 260, 0, 0, 0)
			local s_name = GetItemName(nIndexX)
			EndGiveBox()
			SetTask(TASK_DSK, GetTask(TASK_DSK) - nSoDSK)
			SetTask(TASK_FANCUNG_6, 0)
			Msg2Player("Chóc b¹n n©ng cÊp An Bang Hoµn Mü thµnh c«ng!")
			Msg2SubWorld("<color=yellow>Chóc mõng<color><color=green> " .. GetName() .. " <color><color=yellow>n©ng cÊp Trang BÞ thµnh c«ng, nhËn ®­îc <color> "..s_name.." .")
		else
			Msg2Player("Xin chia buån n©ng cÊp An Bang thÊt b¹i !")
			DelItem(0, -1, 6,1 ,TIENDONG_ID, -1, -1, pos_equiproom, nTienDong)
			Pay(nKvan)
			EndGiveBox()
			SetTask(TASK_DSK, GetTask(TASK_DSK) - nSoDSK)
			SetTask(TASK_FANCUNG_6, GetTask(TASK_FANCUNG_6) + 1)
		end
		-- Msg2Player("ntilecong:"..ntilecong)
		-- Msg2Player("tong:"..nSuccess)
	else
		Msg2Player("Kh¸ch quan ch­a mang ®ñ "..(nKvan/10000).." v¹n l­îng.")
	end
end

function nangcappan()
		local nKvan = 1000000              
		local nTienDong = 15             
		local nSL1 = 50
		local nSL2 = 50
szLoiNoi = "ChØ cÇn ®Æt Ngò Hµnh Ên cÇn n©ng cÊp vµo.\nC¸c nguyªn liÖu kh¸c ®Ó s¼n trong r­¬ng ®å bao gåm:\n - "..(nKvan/10000).." v¹n l­îng.\n".."- "..nTienDong.." tiÒn ®ång. \n TØ lÖ thµnh c«ng: 30%"
OpenGiveBox("§Æt Ngò Hµnh Ên Muèn N©ng CÊp Vµo",szLoiNoi,"batdaunangcapan")

end
function batdaunangcapan()
	local i, j, nIndex, kind, genre, detail, parti, level, series, row;
	local nKvan = 1000000              
	local nTienDong = 15              
	local TIENDONG_ID = 4835         
	local nRealIndex = 0
	local nRealIndexX = 0
	local nAnID = 0
	local count = 0;

	for i = 0, 5 do
		for j = 0, 3 do
			nIndex = GetROItem(ROOAFFAIR, i, j)
			if nIndex > 0 then
				nRealIndex = nIndex
				kind, genre, detail, parti, level, series, row = GetCBItem(nRealIndex)
				if kind == 3 and genre == 0 and detail == 13 and level == 10 then -- Phi phong
					if row >= 3224 and row < 3233 then
						-- if GetPlayerItemIsLock(nRealIndex) == -2 then
							-- Talk(1, "", "Phi phong ®ang ®­îc kho¸ b¶o hiÓm, kh«ng thÓ n©ng cÊp.")
							-- return
						-- end
						if GetPlayerItemIsTimeLimit(nRealIndex) > 0 then
							Talk(1, "", "Ngò Hµnh Ên cã thêi gian sö dông, kh«ng thÓ n©ng cÊp.")
							return
						end
						count = count + 1
						nAnID = row
					end
				end
			end
		end
	end
						-- Msg2Player("nKindfk:"..kind)
						-- Msg2Player("nGenfk:"..genre)
						-- Msg2Player("nDetailfk:"..detail)
						-- Msg2Player("nParticur:"..parti)
						-- Msg2Player("nEQLevel:"..level)
						-- Msg2Player("nSerialfk:"..series)
						-- Msg2Player("row:"..row)

	if count == 0 then
		Msg2Player("Xin ®Æt Ngò Hµnh Ên cÇn n©ng cÊp vµo.")
		return
	elseif count > 1 then
		Msg2Player("ChØ cÇn ®Æt vµo 1 Ngò Hµnh Ên cÇn n©ng cÊp.")
		return
	end


	local nTD  = GetItemCount(0,6,1, TIENDONG_ID, -1, -1, pos_equiproom)

	if nTD < nTienDong then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nTienDong.." TiÒn §ång.")
		return
	end

	if GetCash() >= nKvan then
	local nSuccess =  random(1,5)
		if row >= 3228 then
				nSuccess = random(1, 5)
			else
				nSuccess = random(1, 3)
		end
		-- Msg2Player("ti le :"..nSuccess)
		if nSuccess == 3 then
			RemoveItem(nRealIndex, 1)
			DelItem(0, -1, 6,1 ,TIENDONG_ID, -1, -1, pos_equiproom, nTienDong)
			Pay(nKvan)
			local nIndexX = AddItem2(2, 0, nAnID + 1, 0, 0, 0)
			local s_name = GetItemName(nIndexX)
			SetPlayerItemLock(nIndexX, -2)
			EndGiveBox()

			Msg2Player("Chóc b¹n n©ng cÊp Ngò Hµnh Ên thµnh c«ng!")
			Msg2SubWorld("<color=yellow>Chóc mõng<color><color=green> " .. GetName() .. " <color><color=yellow>n©ng cÊp Ngò Hµnh Ên thµnh c«ng, nhËn ®­îc <color> "..s_name.." .")
		else
			Msg2Player("Xin chia buån n©ng cÊp Ngò Hµnh Ên thÊt b¹i !")
			DelItem(0, -1, 6,1 ,TIENDONG_ID, -1, -1, pos_equiproom, nTienDong)
			Pay(nKvan)
			EndGiveBox()
		end
	else
		Msg2Player("Kh¸ch quan ch­a mang ®ñ "..(nKvan/10000).." v¹n l­îng.")
	end
end

function nangcapptrangsuc()
		local nKvan = 10000000              
		local nTienDong = 50      
		local nNguHanh = 100  		

szLoiNoi = "ChØ cÇn ®Æt Trang Søc cÇn n©ng cÊp vµo.\nC¸c nguyªn liÖu kh¸c ®Ó s¼n trong r­¬ng ®å bao gåm:\n - "..(nKvan/10000).." v¹n l­îng.\n".."- "..nTienDong.." tiÒn ®ång.\n".."- "..nNguHanh.." Ngò Hµnh Kú Th¹ch. \n TØ lÖ thµnh c«ng: 80%"
OpenGiveBox("§Æt Trang Søc Muèn N©ng CÊp Vµo",szLoiNoi,"batdaunangcaptrangsuc")

end
function batdaunangcaptrangsuc()
	local i, j, nIndex, kind, genre, detail, parti, level, series, row;
	local nKvan = 10000000              
	local nTienDong = 50              
	local nNguHanh = 100              
	local TIENDONG_ID = 4835         
	local NGUHANHKYTHACH_ID = 2134         
	local nRealIndex = 0
	local nRealIndexX = 0
	local nAnID = 0
	local count = 0;

	for i = 0, 5 do
		for j = 0, 3 do
			nIndex = GetROItem(ROOAFFAIR, i, j)
			if nIndex > 0 then
				nRealIndex = nIndex
				kind, genre, detail, parti, level, series, row = GetCBItem(nRealIndex)
				if kind == 3 and genre == 0 and detail == 14 and level == 10 then -- Phi phong
					if row >= 7377 and row < 7379 or  row == 3549 then
						-- if GetPlayerItemIsLock(nRealIndex) == -2 then
							-- Talk(1, "", "Phi phong ®ang ®­îc kho¸ b¶o hiÓm, kh«ng thÓ n©ng cÊp.")
							-- return
						-- end
						if GetPlayerItemIsTimeLimit(nRealIndex) > 0 then
							Talk(1, "", "Trang Søc cã thêi gian sö dông, kh«ng thÓ n©ng cÊp.")
							return
						end
						count = count + 1
						nAnID = row
					end
				end
			end
		end
	end
						-- Msg2Player("nKindfk:"..kind)
						-- Msg2Player("nGenfk:"..genre)
						-- Msg2Player("nDetailfk:"..detail)
						-- Msg2Player("nParticur:"..parti)
						-- Msg2Player("nEQLevel:"..level)
						-- Msg2Player("nSerialfk:"..series)
						-- Msg2Player("row:"..row)

	if count == 0 then
		Msg2Player("Xin ®Æt Trang Søc cÇn n©ng cÊp vµo.")
		return
	elseif count > 1 then
		Msg2Player("ChØ cÇn ®Æt vµo 1 Trang Søc cÇn n©ng cÊp.")
		return
	end


	local nTD  = GetItemCount(0,6,1, TIENDONG_ID, -1, -1, pos_equiproom)
	local nNHKT  = GetItemCount(0,6,1, NGUHANHKYTHACH_ID, -1, -1, pos_equiproom)
	if nTD < nTienDong then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nTienDong.." TiÒn §ång.")
		return
	end
	if nNHKT < nNguHanh then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nNguHanh.." Ngò Hµnh Kú Th¹ch.")
		return
	end

	if GetCash() >= nKvan then
	local nSuccess =  random(1,2)
		if row >= 7377 then
				nSuccess = random(1, 3)
			else
				nSuccess = random(1, 2)
		end
		if GetTask(TASK_NEWTHOREN1) == 2 then
			nSuccess =  1
		end
		-- Msg2Player("ti le :"..nSuccess)
		if nSuccess == 1 then
			RemoveItem(nRealIndex, 1)
			DelItem(0, -1, 6,1 ,TIENDONG_ID, -1, -1, pos_equiproom, nTienDong)
			DelItem(0, -1, 6,1 ,NGUHANHKYTHACH_ID, -1, -1, pos_equiproom, nNguHanh)
			Pay(nKvan)
			if row == 3549 then
				nIndexXc = AddItem2(2, 0, 7377, 0, 0, 0)
				SetTask(TASK_NEWTHOREN1,0)
				SetPlayerItemLock(nIndexXc, -2)
				s_name = GetItemName(nIndexXc)
			end
			if row >= 7377 and row < 7379 then
				nIndexXz = AddItem2(2, 0,nAnID + 1, 0, 0, 0)
				SetPlayerItemLock(nIndexXz, -2)
				SetTask(TASK_NEWTHOREN1,0)
				s_name = GetItemName(nIndexXz)
			end
			EndGiveBox()

			Msg2Player("Chóc b¹n n©ng cÊp Trang Søc thµnh c«ng!")
			Msg2SubWorld("<color=yellow>Chóc mõng<color><color=green> " .. GetName() .. " <color>n©ng cÊp Trang Søc thµnh c«ng, nhËn ®­îc <color=yellow> "..s_name.." .")
			
				thoigian = tonumber(date("%H%M%d%m"))
				LoginLog = openfile("dulieu/CheTaoTrangSuc.txt", "a");
				if LoginLog then
				write(LoginLog,"Thanh Cong: ID: ["..s_name.."] - Name: ["..GetName().."] - Acc: ["..GetAccount().."] - Time: ["..thoigian.."] \n");
				end
				closefile(LoginLog)
				
			

		else
			
			Msg2Player("Xin chia buån n©ng cÊp Trang Søc thÊt b¹i !")
			DelItem(0, -1, 6,1 ,TIENDONG_ID, -1, -1, pos_equiproom, nTienDong)
			DelItem(0, -1, 6,1 ,NGUHANHKYTHACH_ID, -1, -1, pos_equiproom, nNguHanh)
			Pay(nKvan)
			EndGiveBox()
			
				thoigian = tonumber(date("%H%M%d%m"))
				LoginLog = openfile("dulieu/CheTaoTrangSuc.txt", "a");
				if LoginLog then
				write(LoginLog,"That Bai: ID: Name: ["..GetName().."] - Acc: ["..GetAccount().."] - Time: ["..thoigian.."] \n");
				end
				closefile(LoginLog)
		end
	else
		Msg2Player("Kh¸ch quan ch­a mang ®ñ "..(nKvan/10000).." v¹n l­îng.")
	end
end


function nangcappanvip()
		local nKvan = 10000000              
		local nTienDong = 300      
		local nNguHanh = 100  		

szLoiNoi = "ChØ cÇn ®Æt Ngò Hµnh Ên cÇn n©ng cÊp vµo.\nC¸c nguyªn liÖu kh¸c ®Ó s¼n trong r­¬ng ®å bao gåm:\n - "..(nKvan/10000).." v¹n l­îng.\n".."- "..nTienDong.." tiÒn ®ång.\n".."- "..nNguHanh.." Ngò Hµnh Kú Th¹ch. \n TØ lÖ thµnh c«ng: 50%"
OpenGiveBox("§Æt Ngò Hµnh Ên Muèn N©ng CÊp Vµo",szLoiNoi,"batdaunangcapanvip")

end
function batdaunangcapanvip()
	local i, j, nIndex, kind, genre, detail, parti, level, series, row;
	local nKvan = 10000000              
	local nTienDong = 300              
	local nNguHanh = 100              
	local TIENDONG_ID = 4835         
	local NGUHANHKYTHACH_ID = 2134         
	local nRealIndex = 0
	local nRealIndexX = 0
	local nAnID = 0
	local count = 0;

	for i = 0, 5 do
		for j = 0, 3 do
			nIndex = GetROItem(ROOAFFAIR, i, j)
			if nIndex > 0 then
				nRealIndex = nIndex
				kind, genre, detail, parti, level, series, row = GetCBItem(nRealIndex)
				if kind == 3 and genre == 0 and detail == 13 and level == 10 then -- Phi phong
					if row >= 5929 and row < 5931 or  row == 3233 then
						-- if GetPlayerItemIsLock(nRealIndex) == -2 then
							-- Talk(1, "", "Phi phong ®ang ®­îc kho¸ b¶o hiÓm, kh«ng thÓ n©ng cÊp.")
							-- return
						-- end
						if GetPlayerItemIsTimeLimit(nRealIndex) > 0 then
							Talk(1, "", "Ngò Hµnh Ên cã thêi gian sö dông, kh«ng thÓ n©ng cÊp.")
							return
						end
						count = count + 1
						nAnID = row
					end
				end
			end
		end
	end
						-- Msg2Player("nKindfk:"..kind)
						-- Msg2Player("nGenfk:"..genre)
						-- Msg2Player("nDetailfk:"..detail)
						-- Msg2Player("nParticur:"..parti)
						-- Msg2Player("nEQLevel:"..level)
						-- Msg2Player("nSerialfk:"..series)
						-- Msg2Player("row:"..row)

	if count == 0 then
		Msg2Player("Xin ®Æt Ngò Hµnh Ên cÇn n©ng cÊp vµo.")
		return
	elseif count > 1 then
		Msg2Player("ChØ cÇn ®Æt vµo 1 Ngò Hµnh Ên cÇn n©ng cÊp.")
		return
	end


	local nTD  = GetItemCount(0,6,1, TIENDONG_ID, -1, -1, pos_equiproom)
	local nNHKT  = GetItemCount(0,6,1, NGUHANHKYTHACH_ID, -1, -1, pos_equiproom)
	if nTD < nTienDong then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nTienDong.." TiÒn §ång.")
		return
	end
	if nNHKT < nNguHanh then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nNguHanh.." Ngò Hµnh Kú Th¹ch.")
		return
	end

	if GetCash() >= nKvan then
	local nSuccess =  random(1,2)
		if GetTask(TASK_NEWTHOREN1) == 1 then
			nSuccess =  1
		end
		if nSuccess == 1 then
			RemoveItem(nRealIndex, 1)
			DelItem(0, -1, 6,1 ,TIENDONG_ID, -1, -1, pos_equiproom, nTienDong)
			DelItem(0, -1, 6,1 ,NGUHANHKYTHACH_ID, -1, -1, pos_equiproom, nNguHanh)
			Pay(nKvan)
			if row == 3233 then
				nIndexXc = AddItem2(2, 0, 5929, 0, 0, 0)
				SetTask(TASK_NEWTHOREN1,0)
				SetPlayerItemLock(nIndexXc, -2)
				s_name = GetItemName(nIndexXc)
			end
			if row >= 5929 then
				nIndexXz = AddItem2(2, 0,nAnID + 1, 0, 0, 0)
				SetPlayerItemLock(nIndexXz, -2)
				SetTask(TASK_NEWTHOREN1,0)
				s_name = GetItemName(nIndexXz)
			end
			EndGiveBox()

			Msg2Player("Chóc b¹n n©ng cÊp Ngò Hµnh Ên thµnh c«ng!")
			Msg2SubWorld("<color=yellow>Chóc mõng<color><color=green> " .. GetName() .. " <color>n©ng cÊp Ngò Hµnh Ên thµnh c«ng, nhËn ®­îc <color=yellow> "..s_name.." .")

		else
			Msg2Player("Xin chia buån n©ng cÊp Ngò Hµnh Ên thÊt b¹i !")
			DelItem(0, -1, 6,1 ,TIENDONG_ID, -1, -1, pos_equiproom, nTienDong)
			DelItem(0, -1, 6,1 ,NGUHANHKYTHACH_ID, -1, -1, pos_equiproom, nNguHanh)
			Pay(nKvan)
			EndGiveBox()
		end
	else
		Msg2Player("Kh¸ch quan ch­a mang ®ñ "..(nKvan/10000).." v¹n l­îng.")
	end
end


function nangcapphiphong()
		local nKvan = 1000000              
		local nTienDong = 10             
		local nSL1 = 50
		local nSL2 = 50
szLoiNoi = "ChØ cÇn ®Æt Phi Phong cÇn n©ng cÊp vµo.\nC¸c nguyªn liÖu kh¸c ®Ó s¼n trong r­¬ng ®å bao gåm:\n - "..(nKvan/10000).." v¹n l­îng.\n".."- "..nTienDong.." tiÒn ®ång. \n TØ lÖ thµnh c«ng: 50%"
OpenGiveBox("§Æt Phi Phong Muèn N©ng CÊp Vµo",szLoiNoi,"batdaunangcapphiphong")

end
function batdaunangcapphiphong()
	local i, j, nIndex, kind, genre, detail, parti, level, series, row;
	local nKvan = 1000000              
	local nTienDong = 10               
	local TIENDONG_ID = 4835         
	local nRealIndex = 0
	local nRealIndexX = 0
	local nPhiPhongID = 0
	local count = 0;

	for i = 0, 5 do
		for j = 0, 3 do
			nIndex = GetROItem(ROOAFFAIR, i, j)
			if nIndex > 0 then
				nRealIndex = nIndex
				kind, genre, detail, parti, level, series, row = GetCBItem(nRealIndex)
				if kind == 3 and genre == 0 and detail == 12 and level >= 0 and  level <= 19 then -- Phi phong
					if row >= 7357 and row < 7376 then
						-- if GetPlayerItemIsLock(nRealIndex) == -2 then
							-- Talk(1, "", "Phi phong ®ang ®­îc kho¸ b¶o hiÓm, kh«ng thÓ n©ng cÊp.")
							-- return
						-- end
						if GetPlayerItemIsTimeLimit(nRealIndex) > 0 then
							Talk(1, "", "Phi phong cã thêi gian sö dông, kh«ng thÓ n©ng cÊp.")
							return
						end
						count = count + 1
						nPhiPhongID = row
					end
				end
			end
		end
	end
						-- Msg2Player("nKindfk:"..kind)
						-- Msg2Player("nGenfk:"..genre)
						-- Msg2Player("nDetailfk:"..detail)
						-- Msg2Player("nParticur:"..parti)
						-- Msg2Player("nEQLevel:"..level)
						-- Msg2Player("nSerialfk:"..series)
						-- Msg2Player("row:"..row)

	if count == 0 then
		Msg2Player("Xin ®Æt Phi Phong cÇn n©ng cÊp vµo.")
		return
	elseif count > 1 then
		Msg2Player("ChØ cÇn ®Æt vµo 1 Phi Phong cÇn n©ng cÊp.")
		return
	end


	local nTD  = GetItemCount(0,6,1, TIENDONG_ID, -1, -1, pos_equiproom)

	if nTD < nTienDong then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nTienDong.." TiÒn §ång.")
		return
	end

	if GetCash() >= nKvan then
	local nSuccess =  random(1, 3)
		if level >= 9 then
				nSuccess = random(1, 7)
			else
				nSuccess = random(1, 2)
		end
		-- Msg2Player("ti le :"..nSuccess)
		if nSuccess == 2 then
			RemoveItem(nRealIndex, 1)
			DelItem(0, -1, 6,1 ,TIENDONG_ID, -1, -1, pos_equiproom, nTienDong)
			Pay(nKvan)
			local nIndexX = AddItem2(2, 0, nPhiPhongID + 1, 0, 0, 0)
			local s_name = GetItemName(nIndexX)
			SetPlayerItemLock(nIndexX, -2)
			EndGiveBox()

			Msg2Player("Chóc b¹n n©ng cÊp Phi Phong thµnh c«ng!")
			Msg2SubWorld("<color=yellow>Chóc mõng<color><color=green> " .. GetName() .. " <color><color=yellow>n©ng cÊp Phi Phong thµnh c«ng, nhËn ®­îc <color> "..s_name.." .")
			
				thoigian = tonumber(date("%H%M%d%m"))
				LoginLog = openfile("dulieu/CheTaoFF.txt", "a");
				if LoginLog then
				write(LoginLog,"Thanh Cong: ID: ["..s_name.."] - Name: ["..GetName().."] - Acc: ["..GetAccount().."] - Time: ["..thoigian.."] \n");
				end
				closefile(LoginLog)
				
		else
			Msg2Player("Xin chia buån n©ng cÊp Phi Phong thÊt b¹i !")
			DelItem(0, -1, 6,1 ,TIENDONG_ID, -1, -1, pos_equiproom, nTienDong)
			Pay(nKvan)
			EndGiveBox()
			
				thoigian = tonumber(date("%H%M%d%m"))
				LoginLog = openfile("dulieu/CheTaoFF.txt", "a");
				if LoginLog then
				write(LoginLog,"That Bai: ID: Name: ["..GetName().."] - Acc: ["..GetAccount().."] - Time: ["..thoigian.."] \n");
				end
				closefile(LoginLog)
		end
	else
		Msg2Player("Kh¸ch quan ch­a mang ®ñ "..(nKvan/10000).." v¹n l­îng.")
	end
end
function openresetgolditem()
	OpenGiveBox("§Æt vµo vËt phÈm","- H·y ®¨t trang bÞ hoµng kim muèn tÈy luyÖn vµo !\n- TÈy luyÖn tèn 10 TiÒn §ång.","resetgoldequip")
end

function resetgoldequip()
	local nIndexEquip = 0
	local nIndex = 0
	local nCountEquip = 0
	local i = 0
	local checkUpgrade = 0
	local xTD = 10
	local nSeries = 0;
	local nNLChinh = 10
	local nTienDong = GetItemCount(0,6,1, 4835, -1, -1, pos_equiproom) -- ®Õm sè KNB trong hµnh trang
	if nTienDong < xTD then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..xTD.." TiÒn §ång.")
		return
	end
	nSeries = GetSeries();
	-- local nResDel = 
	for i=0,5 do
			for j=0,3 do
				nIndex = GetROItem(ROOAFFAIR,i,j)
				if (nIndex > 0) then
					nIndexEquip = nIndex
					kind, genre, detail, parti, level, series, row = GetCBItem(nIndexEquip)
					if kind == 3 and genre == 0 and detail == 13 and level == 10 then -- An
						if row >= 7381 and row <= 7385 then
							if (nSeries == 0) then
								local n5SLCK  = GetItemCount(0,6,1, 1816, -1, -1, pos_equiproom)
								if n5SLCK < nNLChinh then
									Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nNLChinh.." Ngò s¾c long ch©u Kim.")
									return
								end
							elseif (nSeries == 1) then
								local n5SLCM  = GetItemCount(0,6,1, 1817, -1, -1, pos_equiproom)
								if n5SLCM < nNLChinh then
									Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nNLChinh.." Ngò s¾c long ch©u Méc.")
									return
								end
							elseif (nSeries == 2) then
								local n5SLCT  = GetItemCount(0,6,1, 1818, -1, -1, pos_equiproom)
								if n5SLCT < nNLChinh then
									Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nNLChinh.." Ngò s¾c long ch©u Thñy.")
									return
								end
							elseif (nSeries == 3) then
								local n5SLCH  = GetItemCount(0,6,1, 1819, -1, -1, pos_equiproom)
								if n5SLCH < nNLChinh then
									Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nNLChinh.." Ngò s¾c long ch©u Háa.")
									return
								end
							elseif (nSeries == 4) then
								local n5SLCT  = GetItemCount(0,6,1, 1820, -1, -1, pos_equiproom)
								if n5SLCT < nNLChinh then
									Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nNLChinh.." Ngò s¾c long ch©u Thæ.")
									return
								end
							end
						end
					end
		
					local nItemLockState = GetPlayerItemIsLock(nIndexEquip)
					local nItemTimeLimit = GetPlayerItemIsTimeLimit(nIndexEquip)
					if nItemTimeLimit > 0  then
					Talk(1,"","Trang bÞ cã thêi gian sö dông kh«ng thÓ tÈy luyÖn")
					return
					end
					nCountEquip = nCountEquip + 1
			end
		end
	end
	if nCountEquip == 1 then
		local result = ResetGoldEquipItem(nIndexEquip)
		if result and type(result) == "number" and result > 0 then
			checkUpgrade = 1
				if nItemLockState == -2 then
					SetPlayerItemLock(result, -2)
				end
				if (nSeries == 0) then
					DelItem(0, -1, 6,1 ,1816, -1, -1, pos_equiproom, nNLChinh)
				elseif (nSeries == 1) then
					DelItem(0, -1, 6,1 ,1817, -1, -1, pos_equiproom, nNLChinh)
				elseif (nSeries == 2) then
					DelItem(0, -1, 6,1 ,1818, -1, -1, pos_equiproom, nNLChinh)
				elseif (nSeries == 3) then
					DelItem(0, -1, 6,1 ,1819, -1, -1, pos_equiproom, nNLChinh)
				elseif (nSeries == 4) then
					DelItem(0, -1, 6,1 ,1820, -1, -1, pos_equiproom, nNLChinh)
				end
				DelItem(0, -1, 6,1 ,4835, -1, -1, ROOME, xTD) -- xo¸ sè knb trong hµnh trang
				else
					Talk(1, "", "VËt phÈm nµy kh«ng hîp lÖ ®Ó tÈy luyÖn.")
				end
				EndGiveBox()
		else
        Talk(1, "", "ChØ cÇn ®Æt vµo 1 trang bi vao.")
    end
		if(checkUpgrade == 1) then
			Talk(1,"","§· tÈy luyÖn thµnh c«ng ! ")
		else
			Talk(1,"","H·y bá ®óng vËt phÈm quy ®Þnh !")
		end
end
function giaodichvukhicap10()
	--Sale(7, 0)  ---
	-- Sale(69, 0)  ---
	Sale(31, 0)  --- vk 8x 
end

function KhamHaoQuangVK()
	OpenGiveBox("§Æt vµo trang bÞ","Kh¶m ¸nh s¸ng vò khÝ cËn chiÕn cÊp 10","KhamHaoQuangVKB1")
end

function khoiphuctrangbihong()
	OpenGiveBox("§Æt vµo trang bÞ","Kh«i phôc trang bÞ háng, h·y ®Æt nã vµo trong","khoiphuctrangbi")
end

VKPHATSANG = {
	[0] = { -- 
		[1] = 112, -- ®ao mµu tr¾ng s¸ng
		-- [5] = 86, -- ®ao 15x ®Ñp
		[0] = 132, -- kiÕm mµu tr¾ng s¸ng
		--[5] = 109, -- kiÕm xanh ®Ñp	
		[3] = 38, -- kÝch mµu tr¾ng s¸ng
		-- [3] = 39, -- kÝch mµu löa nung
		-- [2] = 40, -- bæng mµu vµng s¸ng hoµng kim
		[2] = 111, -- bæng mµu löa nung
		--[5] = 85, -- bæng 15x ®Ñp
		[4] = 113, -- chuú ph¸t s¸ng tr¾ng
		-- [4] = 87, -- chuú 15x ®Ñp
		[5] = 29, -- song ®ao mµu tÝm ®Ñp
	},
	[1] = 
	{ --
		[2] = 195, -- no
		[0] = 250, -- tieu195
		[1] = 210, -- phi dao
	}
}

function KhamHaoQuangVKB1()
	local nIndexEquip = 0
	local nIndex = 0
	local nCountEquip = 0
	local i = 0
	local checkRemove = 0
	for i=0,5 do
		for j=0,3 do
			nIndex = GetROItem(ROOAFFAIR,i,j)
			if (nIndex > 0) then
				nCountEquip = nCountEquip + 1
				nIndexEquip = nIndex
				checkRemove = checkRemove + 1
			end
		end
	end

	if(checkRemove > 1) then
		Talk(1,"","Mçi lÇn chØ kh¶m mét trang bÞ.")
		return
	end
	
	local soxucan = 200
	local xuhienco = GetTask(T_PLAYER_XU)
	if(xuhienco < soxucan) then	
		Talk(1,"","CÇn cã "..soxucan.." tiÒn xu trong hµnh trang")
		return
	end
	
	local nKindfk, nGenfk, nDetailfk, nParticur, nEQLevel, nSerialfk, nLinefk = GetCBItem(nIndexEquip)
	-- Msg2Player("nKindfk:"..nKindfk)
	-- Msg2Player("nGenfk:"..nGenfk)
	-- Msg2Player("nDetailfk:"..nDetailfk)
	-- Msg2Player("nParticur:"..nParticur)
	-- Msg2Player("nEQLevel:"..nEQLevel)
	-- Msg2Player("nSerialfk:"..nSerialfk)
	-- Msg2Player("nLinefk:"..nLinefk)
	
	if(nEQLevel < 10 or nEQLevel > 10) then
		Talk(1,"","ChØ cã thÓ thay ®æi vò khÝ cËn chiÕn cÊp 10")
		return
	end
	
	if(nParticur < 0 or nParticur > 5) then
		Talk(1,"","ChØ cã thÓ thay ®æi vò khÝ cËn chiÕn cÊp 10")
		return		
	end
	
	-- if(nDetailfk > 0) then
		-- Talk(1,"","ChØ cã thÓ thay ®æi trang bÞ xanh vò khÝ cËn chiÕn cÊp 10")
		-- return
	-- end
	local nResID = VKPHATSANG[nDetailfk] and VKPHATSANG[nDetailfk][nParticur]
	if(SetTask(T_PLAYER_XU, xuhienco - soxucan) == 1) then
		SetGlowLightItem(nIndexEquip,nResID)
		if(checkRemove == 1) then
			EndGiveBox()
			Talk(1,"","Kh¶m xong.")
			logHoatDong(format("[%s]\t%s\tAccount:%s\tName:%s\tKham anh sang :%d", " vu khi", GetLocalDate("%Y-%m-%d %X"), GetAccount(), GetName(), soxucan ))
		else
			Talk(1,"","§¹i hiÖp ch­a bá trang bÞ vµo.")
		end
	else
		Talk(1,"","CÇn cã "..soxucan.." tiÒn xu trong hµnh trang")
		return
	end	
end
FREECELL_KHOIPHUC = 20
function khoiphuctrangbi()
	local nIndexEquip = 0
	local nIndex = 0
	local nCountEquip = 0
	local i = 0
	local checkRemove = 0
	for i=0,5 do
		for j=0,3 do
			nIndex = GetROItem(ROOAFFAIR,i,j)
			if (nIndex > 0) then
				nCountEquip = nCountEquip + 1
				nIndexEquip = nIndex
				checkRemove = checkRemove + 1
			end
		end
	end
	if CalcFreeItemCellCount() < FREECELL_KHOIPHUC then
		Talk(1,"","Kho¶ng trèng hµnh trang kh«ng ®ñ "..FREECELL_KHOIPHUC.." « . §Ò nghÞ dän dÑp hµnh trang tr­íc!")
		return
	end	
	if(checkRemove > 1) then
		Talk(1,"","Mçi lÇn chØ kh«i phôc mét trang bÞ.")
		return
	end

	local soxucan = 10
	local soxucancur = RepairItemGetNumCoin(nIndexEquip)
	if(soxucancur) then
		soxucan = soxucancur
	end

	local xuhienco = GetTask(T_PLAYER_XU)
	--if(GetItemCount(0,4,ID_XU_TIENDONG,0) < soxucan) then
	if(xuhienco < soxucan) then	
		Talk(1,"","CÇn cã "..soxucan.." tiÒn xu trong hµnh trang")
		return
	end
	
	--if (ConsumeItem(soxucan,0,4,ID_XU_TIENDONG,0) == soxucan) then
	if(SetTask(T_PLAYER_XU, xuhienco - soxucan) == 1) then
		RepairItemByCoin(nIndexEquip)
		if(checkRemove == 1) then
			EndGiveBox()
			Talk(1,"","Kh«i phôc xong.")
			logHoatDong(format("[%s]\t%s\tAccount:%s\tName:%s\tKhoi phuc :%d", " trang bi", GetLocalDate("%Y-%m-%d %X"), GetAccount(), GetName(), soxucan ))
		else
			Talk(1,"","§¹i hiÖp ch­a bá trang bÞ vµo.")
		end
	else
		Talk(1,"","CÇn cã "..soxucan.." tiÒn xu trong hµnh trang")
		return
	end
end

function giaodich()
	local TAB_GIAODICH = 
	{
		"Mua KiÕm",
		"Mua §ao",
		"Mua Bæng",
		"Mua Th­¬ng",
		"Mua Chïy",
		"Mua Song ®ao",
		"Mua Phi tiªu",
		"Mua Phi ®ao",
		"Mua Tô tiÔn",
	}
	local tb = {};
	local count = 9
	for i = 1,count do
		tinsert(tb,""..TAB_GIAODICH[i].."/giaodichvp")	
	end	
	tinsert(tb,NOW_END_SAY)		
	Say(10090,count+1,tb)
end

function giaodichvp(nSel)
	local nRow = nSel+1
	local nShop = nRow+12
	Sale(nShop)
end

function tieuhuytrangbi()
	OpenGiveBox("§Æt vµo vËt phÈm","Tiªu hñy vËt phÈm, h·y ®Æt vËt phÈm cÇn tiªu hñy vµo trong","huybotrangbi")
end

function huybotrangbi()
local tbValidIDs = {}
tbValidIDs[5634] = 1


	local nIndexEquip = 0
	local nIndex = 0
	local nCountEquip = 0
	local i = 0
	local checkRemove = 0

	for i=0,5 do
		for j=0,3 do
			nIndex = GetROItem(ROOAFFAIR,i,j)
			if (nIndex > 0) then
				nIndexEquip = nIndex
				kind, genre, detail, parti, level, series, row = GetCBItem(nIndexEquip)
				if kind == 3 and tbValidIDs[row] then
		
				SetTask(TASK_NEWTHOREN1,1)
				
				Talk(1,"","B¹n ®· huy NhËm HiÖp Ên Gi¸m thµnh c«ng tÆng tØ lÖ thµnh c«ng 100% cho lÇn n©ng cÊp T©n NhËm Hµnh Ên Gi¸m .")
				Msg2Player("B¹n ®· huy NhËm HiÖp Ên Gi¸m thµnh c«ng tÆng tØ lÖ thµnh c«ng 100% cho lÇn n©ng cÊp T©n NhËm Hµnh Ên Gi¸m.")
				end
				nCountEquip = nCountEquip + 1
				-- Msg2Player("nKindfk:"..kind)
				-- Msg2Player("nGenfk:"..genre)
				-- Msg2Player("nDetailfk:"..detail)
				-- Msg2Player("nParticur:"..parti)
				-- Msg2Player("nEQLevel:"..level)
				-- Msg2Player("nSerialfk:"..series)
				-- Msg2Player("row:"..row)
				RemoveItem(nIndexEquip, MAX_STACK_NUM)
				EndGiveBox()
				logHoatDong(format("[%s]\t%s\tAccount:%s\tName:%s\tTieu huy :%d", " trang bi", GetLocalDate("%Y-%m-%d %X"), GetAccount(), GetName(), nIndexEquip))
				checkRemove = 1
				
			end
		end
	end
					
	if(checkRemove == 1) then		
		Talk(1,"","Tiªu hñy xong.")
	else
		Talk(1,"","§¹i hiÖp ch­a bá vËt phÈm vµo.")
	end
end

function hopthanhtrangbi()
	local tb = {}
	local count = 8
	for i = 1,count do
		tinsert(tb,""..ITEMHOPTHANH[i][2].."/gohopthanh("..i..")")
	end	
	tinsert(tb,PAGE_2)			
	tinsert(tb,NOW_END_SAY)		
	Say(LUACHON,count+2,tb)
end

function hopthanhtrangbi2()
	local tb = {}
	local count = 8
	for i = 9,count+9 do
		tinsert(tb,""..ITEMHOPTHANH[i][2].."/gohopthanh("..i..")")	
	end			
	tinsert(tb,NOW_END_SAY)		
	Say(LUACHON,count+1,tb)
end

function gohopthanh(nSel)
	SetTaskTemp(TMP_INDEX_NPC,nSel);
	if GetCash() < ITEMHOPTHANH[nSel][6] then
		Talk(1,"",format("Ng­¬i kh«ng ®ñ %d l­îng.",ITEMHOPTHANH[nSel][6]))
		return
	end
	OpenGiveBox(""..ITEMHOPTHANH[nSel][2].."",format(DIEUKIENHOPTHANH,ITEMHOPTHANH[nSel][2],ITEMHOPTHANH[nSel][6]),"common_hopthanh");	
end

function common_hopthanh()
	local nParam = GetTaskTemp(TMP_INDEX_NPC)
	local nEnough = 0
	local nIndex = 0
	local nKIndex = 0
	local nCountEquip = 0
	local bBlock,nTime = 0,0
	local bBLockItem = 0
	local nLuck,nTBKT,nTTT,nLTT,nLLT,nTHBT = 0,0,0,0
	local i = 0
	for i=0,5 do
		for j=0,3 do
			nIndex = GetROItem(ROOAFFAIR,i,j)
			if (nIndex > 0) then
				nCountEquip = nCountEquip + 1
				nKIndex = nIndex
				if (GetPlayerItemIsLock(nKIndex) == -2) then
					Talk(1,"","VËt phÈm kho¸ kh«ng thÓ sö dông 1 !")
					return
				end
				--bBlock,nTime = GetBindItem(2,nIndex)
				--if bBlock > 0 then bBLockItem = bBLockItem + 1 end		
			end
		end
	end

	if bBLockItem > 0 then
		bBLockItem = 2;
	end
	-- nKIndex = GetROItem(ROOAFFAIR,1)
--	if (nCountEquip < 6) then
	--	Talk(1,"",NOTCHINHXAC)		
	--return 
	--end
	
	for i = ITEMHOPTHANH[nParam][4],ITEMHOPTHANH[nParam][5] do
		if GetItemCount(0, 4, i, 0, -1, -1, pos_affairitem) >= 1 then
			nEnough = nEnough + 1
		end		
	end
	for i = ITEMHOPTHANH[nParam][4],ITEMHOPTHANH[nParam][5] do
		if GetItemCount(0, 4, i, 0, -1, -1, pos_affairitem) < 1 then
			Talk(1,"",NOTCHINHXAC)
		return end		
	end
	--if (nEnough ~= 6) then	
	--Talk(1,"",NOTCHINHXAC)
	--return end
	
	nLuck = ITEMHOPTHANH[nParam][7];
	
	nTTT  = GetItemCount(0,4,238,0, -1, -1,pos_affairitem)
	nLTT  = GetItemCount(0,4,239,0, -1, -1,pos_affairitem)
	nLLT  = GetItemCount(0,4,240,0, -1, -1,pos_affairitem)
	nTHBT = GetItemCount(0,4,353,0, -1, -1,pos_affairitem)
	
	
	
	--local nNLTHBT = GetItemCount(0,4,353,0, -1, -1,pos_affairitem)
	
	
	-- if (GetPlayerItemIsLock(nNLTHBT) == -2) then
		-- Talk(1,"","VËt phÈm kho¸ kh«ng thÓ sö dông 2 !")
		-- return
	-- end
	
	
	if (nTTT > 0) then
		nLuck =	nLuck + nTTT*1
		DelItem(0,-1,4,238,0, -1, -1,pos_affairitem,nTTT)
	end	
	
	if (nLTT > 0) then
		nLuck =	nLuck + nLTT*1
		DelItem(0,-1,4,239,0, -1, -1,pos_affairitem,nLTT)
	end	
	
	if (nLLT > 0) then
		nLuck =	nLuck + nLLT*1
		DelItem(0,-1,4,240,0, -1, -1,pos_affairitem,nLLT)
	end	
	
	if (nTHBT > 0) then
		nLuck =	nLuck + nTHBT* 1
		DelItem(0,-1,4,353,0, -1, -1,pos_affairitem,nTHBT)
	end	
	if nLuck < 0 then
		nLuck = 0
	end

	
	sx = random(35,70)
	nTile = sx + nLuck
	--Msg2Player(format("TØ lÖ hiÖn t¹i<color=gold>%s <color>.",nTile))
	if nTile > 70 then 
	for i = ITEMHOPTHANH[nParam][4],ITEMHOPTHANH[nParam][5] do
		DelItem(0, -1, 4, i, 0, -1, -1, pos_affairitem, 1)
	end		
	AddItem2(2,0,ITEMHOPTHANH[nParam][3],0,0,0)
	--AddGoldItem(ITEMHOPTHANH[nParam][3],0); --dinhquoc
	--RemoveItem(nIndex, MAX_STACK_NUM)
	EndGiveBox()
	Pay(ITEMHOPTHANH[nParam][6])
	Msg2Player(format("Hîp thµnh thµnh c«ng trang bÞ <color=gold>%s <color>.",ITEMHOPTHANH[nParam][2]))
				thoigian = tonumber(date("%H%M%d%m"))
				LoginLog = openfile("dulieu/CheTaoHoangKim.txt", "a");
				if LoginLog then
				write(LoginLog,"Thanh Cong: ID: ["..ITEMHOPTHANH[nParam][2].."] - Xac Suat: ["..nTile.."] - Name: ["..GetName().."] - Acc: ["..GetAccount().."] - Time: ["..thoigian.."] \n");
				end
				closefile(LoginLog)
	else
	nXoaItem = random(ITEMHOPTHANH[nParam][4],ITEMHOPTHANH[nParam][5])
		DelItem(0, -1, 4, nXoaItem, 0, -1, -1, pos_affairitem, 1)
	--end	
	Pay(ITEMHOPTHANH[nParam][6])	
	EndGiveBox()
	Msg2Player(format("Xin chia buån hîp thÊt b¹i trang bÞ <color=gold>%s <color>.",ITEMHOPTHANH[nParam][2]))
				thoigian = tonumber(date("%H%M%d%m"))
				LoginLog = openfile("dulieu/CheTaoHoangKim.txt", "a");
				if LoginLog then
				write(LoginLog,"That Bai: ID: ["..ITEMHOPTHANH[nParam][2].."] - Xac Suat: ["..nTile.."] - Name: ["..GetName().."] - Acc: ["..GetAccount().."] - Time: ["..thoigian.."] \n");
				end
				closefile(LoginLog)
	end	
end

function no()
	
end

function TrembleItem()
	local pos_tremble = 3
	local nIndexEquip = GetIdItem(pos_tremble, 0)
	local kind,a,b,c,d,e
	local granmm = random(1,4) -- tØ lÖ thµnh c«ng 10%
	local onokay = 0 -- thÊt b¹i mÊt nguyªn liÖu
	if(granmm == 3) then
		onokay = 1 --thµnh c«ng, 
	end
	--Msg2Player(nIndexEquip)
	if(nIndexEquip > 0) then
		kind,a,b,c,d,e = GetCBItem(nIndexEquip)
		--Msg2Player(format("%d %d %d %d %d %d", kind,a,b,c,d,e))
		--local Rmatype1,RmatypeV1,Rmatype2,RmatypeV2,Rmatype3,RmatypeV3,Rmatype4,RmatypeV4,Rmatype5,RmatypeV5,Rmatype6,RmatypeV6 = GetOTItem(nIndexEquip)
		--Msg2Player(format("%d %d %d %d %d %d %d %d %d %d %d %d", Rmatype1,RmatypeV1,Rmatype2,RmatypeV2,Rmatype3,RmatypeV3,Rmatype4,RmatypeV4,Rmatype5,RmatypeV5,Rmatype6,RmatypeV6))
		local nIndexTuTT = GetIdItem(pos_tremble, 1)
		local nIndexLucTT= GetIdItem(pos_tremble, 2)
		local nIndexLamTT3 = GetIdItem(pos_tremble, 3)
		local nIndexLamTT4 = GetIdItem(pos_tremble, 4)
		local nIndexLamTT5 = GetIdItem(pos_tremble, 5)
		local nIndexLamTT6 = GetIdItem(pos_tremble, 6)
		local nIndexLamTT7 = GetIdItem(pos_tremble, 7)
		
		--Msg2Player(format("%d %d %d %d %d %d %d", nIndexTuTT,nIndexLucTT,nIndexLamTT3,nIndexLamTT4,nIndexLamTT5,nIndexLamTT6, nIndexLamTT7))
		
		if(nIndexTuTT > 0 and nIndexLucTT == 0 and nIndexLamTT3 == 0 and nIndexLamTT4 == 0
		and nIndexLamTT5 == 0 and nIndexLamTT6 == 0 and nIndexLamTT7 == 0) then
			khamnamlevel(onokay,nIndexEquip, nIndexTuTT, d)
		
		elseif(nIndexLucTT > 0 and nIndexTuTT ==0 and nIndexLamTT3 == 0 and nIndexLamTT4 == 0
		and nIndexLamTT5 == 0 and nIndexLamTT6 == 0 and nIndexLamTT7 == 0) then
			khamnamoption(onokay,nIndexEquip, nIndexLucTT, d)
		
		elseif(nIndexLucTT == 0 and nIndexTuTT == 0 and (nIndexLamTT3 > 0 or nIndexLamTT4 > 0
		or nIndexLamTT5 > 0 or nIndexLamTT6 > 0 or nIndexLamTT7 > 0)) then
			khamnamserial(onokay,nIndexEquip, nIndexLamTT3, nIndexLamTT4, nIndexLamTT5, nIndexLamTT6, nIndexLamTT7)
			
		else
			Talk(1,"","Mçi lÇn chØ ®Æt vµo trong 1 viªn thuû tinh. ")
		end
		
	else
		Talk(1,"","Kh«ng t×m thÊy vËt phÈm kh¶m. ")
	end
end

function khamnamlevel(onokay,nIndexEquip, nIndexTuTT, d)
	if (GetPlayerItemIsLock(nIndexTuTT) == -2) then
		Talk(1,"","VËt phÈm kho¸ kh«ng thÓ sö dông !")
		return
	end
	local type1 = 1
	local nextlevel = d + 1
	RemoveItem(nIndexTuTT, 1)
	if(onokay == 1) then
		ChangeInfoItem(type1,nIndexEquip, nextlevel)
		Msg2Player(format("Kh¶m cÊp trang bÞ. "))
	else
		Msg2Player(format("Kh¶m thÊt b¹i thö l¹i lÇn n÷a. "))
	end
	EndTrembleItem()
end


function khamnamoptionOpen(onokay,nIndexEquip, nIndexLucTT, d)
	if (GetPlayerItemIsLock(nIndexLucTT) == -2) then
		Talk(1,"","VËt phÈm kho¸ kh«ng thÓ sö dông !")
		return
	end
	local type1 = 3
	local nextlevel = d
	RemoveItem(nIndexLucTT, 1)		
	if(onokay == 1) then
		ChangeInfoItem(type1,nIndexEquip, nextlevel)
		Msg2Player(format("Kh¶m option trang bÞ. "))
	else
		Msg2Player(format("Kh¶m thÊt b¹i thö l¹i lÇn n÷a. "))
	end	
	EndTrembleItem()
end

function khamnamoption(onokay,nIndexEquip, nIndexLucTT, d)
	Talk(1,"","TÝnh n¨ng nµy t¹m kho¸ !")
	EndTrembleItem()
end

function khamnamserialOpen(onokay,nIndexEquip, nIndexLamTT3, nIndexLamTT4, nIndexLamTT5, nIndexLamTT6, nIndexLamTT7)
	if (GetPlayerItemIsLock(nIndexLamTT3) == -2) then
		Talk(1,"","VËt phÈm kho¸ kh«ng thÓ sö dông !")
		return
	end
	if (GetPlayerItemIsLock(nIndexLamTT4) == -2) then
		Talk(1,"","VËt phÈm kho¸ kh«ng thÓ sö dông !")
		return
	end
	if (GetPlayerItemIsLock(nIndexLamTT5) == -2) then
		Talk(1,"","VËt phÈm kho¸ kh«ng thÓ sö dông !")
		return
	end
	if (GetPlayerItemIsLock(nIndexLamTT6) == -2) then
		Talk(1,"","VËt phÈm kho¸ kh«ng thÓ sö dông !")
		return
	end
	if (GetPlayerItemIsLock(nIndexLamTT7) == -2) then
		Talk(1,"","VËt phÈm kho¸ kh«ng thÓ sö dông !")
		return
	end
	local changeserial = -1
	if(nIndexLamTT3 > 0) then
		changeserial = 0
		RemoveItem(nIndexLamTT3, 1)		
	elseif(nIndexLamTT4 > 0) then
		changeserial = 1
		RemoveItem(nIndexLamTT4, 1)		
	elseif(nIndexLamTT5 > 0) then
		changeserial = 2
		RemoveItem(nIndexLamTT5, 1)		
	elseif(nIndexLamTT6 > 0) then
		changeserial = 3
		RemoveItem(nIndexLamTT6, 1)		
	elseif(nIndexLamTT7 > 0) then
		changeserial = 4						
		RemoveItem(nIndexLamTT7, 1)		
	end
	if(changeserial >= 0) then
		local type1 = 2
		if(onokay == 1) then
			ChangeInfoItem(type1,nIndexEquip, changeserial)
			Msg2Player(format("Kh¶m ngò hµnh trang bÞ. "))
		else
			Msg2Player(format("Kh¶m thÊt b¹i thö l¹i lÇn n÷a. "))
		end			
	end
	EndTrembleItem()	
end

function khamnamserial(onokay,nIndexEquip, nIndexLamTT3, nIndexLamTT4, nIndexLamTT5, nIndexLamTT6, nIndexLamTT7)
	Talk(1,"","TÝnh n¨ng nµy t¹m kho¸ !")
	EndTrembleItem()	
end

-- [DUNGLUYEN 01/09] Menu "Trang bi dung luyen" theo global_tiejiang.lua (Linux). Khong port muc "Nop trang bi da
-- dung luyen" (thuong 20 trieu exp lan dau, task 3998) vi task 3998 dang duoc JX1 dung cho viec khac.
function onSmeltSystem()
	Say("Sau khi dung luyÖn c¸c lo¹i trang bÞ, cã thÓ lµm cho trang bÞ m¹nh h¬n.", 5,
		{"Dung luyÖn V¨n C­¬ng/onSmelt",
		 "N©ng cÊp V¨n Tinh/onUpgradeWenJing",
		 "Ph©n gi¶i V¨n Tinh/onDegradeWenJing",
		 "H­íng dÉn dung luyÖn/onSmeltHelp0",
		 "KÕt thóc ®èi tho¹i/no"})
end

function onSmeltHelp0()
	Say("<color=green>Giíi thiÖu hÖ thèng dung luyÖn: <color>Th«ng qua dung luyÖn cã thÓ gia t¨ng thuéc tÝnh trang bÞ nh­: s¸t th­¬ng, thêi gian cho¸ng, triÖt tiªu s¸t th­¬ng... <color=red>L­u ý: <color>thuéc tÝnh c«ng kÝch chØ ph¸t huy khi kü n¨ng nh©n vËt cã s½n hiÖu øng ®ã; thuéc tÝnh phßng ngù lu«n cã hiÖu qu¶.", 2,
		{"Trang kÕ/onSmeltHelp1", "KÕt thóc ®èi tho¹i/no"})
end

function onSmeltHelp1()
	Say("<color=green>V¨n C­¬ng dung luyÖn: <color>T¹i Thî RÌn chän Trang bÞ dung luyÖn - Dung luyÖn V¨n C­¬ng, ®Æt trang bÞ Hoµng Kim/B¹ch Kim vµ 1 V¨n C­¬ng vµo box. Tiªu hao V¨n C­¬ng vµ 100 v¹n l­îng. Mçi trang bÞ cã sè l­îng vµ phÈm chÊt V¨n C­¬ng tèi ®a kh¸c nhau.", 3,
		{"Trang tr­íc/onSmeltHelp0", "Trang kÕ/onSmeltHelp2", "KÕt thóc ®èi tho¹i/no"})
end

function onSmeltHelp2()
	Say("<color=green>V¨n C­¬ng thÝ luyÖn: <color>Trong box dung luyÖn chän thÎ V¨n C­¬ng ThÝ LuyÖn, ®Æt trang bÞ ®· dung luyÖn vµ 1 HuyÒn Háa Than. Tiªu hao HuyÒn Háa Than vµ 100 v¹n l­îng; toµn bé thuéc tÝnh dung luyÖn mÊt t¸c dông vµ thu l¹i tÊt c¶ V¨n C­¬ng.", 3,
		{"Trang tr­íc/onSmeltHelp1", "Trang kÕ/onSmeltHelp3", "KÕt thóc ®èi tho¹i/no"})
end

function onSmeltHelp3()
	Say("<color=green>N©ng cÊp V¨n C­¬ng: <color>NhÊn chuét ph¶i vµo V¨n Tinh cã cïng thuéc tÝnh vµ phÈm chÊt víi V¨n C­¬ng ®ang cã, sÏ n©ng V¨n C­¬ng lªn 1 cÊp phÈm chÊt. V¨n C­¬ng + V¨n Tinh cïng thuéc tÝnh, cïng cÊp = V¨n C­¬ng cao h¬n 1 cÊp.", 3,
		{"Trang tr­íc/onSmeltHelp2", "Trang kÕ/onSmeltHelp4", "KÕt thóc ®èi tho¹i/no"})
end

function onSmeltHelp4()
	Say("<color=green>N©ng cÊp / Ph©n gi¶i V¨n Tinh: <color>T¹i Thî RÌn chän Trang bÞ dung luyÖn - N©ng cÊp V¨n Tinh: 2 V¨n Tinh gièng nhau hîp thµnh 1 V¨n Tinh cao h¬n 1 cÊp (200.000 l­îng/lÇn). Ph©n gi¶i V¨n Tinh: 1 V¨n Tinh t¸ch thµnh 2 V¨n Tinh thÊp h¬n 1 cÊp.", 2,
		{"Trang tr­íc/onSmeltHelp3", "KÕt thóc ®èi tho¹i/no"})
end
