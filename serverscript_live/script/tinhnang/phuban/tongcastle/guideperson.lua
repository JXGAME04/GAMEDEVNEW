-- [LOCAL54 06/09 toi] 9 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local QueryTreePoint, GiveGuardToken, EnterCastle, CheckPlayer, UpdateTreePoint, s2rExchangeTreePoint, PlayerGetTreePoint, ApplyTreeToken, GiveTreeToken
Include("\\script\\header\\cauhinh_hoatdong.lua")	-- [HD CONFIG 24/08] gate / gia doi Thanh Bao chinh duoc
Include("\\script\\dailogsys\\g_dialog.lua")	-- [TONGCASTLE 23/08] thay npcdailog (keo 18 tep bonusvlmc): DailogClass
Include("\\script\\dailogsys\\dailogsay.lua")	-- g_AskClientNumberEx
Include("\\script\\activitysys\\g_activity.lua")	-- G_ACTIVITY (FunSet rong tren JX1)
Include("\\script\\missions\\basemission\\lib.lua")
Include("\\script\\lib\\common.lua")
Include("\\script\\lib\\log.lua")
Include("\\script\\lib\\objbuffer_head.lua")
Include("\\script\\lib\\awardtemplet.lua")
Include("\\script\\global\\autoexec_head.lua")
Include("\\script\\missions\\tongcastle\\tongcastle.lua")
Include("\\script\\missions\\tongcastle\\castleplayer.lua")
Include("\\script\\lib\\log.lua")
--3399¸öÈËÉñÄ¾µã
TSK_SAVE_DATE = 4057
TSK_TREEPOINT = 3399
TSK_GET_AWARD = 4061

JOIN_TONG_TIME = HD_CFG("TC_VAO_BANG_PHUT", 1440)
TRANSLIFE_COUNT = 4
MIN_LEVEL = 150
MIN_STAY_TIME = HD_CFG("TC_PHUT_TOITHIEU", 45)

AWARD_STATE_TIME = HD_CFG("TC_GIO_NHANDIEM_TU", 1900)
AWARD_END_TIME = HD_CFG("TC_GIO_NHANDIEM_DEN", 2400)

-- [TONGCASTLE 23/08] chinh sach: bo trung sinh 4 + cap 150, giu 'vao bang > 1 ngay'
MSG_PLAYER_LIMIT = "Ph¶i ®¹t cÊp "..HD_CFG("TC_CAP_TOITHIEU", 90).." trë lªn vµ vµo bang nhiÒu h¬n 1 ngµy míi cã thÓ tham gia Bang Héi Thµnh B¶o"

local tbTreeItem = {
	[1] = {szName = "Thanh §ång ThÇn Méc LÖnh", nId = 3205, nPrice = HD_CFG("TC_GIA_LENH_1", 10),},
	[2] = {szName = "B¹ch Ng©n ThÇn Méc LÖnh", nId = 3206, nPrice = HD_CFG("TC_GIA_LENH_2", 20),},
	[3] = {szName = "Hoµng Kim ThÇn Méc LÖnh", nId = 3207, nPrice = HD_CFG("TC_GIA_LENH_3", 120),},
}
--(nÎªÕ¼Áì³ÇÊÐ:·ïÏè 1,³É¶¼ 2,´óÀí 3,ãê¾© 4,ÏåÑô 5,ÑïÖÝ 6,ÁÙ°² 7)
local tbCity = {
	--[37] 	= {4, 981},
	[176]	= {7, 984},
}

local tbTranPos = {
	{1638, 3851},
	{1647, 3174},
	{917,	3106},
	{983, 3884},
}

local tbGuideNpc = 
{
		szName = "Ng­êi chØ dÉn bang héi thµnh b¶o",
		nNpcId = 108,
		nLevel = 95,
		tbPos = 
		{		
				{176, 1663, 3262,},
				--{37, 1701, 3216,},
		},
		szScriptPath = "\\script\\missions\\tongcastle\\guideperson.lua",
}

local tbGuideNpcInside = 
{
		szName = "Ng­êi chØ dÉn bang héi thµnh b¶o",
		nNpcId = 108,
		nLevel = 95,
		tbPos = 
		{		
			{981,1093,3223},
			{981,1029,3285},
			{981,1050,3718},
			{981,1114,3780},
			{981,1506,3766},
			{981,1572,3704},
			{981,1562,3321},
			{981,1499,3260},
			{981,1331,3492},

			{984,1093,3223},
			{984,1029,3285},
			{984,1050,3718},
			{984,1114,3780},
			{984,1506,3766},
			{984,1572,3704},
			{984,1562,3321},
			{984,1499,3260},
			{984,1331,3492},
		},
		szScriptPath = "\\script\\missions\\tongcastle\\guideperson_inside.lua",
}
local szDlgText = "<npc>Chµo c¸c b¹n, t«i lµ ng­êi chØ dÉn bang héi thµnh b¶o"
szDlgText = szDlgText.."<enter> thêi gian më cöa cña Thµnh B¶o Bang Héi lµ chñ nhËt (L©m An) tõ 17:00 ®Õn 19:00, trong ®ã Nh©n Tù Phßng më cöa lóc 17:00, §Þa Tù Phßng më cöa lóc 18:00, Thiªn Tù Phßng më cöa lóc 18:30."
function main()
	local nNpcIndex = GetLastDiagNpc()
	local szNpcName = GetNpcName(nNpcIndex)
	if NpcName2Replace then
		szNpcName = NpcName2Replace(szNpcName)
	end

	local tbDailog = DailogClass:new(szNpcName)
	tbDailog.szTitleMsg = szDlgText
	tbDailog:AddOptEntry("Ta muèn vµo bang héi thµnh b¶o thµnh nµy", EnterCastle, {nNpcIndex})
	tbDailog:AddOptEntry("Ta muèn nhËn ThÇn Méc LÖnh", ApplyTreeToken, {})
	tbDailog:AddOptEntry("Ta muèn nhËn ®iÓm ThÇn Méc", s2rExchangeTreePoint, {})
	tbDailog:AddOptEntry("Ta muèn xem sè ®iÓm thÇn méc cña m×nh", QueryTreePoint)
	
	G_ACTIVITY:OnMessage("ClickNpc", tbDailog)
	tbDailog:Show()
end

function QueryTreePoint()
	UpdateTreePoint()
	local nTreePoint = GetTask(TSK_TREEPOINT)
	Talk(1, "", format("Sè ®iÓm thÇn méc cña ng­¬i lµ %d ®iÓm", nTreePoint))
end

function ApplyGuardToken()
	g_AskClientNumberEx(1, HD_CFG("TC_MUA_BUA_TOIDA", 100), "NhËp vµo sè l­îng muèn nhËn", {GiveGuardToken, {}})
end

function GiveGuardToken(nCount)
	local nPrice = HD_CFG("TC_GIA_BUA", 200)
	local nTreePoint = GetTask(TSK_TREEPOINT)
	if CalcFreeItemCellCount() < ceil(nCount/50) then
		Talk(1, "", "Chç trèng hµnh trang kh«ng ®ñ.")
		return
	end
	if nTreePoint < nPrice*nCount then
		Talk(1, "", "§iÓm thÇn méc cña ng­¬i kh«ng ®ñ")
		return
	end
	SetTask(TSK_TREEPOINT, nTreePoint-nPrice*nCount)
	local tbAward = {
		{szName = "Bang héi thµnh b¶o Thñ VÖ triÖu håi phï", tbProp = {6, 1, 3204, 1, 0, 0}, nCount = nCount,},
	}
	tbAwardTemplet:GiveAwardByList(tbAward, "NhËn ®­îc bang héi thµnh b¶o Thñ VÖ triÖu håi phï")
end

function CreateGuideNpc()
	local tbNpc = tbGuideNpc
	local nPosPathCount = getn(tbGuideNpc.tbPos)
	for i = 1, nPosPathCount do
		local nMapId, nX, nY = unpack(tbGuideNpc.tbPos[i])
		basemission_CallNpc(tbNpc, nMapId, nX*32, nY*32)
	end
end

function CreateGuideNpcInside()
	local tbNpc = tbGuideNpcInside
	local nPosPathCount = getn(tbGuideNpcInside.tbPos)
	for i = 1, nPosPathCount do
		local nMapId, nX, nY = unpack(tbGuideNpcInside.tbPos[i])
		basemission_CallNpc(tbNpc, nMapId, nX*32, nY*32)
	end
end

function EnterCastle(nNpcIndex)
	local szTongName = GetTongName()
	if szTongName == nil or szTongName == "" then
		Talk(1, "", "Ng­¬i vÉn ch­a gia nhËp bang héi, kh«ng thÓ vµo")
		return
	end
	local nX32,nY32, nMapIndex = GetNpcPos(nNpcIndex)
	local nMapId = SubWorldIdx2ID(nMapIndex)
	local nCityId = tbCity[nMapId][1]
	local szCityTong, _ = GetCityOwner(nCityId)
	if szCityTong == nil or szCityTong == "" then
		Talk(1, "", "Thµnh nµy t¹m thêi kh«ng cã bang héi chiÕm gi÷, kh«ng thÓ vµo bang héi thµnh b¶o thµnh nµy")
		return
	end
	local nWeek = tonumber(GetLocalDate("%w"))
	local bOpend = TongCastle:IsInOpenTime("castle")
	local bInOpenDay = TongCastle:IsInOpenDay("castle", nMapId)
	if not (bOpend and bInOpenDay) then
		Talk(1, "", "thêi gian më cöa cña Thµnh B¶o Bang Héi lµ chñ nhËt (L©m An) tõ 17:00 ®Õn 19:00, thêi gian kh¸c kh«ng ®­îc vµo.")
		return
	end
	
	if CheckPlayer() ~= 1 then
		return
	end
	
	local nTransMapId = tbCity[nMapId][2]
	local szTongName = GetTongName()
	local _, nTmpX, nTmpY = GetWorldPos()
	SetTempRevPos(nMapId, nTmpX*32, nTmpY*32)	
	if szCityTong == szTongName then
		NewWorld(nTransMapId, 1311, 3515)
	else
		local nX, nY = unpack(tbTranPos[random(1, getn(tbTranPos))])
		NewWorld(nTransMapId, nX, nY)
	end
	tbLog:PlayerActionLog("LOGTINHNANGJX1_npc_BHTB","BaodanhThamgiaBHTB" )
end

function CheckPlayer()
	-- [TONGCASTLE 23/08] bo trung sinh, cong cap >= 90
	if (GetLevel() < HD_CFG("TC_CAP_TOITHIEU", 90)) then
		Talk(1, "", MSG_PLAYER_LIMIT)
		return
	end
	
	if GetJoinTongTime() < JOIN_TONG_TIME then
		Talk(1, "", MSG_PLAYER_LIMIT)
		return
	end
	
	return 1
end

function UpdateTreePoint()
	local nToday = tonumber(GetLocalDate("%y%m%d"))
	if GetTask(TSK_SAVE_DATE) ~= nToday then
		SetTask(TSK_SAVE_DATE, nToday)
		SetTask(TSK_TREEPOINT, 0)
	end
end

function s2rExchangeTreePoint()
	local nX32,nY32, nMapIndex = GetNpcPos(nNpcIndex)
	local nMapId = SubWorldIdx2ID(nMapIndex)
	local szTongName = GetTongName()
	local szPlayerName = GetName()
	
	if CheckPlayer() ~= 1 then
		return
	end
	
	local nCurTime = tonumber(GetLocalDate("%H%M"))
	if nCurTime < AWARD_STATE_TIME or nCurTime >= AWARD_END_TIME then
		Talk(1, "", "ChØ cã thÓ nhËn ®iÓm ThÇn Méc vµo lóc "..HD_GioPhut(HD_CFG("TC_GIO_NHANDIEM_TU", 1900)).." ®Õn "..HD_GioPhut(HD_CFG("TC_GIO_NHANDIEM_DEN", 2400))..".")
		return
	end

	if CastlePlayer:GetStayTime() < MIN_STAY_TIME then
		Talk(1, "", format("Tham gia Bang Héi Thµnh B¶o %d phót trë lªn míi cã thÓ nhËn th­ëng.", MIN_STAY_TIME))
		return
	end
	
	if PlayerFunLib:GetTaskDaily(TSK_GET_AWARD, 1) > 0 then
		Talk(1, "", "HiÖn t¹i ng­¬i kh«ng cã ®iÓm ThÇn Méc ®Ó nhËn, cã ph¶i ng­¬i ®· nhËn råi kh«ng?")
		return
	end

	local handle = OB_Create()
	ObjBuffer:PushObject(handle, szPlayerName)
	ObjBuffer:PushObject(handle, nMapId)
	ObjBuffer:PushObject(handle, szTongName)
	RemoteExecute("\\script\\mission\\tongcastle\\tongcastle.lua", "tbS3TongCastle:GetTreePoint", handle)
	OB_Release(handle)
end

function PlayerGetTreePoint(nPlayerPoint, nTongPoint)
	if CheckPlayer() ~= 1 then
		return
	end
	
	if nTongPoint == 0 and nPlayerPoint == 0 then
		Talk(1, "", "ThËt tiÕc, ng­¬i vµ bang héi cña ng­¬i trong lÇn nµy kh«ng nhËn ®­îc ®iÓm ThÇn Méc. LÇn sau ph¶i cè g¾ng råi.")
		return
	end
	
	PlayerFunLib:AddTaskDaily(TSK_GET_AWARD, 1)
	
	UpdateTreePoint()
	local nAddPoint = nPlayerPoint + nTongPoint
	SetTask(TSK_TREEPOINT, GetTask(TSK_TREEPOINT) + nAddPoint)
	tbLog:PlayerActionLog("LOGTINHNANGJX1_npc_BHTB","NhanDiemThanMocThanhCong ")	
	Talk(1, "", format("Chóc mõng ng­¬i nhËn ®­îc %d ®iÓm ®iÓm ThÇn Méc",nAddPoint))
end

function ExchangeDialog(ParamHandle)
	local szPlayerName = ObjBuffer:PopObject(ParamHandle)
	local nMapId = ObjBuffer:PopObject(ParamHandle)
	local nPlayerPoint = ObjBuffer:PopObject(ParamHandle)
	local nTongPoint = ObjBuffer:PopObject(ParamHandle)
	
	local nPlayerIndex = SearchPlayer(szPlayerName)
	if nPlayerIndex > 0 then
		CallPlayerFunction(nPlayerIndex, PlayerGetTreePoint, nPlayerPoint, nTongPoint)
	end
end

function ApplyTreeToken()
	UpdateTreePoint()
	local nTreePoint = GetTask(TSK_TREEPOINT)
	local szTitle = format("Tæng ®iÓm ThÇn Méc cßn l¹i: %d", nTreePoint)
	local nMaxCount = HD_CFG("TC_DOI_LENH_TOIDA", 100)
	local tbOpt = {}
	for i = 1, getn(tbTreeItem) do
		local szOption = format("NhËn %s", tbTreeItem[i].szName)
		tinsert(tbOpt, {szOption, g_AskClientNumberEx, {1, nMaxCount, "NhËp vµo sè l­îng muèn nhËn", {GiveTreeToken, {i, tbTreeItem[i].nPrice}}}})
	end
	tinsert(tbOpt, {" KÕt thóc ®èi tho¹i!"})
	CreateNewSayEx(szTitle, tbOpt)
end

function GiveTreeToken(nType, nPrice, nCount)
	if CalcFreeItemCellCount() < nCount then
		Talk(1, "", "Chç trèng hµnh trang kh«ng ®ñ.")
		return
	end
	UpdateTreePoint()
	local nTreePoint = GetTask(TSK_TREEPOINT)
	local nRemainPoint = nTreePoint - nCount * nPrice
	
	if nRemainPoint < 0 then
		Talk(1, "", "§iÓm thÇn méc kh«ng ®ñ, kh«ng thÓ ®æi lÖnh bµi")
	else
		local szItemName = tbTreeItem[nType].szName
		local nItemId = tbTreeItem[nType].nId
		local tbAward = {
			{szName = szItemName, tbProp = {6, 1, nItemId, 1, 0, 0}, nCount = nCount,nBindState=-2,nExpiredTime=10080},
		}
		SetTask(TSK_TREEPOINT, nRemainPoint)
		tbAwardTemplet.GiveAwardByList(tbAwardTemplet, tbAward, "TongCastle")
		tbLog:PlayerActionLog("TongCastle", "BuyToken", "Type:"..nType, "Price:"..nPrice, "nCount:"..nCount)
	end
end

function ResHaha(ParamHandle)
	local szPlayerName = ObjBuffer:PopObject(ParamHandle)
	local szMsg = ObjBuffer:PopObject(ParamHandle)
	local nPlayerIndex = SearchPlayer(szPlayerName)
	if nPlayerIndex > 0 then
		CallPlayerFunction(nPlayerIndex, Msg2Player, szMsg)
	end
end

AutoFunctions:Add(CreateGuideNpc)
AutoFunctions:Add(CreateGuideNpcInside)
