-- ?gm ds LoadScript("\\script\\protocol.lua")
if MODEL_GAMECLIENT ~= 1 then
    Include("\\script\\script_protocol\\protocol_def_gs.lua")
else
    Include("\\script\\script_protocol\\protocol_def_c.lua")
end


ScriptProtocol = {}

ScriptProtocol.KE_SCRIPT_PROTOCOL = 
{
    "emSCRIPT_PROTOCOL_ECHO",
    "emSCRIPT_PROTOCOL_EQUIP_RANK",	-- 兵甲排行榜
    "emSCRIPT_PROTOCOL_CLIENT_CMD",
    "emSCRIPT_PROTOCOL_REQUESTTABLE",
    "emSCRIPT_PROTOCOL_STORES_REQUEST_ITEM",
    "emSCRIPT_PROTOCOL_BATTLE",
    "emSCRIPT_PROTOCAL_HuoYueDu_Award", --给活跃度奖励
    "emSCRIPT_PROTOCOL_ADD_BUBBLE",--显示一个新手泡泡
    "emSCRIPT_PROTOCOL_FRESH_TASK_OPEN_TEAM_WND",
    "emSCRIPT_PROTOCOL_FRESH_TASK_OPEN_FRIEND_WND",
    "emSCRIPT_PROTOCOL_OPEN_CREDITS_SHOP",
    "emSCRIPT_PROTOCOL_SIGNUP_AREAN",
    "emSCRIPT_PROTOCOL_QIANCHONGLOU",
    "emSCRIPT_PROTOCAL_TaskGuide_QuitTask",
    "emSCRIPT_PROTOCAL_LEVELUP_AWARD",
    "emSCRIPT_PROTOCAL_LEVELUP_NOTICE",
    -- "emSCRIPT_PROTOCAL_SWEEPSTAKE",
    "emSCRIPT_PROTOCOL_FANDAOMIJING",
    "emSCRIPT_PROTOCOL_HANGER_RECRUIT",
    "emSCRIPT_PROTOCOL_HANGER_ADDTIME",
    "emSCRIPT_PROTOCOL_HANGER_DISPATCH",
    "emSCRIPT_PROTOCOL_HANGER_AWARD",
    "emSCRIPT_PROTOCOL_HANGER_TASK_IMMEDIATEFINISH",
    "emSCRIPT_PROTOCOL_HANGER_FETCH_DATA",
    "emSCRIPT_PROTOCAL_TRANS_NIMBUS",
    "emSCRIPT_PROTOCAL_C2S_EXITGAME",
    ---------------------签到系统 Begin--------------------------
    "emSCRIPT_PROTOCAL_SIGNIN_FETCH_DATA",
    "emSCRIPT_PROTOCAL_SIGNIN_DO",
    "emSCRIPT_PROTOCAL_SIGNIN_AWARD",
    "emSCRIPT_PROTOCAL_SIGNIN_SYNC_DATE",
    "emSCRIPT_PROTOCAL_SIGNIN_SYNC_AWARD",
    "emSCRIPT_PROTOCAL_GET_OFFLINE_EXP",
    ---------------------签到系统 End--------------------------
    "emSCRIPT_PROTOCOL_COMPOSE_OPENUI",
    "emSCRIPT_PROTOCOL_COMPOSE_RESULT",
    "emSCRIPT_PROTOCAL_PROCESS_CHECK",
    ---------------------披风系统(镶嵌) Begin--------------------------
    "emSCRIPT_PROTOCOL_MANTLE_OPENUI",  -- S2C
    "emSCRIPT_PROTOCOL_MANTLE_DOINLAY", -- C2S
    "emSCRIPT_PROTOCOL_MANTLE_INLAYRESULT",
    ---------------------披风系统(镶嵌) End--------------------------
    ---------------------帮会活跃度 Begin--------------------------
    "emSCRIPT_PROTOCOL_TONGHYD_REQUESTP1",
    "emSCRIPT_PROTOCOL_TONGHYD_RETURNP1",
    "emSCRIPT_PROTOCOL_TONGHYD_REQUESTP2",
    "emSCRIPT_PROTOCOL_TONGHYD_RETURNP2",
    "emSCRIPT_PROTOCOL_TONGHYD_GETAWARD",
    ---------------------帮会活跃度 End--------------------------
    ---------------------淮河河畔宋金双方士气值 Begin--------------------------
    "emSCRIPT_PROTOCOL_OPENUI_SHIQIZHI",
    "emSCRIPT_PROTOCOL_SONGJIN_SHIQIZHI",
    "emSCRIPT_PROTOCOL_UPDATE_SHIQIZHI",
    ---------------------淮河河畔宋金双方士气值 End  --------------------------

    ---------------------英雄塔Ui Begin----------------------------------------
    "emSCRIPT_PROTOCOL_TOGGLE_TOWERTASKBTN",
    "emSCRIPT_PROTOCOL_TOGGLE_TOWERTASK",
    "emSCRIPT_PROTOCOL_TOWERTASK_SETACCEPT",
    "emSCRIPT_PROTOCOL_TOWERTASK_SETTASK",
    "emSCRIPT_PROTOCOL_TOWERTASK_SETTIMER",
    "emSCRIPT_PROTOCOL_TOWERTASK_ADDTARGET",
    "emSCRIPT_PROTOCOL_TOWERTASK_ADDTARGETCOUNT",
    "emSCRIPT_PROTOCAL_TOWERTASK_START",
    "emSCRIPT_PROTOCAL_TOWERTASK_AWARD",
    "emSCRIPT_PROTOCAL_TOWERTASK_LEAVE",
    
    "emSCRIPT_PROTOCOL_HTLISTClOSE",
    "emSCRIPT_PROTOCOL_HTLISTOPEN",
    "emSCRIPT_PROTOCOL_HTLISTOLDSETDATA",
    "emSCRIPT_PROTOCOL_HTLISTOLDdELDATA",
    "emSCRIPT_PROTOCOL_HTLISTOLDDELDATAS",
    "emSCRIPT_PROTOCOL_HTLISTNOWSETDATA",
    "emSCRIPT_PROTOCOL_HTLISTNOWSETMINEDATA",
    "emSCRIPT_PROTOCOL_HTLISTNOWDELDATA",
    "emSCRIPT_PROTOCOL_HTLISTNOWDELDATAS",
    "emSCRIPT_PROTOCOL_OPENWNDFLIPS",
    "emSCRIPT_PROTOCOL_CLOSEWNDFLIPS",
    "emSCRIPT_PROTOCOL_WNDFLIPSPLAYSINGLE",
    ---------------------英雄塔Ui End------------------------------------------
    ---------------------剑冢迷宫Ui Begin------------------------------------------
    "emSCRIPT_PROTOCOL_TOGGLE_SWORDTASK",
    "emSCRIPT_PROTOCOL_SWORDTASK_SETTASK",
    "emSCRIPT_PROTOCOL_SWORDTASK_SETTIMER",
    "emSCRIPT_PROTOCOL_SWORDTASK_ADDTARGET",
    "emSCRIPT_PROTOCOL_SWORDTASK_ADDTARGETCOUNT",
    "emSCRIPT_PROTOCOL_SWORDTASK_SETTARGETCOUNT",
    ---------------------剑冢迷宫Ui End------------------------------------------
	---------------------锻造Ui Begin------------------------------------------
	"emSCRIPT_PROTOCOL_DUANZAO_OPENUI",
	"emSCRIPT_PROTOCOL_DUANZAO_RESULT",
	---------------------锻造Ui End------------------------------------------
    "emSCRIPT_PROTOCAL_GOLDBOSS_DMG_STAT",
    "emSCRIPT_PROTOCAL_NEW_SEVEN_CITY_REPORT",
    "emSCRIPT_PROTOCOL_RELOAD_CLIENT_SCRIPT",
    "emSCRIPT_PROTOCOL_RELOAD_GOLDBOSS_SCHEME",

    ------------------------邮箱系统---------------------------
    -- S2C
    "emSCRIPT_PROTOCOL_MAIL_HEADERLIST",
    "emSCRIPT_PROTOCOL_MAIL_NEWMAIL",
    "emSCRIPT_PROTOCOL_MAIL_STATECHANGE",
    "emSCRIPT_PROTOCOL_MAIL_DELETE",
    "emSCRIPT_PROTOCOL_MAIL_WHOLEMAIL",
    "emSCRIPT_PROTOCOL_MAIL_OPENWINDOW",
    -- C2S
    "emSCRIPT_PROTOCOL_MAIL_REQUEST_HEADERLIST",
    "emSCRIPT_PROTOCOL_MAIL_REQUEST_DELETE",
    "emSCRIPT_PROTOCOL_MAIL_REQUEST_WHOLEMAIL",
    "emSCRIPT_PROTOCOL_MAIL_REQUEST_STATECHANGE",
    "emSCRIPT_PROTOCOL_MAIL_REQUEST_AUTODELETE",
    "emSCRIPT_PROTOCOL_MAIL_REQUEST_OPENURL",
    --------------------拍卖行系统-----------------------
    -- S2C
    "emSCRIPT_PROTOCOL_AUCTION_OFFERPRICERET",
    "emSCRIPT_PROTOCOL_AUCTION_REFUNDRET",
    "emSCRIPT_PROTOCOL_AUCTION_ACTIVITYLIST",
    "emSCRIPT_PROTOCOL_AUCTION_ACTIVITYINFO",
    "emSCRIPT_PROTOCOL_AUCTION_ITEMINFO",
    "emSCRIPT_PROTOCOL_AUCTION_MEMBERLIST",
    "emSCRIPT_PROTOCOL_AUCTION_NEWACTIVITY",
    "emSCRIPT_PROTOCOL_AUCTION_ENDACTIVITY",
    "emSCRIPT_PROTOCOL_AUCTION_NEWITEM",
    "emSCRIPT_PROTOCOL_AUCTION_ENDITEM",
    "emSCRIPT_PROTOCOL_AUCTION_TPRICECHANGE",
    -- C2S
    "emSCRIPT_PROTOCOL_AUCTION_REQUEST_OFFERENGLISHPRICE",  -- 英式报价
    "emSCRIPT_PROTOCOL_AUCTION_REQUEST_OFFERDUTCHPRICE",    -- 荷式报价
    "emSCRIPT_PROTOCOL_AUCTION_REQUEST_REFUND",
    "emSCRIPT_PROTOCOL_AUCTION_REQUEST_ACTIVITYLIST",
    "emSCRIPT_PROTOCOL_AUCTION_REQUEST_ACTIVITYCONTENT",
    "emSCRIPT_PROTOCOL_AUCTION_REQUEST_ITEMCONTENT",
    "emSCRIPT_PROTOCOL_AUCTION_REQUEST_MEMBERLIST",
    "emSCRIPT_PROTOCOL_AUCTION_REQUEST_GETBACKITEM",
    --------------------签到活动-----------------------
    -- S2C
    "emSCRIPT_PROTOCOL_SIGN_ACT_UPDATE_INFO",
    "emSCRIPT_PROTOCOL_SIGN_ACT_OPEN_WINDOW",
    -- C2S
    "emSCRIPT_PROTOCOL_SIGN_ACT_GET_AWARD",
    "emSCRIPT_PROTOCOL_SIGN_ACT_OPEN_QZG",
    "emSCRIPT_PROTOCOL_SIGN_ACT_OPEN_BGS",
    --------------------2021新商店---------------------------
    -- S2C
    "emSCRIPT_PROTOCOL_SHOP_2021_SEND_SERVER_INFO",
    "emSCRIPT_PROTOCOL_SHOW_SHOP_2021_ICON",
    -- C2S
    "emSCRIPT_PROTOCOL_SHOP_2021_REQUEST_BUY_ITEM",
    "emSCRIPT_PROTOCOL_SHOP_2021_REQUEST_SERVER_INFO",
    --------------------五行大阵-----------------------
    -- S2C
    "emSCRIPT_PROTOCOL_FIVESERIESZONE_BEGIN",
    "emSCRIPT_PROTOCOL_FIVESERIESZONE_END",
    -- C2S
    "emSCRIPT_PROTOCOL_FIVESERIESZONE_ENTER_DUNGEON",
    --------------------玲珑塔-----------------------
    -- S2C
    "emSCRIPT_PROTOCOL_LINGLONGTOWER_SHOW_HELPER",
    "emSCRIPT_PROTOCOL_LINGLONGTOWER_HIDE_HELPER",
    "emSCRIPT_PROTOCOL_LINGLONGTOWER_UPDATE_HELPER",
    --------------------帮会联赛-----------------------
    -- S2C
    "emSCRIPT_PROTOCOL_TONG_BATTLE_UPDATE_INFO",
    "emSCRIPT_PROTOCOL_TONG_BATTLE_CLOSE_WND",

    -- C2S
    "emSCRIPT_PROTOCOL_TONG_BATTLE_REQUEST_INFO",
    --------------------坐骑界面-----------------------
    -- S2C 
    -- "emSCRIPT_PROTOCOL_OPEN_HORSE_OUT_LOOK_WND",
    "emSCRIPT_PROTOCOL_OPEN_HORSE_OUT_LOOK_WND",
    --------------------2023周年庆---------------------------
    -- S2C
    "emSCRIPT_PROTOCOL_SHOW_2023_ONLINE_AWARD_ICON",
    "emSCRIPT_PROTOCOL_HIDE_2023_ONLINE_AWARD_ICON",
    -- C2S
    -- "emSCRIPT_PROTOCOL_REQUEST_GET_2023_ONLINE_AWARD",
    --------------------BOSS血条-----------------------
    -- S2C
    "emSCRIPT_PROTOCOL_BOSS_HP_BAR_OPEN_WND",
    --------------------2026战令---------------------------
    -- S2C
    "emSCRIPT_PROTOCOL_SEASON_TICKET_SHOW_ICON",
    "emSCRIPT_PROTOCOL_SEASON_TICKET_UPDATE_WND",
    -- C2S
    "emSCRIPT_PROTOCOL_SEASON_TICKET_GET_AWARD",
    "emSCRIPT_PROTOCOL_SEASON_TICKET_GET_SCORE",
    --------------------by liugang4-----------------------


    --------------------门派多修---------------------------
    -- C2S
    "emSCRIPT_PROTOCOL_MUTIPLEFACTION_REQUEST_CHANGE",
    -- S2C
    "emSCRIPT_PROTOCOL_MUTIPLEFACTION_OPEN_PAGE",

    --------------------天罡符---------------------------
    -- S2C
    "emSCRIPT_PROTOCOL_TIANGANGFU_OPENUI",
    "emSCRIPT_PROTOCOL_TIANGANGFU_SELECTAGAIN",
    -- C2S
    "emSCRIPT_PROTOCOL_TIANGANGFU_ZHULING",
    "emSCRIPT_PROTOCOL_TIANGANGFU_UNZHULING",
    "emSCRIPT_PROTOCOL_TIANGANGFU_SHENGLING",
    "emSCRIPT_PROTOCOL_TIANGANGFU_HUANLING",

    ------------------------首冲---------------------------
    -- S2C
    "emSCRIPT_PROTOCOL_NORMALAWARD_REPONSE_AWARD",
    "emSCRIPT_PROTOCOL_NORMALAWARD_REPONSE_AWARDSTATE",
    "emSCRIPT_PROTOCOL_NORMALAWARD_REPONSE_RECEIVE_AWARD",
    -- C2S
    "emSCRIPT_PROTOCOL_NORMALAWARD_REQUEST_AWARD",
    "emSCRIPT_PROTOCOL_NORMALAWARD_REQUEST_AWARDSTATE",
    "emSCRIPT_PROTOCOL_NORMALAWARD_REQUEST_RECEIVE_AWARD",

    ------------------------抽锦鲤---------------------------
    -- S2C
    "emSCRIPT_PROTOCOL_TWINKLE_SWEEPSTAKE_BOX",

    
    ------------------------好友招募---------------------------
    -- C2S
    "emSCRIPT_PROTOCOL_RECRUIT_REQUEST_IMPART",
    -- S2C
    "emSCRIPT_PROTOCOL_CLOSE_PROGRESSBAR",


    --------------侠峰论剑--------------------
    "emScript_PROTOCOL_S2C_GREATTEAMFIGHT_SYNC",
    "emScript_PROTOCOL_C2S_GREATTEAMFIGHT_REQUEST",

    "emScript_PROTOCOL_C2S_REPORT",


    "emScript_PROTOCOL_C2S_HIGHBLUE",
    "emScript_PROTOCOL_S2C_HIGHBLUE",

    "emScript_PROTOCOL_S2C_OPEN_SCRIPT_WND",

    "emScript_PROTOCOL_S2C_FREEDOMRANK",
    "emScript_PROTOCOL_C2S_FREEDOMRANK",


    "emSCRIPT_PROTOCOL_TITLE_SEND_TITLE_INFO",
    "emSCRIPT_PROTOCOL_OPEN_OUT_LOOK_MANAGE_WND",

    "emSCRIPT_PROTOCOL_TITLE_REQUEST_TITLE_INFO",
    "emSCRIPT_PROTOCOL_TITLE_REQUEST_ACTIVE_SPR",
    "emSCRIPT_PROTOCOL_TITLE_REQUEST_ACTIVE_ATTR",

    "emScript_PROTOCOL_C2S_RESP_INFO",
    "emScript_PROTOCOL_S2C_REQ_INFO",

    "emScript_PROTOCOL_C2S_SERIESSTAMP",
    "emScript_PROTOCOL_S2C_SERIESSTAMP",

    "emScript_PROTOCOL_S2C_NEWKILLER",
    "emScript_PROTOCOL_C2S_NEWKILLER",

    "emScript_PROTOCOL_C2S_SEVENDAYMISSION",
    "emScript_PROTOCOL_S2C_SEVENDAYMISSION",
    --------------世界线--------------------
    "emScript_PROTOCOL_S2C_WORLDLINE",

    "emScript_PROTOCOL_S2C_TALK_TWOOPT",
    "emScript_PROTOCOL_C2S_TALK_TWOOPT",

    "emSCRIPT_PROTOCOL_COUNT",

    "emScript_PROTOCOL_S2C_CARD_ACT_BTN",
    "emScript_PROTOCOL_C2S_CARD_ACT_BTN",

    "emScript_PROTOCOL_C2S_ONLINE_ACT_BTN",
    "emScript_PROTOCOL_S2C_ONLINE_ACT_BTN",

    "emScript_PROTOCOL_C2S_RETURN_FRIEND_CLAIM",
    "emScript_PROTOCOL_S2C_RETURN_FRIEND_ICON"
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

function ScriptProtocol:GetLastProtolId()
    return self.nLastProtolId or -1
end

--服务器收到的协议, 服务器处理
function ScriptProtocol:ProtocolProcess(nProtolId, nHandle)
	if type(nProtolId) == "number" and type(self.tbProtocolDef[nProtolId]) == "table" then	
        self.nLastProtolId = nProtolId
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


function ScriptProtocol:SendData(szEnum, nHandle)
    if (type(self[szEnum]) == "number") then
        
        if MODEL_GAMECLIENT ~= 1 and SendScriptData then
            SendScriptData(self[szEnum], nHandle)
        elseif SendScriptDataToServer then
            SendScriptDataToServer(self[szEnum], nHandle);
        end
    end
end

function ScriptProtocol:Echo(nHandle)
end

function ScriptProtocol:SendTable(szProtocol, tbData)
    local handle = OB_Create()
    ObjBuffer:PushByType(handle, OBJTYPE_TABLE, tbData);
	self:SendData(szProtocol, handle);
	OB_Release(handle);
end

ScriptProtocol:_InitProtocolEnum()

ScriptProtocol:RegProtocolSet(Def)

--临时解决方案
-- 客户端的协议处理移到了 ./script_protocol/protocol_def_c.lua
