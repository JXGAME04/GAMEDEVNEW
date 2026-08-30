--Author: Fong KiÒu
--Date: 19/08/2021
--Function: Npc Tiªu §Çu

Include("\\script\\lib\\lib_task.lua")
-- [CFGVT 29/08] hai tep duoi day la LA (khong Include gi).
Include("\\script\\cauhinh\\ch_lib.lua")
Include("\\script\\cauhinh\\ch_thuong.lua")
-- [CFGVT 29/08] Bo doc cau hinh cho tep nay. Tra ve MAC DINH (= so cu) khi bo cau
-- hinh chua nap, nen kem nhat cung khong the doi hanh vi.
function VT_CFG(szKhoa, macdinh)
	if (G_CFG ~= nil) then
		return G_CFG(szKhoa, macdinh)
	end
	return macdinh
end

Include("\\script\\lib\\lib_map.lua")
Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\lib\\lib_server.lua")
Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\event\\event_vantieu\\lib_vt.lua")

END_TALK 									= "Ta chØ ghÐ ngang qua./no"
KIND_TIEU_XA 							= "Ng­¬i ®· nhËn ®­îc nhiÖm vô ¸p %s %s"
TIEUXABIHUY 							= "Ng­¬i ch­a nhËn nhiÖm vô ¸p tiªu hoÆc tiªu xa ®· bÞ ph¸ huû."
HUYNHIEMVU 							= "Huû nhiÖm vô ¸p tiªu thµnh c«ng."
NOENOUGHMONEY 				= "Ng­¬i kh«ng ®ñ %d l­îng."
GO_TIEUSU 								= "H·y nhanh chãng ¸p tiªu tíi Long M«n Tiªu S­ [%s] to¹ ®é (%d/%d), ng­¬i cã tèi ®a 30 phót."

function main(nNpcIndex)
	
	 dofile("script/event/event_vantieu/tieudau.lua")
	
	local nYr,nMo,nDy,nHr,nMi,nSe,nDyfW = GetTimeNow()	
	if(nHr < TIME_VANTIEU[1] or nHr > TIME_VANTIEU[2]) then
		Talk(1,"","Ho¹t ®éng diÔn ra vµo lóc 15h ®Õn 23h hµng ngµy, b©y giê kh«ng ph¶i lóc")
		return
	end
	
	if(GetLevel() < LEVEL_LIMIT_VT) then
		Talk(1,"","Kh«ng ®ñ ®¼ng cÊp "..LEVEL_LIMIT_VT.." ®Ó tham gia ho¹t ®éng")
		return
	end
	
	local nSTP = GetTask(T_NHANNVVTIEU)
	local dwNpcID = GetTask(T_NPCID)
	SetTaskTemp(TMP_INDEX_NPC, nNpcIndex)	
	
	if (nSTP == 0) then			-- chua nhan
		SayEx({12655,
		"NhËn nhiÖm vô ¸p tiªu/vantieu",
		"Ta muèn mua ®¹o cô ¸p tiªu/mua",
		END_TALK})
		return 
	end		
	
	if (nSTP == 1) and (dwNpcID == 0) then		-- da nhan nv, chua khoi tao npc
		SayEx({12655,
		"Ta muèn dïng Ho¸n Tiªu ChØ lµm míi nhiÖm vô ¸p tiªu/lammoi",
		"Ta muèn ¸p tiªu ngay/go_tieuxa",
		"Ta muèn sö dông uû nhiÖm tr¹ng nhËn tiªu xa vµng/uynhiemtrang",
		"Ta muèn mua ®¹o cô ¸p tiªu/mua",
		"H·y cho ta suy nghÜ thªm/Exit"})	
		return 
	end
	
	if (nSTP == 1) and (dwNpcID > 0) then		-- dang van tieu
		SayEx({12655,
		"Tõ bá nhiÖm vô ¸p tiªu/huytieu",
		"TruyÒn tèng tíi tiªu xa (100000 l­îng)/truyentongtieuxa",
		--"H·y ®­a ta tíi n¬i ¸p tiªu/dichuyen", --®i ngay ®Õn n¬i tr¶ nhiÖm vô
		"Ta muèn mua ®¹o cô ¸p tiªu/mua",
		"Xem th«ng tin tiªu xa/thongtin",
		END_TALK})	
		return 
	end	

	if (nSTP == 2) then			-- hoan thanh
		SayEx({12655,
		"NhËn th­ëng nhiÖm vô ¸p tiªu/thuongtieu",	
		"Ta muèn mua ®¹o cô ¸p tiªu/mua",		
		END_TALK})
		return 
	end	
end 

function vantieu()
	if GetTask(T_NVVTTRONGNGAY) >= MAXVANTIEU then
		Talk(1,"","H«m nay <sex> ®· vËn "..MAXVANTIEU.." lÇn tiªu råi.")	
		return 
	end
	OpenGiveBox("§Æt vµo vËt phÈm","PhÝ ¸p tiªu: 1 Hé Tiªu LÖnh, 50 v¹n l­îng","ktdieukien")
end

function ktdieukien()
	if GetCash() < NEED_MONEY then
		Talk(1,"",format(NOENOUGHMONEY,NEED_MONEY))
		return 
	end
	if(GetItemCount(0, ITEM_HOTIEULENH[1],ITEM_HOTIEULENH[2],ITEM_HOTIEULENH[3],-1,-1, pos_affairitem)>=1) then
		if(ConsumeItem(1 ,0, ITEM_HOTIEULENH[1],ITEM_HOTIEULENH[2],ITEM_HOTIEULENH[3],-1,-1, pos_affairitem) >= 1) then
			Pay(NEED_MONEY)
			chitiettieuxa()
			EndGiveBox()	-- dong give box	
		end
	else
		Talk(1,"","Vui lßng ®Æt vµo chÝnh x¸c vËt phÈm.")
	end	
end

function chitiettieuxa()
	local nRand = random(1, 50)
	local nLoai = 0

	if nRand < 20 then
		nLoai = 1
	elseif nRand < 40 then
		nLoai = 2
	else
		nLoai = 3 
	end
	SetTask(T_NHANNVVTIEU,1)
	SetTask(T_LOAITIEUXA, nLoai)
	local tbInfo = KIND_TIEUXA[nLoai]
	Talk(1, "luachon", format(KIND_TIEU_XA, tbInfo[4], tbInfo[3]))
end

function luachon()
	SayEx({12655,
	"Ta muèn dïng Ho¸n Tiªu ChØ lµm míi nhiÖm vô ¸p tiªu/lammoi",
	"Ta muèn ¸p tiªu ngay/go_tieuxa",
	"Ta muèn sö dông uû nhiÖm tr¹ng nhËn tiªu xa vµng/uynhiemtrang",
	"Ta muèn mua ®¹o cô ¸p tiªu/mua",
	"H·y cho ta suy nghÜ thªm/Exit"})	
end

function lammoi()
	local nCount = 1 local nLine = 1
	if DelItem(0,nLine,ITEM_HOANTIEUCHI[1],ITEM_HOANTIEUCHI[2],ITEM_HOANTIEUCHI[3],-1,-1,ROOME,nCount) >= 1 then
		TimeBox("lµm míi tiªu xa",5,"chitiettieuxa")
	else
		Talk(1,"luachon","Ng­¬i kh«ng cã Ho¸n Tiªu ChØ.")
	end
end

function uynhiemtrang()
	local nCount = 1
	if DelItem(0,1,ITEM_UYNHIEMTRANG[1],ITEM_UYNHIEMTRANG[2],ITEM_UYNHIEMTRANG[3],-1,-1,ROOME,nCount) >= nCount then
		SetTask(T_LOAITIEUXA,3)
		Talk(1,"go_tieuxa",format(KIND_TIEU_XA,KIND_TIEUXA[GetTask(T_LOAITIEUXA)][4],KIND_TIEUXA[GetTask(T_LOAITIEUXA)][3]))
	else
		Talk(1,"luachon","Ng­¬i kh«ng cã Uû NhiÖm Tr¹ng.")
	end
end

function go_tieuxa()
	local nParam = GetNpcValue(GetTaskTemp(TMP_INDEX_NPC))
	local nKind = GetTask(T_LOAITIEUXA)
	local nW,nX,nY = GetWorldPos()
	local nName = GetName() --	npc tieu xa
	local nCamp = GetCurCamp()
	local nNpcName = KIND_TIEUXA[nKind][3]
	local nLifeNpc = nKind * 2000000
	local nNpcIdx = AddNpcEx3({KIND_TIEUXA[nKind][2]},1,{0,1,2,3,4},nW,nX*32,nY*32,DROPRATETIEU,ONDEATHTIEU,nName..nNpcName,nCamp,0,nLifeNpc,nil,nil,nil,nil,nil,nil,100)
	SetNpcValue(nNpcIdx,nParam)--quy ®Þnh lé tr×nh
	SetNpcBoss(nNpcIdx,nKind)
	SetNpcValue(nNpcIdx,1, nKind)--quy ®Þnh lo¹i tiªu
	SetNpcOwner(nNpcIdx,nName,1)
	SetNpcFindPathTime(nNpcIdx,TIME_LIMIT_VT)	-- 30 phut
	SetNpcTimer(nNpcIdx,TIME_LIMIT_VT)	-- 30 phut
	local dwNpcID = GetNpcID(nNpcIdx) --	hoan thanh xong nhan nhiem vu
	SetTask(T_NPCID,dwNpcID)
	local str = format(GO_TIEUSU,GetMapName(NPC_TALKTIEU[nParam][9]),floor(NPC_TALKTIEU[nParam][10]/8),floor(NPC_TALKTIEU[nParam][11]/16))
	Talk(1,"",str)
	str = format("<color=green>Xa Tiªu:<color> tíi %s %d/%d. ",GetMapName(NPC_TALKTIEU[nParam][9]),floor(NPC_TALKTIEU[nParam][10]/8),floor(NPC_TALKTIEU[nParam][11]/16))
	AddNote(str)
end

function dichuyen()
	if GetItemCount(0,ITEM_UYNHIEMTRANG[1],ITEM_UYNHIEMTRANG[2],ITEM_UYNHIEMTRANG[3],-1,-1,pos_equiproom) < 1 then
		Talk(1,"","Ng­¬i kh«ng mang theo Uû NhiÖm Tr¹ng .")
		return 
	end
	local nParam = GetNpcValue(GetTaskTemp(TMP_INDEX_NPC))
	local dwNpcID = GetTask(T_NPCID)
	local nNpcIdx = FindNpcFrID(dwNpcID)
	local nLine = 1 local nCount = 1
	local nResDel = DelItem(0,nLine,ITEM_UYNHIEMTRANG[1],ITEM_UYNHIEMTRANG[2],ITEM_UYNHIEMTRANG[3],-1,-1,ROOME,nCount)
	if (nNpcIdx > 0 and nResDel >= nCount) then
		NpcNewWorld(nNpcIdx,NPC_TALKTIEU[nParam][9],NPC_TALKTIEU[nParam][10],NPC_TALKTIEU[nParam][11])
		NewWorld(NPC_TALKTIEU[nParam][9],NPC_TALKTIEU[nParam][10],NPC_TALKTIEU[nParam][11])
		SetFightState(1)
		return 
	end
	Talk(1,"","Ng­¬i ch­a nhËn nhiÖm vô ¸p tiªu.")
end

function truyentongtieuxa()	
	if GetCash() < NEED_MONEY_GO then
		Talk(1,"",format(NOENOUGHMONEY,NEED_MONEY_GO))
		return 
	end

	local dwNpcID = GetTask(T_NPCID)
	local nNpcIdx = FindNpcFrID(dwNpcID)
	if (nNpcIdx > 0) then
		local nValue = GetNpcFightState(nNpcIdx)
		local nW,nX,nY = GetNpcWorldPos(nNpcIdx)
		NewWorld(nW,nX,nY)
		if (nValue > 0) then
			SetFightState(1)
		end
		Pay(NEED_MONEY_GO)
		return 
	end	
	
	Talk(1,"",TIEUXABIHUY)
end

function thongtin()
	local dwNpcID = GetTask(T_NPCID)
	local nNpcIdx = FindNpcFrID(dwNpcID)
	if (nNpcIdx > 0) then
		local nW,nX,nY = GetNpcWorldPos(nNpcIdx)
		local nKind = GetNpcValue(nNpcIdx,1)
		if(nKind < 1) then nKind = 1 end
		if(nKind > 3) then nKind = 3 end
		local nColor = KIND_TIEUXA[nKind][4]
		local nNpcName = KIND_TIEUXA[nKind][3]
		local nMapName = GetMapName(nW)
		nX,nY = floor(nX/8),floor(nY/16)
		Msg2Player("Th«ng tin tiªu xa:"..nColor..""..nNpcName.." <color>vÞ trÝ "..nMapName.." "..nX.."/"..nY.."")
		return 
	end
	Talk(1,"",TIEUXABIHUY)
end

function huytieu()
	local dwNpcID = GetTask(T_NPCID)
	local nNpcIdx = FindNpcFrID(dwNpcID)
	if (nNpcIdx > 0) then
		DelNpc(nNpcIdx)
	end
	SetTask(T_NPCID,0)
	SetTask(T_LOAITIEUXA,0)
	SetTask(T_NHANNVVTIEU,0)
	SetTask(T_NVVTTRONGNGAY,GetTask(T_NVVTTRONGNGAY)+1)
	Talk(1,"",HUYNHIEMVU)
	
	local nCount = 1 local nLine = 1 --nÕu cã tiªu kú hoµn l¹i 50% tiÒn c«ng vô
	if DelItem(0,nLine,ITEM_TIEUKY[1],ITEM_TIEUKY[2],ITEM_TIEUKY[3],-1,-1,ROOME,nCount) >= 1 then
		Earn(ITEM_TIEUKY[8])
		Msg2Player("NhiÖm vô kh«ng hoµn thµnh nhËn l¹i tiÒn lao vô. ")
	end
end

function thuongtieu()
	local nKind = GetTask(T_LOAITIEUXA)
	local nExp = GetTongExp()
	for k=1,VT_CFG("VT_EXP_NEN_LAN", 1000) do AddOwnExp(VT_CFG("VT_EXP_NEN_MOI", 50000)) end
	AddSumExp(VT_CFG("VT_EXP_NEN_SUM", 500000000))
	Msg2Player("B¹n vËn tiªu thµnh c«ng, nhËn ®­îc 50.000.000 kinh nghiÖm")
	if(nKind == 1) then --th­ëng thªm nÕu lµ tiªu ®ång
		AddSumExp(VT_CFG("VT_EXP_DONG", 100000000))
		AddItemSL(4844,VT_CFG("VT_HOMACH_DONG", 300),-2) -- ho mach don
		AddItemSL(4847,VT_CFG("VT_CHANNGUYEN_DONG", 50),-2) -- chan nguyen don
		AddItemSL(4850,VT_CFG("VT_RUONG_DONG", 2),0) -- ruong trang bi xanh
		
		SetTongExp(nExp + VT_CFG("VT_DIEM_TONG_DONG", 100))
		SetTask(T_NPCID,0)
		Msg2Player("Tiªu ®ång b¹n nhËn thªm 10.000.000 kinh nghiÖm")
	elseif(nKind == 2) then -- b¹c
		AddSumExp(VT_CFG("VT_EXP_BAC", 300000000))
		AddItemSL(4844,VT_CFG("VT_HOMACH_BAC", 400),-2) -- ho mach don
		AddItemSL(4847,VT_CFG("VT_CHANNGUYEN_BAC", 70),-2) -- chan nguyen don
		AddItemSL(4850,VT_CFG("VT_RUONG_BAC", 5),0) -- ruong trang bi xanh
		
		SetTongExp(nExp + VT_CFG("VT_DIEM_TONG_BAC", 200))
		SetTask(T_NPCID,0)
		Msg2Player("Tiªu b¹c b¹n nhËn thªm 30.000.000 kinh nghiÖm")
	elseif(nKind == 3) then -- vµng
		AddSumExp(VT_CFG("VT_EXP_VANG", 500000000))
		AddItemSL(4844,VT_CFG("VT_HOMACH_VANG", 500),-2) -- ho mach don
		AddItemSL(4847,VT_CFG("VT_CHANNGUYEN_VANG", 100),-2) -- chan nguyen don
		AddItemSL(4850,VT_CFG("VT_RUONG_VANG", 10),0) -- ruong trang bi xanh
		
		SetTongExp(nExp + VT_CFG("VT_DIEM_TONG_VANG", 300))
		SetTask(T_NPCID,0)
		Msg2Player("Tiªu vµng b¹n nhËn thªm 50.000.000 kinh nghiÖm")
	end
	AddItemSL(1023,VT_CFG("VT_LENHBAI_BOSS", 10),-2) -- LB bosss
	local randomtg = random(1,100)
	if(randomtg <= 10) then
		AddItem(6,1,random(121, 123),0,0,0,0)--Phuc Duyen
	elseif(randomtg == 20) then
		AddItem(4,239,0,0,0,0,0)--Thuy tinh
	elseif(randomtg == 30) then
		AddItem(4,240,0,0,0,0,0)--Thuy tinh
	elseif(randomtg == 40) then
		AddItem(4,353,0,0,0,0,0)--THBT
	elseif(randomtg == 50) then
		AddItem(4,238,0,0,0,0,0)--Thuy tinh
	elseif(randomtg == 60) then
		AddItem(6,1,123,0,0,0,0)--Phuc Duyen dai
	elseif(randomtg == 70) then
		AddItem(6,1,124,0,0,0,0)--QuÕ hoa töu que hoa tuu
	elseif(randomtg == 80) then
		AddItem(6,1,71,0,0,0,0)--Tien thao lo
	elseif(randomtg >= 90) then
		AddItem(6,1,20,0,0,0,0)--hoa hong
	end
	UpdateTongExpAndLevel()
	local msgLog = format("Xin chóc mõng ®¹i hiÖp <color=green>%s<color> ®· ¸p tiªu thµnh c«ng nhËn ®­îc th­ëng gi¸ trÞ. ", GetName())
	Msg2SubWorld(msgLog)
	logHoatDong(msgLog)
	SetTask(T_NPCID,0)
	SetTask(T_NHANNVVTIEU,0)
	SetTask(T_NVVTTRONGNGAY,GetTask(T_NVVTTRONGNGAY)+1)
	SetTask(T_LOAITIEUXA,0)
end

function thuongtieu2()
	local nKind = GetTask(T_LOAITIEUXA)
	local randomtg = random(5,10)
	local moneythg = NEED_MONEY*randomtg*nKind
	local nexpthg = nKind*100000*randomtg
	Earn(moneythg)
	AddOwnExp(nexpthg)
	if(nKind == 2 and randomtg >= 8) then
		AddEventItem(353)
	end
	if(nKind == 3 and randomtg >= 7) then
		AddEventItem(353) AddEventItem(353)
	end	
	local msgLog = format("Xin chóc mõng ®¹i hiÖp <color=green>%s<color> ®· ¸p tiªu thµnh c«ng nhËn ®­îc th­ëng gi¸ trÞ. ", GetName())
	Msg2SubWorld(msgLog)
	logHoatDong(msgLog)
	SetTask(T_NPCID,0)
	SetTask(T_NHANNVVTIEU,0)
	SetTask(T_NVVTTRONGNGAY,GetTask(T_NVVTTRONGNGAY)+1)
	SetTask(T_LOAITIEUXA,0)
end

function mua()
	SayEx({12655,
	"Mua Ho¸n Tiªu ChØ/muahoantieuchi",
	"Mua Uû NhiÖm Tr¹ng/muauynhiemtrang",
	"Mua Hé Tiªu LÖnh/muahotieulenh",
	END_TALK})
end

function muahoantieuchi()
	local nItem = ITEM_HOANTIEUCHI
	local nganluongm = nItem[8]	
	if(GetCash() < nganluongm) then
		Talk(1,"","CÇn cã "..nganluongm.." l­îng. ")
		return
	end
	Pay(nganluongm)
	AddItem(nItem[1],nItem[2],nItem[3],nItem[4],nItem[5],nItem[6],nItem[7])
	local logMgs = GetName().." mua hoan tieu chi gia: ".. nganluongm
	Msg2Player(logMgs)
	logHoatDong(logMgs)
end

function muauynhiemtrang()
	local nItem = ITEM_UYNHIEMTRANG
	local nganluongm = nItem[8]	
	if(GetCash() < nganluongm) then
		Talk(1,"","CÇn cã "..nganluongm.." l­îng. ")
		return
	end
	Pay(nganluongm)
	AddItem(nItem[1],nItem[2],nItem[3],nItem[4],nItem[5],nItem[6],nItem[7])
	local logMgs = GetName().." mua uy nhiem trang gia: ".. nganluongm
	Msg2Player(logMgs)
	logHoatDong(logMgs)	
end

function muahotieulenh()
	local nItem = ITEM_HOTIEULENH
	local nganluongm = nItem[8]	
	if(GetCash() < nganluongm) then
		Talk(1,"","CÇn cã "..nganluongm.." l­îng. ")
		return
	end
	Pay(nganluongm)
	AddItem(nItem[1],nItem[2],nItem[3],nItem[4],nItem[5],nItem[6],nItem[7])
	local logMgs = GetName().." mua ho tieu lenh gia: ".. nganluongm
	Msg2Player(logMgs)
	logHoatDong(logMgs)		
end

function no()
	Talk(1,"",12245)
end

function Exit()
	
end