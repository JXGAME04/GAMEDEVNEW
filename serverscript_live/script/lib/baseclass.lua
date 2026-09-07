-- ================================================================================================
-- [HE THONG] script/lib/baseclass.lua
-- Muc dich  : LOP CO SO kieu JX2: ke thua qua tag method (nay la metatable trong lua4compat).
-- Duoc nap  : Include tu 14 tep (vd dailytask.lua, g_activity.lua, g_dialog.lua, class.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Include   : common.lua
-- Ham (dong): tbBaseClass:new (21), tbBaseClass:NewClass (35)
-- Sua nong  : CO - trong chuoi Include cua timerserver.lua, tu nap lai trong <= 1 phut sau khi ghi tep
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
Include("\\script\\lib\\common.lua")
if not tbBaseClass then

local t = newtag()
local  Tgettable = function(t, k)

	if rawget(t,k) == nil then
		return rawget(t,"_tbBase")[k]
	else
		return rawget(t,k)
	end
end

local Tsettable = function(t, k, v)
	rawset(t,k,v)
end

local t = newtag()

tbBaseClass = {}
function tbBaseClass:new(...) local arg = {n = select("#", ...), ...};
	local tb = 
	{
		_tbBase = self
	}
	settag(tb,t)
	
	local isOk
	if (tb._init) then
		isOk = tb:_init(unpack(arg));
	end;
	return tb,isOk;

end
function tbBaseClass:NewClass(tbClass, ...) local arg = {n = select("#", ...), ...};
	local tb = 
	{
		_tbBase = tbClass,
		new = self.new
	}
	settag(tb,t)
	
	local isOk
	if (tb._init) then
		tinsert(arg,1,tb)
		isOk = call(tb._init,arg);
	end;
	return tb,isOk;

end
settagmethod(t,"gettable",Tgettable)
settagmethod(t,"settable",Tsettable)

end
