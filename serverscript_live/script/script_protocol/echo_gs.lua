-- ================================================================================================
-- [HE THONG] script/script_protocol/echo_gs.lua
-- Muc dich  : Handler ECHO thu kenh ScriptProtocol (MAIL 03/09).
-- Duoc nap  : Include tu 1 tep (vd lenhbaiadmin.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Include   : protocol.lua
-- Ham (dong): EchoBack (5), EchoTest (10)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
-- echo_gs.lua - [MAIL 03/09] handler ECHO phia MAY CHU de thu kenh ScriptProtocol.
-- EchoBack duoc goi qua DynamicExecuteByPlayer (PlayerIndex da dat) tu protocol_def_gs.lua.
-- EchoTest goi tu menu lenh bai admin (lenhbaiadmin.lua: /specho).
Include("\\script\\protocol.lua")

function EchoBack(szText)
	GhiLog("SCRIPTPROTO", "ECHO tu client: "..(szText or "nil"))
	Msg2Player("May chu da nhan ECHO: "..(szText or "nil"))
end

function EchoTest()
	local h = OB_Create()
	ObjBuffer:PushByType(h, OBJTYPE_STRING, "xin chao tu may chu")
	local nOk = SendScriptData(ScriptProtocol["emSCRIPT_PROTOCOL_ECHO"], h)
	OB_Release(h)
	Msg2Player("Da gui ECHO xuong client (ket qua "..(nOk or "nil")..")")
end
