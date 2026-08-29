Include("\\script\\ui\\liansaigamble.lua")
Include("\\script\\ui\\liansaienterance.lua")
Include("\\script\\event\\liansai\\head.lua")
Include("\\script\\lib\\common.lua")
Include("\\script\\lib\\objbuffer_head.lua")
Include("\\script\\protocol.lua")

tbLianSai_c = {}

function tbLianSai_c:SyncReport(szState, nSec)
    LianSai_SyncReportData(szState, nSec)
end

function tbLianSai_c:CloseReport()
    LianSai_CloseReport()
end

function tbLianSai_c:SycnTeamList(tbTeamList, nNowState, nCounter, nChooseTeam, nFinalTeamCount)
    self.nNowState = nNowState
    UiManage:OpenWindow("UI_LIANSAIGAMBLE")
    local tbLianSaiGambleUI = UiManage:GetClass("UI_LIANSAIGAMBLE")
    if nFinalTeamCount == 16 then
        tbLianSaiGambleUI:HidePageTwo()
    else
        tbLianSaiGambleUI:ShowPageTwo()
    end
    tbLianSaiGambleUI:InitUI()
    AddTimer(0, "tbLianSai_c:OnTime", nCounter)
    for i = 1, nFinalTeamCount do
        local szTeamName = tbTeamList[(i - 1) * 4 + 1] or ""
        local nState = tbTeamList[(i - 1) * 4 + 2] or BATTLE_LOST
        local nRound = tbTeamList[(i - 1) * 4 + 3] or 1
        local nChooseCount = tbTeamList[(i - 1) * 4 + 4] or 0

        if i <= 8 then
            tbLianSaiGambleUI:DrawLeftWinLine_PageOne(i, nRound, nState)
        elseif i > 8 and i <= 16 then
            tbLianSaiGambleUI:DrawRightWinLine_PageOne(i - 8, nRound, nState)
        elseif nFinalTeamCount == 32 and i > 16 and i <= 24 then
            tbLianSaiGambleUI:DrawLeftWinLine_PageTwo(i - 16, nRound, nState)
        elseif nFinalTeamCount == 32 and i > 24 and i <= 32 then
            tbLianSaiGambleUI:DrawRightWinLine_PageTwo(i - 24, nRound, nState)
        end

        if i <= 16 then
            tbLianSaiGambleUI:SetTeamData_PageOne(i, szTeamName, nRound, nState, nChooseCount)
            if self.nNowState == tbLianSai_config.STATE_NONE then
                tbLianSaiGambleUI:SetButtonEnable_PageOne(i, 0)
            elseif self.nNowState == tbLianSai_config.STATE_FIGHT and i ~= nChooseTeam then
                tbLianSaiGambleUI:SetButtonEnable_PageOne(i, 0)
            elseif i ~= nChooseTeam and nChooseTeam ~= 0 then
                tbLianSaiGambleUI:SetButtonEnable_PageOne(i, 0)
            elseif i == nChooseTeam then
                tbLianSaiGambleUI:SetButtonEnable_PageOne(i, 0)
                tbLianSaiGambleUI:SetButtonCheck_PageOne(i, 1)
                tbLianSaiGambleUI:SetButtonLabel_PageOne(i, "ß∑ Æ∆t c≠Óc")
            end
        elseif nFinalTeamCount == 32 and i > 16 then
            tbLianSaiGambleUI:SetTeamData_PageTwo(i - 16, szTeamName, nRound, nState, nChooseCount)
            if self.nNowState == tbLianSai_config.STATE_NONE then
                tbLianSaiGambleUI:SetButtonEnable_PageTwo(i - 16, 0)
            elseif self.nNowState == tbLianSai_config.STATE_FIGHT and i ~= nChooseTeam then
                tbLianSaiGambleUI:SetButtonEnable_PageTwo(i - 16, 0)
            elseif i ~= nChooseTeam and nChooseTeam ~= 0 then
                tbLianSaiGambleUI:SetButtonEnable_PageTwo(i - 16, 0)
            elseif i == nChooseTeam then
                tbLianSaiGambleUI:SetButtonEnable_PageTwo(i - 16, 0)
                tbLianSaiGambleUI:SetButtonCheck_PageTwo(i - 16, 1)
                tbLianSaiGambleUI:SetButtonLabel_PageTwo(i - 16, "ß∑ Æ∆t c≠Óc")
            end
        end
    end
end

function tbLianSai_c:OnTime(nCounter)
    if 1 ~= UiManage:IsWindowVisible("UI_LIANSAIGAMBLE") then
		return 0, 0
    end

    local tbLianSaiGambleUI = UiManage:GetClass("UI_LIANSAIGAMBLE")
    local nMin = floor(nCounter/60)
    local nSec = mod(nCounter, 60)

    if self.nNowState == tbLianSai_config.STATE_READY then
        tbLianSaiGambleUI:SetGambleTxt_PageOne("D˘ Æo∏n cﬂn")
        tbLianSaiGambleUI:SetGambleTime_PageOne(format("%02d:%02d", nMin, nSec))
        tbLianSaiGambleUI:SetGambleTxt_PageTwo("D˘ Æo∏n cﬂn")
        tbLianSaiGambleUI:SetGambleTime_PageTwo(format("%02d:%02d", nMin, nSec))
    elseif self.nNowState == tbLianSai_config.STATE_FIGHT then
        tbLianSaiGambleUI:SetGambleTxt_PageOne("Chi’n Æ u cﬂn")
        tbLianSaiGambleUI:SetGambleTime_PageOne(format("%02d:%02d", nMin, nSec))
        tbLianSaiGambleUI:SetGambleTxt_PageTwo("Chi’n Æ u cﬂn")
        tbLianSaiGambleUI:SetGambleTime_PageTwo(format("%02d:%02d", nMin, nSec))
    else
        tbLianSaiGambleUI:SetGambleTxt_PageOne("ß∑ k’t thÛc Æ∆t c≠Óc")
        tbLianSaiGambleUI:SetGambleTime_PageOne("00:00")
        tbLianSaiGambleUI:SetGambleTxt_PageTwo("ß∑ k’t thÛc Æ∆t c≠Óc")
        tbLianSaiGambleUI:SetGambleTime_PageTwo("00:00")
        return 0, 0
    end

    if mod(nCounter, 10) == 0 then
		ScriptProtocol:SendData("emSCRIPT_PROTOCOL_C2S_LIANSAI_REFREASH", 0)
        return 0 ,0
    end

	return 18 , nCounter - 1
end

function tbLianSai_c:OpenEntrance()
    UiManage:OpenWindow("UI_LIANSAIENTRANCE")
end

function tbLianSai_c:CloseEntrance()
    UiManage:CloseWindow("UI_LIANSAIENTRANCE")
end