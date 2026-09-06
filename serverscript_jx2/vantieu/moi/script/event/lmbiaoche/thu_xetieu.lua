-- [LMBC 06/09] BO THU AI BAM CHU cua xe tieu Long Mon Tieu Cuc.
-- Muc dich: kiem 6 ham engine moi va hanh vi bam chu TRUOC khi co du NPC / vat pham.
-- Goi tu lenh bai admin hoac dong lenh GM:
--   DynamicExecute("\\script\\event\\lmbiaoche\\thu_xetieu.lua", "XT_Tao")
--
-- XT_Tao()   sinh mot xe tieu 10 sao ngay canh minh (mau NPC 2233 da co san)
-- XT_ViTri() hoi vi tri xe          XT_Song() hoi xe con song khong
-- XT_Xoa()   xoa xe                 XT_Tat()/XT_Bat() cong tat nong he xe tieu

XT_NPC_XE_BANG = 2233

function XT_Tao()
	if (IsBiaoCheAlive() == 1) then
		Talk(1, "", "Ng­¬i ®ang cã Tiªu Xa råi, h·y xo¸ tr­íc ®·.")
		return
	end
	local szTen = format("Tiªu Xa cña %s", GetName())
	local nIdx = CreateBiaoChe(random(1, 4), XT_NPC_XE_BANG, 95, szTen, 30 * 60 * 18)
	if (nIdx == nil or nIdx == 0) then
		Talk(1, "", "Kh«ng t¹o ®­îc Tiªu Xa. Xem nhËt ký m¸y chñ.")
		return
	end
	SetNpcScript(nIdx, "\\script\\event\\lmbiaoche\\thu_xetieu.lua")
	Msg2Player(format("§· t¹o Tiªu Xa, chØ sè NPC = %d", nIdx))
end

function XT_ViTri()
	local nX, nY, nMap = GetBiaoChePos()
	if (nMap == nil or nMap == -1) then
		Talk(1, "", "Kh«ng cã Tiªu Xa nµo.")
		return
	end
	Msg2Player(format("Tiªu Xa ë b¶n ®å %d (%d,%d)", nMap, floor(nX / 32), floor(nY / 32)))
end

function XT_Song()
	if (IsBiaoCheAlive() == 1) then
		Msg2Player("Tiªu Xa cßn sèng.")
	else
		Msg2Player("Kh«ng cã Tiªu Xa hoÆc xe ®· bÞ ph¸.")
	end
end

function XT_Xoa()
	if (DeleteBiaoChe() == 1) then
		Msg2Player("§· xo¸ Tiªu Xa.")
	else
		Msg2Player("Kh«ng cã Tiªu Xa ®Ó xo¸.")
	end
end

function XT_Tat()
	BC_SetEnable(0)
	Msg2Player("§· t¾t hÖ Tiªu Xa.")
end

function XT_Bat()
	BC_SetEnable(1)
	Msg2Player("§· bËt hÖ Tiªu Xa.")
end

-- ==== ba ham engine goi nguoc: dat o day de xac nhan chung co chay ====
function OnBiaoCheDisapper(nNpcIndex, szPlayerName)
	WriteYunBiaoLog(format("[THU] OnBiaoCheDisapper npc=%d chu=%s", nNpcIndex, szPlayerName))
end

function OnBiaoCheFarAwayPlayerDisapper(nNpcIndex, szPlayerName)
	WriteYunBiaoLog(format("[THU] OnBiaoCheFarAwayPlayerDisapper npc=%d chu=%s", nNpcIndex, szPlayerName))
end

function OnBiaoCheChangeMapNotice()
	Msg2Player("Tiªu Xa ®· theo ng­¬i sang b¶n ®å míi.")
end

function OnDeath(nNpcIndex)
	SyncBiaoCheDeathInfoToRelay(nNpcIndex)
	WriteYunBiaoLog(format("[THU] xe %d bi pha", nNpcIndex))
end

function OnRevive()
end
