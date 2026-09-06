-- ============================================================================
-- HD3_THUYENPHUBAC.LUA - thuyen phu BO BAC Phong Lang Do.
-- SINH boi ReverseTools/port_3hd/thicong/c47_thuyenphu_bobac.py - DUNG SUA TAY.
--
-- Ban LINUX KHONG co NPC o bo Bac: huichengfu.lua:12-14 chi biet BA ben
-- (1149,3020) (1280,2909) (1538,2808) = bo NAM, con fld_head.lua:15
-- northMAP_POS chi la CHO CAP BEN khi het gio / chet. Du an cu co NPC bo Bac
-- (thuyenphubac.lua) lam nhiem vu dua khach ve bo Nam - giu lai dung phan do.
-- KHONG giu phan tra nhiem vu doi Truy Cong Lenh cua ban Viet: he Bac Dau cua
-- Linux can nguoi choi MANG THEO Truy Cong Lenh (khong tru) nen tru la hong.
-- ============================================================================
Include("\\script\\header\\cauhinh_hoatdong.lua")

-- ben Nam tuong ung (dung so hieu ben) - lay tu huichengfu.lua ban Linux
HD3_PLD_BENNAM_POS = { {1149, 3020}, {1280, 2909}, {1538, 2808} }
HD3_PLD_PHI_VENAM = 1000	-- MONEY_VETHANH cua du an cu, giu nguyen

function main(NpcIndex)
	HD3_TPB_BEN = GetNpcValue(NpcIndex)
	if (HD3_TPB_BEN == nil or HD3_TPB_BEN < 1 or HD3_TPB_BEN > 3) then
		HD3_TPB_BEN = 1
	end
	local nPhi = HD_CFG("HD3_PLD_PHI_VENAM", HD3_PLD_PHI_VENAM)
	local tb = {
		format("§­a ta trë vÒ bê Nam (%d quan tiÒn)/hd3_tpb_venam", nPhi),
		"KÕt thóc ®èi tho¹i/hd3_tpb_no",
	}
	Say("<color=green>ThuyÒn phu:<color> ®©y lµ bê B¾c Phong L¨ng §é. ThuyÒn cña ta chØ ®­a kh¸ch trë l¹i bê Nam th«i.", getn(tb), tb)
end

function hd3_tpb_venam()
	local nPhi = HD_CFG("HD3_PLD_PHI_VENAM", HD3_PLD_PHI_VENAM)
	if (GetCash() < nPhi) then
		Talk(1, "", format("CÇn %d quan tiÒn míi qua s«ng ®­îc, h·y quay l¹i khi ®ñ tiÒn.", nPhi))
		return
	end
	local nBen = HD3_TPB_BEN
	if (nBen == nil or nBen < 1 or nBen > 3) then nBen = 1 end
	Pay(nPhi)
	NewWorld(336, HD3_PLD_BENNAM_POS[nBen][1], HD3_PLD_BENNAM_POS[nBen][2])
	SetFightState(1)
	SetDeathScript("")
	SetLogoutRV(0)
end

function hd3_tpb_no()
end
