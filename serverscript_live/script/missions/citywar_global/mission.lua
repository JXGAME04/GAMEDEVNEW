-- [CFGCTC 30/08] hai tep duoi day la LA (khong Include gi).
Include("\\script\\cauhinh\\ch_lib.lua")
Include("\\script\\cauhinh\\ch_lich.lua")
-- [CFGCTC 30/08] Bo doc cau hinh cho tep nay. Tra ve MAC DINH (= so cu)
-- khi bo cau hinh chua nap, nen kem nhat cung khong the doi hanh vi.
function CTC_CFG(szKhoa, macdinh)
	if (G_CFG ~= nil) then
		return G_CFG(szKhoa, macdinh)
	end
	return macdinh
end

FRAME2TIME = 18;
INTERVAL = CTC_CFG("CTC_CHUKY_QUET_MO_TRAN_PHUT", 5) * 60 * FRAME2TIME;  --5∑÷÷”“ª¥Œ

function InitMission()
	SetGlbMissionV(1, 1);
	StartGlbMSTimer(8, 18, INTERVAL);
end;

function RunMission()

end;

function EndMission()
	StopGlbMSTimer(8, 18);
end;
