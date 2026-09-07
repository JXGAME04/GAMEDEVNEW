-- ================================================================================================
-- [HE THONG] script/lib/pay.lua
-- Muc dich  : Diem mo rong (extpoint) - goc JX2.
-- Duoc nap  : Include tu 6 tep (vd class.lua, head.lua, huangzhendan.lua, xuezhanlingqi.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Ham (dong): IsCharged (2)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
--有关扩展点的库
function IsCharged()
	if( GetExtPoint( 0 ) >= 1 ) then
		return 1;
	else
		return 0;
	end;
end;
