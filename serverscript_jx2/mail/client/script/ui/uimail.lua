-- [MAIL 03/09] uimail.lua = ban client VLTK 2.0 (slistcl.pak uid 9565EFB1) + sua cho JX1 (tim "[MAIL 03/09 JX1]").
-- Cac ham C++ (KMailClient.cpp): OpenMailWindow AddMailHeader SetMailHeader DeleteOneMail CleanMailAll CleanMailList
-- CleanMailDetail UpdateMailCount UpdateMailDetail SetMailBntStatus SetMailIconVisible NewMailUIEventArrival SelectMail
-- SwitchMailManager SetFilterText FormatTime2String MailConfirm Msg2Player.
Include("\\script\\protocol.lua")
Include("\\script\\lib\\objbuffer_head.lua")
Include("\\script\\mail\\maildef.lua")


-- tbMail.szRoleName   = HEADER,ËùÊôÍæ¼Ò£¬¿Í»§¶Ë¿ÉÒÔºöÂÔ
-- tbMail.szSender     = HEADER,·¢¼þÈË
-- tbMail.szTitle      = HEADER,±êÌâ
-- tbMail.szDescribe   = ÄÚÈÝ
-- tbMail.nSendTime    = HEADER,·¢ËÍÊ±¼ä
-- tbMail.nCacheTime   = »º´æÊ±¼ä
-- tbMail.nRecvTime    = ÊÕ¼þÊ±¼ä
-- tbMail.nExpiredTime = HEADER,¹ýÆÚÊ±¼ä
-- tbMail.nState       = HEADER,µ±Ç°×´Ì¬£¬Ïê¼ûÓÚMailDef.lua
-- tbMail.tbAward      = ½±Àø±í
-- tbMail.nAwardCount  = HEADER,½±ÀøÊýÁ¿

-- ?gm ds LoadClientScript("\\script\\ui\\uimail.lua")


UIMail = {}
UIMail.tbMailList = {}
UIMail.tbMailCheckList = {}
UIMail.nCurrentId = 0
UIMail.bHaveNewMail = 0
UIMail.nCurSelectedFilterIndex = 1

-- Ãû×Ö½Ð×öMinId£¬ÆäÊµ²»ÊÇ×îÐ¡ID£¬ÊÇ¸ø·þÎñ¶Ë£¬´ÓÕâ¸öID¿ªÊ¼È«È¡ÏÂÀ´µÄÒâË¼£¬ÊÇ±¾µØµÄ×î´óID
function UIMail:GetMinId()
    local nMinId = 0
    for nId, tbHeader in self.tbMailList do
        if nMinId <= 0 or nMinId < nId then
            nMinId = nId
        end
    end
    return nMinId
end

function UIMail:GetCount()
    local nCount = 0
    for nId, tbHeader in self.tbMailList do
        nCount = nCount + 1
    end
    return nCount
end

function UIMail:RebuildUI()
    self.nCurrentId = 0
    self.tbMailCheckList = {}
    CleanMailAll()
    for nId, tbHeader in self.tbMailList do
        local funChecker = self.tbFilterFun[self.nCurSelectedFilterIndex]
        if not funChecker then
            Talk(1, "", "M¸y läc lçi!")
            return
        end

        if funChecker(self, tbHeader) == 1 then
            AddMailHeader(nId, 
                tbHeader.szSender, 
                tbHeader.szTitle, 
                tbHeader.nState >= MAILDEF.tbState.READED and 1 or 0, 
                tbHeader.nAwardCount > 0 and tbHeader.nState < MAILDEF.tbState.DRAWED and 1 or 0,
                tbHeader.nExpiredTime, 
                tbHeader.nSendTime)
        end
    end

    UpdateMailCount()
end

function UIMail:ReCheckMailIconState()
    -- Èç¹û´æÔÚ»¹Ã»ÊÕµÄÓÊ¼þ£¬Ö±½ÓÏÔÊ¾
    if self.bHaveNewMail == 1 then
        SetMailIconVisible(1)
        return
    end

    local bHaveUnReaded = 0
    for nId, tbMail in self.tbMailList do
        if tbMail.nState <= MAILDEF.tbState.UNREAD then
            bHaveUnReaded = 1
            break
        end
    end
    SetMailIconVisible(bHaveUnReaded)
end

function UIMail:HeaderListArrival(nMinId, tbHeaderList, nComplete)
    MAILDEF:WriteLog(format("UIMail:HeaderListArrival nMinId = %d, tbHeaderList = %s, nComplete = %d", nMinId, dump2str(tbHeaderList), nComplete))

    local nRepeatCount = 0
    for nId, tbMailHeader in tbHeaderList do
        if not self.tbMailList[nId] then
            self.tbMailList[nId] = tbMailHeader
            
            local funChecker = self.tbFilterFun[self.nCurSelectedFilterIndex]
            if not funChecker then
                Talk(1, "", "M¸y läc lçi!")
                return
            end
            if funChecker(self, tbMailHeader) == 1 then
                AddMailHeader(nId,
                    tbMailHeader.szSender,
                    tbMailHeader.szTitle, 
                    tbMailHeader.nState >= MAILDEF.tbState.READED and 1 or 0, 
                    tbMailHeader.nAwardCount > 0 and tbMailHeader.nState < MAILDEF.tbState.DRAWED and 1 or 0,
                    tbMailHeader.nExpiredTime, 
                    tbMailHeader.nSendTime)
            end
        else
            nRepeatCount = nRepeatCount + 1
        end
    end
    
    UpdateMailCount()
    
    self.bHaveNewMail = 0
    self:ReCheckMailIconState()

    if nComplete == 0 and nRepeatCount ~= tblen(tbHeaderList) then -- ²»ÊÇÖØ¸´µÄÇëÇó£¬Ò»ÏÂ×Ó¸øÍæ¼Ò·¢¶à·âÓÊ¼þµÄÊ±ºò»áµ¼ÖÂ¿Í»§¶ËÖØ¸´ÇëÇó
        self:RequestMailHeaderList()
    end
end

function UIMail:NewMailEventArrival(nNewPrivateId)
    MAILDEF:WriteLog(format("UIMail:NewMailEventArrival nNewPrivateId:%d", nNewPrivateId))
    -- ÉÁË¸ÓÒ²àÐÂÓÊ¼þÍ¼±ê£¬½çÃæ¿ª×ÅµÄ»°£¬ÇëÇóÒ»´ÎÓÊ¼þ
    NewMailUIEventArrival()
    self.bHaveNewMail = 1
    self:ReCheckMailIconState()
    self:OpenMailWindow(1)   -- [MAIL 03/09 JX1 D4] co thu moi -> hien hop thu ngay
end

function UIMail:StateChangeEventArrival(nId, nToState, nSuccessed)
    MAILDEF:WriteLog(format("UIMail:StateChangeEventArrival nId:%d, nToState:%d, nSuccessed:%d", nId, nToState, nSuccessed))

    if nSuccessed then
        local tbMail = self.tbMailList[nId]
        if tbMail then
            tbMail.nState = nToState
            --Èç¹ûÓÊ¼þ½çÃæÒÑ¿ªÍ¨Öª½çÃæ¸Ä±äÓÊ¼þ×´Ì¬
            SetMailHeader(nId, 
                tbMail.szSender, 
                tbMail.szTitle, 
                tbMail.nState >= MAILDEF.tbState.READED and 1 or 0, 
                tbMail.nAwardCount > 0 and tbMail.nState < MAILDEF.tbState.DRAWED and 1 or 0,
                tbMail.nExpiredTime, 
				tbMail.nSendTime)

            if self.nCurrentId == nId then
                self:ShowMailContent(nId)
            end
        else
            -- TODO:´íÎó´¦Àí
        end
    else
        -- ÌáÊ¾¶ÁÈ¡/ÁìÈ¡ÓÊ¼þÊ§°Ü
        if nToState == MAILDEF.tbState.READE then
            Msg2Player("§äc th­ thÊt b¹i!")
        end
        if nToState == MAILDEF.tbState.DRAWED then
            Msg2Player("NhËn th­ thÊt b¹i!")
        end
    end
    
    self:ReCheckMailIconState()
end

function UIMail:DeleteEventArrival(nId, nReason)
    MAILDEF:WriteLog(format("UIMail:DeleteEventArrival nId:%d, nReason:%d", nId, nReason))
    self.tbMailList[nId] = nil
    self.tbMailCheckList[nId] = nil
    -- DeleteOneMail ½Ó¿ÚÓÐÎÊÌâ£¬ÏÈÓÃrebuild
    --self:RebuildUI()
    DeleteOneMail(nId)
    if nId == self.nCurrentId then
        CleanMailDetail()
    end
    self:ReCheckMailIconState()
end

function UIMail:WholeMailArrival(tbMail)
    MAILDEF:WriteLog(format("UIMail:WholeMailArrival tbMail = %s", dump2str(tbMail)))
    self.tbMailList[tbMail.nPrivateId] = tbMail
    -- Èç¹ûÓÊ¼þ½çÃæÒÑ¿ªÍ¨Öª½çÃæÓÊ¼þÄÚÈÝÒÑµ½´ï
    local tbCurMail = self.tbMailList[self.nCurrentId]
    if tbCurMail and tbCurMail.szDescribe then
        -- [MAIL 03/09 JX1] phan thuong do may chu mo ta san (tbAwardInfo), dong goi chuoi cho C++
        local szAwards = ""
        local nAwardCount = 0
        if tbCurMail.nAwardCount > 0 and tbCurMail.nState < MAILDEF.tbState.DRAWED then
            szAwards        = self:PackAwardInfo(tbCurMail.tbAwardInfo)
            nAwardCount     = tbCurMail.nAwardCount
        end
        UpdateMailDetail(self.nCurrentId, tbCurMail.szSender, tbCurMail.szTitle, tbCurMail.szDescribe, 1, nAwardCount > 0 and 1 or 0, FormatTime2String("%Y/%m/%d %H:%M:%S", tbCurMail.nSendTime), nAwardCount, szAwards)
    end
    self:ReCheckMailIconState()
end

function UIMail:ShowMailContent(nId)
    if self.tbMailList[nId] then
        if self.tbMailList[nId].szDescribe then
            return self:WholeMailArrival(self.tbMailList[nId])
        else
            return self:RequestWholeMail(nId)
        end
    else
        CleanMailDetail()
    end
end

-- [MAIL 03/09 JX1] tbAwardInfo do mailmanager.lua may chu gui, moi phan tu la bang:
--   {szKind="item", nGenre, nDetail, nParticular, nLevel, nSeries, nLuck, nCount}  -> vat pham (client dung lai tam)
--   {szKind="icon", szIcon, szName, szDesc, nCount}                              -> Ngan luong / xu / EXP
-- -> "item|g|d|p|l|s|k|n\n" hoac "icon|spr|ten|mo ta|n\n" (C++ KMailClient.cpp tach).
function UIMail:PackAwardInfo(tbAwardInfo)
    local szAll = ""
    if type(tbAwardInfo) ~= "table" then
        return szAll
    end
    for _, tbInfo in tbAwardInfo do
        if type(tbInfo) == "table" then
            if tbInfo.szKind == "item" then
                szAll = szAll.."item|"..(tbInfo.nGenre or 0).."|"..(tbInfo.nDetail or 0).."|"..(tbInfo.nParticular or 0)
                    .."|"..(tbInfo.nLevel or 0).."|"..(tbInfo.nSeries or 0).."|"..(tbInfo.nLuck or 0).."|"..(tbInfo.nCount or 1).."\n"
            else
                local szDesc = gsub(gsub(tbInfo.szDesc or "", "|", "/"), "\n", "<enter>")
                szAll = szAll.."icon|"..(tbInfo.szIcon or "").."|"..(tbInfo.szName or "").."|"..szDesc.."|"..(tbInfo.nCount or 1).."\n"
            end
        end
    end
    return szAll
end
function UIMail:GetAwardItemInfo(tbAward)
    local tbResult = {}
    for _, tbInfo in tbAward do
        if tbInfo.nJxb then
            tinsert(tbResult, {"\\spr\\Ui4\\email\\ÒøÁ½Í¼±ê.spr", format("%d Ng©n L­îng", tbInfo.nJxb), "", 0})
        elseif tbInfo.nBindingGold then
            tinsert(tbResult, {"\\spr\\item\\script\\jinding.spr", format("%d Kim §Ünh", tbInfo.nBindingGold), "", 0})
        elseif tbInfo.nRecruitScore then
            tinsert(tbResult, {"\\spr\\item\\questkey\\quyuanmifang.spr", format("%d §iÓm Chiªu Mé", tbInfo.nRecruitScore), "", 0})
        elseif tbInfo.nExp then
            tinsert(tbResult, {"\\spr\\item\\exp.spr", format("%d kinh nghiÖm", tbInfo.nExp), "", 0})
        elseif tbInfo.nExp_tl then
            tinsert(tbResult, {"\\spr\\item\\exp.spr", format("%d xÕp chång kinh nghiÖm", tbInfo.nExp_tl), "", 0})
        elseif tbInfo.nAnyExp then
            tinsert(tbResult, {"\\spr\\item\\exp.spr", format("%d EXP/Tu Vi", tbInfo.nAnyExp), "", 0})
        elseif tbInfo.nAnyExp_tl then
            tinsert(tbResult, {"\\spr\\item\\exp.spr", format("%d céng dån EXP/Tu Vi", tbInfo.nAnyExp_tl), "", 0})
        elseif tbInfo.n260Exp then
            tinsert(tbResult, {"\\spr\\item\\exp.spr", format("%d Tu Vi", tbInfo.n260Exp), "", 0})
        elseif tbInfo.n260Exp_tl then
            tinsert(tbResult, {"\\spr\\item\\exp.spr", format("%d céng dån Tu Vi", tbInfo.n260Exp_tl), "", 0})
        elseif tbInfo.nRepute then
            tinsert(tbResult, {"\\spr\\Ui4\\email\\ÒøÁ½Í¼±ê.spr", format("%d Danh Väng", tbInfo.nRepute), "", 0})
        elseif tbInfo.nHonour then
            tinsert(tbResult, {"\\spr\\Ui4\\email\\ÒøÁ½Í¼±ê.spr", format("%d §iÓm Vinh Dù", tbInfo.nHonour), "", 0})
        elseif tbInfo.tbProp then
            local nItemIndex = self:NewItemTmp(tbInfo)
            if nItemIndex > 0 then
                local szName = GetItemFullName(nItemIndex)
                -- local szName = GetItemFullName(nItemIndex)
                local nStart, nEnd = strfind(szName, "\n", 1)
                if nStart then
                    szName = strsub(szName, 1, nStart)
                end
                nStart, nEnd = strfind(szName, ">", 1)
                if nStart then
                    szName = strsub(szName, nStart + 1, strlen(szName))
                end
                local szDesc = GetItemSimpleDesc(nItemIndex) or "Ch­a biÕt"
                if tbInfo.nBindState == -2 then
                    szDesc = format("%s<enter><color=red>%s<color>", szDesc, "Khãa vÜnh viÔn")
                end
                if tbInfo.nExpiredTime and tbInfo.nExpiredTime > 0 then
                    if tbInfo.nExpiredTime < 20190613 then
                        szDesc = format("%s<enter><color=red>Cã thêi h¹n %d ngµy<color>", szDesc, floor(tbInfo.nExpiredTime/1440))
                    else
                        local nYear     = floor(tbInfo.nExpiredTime/10000)
                        local nMonth    = floor(mod(tbInfo.nExpiredTime, 10000)/100)
                        local nDay      = mod(tbInfo.nExpiredTime, 100)
                        szDesc = format("%s<enter><color=red>Cã hiÖu lùc ®Õn %d ngµy %d th¸ng %d n¨m<color>", szDesc,  nDay, nMonth,nYear )
                    end
                end

                local szIcon = GetItemIconImg(nItemIndex)
                if tbInfo.nQuality then
                    if tbInfo.nQuality <= 4 then
                        szIcon = "\\spr\\item\\dazhuanpan\\jinbox.spr"
                    else
                        szIcon = "\\spr\\item\\unknownitem.spr"
                    end
                else
                    -- ·¢ÁË¿í¸ß²»Îª1µÄµÀ¾ß
                    local nWidth = GetItemWidth(nItemIndex)
                    local nHeight = GetItemHeight(nItemIndex)
                    if nWidth > 1 or nHeight > 1 then
                        szIcon = "\\spr\\item\\unknownitem.spr"
                    end
                end
                tinsert(tbResult, {szIcon, szName, szDesc, tbInfo.nCount or 1})
                DelItemNonBelong(nItemIndex)
            else
                tinsert(tbResult, {"\\spr\\item\\unknownitem.spr", "??", "????", 1})
                MAILDEF:WriteLog(format("UIMail:GetAwardItemInfo ERROR nItemIndex <= 0!!!  nItemIndex:%d, tbInfo:%s", nItemIndex, dump2str(tbInfo)))
            end
        end
    end
    return tbResult
end

function UIMail:NewItemTmp(tbItem)
    local nRow = 0
    if tbItem.nQuality and (tbItem.nQuality == 1 or tbItem.nQuality == 4) then
        nRow = 1
    end
    
    return NewItemEx(
        tbItem.nVersion or 4,
        format("%u", tbItem.nRandSeed or 0),
        tbItem.nQuality or 0,
        tbItem.tbProp[1] or 0,
        (tbItem.tbProp[2] or 0) - nRow,
        tbItem.tbProp[3] or 0,
        tbItem.tbProp[4] or 0,
        tbItem.tbProp[5] or 0,
        tbItem.tbProp[6] or 0)
end

function UIMail:OpenMailWindow(nType)
    OpenMailWindow(nType)
    self.nCurSelectedFilterIndex = 1
    SetFilterText(self.nCurSelectedFilterIndex)
    self:RebuildUI()
end

----------------------CALL BY C++--------------------
-- ÇëÇó´ò¿ªÍøÒ³
function UIMail:RequestOpenURL(szUrl)
    local handle = OB_Create()
    ObjBuffer:PushByType(handle, OBJTYPE_STRING, szUrl)
    ScriptProtocol:SendData("emSCRIPT_PROTOCOL_MAIL_REQUEST_OPENURL", handle)
    OB_Release(handle)
end

-- ÉêÇë×Ô¶¯É¾³ý
function UIMail:RequestAutoDeleteMails()
    ScriptProtocol:SendData("emSCRIPT_PROTOCOL_MAIL_REQUEST_AUTODELETE", 0)
end

function UIMail:OnSelect(nId)
    self.nCurrentId = nId

    if self.tbMailList[nId] then
        SelectMail(nId)
    end

    self:ShowMailContent(nId)

    if self.tbMailList[nId] and self.tbMailList[nId].nState == MAILDEF.tbState.UNREAD then
        self:RequestChangeState(nId, MAILDEF.tbState.READED)
    end
end

--- Ïò·þÎñ¶Ë·¢ËÍÇëÇóÕªÒªÁÐ±í£¬nMinId=0±íÊ¾È«Á¿£¬nMinId>0£¬±íÊ¾ÇëÇóid´óÓÚnMinIdµÄÓÊ¼þheaderÁÐ±í
function UIMail:RequestMailHeaderList()
    local handle = OB_Create()
    ObjBuffer:PushByType(handle, OBJTYPE_NUMBER, self:GetMinId())
    ScriptProtocol:SendData("emSCRIPT_PROTOCOL_MAIL_REQUEST_HEADERLIST", handle)
    OB_Release(handle)
end

-- ÇëÇóÉ¾³ýÓÊ¼þ
function UIMail:RequestDeleteMail()
    local tbCurMail = self.tbMailList[self.nCurrentId]
    if not tbCurMail then
        Talk(1, "", "Ch­a më th­ nµo.")
        return
    end
    
    if tbCurMail.nAwardCount > 0 and tbCurMail.nState < MAILDEF.tbState.DRAWED then
        local szTitle ="Th­ cßn ®Ýnh kÌm ch­a nhËn, sau khi xãa kh«ng thÓ nhËn vËt phÈm ®Ýnh kÌm, x¸c nhËn xãa?"
        local tbOption =
        {
            "§ång ý/g_ConfirmDeleteMail",
            "§Ó ta suy nghÜ/no",
        }
        MailConfirm(szTitle, tbOption[1], tbOption[2])
    else
        self:ConfirmDeleteMail()
    end
end

function g_ConfirmDeleteMail()
    UIMail:ConfirmDeleteMail()
end

function UIMail:ConfirmDeleteMail()
    local handle = OB_Create()
    ObjBuffer:PushByType(handle, OBJTYPE_NUMBER, self.nCurrentId)
    ScriptProtocol:SendData("emSCRIPT_PROTOCOL_MAIL_REQUEST_DELETE", handle)
    OB_Release(handle)
end

-- ÇëÇóÉ¾³ýËùÓÐ¹´Ñ¡ÓÊ¼þ
function UIMail:RequestDeleteAllCheckedMail()
    if tblen(self.tbMailCheckList) <= 0 then
        Talk(1, "", "Ch­a chän th­ nµo.")
        return
    end

    local nUnDrawedCount = 0
    for nId, _ in self.tbMailCheckList do
        local tbMail = self.tbMailList[nId]
        if tbMail and tbMail.nAwardCount > 0 and tbMail.nState < MAILDEF.tbState.DRAWED then
            nUnDrawedCount = nUnDrawedCount + 1
        end
    end

    if nUnDrawedCount > 0 then
        local szTitle ="Th­ cßn ®Ýnh kÌm ch­a nhËn, sau khi xãa kh«ng thÓ nhËn vËt phÈm ®Ýnh kÌm, x¸c nhËn xãa?"
        local tbOption =
        {
            "§ång ý/g_ConfirmDeleteAllCheckedMail",
            "§Ó ta suy nghÜ/no",
        }
        MailConfirm(szTitle, tbOption[1], tbOption[2])
    else
        self:ConfirmDeleteAllCheckedMail()
    end
end

function g_ConfirmDeleteAllCheckedMail()
    UIMail:ConfirmDeleteAllCheckedMail()
end

function UIMail:ConfirmDeleteAllCheckedMail()
    for nId, _ in self.tbMailCheckList do
        local handle = OB_Create()
        ObjBuffer:PushByType(handle, OBJTYPE_NUMBER, nId)
        ScriptProtocol:SendData("emSCRIPT_PROTOCOL_MAIL_REQUEST_DELETE", handle)
        OB_Release(handle)
    end
end

-- ÇëÇóÒ»·âÍêÕûÓÊ¼þÄÚÈÝ
function UIMail:RequestWholeMail(nId)
    local handle = OB_Create()
    ObjBuffer:PushByType(handle, OBJTYPE_NUMBER, nId)
    ScriptProtocol:SendData("emSCRIPT_PROTOCOL_MAIL_REQUEST_WHOLEMAIL", handle)
    OB_Release(handle)
end

-- ÇëÇó½«ÓÊ¼þ×´Ì¬¸Ä³ÉÒÑ¶Á/ÒÑÁìÈ¡
function UIMail:RequestChangeState(nId, nToState)
    if nId <= 0 then
        nId = self.nCurrentId
    end

    if nId <= 0 or not self.tbMailList[nId] then
        Msg2Player("Th­ kh«ng tån t¹i!")
        return
    end

    if nToState == MAILDEF.tbState.READE and self.tbMailList[nId].nState >= MAILDEF.tbState.READE then
        Msg2Player("Th­ ë tr¹ng th¸i ®· ®äc!")
        return
    end

    if nToState == MAILDEF.tbState.DRAWED and self.tbMailList[nId].nState >= MAILDEF.tbState.DRAWED then
        Msg2Player("§· nhËn ®Ýnh kÌm!")
        return
    end

    if nToState == MAILDEF.tbState.DRAWED and self.tbMailList[nId].nAwardCount <= 0 then
        Msg2Player("Th­ kh«ng cã ®Ýnh kÌm cã thÓ nhËn!")
        return
    end

    local handle = OB_Create()
    ObjBuffer:PushByType(handle, OBJTYPE_NUMBER, nId)
    ObjBuffer:PushByType(handle, OBJTYPE_NUMBER, nToState)
    ScriptProtocol:SendData("emSCRIPT_PROTOCOL_MAIL_REQUEST_STATECHANGE", handle)
    OB_Release(handle)
end

function UIMail:OnMailCheck(nId, nToState, nByManual)
    if nToState == 1 then
        self.tbMailCheckList[nId] = 1
        if nByManual == 1 then
            self:OnSelect(nId)
        end
    else
        self.tbMailCheckList[nId] = nil
    end
end

function UIMail:OnUpdate()
    local nAcceptShow         = 1
    local nAcceptEnable       = 0
    local nDeleteOneShow      = 1
    local nDeleteOneEnable    = 0
    local nDeleteAllShow      = 0
    local nDeleteAllEnable    = 0
    
    local tbCurMail = self.tbMailList[self.nCurrentId]
    if tbCurMail then
        nAcceptEnable       = (tbCurMail.nAwardCount > 0 and tbCurMail.nState < MAILDEF.tbState.DRAWED) and 1 or 0
        nDeleteOneShow      = 1
        nDeleteOneEnable    = 1
        nDeleteAllShow      = 0
        nDeleteAllEnable    = 0
    else
        nAcceptEnable       = 0
        nDeleteOneShow      = 1
        nDeleteOneEnable    = 0
        nDeleteAllShow      = 0
        nDeleteAllEnable    = 0
    end

    if tblen(self.tbMailCheckList) > 0 then
        nDeleteOneShow      = 0
        nDeleteOneEnable    = 0
        nDeleteAllShow      = 1
        nDeleteAllEnable    = 1
    end

    SetMailBntStatus(nAcceptShow, nAcceptEnable, nDeleteOneShow, nDeleteOneEnable, nDeleteAllShow, nDeleteAllEnable)
end

function UIMail:OnMailFilterSelected(nSelectedIndex)
    -- C++ ´Ó0¿ªÊ¼
    self.nCurSelectedFilterIndex = nSelectedIndex + 1
    self:RebuildUI()
end

function UIMail:CheckIdOnOpen(nId)
    if not self.tbMailList[nId] then
        DeleteOneMail(nId)
        if nId == self.nCurrentId then
            CleanMailDetail()
        end
    end
end

function UIMail:OnMailIconClick()
    -- [MAIL 03/09 JX1 D4] bam bieu tuong thu (duoi Bau Cua) -> mo hop thu ngay, khong can den Tin Su
    self.bHaveNewMail = 0
    self:OpenMailWindow(1)
    self:RequestMailHeaderList()
end

function g_ConfirmFindMessager()
    local tbMapPath = 
    {
        [176]   = {176,1424,3272},
        [80]    = {80,1723,2973},
        [37]    = {37,1762,3088},
        [78]    = {78,1531,3269},
        [11]    = {11,3127,5076},
        [162]   = {162,1564,3172},
        [1]     = {1,1633,3129},
    }

    local nCurMapId = GetWorldPos()
    local tbAutoFindPath = tbMapPath[176]
    if tbMapPath[nCurMapId] then
        tbAutoFindPath = tbMapPath[nCurMapId]
    end
    -- [MAIL 03/09 JX1] client JX1 chua co AutoCrossMapFindPath: chi nhac, khong tu chay
end

----------------------CALL BY C++--------------------


-----------------FilterChecker--------------
function UIMail:FilterChecker_All(tbMail)
    return 1
end

function UIMail:FilterChecker_UNREAD(tbMail)
    if tbMail.nState <= MAILDEF.tbState.UNREAD then
        return 1
    end
    return 0
end

function UIMail:FilterChecker_STH2Award(tbMail)
    if tbMail.nAwardCount > 0 and tbMail.nState < MAILDEF.tbState.DRAWED then
        return 1
    end
    return 0
end

function UIMail:FilterChecker_SENDERSYS(tbMail)
    if (tbMail.szSender == "Th­ hÖ thèng") or (tbMail.szSender == "Nhµ ph¸t hµnh") then
        return 1
    end
    return 0
end

function UIMail:FilterChecker_SENDERPMH(tbMail)
    if tbMail.szSender == "Ch­ëng quÇy khu ®Êu gi¸" then
        return 1
    end
    return 0
end

UIMail.tbFilterType =           -- ¹ýÂËÆ÷ÀàÐÍ£¬¸únCurSelectedFilterIndex¶ÔÓ¦£¬Ð´ÉÏ=1,=2ÈÃÎ¬»¤ÕßºÃ¿´µã
{
    ALL_MAIL            = 1,    -- È«²¿ÐÅ¼þ
    UNREAD_MAIL         = 2,    -- Î´¶ÁÐÅ¼þ
    HAVE_STH_TOAWARD    = 3,    -- Î´ÁìÐÅ¼þ
    SENDER_SYSTEM       = 4,    -- ·¢¼þÈË£»ÏµÍ³ÐÅ¼þ
    SENDER_PAIMAIHANG   = 5,    -- ·¢¼þÈË£ºÅÄÂôÐÐÕÆ¹ñ
}

UIMail.tbFilterFun =    -- ¹ýÂËÆ÷¼ì²éº¯Êý
{
    [UIMail.tbFilterType.ALL_MAIL]          = UIMail.FilterChecker_All,       -- È«²¿ÐÅ¼þ
    [UIMail.tbFilterType.UNREAD_MAIL]       = UIMail.FilterChecker_UNREAD,    -- Î´¶ÁÐÅ¼þ
    [UIMail.tbFilterType.HAVE_STH_TOAWARD]  = UIMail.FilterChecker_STH2Award, -- Î´ÁìÐÅ¼þ
    [UIMail.tbFilterType.SENDER_SYSTEM]     = UIMail.FilterChecker_SENDERSYS, -- ·¢¼þÈË£»ÏµÍ³ÐÅ¼þ
    [UIMail.tbFilterType.SENDER_PAIMAIHANG] = UIMail.FilterChecker_SENDERPMH, -- ·¢¼þÈË£ºÅÄÂôÐÐÐÅ¼þ
}

function no()
    return
end

-- Debug("ShowMouseWnd")