-- uiauction_house.lua - [DAUGIA 04/09] cua so DAU GIA phia client. Viet lai tu \script\ui\uiauction_house.lua cua
-- client JX1 cu (2.0) cho JX1: van goi 21 ham C++ cung ten (KAuctionClient.cpp), nhung vat pham truyen bang 6 so
-- (genre,detail,particular,level,series,luck) + so luong + ten + ten tien; hop xac nhan bo (hanh dong truc tiep).
Include("\\script\\protocol.lua")
Include("\\script\\lib\\objbuffer_head.lua")
Include("\\script\\auction_house\\auction_def.lua")

UIAuctionHouse = UIAuctionHouse or {}
UIAuctionHouse.tbActivityList = {}
UIAuctionHouse.tbActivityList[AUCTION_DEF.tbAuctionTypeEnum.eType_TONG] = {}
UIAuctionHouse.tbActivityList[AUCTION_DEF.tbAuctionTypeEnum.eType_WORLD] = {}
UIAuctionHouse.tbActivityList[AUCTION_DEF.tbAuctionTypeEnum.eType_PERSONAL] = {}
UIAuctionHouse.nCurTypeIndex = AUCTION_DEF.tbAuctionTypeEnum.eType_PERSONAL
UIAuctionHouse.szCurActivityName = ""
UIAuctionHouse.nCurPageIndex = 1

function UIAuctionHouse:Reset()
    self.tbActivityList = {}
    self.tbActivityList[AUCTION_DEF.tbAuctionTypeEnum.eType_TONG] = {}
    self.tbActivityList[AUCTION_DEF.tbAuctionTypeEnum.eType_WORLD] = {}
    self.tbActivityList[AUCTION_DEF.tbAuctionTypeEnum.eType_PERSONAL] = {}
    self.szCurActivityName = ""
    self.nCurPageIndex = 1
end

function UIAuctionHouse:TalkToPlayer(szMsg)
    Msg2Player(szMsg)
end

function UIAuctionHouse:SwitchToAuctionWindow(nType)
    self.nCurTypeIndex = nType
    self.szCurActivityName = ""
    self.nCurPageIndex = 1
    AuctionClearAll()
    SwitchAuctionWindow(nType)
    for szName, tbInfo in self.tbActivityList[nType] do
        AuctionAddActivity(nType, szName, tbInfo.nStartTime or 0)
    end
    self:RequestActivityList(nType)
end

function UIAuctionHouse:GetActivity(szAct)
    return self.tbActivityList[self.nCurTypeIndex][szAct]
end

function UIAuctionHouse:GetActivityMaxPage(szAct)
    local a = self:GetActivity(szAct)
    if not a or not a.nTotalCount or a.nTotalCount <= 0 then
        return 0
    end
    return floor((a.nTotalCount - 1) / AUCTION_DEF.nMaxItemPerPage) + 1
end

function UIAuctionHouse:ResetPageInfo(nType, szAct)
    if szAct ~= "" then
        local nMax = self:GetActivityMaxPage(szAct)
        if nMax > 0 and self.nCurPageIndex > nMax then
            self.nCurPageIndex = nMax
        end
        if nMax > 0 then
            AuctionSetCurrentPageTxt(nType, format("%d/%d", self.nCurPageIndex, nMax))
            return
        end
    end
    AuctionSetCurrentPageTxt(nType, "-/-")
end

function UIAuctionHouse:FirstActivity()
    local szMin, nMin = "", 0
    for szName, tbInfo in self.tbActivityList[self.nCurTypeIndex] do
        if szMin == "" or (tbInfo.nStartTime or 0) < nMin then
            szMin = szName
            nMin = tbInfo.nStartTime or 0
        end
    end
    return szMin
end

function UIAuctionHouse:GetItem(szAct, nId)
    local a = self:GetActivity(szAct)
    if not a or not a.tbItemList then
        return nil
    end
    for nPage, tbPage in a.tbItemList do
        for i, tbItem in tbPage do
            if tbItem.nId == nId then
                return tbItem, nPage
            end
        end
    end
    return nil
end

-- ---------------------------------------------------------------- gui len may chu
function UIAuctionHouse:RequestActivityList(nType)
    local h = OB_Create()
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nType)
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, 0)
    ScriptProtocol:SendData("emSCRIPT_PROTOCOL_AUCTION_REQUEST_ACTIVITYLIST", h)
    OB_Release(h)
end

function UIAuctionHouse:RequestActivityContent(szAct, nPage)
    local h = OB_Create()
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, self.nCurTypeIndex)
    ObjBuffer:PushByType(h, OBJTYPE_STRING, szAct)
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nPage)
    ObjBuffer:PushByType(h, OBJTYPE_TABLE, {})
    ScriptProtocol:SendData("emSCRIPT_PROTOCOL_AUCTION_REQUEST_ACTIVITYCONTENT", h)
    OB_Release(h)
end

function UIAuctionHouse:RequestMemberList()
    if self.szCurActivityName == "" then
        return
    end
    local h = OB_Create()
    ObjBuffer:PushByType(h, OBJTYPE_STRING, self.szCurActivityName)
    ScriptProtocol:SendData("emSCRIPT_PROTOCOL_AUCTION_REQUEST_MEMBERLIST", h)
    OB_Release(h)
end

function UIAuctionHouse:SendItemReq(szEnum, nId, nPrice)
    if self.szCurActivityName == "" then
        self:TalkToPlayer("Ch­a chän phiªn.")
        return
    end
    local h = OB_Create()
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, self.nCurTypeIndex)
    ObjBuffer:PushByType(h, OBJTYPE_STRING, self.szCurActivityName)
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nId)
    if nPrice then
        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nPrice)
    end
    ScriptProtocol:SendData(szEnum, h)
    OB_Release(h)
end

function UIAuctionHouse:RequestOfferEnglishPrice(nId, nPrice)
    self:SendItemReq("emSCRIPT_PROTOCOL_AUCTION_REQUEST_OFFERENGLISHPRICE", nId, nPrice)
end

function UIAuctionHouse:RequestOfferDutchPrice(nId, nPrice)
    self:SendItemReq("emSCRIPT_PROTOCOL_AUCTION_REQUEST_OFFERDUTCHPRICE", nId, nPrice)
end

function UIAuctionHouse:RequestGetBackItem(nId)
    self:SendItemReq("emSCRIPT_PROTOCOL_AUCTION_REQUEST_GETBACKITEM", nId, nil)
end

function UIAuctionHouse:RequestRefund(nId)
    self:SendItemReq("emSCRIPT_PROTOCOL_AUCTION_REQUEST_REFUND", nId, nil)
end

-- ---------------------------------------------------------------- tu C++ (nguoi bam)
function UIAuctionHouse:OnAuctionIconClick(nToOpen)
    if nToOpen == 1 then
        OpenAuctionWindow()
        self:SwitchToAuctionWindow(self.nCurTypeIndex)
    else
        CloseAuctionWindow()
    end
end

function UIAuctionHouse:OnOpenFromServer(nType)
    OpenAuctionWindow()
    self:SwitchToAuctionWindow(nType or AUCTION_DEF.tbAuctionTypeEnum.eType_PERSONAL)
end

function UIAuctionHouse:OnPageBtnClick(nType)
    if nType == self.nCurTypeIndex then
        return
    end
    self:SwitchToAuctionWindow(nType)
end

-- [A4] nut goc phai: Ca nhan = ky gui, The gioi = GM mo phien, Bang hoi = bang chu dat mon / thanh vien xem danh sach
function UIAuctionHouse:OnPutOnClick(nType)
    local h = OB_Create()
    AuctionPutOnMode(1)
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nType or self.nCurTypeIndex)
    ScriptProtocol:SendData("emSCRIPT_PROTOCOL_AUCTION_REQUEST_PUTON", h)
    OB_Release(h)
end

-- [A6] o gia + nut doi tien nam ngay trong hop dua vat pham; bam Dong y thi bao len truoc
-- [A20c] ba so: gia mua ngay, loai tien, gia co ban
function UIAuctionHouse:OnSetPrice(nPrice, nCur, nBase)
    local h = OB_Create()
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nPrice or 0)
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nCur or 1)
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nBase or 0)
    ScriptProtocol:SendData("emSCRIPT_PROTOCOL_AUCTION_REQUEST_SETPRICE", h)
    OB_Release(h)
end

function UIAuctionHouse:OnWindowClosed()
end

function UIAuctionHouse:OnSelect(szAct)
    if self.szCurActivityName == szAct then
        return
    end
    self.szCurActivityName = szAct
    self.nCurPageIndex = 1
    AuctionOnActivitySelect(self.nCurTypeIndex, szAct)
    AuctionClearItemList()
    self:ResetPageInfo(self.nCurTypeIndex, szAct)
    if szAct ~= "" then
        self:RequestActivityContent(szAct, 1)
    end
end

function UIAuctionHouse:OnClickPrevPageBtn()
    if self.nCurPageIndex <= 1 then
        Msg2Player("§· lµ trang ®Çu.")
        return
    end
    self.nCurPageIndex = self.nCurPageIndex - 1
    self:RequestActivityContent(self.szCurActivityName, self.nCurPageIndex)
end

function UIAuctionHouse:OnClickNextPageBtn()
    local nMax = self:GetActivityMaxPage(self.szCurActivityName)
    if self.nCurPageIndex + 1 > nMax then
        Msg2Player("§· lµ trang cuèi.")
        return
    end
    self.nCurPageIndex = self.nCurPageIndex + 1
    self:RequestActivityContent(self.szCurActivityName, self.nCurPageIndex)
end

-- ---------------------------------------------------------------- tu may chu (protocol_def_c.lua)
function UIAuctionHouse:ShowItem(nType, tbItem, bAdd)
    -- [A11] may chu da gui san bMine (client khong co GetName)
    local bMine = tbItem.bMine or 0
    local szCur = AUCTION_DEF.tbCurrencyName[tbItem.nCurrencyType] or ""
    -- [A9] 6 so cua vat pham nam thang trong dong (khong con bang long)
    if tbItem.nType == AUCTION_DEF.tbItemTypeEnum.eType_ENGLISH then
        if bAdd == 1 then
            AuctionAddEnglishItem(nType, tbItem.nId, tbItem.nStartTime, tbItem.nGuaranteedPrice, tbItem.nRangePerOffer, tbItem.nCurrencyType,
                tbItem.nMaxPrice, tbItem.nSelfPrice, tbItem.nRemainingTime, bMine,
                tbItem.nG or 0, tbItem.nD or 0, tbItem.nP or 0, tbItem.nL or 0, tbItem.nS or 0, tbItem.nK or 0, tbItem.nCount or 1, tbItem.szName or "", szCur,
                tbItem.szInfo or "", tbItem.nBuyNow or 0)
        else
            AuctionSetEnglishItem(nType, tbItem.nId, tbItem.nStartTime, tbItem.nGuaranteedPrice, tbItem.nRangePerOffer, tbItem.nCurrencyType,
                tbItem.nMaxPrice, tbItem.nSelfPrice, tbItem.nRemainingTime, bMine, szCur)
        end
    else
        if bAdd == 1 then
            AuctionAddDutchItem(nType, tbItem.nId, tbItem.nStartTime, tbItem.nCurPrice, tbItem.nGuaranteedPrice, tbItem.nCurrencyType,
                tbItem.nRemainingTime, tbItem.nNextPrice, tbItem.nTotalRemainingTime, bMine,
                tbItem.nG or 0, tbItem.nD or 0, tbItem.nP or 0, tbItem.nL or 0, tbItem.nS or 0, tbItem.nK or 0, tbItem.nCount or 1, tbItem.szName or "", szCur,
                tbItem.szInfo or "")
        else
            AuctionSetDutchItem(nType, tbItem.nId, tbItem.nStartTime, tbItem.nCurPrice, tbItem.nGuaranteedPrice, tbItem.nCurrencyType,
                tbItem.nRemainingTime, tbItem.nNextPrice, tbItem.nTotalRemainingTime, bMine, szCur)
        end
    end
end

function UIAuctionHouse:OnActivityListResponse(nType, tbList)
    if not tbList then
        return
    end
    for szName, tbInfo in tbList do
        if szName == "__money" then
            AuctionSetMoney(tbInfo.nTotalCount or 0, tbInfo.nStartTime or 0)
        else
            local a = self.tbActivityList[nType][szName]
            if not a then
                self.tbActivityList[nType][szName] = {nTotalCount = tbInfo.nTotalCount or 0, nStartTime = tbInfo.nStartTime or 0}
                if nType == self.nCurTypeIndex then
                    AuctionAddActivity(nType, szName, tbInfo.nStartTime or 0)
                end
            else
                a.nTotalCount = tbInfo.nTotalCount or 0
                a.nStartTime = tbInfo.nStartTime or 0
            end
        end
    end
    if nType == self.nCurTypeIndex and self.szCurActivityName == "" then
        local szFirst = self:FirstActivity()
        if szFirst ~= "" then
            self:OnSelect(szFirst)
        end
    end
end

function UIAuctionHouse:OnActivityContentResponse(nType, szAct, tbPage, nTotal, nPage, bNeedUpdate)
    local a = self.tbActivityList[nType][szAct]
    if not a then
        a = {nTotalCount = 0, nStartTime = 0}
        self.tbActivityList[nType][szAct] = a
    end
    a.nTotalCount = nTotal or 0
    a.tbItemList = a.tbItemList or {}
    local nNow = GetLocalTime()
    tbPage = tbPage or {}
    for i, tbItem in tbPage do
        tbItem.nRemainingTime = (tbItem.nRemainingTime or 0) + nNow
        tbItem.nTotalRemainingTime = (tbItem.nTotalRemainingTime or 0) + nNow
    end
    a.tbItemList[nPage] = tbPage
    if nType == self.nCurTypeIndex and self.szCurActivityName == szAct and self.nCurPageIndex == nPage then
        AuctionClearItemList()
        self:ResetPageInfo(nType, szAct)
        for i, tbItem in tbPage do
            self:ShowItem(nType, tbItem, 1)
        end
    end
end

function UIAuctionHouse:OnItemContentResponse(tbItem)
end

function UIAuctionHouse:OnMemberListResponse(nType, tbInfo)
    if not tbInfo then
        return
    end
    AuctionClearMemberList()
    -- [A9] danh sach thanh vien den duoi dang chuoi "ten,cap,chuc,online;..."
    local sz = tbInfo.szMemberList or ""
    local n = 0
    while sz ~= "" do
        local a, b, one = strfind(sz, "^([^;]*);?")
        if not a then
            break
        end
        sz = strsub(sz, b + 1)
        local _, _, szTen, szCap, szChuc, szOn = strfind(one, "^(.-),(%d+),(%d+),(%d+)$")
        if szTen then
            n = n + 1
            AuctionAddActivityMember(szTen, tonumber(szCap) or 0, tonumber(szChuc) or 0, tonumber(szOn) or 0)
        end
    end
    AuctionSetSalaryAndCount(tbInfo.nCount or n, tbInfo.nSalary or 0)
end

function UIAuctionHouse:OnOfferPriceResponse(nType, szAct, nId, nNewPrice)
    local tbItem = self:GetItem(szAct, nId)
    if tbItem then
        tbItem.nSelfPrice = nNewPrice
        if tbItem.nType == AUCTION_DEF.tbItemTypeEnum.eType_DUTCH then
            PopBlackTips("Chóc mõng ®¹i hiÖp ®· mua ®­îc "..(tbItem.szName or "")..", vËt phÈm göi vµo hép th­.")
        else
            tbItem.nMaxPrice = nNewPrice
            if nType == self.nCurTypeIndex and self.szCurActivityName == szAct then
                self:ShowItem(nType, tbItem, 0)
            end
        end
    end
end

function UIAuctionHouse:OnRefundResponse(nType, szAct, nId, nOk)
end

function UIAuctionHouse:OnNewActivityEvent(nType, szAct, nStart)
    if not self.tbActivityList[nType][szAct] then
        self.tbActivityList[nType][szAct] = {nTotalCount = 0, nStartTime = nStart or 0}
        if self.nCurTypeIndex == nType then
            AuctionAddActivity(nType, szAct, nStart or 0)
        end
    end
    NewAuctionEventArrival()
end

function UIAuctionHouse:OnEndActivityEvent(nType, szAct)
    if self.tbActivityList[nType][szAct] then
        AuctionDelActivity(nType, szAct)
        self.tbActivityList[nType][szAct] = nil
        if self.nCurTypeIndex == nType and self.szCurActivityName == szAct then
            AuctionClearItemList()
            self.szCurActivityName = ""
            self:ResetPageInfo(nType, "")
        end
    end
end

-- [A19] so mon THUC SU dang nam tren trang dang xem (ban goc co ham nay, ban ta thieu)
function UIAuctionHouse:GetCurPageSize()
    local a = self.tbActivityList[self.nCurTypeIndex]
    if not a then
        return 0
    end
    a = a[self.szCurActivityName]
    if not a then
        return 0
    end
    local nTong = a.nTotalCount or 0
    local nBo = (self.nCurPageIndex - 1) * AUCTION_DEF.nMaxItemPerPage
    local nCon = nTong - nBo
    if nCon < 0 then
        nCon = 0
    end
    if nCon > AUCTION_DEF.nMaxItemPerPage then
        nCon = AUCTION_DEF.nMaxItemPerPage
    end
    return nCon
end

function UIAuctionHouse:OnNewItemEvent(nType, szAct, nNewId, nTotal)
    local a = self.tbActivityList[nType][szAct]
    if not a then
        a = {nTotalCount = 0, nStartTime = 0}
        self.tbActivityList[nType][szAct] = a
        if self.nCurTypeIndex == nType then
            AuctionAddActivity(nType, szAct, 0)
        end
    end
    a.nTotalCount = nTotal or 0
    -- [A19] chi hoi lai khi TRANG DANG XEM CHUA DAY (dung dieu kien thu tu cua ban goc,
    -- uiauction_house.lua:542). Trang da day thi mon moi nam o trang sau, hoi lai chi to doi goi.
    if self.nCurTypeIndex == nType and self.szCurActivityName == szAct and AuctionUiIsOpen() == 1
        and self:GetCurPageSize() < AUCTION_DEF.nMaxItemPerPage then
        self:RequestActivityContent(szAct, self.nCurPageIndex)
    end
    NewAuctionEventArrival()
end

function UIAuctionHouse:OnEndItemEvent(nType, szAct, nEndId, nTotal)
    local a = self.tbActivityList[nType][szAct]
    if a then
        a.nTotalCount = nTotal or 0
    end
    if self.nCurTypeIndex == nType and self.szCurActivityName == szAct then
        -- [A19] CHI danh dau mon tai cho, KHONG nap lai ca trang.
        -- Ban goc co y chu thich bo dong nap lai (uiauction_house.lua:568): don danh sach duoi
        -- tay nguoi dang bam la mua nham mon, va loi goi nap lai con xoa luon ket qua cua
        -- AuctionEndItem ngay dong tren no. Voi 2 mon mot trang thi moi mon ai do ban xong deu
        -- bat MOI nguoi dang mo cua so hoi lai ca trang - dung kieu doi goi (bai hoc F4 04/09).
        AuctionEndItem(nType, nEndId)
        self:ResetPageInfo(nType, szAct)
    end
end

function UIAuctionHouse:OnPriceChangeEvent(nType, szAct, nId, nNewPrice, nNextPrice, nRemaining, nCurFloat)
    local tbItem = self:GetItem(szAct, nId)
    if tbItem then
        tbItem.nMaxPrice = nNewPrice
        tbItem.nCurPrice = nNewPrice
        tbItem.nNextPrice = nNextPrice
        tbItem.nRemainingTime = (nRemaining or 0) + GetLocalTime()
        if nType == self.nCurTypeIndex and self.szCurActivityName == szAct then
            self:ShowItem(nType, tbItem, 0)
        end
    end
end

SetAuctionIconVisible(0)
