-- [BDH 28/08] BO TEST DONG HANH tren lenh bai admin (GO khi het can)
-- Cac ham PARTNER_* la API C da dang ky; chay trong ngu canh NGUOI BAM.
Include("\\script\\partner\\partner_test_bdh.lua")

function BDH_Root()
	-- [29/08] menu partner cu DA GO theo yeu cau chu ("cac dong pet cu khong
	-- dung thi xoa di") - vao thang he PET ban PC. Ham BDH_A_* giu lai duoi
	-- file de khoi pha nhung khong con duong vao.
	BDH_P_Root()
end

function BDH_A_CapKim()
	if (PARTNER_Count() >= 3) then
		Msg2Player("§· ®ñ 3 con - xãa bít råi cÊp.")
		return
	end
	local r = PARTNER_AddFightPartner(5, 0, 1, 5, 5, 5, 5, 5, 5)
	Msg2Player("CÊp thó hÖ Kim: " .. r)
	BDH_Root()
end

function BDH_A_CapNgau()
	if (PARTNER_Count() >= 3) then
		Msg2Player("§· ®ñ 3 con - xãa bít råi cÊp.")
		return
	end
	local r = PARTNER_AddFightPartner(random(1, 5), random(0, 4), random(1, 4),
		random(1, 10), random(1, 10), random(1, 10), random(1, 10), random(1, 10), random(1, 10))
	Msg2Player("CÊp thó ngÉu nhiªn: " .. r)
	BDH_Root()
end

function BDH_A_Goi()
	local nCur, nSt = PARTNER_GetCurPartner()
	if (nSt == 1) then
		PARTNER_CallOutCurPartner(0)
		Msg2Player("§· thu vÒ.")
	else
		local r = PARTNER_CallOutCurPartner(1)
		Msg2Player("Gäi ra: " .. r .. " (0 = h«n mª/cooldown/map cÊm)")
	end
end

function BDH_A_Exp()
	local nCur = PARTNER_GetCurPartner()
	if (nCur == 0) then return end
	PARTNER_AddExp(nCur, 10000, 0)
	Msg2Player("Exp = " .. PARTNER_GetExp(nCur) .. " - cÊp " .. PARTNER_GetLevel(nCur))
end

function BDH_A_LenCap()
	local nCur = PARTNER_GetCurPartner()
	if (nCur == 0) then return end
	PARTNER_LevelUp(nCur)
	Msg2Player("CÊp míi = " .. PARTNER_GetLevel(nCur))
end

function BDH_A_Emo()
	local nCur = PARTNER_GetCurPartner()
	if (nCur == 0) then return end
	PARTNER_SetEmotionDegree(nCur, 100)
	Msg2Player("Th©n mËt = " .. PARTNER_GetEmotionDegree(nCur))
end

function BDH_A_Tui()
	SetPartnerBagLevel(10)
	Msg2Player("Tói ®ång hµnh cÊp 10 (6x10 « më hÕt).")
end

function BDH_A_Skill()
	local nCur = PARTNER_GetCurPartner()
	if (nCur == 0) then return end
	PARTNER_AddSkill(nCur, 2, 594, 3, 0)
	local lv = PARTNER_GetSkillInfo(nCur, 594)
	Msg2Player("§· d¹y kü n¨ng 594 cÊp " .. lv)
end

function BDH_A_Info()
	local nCur = PARTNER_GetCurPartner()
	if (nCur == 0) then
		Msg2Player("Ch­a cã con nµo ®­îc chän.")
		return
	end
	Msg2Player("Tªn: " .. PARTNER_GetName(nCur) .. " - cÊp " .. PARTNER_GetLevel(nCur) .. " - exp " .. PARTNER_GetExp(nCur))
	Msg2Player("HÖ: " .. PARTNER_GetSeries(nCur) .. " - tÝnh c¸ch " .. PARTNER_GetCharacter(nCur) .. " - th©n mËt " .. PARTNER_GetEmotionDegree(nCur) .. "/100")
	local a1, a2, a3, a4, a5, a6 = PARTNER_GetAttribs(nCur)
	Msg2Player("Sinh lùc " .. floor(a1) .. " - søc ®¸nh " .. floor(a2) .. " - chÝnh x¸c " .. floor(a3))
	Msg2Player("NÐ " .. floor(a4) .. " - tèc ®é " .. floor(a5) .. " - may m¾n " .. floor(a6) .. " - m¸u NPC %" .. PARTNER_GetEndure())
end

function BDH_A_Xoa()
	local nCur = PARTNER_GetCurPartner()
	if (nCur == 0) then return end
	local r = PARTNER_RemovePartner(nCur)
	Msg2Player("§· xãa con " .. nCur .. " - cßn " .. PARTNER_Count() .. " con")
end

function BDH_A_FullTest()
	BDH_TestAPI()
	Msg2Player("§· ch¹y bé test - PASS/FAIL ghi ë bin/server/bdh_test.log")
end

-- [PETSYS 28/08] bo test he Ban Dong Hanh PC
Include("\\script\\petsys\\protocol_process_gs.lua")

function BDH_P_Root()
	SayEx({format("Pet: co=%d goi=%d cap=%d | diem %d/%d/%d/%d | xu %d",
		PET_IsCreate(), PET_IsSummon(), PET_GetLevel(),
		PET_GetUpgradePoint(), PET_GetGrownPoint(), PET_GetTamePoint(),
		PET_GetXiuzhenPoint(), GetTask(251)),
	"Cap 5 Thiep + 200 Thuoc + 20 moi loai trai/BDH_P_CapDo",
	"Cap 5 Bi kip + 6 trang bi/BDH_P_CapKN",
	"Bon diem = 9999 + 2000 chan nguyen + 500 xu/BDH_P_Diem",
	"Tao pet nhanh (khong can thiep)/BDH_P_TaoNhanh",
	"Xoa het ky nang da hoc/BDH_P_XoaKN",
	"Thao het trang bi/BDH_P_XoaTB",
	"Dong bo diem xuong client/BDH_P_DongBo",
	"Ket thuc doi thoai./no"})
end

-- [PETSYS 29/08] ban lai 29 o task pet xuong client (SetTask -> SetSaveVal
-- -> SyncTaskValueToClient) - do kenh dong bo / va tam khi login-sync truot
function BDH_P_TrangBi()
	local i
	for i = 4881, 4886 do
		AddItem(6, 1, i, 1, 0, 0)
	end
	Msg2Player("Da cap 6 trang bi Dong Hanh")
end
function BDH_P_BiKip()
	local i
	for i = 1, 5 do
		AddItem(6, 1, 4880, 1, 0, 0)
	end
	Msg2Player("Da cap 5 Bi kip Dong Hanh")
end
function BDH_P_ChanNguyen()
	SetTask(362, GetTask(362) + 2000)
	Msg2Player("Da cap 2000 diem chan nguyen (task 362) - tong: " .. GetTask(362))
end
function BDH_P_DongBo()
	local i
	for i = 5110, 5138 do
		SetTask(i, GetTask(i))
	end
	Msg2Player("Da ban lai 29 o diem pet - mo cua so Ban Dong Hanh xem")
end

function BDH_P_CapDo()
	local i
	for i = 1, 5 do
		AddItem(6, 1, 4874, 1, 0, 0)
	end
	for i = 1, 200 do
		AddItem(6, 1, 4875, 1, 0, 0)
	end
	for i = 1, 20 do
		AddItem(6, 1, 4876, 1, 0, 0)
		AddItem(6, 1, 4877, 1, 0, 0)
		AddItem(6, 1, 4878, 1, 0, 0)
		AddItem(6, 1, 4879, 1, 0, 0)
	end
	Msg2Player("Da cap 5 Thiep + 200 Thuoc + 20 moi loai trai")
end

function BDH_P_TaoNhanh()
	if (PET_IsCreate() == 1) then
		Msg2Player("§· cã pet råi.")
		return
	end
	SetTask(5124, random(1, 4))
	PET_Create(566, "C« B¹ch")
	PET_Summon()
	Msg2Player("§· t¹o + gäi pet (C« B¹ch, lo¹i aura " .. GetTask(5124) .. ")")
end

function BDH_P_Diem()
	PET_SetUpgradePoint(9999)
	PET_SetGrownPoint(9999)
	PET_SetTamePoint(9999)
	PET_SetXiuzhenPoint(9999)
	SetTask(362, GetTask(362) + 2000)
	SetTask(251, GetTask(251) + 500)
	Msg2Player("Da cap 4 diem 9999 + 2000 chan nguyen + 500 xu")
end

function BDH_P_Goi()
	PetSys_Protocol(2)
end

function BDH_P_Thu()
	PetSys_Protocol(3)
end

function BDH_P_LenCap()
	PetSys_Protocol(4)
end

function BDH_P_TuLuyen()
	PetSys_Protocol(7)
end

function BDH_P_NgoaiQuan()
	PetSys_Protocol(6)
end

function BDH_P_Xoa()
	PetSys_Protocol(1)
end

