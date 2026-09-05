Include("\\script\\chienlenh\\cl_def.lua")	-- [CL 04/09] so hieu nhiem vu Chien Lenh
--Author: Fong Ki“u
--Date: 2021
--Function: Boss Æπi hoµng kim ch’t

Include("\\script\\lib\\lib_task.lua")
-- [CFGBHK 30/08] hai tep duoi day la LA (khong Include gi).
Include("\\script\\cauhinh\\ch_lib.lua")
Include("\\script\\cauhinh\\ch_thuong.lua")
-- [CFGBHK 30/08] Bo doc cau hinh cho tep nay. Tra ve MAC DINH (= so cu)
-- khi bo cau hinh chua nap, nen kem nhat cung khong the doi hanh vi.
function BHK_CFG(szKhoa, macdinh)
	if (G_CFG ~= nil) then
		return G_CFG(szKhoa, macdinh)
	end
	return macdinh
end

Include("\\script\\lib\\lib_server.lua")
Include("\\script\\lib\\lib_ham.lua")

KILLBOSSEXPAWARD 			= BHK_CFG("BHK_EXP_GIET_BOSS_DAI", 25000000) * EXP_RATE
KILLBOSSNEAREXPAWARD 	= BHK_CFG("BHK_EXP_LANCAN_BOSS_DAI", 10000000) * EXP_RATE
PHAMVI_HUONGEXP 				= BHK_CFG("BHK_PHAMVI_HUONG_EXP_DAI", 200)

function OnDeath(nNpcIndex, nDamageIndex)
	DelNpc(nNpcIndex)
	local nPlayerIndex = NpcIdx2PIdx(nDamageIndex)
	if not nPlayerIndex or nPlayerIndex <= 0 then return end
	
	PlayerIndex = nPlayerIndex	
	CL_Xong(CL_NV.BOSS_GIET)	-- [CL 04/09] BOSS Hoang Kim: ke ket lieu
	local wb, xb, yb = GetWorldPos(nNpcIndex)
	local nTeamId = GetTeam()
	local nCaptain = PlayerIndex
	local nExp = GetTongExp()
	local naddtungphaiList = {253, 258, 263, 239, 243, 248, 313, 308, 318, 298, 303, 268, 276, 283, 288, 331, 333, 338, 343, 353, 358, 363, 368}

	local randIndex = random(1, getn(naddtungphaiList))  
	local itemid = naddtungphaiList[randIndex]  
	for i = 1, GetPlayerCount() do
		PlayerIndex = i
		local wn, xn, yn = GetWorldPos()
		local szName = GetName()
		if szName and szName ~= "" then
			if (nTeamId == nil and i == nPlayerIndex) then --ng≠Íi kill boss kh´ng c„ tÊ ÆÈi
				AddSumExp(KILLBOSSEXPAWARD)
				AddItemSL(4850,3,0) -- ruong trang bi xanh
			         
				-- AddItemSL(itemid, 1, 0)   -- manh do pho hkmp 
				SetTongExp(nExp + 50)
				UpdateTongExpAndLevel()
				Msg2Player("Bπn chÿ s®n boss 1 m◊nh, nhÀn Æ≠Óc "..KILLBOSSEXPAWARD.." kinh nghi÷m")
			elseif (nTeamId ~= nil and GetTeam() == nTeamId and wb == wn and abs(xb - xn) < PHAMVI_HUONGEXP and abs(yb - yn) < PHAMVI_HUONGEXP) then --trong cÔng tÊ ÆÈi
				AddSumExp(KILLBOSSEXPAWARD)
				Msg2Player("Bπn thuÈc nh„m ti™u di÷t Boss, nhÀn Æ≠Óc "..KILLBOSSEXPAWARD.." kinh nghi÷m")
			
			elseif (wb == wn and abs(xb - xn) < PHAMVI_HUONGEXP and abs(yb - yn) < PHAMVI_HUONGEXP) then --nh˜ng ng≠Íi l©n cÀn
				AddSumExp(KILLBOSSNEAREXPAWARD)
				Msg2Player("Bπn Î trong khu v˘c s®n boss, nhÀn Æ≠Óc "..KILLBOSSNEAREXPAWARD.."  kinh nghi÷m")
			end
		end
	end

	PlayerIndex = nCaptain
	-- UpdateTongExpAndLevel()
end

function OnRevive(nNpcIndex)
		local Series = GetNpcSeries(nNpcIndex)
	local nSTVL, nDoc, nBang, nHoa, nLoi = 0,0,0,0,0
	if(Series==0) then	--KIM
		SetNpcSkill(NpcIndex, 321, 20, 1);
		SetNpcSkill(NpcIndex, 319, 20, 2);
		SetNpcSkill(NpcIndex, 322, 20, 3);
		SetNpcSkill(NpcIndex, 325, 20, 4);
		nSTVL = 500
	elseif(Series==1) then	--MOC
		SetNpcSkill(NpcIndex, 339, 50, 1);
		SetNpcSkill(NpcIndex, 342, 50, 2);
		SetNpcSkill(NpcIndex, 302, 50, 3);
		SetNpcSkill(NpcIndex, 390, 50, 4);
		nDoc = 500
	elseif(Series==2) then	--THUY
		SetNpcSkill(NpcIndex, 328, 20, 1);
		SetNpcSkill(NpcIndex, 380, 20, 2);
		SetNpcSkill(NpcIndex, 336, 20, 3);
		SetNpcSkill(NpcIndex, 337, 20, 4);
		nBang = 500
	elseif(Series==3) then	--HOA
		SetNpcSkill(NpcIndex, 357, 20, 1);
		SetNpcSkill(NpcIndex, 359, 20, 2);
		SetNpcSkill(NpcIndex, 361, 20, 3);
		SetNpcSkill(NpcIndex, 362, 20, 4);
		nHoa = 500
	elseif(Series==4) then	--THO
		SetNpcSkill(NpcIndex, 365, 20, 1);
		SetNpcSkill(NpcIndex, 368, 20, 2);
		SetNpcSkill(NpcIndex, 372, 20, 3);
		SetNpcSkill(NpcIndex, 375, 20, 4);
		nLoi = 500
	else					--KHONG CO HE, truong hop nay la add sai hay sao do
		--SetNpcSkill(nNpcIndex, 1055, 20, 1)
		--SetNpcSkill(nNpcIndex, 1057, 20, 2)
		--SetNpcSkill(nNpcIndex, 1058, 20, 3)
		--SetNpcSkill(nNpcIndex, 1060, 20, 4)
		nSTVL = 500
	end
		--SetNpcReplenish(nNpcIndex,1);--phuc hoi sinh luc	
		SetNpcSpeed(nNpcIndex, 10)--toc do di chuyen tang len
     --   AddNpcSkillState(nNpcIndex, 16, 15, 1, 60*18*30)
		SetNpcHitRecover(nNpcIndex,100);--cao nhat la 100(ko giat) --tuy cam hung
		SetNpcActiveRange(nNpcIndex, 600)
		SetNpcDmgEx(nNpcIndex,nSTVL, nDoc, nBang, nHoa, nLoi ,0)
		SetNpcDmgEx(nNpcIndex,nSTVL, nDoc, nBang, nHoa, nLoi ,1)
		SetNpcResist(nNpcIndex, 95, 95, 95, 95, 95)--khang' cac loai
		SetNpcBoss(nNpcIndex, 5)
end

function OnTimer(nNpcIndex)
	DelNpc(nNpcIndex)
end
	