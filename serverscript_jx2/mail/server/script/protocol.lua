Include("\\script\\lib\\objbuffer_head.lua")



ScriptProtocol = {}

ScriptProtocol.KE_SCRIPT_PROTOCOL = 
{
	"emSCRIPT_PROTOCOL_ECHO",
	"emSCRIPT_PROTOCOL_EQUIP_RANK",
	"emSCRIPT_PROTOCOL_STORES_REQUEST_ITEM",
	"emSCRIPT_PROTOCOL_REQUESTTABLE",
	"emSCRIPT_PROTOCOL_BATTLE",
	"emSCRIPT_PROTOCOL_BINGO_MACHINE",
	"emSCRIPT_PROTOCAL_HuoYueDu_Award", --给活跃度奖励	
	"emSCRIPT_PROTOCOL_COUNT",
	"emSCRIPT_PROTOCOL_BINGO_OPENWINDOW",
	"emSCRIPT_PROTOCOL_BINGO_COIN",
	"emSCRIPT_PROTOCOL_BINGO_GET_AWARD_RESULT",
	"emSCRIPT_PROTOCOL_BINGO_GET_COIN",
	--"emSCRIPT_PROTOCOL_OPEN_CREDITS_SHOP",
	--"emSCRIPT_PROTOCOL_SIGNUP_AREAN",
	"emSCRIPT_PROTOCOL_QIANCHONGLOU",
	"emSCRIPT_PROTOCOL_FLIP_CARD",
	"emSCRIPT_PROTOCOL_PET",
	"emSCRIPT_PROTOCOL_LOGIN_PRIZE",
	"emSCRIPT_PROTOCOL_FUNC_PRIZE",
	--"emSCRIPT_PROTOCOL_ACC_TIME_PRIZE",
	"emSCRIPT_PROTOCOL_FULI_REPORT",
	"emSCRIPT_PROTOCOL_OPEN_URL",
	"emSCRIPT_PROTOCOL_SECURITY_LOCK",
	"emSCRIPT_PROTOCOL_SECURITY_UNLOCK",
	"emSCRIPT_PROTOCOL_SECURITY_CONFIG",
	"emSCRIPT_PROTOCOL_SECURITY_RESET",
	"emSCRIPT_PROTOCOL_POP_BLACK_TIPS",
	"emSCRIPT_PROTOCOL_CLIENT_EVENT",
	"emSCRIPT_PROTOCOL_ACHIEVEMENT_REWARD",
	"emSCRIPT_PROTOCOL_ACHIEVEMENT_ACHIEVED",
	"emSCRIPT_PROTOCOL_TONG_GUTA",
	"emSCRIPT_PROTOCOL_TONG_PKMESSAGE",
	---------------------披风系统(镶嵌) Begin--------------------------
	"emSCRIPT_PROTOCOL_MANTLE_OPENUI",  -- S2C
	"emSCRIPT_PROTOCOL_MANTLE_DOINLAY", -- C2S
	"emSCRIPT_PROTOCOL_MANTLE_INLAYRESULT",
    ---------------------披风系统(镶嵌) End--------------------------
    "emSCRIPT_PROTOCOL_OPENWINDOW",
    ---------------------外装盒子 Begin--------------------------
    "emSCRIPT_PROTOCOL_NEW_CLOTHES_SHOP",
    "emSCRIPT_PROTOCOL_NEW_CLOTHES_WARDROBE",
    ---------------------外装盒子 End--------------------------
    ---------------------以物换物通用商店 Begin--------------------------
    "emSCRIPT_PROTOCOL_EXCHANGE_SHOP",
    ---------------------以物换物通用商店 End--------------------------
	-- [MAIL 03/09] he THU (client VLTK 2.0 protocol.lua:112-124) - noi CUOI de khong doi ma cu
	"emSCRIPT_PROTOCOL_MAIL_HEADERLIST",
	"emSCRIPT_PROTOCOL_MAIL_NEWMAIL",
	"emSCRIPT_PROTOCOL_MAIL_STATECHANGE",
	"emSCRIPT_PROTOCOL_MAIL_DELETE",
	"emSCRIPT_PROTOCOL_MAIL_WHOLEMAIL",
	"emSCRIPT_PROTOCOL_MAIL_OPENWINDOW",
	"emSCRIPT_PROTOCOL_MAIL_REQUEST_HEADERLIST",
	"emSCRIPT_PROTOCOL_MAIL_REQUEST_DELETE",
	"emSCRIPT_PROTOCOL_MAIL_REQUEST_WHOLEMAIL",
	"emSCRIPT_PROTOCOL_MAIL_REQUEST_STATECHANGE",
	"emSCRIPT_PROTOCOL_MAIL_REQUEST_AUTODELETE",
	"emSCRIPT_PROTOCOL_MAIL_REQUEST_OPENURL",
}

function ScriptProtocol:_InitProtocolEnum()
	for key, v in self.KE_SCRIPT_PROTOCOL do
		self[v] = key
	end
end


function ScriptProtocol:RegProtocol(szProtocolEnum, szFile, szFun, tbParamFormat)
	self.tbProtocolDef = self.tbProtocolDef or {}
	
	
	local nProtocolId =  self[szProtocolEnum]
	
	if type(nProtocolId) ~= "number" then
		print("reg script protocol fail because it's nil"..szProtocolEnum)
		return
	end
	if self.tbProtocolDef[nProtocolId] then
		error("reg script protocol fail because reged "..(self.KE_SCRIPT_PROTOCOL[nProtocolId] or "nil"))
		return
	end
	
	self.tbProtocolDef[nProtocolId] = {szFile, szFun, tbParamFormat}
end

function ScriptProtocol:HandleProcess(nHandle, tbParamFormat)
	
	if type(tbParamFormat) ~= "table" then
		return {nHandle}
	end
	
	local tbParam = {}

	for i=1, getn(tbParamFormat) do
		local var = ObjBuffer:PopByType(nHandle, tbParamFormat[i])
		tinsert(tbParam, var)
	end
	
	return tbParam
end


--服务器收到的协议, 服务器处理
function ScriptProtocol:ProtocolProcess(nProtolId, nHandle)
	if type(nProtolId) == "number" and type(self.tbProtocolDef[nProtolId]) == "table" then
		local szFile = self.tbProtocolDef[nProtolId][1]
		local szFun = self.tbProtocolDef[nProtolId][2]
		local tbParamFormat = self.tbProtocolDef[nProtolId][3]
		local tbParam = self:HandleProcess(nHandle, tbParamFormat)
		if MODEL_GAMESERVER == 1 then
			DynamicExecuteByPlayer(PlayerIndex, szFile, szFun, unpack(tbParam))
		elseif MODEL_GAMECLIENT == 1 then
			if szFile ~= "" then
				Require(szFile);
			end
			DynamicExecute(szFile, szFun, unpack(tbParam))
		end
	end
end

function ScriptProtocol:RegProtocolSet(tbDefSet)
	for i=1, getn(tbDefSet) do 
		local tb = tbDefSet[i]
		if tb then
			self:RegProtocol(unpack(tb))
		end
	end
end




function ScriptProtocol:Echo(nHandle)
end

ScriptProtocol:_InitProtocolEnum()


--临时解决方案
-- 客户端的协议处理移到了 ./script_protocol/protocol_def_c.lua

-- [MAIL 03/09] du phong cho ProtocolProcess: unpack (Lua 4 khong co san, common.lua co the chua Include)
-- va Require (client 2.0 goi truoc DynamicExecute; o ta = EnsureScript trong KScriptProtocol.cpp).
if (unpack == nil) then
	function unpack(tb, n)
		if (not n) then
			n = 1
		end
		if (not tb or tb[n] == nil) then
			return
		end
		return tb[n], unpack(tb, n + 1)
	end
end
if (Require == nil) then
	function Require(szFile)
		if (szFile and szFile ~= "" and EnsureScript) then
			return EnsureScript(szFile)
		end
		return 0
	end
end
