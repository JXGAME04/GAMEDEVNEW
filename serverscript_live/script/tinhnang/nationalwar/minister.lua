-- ===========================================================================
-- DAI THAN QUOC GIA - hieu ung hao quang + ky nang
-- Port tu script\nationalwar\{head,nationalwar,login}.lua cua ban Linux.
-- Bang skill 953-962, tianzi.lua, skillstate.lua, 5 sprite hao quang va bang
-- Status152-156 DA CO SAN nguyen ven trong cay nay - chi thieu duong goi.
--
-- Chuc (khop field 51/52/53 cua bang, dat boi COP 34/35):
--   1 = Thua Tuong  (JX2 NWPOSITION_MINISTER)
--   2 = Nguyen Soai (NWPOSITION_MARSHAL)
--   3 = Tien Phong  (NWPOSITION_PIONEER)
-- ===========================================================================

NW_STATE_MINISTER = 955
NW_STATE_MARSHAL  = 954
NW_STATE_PIONEER  = 953
NW_SKILL_MINISTER = 960
NW_SKILL_MARSHAL  = 959
NW_SKILL_PIONEER  = 958

NW_DUTY_STATE = { NW_STATE_MINISTER, NW_STATE_MARSHAL, NW_STATE_PIONEER }
NW_DUTY_SKILL = { NW_SKILL_MINISTER, NW_SKILL_MARSHAL, NW_SKILL_PIONEER }

NW_STATE_TIME = 99999999

-- Go sach moi hao quang / ky nang dai than (ban goc lam y het truoc khi cap
-- lai: remove-then-add de khong chong tran cap thuoc tinh)
function nw_clear_duty_effect()
	local i = 1
	while (NW_DUTY_STATE[i] ~= nil) do
		RemoveSkillState(NW_DUTY_STATE[i])
		RemoveSkillState(NW_DUTY_SKILL[i])
		i = i + 1
	end
end

-- Cap hieu ung theo chuc dang giu (nDuty 1..3); nDuty = 0 chi go sach
function nw_apply_duty_effect(nDuty)
	nw_clear_duty_effect()
	if (nDuty == nil or nDuty < 1 or nDuty > 3) then
		return 0
	end
	-- ifMagic: 0 = chi gan TRANG THAI/hinh (hao quang), 1 = thi trien
	-- that (ky nang co thuoc tinh) - phan bien M1 chi ra ban dau dao nguoc
	AddSkillState(NW_DUTY_STATE[nDuty], 1, 0, NW_STATE_TIME)
	AddSkillState(NW_DUTY_SKILL[nDuty], 1, 1, NW_STATE_TIME)
	return 1
end

-- Goi luc dang nhap va moi khi doi chuc: doc chuc that tu bang (C++
-- GetTongDuty doc field 51/52/53 roi so voi NameID cua chinh minh)
function nw_refresh_duty()
	local nDuty = 0
	if (GetTongDuty ~= nil) then
		nDuty = GetTongDuty()
	end
	return nw_apply_duty_effect(nDuty)
end
