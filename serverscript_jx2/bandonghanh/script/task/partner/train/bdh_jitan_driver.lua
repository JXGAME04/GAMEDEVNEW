-- SINH TU DONG [BDH-G5] bdh_jitan_driver.lua - te dan Truong Ca Mon thuc tinh gio chan
-- Goc: partner_jitan.lua chi DOC gb task; ban Linux thieu driver -> viet theo mo ta
-- trong thoai NPC ("moi gio chan linh luc te dan thuc tinh, dung 1 lan/toan server").
Include("\\script\\lib\\gb_taskfuncs.lua")

BDH_JITAN_KEY = "changgemen_jitan_bdh"

function BDH_JitanTick()
	local nYr, nMo, nDy, nHr, nMi = GetTimeNow()
	if (mod(nHr, 2) ~= 0) then
		return
	end
	if (nMi ~= 0) then
		return
	end
	local i
	for i = 1, 5 do
		gb_SetTask(BDH_JITAN_KEY, i, 1)
	end
end

-- do tam 28/08 (GO SAU): in dia chi ham PET_ mot lan de doi chieu no-op
if (BDH_DaDoDiaChi == nil) then
	BDH_DaDoDiaChi = 1
	appendto("bdh_test.log")
	write("pet dbg5: SetLevel=" .. tostring(PET_SetLevel) .. " GetLevel=" .. tostring(PET_GetLevel) .. " SetUp=" .. tostring(PET_SetUpgradePoint) .. " GetUp=" .. tostring(PET_GetUpgradePoint) .. " SetGrown=" .. tostring(PET_SetGrownPoint))
	write("het-dong")
	writeto()
end
