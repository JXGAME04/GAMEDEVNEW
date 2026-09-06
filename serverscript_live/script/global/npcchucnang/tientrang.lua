--Author: Fong KiÒu
--Date: 2021
--Function: Chñ tiÒn trang l©m an

Include("\\script\\log_game\\save_log.lua")
Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_map.lua")
Include("\\script\\lib\\lib_vatpham.lua")

TY_LE_DOI_KNB_XU = 100

function main()
	
	dofile("script/global/npcchucnang/tientrang.lua")
	
	Talk(1,"gamebank_proc","ViÖc lµm ¨n cña bæn tiÖm ngµy cµng ph¸t ®¹t, ta dù ®Þnh sang n¨m më thªm vµi chi nh¸nh. Nh­ng viÖc nµy cßn ph¶i th­¬ng l­îng l¹i víi Hé bé V­¬ng ®¹i nh©n, kh«ng cã sù b¶o hé cña «ng ta, cöa tiÖm cho vay cña ta sÏ ch¼ng lµm g× ®­îc")

end

function gamebank_proc()
	local msg = {
		"Ta muèn rót Kim Nguyªn B¶o/rutknb", 
		"KiÓm tra ng©n khè/show_ingot", 
		"Ta muèn trao ®æi KNB thµnh Xu/doiknb", 
		"Ta muèn ®æi Xu thµnh TiÒn §ång/doiXuTonTD", 
		"Ta muèn ®æi TiÒn §ång thµnh Xu/maindoitdtoxu",
		"Nh©n tiÖn ghÐ qua th«i/no" 
	}
	
	local talk = ""
	if (GetSex() == 0) then
		talk = "§óng råi, thiÕu hiÖp cÇn g×?"
	else
		talk = "§óng råi, n÷ hiÖp cÇn g×?"
	end

	Say(talk, getn(msg), msg)
end

function rutknb()
	local msg = {
		format("Ta muèn rót 1 KNB/#get_ingotknb(%d)", 1), 
		format("Ta muèn rót 5 KNB/#get_ingotknb(%d)", 5), 
		format("Ta muèn rót 10 KNB/#get_ingotknb(%d)", 10), 
		format("Ta muèn rót 20 KNB/#get_ingotknb(%d)", 20), 
		"Nh©n tiÖn ghÐ qua th«i/no"
	}
	local talk = ""
	if (GetSex() == 0) then
		talk = "§óng råi, thiÕu hiÖp cÇn g×?"
	else
		talk = "§óng råi, n÷ hiÖp cÇn g×?"
	end
	Say(talk, getn(msg), msg)
end

function get_ingotknb(n_knb)
	local nMoney = GetExtPoint()
	if (nMoney >= 32768) then
		nMoney = 0
		Msg2Player("Chøc n¨ng bÞ lçi!")
		return
	end
	if(nMoney < n_knb) then
		Talk(1,"","Xin lçi! Kh¸ch quan kh«ng ®ñ sè KNB t¹i bæn trang!") 
		return
	end
	if CalcFreeItemCellCount() < n_knb then
		Talk(1, "", "CÇn Ýt nhÊt "..n_knb.." « trèng trong hµnh trang.")
		return
	end
	if (nMoney <= 0) then
		Talk(1,"","Xin lçi! Kh¸ch quan kh«ng cã göi KNB t¹i bæn trang!") 
		return
	else
		Say("<#> Kh¸ch quan ®ang göi ë ®©y tæng céng <color=green>"..nMoney.."<color> KNB, muèn rót chø?", 2, 
		format("Ta muèn rót %d KNB/#get_ingotknb_ok(%d)", n_knb, n_knb),
		 "Kh«ng rót/no") 
	end
end

function get_ingotknb_ok(n_knb)
	Msg2Player("§ang rót Kim Nguyªn B¶o! Xin cÈn träng vµ ®õng tho¸t khái trß ch¬i!")			
	local npayRes = PayExtPoint(n_knb)
	if npayRes > 0 then
		for i = 1, n_knb do
			AddEventItem(ID_KIM_NGUYEN_BAO, 1)
			AddItemSL(4850,5,0) -- ruong trang bi xanh
		end
		
		SetTask(TASK_DSK, GetTask(TASK_DSK) + n_knb *100)
		Talk(1,"","Kh¸ch quan ®· nhËn ®­îc <color=red>"..n_knb.."<color> Kim Nguyªn B¶o")
		logRutXu(format("***Account [%s] nh©n vËt [%s] ®· rót %d KNB ë TiÒn Trang L©m An.***", GetAccount(), GetName(), n_knb))
		SetTask(T_RUT_KNB, GetTask(T_RUT_KNB) + n_knb)
		SaveNow()		
	else
		Talk(1,"","Rót xu bÞ lçi liªn hÖ víi nhµ s¶n xuÊt ®Ó gi¶i quyÕt!")
	end
end

function show_ingot()
	local nMoney = GetExtPoint()
	if (nMoney >= 32768) then
		nMoney = 0
		Msg2Player("T¹m thêi kh«ng thÓ kiÓm tra! Xin liªn hÖ víi nhµ s¶n xuÊt ®Ó gi¶i quyÕt!") 
		return
	end
	if (nMoney <= 0) then
		Say("<npc>: Kh¸ch quan ch­a hÒ göi kú tr©n b¶o vËt g× ë ®©y", 1, "BiÕt råi/no") 
		return
	else
		local nCurVar = nMoney
		Say("<npc>: Kh¸ch quan ®ang göi ë ®©y <color=green>"..nCurVar.."<color> Kim Nguyªn B¶o, ®· tõng rót <color=yellow>"..GetTask(T_RUT_KNB) .."<color> Kim Nguyªn B¶o", 1, "BiÕt råi/no") 
	end
end

function doiknb()
	local msg = {
		format("§æi 20 KNB thµnh 2000 Xu/DoiKnbToCoin(%d)", 20),
		format("§æi 10 KNB thµnh 1000 Xu/DoiKnbToCoin(%d)", 10),
		format("§æi 5 KNB thµnh 500 Xu/DoiKnbToCoin(%d)", 5),
		format("§æi 1 KNB thµnh 100 Xu/DoiKnbToCoin(%d)", 1),
		"Nh©n tiÖn ghÐ qua th«i/no" 
	}
	local talk = ""
	if (GetSex() == 0) then
		talk = "§óng råi, thiÕu hiÖp cÇn g×?"
	else
		talk = "§óng råi, n÷ hiÖp cÇn g×?"
	end
	Say(talk, getn(msg), msg)
end

function DoiKnbToCoin(n_knb)
	--if CalcFreeItemCellCount() < n_knb then
	--	Talk(1, "", "CÇn Ýt nhÊt "..n_knb.." « trèng trong hµnh trang.")
	--	return
	--end	
	local KNB = GetItemCount(0, 4, ID_KIM_NGUYEN_BAO, 0, -1, -1, pos_equiproom) -- ®Õm sè KNB trong hµnh trang
	if KNB < n_knb then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..n_knb.." Kim Nguyªn B¶o.")
		return
	end
	local nResDel = DelItem(0, -1, 4, ID_KIM_NGUYEN_BAO, 0, -1, -1, ROOME, n_knb) -- xo¸ sè knb trong hµnh trang
	if(nResDel >= n_knb) then
		local nTienDong = n_knb * TY_LE_DOI_KNB_XU
		--for i = 1, n_knb do
		--	AddEventItem(ID_XU_TIENDONG, TY_LE_DOI_KNB_XU)
		--end
		SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) + nTienDong)
		Msg2Player("Kh¸ch quan ®· nhËn ®­îc "..nTienDong.."  Xu.")
		logRutXu(format("***Account [%s] nh©n vËt [%s] ®· ®æi %d KNB thµnh %d Xu ë TiÒn Trang.***", GetAccount(), GetName(), n_knb, nTienDong))
	else
		Talk(1, "", "Hµnh trang kh«ng ®ñ "..n_knb.." Kim Nguyªn B¶o.")
	end
end

function doiXuTonTD()
	local msg = {
		format("§æi 11 Xu thµnh 1 TiÒn §ång/DoiXuToTDG(%d)", 11),
		format("§æi 110 Xu thµnh 10 TiÒn §ång/DoiXuToTDG(%d)", 110),
		format("§æi 550 Xu thµnh 50 TiÒn §ång/DoiXuToTDG(%d)", 550),
		format("§æi 1100 Xu thµnh 100 TiÒn §ång/DoiXuToTDG(%d)", 1100),
		"Nh©n tiÖn ghÐ qua th«i/no" 
	}
	local talk = ""
	if (GetSex() == 0) then
		talk = "§óng råi, thiÕu hiÖp cÇn g×?"
	else
		talk = "§óng råi, n÷ hiÖp cÇn g×?"
	end
	Say(talk, getn(msg), msg)
end

function DoiXuToTDG(nXu)
	
	local nTD = nXu / 11
	if GetTask(T_PLAYER_XU) < nXu then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nXu.." Xu.")
		return
	end

	if(GetTask(T_PLAYER_XU) >= nXu) then
		SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) - nXu)
		for i=1, nTD  do	
		 AddItem(6,1,4835,0,0,0,0)
	   end
		Msg2Player("Kh¸ch quan ®· nhËn ®­îc "..nTD.."  TiÒn §ång.")
		logRutXu(format("***Account [%s] nh©n vËt [%s] ®· ®æi %d Xu thµnh %d TiÒn §ång ë TiÒn Trang.***", GetAccount(), GetName(), nXu, nTD))
	else
		Talk(1, "", "Hµnh trang kh«ng ®ñ "..nXu.." Xu.")
	end
end

function maindoitdtoxu()
	local msg = {
		format("§æi 1 TiÒn §ång thµnh 10 Xu/DoiTdToXu(%d)", 1),
		format("§æi 10 TiÒn §ång thµnh 100 Xu/DoiTdToXu(%d)", 10),
		format("§æi 50 TiÒn §ång thµnh 500 Xu/DoiTdToXu(%d)", 50),
		format("§æi 100 TiÒn §ång thµnh 1000 Xu/DoiTdToXu(%d)", 100),
		"Nh©n tiÖn ghÐ qua th«i/no" 
	}
	local talk = ""
	if (GetSex() == 0) then
		talk = "§óng råi, thiÕu hiÖp cÇn g×?"
	else
		talk = "§óng råi, n÷ hiÖp cÇn g×?"
	end
	Say(talk, getn(msg), msg)
end

function DoiTdToXu(xTD)
	
	local ETD = GetItemCount(0,6,1, 4835, -1, -1, pos_equiproom) -- ®Õm sè KNB trong hµnh trang
	if ETD < xTD then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..xTD.." TiÒn §ång.")
		return
	end
	local nResDel = DelItem(0, -1, 6,1 ,4835, -1, -1, ROOME, xTD) -- xo¸ sè knb trong hµnh trang
	if(nResDel >= xTD) then
		local TongXu = xTD * 10

		SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) + TongXu)
		Msg2Player("Kh¸ch quan ®· nhËn ®­îc "..TongXu.."  Xu.")
		logRutXu(format("***Account [%s] nh©n vËt [%s] ®· ®æi %d TiÒn §ång thµnh %d Xu ë TiÒn Trang.***", GetAccount(), GetName(), xTD, TongXu))
	else
		Talk(1, "", "Hµnh trang kh«ng ®ñ "..xTD.." TiÒn §ång.")
	end
end

function DoiXuToTD()
	
	local nXu = 110 -- ®Õm Xu trong hµnh trang
	if GetTask(T_PLAYER_XU) < nXu then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nXu.." Xu.")
		return
	end
	
	if(GetTask(T_PLAYER_XU) >= nXu) then
		SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) - nXu)
		AddEventItem(ID_KIM_NGUYEN_BAO, 1)
		Msg2Player("Kh¸ch quan ®· nhËn ®­îc 1  KNB.")
		logRutXu(format("***Account [%s] nh©n vËt [%s] ®· ®æi ®­îc 1 KNB ë TiÒn Trang.***", GetAccount(), GetName()))
	else
		Talk(1, "", "Hµnh trang kh«ng ®ñ "..nXu.." Xu.")
	end
end

function no()
	
end