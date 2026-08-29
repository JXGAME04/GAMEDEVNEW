------------------------------------------------------
-- ÎÄ¼þÃû¡¡£ºliansaigamble.lua
-- ´´½¨Õß¡¡£º
-- ´´½¨Ê±¼ä£º2020-09-27-11-29
-- Ãè  Êö  £ºÁªÈüÑº×¢
------------------------------------------------------
if 1 ~= MODEL_GAMECLIENT then
	return
end

Include("\\script\\lib\\common.lua")
Include("\\script\\lib\\objbuffer_head.lua")
Include("\\script\\protocol.lua")
Include("\\script\\ui\\manage.lua")
Include("\\script\\event\\liansai\\head.lua")

local tbClass	= {}

tbClass.UIGROUP	= "UI_LIANSAIGAMBLE"

tbClass.UICOMP	= {
	BTN_CLOSE = "BtnClose",
	TXT_TEAMNAMEONE = "TeamNameOne_%d",
	IMG_WINLINEONELEFTV = "LineOneLeft_V_%d_%d",
	IMG_WINLINEONELEFTH = "LineOneLeft_H_%d_%d",
	IMG_WINLINEONELEFTFINAL = "LineOneLeft_H_4",
	IMG_WINLINEONERIGHTV = "LineOneRight_V_%d_%d",
	IMG_WINLINEONERIGHTH = "LineOneRight_H_%d_%d",
	IMG_WINLINEONERIGHTFINAL = "LineOneRight_H_4",
	IMG_TEAMDATAONE = "TeamImgOne_%d",
	BTN_GAMBLETEAMONE = "TeamButtonOne_%d",
	TXT_TEAMGAMBLECOUNTONE = "TeamGambleCountOne_%d",
	TXT_COUNTERONE = "TxtCounterOne",
	TXT_TIMERDESCONE = "TxtTimerOne",

	TXT_TEAMNAMETWO = "TeamNameTwo_%d",
	IMG_WINLINETWOLEFTV = "LineTwoLeft_V_%d_%d",
	IMG_WINLINETWOLEFTH = "LineTwoLeft_H_%d_%d",
	IMG_WINLINETWOLEFTFINAL = "LineTwoLeft_H_4",
	IMG_WINLINETWORIGHTV = "LineTwoRight_V_%d_%d",
	IMG_WINLINETWORIGHTH = "LineTwoRight_H_%d_%d",
	IMG_WINLINETWORIGHTFINAL = "LineTwoRight_H_4",
	IMG_TEAMDATATWO = "TeamImgTwo_%d",
	BTN_GAMBLETEAMTWO = "TeamButtonTwo_%d",
	TXT_TEAMGAMBLECOUNTTWO = "TeamGambleCountTwo_%d",
	TXT_COUNTERTWO = "TxtCounterTwo",
	TXT_TIMERDESCTWO = "TxtTimerTwo",

	TXT_WIN = "TxtWin_%d",
	TXT_WINTEAM = "TxtWinTeam_%d",

	BTN_PAGEBUTTON_ONE = "ButtonPageOne",
	BTN_PAGEBUTTON_TWO = "ButtonPageTwo",
	PAGE_GAMBLE_TWO = "PageGambleTwo",
	TXT_QUALIFY_ONE = "TxtQualifyOne",
	PAGESET_GAMBLE = "PageSetGamble",
}

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
	self:InitUI()
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

function tbClass:InitUI()
	local nTemp = 16
	for i = 1, 3 do
		nTemp = nTemp / 2
		for j = 1, nTemp do
			local szLineUI = format(self.UICOMP.IMG_WINLINEONELEFTV, i, j)
			Wnd_Hide(self.UIGROUP, szLineUI)
			szLineUI = format(self.UICOMP.IMG_WINLINEONELEFTH, i, j)
			Wnd_Hide(self.UIGROUP, szLineUI)

			szLineUI = format(self.UICOMP.IMG_WINLINETWOLEFTV, i, j)
			Wnd_Hide(self.UIGROUP, szLineUI)
			szLineUI = format(self.UICOMP.IMG_WINLINETWOLEFTH, i, j)
			Wnd_Hide(self.UIGROUP, szLineUI)
		end
	end
	Wnd_Hide(self.UIGROUP, self.UICOMP.IMG_WINLINEONELEFTFINAL)
	Wnd_Hide(self.UIGROUP, self.UICOMP.IMG_WINLINETWOLEFTFINAL)

	nTemp = 16
	for i = 1, 3 do
		nTemp = nTemp / 2
		for j = 1, nTemp do
			local szLineUI = format(self.UICOMP.IMG_WINLINEONERIGHTV, i, j)
			Wnd_Hide(self.UIGROUP, szLineUI)
			szLineUI = format(self.UICOMP.IMG_WINLINEONERIGHTH, i, j)
			Wnd_Hide(self.UIGROUP, szLineUI)

			szLineUI = format(self.UICOMP.IMG_WINLINETWORIGHTV, i, j)
			Wnd_Hide(self.UIGROUP, szLineUI)
			szLineUI = format(self.UICOMP.IMG_WINLINETWORIGHTH, i, j)
			Wnd_Hide(self.UIGROUP, szLineUI)
		end
	end
	Wnd_Hide(self.UIGROUP, self.UICOMP.IMG_WINLINEONERIGHTFINAL)
	Wnd_Hide(self.UIGROUP, self.UICOMP.IMG_WINLINETWORIGHTFINAL)

	for nIndex = 1, 16 do
		Wnd_SetEnable(self.UIGROUP, format(self.UICOMP.BTN_GAMBLETEAMONE, nIndex), 1)
		Btn_SetLabel(self.UIGROUP, format(self.UICOMP.BTN_GAMBLETEAMONE, nIndex), "§Æt c­îc")
		Btn_Check(self.UIGROUP, format(self.UICOMP.BTN_GAMBLETEAMONE, nIndex), 0)
		Img_SetFrame(self.UIGROUP, format(self.UICOMP.IMG_TEAMDATAONE, nIndex), 0)

		Wnd_SetEnable(self.UIGROUP, format(self.UICOMP.BTN_GAMBLETEAMTWO, nIndex), 1)
		Btn_SetLabel(self.UIGROUP, format(self.UICOMP.BTN_GAMBLETEAMTWO, nIndex), "§Æt c­îc")
		Btn_Check(self.UIGROUP, format(self.UICOMP.BTN_GAMBLETEAMTWO, nIndex), 0)
		Img_SetFrame(self.UIGROUP, format(self.UICOMP.IMG_TEAMDATATWO, nIndex), 0)
	end

	for i = 1, 2 do
		Txt_SetTxt(self.UIGROUP, format(self.UICOMP.TXT_WINTEAM, i), "§ang tranh giµnh quyÕt liÖt")
	end
end

function tbClass:HidePageTwo()
	Wnd_Hide(self.UIGROUP, self.UICOMP.BTN_PAGEBUTTON_ONE)
	Wnd_Hide(self.UIGROUP, self.UICOMP.BTN_PAGEBUTTON_TWO)
	-- Wnd_Hide(self.UIGROUP, self.UICOMP.PAGE_GAMBLE_TWO)
	self.nFinalTeamCount = 16
	Txt_SetTxt(self.UIGROUP, self.UICOMP.TXT_QUALIFY_ONE, "Qu¸n Qu©n Liªn §Êu Míi")
end

function tbClass:ShowPageTwo()
	Wnd_Show(self.UIGROUP, self.UICOMP.BTN_PAGEBUTTON_ONE)
	Wnd_Show(self.UIGROUP, self.UICOMP.BTN_PAGEBUTTON_TWO)
	self.nFinalTeamCount = 32
	Txt_SetTxt(self.UIGROUP, self.UICOMP.TXT_QUALIFY_ONE, "T­ c¸ch tham dù chung kÕt")
end

function tbClass:SetGambleTxt_PageOne(szTxt)
	Txt_SetTxt(self.UIGROUP, self.UICOMP.TXT_TIMERDESCONE, szTxt)
end

function tbClass:SetGambleTxt_PageTwo(szTxt)
	Txt_SetTxt(self.UIGROUP, self.UICOMP.TXT_TIMERDESCTWO, szTxt)
end

function tbClass:SetGambleTime_PageOne(szTime)
	Txt_SetTxt(self.UIGROUP, self.UICOMP.TXT_COUNTERONE, szTime)
end

function tbClass:SetGambleTime_PageTwo(szTime)
	Txt_SetTxt(self.UIGROUP, self.UICOMP.TXT_COUNTERTWO, szTime)
end

function tbClass:OnLBClick(szWnd, nParam)
	-- if szWnd == self.UICOMP.BTN_CLOSE then
	-- 	UiManage:CloseWindow(self.UIGROUP)
	-- end
end

function tbClass:OnLBClick(szWnd, nParam)
	local szButton1 = gsub(szWnd, "TeamButtonOne_", "")
	local szButton2 = gsub(szWnd, "TeamButtonTwo_", "")
	if szWnd == self.UICOMP.BTN_CLOSE then
		UiManage:CloseWindow(self.UIGROUP)
	elseif strlen(szButton1) <= 2 then
		local nTeamIndex = tonumber(szButton1)
		local handle = OB_Create()
		ObjBuffer:PushByType(handle, OBJTYPE_NUMBER, nTeamIndex)
		ScriptProtocol:SendData("emSCRIPT_PROTOCOL_C2S_LIANSAI_SETGAMBLE", handle)
		OB_Release(handle)
	elseif strlen(szButton2) <= 2 then
		local nTeamIndex = tonumber(szButton2)
		local handle = OB_Create()
		ObjBuffer:PushByType(handle, OBJTYPE_NUMBER, nTeamIndex + 16)
		ScriptProtocol:SendData("emSCRIPT_PROTOCOL_C2S_LIANSAI_SETGAMBLE", handle)
		OB_Release(handle)
	end
end

function tbClass:SetTeamData_PageOne(nIndex, szTeamName, nRound, nState, nChooseCount)
	if szTeamName == "" then
		Wnd_SetEnable(self.UIGROUP, format(self.UICOMP.BTN_GAMBLETEAMONE, nIndex), 0)
	end
	local szUi = format(self.UICOMP.TXT_TEAMNAMEONE, nIndex)
	Txt_SetTxt(self.UIGROUP, szUi, szTeamName)
	szUi = format(self.UICOMP.TXT_TEAMGAMBLECOUNTONE, nIndex)
	Txt_SetTxt(self.UIGROUP, szUi, format("%d", nChooseCount))
	local nFinalRound = self.nFinalTeamCount == 32 and 6 or 5
	if nState == BATTLE_LOST then
		Img_SetFrame(self.UIGROUP, format(self.UICOMP.IMG_TEAMDATAONE, nIndex), 1)
		self:SetButtonEnable_PageOne(nIndex, 0)
	elseif nState == BATTLE_WIN and nRound == nFinalRound then
		Img_SetFrame(self.UIGROUP, format(self.UICOMP.IMG_TEAMDATAONE, nIndex), 2)
		for i = 1, 2 do
			Txt_SetTxt(self.UIGROUP, format(self.UICOMP.TXT_WINTEAM, i), szTeamName)
		end
	elseif nState == BATTLE_WIN then
		Img_SetFrame(self.UIGROUP, format(self.UICOMP.IMG_TEAMDATAONE, nIndex), 0)
	end
end

function tbClass:SetTeamData_PageTwo(nIndex, szTeamName, nRound, nState, nChooseCount)
	if szTeamName == "" then
		Wnd_SetEnable(self.UIGROUP, format(self.UICOMP.BTN_GAMBLETEAMTWO, nIndex), 0)
	end
	local szUi = format(self.UICOMP.TXT_TEAMNAMETWO, nIndex)
	Txt_SetTxt(self.UIGROUP, szUi, szTeamName)
	szUi = format(self.UICOMP.TXT_TEAMGAMBLECOUNTTWO, nIndex)
	Txt_SetTxt(self.UIGROUP, szUi, format("%d", nChooseCount))
	local nFinalRound = self.nFinalTeamCount == 32 and 6 or 5
	if nState == BATTLE_LOST then
		Img_SetFrame(self.UIGROUP, format(self.UICOMP.IMG_TEAMDATATWO, nIndex), 1)
		self:SetButtonEnable_PageTwo(nIndex, 0)
	elseif nState == BATTLE_WIN and nRound == nFinalRound then
		Img_SetFrame(self.UIGROUP, format(self.UICOMP.IMG_TEAMDATATWO, nIndex), 2)
		for i = 1, 2 do
			Txt_SetTxt(self.UIGROUP, format(self.UICOMP.TXT_WINTEAM, i), szTeamName)
		end
	elseif nState == BATTLE_WIN then
		Img_SetFrame(self.UIGROUP, format(self.UICOMP.IMG_TEAMDATATWO, nIndex), 0)
	end
end

function tbClass:DrawLeftWinLine_PageTwo(nIndex, nRound, nState)
	if nState == BATTLE_LOST then
		return
	end

	for i = 1, nRound do
		if i == 4 then
			Wnd_Show(self.UIGROUP, self.UICOMP.IMG_WINLINETWOLEFTFINAL)
			break
		end
		local nX = 2 ^ (i - 1)
		local nY = ceil(nIndex/nX)
		local szWinLineUi = format(self.UICOMP.IMG_WINLINETWOLEFTH, i, nY)
		Wnd_Show(self.UIGROUP, szWinLineUi)
		szWinLineUi = format(self.UICOMP.IMG_WINLINETWOLEFTV, i, nY)
		Wnd_Show(self.UIGROUP, szWinLineUi)
	end
end

function tbClass:DrawLeftWinLine_PageOne(nIndex, nRound, nState)
	if nState == BATTLE_LOST then
		return
	end

	for i = 1, nRound do
		if i == 4 then
			Wnd_Show(self.UIGROUP, self.UICOMP.IMG_WINLINEONELEFTFINAL)
			break
		end
		local nX = 2 ^ (i - 1)
		local nY = ceil(nIndex/nX)
		local szWinLineUi = format(self.UICOMP.IMG_WINLINEONELEFTH, i, nY)
		Wnd_Show(self.UIGROUP, szWinLineUi)
		szWinLineUi = format(self.UICOMP.IMG_WINLINEONELEFTV, i, nY)
		Wnd_Show(self.UIGROUP, szWinLineUi)
	end
end

function tbClass:DrawRightWinLine_PageOne(nIndex, nRound, nState)
	if nState == BATTLE_LOST then
		return
	end

	for i = 1, nRound do
		if i == 4 then
			Wnd_Show(self.UIGROUP, self.UICOMP.IMG_WINLINEONERIGHTFINAL)
			break
		end
		local nX = 2 ^ (i - 1)
		local nY = ceil(nIndex/nX)
		local szWinLineUi = format(self.UICOMP.IMG_WINLINEONERIGHTH, i, nY)
		Wnd_Show(self.UIGROUP, szWinLineUi)
		szWinLineUi = format(self.UICOMP.IMG_WINLINEONERIGHTV, i, nY)
		Wnd_Show(self.UIGROUP, szWinLineUi)
	end
end

function tbClass:DrawRightWinLine_PageTwo(nIndex, nRound, nState)
	if nState == BATTLE_LOST then
		return
	end

	for i = 1, nRound do
		if i == 4 then
			Wnd_Show(self.UIGROUP, self.UICOMP.IMG_WINLINETWORIGHTFINAL)
			break
		end
		local nX = 2 ^ (i - 1)
		local nY = ceil(nIndex/nX)
		local szWinLineUi = format(self.UICOMP.IMG_WINLINETWORIGHTH, i, nY)
		Wnd_Show(self.UIGROUP, szWinLineUi)
		szWinLineUi = format(self.UICOMP.IMG_WINLINETWORIGHTV, i, nY)
		Wnd_Show(self.UIGROUP, szWinLineUi)
	end
end

function tbClass:SetAllButtonLabel_PageOne(szLabel)
	for nIndex = 1, 16 do
		Btn_SetLabel(self.UIGROUP, format(self.UICOMP.BTN_GAMBLETEAMONE, nIndex), szLabel)
	end
end

function tbClass:SetAllButtonLabel_PageTwo(szLabel)
	for nIndex = 1, 16 do
		Btn_SetLabel(self.UIGROUP, format(self.UICOMP.BTN_GAMBLETEAMTWO, nIndex), szLabel)
	end
end

function tbClass:SetButtonLabel_PageOne(nIndex, szLabel)
	Btn_SetLabel(self.UIGROUP, format(self.UICOMP.BTN_GAMBLETEAMONE, nIndex), szLabel)
end

function tbClass:SetButtonLabel_PageTwo(nIndex, szLabel)
	Btn_SetLabel(self.UIGROUP, format(self.UICOMP.BTN_GAMBLETEAMTWO, nIndex), szLabel)
end

function tbClass:SetButtonCheck_PageOne(nIndex, nCheck)
	Btn_Check(self.UIGROUP, format(self.UICOMP.BTN_GAMBLETEAMONE, nIndex), nCheck)
end

function tbClass:SetButtonCheck_PageTwo(nIndex, nCheck)
	Btn_Check(self.UIGROUP, format(self.UICOMP.BTN_GAMBLETEAMTWO, nIndex), nCheck)
end

function tbClass:SetButtonEnable_PageOne(nIndex, nEnable)
	Wnd_SetEnable(self.UIGROUP, format(self.UICOMP.BTN_GAMBLETEAMONE, nIndex), nEnable)
end

function tbClass:SetButtonEnable_PageTwo(nIndex, nEnable)
	Wnd_SetEnable(self.UIGROUP, format(self.UICOMP.BTN_GAMBLETEAMTWO, nIndex), nEnable)
end

function tbClass:SetAllButtonEnable_PageOne(nEnable)
	for nIndex = 1, 16 do
		Wnd_SetEnable(self.UIGROUP, format(self.UICOMP.BTN_GAMBLETEAMONE, nIndex), nEnable)
	end
end

function tbClass:SetAllButtonEnable_PageTwo(nEnable)
	for nIndex = 1, 16 do
		Wnd_SetEnable(self.UIGROUP, format(self.UICOMP.BTN_GAMBLETEAMTWO, nIndex), nEnable)
	end
end

UiManage:RegisterClass(tbClass)

