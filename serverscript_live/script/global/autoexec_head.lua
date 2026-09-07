-- ================================================================================================
-- [HE THONG] script/global/autoexec_head.lua
-- Muc dich  : AutoFunctions: danh sach ham chay tu dong luc boot (chi trong state nay - JX1 moi tep mot state).
-- Duoc nap  : Include tu 4 tep (vd guideperson.lua, tongcastle.lua, treedeath.lua, readymap.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Ham (dong): AutoFunctions:Add (5), AutoFunctions:Run (15)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
-- 系统启动时自动运行的函数 wangbin 2010.12.10
AutoFunctions = {
	m_Functions = {}
}

function AutoFunctions:Add(func, param)
	if (func ~= nil) then
		local tb = {}
		tb.Func = func
		tb.Param = param
		tb.Pack = curpack()
		tinsert(self.m_Functions, tb)
	end
end

function AutoFunctions:Run()
	for i = 1, getn(self.m_Functions) do
		local tb = self.m_Functions[i]
		local pack = usepack(tb.Pack)
		tb.Func(tb.Param)
		usepack(pack)
	end
end
