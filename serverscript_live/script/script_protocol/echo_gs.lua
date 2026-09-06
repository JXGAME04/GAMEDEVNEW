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
