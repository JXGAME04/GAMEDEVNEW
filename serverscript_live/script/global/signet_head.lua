-- ================================================================================================
-- [HE THONG] script/global/signet_head.lua
-- Muc dich  : An (signet).
-- Duoc nap  : Include tu 1 tep (vd bossdeath.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Ham (dong): getSignetDropRate (5), IsDoubledMoneyBox (15), getSignetYMD (22)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
IncludeLib("TONG")
TASKID_MONEYBOX_LASTTIME = 2446	--ÌÕÖìÇ®´üÐ§Á¦½ØÖ¹Ê±¼ä
TONGTSK_MONEYBOX_DROPMAX = 1035	--ÌÕÖìÇ®´ü·­±¶µôÂäboss±¦Ó¡µÄÏÞÖÆ
SIGNET_MAXDROP = 180

function getSignetDropRate(nCnt)
	local rate = random(14,18) * nCnt;
	local resid = mod(rate, 10);
	if (resid > 5) then
		return ceil(rate / 10);
	else
		return floor(rate / 10);
	end;
end;

function IsDoubledMoneyBox(nTongID)
	if (floor(GetCurServerTime()/60) < GetTask(TASKID_MONEYBOX_LASTTIME) and TONG_GetTaskValue(nTongID, TONGTSK_MONEYBOX_DROPMAX) < SIGNET_MAXDROP) then
		return 1
	end
	return nil
end

function getSignetYMD(nDay)
	nDay = floor(nDay/ 1000000)
	local nDate = mod(nDay, 100)
	local nMonth = mod(floor(nDay / 100), 100)
	local nYear = floor(nDay / 10000)
	return nYear, nMonth, nDate
end
