-- auction_manager.lua - [DAUGIA 04/09] bo may DAU GIA phia may chu. Kho: bang MySQL auction_item (KAuctionServer.cpp,
-- 14 ham AUC_*). Giao/tra VAT PHAM va TIEN deu qua THU (mailmanager.lua) nen nguoi offline khong mat gi; vat pham giu
-- NGUYEN thuoc tinh (AUC_ItemToRec / AUC_GiveRec). Client: \script\ui\uiauction_house.lua. Giao thuc: 20 ten AUCTION_*.
-- Loai phien: 1 bang hoi (chi thanh vien bang, tien ban vao QUY BANG), 2 the gioi (GM mo, tien ban vao he thong),
-- 3 ca nhan = ky gui (moi nguoi dat ban, 24 gio, gia co dinh, tien ve nguoi ban tru thue).
Include("\\script\\protocol.lua")
Include("\\script\\lib\\objbuffer_head.lua")
Include("\\script\\auction_house\\auction_def.lua")
Include("\\script\\mail\\mailmanager.lua")

AUC_Viewers = AUC_Viewers or {}      -- [PlayerIndex] = {nType, szAct}  nguoi dang mo tab nao
AUC_LastCount = AUC_LastCount or {}  -- [nType..'|'..szAct] = so mon lan quet truoc (bao ket thuc phien)

function AUC_Log(sz)
    GhiLog("DAUGIA", sz)
end

function AUC_SendTo(nPlayerIdx, szEnum, h)
    if nPlayerIdx == nil or nPlayerIdx <= 0 then
        return 0
    end
    return SendScriptDataToPlayer(nPlayerIdx, ScriptProtocol[szEnum], h)
end

-- ---------------------------------------------------------------- tien
function AUC_GetMoney(nCur)
    if nCur == AUCTION_DEF.tbCurrency.XU then
        return GetTask(AUCTION_DEF.XU_TASK)
    end
    return GetCash()
end

function AUC_PayMoney(nCur, n)
    n = floor(n)
    if n <= 0 then
        return 1
    end
    if nCur == AUCTION_DEF.tbCurrency.XU then
        local x = GetTask(AUCTION_DEF.XU_TASK)
        if x < n then
            return 0
        end
        SetTask(AUCTION_DEF.XU_TASK, x - n)
        return 1
    end
    if GetCash() < n then
        return 0
    end
    return Pay(n)
end

function AUC_MoneyAward(nCur, n)
    n = floor(n)
    if nCur == AUCTION_DEF.tbCurrency.XU then
        return "xu:"..n
    end
    return "money:"..n
end

function AUC_CurName(nCur)
    return AUCTION_DEF.tbCurrencyName[nCur] or "?"
end

-- ---------------------------------------------------------------- thu
function AUC_MailItem(szRole, szTitle, szContent, nId)
    return MailManager_SendMail(szRole, AUCTION_DEF.szSender, szTitle, szContent, "aucitem:"..nId, AUCTION_DEF.nMailDays, "daugia")
end

function AUC_MailMoney(szRole, szTitle, szContent, nCur, n)
    if floor(n) <= 0 then
        return 0
    end
    return MailManager_SendMail(szRole, AUCTION_DEF.szSender, szTitle, szContent, AUC_MoneyAward(nCur, n), AUCTION_DEF.nMailDays, "daugia")
end

-- ---------------------------------------------------------------- doc kho
-- tra ve danh sach dong dang ban cua loai nType (bang hoi: chi bang nTong)
function AUC_Rows(nType, nTong)
    local tb = AUC_List(nType, 200, 0)
    if nType ~= AUCTION_DEF.tbAuctionTypeEnum.eType_TONG then
        return tb
    end
    local out = {}
    for i = 1, getn(tb) do
        if tb[i].tong == nTong then
            tinsert(out, tb[i])
        end
    end
    return out
end

function AUC_ActName(nType, r)
    if nType == AUCTION_DEF.tbAuctionTypeEnum.eType_PERSONAL then
        return AUCTION_DEF.szPersonalActivity
    end
    return r.activity
end

-- {[szAct] = {nTotalCount, nStartTime}}
function AUC_Activities(nType, nTong)
    local rows = AUC_Rows(nType, nTong)
    local tb = {}
    for i = 1, getn(rows) do
        local szAct = AUC_ActName(nType, rows[i])
        if szAct ~= "" then
            if not tb[szAct] then
                tb[szAct] = {nTotalCount = 0, nStartTime = rows[i].start}
            end
            tb[szAct].nTotalCount = tb[szAct].nTotalCount + 1
            if rows[i].start < tb[szAct].nStartTime then
                tb[szAct].nStartTime = rows[i].start
            end
        end
    end
    if nType == AUCTION_DEF.tbAuctionTypeEnum.eType_PERSONAL and not tb[AUCTION_DEF.szPersonalActivity] then
        tb[AUCTION_DEF.szPersonalActivity] = {nTotalCount = 0, nStartTime = 0}
    end
    return tb
end

function AUC_SplitDesc(szDesc)
    local t = {}
    local s = szDesc or ""
    while 1 do
        local a, b, num = strfind(s, "^(%-?%d+),?")
        if not a then
            break
        end
        tinsert(t, tonumber(num) or 0)
        s = strsub(s, b + 1)
        if s == "" then
            break
        end
    end
    while getn(t) < 6 do
        tinsert(t, 0)
    end
    return t
end

-- dong CSDL -> bang gui client (ten truong theo uiauction_house.lua 2.0)
function AUC_RowToClient(r, szMe, nNow)
    local tb = {}
    tb.nId = r.id
    tb.nType = r.kind
    tb.nStartTime = r.start
    tb.nCurrencyType = r.currency
    -- [A11] MAY CHU tinh san "mon cua chinh minh": client KHONG co ham GetName
    -- (GetName chi dang ky trong khoi #ifdef _SERVER) - goi ben client nem loi va dut vong ve.
    -- [A12] bo szCurrencyName + szBelongRole cho nhe goi (bo dem ObjBuffer chi 4096 byte).
    tb.bMine = 0
    if szMe ~= "" and r.seller == szMe then
        tb.bMine = 1
    end
    -- [A9] KHONG long bang ba tang (tbPage > dong > tbItem): ObjBuffer khong dua qua duoc,
    -- ben nhan ra bang RONG. Trai 6 so cua vat pham thanh 6 truong so.
    local pr = AUC_SplitDesc(r.desc)
    tb.nG = pr[1]
    tb.nD = pr[2]
    tb.nP = pr[3]
    tb.nL = pr[4]
    tb.nS = pr[5]
    tb.nK = pr[6]
    tb.szName = r.name
    tb.nCount = 1
    tb.nGuaranteedPrice = r.guar
    tb.nCurPrice = r.cur
    tb.nMaxPrice = r.cur
    tb.nRangePerOffer = floor(r.guar / 10)
    if tb.nRangePerOffer < 1 then
        tb.nRangePerOffer = 1
    end
    tb.nSelfPrice = 0
    if r.kind == AUCTION_DEF.tbItemTypeEnum.eType_ENGLISH and r.buyer == szMe then
        tb.nSelfPrice = r.cur
    end
    tb.nRemainingTime = r.endtime - nNow
    tb.nTotalRemainingTime = r.endtime - nNow
    tb.nNextPrice = r.cur
    tb.nFloatInterval = AUCTION_DEF.nDutchFloatInterval
    tb.nTotalFloatTimes = AUCTION_DEF.nDutchFloatTimes
    tb.nCurFloatTimes = AUCTION_DEF.nDutchFloatTimes - r.dropleft
    if r.kind == AUCTION_DEF.tbItemTypeEnum.eType_DUTCH and r.dropleft > 0 and r.nextdrop > 0 then
        tb.nRemainingTime = r.nextdrop - nNow
        local nNext = floor(r.cur * (1 - AUCTION_DEF.nDutchFloatRange))
        if nNext < r.guar then
            nNext = r.guar
        end
        tb.nNextPrice = nNext
    end
    return tb
end

-- ---------------------------------------------------------------- gui xuong client
function AUC_SendActivityList(nPlayerIdx, nType, nTong)
    local h = OB_Create()
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nType)
    local tb = AUC_Activities(nType, nTong)
    tb["__money"] = {nTotalCount = GetCash(), nStartTime = GetTask(AUCTION_DEF.XU_TASK)}
    ObjBuffer:PushByType(h, OBJTYPE_TABLE, tb)
    AUC_SendTo(nPlayerIdx, "emSCRIPT_PROTOCOL_AUCTION_ACTIVITYLIST", h)
    OB_Release(h)
end

function AUC_SendActivityContent(nPlayerIdx, nType, nTong, szAct, nPage)
    local rows = AUC_Rows(nType, nTong)
    local lst = {}
    for i = 1, getn(rows) do
        if AUC_ActName(nType, rows[i]) == szAct then
            tinsert(lst, rows[i])
        end
    end
    local nTotal = getn(lst)
    if nPage < 1 then
        nPage = 1
    end
    local nFrom = (nPage - 1) * AUCTION_DEF.nMaxItemPerPage + 1
    local nTo = nPage * AUCTION_DEF.nMaxItemPerPage
    local tbPage = {}
    local szMe = GetName()
    local nNow = GetCurrentTime()
    local k = 0
    for i = nFrom, nTo do
        if lst[i] then
            k = k + 1
            tbPage[k] = AUC_RowToClient(lst[i], szMe, nNow)
        end
    end
    local h = OB_Create()
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nType)
    ObjBuffer:PushByType(h, OBJTYPE_STRING, szAct)
    ObjBuffer:PushByType(h, OBJTYPE_TABLE, tbPage)
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nTotal)
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nPage)
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, 1)
    AUC_SendTo(nPlayerIdx, "emSCRIPT_PROTOCOL_AUCTION_ACTIVITYINFO", h)
    OB_Release(h)
end

-- bao cho moi nguoi dang xem (nType, szAct): szEnum voi ham push tham so
function AUC_Broadcast(nType, szAct, szEnum, fnPush)
    for nIdx, v in AUC_Viewers do
        if v.nType == nType and (szAct == nil or v.szAct == szAct or v.szAct == "") then
            local h = OB_Create()
            fnPush(h)
            if AUC_SendTo(nIdx, szEnum, h) ~= 1 then
                AUC_Viewers[nIdx] = nil
            end
            OB_Release(h)
        end
    end
end

function AUC_NotifyNewItem(nType, szAct, nId)
    local nCount = 0
    local tb = AUC_Activities(nType, 0)
    if tb[szAct] then
        nCount = tb[szAct].nTotalCount
    end
    AUC_Broadcast(nType, szAct, "emSCRIPT_PROTOCOL_AUCTION_NEWITEM", function(h)
        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nType)
        ObjBuffer:PushByType(h, OBJTYPE_STRING, %szAct)
        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nId)
        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nCount)
    end)
end

function AUC_NotifyEndItem(nType, szAct, nId)
    local nCount = 0
    local tb = AUC_Activities(nType, 0)
    if tb[szAct] then
        nCount = tb[szAct].nTotalCount
    end
    AUC_Broadcast(nType, szAct, "emSCRIPT_PROTOCOL_AUCTION_ENDITEM", function(h)
        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nType)
        ObjBuffer:PushByType(h, OBJTYPE_STRING, %szAct)
        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nId)
        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nCount)
    end)
end

function AUC_NotifyPrice(nType, szAct, r, nNow)
    local tb = AUC_RowToClient(r, "", nNow)
    AUC_Broadcast(nType, szAct, "emSCRIPT_PROTOCOL_AUCTION_TPRICECHANGE", function(h)
        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nType)
        ObjBuffer:PushByType(h, OBJTYPE_STRING, %szAct)
        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %tb.nId)
        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %tb.nCurPrice)
        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %tb.nNextPrice)
        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %tb.nRemainingTime)
        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %tb.nCurFloatTimes)
    end)
end

function AUC_NotifyNewActivity(nType, szAct, nStart)
    AUC_Broadcast(nType, nil, "emSCRIPT_PROTOCOL_AUCTION_NEWACTIVITY", function(h)
        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nType)
        ObjBuffer:PushByType(h, OBJTYPE_STRING, %szAct)
        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nStart)
    end)
end

function AUC_NotifyEndActivity(nType, szAct)
    AUC_Broadcast(nType, nil, "emSCRIPT_PROTOCOL_AUCTION_ENDACTIVITY", function(h)
        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nType)
        ObjBuffer:PushByType(h, OBJTYPE_STRING, %szAct)
    end)
end

-- ---------------------------------------------------------------- nhan tu client (protocol_def_gs.lua)
function AUC_MyTong()
    local _, nTong = GetTongName()
    return nTong or 0
end

function AUC_OnRequestActivityList(nType, nStartTime)
    AUC_Viewers[PlayerIndex] = {nType = nType, szAct = ""}
    AUC_SendActivityList(PlayerIndex, nType, AUC_MyTong())
end

function AUC_OnRequestActivityContent(nType, szAct, nPage, tbIds)
    AUC_Viewers[PlayerIndex] = {nType = nType, szAct = szAct}
    AUC_SendActivityContent(PlayerIndex, nType, AUC_MyTong(), szAct, nPage)
end

function AUC_OnRequestItemContent(nType, szAct, nId)
    local r = AUC_Get(nId)
    if not r then
        return
    end
    local h = OB_Create()
    ObjBuffer:PushByType(h, OBJTYPE_TABLE, AUC_RowToClient(r, GetName(), GetCurrentTime()))
    AUC_SendTo(PlayerIndex, "emSCRIPT_PROTOCOL_AUCTION_ITEMINFO", h)
    OB_Release(h)
end

function AUC_OnRequestMemberList(szAct)
    local nTong = AUC_MyTong()
    local szMem = ""
    local nCount = 0
    if nTong > 0 then
        local nMem = TONG_GetFirstMember(nTong, -1)
        while nMem and nMem > 0 and nCount < 60 do
            local szTen = TONGM_GetName(nTong, nMem)
            if szTen and szTen ~= "" then
                -- [A9] noi thanh CHUOI (bang long ba tang khong qua duoc ObjBuffer)
                if szMem ~= "" then
                    szMem = szMem..";"
                end
                szMem = szMem..szTen..","..((TONGM_GetLevel and TONGM_GetLevel(nTong, nMem)) or 0)..","..(TONGM_GetFigure(nTong, nMem) or 3)..","..(TONGM_GetOnline(nTong, nMem) or 0)
                nCount = nCount + 1
            end
            nMem = TONG_GetNextMember(nTong, nMem, -1)
        end
    end
    local tb = {szActivityName = szAct, szMemberList = szMem, nCount = nCount, nSalary = 0}
    if nTong > 0 and nCount > 0 then
        tb.nSalary = floor(TONG_GetMoney(nTong) / nCount)
    end
    local h = OB_Create()
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, AUCTION_DEF.tbAuctionTypeEnum.eType_TONG)
    ObjBuffer:PushByType(h, OBJTYPE_TABLE, tb)
    AUC_SendTo(PlayerIndex, "emSCRIPT_PROTOCOL_AUCTION_MEMBERLIST", h)
    OB_Release(h)
end

function AUC_ReplyOffer(nType, szAct, nId, nPrice)
    local h = OB_Create()
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nType)
    ObjBuffer:PushByType(h, OBJTYPE_STRING, szAct)
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nId)
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nPrice)
    AUC_SendTo(PlayerIndex, "emSCRIPT_PROTOCOL_AUCTION_OFFERPRICERET", h)
    OB_Release(h)
end

-- kiem tra chung: dong con ban, dung loai, dung bang
function AUC_CheckRow(r, nType)
    if not r or r.state ~= 0 then
        Msg2Player("VËt phÈm nµy ®· b¸n hoÆc ®· kÕt thóc.")
        return 0
    end
    if r.atype ~= nType then
        return 0
    end
    if nType == AUCTION_DEF.tbAuctionTypeEnum.eType_TONG and r.tong ~= AUC_MyTong() then
        Msg2Player("Phiªn nµy cña bang kh¸c.")
        return 0
    end
    return 1
end

-- MUA NGAY / ky gui / Ha Lan: tra dung gia hien tai
function AUC_OnRequestOfferDutch(nType, szAct, nId, nPrice)
    local r = AUC_Get(nId)
    if AUC_CheckRow(r, nType) == 0 then
        return
    end
    if r.seller == GetName() then
        Msg2Player("Kh«ng thÓ mua mãn do chÝnh m×nh ký göi.")
        return
    end
    if nPrice < r.cur then
        Msg2Player("Gi¸ ®· thay ®æi, h·y xem l¹i.")
        return
    end
    if AUC_GetMoney(r.currency) < r.cur then
        Msg2Player("Kh«ng ®ñ "..AUC_CurName(r.currency).." (cÇn "..r.cur..").")
        return
    end
    if AUC_Buy(nId, GetName(), r.cur) ~= 1 then
        Msg2Player("Cã ng­êi võa mua tr­íc, xin lçi ®¹i hiÖp.")
        return
    end
    if AUC_PayMoney(r.currency, r.cur) ~= 1 then
        -- tra lai trang thai (hiem: tien vua doi)
        AUC_SetState(nId, 0, 2)
        Msg2Player("Trõ tiÒn thÊt b¹i.")
        return
    end
    AUC_Settle(r, GetName(), r.cur)
    AUC_ReplyOffer(nType, szAct, nId, r.cur)
    AUC_NotifyEndItem(nType, AUC_ActName(nType, r), nId)
end

-- KIEU ANH: tra gia (giu tien nguoi tra; nguoi bi vuot duoc hoan qua thu)
function AUC_OnRequestOfferEnglish(nType, szAct, nId, nNewPrice)
    local r = AUC_Get(nId)
    if AUC_CheckRow(r, nType) == 0 then
        return
    end
    if r.kind ~= AUCTION_DEF.tbItemTypeEnum.eType_ENGLISH then
        return AUC_OnRequestOfferDutch(nType, szAct, nId, nNewPrice)
    end
    if r.seller == GetName() then
        Msg2Player("Kh«ng thÓ tù tr¶ gi¸ mãn cña m×nh.")
        return
    end
    local nStep = floor(r.guar / 10)
    if nStep < 1 then
        nStep = 1
    end
    local nMin = r.guar
    if r.buyer ~= "" then
        nMin = r.cur + nStep
    end
    if nNewPrice < nMin then
        Msg2Player("Gi¸ tr¶ ph¶i tõ "..nMin.." "..AUC_CurName(r.currency)..".")
        return
    end
    if AUC_GetMoney(r.currency) < nNewPrice then
        Msg2Player("Kh«ng ®ñ "..AUC_CurName(r.currency)..".")
        return
    end
    local nNow = GetCurrentTime()
    local nEnd = r.endtime
    if nEnd - nNow < AUCTION_DEF.nRefreshRemingTimeWhenOffer then
        nEnd = nNow + AUCTION_DEF.nRefreshRemingTimeWhenOffer
    end
    local szOld, nOld = r.buyer, r.cur
    if AUC_Bid(nId, GetName(), nNewPrice, nEnd) ~= 1 then
        Msg2Player("Cã ng­êi võa tr¶ gi¸ cao h¬n, h·y xem l¹i.")
        return
    end
    if AUC_PayMoney(r.currency, nNewPrice) ~= 1 then
        Msg2Player("Trõ tiÒn thÊt b¹i.")
        AUC_Log(format("LOI: bid id %d cua %s da ghi nhung tru tien that bai", nId, GetName()))
        return
    end
    if szOld ~= "" and szOld ~= GetName() and nOld > 0 then
        AUC_MailMoney(szOld, "Hoµn tiÒn ®Êu gi¸", "Cã ng­êi tr¶ gi¸ cao h¬n cho "..r.name..", hoµn l¹i tiÒn ®· tr¶.", r.currency, nOld)
    elseif szOld == GetName() and nOld > 0 then
        -- tu nang gia cua minh: hoan phan cu
        AUC_MailMoney(szOld, "Hoµn tiÒn ®Êu gi¸", "§¹i hiÖp n©ng gi¸ cho "..r.name..", hoµn l¹i møc tr¶ tr­íc.", r.currency, nOld)
    end
    Msg2Player("§· tr¶ gi¸ "..nNewPrice.." "..AUC_CurName(r.currency).." cho "..r.name..".")
    AUC_ReplyOffer(nType, szAct, nId, nNewPrice)
    local r2 = AUC_Get(nId)
    if r2 then
        AUC_NotifyPrice(nType, AUC_ActName(nType, r2), r2, nNow)
    end
end

function AUC_OnRequestRefund(nType, szAct, nId)
    local r = AUC_Get(nId)
    if not r then
        return
    end
    if r.state == 0 and r.buyer == GetName() then
        Msg2Player("§¹i hiÖp ®ang gi÷ gi¸ cao nhÊt, kh«ng thÓ rót l¹i.")
    else
        Msg2Player("§¹i hiÖp kh«ng cã møc gi¸ nµo ®ang gi÷ ë mãn nµy.")
    end
    local h = OB_Create()
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nType)
    ObjBuffer:PushByType(h, OBJTYPE_STRING, szAct)
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nId)
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, 0)
    AUC_SendTo(PlayerIndex, "emSCRIPT_PROTOCOL_AUCTION_REFUNDRET", h)
    OB_Release(h)
end

-- lay lai mon ky gui (khong hoan phi)
function AUC_OnRequestGetBack(nType, szAct, nId)
    local r = AUC_Get(nId)
    if AUC_CheckRow(r, nType) == 0 then
        return
    end
    if r.seller ~= GetName() then
        Msg2Player("Kh«ng ph¶i mãn cña ®¹i hiÖp.")
        return
    end
    if r.buyer ~= "" then
        Msg2Player("§· cã ng­êi tr¶ gi¸, kh«ng thÓ rót l¹i.")
        return
    end
    if AUC_SetState(nId, 3, 1) ~= 1 then
        Msg2Player("Kh«ng rót ®­îc, h·y thö l¹i.")
        return
    end
    AUC_MailItem(r.seller, "Rót vËt phÈm ký göi", "§¹i hiÖp ®· rót "..r.name.." khái khu ®Êu gi¸, phÝ ký göi kh«ng hoµn.", nId)
    Msg2Player("§· rót "..r.name..", vËt phÈm göi vÒ hép th­.")
    AUC_Log(format("%s rut lai id %d (%s)", GetName(), nId, r.name))
    AUC_NotifyEndItem(nType, AUC_ActName(nType, r), nId)
end

-- ---------------------------------------------------------------- chot giao dich
-- r: dong (truoc khi doi), szBuyer, nPrice: gia chot. Giao do cho nguoi mua, tien cho nguoi ban / quy bang.
function AUC_Settle(r, szBuyer, nPrice)
    local nTax = floor(nPrice * AUCTION_DEF.nAuctionTaxRate / 100)
    local nNet = nPrice - nTax
    AUC_MailItem(szBuyer, "§Êu gi¸ thµnh c«ng", "§¹i hiÖp ®· mua ®­îc "..r.name.." víi gi¸ "..nPrice.." "..AUC_CurName(r.currency)..". VËt phÈm ®Ýnh kÌm trong th­.", r.id)
    if r.atype == AUCTION_DEF.tbAuctionTypeEnum.eType_TONG and r.tong > 0 then
        if r.currency == AUCTION_DEF.tbCurrency.MONEY then
            TONG_ApplyAddMoney(r.tong, nNet)
            TONG_ApplyAddEventRecord(r.tong, "§Êu gi¸ bang: b¸n "..r.name.." ®­îc "..nNet.." Ng©n l­îng vµo quü")
        else
            AUC_MailMoney(r.seller, "TiÒn b¸n ®Êu gi¸ bang", "B¸n "..r.name.." trong phiªn bang, thu "..nNet.." Xu.", r.currency, nNet)
        end
    elseif r.atype == AUCTION_DEF.tbAuctionTypeEnum.eType_WORLD then
        -- phien the gioi do GM mo: tien vao he thong (khong tra ai)
    else
        AUC_MailMoney(r.seller, "TiÒn b¸n ký göi", "Mãn "..r.name.." ®· b¸n ®­îc "..nPrice.." "..AUC_CurName(r.currency)..", trõ thuÕ "..nTax..", ®¹i hiÖp nhËn "..nNet..".", r.currency, nNet)
    end
    AUC_SetState(r.id, 3, 3)
    AUC_Log(format("BAN id %d %s: %s -> %s gia %d tien %d (thue %d)", r.id, r.name, r.seller, szBuyer, nPrice, r.currency, nTax))
end

-- het han / khong ai mua: tra do ve nguoi ban
function AUC_Expire(r)
    if AUC_SetState(r.id, 2, 1) ~= 1 then
        return
    end
    if r.atype == AUCTION_DEF.tbAuctionTypeEnum.eType_WORLD then
        AUC_Log(format("LUU PHACH the gioi id %d %s", r.id, r.name))
        AUC_SetState(r.id, 3, 3)
        return
    end
    AUC_MailItem(r.seller, "VËt phÈm ch­a b¸n ®­îc", "Mãn "..r.name.." hÕt h¹n mµ ch­a ai mua, tr¶ l¹i ®¹i hiÖp.", r.id)
    AUC_Log(format("HET HAN id %d %s tra %s", r.id, r.name, r.seller))
end

-- kieu Anh ket thuc co nguoi tra gia
function AUC_FinishEnglish(r)
    if r.buyer == "" then
        return AUC_Expire(r)
    end
    if AUC_SetState(r.id, 1, 1) ~= 1 then
        return
    end
    AUC_Settle(r, r.buyer, r.cur)
end

-- ---------------------------------------------------------------- DAT BAN (goi tu NPC / GM)
-- nType, szAct, nKind, nCur, nPrice, nItemIdx, nTong -> id (0 = loi). Chinh nguoi choi hien tai la nguoi ban.
function AUC_PutOnItem(nType, szAct, nKind, nCur, nPrice, nItemIdx, nTong)
    if nItemIdx == nil or nItemIdx <= 0 then
        return 0
    end
    local szRec, szName, szDesc, nCells = AUC_ItemToRec(nItemIdx)
    if szRec == nil or szRec == "" then
        Msg2Player("Mãn nµy kh«ng thÓ ký göi (quÆng, nguyªn liÖu th«...).")
        return 0
    end
    if GetItemBindState and GetItemBindState(nItemIdx) ~= 0 then
        Msg2Player("VËt phÈm kho¸ kh«ng thÓ ký göi.")
        return 0
    end
    local nNow = GetCurrentTime()
    local nBase, nCurP, nGuar, nEnd, nNextDrop, nDropLeft = nPrice, nPrice, nPrice, nNow + AUCTION_DEF.nPersonalDuration, 0, 0
    if nType ~= AUCTION_DEF.tbAuctionTypeEnum.eType_PERSONAL then
        if nKind == AUCTION_DEF.tbItemTypeEnum.eType_ENGLISH then
            nEnd = nNow + AUCTION_DEF.nEnglishRemainingTime
        else
            nCurP = floor(nPrice * AUCTION_DEF.nDutchInitRate)
            nBase = nCurP
            nNextDrop = nNow + AUCTION_DEF.nDutchFloatInterval
            nDropLeft = AUCTION_DEF.nDutchFloatTimes
            nEnd = nNow + AUCTION_DEF.nDutchFloatInterval * (AUCTION_DEF.nDutchFloatTimes + 1)
        end
    end
    local nDeposit = 0
    if nType == AUCTION_DEF.tbAuctionTypeEnum.eType_PERSONAL then
        nDeposit = floor(nPrice * AUCTION_DEF.nPersonalPutOnCost / 100)
        if AUC_PayMoney(nCur, nDeposit) ~= 1 then
            Msg2Player("Kh«ng ®ñ "..AUC_CurName(nCur).." ®Ó tr¶ phÝ ký göi "..nDeposit..".")
            return 0
        end
    end
    local nId = AUC_PutOn(nType, szAct or "", nKind, GetName(), nTong or 0, szName, szDesc, szRec, nCells or 1, nCur,
        nBase, nCurP, nGuar, nDeposit, nNow, nEnd, nNextDrop, nDropLeft)
    if nId <= 0 then
        if nDeposit > 0 then
            if nCur == AUCTION_DEF.tbCurrency.XU then
                SetTask(AUCTION_DEF.XU_TASK, GetTask(AUCTION_DEF.XU_TASK) + nDeposit)
            else
                Earn(nDeposit)
            end
        end
        Msg2Player("Kho ®Êu gi¸ lçi, h·y thö l¹i sau.")
        return 0
    end
    RemoveItemByIndex(nItemIdx, GetItemStackCount(nItemIdx))
    AUC_Log(format("DAT BAN id %d loai %d '%s' %s gia %d tien %d nguoi %s", nId, nType, szAct or "", szName, nPrice, nCur, GetName()))
    local szActReal = szAct
    if nType == AUCTION_DEF.tbAuctionTypeEnum.eType_PERSONAL then
        szActReal = AUCTION_DEF.szPersonalActivity
    end
    AUC_NotifyNewItem(nType, szActReal, nId)
    return nId
end

-- ---------------------------------------------------------------- DAT BAN TU NUT TREN CUA SO (khong dung NPC)
-- Chu 04/09: "khong can lam npc ma lam nut tren box dau gia luon". Nut goc phai moi trang gui
-- REQUEST_PUTON; o day mo hai hop nhap so roi hop dat vat pham. CA BA hop deu CHI DINH duong dan script
-- (OpenGetNumber 4 doi, GiveItemUI 8 doi) nen callback chay dung trong tep nay du khong thoai voi NPC nao.
AUC_SCRIPT = "\\script\\auction_house\\auction_manager.lua"
AUC_TMP = AUC_TMP or {}

function AUC_OnRequestPutOn(nType)
    local szTong, nTong = GetTongName()
    nTong = nTong or 0
    local nKind = AUCTION_DEF.tbItemTypeEnum.eType_ENGLISH
    if nType == AUCTION_DEF.tbAuctionTypeEnum.eType_PERSONAL then
        nKind = AUCTION_DEF.tbItemTypeEnum.eType_DUTCH
        if AUC_CountSeller(GetName()) >= AUCTION_DEF.nMaxItemPerSeller then
            Msg2Player("§¹i hiÖp ®ang ký göi tèi ®a "..AUCTION_DEF.nMaxItemPerSeller.." mãn, h·y chê b¸n xong hoÆc rót bít.")
            return
        end
    elseif nType == AUCTION_DEF.tbAuctionTypeEnum.eType_WORLD then
        if not (admincheck and admincheck() == 1) then
            Msg2Player("ChØ GM míi më ®­îc phiªn ®Êu gi¸ thÕ giíi.")
            return
        end
    else
        if nTong <= 0 then
            Msg2Player("§¹i hiÖp ch­a gia nhËp bang héi.")
            return
        end
        if TONG_GetMaster(nTong) ~= GetName() then
            -- khong phai bang chu: nut nay xem DANH SACH THANH VIEN cua phien bang
            AUC_OnRequestMemberList(szTong)
            return
        end
    end
    -- [A6] MOT hop duy nhat: hop dua vat pham da co o nhap gia + nut doi loai tien.
    -- Client gui AUCTION_REQUEST_SETPRICE (gia, loai tien) NGAY TRUOC khi bam Dong y.
    AUC_TMP[PlayerIndex] = {nType = nType, nKind = nKind, nTong = nTong, nCur = AUCTION_DEF.tbCurrency.MONEY, nPrice = 0}
    GiveItemUI("Ký göi ®Êu gi¸: ®Æt vËt phÈm vµo «, nhËp gi¸ råi bÊm §ång ý", "PhÝ ký göi "..AUCTION_DEF.nPersonalPutOnCost.."% gi¸ b¸n, thuÕ "..AUCTION_DEF.nAuctionTaxRate.."% khi b¸n ®­îc", "AUC_OnGiveOk", "AUC_OnGiveCancel", 0, "AUC_OnGiveCheck", 0, AUC_SCRIPT)
end

-- [A6] client bao GIA + LOAI TIEN (o ngay trong hop dua vat pham) truoc khi bam Dong y
function AUC_OnRequestSetPrice(nPrice, nCur)
    local t = AUC_TMP[PlayerIndex]
    if not t then
        return
    end
    t.nPrice = floor(nPrice or 0)
    if nCur == AUCTION_DEF.tbCurrency.XU then
        t.nCur = AUCTION_DEF.tbCurrency.XU
    else
        t.nCur = AUCTION_DEF.tbCurrency.MONEY
    end
end

function AUC_OnGiveCheck(nCount)
    return 1
end

function AUC_OnGiveCancel()
    AUC_TMP[PlayerIndex] = nil
end

function AUC_OnGiveOk(nCount)
    local t = AUC_TMP[PlayerIndex]
    AUC_TMP[PlayerIndex] = nil
    if not t then
        return
    end
    if (t.nPrice or 0) < 1 or t.nPrice > 2000000000 then
        Msg2Player("Ch­a nhËp gi¸ b¸n hîp lÖ.")
        return
    end
    local nIdx = GetGiveItemUnit(1)
    if nIdx == nil or nIdx <= 0 then
        Msg2Player("Ch­a ®Æt vËt phÈm vµo «.")
        return
    end
    local szAct = ""
    if t.nType == AUCTION_DEF.tbAuctionTypeEnum.eType_TONG then
        szAct = GetTongName()
    elseif t.nType == AUCTION_DEF.tbAuctionTypeEnum.eType_WORLD then
        szAct = "Phiªn "..GetLocalDate("%H:%M %d/%m")
    end
    local nId = AUC_PutOnItem(t.nType, szAct, t.nKind, t.nCur, t.nPrice, nIdx, t.nTong)
    if nId > 0 then
        Msg2Player("§· ®­a vµo khu ®Êu gi¸, m· sè "..nId..". TiÒn b¸n vµ vËt phÈm tr¶ vÒ qua hép th­.")
    end
end

-- ---------------------------------------------------------------- QUET (auctionpoll.lua goi moi 30 giay)
function AUC_Tick()
    local nNow = GetCurrentTime()
    -- 1) het han / ket thuc
    local tb = AUC_Sweep(nNow, 30)
    for i = 1, getn(tb) do
        local r = tb[i]
        local szAct = AUC_ActName(r.atype, r)
        if r.kind == AUCTION_DEF.tbItemTypeEnum.eType_ENGLISH then
            AUC_FinishEnglish(r)
        else
            AUC_Expire(r)
        end
        AUC_NotifyEndItem(r.atype, szAct, r.id)
    end
    -- 2) Ha Lan giam gia (the gioi / bang hoi)
    for _, nType in {AUCTION_DEF.tbAuctionTypeEnum.eType_WORLD, AUCTION_DEF.tbAuctionTypeEnum.eType_TONG} do
        local rows = AUC_List(nType, 200, 0)
        for i = 1, getn(rows) do
            local r = rows[i]
            if r.kind == AUCTION_DEF.tbItemTypeEnum.eType_DUTCH and r.dropleft > 0 and r.nextdrop > 0 and r.nextdrop <= nNow then
                local nNext = floor(r.cur * (1 - AUCTION_DEF.nDutchFloatRange))
                if nNext < r.guar then
                    nNext = r.guar
                end
                if AUC_SetPrice(r.id, nNext, nNow + AUCTION_DEF.nDutchFloatInterval, r.dropleft - 1, r.endtime) == 1 then
                    r.cur = nNext
                    r.nextdrop = nNow + AUCTION_DEF.nDutchFloatInterval
                    r.dropleft = r.dropleft - 1
                    AUC_NotifyPrice(nType, AUC_ActName(nType, r), r, nNow)
                end
            end
        end
        -- 3) phien het mon -> bao ket thuc phien
        local tbAct = AUC_Activities(nType, 0)
        for k, v in AUC_LastCount do
            local a, b, t, n = strfind(k, "^(%d+)|(.*)$")
            if a and tonumber(t) == nType and not tbAct[n] then
                AUC_NotifyEndActivity(nType, n)
                AUC_LastCount[k] = nil
            end
        end
        for n, v in tbAct do
            AUC_LastCount[nType.."|"..n] = v.nTotalCount
        end
    end
end
