-- SINH TU DONG [BDH 27/08] - BO TEST TU DONG HE BAN DONG HANH (go sau khi nghiem thu)
-- Duoc timerserver.lua RunTime goi moi phut qua call() bao ve.
-- Trang thai: GetGlbValue(8891): 0 sinh bot -> 1 test API -> 2 sinh lai -> 3 kiem luu ben -> 9 xong
-- Ket qua ghi: bin\server\bdh_test.log

BDH_GLB_STATE = 8891
BDH_GLB_LEVEL = 8892
BDH_GLB_TICK  = 8893

function BDH_Log(sz)
	appendto("bdh_test.log")
	write(sz .. "\n")
	writeto()
end

function BDH_Check(szTen, bOk, szThem)
	if (bOk) then
		BDH_Log("PASS  " .. szTen .. "  " .. (szThem or ""))
		return 1
	end
	BDH_Log("FAIL  " .. szTen .. "  " .. (szThem or ""))
	return 0
end

function BDH_TimNguoiChoi()
	local i
	for i = 1, 128 do
		local szName = CallPlayerFunction(i, GetName)
		if (szName ~= nil and szName ~= "") then
			return i, szName
		end
	end
	return 0, ""
end

function BDH_TestAPI()
	-- chay trong ngu canh bot (PlayerIndex da doi)
	local n0 = PARTNER_Count()
	BDH_Log("Count truoc = " .. n0)
	if (n0 == 0) then
		local r = PARTNER_AddFightPartner(5, 0, 1, 5, 5, 5, 5, 5, 5)
		BDH_Check("AddFightPartner", r == 1, "tra " .. r)
	else
		BDH_Log("bot da co san " .. n0 .. " dong hanh (chay lai test)")
	end
	local cur, st = PARTNER_GetCurPartner()
	BDH_Check("GetCurPartner.cur", cur == 1, "cur=" .. cur .. " st=" .. st)
	local co = PARTNER_CallOutCurPartner(1)
	BDH_Check("CallOut", co == 1, "tra " .. co)
	local cur2, st2 = PARTNER_GetCurPartner()
	BDH_Check("CallOut.state", st2 == 1, "st=" .. st2)
	PARTNER_SetName(1, "BDHTest")
	local nm = PARTNER_GetName(1)
	BDH_Check("SetName/GetName", nm == "BDHTest", "ten=[" .. nm .. "]")
	local sidx = PARTNER_GetSettingIdx(1)
	BDH_Check("GetSettingIdx", sidx == 1060, "tpl=" .. sidx .. " (mong 1060 Kim Phong ky 1)")
	local sr = PARTNER_GetSeries(1)
	BDH_Check("GetSeries", sr == 0, "series=" .. sr)
	if (n0 == 0) then
		local lv0 = PARTNER_GetLevel(1)
		PARTNER_AddExp(1, 5000, 0)
		local lv1 = PARTNER_GetLevel(1)
		BDH_Check("AddExp5000->Level", lv1 == 7, "lv " .. lv0 .. " -> " .. lv1 .. " (mong 7)")
	else
		BDH_Log("bo qua AddExp (chay lai tren pet cu, exp cong don)")
	end
	PARTNER_SetTaskValue(1, 2, 7)
	local tv = PARTNER_GetTaskValue(1, 2)
	BDH_Check("Set/GetTaskValue", tv == 7, "tv=" .. tv)
	PARTNER_SetTaskValue(1, 2, 0)
	PARTNER_AddSkill(1, 2, 594, 3, 0)
	local sl, se = PARTNER_GetSkillInfo(1, 594)
	BDH_Check("AddSkill/GetSkillInfo", sl == 3, "lv=" .. sl .. " exp=" .. se)
	local a1, a2, a3, a4, a5, a6 = PARTNER_GetAttribs(1)
	BDH_Check("GetAttribs.life", a1 ~= nil and a1 >= 14000, "life=" .. (a1 or -1) ..
		" str=" .. (a2 or -1) .. " hit=" .. (a3 or -1) .. " def=" .. (a4 or -1) ..
		" spd=" .. (a5 or -1) .. " luck=" .. (a6 or -1))
	local emo = PARTNER_GetEmotionDegree(1)
	BDH_Check("Emotion.init", emo == 50, "emo=" .. emo)
	local eg = PARTNER_GetEndure()
	BDH_Log("Endure(%mau npc) = " .. eg)
	SetGlbValue(BDH_GLB_LEVEL, PARTNER_GetLevel(1))
	BDH_Log("--- API xong, cho luu + relog bot ---")
	return 1
end

function BDH_TestPersist()
	local n = PARTNER_Count()
	BDH_Check("Persist.Count", n == 1, "count=" .. n)
	local lv = PARTNER_GetLevel(1)
	local lvLuu = GetGlbValue(BDH_GLB_LEVEL)
	if (lvLuu == 0) then
		lvLuu = lv
	end
	BDH_Check("Persist.Level", lv == lvLuu, "lv=" .. lv .. " mong=" .. lvLuu)
	local nm = PARTNER_GetName(1)
	BDH_Check("Persist.Name", nm == "BDHTest", "ten=[" .. nm .. "]")
	local sl, se = PARTNER_GetSkillInfo(1, 594)
	BDH_Check("Persist.Skill594", sl == 3, "lv=" .. sl)
	local sr = PARTNER_GetSeries(1)
	BDH_Check("Persist.Series", sr == 0, "series=" .. sr)
	BDH_Log("=== TEST BAN DONG HANH KET THUC ===")
	return 1
end

function BDH_TestTick()
	local nState = GetGlbValue(BDH_GLB_STATE)
	if (nState >= 9) then
		return
	end
	local nTick = GetGlbValue(BDH_GLB_TICK)
	SetGlbValue(BDH_GLB_TICK, nTick + 1)
	if (nState == 0) then
		BDH_Log("=== BAT DAU TEST BDH (tick " .. nTick .. ") ===")
		local nXep = PB_AddBot(1, 1)
		BDH_Log("PB_AddBot(1,1) xep hang = " .. nXep .. ", bot song = " .. PB_BotCount())
		SetGlbValue(BDH_GLB_STATE, 1)
		return
	end
	if (nState == 1) then
		local idx, szName = BDH_TimNguoiChoi()
		if (idx == 0) then
			BDH_Log("tick " .. nTick .. ": chua thay nguoi choi/bot nao (bot song=" .. PB_BotCount() .. ")")
			if (nTick > 6) then
				BDH_Log("QUA 6 phut khong co bot - DUNG TEST (kiem tai khoan bot)")
				SetGlbValue(BDH_GLB_STATE, 9)
			end
			return
		end
		BDH_Log("dung nguoi choi idx=" .. idx .. " ten=[" .. szName .. "]")
		CallPlayerFunction(idx, BDH_TestAPI)
		PB_SaveAll()
		PB_ClearBot()
		BDH_Log("da PB_SaveAll + PB_ClearBot, cho phut sau sinh lai")
		SetGlbValue(BDH_GLB_STATE, 2)
		return
	end
	if (nState == 2) then
		local nXep = PB_AddBot(1, 1)
		BDH_Log("sinh lai bot: xep hang = " .. nXep)
		SetGlbValue(BDH_GLB_STATE, 3)
		return
	end
	if (nState == 3) then
		local idx, szName = BDH_TimNguoiChoi()
		if (idx == 0) then
			BDH_Log("tick " .. nTick .. ": cho bot relog...")
			if (nTick > 12) then
				SetGlbValue(BDH_GLB_STATE, 9)
				BDH_Log("QUA HAN cho relog - DUNG TEST")
			end
			return
		end
		CallPlayerFunction(idx, BDH_TestPersist)
		PB_ClearBot()
		SetGlbValue(BDH_GLB_STATE, 9)
		return
	end
end

-- kiem ban do partner da nap chua (log 1 lan moi phien)
if (BDH_DaKiemMap == nil) then
	BDH_DaKiemMap = 1
	BDH_Log("MAPCHECK 512=" .. SubWorldID2Idx(512) .. " 513=" .. SubWorldID2Idx(513) .. " 514=" .. SubWorldID2Idx(514) .. " 515=" .. SubWorldID2Idx(515) .. " 539=" .. SubWorldID2Idx(539))
end

-- [BDH-G4] kiem duong DOI THOAI (nut Talk): nap partner_talk.lua + compat task_main
-- trong ngu canh bot roi goi main() - dung duong ma c2s_partnerop op TALK se chay.
function BDH_TestTalk()
	local r = dofile("script/partner/partner_talk.lua")
	if (main == nil) then
		BDH_Log("FAIL  Talk.dofile  khong thay ham main")
		return 0
	end
	PARTNER_CallOutCurPartner(1)
	BDH_TalkLoi = 0
	local cuErr = _ERRORMESSAGE
	_ERRORMESSAGE = function(m)
		BDH_TalkLoi = 1
		BDH_Log("FAIL  Talk.main  " .. tostring(m))
	end
	dostring("main()")
	_ERRORMESSAGE = cuErr
	if (BDH_TalkLoi == 0) then
		BDH_Log("PASS  Talk.main  doi thoai partner_talk chay khong loi")
	end
	return 1
end

function BDH_TalkTick()
	local nS = GetGlbValue(8899)
	if (nS >= 9) then
		return
	end
	if (GetGlbValue(8891) < 9) then
		return
	end
	if (nS == 0) then
		PB_AddBot(1, 1)
		BDH_Log("=== TEST TALK: sinh bot ===")
		SetGlbValue(8899, 1)
		return
	end
	if (nS == 1) then
		local idx, szName = BDH_TimNguoiChoi()
		if (idx == 0) then
			return
		end
		CallPlayerFunction(idx, BDH_TestTalk)
		PB_ClearBot()
		SetGlbValue(8899, 9)
		BDH_Log("=== TEST TALK XONG ===")
		return
	end
end

-- [BDH-GM] TU CAP 1 dong hanh cho NGUOI CHOI THAT dau tien dang nhap (de chu game
-- nghiem thu bang mat). Chi cap 1 lan (GLB 8890), bo qua bot LyHieu1 va ai da co pet.
-- Cap kem tui cap 2 (2x3 o) de test keo-tha. GO SAU KHI NGHIEM THU.
function BDH_GmCapThu()
	if (PARTNER_Count() ~= 0) then
		return 0
	end
	local r = PARTNER_AddFightPartner(5, 0, 1, 5, 5, 5, 5, 5, 5)
	if (r == 1) then
		SetPartnerBagLevel(2)
		Msg2Player("Da cap 1 Ban Dong Hanh de test. Bam P mo thanh nhanh, Y thuoc tinh, I ky nang, U tui, B goi ra, L doi thoai.")
		BDH_Log("GM: da cap dong hanh + tui cap 2 cho [" .. GetName() .. "]")
	end
	return r
end

function BDH_GmTick()
	do return end	-- [28/08] DA TAT theo yeu cau chu game - khong tu cap pet
	if (GetGlbValue(8890) >= 9) then
		return
	end
	local i
	for i = 1, 128 do
		local szName = CallPlayerFunction(i, GetName)
		if (szName ~= nil and szName ~= "" and szName ~= "LyHieu1") then
			local r = CallPlayerFunction(i, BDH_GmCapThu)
			if (r == 1) then
				BDH_Log("=== GM CAP THU cho [" .. szName .. "] ===")
				SetGlbValue(8890, 9)
				return
			end
		end
	end
end

-- [PETSYS 28/08] test he Ban Dong Hanh PC (C-API PET_* + protocol) - GO SAU NGHIEM THU
function BDH_TestPet()
	local r = PET_Create(566, "PetTest")
	BDH_Check("Pet.Create", r == 1 and PET_IsCreate() == 1, "tra " .. r)
	local s1 = PET_Summon()
	BDH_Check("Pet.Summon", s1 == 1 and PET_IsSummon() == 1, "tra " .. s1)
	PET_SetUpgradePoint(43)
	local dbg4 = ""
	local kq
	for kq = 5090, 5199 do
		if (GetTask(kq) == 43) then
			dbg4 = dbg4 .. kq .. ","
		end
	end
	BDH_Log("pet dbg5: addr SetLevel=" .. tostring(PET_SetLevel) .. " SetUp=" .. tostring(PET_SetUpgradePoint) .. " GetUp=" .. tostring(PET_GetUpgradePoint) .. " SetGrown=" .. tostring(PET_SetGrownPoint))
	BDH_Log("pet dbg4: C_Set(43) roi vao cac o: [" .. dbg4 .. "] C_Get=" .. PET_GetUpgradePoint())
	PET_SetGrownPoint(80)
	PET_SetTamePoint(125)
	BDH_Check("Pet.Points", PET_GetUpgradePoint() == 40 and PET_GetGrownPoint() == 80 and PET_GetTamePoint() == 125, "doc=" .. PET_GetUpgradePoint() .. "/" .. PET_GetGrownPoint() .. "/" .. PET_GetTamePoint())
	PET_SetSkill(1, 2)
	PET_SetLevel(11)
	BDH_Check("Pet.SkillLevel", PET_GetSkill(1) == 2 and PET_GetLevel() == 11, "loai=2 lv=11")
	PET_ClearAttrib()
	PET_AddAttrib(1, 7)
	PET_AddAttrib(5, 250)
	BDH_Check("Pet.Attrib", PET_GetAttrib(1) == 7 and PET_GetAttrib(5) == 250, "str=7 hp=250")
	PET_SetName("MeoMeo")
	BDH_Check("Pet.Name", PET_GetName() == "MeoMeo", "ten=[" .. PET_GetName() .. "]")
	-- duong protocol that: op 3 = UnSummon (qua protocol_process_gs.lua)
	dofile("script/petsys/protocol_process_gs.lua")
	BDH_Log("pet dbg: type Protocol=" .. type(PetSys_Protocol) .. " type PetSys=" .. type(PetSys) .. " summon truoc=" .. PET_IsSummon())
	PetSys_Protocol(3)
	BDH_Check("Pet.ProtocolUnSummon", PET_IsSummon() == 0, "sau op3 summon=" .. PET_IsSummon())
	SetGlbValue(8888, PET_GetLevel())
	BDH_Log("--- PET api xong, cho relog kiem luu ben ---")
	return 1
end

function BDH_TestPetPersist()
	BDH_Check("PetPersist.Create", PET_IsCreate() == 1, "")
	BDH_Check("PetPersist.Level", PET_GetLevel() == GetGlbValue(8888), "lv=" .. PET_GetLevel())
	BDH_Check("PetPersist.Name", PET_GetName() == "MeoMeo", "ten=[" .. PET_GetName() .. "]")
	BDH_Check("PetPersist.Point", PET_GetGrownPoint() == 80, "grown=" .. PET_GetGrownPoint())
	BDH_Log("=== TEST PETSYS KET THUC ===")
	return 1
end

function BDH_PetTick()
	local nS = GetGlbValue(8883)
	if (nS >= 9) then
		return
	end
	if (GetGlbValue(8891) < 9) then
		return
	end
	if (nS == 0) then
		PB_AddBot(1, 1)
		BDH_Log("=== TEST PETSYS: sinh bot ===")
		SetGlbValue(8883, 1)
		return
	end
	if (nS == 1) then
		local idx = BDH_TimNguoiChoi()
		if (idx == 0) then
			return
		end
		CallPlayerFunction(idx, BDH_TestPet)
		PB_SaveAll()
		PB_ClearBot()
		SetGlbValue(8883, 2)
		return
	end
	if (nS == 2) then
		PB_AddBot(1, 1)
		SetGlbValue(8883, 3)
		return
	end
	if (nS == 3) then
		local idx = BDH_TimNguoiChoi()
		if (idx == 0) then
			return
		end
		CallPlayerFunction(idx, BDH_TestPetPersist)
		PB_ClearBot()
		SetGlbValue(8883, 9)
		return
	end
end
