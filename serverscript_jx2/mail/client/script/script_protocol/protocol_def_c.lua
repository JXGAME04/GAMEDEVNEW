-- protocol_def_c.lua - [MAIL 03/09] bo dieu phoi ScriptProtocol phia CLIENT
-- (khuon \script\script_protocol\protocol_def_c.lua cua client VLTK 2.0, uid D0E42D05).
-- May chu gui goi s2c_scriptdata -> KScriptProtocol.cpp goi ScriptProtocol:ProtocolProcess(id, h)
-- trong state cua tep nay; ProtocolProcess (protocol.lua) tra bang duoi de Require + DynamicExecute.
-- Co MODEL_GAMECLIENT do KSortScript.cpp dat cho thu muc script_protocol (va KScriptProtocol.cpp dat lai).
if MODEL_GAMECLIENT ~= 1 then
	return
end

Include("\\script\\protocol.lua")

Def =
{
	{
		"emSCRIPT_PROTOCOL_ECHO",
		"\\script\\script_protocol\\protocol_def_c.lua",
		"ScriptProtocol_EchoArrival",
		{OBJTYPE_STRING},
	},
	-- ---- he THU (uimail.lua chep tu VLTK 2.0) ----
	{
		"emSCRIPT_PROTOCOL_MAIL_HEADERLIST",
		"\\script\\ui\\uimail.lua",
		"UIMail:HeaderListArrival",
		{OBJTYPE_NUMBER, OBJTYPE_TABLE, OBJTYPE_NUMBER},
	},
	{
		"emSCRIPT_PROTOCOL_MAIL_NEWMAIL",
		"\\script\\ui\\uimail.lua",
		"UIMail:NewMailEventArrival",
		{OBJTYPE_NUMBER},
	},
	{
		"emSCRIPT_PROTOCOL_MAIL_STATECHANGE",
		"\\script\\ui\\uimail.lua",
		"UIMail:StateChangeEventArrival",
		{OBJTYPE_NUMBER, OBJTYPE_NUMBER, OBJTYPE_NUMBER},
	},
	{
		"emSCRIPT_PROTOCOL_MAIL_DELETE",
		"\\script\\ui\\uimail.lua",
		"UIMail:DeleteEventArrival",
		{OBJTYPE_NUMBER, OBJTYPE_NUMBER},
	},
	{
		"emSCRIPT_PROTOCOL_MAIL_WHOLEMAIL",
		"\\script\\ui\\uimail.lua",
		"UIMail:WholeMailArrival",
		{OBJTYPE_TABLE},
	},
	{
		"emSCRIPT_PROTOCOL_MAIL_OPENWINDOW",
		"\\script\\ui\\uimail.lua",
		"UIMail:OpenMailWindow",
		{OBJTYPE_NUMBER},
	},
	-- [DAUGIA 04/09] dau gia -> \script\ui\uiauction_house.lua
	{
		"emSCRIPT_PROTOCOL_AUCTION_OFFERPRICERET",
		"\\script\\ui\\uiauction_house.lua",
		"UIAuctionHouse:OnOfferPriceResponse",
		{OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER, OBJTYPE_NUMBER},
	},
	{
		"emSCRIPT_PROTOCOL_AUCTION_REFUNDRET",
		"\\script\\ui\\uiauction_house.lua",
		"UIAuctionHouse:OnRefundResponse",
		{OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER, OBJTYPE_NUMBER},
	},
	{
		"emSCRIPT_PROTOCOL_AUCTION_ACTIVITYLIST",
		"\\script\\ui\\uiauction_house.lua",
		"UIAuctionHouse:OnActivityListResponse",
		{OBJTYPE_NUMBER, OBJTYPE_TABLE},
	},
	{
		"emSCRIPT_PROTOCOL_AUCTION_ACTIVITYINFO",
		"\\script\\ui\\uiauction_house.lua",
		"UIAuctionHouse:OnActivityContentResponse",
		{OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_TABLE, OBJTYPE_NUMBER, OBJTYPE_NUMBER, OBJTYPE_NUMBER},
	},
	{
		"emSCRIPT_PROTOCOL_AUCTION_ITEMINFO",
		"\\script\\ui\\uiauction_house.lua",
		"UIAuctionHouse:OnItemContentResponse",
		{OBJTYPE_TABLE},
	},
	{
		"emSCRIPT_PROTOCOL_AUCTION_MEMBERLIST",
		"\\script\\ui\\uiauction_house.lua",
		"UIAuctionHouse:OnMemberListResponse",
		{OBJTYPE_NUMBER, OBJTYPE_TABLE},
	},
	{
		"emSCRIPT_PROTOCOL_AUCTION_NEWACTIVITY",
		"\\script\\ui\\uiauction_house.lua",
		"UIAuctionHouse:OnNewActivityEvent",
		{OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER},
	},
	{
		"emSCRIPT_PROTOCOL_AUCTION_ENDACTIVITY",
		"\\script\\ui\\uiauction_house.lua",
		"UIAuctionHouse:OnEndActivityEvent",
		{OBJTYPE_NUMBER, OBJTYPE_STRING},
	},
	{
		"emSCRIPT_PROTOCOL_AUCTION_NEWITEM",
		"\\script\\ui\\uiauction_house.lua",
		"UIAuctionHouse:OnNewItemEvent",
		{OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER, OBJTYPE_NUMBER},
	},
	{
		"emSCRIPT_PROTOCOL_AUCTION_PUTONOK",
		"\\script\\ui\\uiauction_house.lua",
		"UIAuctionHouse:OnPutOnOk",
		{OBJTYPE_NUMBER, OBJTYPE_STRING},
	},
	{
		"emSCRIPT_PROTOCOL_AUCTION_ENDITEM",
		"\\script\\ui\\uiauction_house.lua",
		"UIAuctionHouse:OnEndItemEvent",
		{OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER, OBJTYPE_NUMBER},
	},
	{
		"emSCRIPT_PROTOCOL_AUCTION_TPRICECHANGE",
		"\\script\\ui\\uiauction_house.lua",
		"UIAuctionHouse:OnPriceChangeEvent",
		{OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER, OBJTYPE_NUMBER, OBJTYPE_NUMBER, OBJTYPE_NUMBER, OBJTYPE_NUMBER},
	},
	{
		"emSCRIPT_PROTOCOL_AUCTION_OPENWND",
		"\\script\\ui\\uiauction_house.lua",
		"UIAuctionHouse:OnOpenFromServer",
		{OBJTYPE_NUMBER},
	},
}

ScriptProtocol:RegProtocolSet(Def)

-- client -> may chu (KScriptProtocol.cpp: SendScriptDataToServer)
function ScriptProtocol:SendData(szEnum, nHandle)
	if (type(self[szEnum]) == "number") then
		return SendScriptDataToServer(self[szEnum], nHandle)
	end
	return 0
end

-- ECHO thu kenh: may chu gui chuoi -> hien len khung thoai, roi gui tra lai mot lan
-- (may chu chi ghi log + Msg2Player, KHONG gui lai nua nen khong lap vo tan).
function ScriptProtocol_EchoArrival(szText)
	Talk(1, "", "[ECHO] "..(szText or ""))
	local h = OB_Create()
	ObjBuffer:PushByType(h, OBJTYPE_STRING, "client da nhan: "..(szText or ""))
	ScriptProtocol:SendData("emSCRIPT_PROTOCOL_ECHO", h)
	OB_Release(h)
end
