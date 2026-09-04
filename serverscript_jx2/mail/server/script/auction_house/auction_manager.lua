-- auction_manager.lua - [DAUGIA 04/09] bo may DAU GIA phia may chu. Kho: bang MySQL auction_item (KAuctionServer.cpp,
-- 14 ham AUC_*). Giao/tra VAT PHAM va TIEN deu qua THU (mailmanager.lua) nen nguoi offline khong mat gi; vat pham giu
-- NGUYEN thuoc tinh (AUC_ItemToRec / AUC_GiveRec). Client: \script\ui\uiauction_house.lua. Giao thuc: 20 ten AUCTION_*.
-- Loai phien: 1 bang hoi (chi thanh vien bang, tien ban vao QUY BANG), 2 the gioi (GM mo, tien ban vao he thong),
-- 3 ca nhan = ky gui (moi nguoi dat ban, 24 gio, gia co dinh, tien ve nguoi ban tru thue).
Include("\\script\\protocol.lua")
Include("\\script\\lib\\objbuffer_head.lua")
Include("\\script\\auction_house\\auction_def.lua")
Include("\\script\\mail\\mailmanager.lua")

-- [B3] [PlayerIndex] = {nType, szAct, szName, nTong}: luu TEN de biet o nguoi choi da bi cap lai
-- cho nguoi khac, va luu BANG de khong gui goi phien bang A cho nguoi bang B.
AUC_Viewers = AUC_Viewers or {}
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

-- [A29] viet so tien cho gon: 1 van = 10.000 ; 1 uc = 10.000 van. Dung trong MOI cau thong bao
-- va noi dung thu - chu bao "thong bao trong thu van chua rut gon, van day so dai".
function AUC_SoTien(n)
    n = floor(n or 0)
    if n < 0 then
        n = 0
    end
    -- [A32] chu chot: KHONG dung "uc", cu dem theo van (100 van -> 1000 van -> 10000 van)
    if n >= 10000 then
        local nVan = floor(n / 10000)
        local nLe = mod(n, 10000)
        if nLe > 0 then
            return nVan.." v¹n "..nLe
        end
        return nVan.." v¹n"
    end
    return ""..n
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
-- [A19] AUC_List cat theo trang (nMax, nAfterId). Truoc day goi dung MOT lan voi 200 nen
-- qua 200 mon la tong so va so trang bi cat AM THAM. Nay lap theo con tro id.
AUC_ROWS_MAX = 2000
function AUC_Rows(nType, nTong)
    local tb = {}
    local nAfter = 0
    while getn(tb) < AUC_ROWS_MAX do
        local lo = AUC_List(nType, 200, nAfter)
        local n = getn(lo)
        if n <= 0 then
            break
        end
        for i = 1, n do
            tinsert(tb, lo[i])
        end
        nAfter = lo[n].id
        if n < 200 then
            break
        end
    end
    if getn(tb) >= AUC_ROWS_MAX then
        AUC_Log(format("CANH BAO: loai %d da cham tran %d dong - danh sach bi cat", nType, AUC_ROWS_MAX))
    end
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
    -- [A16] Bo mat THAT cua mon nam trong cot item_rec chu khong phai item_desc (chi 6 so).
    -- Hoang kim nhan dien bang CAP (nature = NATURE_GOLD, row); thieu no thi client dung lai
    -- mot mon thuong khac han. Doc thang tu rec nen CA cac mon dat ban tu truoc cung hien dung,
    -- khong phai doi cot hay chuyen doi CSDL.
    local szInfo, nStk = AUC_RecDesc(r.rec)
    tb.szInfo = szInfo or ""
    tb.nCount = nStk or 1
    if tb.nCount < 1 then
        tb.nCount = 1
    end
    tb.nGuaranteedPrice = r.guar
    tb.nCurPrice = r.cur
    -- [A17] chua ai tra gia thi KHONG duoc bao "gia cao nhat": client lay nMaxPrice + buoc gia
    -- lam gia de nghi, nen nguoi tra dau tien bi day len thua mot buoc.
    tb.nMaxPrice = 0
    if r.buyer ~= "" then
        tb.nMaxPrice = r.cur
    end
    -- [A20b] gia MUA NGAY (chi dong ky gui ca nhan moi co) - client ve nut "Mua ngay"
    tb.nBuyNow = 0
    if r.atype == AUCTION_DEF.tbAuctionTypeEnum.eType_PERSONAL and r.kind == AUCTION_DEF.tbItemTypeEnum.eType_ENGLISH then
        tb.nBuyNow = r.base or 0
    end
    -- buoc moi luot = 10% GIA CO BAN (dung y chu chot 04/09)
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
    -- [A14b] ky gui ha THANG ve gia san mot nhip, phien the gioi/bang hoi ha 10% moi nhip.
    -- O "Gia ke tiep" phai bao dung cai vong quet se lam, khong thi nguoi mua thay mot dang tra mot neo.
    local bCaNhan = (r.atype == AUCTION_DEF.tbAuctionTypeEnum.eType_PERSONAL)
    if bCaNhan then
        tb.nFloatInterval = AUCTION_DEF.nPersonalFloatInterval
        tb.nTotalFloatTimes = AUCTION_DEF.nPersonalFloatTimes
        tb.nCurFloatTimes = AUCTION_DEF.nPersonalFloatTimes - r.dropleft
    else
        tb.nFloatInterval = AUCTION_DEF.nDutchFloatInterval
        tb.nTotalFloatTimes = AUCTION_DEF.nDutchFloatTimes
        tb.nCurFloatTimes = AUCTION_DEF.nDutchFloatTimes - r.dropleft
    end
    if r.kind == AUCTION_DEF.tbItemTypeEnum.eType_DUTCH and r.dropleft > 0 and r.nextdrop > 0 then
        tb.nRemainingTime = r.nextdrop - nNow
        local nNext = r.guar
        if not bCaNhan then
            nNext = floor(r.cur * (1 - AUCTION_DEF.nDutchFloatRange))
            if nNext < r.guar then
                nNext = r.guar
            end
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
-- nTongLoc: chi phien bang hoi moi dung (chi gui cho nguoi CUNG bang)
function AUC_Broadcast(nType, szAct, szEnum, fnPush, nTongLoc)
    for nIdx, v in AUC_Viewers do
        if v.nType == nType and (nTongLoc == nil or nTongLoc == 0 or v.nTong == nTongLoc)
            and (szAct == nil or v.szAct == szAct or v.szAct == "") then
            local h = OB_Create()
            fnPush(h)
            if AUC_SendTo(nIdx, szEnum, h) ~= 1 then
                AUC_Viewers[nIdx] = nil
            end
            OB_Release(h)
        end
    end
end

-- [B3] PHAI truyen nTong that: AUC_Rows loc theo bang, truyen 0 thi phien bang luon dem = 0
function AUC_NotifyNewItem(nType, szAct, nId, nTong)
    local nCount = 0
    local tb = AUC_Activities(nType, nTong or 0)
    if tb[szAct] then
        nCount = tb[szAct].nTotalCount
    end
    AUC_Broadcast(nType, szAct, "emSCRIPT_PROTOCOL_AUCTION_NEWITEM", function(h)
        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nType)
        ObjBuffer:PushByType(h, OBJTYPE_STRING, %szAct)
        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nId)
        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nCount)
    end, nTong)
end

-- [B3] PHAI truyen nTong that: AUC_Rows loc theo bang, truyen 0 thi phien bang luon dem = 0
function AUC_NotifyEndItem(nType, szAct, nId, nTong)
    local nCount = 0
    local tb = AUC_Activities(nType, nTong or 0)
    if tb[szAct] then
        nCount = tb[szAct].nTotalCount
    end
    AUC_Broadcast(nType, szAct, "emSCRIPT_PROTOCOL_AUCTION_ENDITEM", function(h)
        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nType)
        ObjBuffer:PushByType(h, OBJTYPE_STRING, %szAct)
        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nId)
        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nCount)
    end, nTong)
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
    AUC_Viewers[PlayerIndex] = {nType = nType, szAct = "", szName = GetName(), nTong = AUC_MyTong()}
    AUC_SendActivityList(PlayerIndex, nType, AUC_MyTong())
end

function AUC_OnRequestActivityContent(nType, szAct, nPage, tbIds)
    AUC_Viewers[PlayerIndex] = {nType = nType, szAct = szAct, szName = GetName(), nTong = AUC_MyTong()}
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
    -- [A20] Hai duong duoc phep mua dut:
    --   dong Ha Lan (phien the gioi/bang): mua o gia dang ha dan
    --   dong ky gui ca nhan: mua o GIA MUA NGAY (cot base_price)
    -- Ngoai hai duong do thi tu choi, khong thi ghi de nguoi dang giu gia cao nhat cua dong
    -- kieu Anh ma khong hoan tien ho (loi [B2] cu).
    local nGiaMua = r.cur
    local bKyGui = 0
    if r.kind ~= AUCTION_DEF.tbItemTypeEnum.eType_DUTCH then
        if r.atype == AUCTION_DEF.tbAuctionTypeEnum.eType_PERSONAL and (r.base or 0) > 0 then
            nGiaMua = r.base
            bKyGui = 1
        else
            Msg2Player("Mãn nµy ®Êu theo kiÓu t¨ng gi¸, h·y dïng nót B¸o gi¸.")
            return
        end
    end
    if r.seller == GetName() then
        Msg2Player("Kh«ng thÓ mua mãn do chÝnh m×nh ký göi.")
        return
    end
    -- [A20b] nGiaMua = gia phai tra: dong Ha Lan la gia dang ha dan, dong ky gui la GIA MUA NGAY
    if nPrice < nGiaMua then
        Msg2Player("Gi¸ ®· thay ®æi, h·y xem l¹i.")
        return
    end
    if AUC_GetMoney(r.currency) < nGiaMua then
        Msg2Player("Kh«ng ®ñ "..AUC_CurName(r.currency).." (cÇn "..AUC_SoTien(nGiaMua)..").")
        return
    end
    -- nho nguoi dang giu gia cao nhat TRUOC khi AUC_Buy ghi de ten nguoi mua
    local szGiuGia, nGiuGia = r.buyer, r.cur
    if AUC_Buy(nId, GetName(), nGiaMua) ~= 1 then
        Msg2Player("Cã ng­êi võa mua tr­íc, xin lçi ®¹i hiÖp.")
        return
    end
    if AUC_PayMoney(r.currency, nGiaMua) ~= 1 then
        -- tra lai trang thai (hiem: tien vua doi)
        -- [B2] PHAI xoa luon nguoi mua, khong thi nguoi ban khong rut lai duoc va kieu Anh
        -- se giao mon cho nguoi chua tra dong nao khi het gio.
        AUC_Rollback(nId)
        -- [A24] AUC_Rollback xoa nguoi mua nhung KHONG tra cur_price, nen AUC_Bid ben duoi
        -- truot chot cur_price < ? va nguoi giu gia cu khong bao gio duoc khoi phuc.
        if bKyGui == 1 and szGiuGia ~= "" and nGiuGia > 0 then
            AUC_SetPrice(nId, 0, 0, 0, r.endtime)
            AUC_Bid(nId, szGiuGia, nGiuGia, r.endtime)
        end
        Msg2Player("Trõ tiÒn thÊt b¹i.")
        return
    end
    -- [A20b] Mua dut mot dong DANG CO nguoi tra gia: tien cua nguoi do da bi tru tu luc tra,
    -- phai hoan NGAY qua thu, khong thi ho mat trang (chinh la loi [B2] o duong nguoc lai).
    if szGiuGia ~= "" and nGiuGia > 0 then
        AUC_MailMoney(szGiuGia, "Hoµn tiÒn ®Êu gi¸", "Cã ng­êi mua ngay "..r.name..", hoµn l¹i tiÒn ®¹i hiÖp ®· tr¶.", r.currency, nGiuGia)
    end
    AUC_Settle(r, GetName(), nGiaMua)
    AUC_ReplyOffer(nType, szAct, nId, nGiaMua)
    AUC_NotifyEndItem(nType, AUC_ActName(nType, r), nId, r.tong)
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
        Msg2Player("Gi¸ tr¶ ph¶i tõ "..AUC_SoTien(nMin).." "..AUC_CurName(r.currency)..".")
        return
    end
    -- [A24] TRA BANG HOAC VUOT GIA MUA NGAY = CHOT BAN NGAY o dung gia mua ngay.
    -- Truoc day duong tra gia khong co tran tren nen gia cao nhat vuot duoc gia mua ngay,
    -- roi bat ky ai bam Mua ngay cung lay duoc mon o gia THAP HON: nguoi ban mat phan chenh,
    -- nguoi dang giu gia cao nhat bi cuop mon. Chot o day thi cur khong bao gio vuot base nua.
    if (r.base or 0) > 0 and r.atype == AUCTION_DEF.tbAuctionTypeEnum.eType_PERSONAL and nNewPrice >= r.base then
        return AUC_OnRequestOfferDutch(nType, szAct, nId, r.base)
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
        -- [A17] AUC_Bid DA ghi ten nguoi mua moi roi. Return thang o day thi nguoi giu gia cu
        -- khong duoc hoan, ma het gio lai giao mon cho nguoi vua tru tien hong.
        -- [A24] AUC_Rollback o day la MA CHET: no doi WHERE state=1 ma duong tra gia khong he
        -- dat state=1. Duong lui dung la ha cur_price ve 0 roi ghi lai nguoi cu - khong ha thi
        -- AUC_Bid truot chot cur_price < ? (gia bang nhau) va nguoi cu mat sach tien.
        AUC_SetPrice(nId, 0, 0, 0, nEnd)
        if szOld ~= "" and nOld > 0 then
            AUC_Bid(nId, szOld, nOld, nEnd)
        end
        Msg2Player("Trõ tiÒn thÊt b¹i.")
        AUC_Log(format("LOI: bid id %d cua %s da ghi nhung tru tien that bai - da lui dong", nId, GetName()))
        return
    end
    if szOld ~= "" and szOld ~= GetName() and nOld > 0 then
        AUC_MailMoney(szOld, "Hoµn tiÒn ®Êu gi¸", "Cã ng­êi tr¶ gi¸ cao h¬n cho "..r.name..", hoµn l¹i tiÒn ®· tr¶.", r.currency, nOld)
    elseif szOld == GetName() and nOld > 0 then
        -- tu nang gia cua minh: hoan phan cu
        AUC_MailMoney(szOld, "Hoµn tiÒn ®Êu gi¸", "§¹i hiÖp n©ng gi¸ cho "..r.name..", hoµn l¹i møc tr¶ tr­íc.", r.currency, nOld)
    end
    Msg2Player("§· tr¶ gi¸ "..AUC_SoTien(nNewPrice).." "..AUC_CurName(r.currency).." cho "..r.name..".")
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
    -- [B2] gui thu TRUOC roi moi danh dau da xu ly
    if AUC_MailItem(r.seller, "Rót vËt phÈm ký göi", "§¹i hiÖp ®· rót "..r.name.." khái khu ®Êu gi¸, phÝ ký göi kh«ng hoµn.", nId) <= 0 then
        Msg2Player("Kh«ng rót ®­îc, h·y thö l¹i.")
        return
    end
    AUC_SetState(nId, 3, 1)
    Msg2Player("§· rót "..r.name..", vËt phÈm göi vÒ hép th­.")
    AUC_Log(format("%s rut lai id %d (%s)", GetName(), nId, r.name))
    AUC_NotifyEndItem(nType, AUC_ActName(nType, r), nId, r.tong)
end

-- ---------------------------------------------------------------- chot giao dich
-- r: dong (truoc khi doi), szBuyer, nPrice: gia chot. Giao do cho nguoi mua, tien cho nguoi ban / quy bang.
function AUC_Settle(r, szBuyer, nPrice)
    local nTax = floor(nPrice * AUCTION_DEF.nAuctionTaxRate / 100)
    local nNet = nPrice - nTax
    -- [A24] PHAI kiem ket qua gui thu: truoc day bo qua nen thu hong la tien nguoi mua da tru
    -- ma mon bien mat, con dong nam o state = 1 - khong vong quet nao nhin toi, ket vinh vien.
    -- Nay gui hong thi hoan tien nguoi mua va tra dong ve dang ban de ban lai.
    if AUC_MailItem(szBuyer, "§Êu gi¸ thµnh c«ng", "§¹i hiÖp ®· mua ®­îc "..r.name.." víi gi¸ "..AUC_SoTien(nPrice).." "..AUC_CurName(r.currency)..". VËt phÈm ®Ýnh kÌm trong th­.", r.id) <= 0 then
        AUC_MailMoney(szBuyer, "Hoµn tiÒn ®Êu gi¸", "Kh«ng giao ®­îc "..r.name..", hoµn l¹i tiÒn ®¹i hiÖp ®· tr¶.", r.currency, nPrice)
        AUC_Rollback(r.id)
        AUC_SetPrice(r.id, 0, 0, 0, r.endtime)
        AUC_Log(format("LOI: id %d ban cho %s nhung GUI THU HONG - da hoan tien va tra dong ve dang ban", r.id, szBuyer))
        return 0
    end
    if r.atype == AUCTION_DEF.tbAuctionTypeEnum.eType_TONG and r.tong > 0 then
        if r.currency == AUCTION_DEF.tbCurrency.MONEY then
            TONG_ApplyAddMoney(r.tong, nNet)
            TONG_ApplyAddEventRecord(r.tong, "§Êu gi¸ bang: b¸n "..r.name.." ®­îc "..AUC_SoTien(nNet).." Ng©n l­îng vµo quü")
        else
            AUC_MailMoney(r.seller, "TiÒn b¸n ®Êu gi¸ bang", "B¸n "..r.name.." trong phiªn bang, thu "..AUC_SoTien(nNet).." Xu.", r.currency, nNet)
        end
    elseif r.atype == AUCTION_DEF.tbAuctionTypeEnum.eType_WORLD then
        -- phien the gioi do GM mo: tien vao he thong (khong tra ai)
    else
        -- [A20 chu chot] hoan luon TIEN COC khi ban duoc: coc chi mat khi nguoi ban tu rut mon ve
        local nCoc = r.deposit or 0
        AUC_MailMoney(r.seller, "TiÒn b¸n ký göi", "Mãn "..r.name.." ®· b¸n ®­îc "..AUC_SoTien(nPrice).." "..AUC_CurName(r.currency)..", trõ thuÕ "..AUC_SoTien(nTax)..", hoµn cäc "..AUC_SoTien(nCoc)..", ®¹i hiÖp nhËn "..AUC_SoTien((nNet + nCoc))..".", r.currency, nNet + nCoc)
    end
    AUC_SetState(r.id, 3, 3)
    AUC_Log(format("BAN id %d %s: %s -> %s gia %d tien %d (thue %d)", r.id, r.name, r.seller, szBuyer, nPrice, r.currency, nTax))
end

-- het han / khong ai mua: tra do ve nguoi ban
function AUC_Expire(r)
    -- [B2] GUI THU TRUOC roi moi doi trang thai: truoc day doi truoc, thu gui hong la mon ket
    -- trong bang vinh vien (moi vong quet chi lay state = 0).
    if r.atype == AUCTION_DEF.tbAuctionTypeEnum.eType_WORLD then
        AUC_Log(format("LUU PHACH the gioi id %d %s", r.id, r.name))
        AUC_SetState(r.id, 3, 3)
        return
    end
    if AUC_MailItem(r.seller, "VËt phÈm ch­a b¸n ®­îc", "Mãn "..r.name.." hÕt h¹n mµ ch­a ai mua, tr¶ l¹i ®¹i hiÖp.", r.id) <= 0 then
        AUC_Log(format("gui thu tra mon id %d cho %s HONG - giu nguyen de vong quet sau thu lai", r.id, r.seller))
        return
    end
    AUC_SetState(r.id, 2, 1)
    -- [A20 chu chot] het han e hang thi hoan luon tien coc
    if (r.deposit or 0) > 0 then
        AUC_MailMoney(r.seller, "Hoµn cäc ký göi", "Mãn "..r.name.." hÕt h¹n ch­a b¸n ®­îc, hoµn l¹i tiÒn cäc.", r.currency, r.deposit)
    end
    AUC_Log(format("HET HAN id %d %s tra %s (hoan coc %d)", r.id, r.name, r.seller, r.deposit or 0))
end

-- kieu Anh ket thuc co nguoi tra gia
function AUC_FinishEnglish(r)
    -- [A24] PHAI kiem ca gia: neu vi mot duong lui nao do ma dong con ten nguoi mua trong khi
    -- gia da ve 0 thi khong duoc ban mon voi gia 0 - tra mon ve nguoi ban moi dung.
    if r.buyer == "" or (r.cur or 0) <= 0 then
        return AUC_Expire(r)
    end
    if AUC_SetState(r.id, 1, 1) ~= 1 then
        return
    end
    AUC_Settle(r, r.buyer, r.cur)
end

-- ---------------------------------------------------------------- DAT BAN (goi tu NPC / GM)
-- nPrice = gia mua ngay (ky gui) hoac gia khoi diem (phien the gioi/bang); nBaseIn = gia co ban khi ky gui.
function AUC_PutOnItem(nType, szAct, nKind, nCur, nPrice, nItemIdx, nTong, nBaseIn)
    if nItemIdx == nil or nItemIdx <= 0 then
        return 0
    end
    local szRec, szName, szDesc, nCells, nStack, nHetHan = AUC_ItemToRec(nItemIdx)
    if szRec == nil or szRec == "" then
        Msg2Player("Mãn nµy kh«ng thÓ ký göi (quÆng, nguyªn liÖu th«...).")
        return 0
    end
    -- [B2] mon co HAN DUNG: han la MOC thoi gian tuyet doi nen mon se chet ngay trong kho dau gia
    -- hoac trong hop thu, va ban ghi ky gui khong luu moc do. Cam ky gui.
    -- [A15 04/09] nHetHan do AUC_ItemToRec tra ve (= KItem::GetExpireTime, 0 = khong co han).
    -- Truoc day dung GetItemLife la SAI: ham do nhan MA SU KIEN, tra -1 khi khong thay,
    -- nen chan sach moi lan ky gui (chu bao 04/09: "bo do vao thi bao co han su dung").
    if (nHetHan or 0) > 0 then
        Msg2Player("VËt phÈm cã h¹n sö dông kh«ng thÓ ký göi.")
        return 0
    end
    if GetItemBindState and GetItemBindState(nItemIdx) ~= 0 then
        Msg2Player("VËt phÈm kho¸ kh«ng thÓ ký göi.")
        return 0
    end
    local nNow = GetCurrentTime()
    -- [A20 04/09 chu chot] KY GUI CA NHAN = dau gia TANG DAN co MUA NGAY:
    --   guar = gia CO BAN (khoi diem)   cur = gia cao nhat dang co, 0 = chua ai tra
    --   base = gia MUA NGAY             buoc moi luot = 10% gia co ban (AUC_RowToClient)
    -- Het 24 gio: ai giu gia cao nhat thi duoc mon; khong ai tra thi tra mon ve nguoi ban.
    local nGuar = nBaseIn or nPrice
    local nCurP = 0
    local nBase = nPrice
    local nEnd = nNow + AUCTION_DEF.nPersonalDuration
    local nNextDrop = 0
    local nDropLeft = 0
    if nType == AUCTION_DEF.tbAuctionTypeEnum.eType_PERSONAL then
        nKind = AUCTION_DEF.tbItemTypeEnum.eType_ENGLISH
    end
    if nType ~= AUCTION_DEF.tbAuctionTypeEnum.eType_PERSONAL then
        nGuar = nPrice
        if nKind == AUCTION_DEF.tbItemTypeEnum.eType_ENGLISH then
            -- [A17] gia khoi diem = gia san, chua ai tra thi cur = 0 (khong phai 150%),
            -- khong thi nguoi tra gia DAU TIEN luon bi tu choi vi cau SQL doi cur_price < gia tra.
            nCurP = 0
            nBase = nGuar
            nNextDrop = 0
            nDropLeft = 0
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
        -- [A20] coc tinh tren GIA CO BAN, kep boi tran de mot lan go nham khong tru sach vi
        nDeposit = floor(nGuar * AUCTION_DEF.nPersonalPutOnCost / 100)
        local nTran = AUCTION_DEF.nMaxDepositMoney
        if nCur == AUCTION_DEF.tbCurrency.XU then
            nTran = AUCTION_DEF.nMaxDepositXu
        end
        if nDeposit > nTran then
            nDeposit = nTran
        end
        if AUC_PayMoney(nCur, nDeposit) ~= 1 then
            Msg2Player("Kh«ng ®ñ "..AUC_CurName(nCur).." ®Ó tr¶ phÝ ký göi "..AUC_SoTien(nDeposit)..".")
            return 0
        end
    end
    -- [B2] XOA MON TRUOC roi moi ghi kho: truoc day ghi kho xong moi xoa va bo qua ket qua,
    -- xoa hong la mon vua nam trong tui vua nam trong kho dau gia = nhan doi.
    -- Dung so luong THO tu AUC_ItemToRec (GetItemStackCount bi kep theo tran chong nen xoa thieu).
    if RemoveItemByIndex(nItemIdx, nStack or 1) ~= 1 then
        Msg2Player("Kh«ng lÊy ®­îc vËt phÈm khái hµnh trang, h·y thö l¹i.")
        if nDeposit > 0 then
            if nCur == AUCTION_DEF.tbCurrency.XU then
                SetTask(AUCTION_DEF.XU_TASK, GetTask(AUCTION_DEF.XU_TASK) + nDeposit)
            else
                Earn(nDeposit)
            end
        end
        return 0
    end
    local nId = AUC_PutOn(nType, szAct or "", nKind, GetName(), nTong or 0, szName, szDesc, szRec, nCells or 1, nCur,
        nBase, nCurP, nGuar, nDeposit, nNow, nEnd, nNextDrop, nDropLeft)
    if nId <= 0 then
        -- ghi kho hong: TRA MON LAI NGAY (AUC_GiveRec dung lai dung mon vua xoa)
        if AUC_GiveRec(szRec) <= 0 then
            AUC_Log(format("MAT DO: %s ky gui %s nhung ghi kho hong VA tra lai hong", GetName(), szName))
        end
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
    AUC_Log(format("DAT BAN id %d loai %d '%s' %s gia %d tien %d nguoi %s", nId, nType, szAct or "", szName, nPrice, nCur, GetName()))
    local szActReal = szAct
    if nType == AUCTION_DEF.tbAuctionTypeEnum.eType_PERSONAL then
        szActReal = AUCTION_DEF.szPersonalActivity
    end
    AUC_NotifyNewItem(nType, szActReal, nId, nTong)
    return nId
end

AUCPOLL_FRAMES = 30 * 18
AUCPOLL_GLB    = 9002
function AucPoll_Tick(nParam, nTimerId)
    SetGlbValue(AUCPOLL_GLB, GetCurrentTime())
    if AUC_Ready() == 1 then
        AUC_Tick()
    end
    return AUCPOLL_FRAMES
end
-- [A29b 04/09] GOC cua "bang hoi het gio chua tra item ve": VONG QUET CHUA TUNG CHAY.
-- Chot chong dang ky hai lan truoc day la GetGlbValue(...) ~= 1, ma GlbValue LUU VINH VIEN:
-- lan dau dat = 1 roi thi qua MOI lan khoi dong may chu deu thay = 1 va khong dang ky lai,
-- nen khong mon nao het han tra ve, phien Ha Lan khong ha gia. Nhat ky: khong mot dong HET HAN.
-- Nay dung NHIP TIM: vong quet ghi gio moi lan chay; nap tep thay nhip tim cu hon hai phut
-- thi dang ky lai. Tu lanh sau moi lan khoi dong, va khong dang ky chong vi sau lan dau
-- nhip tim luon moi (bien AUCPOLL_DANGKY chan luon viec Include lai tep trong cung state).
if AUCPOLL_DANGKY ~= 1 then
    local nNhip = GetGlbValue(AUCPOLL_GLB) or 0
    if GetCurrentTime() - nNhip > 120 then
        AUCPOLL_DANGKY = 1
        SetGlbValue(AUCPOLL_GLB, GetCurrentTime())
        AddTimer(AUCPOLL_FRAMES, "AucPoll_Tick", 0)
        AUC_Log("dang ky vong quet dau gia (nhip tim cu %d giay)")
    end
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
    -- [A21] noi ro luat cho nguoi ban truoc khi ho bam Dong y
    -- [A23 04/09] Chu bao "cho nhap gia bi de chu len": o mo ta cua hop trai tu y 96 den 182,
    -- dung cho hai hang nhap gia. Chu dai ba bon dong la de len chung (loi cua dot A21).
    -- Nay trong hop chi de MOT dong ngan; luat day du noi qua khung chat, doc lai duoc.
    if nType == AUCTION_DEF.tbAuctionTypeEnum.eType_PERSONAL then
    Msg2Player("Ký göi: nhËp gi¸ mua ngay vµ gi¸ c¬ b¶n (c¬ b¶n ph¶i thÊp h¬n gi¸ mua ngay).")
    Msg2Player("Ng­êi mua tr¶ gi¸ lªn tõng l­ît, mçi l­ît thªm 10% gi¸ c¬ b¶n, hoÆc tr¶ th¼ng gi¸ mua ngay.")
    Msg2Player("Cäc "..AUCTION_DEF.nPersonalPutOnCost.."% gi¸ c¬ b¶n, hoµn l¹i khi b¸n ®­îc hoÆc hÕt h¹n, chØ mÊt khi tù rót mãn. ThuÕ "..AUCTION_DEF.nAuctionTaxRate.."% khi b¸n ®­îc.")
    end
    local szNhac = "Cäc "..AUCTION_DEF.nPersonalPutOnCost.."%, thuÕ "..AUCTION_DEF.nAuctionTaxRate.."% khi b¸n."
    GiveItemUI("Ký göi ®Êu gi¸", szNhac, "AUC_OnGiveOk", "AUC_OnGiveCancel", 0, "AUC_OnGiveCheck", 0, AUC_SCRIPT)
end

-- [A6] client bao GIA + LOAI TIEN (o ngay trong hop dua vat pham) truoc khi bam Dong y
-- [A20] nPrice = gia MUA NGAY, nBase = gia CO BAN (khoi diem, phai thap hon gia mua ngay)
function AUC_OnRequestSetPrice(nPrice, nCur, nBase)
    local t = AUC_TMP[PlayerIndex]
    if not t then
        return
    end
    t.nPrice = floor(nPrice or 0)
    t.nBase = floor(nBase or 0)
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
        Msg2Player("Ch­a nhËp gi¸ mua ngay hîp lÖ.")
        return
    end
    -- [A20] ky gui ca nhan doi DU HAI gia va gia co ban phai THAP HON gia mua ngay,
    -- khong thi khong con gi de dau (chu hoi dung cho nay hom 04/09).
    if t.nType == AUCTION_DEF.tbAuctionTypeEnum.eType_PERSONAL then
        if (t.nBase or 0) < 1 then
            Msg2Player("Ch­a nhËp gi¸ c¬ b¶n.")
            return
        end
        if t.nBase >= t.nPrice then
            Msg2Player("Gi¸ c¬ b¶n ph¶i thÊp h¬n gi¸ mua ngay.")
            return
        end
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
    local nId = AUC_PutOnItem(t.nType, szAct, t.nKind, t.nCur, t.nPrice, nIdx, t.nTong, t.nBase)
    if nId > 0 then
        Msg2Player("§· ®­a vµo khu ®Êu gi¸, m· sè "..nId..". TiÒn b¸n vµ vËt phÈm tr¶ vÒ qua hép th­.")
        -- [A28] bao RIENG nguoi vua ky gui: dong hop dua vat pham va nap lai trang cho ho.
        -- Khong bam vao goi NEWITEM (goi do phat cho MOI nguoi dang xem) vi nguoi khac ky gui
        -- cung sinh ra no, se dong nham hop cua nguoi dang dat mon vao.
        local hOk = OB_Create()
        ObjBuffer:PushByType(hOk, OBJTYPE_NUMBER, t.nType)
        ObjBuffer:PushByType(hOk, OBJTYPE_STRING, szAct)
        AUC_SendTo(PlayerIndex, "emSCRIPT_PROTOCOL_AUCTION_PUTONOK", hOk)
        OB_Release(hOk)
    end
end

-- ---------------------------------------------------------------- QUET (moi 30 giay)
-- [B3] Timer PHAI dang ky ngay trong tep nay: Include = lua_dofile vao CHINH state goi, nen dat o
-- tep rieng thi AUC_Tick chay o state khac state giu AUC_Viewers -> moi thong bao tu dong khong toi ai.
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
        AUC_NotifyEndItem(r.atype, szAct, r.id, r.tong)
    end
    -- [A14] 1b) ky gui ca nhan: ha mot nhip THANG ve gia san khi den moc.
    -- De khoi rieng chu khong nhet vao vong duoi, vi vong duoi con lo bao KET THUC PHIEN -
    -- tab Ca nhan la cho ban thuong truc, khong co phien nao de ket thuc.
    local rowsP = AUC_List(AUCTION_DEF.tbAuctionTypeEnum.eType_PERSONAL, 200, 0)
    for i = 1, getn(rowsP) do
        local r = rowsP[i]
        if r.dropleft > 0 and r.nextdrop > 0 and r.nextdrop <= nNow then
            if AUC_SetPrice(r.id, r.guar, 0, 0, r.endtime) == 1 then
                r.cur = r.guar
                r.nextdrop = 0
                r.dropleft = 0
                AUC_NotifyPrice(AUCTION_DEF.tbAuctionTypeEnum.eType_PERSONAL, AUCTION_DEF.szPersonalActivity, r, nNow)
            end
        end
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
