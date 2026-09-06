-- [GOBAY 29/08] Code_New -> Code_GiftNew (2 cho trong ma) - xem ReverseTools/cauhinh/t02_go_bay_codenew.py
--Author: Fong KiÒu
--Date: 07/07/2021
--Function: Npc LÔ Quan 7 thµnh thÞ

Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_server.lua")
Include("script\\global\\vatpham.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\giftcode_fancung.lua")
Include("\\script\\giftcode_new.lua")
Include("\\script\\event\\event_cauhoi\\lib.lua")

function main()
	
	dofile("script/global/npcchucnang/lequan.lua")
	
	CheckPlayerTitle()
	
	local tbOption = {"Chµo <sex> <color=red><player><color> ng­êi cÇn g× tõ LÔ Quan ta ®©y",
	
		"Xãa Danh HiÖu/xoadanhhieu",  -- oke
		-- "NhËn GiftCode Fan Cøng/nhapGiftcodeUI",  -- oke
		"N©ng CÊp BÝ KiÕp 120/ghepbk120",
		"N©ng CÊp §¹i Thµnh BÝ KiÕp 120/ghepdtbk120",
		"GiftCode Ng¹o ThÕ T©n Thñ/nhapGiftcodeUINew",  -- oke	
		"Mua §¹i Thµnh BÝ KiÕp 90 b»ng 300 ®iÓm sù kiÖn /okaymuadaithanh90",
		-- "LÊy l¹i mËt khÈu r­¬ng/DKLayLaiMatKhauRuong",
	}
	

	-- if(GetTask(T_CH_DANHANTHG) == 0 and GetTask(T_RIGHTQUESTION) >= NUM_NHANTHUONG) then
		-- tinsert(tbOption,"NhËn th­ëng ho¹t ®éng Hoa §¨ng /NhanThuongCauHoi")
	-- end
	if(GetAccount() == "") or (GetAccount() == "") or (GetAccount() == "") or (GetAccount() == "") or (GetAccount() == "") then 
	tinsert(tbOption,"NhËn kich TK /kichtaikhoan")
	end
	
	tinsert(tbOption,"Ta chØ ghÐ th¨m «ng th«i./no")
	SayEx(tbOption)
	
end
function okaymuadaithanh90()
	if(GetTask(TASK_DSK) < 300) then
		Talk(1,"","<sex> kh«ng ®ñ ®iÓm 300 tÝch luü mµ muèn lõa ta sao?")
		return
	end
	SetTask(TASK_DSK, GetTask(TASK_DSK) - 300)
	AddItem(6,1,2433,0,0,0,0) --- dai thanh bi kiep
	Msg2Player("Xin chóc mõng ®· nhËn ®­îc 1 §¹i Thµnh BÝ KiÕp 90")
end
function xoadanhhieu()
	SetPlayerHonorId(PlayerIndex,0)
end
function nhapGiftcodeUINew()
	OpenGetString("NhËp Giftcode", "UseGiftcodeNew")
end


function UseGiftcodeNew()
	local szCode = GetStringFromUI()
	if CalcFreeItemCellCount() < 40 then
		Talk(1,"","Kho¶ng trèng hµnh trang kh«ng ®ñ 40 « . §Ò nghÞ dän dÑp hµnh trang tr­íc!")
		return
	end	
	
	if 0 == GiftcodeIsValid("Code_Tuan", szCode) then
		Talk(1, "", "Giftcode kh«ng hîp lÖ hoÆc ®· ®­îc sö dông.")
		return
	end

	if GetTask(TASK_FANCUNG) > 0 then
		Talk(1,"","Nh©n vËt nµy ®· sö dông GiftCode Fan Cøng, kh«ng thÓ sö dông GiftCode nµy")
		return	
	end
	
	if GetTask(TASK_FANCUNG_1) > 0 then
		Talk(1,"","Nh©n vËt nµy ®· sö dông GiftCode Ng¹o ThÕ T©n Thñ, kh«ng thÓ sö dông ®­îc n÷a ")
		return	
	end
	
	if UseGiftcodeS("Code_Tuan", szCode) == 0 then
		Talk(1, "", "Giftcode nµy ®· ®­îc xö dông.")
		return
	end

	
	SetTask(TASK_FANCUNG_1,1)
	AddItemSL(71,5,-2) -- tiªn th¶o lé
	AddItemSL(22,1,-2) -- tÈy tñy kinh
	AddItemSL(4843,1,-2) -- tói t©n thñ
	AddItemSL(4821,1,-2) -- tói m¸u ho¹t ®éng
	AddItemSL(4844,500,-2) -- ho mach do
	AddItemSL(4844,500,-2) -- ho mach do
	AddItemSL(4844,500,-2) -- ho mach do
	AddItemSL(4844,500,-2) -- ho mach do
	AddItemSL(4844,500,-2) -- ho mach do
	AddItemSL(4844,500,-2) -- ho mach do
	AddItemSL(4844,500,-2) -- ho mach do
	AddItemSL(4844,500,-2) -- ho mach do
	AddItemSL(4844,500,-2) -- ho mach do
	AddItemSL(4844,500,-2) -- ho mach do

	AddItemSL(4847,500,-2)  -- chan nguyen don
	AddItemSL(4847,500,-2)  -- chan nguyen don
	AddItemSL(4847,500,-2)  -- chan nguyen don
	AddItemSL(4847,500,-2)  -- chan nguyen don
	AddItemSL(4847,500,-2)  -- chan nguyen don
	AddItemSL(4847,500,-2)  -- chan nguyen don
	AddItemSL(4850,20,0) -- ruong trang bi xanh
	AddSkillState(1558, 50, 1,10886400, -1)
	SetPlayerHonorId(PlayerIndex,41)

	
	-- local nNAME_Change = TaoBangNew(Code_GiftNew,"Code_GiftNew")
	-- SaveDataNew("script/giftcode_new.lua",nNAME_Change)
	Msg2SubWorld("<color=green> Chóc mõng <color><color=yellow>"..GetName().."<color><color=green> ®· nhËn ®­îc GiftCode Ng¹o ThÕ T©n Thñ.<color>")		
	Talk(1, "", "NhËn th­ëng GiftCode Ng¹o ThÕ T©n Thñ thµnh c«ng.")
	
end

function nhapGiftcodeUI()
	OpenGetString("NhËp Giftcode", "UseGiftcode")
end


function UseGiftcode()
	local szCode = GetStringFromUI()
	
	if CalcFreeItemCellCount() < 40 then
		Talk(1,"","Kho¶ng trèng hµnh trang kh«ng ®ñ 40 « . §Ò nghÞ dän dÑp hµnh trang tr­íc!")
		return
		end	
		
	if not Code_FanCung[szCode] then
		Talk(1, "", "Giftcode kh«ng hîp lÖ.")
		return
	end


	if GetTask(TASK_FANCUNG_1) > 0 then
		Talk(1,"","Nh©n vËt nµy ®· sö dông GiftCode Ng¹o ThÕ T©n Thñ, kh«ng thÓ sö dông GiftCode nµy")
		return	
	end
	if Code_FanCung[szCode][2] == 1 then
		Talk(1, "", "Giftcode nµy ®· ®­îc xö dông.")
		return
	end
	if GetTask(TASK_FANCUNG) > 0 then
		Talk(1,"","Nh©n vËt nµy ®· sö dông Gift Code, kh«ng thÓ sö dông thªm lÇn n÷a")
		return	
	end

	
	Code_FanCung[szCode][2] = 1
	SetTask(TASK_FANCUNG,1)
	AddItemSL(71,5,-2) -- tiªn th¶o lé
	AddItemSL(22,1,-2) -- tÈy tñy kinh
	AddSkillState(1559, 50, 1,10886400, -1)
	SetPlayerHonorId(PlayerIndex,53)
	mIndex = AddItem(0,10,5,random(1,4),0,0,10)
	SetPlayerItemLock(mIndex, -2)
	
	local nNAME_Change = TaoBangNew(Code_FanCung,"Code_FanCung")
	SaveDataNew("script/giftcode_fancung.lua",nNAME_Change)
	Msg2SubWorld("<color=blue> Chóc mõng <color> <color=yellow>"..GetName().." <color=blue> ®· nhËn ®­îc<color=white> GiftCode FAN Cøng <color>")		
	Talk(1, "", "NhËn th­ëng giftcode Fan Cøng thµnh c«ng.")
	
end
function hotrocap90GM()
 if (GetLevel() < 80) then
		 for i = GetLevel(), 80-1 do 
		 AddOwnExp(100000000000)
		 end
	 end
	 AddMagic(210, 1)
end
function DKLayLaiMatKhauRuong()
	Talk(1,"","Sau thêi gian 7 ngµy <sex> quay l¹i ®©y ®Ó ®­îc cÊp mËt khÈu míi.")
	-- OpenResetPass()
end
function kichtaikhoan()
    OpenGetString("Tªn Tµi Kho¶n", "CheckTK")
end

function CheckTK()
    local nTaiKhoan = GetStringFromUI()  -- Assuming GetStringFromUI retrieves the input string
	  if nTaiKhoan == "" then
        Talk(1, "", "Tµi kho¶n nµy kh«ng ®­îc ®Ó trèng !")
        return
    end
        KickOutAccount("" .. nTaiKhoan .. "")
		Msg2Player("<color=yellow>da kich thanh cong.")
end
function ghepdtbk120()
		local nKvan = 1000000              
		local nTienDong = 15   
		local nSoDSK = 100
szLoiNoi = "ChØ cÇn ®Æt ®¹i thµnh bÝ kiÕp 90 vµo.\nC¸c nguyªn liÖu kh¸c ®Ó s¼n trong r­¬ng ®å bao gåm:\n- "..(nKvan/10000).." v¹n l­îng.\n".."- "..nTienDong.." tiÒn ®ång.\n".."- "..nSoDSK.." §iÓm sù kiÖn.\n".."- 1 Tö Lôc Lam Thñy Tinh.\n".."- 6 THBT. \n TØ lÖ thµnh c«ng: 40%"
OpenGiveBox("§Æt ®¹i thµnh bÝ kiÕp 90",szLoiNoi,"ghepdtbk120oke")

end
function ghepdtbk120oke()
	local i, j, nIndex, kind, genre, detail, parti, level, series, row;
	local nKvan = 1000000              
	local nTienDong = 15
	local TIENDONG_ID = 4835     
	local nDSK = GetTask(TASK_DSK)	    
	local nSoDSK = 100	 
	local nRealIndex = 0
	local nRealIndexX = 0
	local nBKID = 0
	local count = 0;
	for i = 0, 5 do
		for j = 0, 3 do
			nIndex = GetROItem(ROOAFFAIR, i, j)
			if nIndex > 0 then
				nRealIndex = nIndex
				kind, genre, detail, parti, level, series, row = GetCBItem(nRealIndex)
				if kind == 0 and genre == 6 and detail == 1 and parti == 2433 then -- dai thanh bi kiem
					if parti == 2433 then
						if GetPlayerItemIsLock(nRealIndex) == -2 then
							Talk(1, "", "Trang bÞ ®ang ®­îc kho¸, kh«ng thÓ n©ng cÊp.")
							return
						end
						if GetPlayerItemIsTimeLimit(nRealIndex) > 0 then
							Talk(1, "", "Trang bÞ cã thêi gian sö dông, kh«ng thÓ n©ng cÊp.")
							return
						end
						count = count + 1
						nBKID = parti
					end
				end
			end
		end
	end
	
	if count == 0 then
		Msg2Player("Xin ®Æt ®¹i thµnh bÝ kiÕp 90 vµo.")
		return
	elseif count > 1 then
		Msg2Player("ChØ cÇn ®Æt vµo 1 ®¹i thµnh bÝ kiÕp 90 .")
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
		local nRand = random(1, 2)
		local nSuccess =  nRand 
		if nSuccess == 2 then
			RemoveItem(nRealIndex, 1)
			DelItem(0, -1, 6,1 ,TIENDONG_ID, -1, -1, pos_equiproom, nTienDong)
			DelItem(0,-1,4,238,0, -1, -1,pos_equiproom,nTTT)
			DelItem(0,-1,4,239,0, -1, -1,pos_equiproom,nLTT)
			DelItem(0,-1,4,240,0, -1, -1,pos_equiproom,nLLT)
			DelItem(0,-1,4,353,0, -1, -1,pos_equiproom,nTHBT)
			Pay(nKvan)
			local nIndexX = AddItemSL(2434,1,0)--27 --313
			local s_name = GetItemName(nIndexX)
			EndGiveBox()
			SetTask(TASK_DSK, GetTask(TASK_DSK) - nSoDSK)
			Msg2Player("<color=yellow>Chóc mõng<color><color=green> " .. GetName() .. " <color><color=yellow>ghÐp thµnh c«ng ®¹i thµnh bÝ kiÕp 120!")
			Msg2SubWorld("<color=yellow>Chóc mõng<color><color=green> " .. GetName() .. " <color><color=yellow>ghÐp thµnh c«ng, nhËn ®­îc <color> "..s_name.." .")
		else
			Msg2Player("Xin chia buån ghÐp ®¹i thµnh bÝ kiÕp 120 thÊt b¹i !")
			RemoveItem(nRealIndex, 1)
			DelItem(0, -1, 6,1 ,TIENDONG_ID, -1, -1, pos_equiproom, nTienDong)
			DelItem(0,-1,4,238,0, -1, -1,pos_equiproom,nTTT)
			DelItem(0,-1,4,239,0, -1, -1,pos_equiproom,nLTT)
			DelItem(0,-1,4,240,0, -1, -1,pos_equiproom,nLLT)
			DelItem(0,-1,4,353,0, -1, -1,pos_equiproom,nTHBT)
			Pay(nKvan)
			EndGiveBox()
			SetTask(TASK_DSK, GetTask(TASK_DSK) - nSoDSK)
			
		end
			else
			Msg2Player("Kh¸ch quan ch­a mang ®ñ "..(nKvan/10000).." v¹n l­îng.")
		end
end

function ghepbk120()
		local nKvan = 1000000              
		local nTienDong = 10   
		local nSoDSK = 100
szLoiNoi = "ChØ cÇn ®Æt BÝ KiÕp 90 vµo.\nC¸c nguyªn liÖu kh¸c ®Ó s¼n trong r­¬ng ®å bao gåm:\n- "..(nKvan/10000).." v¹n l­îng.\n".."- "..nTienDong.." tiÒn ®ång.\n".."- "..nSoDSK.." §iÓm sù kiÖn.\n".."- 1 Tö Lôc Lam Thñy Tinh.\n".."- 6 THBT. \n TØ lÖ thµnh c«ng: 60%"
OpenGiveBox("§Æt BÝ KiÕp 90 Vµo",szLoiNoi,"ghepbk120oke")

end
function ghepbk120oke()
	local i, j, nIndex, kind, genre, detail, parti, level, series, row;
	local nKvan = 1000000              
	local nTienDong = 10
	local TIENDONG_ID = 4835     
	local nDSK = GetTask(TASK_DSK)	    
	local nSoDSK = 100	 
	local nRealIndex = 0
	local nRealIndexX = 0
	local nBKID = 0
	local count = 0;
	for i = 0, 5 do
		for j = 0, 3 do
			nIndex = GetROItem(ROOAFFAIR, i, j)
			if nIndex > 0 then
				nRealIndex = nIndex
				kind, genre, detail, parti, level, series, row = GetCBItem(nRealIndex)
				if kind == 0 and genre == 6 and detail == 1 and (parti >= 27 and  parti <= 59) then -- do pho hkmp
					if parti >= 27 and parti <= 59 then
						count = count + 1
						nBKID = parti
					end
				end
			end
		end
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
		local nRand = random(1, 5)
		local nSuccess =  nRand 
		if nSuccess < 3 then
			RemoveItem(nRealIndex, 1)
			DelItem(0, -1, 6,1 ,TIENDONG_ID, -1, -1, pos_equiproom, nTienDong)
			DelItem(0,-1,4,238,0, -1, -1,pos_equiproom,nTTT)
			DelItem(0,-1,4,239,0, -1, -1,pos_equiproom,nLTT)
			DelItem(0,-1,4,240,0, -1, -1,pos_equiproom,nLLT)
			DelItem(0,-1,4,353,0, -1, -1,pos_equiproom,nTHBT)
			Pay(nKvan)
			local nIndexX = AddItemSL(1126,1,0)--27 --313
			local s_name = GetItemName(nIndexX)
			EndGiveBox()
			SetTask(TASK_DSK, GetTask(TASK_DSK) - nSoDSK)
			Msg2Player("<color=yellow>Chóc mõng<color><color=green> " .. GetName() .. " <color><color=yellow>ghÐp thµnh c«ng bÝ kiÕp 120!")
			Msg2SubWorld("<color=yellow>Chóc mõng<color><color=green> " .. GetName() .. " <color><color=yellow>ghÐp thµnh c«ng, nhËn ®­îc <color> "..s_name.." .")
		else
			Msg2Player("Xin chia buån ghÐp bÝ kiÕp 120 thÊt b¹i !")
			RemoveItem(nRealIndex, 1)
			DelItem(0, -1, 6,1 ,TIENDONG_ID, -1, -1, pos_equiproom, nTienDong)
			DelItem(0,-1,4,238,0, -1, -1,pos_equiproom,nTTT)
			DelItem(0,-1,4,239,0, -1, -1,pos_equiproom,nLTT)
			DelItem(0,-1,4,240,0, -1, -1,pos_equiproom,nLLT)
			DelItem(0,-1,4,353,0, -1, -1,pos_equiproom,nTHBT)
			Pay(nKvan)
			EndGiveBox()
			SetTask(TASK_DSK, GetTask(TASK_DSK) - nSoDSK)
			
		end
			else
			Msg2Player("Kh¸ch quan ch­a mang ®ñ "..(nKvan/10000).." v¹n l­îng.")
		end
end
function no()
	
end
