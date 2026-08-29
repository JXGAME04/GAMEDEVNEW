------------------------------------------------------
-- 文件名　：liansaienterance.lua
-- 创建者　：
-- 创建时间：2020-10-09-09-41
-- 描  述  ：联赛入口
------------------------------------------------------
if 1 ~= MODEL_GAMECLIENT then
	return
end
Include("\\script\\lib\\objbuffer_head.lua")
Include("\\script\\protocol.lua")
Include("\\script\\ui\\manage.lua")

local tbClass	= {}

tbClass.UIGROUP	= "UI_LIANSAIENTRANCE"

tbClass.UICOMP	= {}

function tbClass:OnCreate()
	-- delete this func if you don't need it
end

function tbClass:OnDestroy()
	-- delete this func if you don't need it
end

function tbClass:PreOpen()
	-- delete this func if you don't need it
	-- when existed, return 1 if you want to continue the "open" process
	return 1
end

function tbClass:OnOpen()
	-- delete this func if you don't need it
	-- when existed, return 1 if you want to continue the "open" process
	return 1
end

function tbClass:PostOpen()
	-- delete this func if you don't need it
end

function tbClass:PreClose()
	-- delete this func if you don't need it
	-- when existed, return 1 if you want to continue the "close" process
	return 1
end

function tbClass:OnClose()
	-- delete this func if you don't need it
	-- when existed, return 1 if you want to continue the "close" process
	return 1
end

function tbClass:PostClose()
	-- delete this func if you don't need it
end

function tbClass:OnLBClick(szWnd, nParam)
	if szWnd == "BtnGamble" then
		ScriptProtocol:SendData("emSCRIPT_PROTOCOL_C2S_LIANSAI_GOTOGAMBLE", 0)
	end
end

UiManage:RegisterClass(tbClass)

