--Author: Fong Ki“u
--Date: 2021
--Function: Boss ti”u hoµng kim ch’t

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

KILLBOSSEXPAWARD 			= BHK_CFG("BHK_EXP_GIET_BOSS_TIEU", 20000000) * EXP_RATE
KILLBOSSNEAREXPAWARD 	= BHK_CFG("BHK_EXP_LANCAN_BOSS_TIEU", 10000000) * EXP_RATE
PHAMVI_HUONGEXP 				= BHK_CFG("BHK_PHAMVI_HUONG_EXP_TIEU", 200)

function OnDeath(nNpcIndex, nDamageIndex)
	DelNpc(nNpcIndex)	
	local nPlayerIndex = NpcIdx2PIdx(nDamageIndex)
	if not nPlayerIndex or nPlayerIndex <= 0 then return end
	
	PlayerIndex = nPlayerIndex	
	local wb, xb, yb = GetWorldPos(nNpcIndex)
	local nTeamId = GetTeam()
	local nCaptain = PlayerIndex
	local nExp = GetTongExp()
	local naddtungphaiList = {253, 258, 263, 239, 243, 248, 313, 308, 318, 298, 303, 268, 276, 283, 288, 331, 333, 338, 343, 353, 358, 363, 368}
	local randIndex = random(1, getn(naddtungphaiList))  
	local itemid = naddtungphaiList[randIndex]  
	for i=1, GetPlayerCount() do
		PlayerIndex = i
		local wn, xn, yn = GetWorldPos()
		local szName = GetName()
		if szName and szName ~= "" then
			if (nTeamId == nil and i == nPlayerIndex) then --ng≠Íi kill boss kh´ng c„ tÊ ÆÈi
				AddSumExp(KILLBOSSEXPAWARD)
				AddItemSL(4850,2,0) -- ruong trang bi xanh
				AddItemSL(itemid, 1, 0)   -- manh do pho hkmp 
				SetTongExp(nExp + 20)		
				UpdateTongExpAndLevel()
				Msg2Player("Bπn chÿ s®n boss 1 m◊nh, nhÀn Æ≠Óc "..KILLBOSSEXPAWARD.." kinh nghi÷m")
			elseif (nTeamId ~= nil and GetTeam() == nTeamId and wb == wn and abs(xb-xn) < PHAMVI_HUONGEXP and abs(yb-yn) < PHAMVI_HUONGEXP) then --trong cÔng tÊ ÆÈi
				AddSumExp(KILLBOSSEXPAWARD)
				--Msg2SubWorld("ßπi hi÷p ["..GetName().."] Æ∑ Æ¨n th©n ti™u di÷t boss, nhÀn Æ≠Óc "..KILLBOSSEXPAWARD.." kinh nghi÷m ")
				Msg2Player("Bπn thuÈc nh„m ti™u di÷t Boss, nhÀn Æ≠Óc "..KILLBOSSEXPAWARD.." kinh nghi÷m")
			else	--kh´ng cÔng tÊ ÆÈi
				if(wb == wn and abs(xb-xn) < PHAMVI_HUONGEXP and abs(yb-yn) < PHAMVI_HUONGEXP) then --l©n cÀn
					AddSumExp(KILLBOSSNEAREXPAWARD)
					Msg2Player("Bπn Î trong khu v˘c s®n boss, nhÀn Æ≠Óc "..KILLBOSSNEAREXPAWARD.."  kinh nghi÷m")
				end				
			end
		end
	end
	PlayerIndex = nCaptain
	-- UpdateTongExpAndLevel()
end

function OnRevive(nNpcIndex)
	local Series = GetNpcSeries(nNpcIndex)
	local nSTVL, nDoc, nBang, nHoa, nLoi = 0,0,0,0,0
	if(Series==0) then		--KIM
		nSTVL = 500 * DAMAGE_UPPER_BOSS
	elseif(Series==1) then	--MOC
		nDoc = 200 * DAMAGE_UPPER_BOSS
	elseif(Series==2) then	--THUY
		--SetNpcSkill(nNpcIndex, 88, 1, 1)
		--SetNpcSkill(nNpcIndex, 88, 1, 2)
		--SetNpcSkill(nNpcIndex, 88, 1, 3)
		--SetNpcSkill(nNpcIndex, 88, 1, 4)
		nBang = 500 * DAMAGE_UPPER_BOSS
	elseif(Series==3) then	--HOA
		nHoa = 500 * DAMAGE_UPPER_BOSS
	elseif(Series==4) then	--THO
		nLoi = 500 * DAMAGE_UPPER_BOSS
	else		--KHONG CO HE, truong hop nay la add sai hay sao do
		nSTVL = 500 * DAMAGE_UPPER_BOSS
	end
	SetNpcActiveRange(nNpcIndex, 600)
	SetNpcHitRecover(nNpcIndex,100);--cao nhat la 100(ko giat) --tuy cam hung
	SetNpcDmgEx(nNpcIndex,nSTVL, nDoc, nBang, nHoa, nLoi ,0)
	SetNpcDmgEx(nNpcIndex,nSTVL, nDoc, nBang, nHoa, nLoi ,1)
	SetNpcResist(nNpcIndex, 75, 75, 75, 75, 75)--khang' cac loai
	SetNpcBoss(nNpcIndex, 3)
end

function OnTimer(nNpcIndex)
	DelNpc(nNpcIndex)
end
	