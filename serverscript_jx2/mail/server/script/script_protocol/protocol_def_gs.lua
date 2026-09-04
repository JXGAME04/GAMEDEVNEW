if MODEL_GAMESERVER ~= 1 then
	return
end

Include("\\script\\protocol.lua")
local Def = 
{
	{
		"emSCRIPT_PROTOCOL_ECHO", --协议号枚举
		"\\script\\script_protocol\\echo_gs.lua",	-- [MAIL 03/09] thu kenh: chuoi tu client
		"EchoBack",
		{OBJTYPE_STRING},
	},
	{
		"emSCRIPT_PROTOCOL_EQUIP_RANK",
		"\\script\\misc\\rank\\equip_value_rank.lua",
		"SendRankData",
	},
	{
		"emSCRIPT_PROTOCOL_STORES_REQUEST_ITEM",
		"\\script\\item\\dynamic_shop\\dynamic_shop_gs.lua",
		"DynamicShop:SendItem",
		{OBJTYPE_NUMBER}
	},
	{	-- 客户端申请显示一个表格窗口，类似宋金、七城大战的即时战报
		"emSCRIPT_PROTOCOL_REQUESTTABLE",
		"\\script\\requesttable.lua",
		"RequestTable",
	},
	{
		"emSCRIPT_PROTOCOL_BATTLE",
		"\\script\\missions\\battle\\protocol_gs.lua",
		"proc_protocol",
		{OBJTYPE_NUMBER, OBJTYPE_NUMBER}
	},
	{
		"emSCRIPT_PROTOCOL_BINGO_MACHINE",
		"\\script\\event\\bingo_machine\\bingo_machine_gs.lua",
		"BingoMachine:ProcProtocol",
		{OBJTYPE_STRING, OBJTYPE_NUMBER, OBJTYPE_NUMBER},
	},
	{
		"emSCRIPT_PROTOCOL_BINGO_GET_COIN",
		"\\script\\event\\bingo_machine\\bingo_machine_gs.lua",
		"BingoMachine:GetCoin",
		{OBJTYPE_STRING},
	},
	{
		"emSCRIPT_PROTOCAL_HuoYueDu_Award",
		"\\script\\huoyuedu\\award.lua",
		"tbHuoYueDu:GiveAward",
		{OBJTYPE_NUMBER},
	},
	--{
	--	"emSCRIPT_PROTOCOL_OPEN_CREDITS_SHOP",
	--	"\\script\\missions\\arena\\npc\\officer.lua",
	--	"open_credits_shop",
	--	{},
	--},
	--{
	--	"emSCRIPT_PROTOCOL_SIGNUP_AREAN",
	--	"\\script\\missions\\arena\\protocol.lua",
	--	"apply_signup",
	--	{},
	--},
	
	{
		"emSCRIPT_PROTOCOL_QIANCHONGLOU",
		"\\script\\missions\\qianchonglou\\challenger.lua",
		"tbPlayerHandle:ProcessProtocol",
		{OBJTYPE_NUMBER, OBJTYPE_TABLE},
		
	},	
	
	{
		"emSCRIPT_PROTOCOL_FLIP_CARD",
		"\\script\\flipcard\\flipcard_gs.lua",
		"tbFlipCard:OnFlip",
		{OBJTYPE_NUMBER},
		
	},	
	{
		"emSCRIPT_PROTOCOL_PET",
		"\\script\\petsys\\protocol_process_gs.lua",
		"PetSys:ProtocolProcess",
		{OBJTYPE_NUMBER},
		
	},
	{
		"emSCRIPT_PROTOCOL_LOGIN_PRIZE",
		"\\script\\event\\prize\\loginprize.lua",
		"tbLoginPrize:ProcessProtocol",
		{OBJTYPE_NUMBER, OBJTYPE_NUMBER},
		
	},
--	{
--		"emSCRIPT_PROTOCOL_ACC_TIME_PRIZE",
--		"\\script\\event\\prize\\acc_time_prize_gs.lua",
--		"tbAccTimePrize:OnGetOnlineTimeAward",
--		{},
--	},
	{
		"emSCRIPT_PROTOCOL_FUNC_PRIZE",
		"\\script\\event\\prize\\func_prize_gs.lua",
		"tbFuncPrize:OnAward",
		{OBJTYPE_NUMBER},
	},
	{
		"emSCRIPT_PROTOCOL_OPEN_URL",
		"\\script\\event\\questionnaire\\questionnaire.lua",
		"add_questionnaire_count",
		{OBJTYPE_NUMBER},
	},
	{
		"emSCRIPT_PROTOCOL_SECURITY_LOCK",
		"\\script\\global\\securitylock\\security_lock.lua",
		"tbSecurityLock:Lock",
		{OBJTYPE_NUMBER},
	},
	{
		"emSCRIPT_PROTOCOL_SECURITY_UNLOCK",
		"\\script\\global\\securitylock\\security_lock.lua",
		"tbSecurityLock:UnLock",
		{OBJTYPE_STRING},
	},
	{
		"emSCRIPT_PROTOCOL_SECURITY_CONFIG",
		"\\script\\global\\securitylock\\security_lock.lua",
		"tbSecurityLock:SaveConfig",
		{OBJTYPE_NUMBER},
	},
	{
		"emSCRIPT_PROTOCOL_SECURITY_RESET",
		"\\script\\global\\securitylock\\security_lock.lua",
		"tbSecurityLock:ResetPassword",
		{OBJTYPE_STRING, OBJTYPE_STRING},
	},
	{
		"emSCRIPT_PROTOCOL_CLIENT_EVENT",
		"\\script\\global\\events.lua",
		"OnClientEvent",
		{OBJTYPE_STRING, OBJTYPE_TABLE},
	},
	{
		"emSCRIPT_PROTOCOL_ACHIEVEMENT_REWARD",
		"\\script\\global\\achievementsys\\head.lua",
		"AchievementSys:GetAward",
		{OBJTYPE_NUMBER},
	},
	{
		"emSCRIPT_PROTOCOL_TONG_GUTA",
		"\\script\\missions\\tong_guta\\ui_gs.lua",
		"GuTaUI:OnReceiveData",
		{OBJTYPE_NUMBER, OBJTYPE_TABLE},
	},
	{
		"emSCRIPT_PROTOCOL_TONG_PKMESSAGE",
		"\\script\\TongPkMessage\\Tong_Pk_Message_gs.lua",
		"tbProcessServerProtocol:OnRecvProtocol",
		{OBJTYPE_NUMBER},
	},
	{
    	"emSCRIPT_PROTOCOL_MANTLE_DOINLAY",
		"\\script\\global\\mantlesystem\\mantleupgrade_npc.lua",
        "doMantleMosaicStone",
		{OBJTYPE_TABLE},
	},
	{
		"emSCRIPT_PROTOCOL_NEW_CLOTHES_SHOP",
		"\\script\\global\\appearancesystem\\shop_s.lua",
        "tbAppearanceShopServer:OnRecvProtocol",
		{OBJTYPE_STRING, OBJTYPE_TABLE},
	},
	{
		"emSCRIPT_PROTOCOL_NEW_CLOTHES_WARDROBE",
		"\\script\\global\\appearancesystem\\wardrobe_s.lua",
        "tbAppearanceWardrobeServer:OnRecvProtocol",
		{OBJTYPE_STRING, OBJTYPE_TABLE},
	},
	{
		"emSCRIPT_PROTOCOL_EXCHANGE_SHOP",
		"\\script\\global\\exchangeshop\\main_s.lua",
        "tbExchangeShopServer:OnRecvProtocol",
		{OBJTYPE_STRING, OBJTYPE_TABLE},
	},
	-- [MAIL 03/09] he THU (uimail.lua 2.0 gui) -> \script\mail\mailmanager.lua
	{ "emSCRIPT_PROTOCOL_MAIL_REQUEST_HEADERLIST", "\\script\\mail\\mailmanager.lua", "MailManager_OnRequestHeaderList", {OBJTYPE_NUMBER} },
	{ "emSCRIPT_PROTOCOL_MAIL_REQUEST_DELETE", "\\script\\mail\\mailmanager.lua", "MailManager_OnRequestDelete", {OBJTYPE_NUMBER} },
	{ "emSCRIPT_PROTOCOL_MAIL_REQUEST_WHOLEMAIL", "\\script\\mail\\mailmanager.lua", "MailManager_OnRequestWholeMail", {OBJTYPE_NUMBER} },
	{ "emSCRIPT_PROTOCOL_MAIL_REQUEST_STATECHANGE", "\\script\\mail\\mailmanager.lua", "MailManager_OnRequestStateChange", {OBJTYPE_NUMBER, OBJTYPE_NUMBER} },
	{ "emSCRIPT_PROTOCOL_MAIL_REQUEST_AUTODELETE", "\\script\\mail\\mailmanager.lua", "MailManager_OnRequestAutoDelete", nil },
	{ "emSCRIPT_PROTOCOL_MAIL_REQUEST_OPENURL", "\\script\\mail\\mailmanager.lua", "MailManager_OnRequestOpenUrl", {OBJTYPE_STRING} },
	-- [DAUGIA 04/09] dau gia -> \script\auction_house\auction_manager.lua
	{ "emSCRIPT_PROTOCOL_AUCTION_REQUEST_OFFERENGLISHPRICE", "\\script\\auction_house\\auction_manager.lua", "AUC_OnRequestOfferEnglish", {OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER, OBJTYPE_NUMBER} },
	{ "emSCRIPT_PROTOCOL_AUCTION_REQUEST_OFFERDUTCHPRICE", "\\script\\auction_house\\auction_manager.lua", "AUC_OnRequestOfferDutch", {OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER, OBJTYPE_NUMBER} },
	{ "emSCRIPT_PROTOCOL_AUCTION_REQUEST_REFUND", "\\script\\auction_house\\auction_manager.lua", "AUC_OnRequestRefund", {OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER} },
	{ "emSCRIPT_PROTOCOL_AUCTION_REQUEST_ACTIVITYLIST", "\\script\\auction_house\\auction_manager.lua", "AUC_OnRequestActivityList", {OBJTYPE_NUMBER, OBJTYPE_NUMBER} },
	{ "emSCRIPT_PROTOCOL_AUCTION_REQUEST_ACTIVITYCONTENT", "\\script\\auction_house\\auction_manager.lua", "AUC_OnRequestActivityContent", {OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER, OBJTYPE_TABLE} },
	{ "emSCRIPT_PROTOCOL_AUCTION_REQUEST_ITEMCONTENT", "\\script\\auction_house\\auction_manager.lua", "AUC_OnRequestItemContent", {OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER} },
	{ "emSCRIPT_PROTOCOL_AUCTION_REQUEST_MEMBERLIST", "\\script\\auction_house\\auction_manager.lua", "AUC_OnRequestMemberList", {OBJTYPE_STRING} },
	{ "emSCRIPT_PROTOCOL_AUCTION_REQUEST_GETBACKITEM", "\\script\\auction_house\\auction_manager.lua", "AUC_OnRequestGetBack", {OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER} },
	{ "emSCRIPT_PROTOCOL_AUCTION_REQUEST_PUTON", "\\script\\auction_house\\auction_manager.lua", "AUC_OnRequestPutOn", {OBJTYPE_NUMBER} },
}

ScriptProtocol:RegProtocolSet(Def)


function ScriptProtocol:SendData(szEnum, nHandle)
	if (type(self[szEnum]) == "number") then
		SendScriptData(self[szEnum], nHandle)
	end
end