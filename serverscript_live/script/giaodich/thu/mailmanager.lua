-- mailmanager.lua - [MAIL 03/09] he THU phia MAY CHU (thay \script\mail\MailManager.lua cua VLTK 2.0 - khong co o dau).
-- Thu nam trong bang MySQL `mail` (KMailServer.cpp: MailDB_*). Nguon gui: web admin ghi thang bang (state 0),
-- hoat dong / top tuan-thang / dua top goi MailManager_SendMail(...). Client = uimail.lua cua 2.0, giao thuc giu nguyen.
-- Cac ham MailManager_OnRequest* duoc goi qua DynamicExecuteByPlayer (PlayerIndex da dat) tu protocol_def_gs.lua.
-- Dinh kem (cot award): "item:genre,detail,particular,level,series,luck,soluong;money:N;xu:N;exp:N" (nhieu muc cach ;).
Include("\\script\\protocol.lua")
Include("\\script\\mail\\maildef.lua")

MAILMGR_PAGE        = 12              -- so header moi goi (ObjBuffer 4 KB)
MAILMGR_MAX_CONTENT = 2000            -- MAILUI_CONTENT_LEN client = 2048
MAILMGR_ICON_MONEY  = "\\spr\\Ui4\\email\\ÒøÁ½Í¼±ê.spr"
MAILMGR_ICON_XU     = "\\spr\\item\\script\\jinding.spr"
MAILMGR_ICON_EXP    = "\\spr\\item\\exp.spr"
MAILMGR_SENDER_SYS  = "Th­ hÖ thèng"
MAILMGR_SENDER_NPH  = "Nhµ ph¸t hµnh"
MAILMGR_XU_TASK     = 251             -- xu hien o hanh trang = task 251 (petsys\jx1_compat.lua)
MAILMGR_ICON_GOLD   = MAILMGR_ICON_XU   -- [D9] trang bi hoang kim (gold:) - tam dung bieu tuong xu
MAILMGR_ICON_TASK   = MAILMGR_ICON_EXP  -- [D9] diem nhiem vu (task:) / danh vong (repute:)
MAILMGR_TASK_NAME   = {[337] = "§iÓm sù kiÖn"}   -- [D9] ten hien thi cho task:id
function MailManager_TaskName(nTaskId)
    return MAILMGR_TASK_NAME[nTaskId] or ("NhiÖm vô "..nTaskId)
end
MailAutoDel = MailAutoDel or {}      -- [ten nhan vat] = 1: tu xoa thu khong dinh kem sau khi doc

function MailManager_Split(s, sep)
    local tb = {}
    if type(s) ~= "string" then
        return tb
    end
    local nPos = 1
    while 1 do
        local a, b = strfind(s, sep, nPos, 1)
        if not a then
            tinsert(tb, strsub(s, nPos))
            break
        end
        tinsert(tb, strsub(s, nPos, a - 1))
        nPos = b + 1
    end
    return tb
end

function MailManager_Trim(s)
    local _, _, t = strfind(s or "", "^%s*(.-)%s*$")
    return t or ""
end

-- "item:6,1,4139,0,0,0,1;money:1000;xu:10;exp:5000" -> bang cac muc
function MailManager_ParseAward(szAward)
    local tb = {}
    if type(szAward) ~= "string" or szAward == "" then
        return tb
    end
    local parts = MailManager_Split(szAward, ";")
    for i = 1, getn(parts) do
        local p = MailManager_Trim(parts[i])
        local _, _, kind, val = strfind(p, "^(%a+)%s*:%s*(.*)$")
        if kind then
            kind = strlower(kind)
            val = MailManager_Trim(val)
            if kind == "item" then
                -- [D9] item:g,d,p,l,s,k,n[,lock][,expSec][,magic][,stack]: lock -2 = khoa vinh vien (SetPlayerItemLock),
                -- expSec = het han sau N giay (AddTimeItem), magic = tham so 7 AddItem, stack 1 = giao 1 chong n mon (AddItemSL)
                local nums = MailManager_Split(val, ",")
                if getn(nums) >= 6 then
                    tinsert(tb, {szKind = "item", nGenre = tonumber(nums[1]) or 0, nDetail = tonumber(nums[2]) or 0,
                        nParticular = tonumber(nums[3]) or 0, nLevel = tonumber(nums[4]) or 0, nSeries = tonumber(nums[5]) or 0,
                        nLuck = tonumber(nums[6]) or 0, nCount = tonumber(nums[7]) or 1, nLock = tonumber(nums[8]) or 0,
                        nExpSec = tonumber(nums[9]) or 0, nMagic = tonumber(nums[10]) or 0, nStack = tonumber(nums[11]) or 0})
                end
            elseif kind == "gold" then
                -- [D9] gold:record,n[,lock][,expSec] = trang bi hoang kim theo dong goldequip.txt (AddItem2 NATURE_GOLD, nhu Da Tau)
                local nums = MailManager_Split(val, ",")
                if getn(nums) >= 1 then
                    tinsert(tb, {szKind = "gold", nRecord = tonumber(nums[1]) or 0, nCount = tonumber(nums[2]) or 1,
                        nLock = tonumber(nums[3]) or 0, nExpSec = tonumber(nums[4]) or 0})
                end
            elseif kind == "aucitem" then
                -- [DAUGIA 04/09] aucitem:<id> = mon trong bang auction_item (giu nguyen thuoc tinh, AUC_GiveRec)
                local nAid = tonumber(val) or 0
                if nAid > 0 then
                    local ai = AUC_Get(nAid)
                    -- [A16 04/09] Doc bo so tu cot item_rec (AUC_RecDesc) chu khong phai item_desc:
                    -- desc chi co 6 so nen hoang kim ve ra mon khac han va so chong luon hien 1.
                    if not ai or ai.rec == nil or ai.rec == "" then
                        -- Dong dau gia khong con: KHONG duoc day muc rong (6 so 0 se dung ra mot mon
                        -- rac tu khe Item[] dung lai). Hien mot bieu tuong chung chung cho an toan.
                        tinsert(tb, {szKind = "aucitem", nAucId = nAid, szInfo = "", nCount = 1, nCells = 6,
                            nGenre = 0, nDetail = 0, nParticular = 0, nLevel = 0, nSeries = 0, nLuck = 0, bLoi = 1})
                    else
                        local szInfo, nStk = AUC_RecDesc(ai.rec)
                        local pr = {0, 0, 0, 0, 0, 0}
                        local kx = 0
                        local sd = szInfo
                        while kx < 6 do
                            local a1, b1, num = strfind(sd, "^(%-?%d+),?")
                            if not a1 then break end
                            kx = kx + 1
                            pr[kx] = tonumber(num) or 0
                            sd = strsub(sd, b1 + 1)
                        end
                        tinsert(tb, {szKind = "aucitem", nAucId = nAid, nGenre = pr[1], nDetail = pr[2], nParticular = pr[3],
                            nLevel = pr[4], nSeries = pr[5], nLuck = pr[6], nCount = nStk or 1,
                            szInfo = szInfo or "", nCells = ai.cells or 6})
                    end
                end
            elseif kind == "task" then
                -- [D9] task:id,n = cong n vao o nhiem vu id (337 = diem su kien Tong Kim)
                local nums = MailManager_Split(val, ",")
                if getn(nums) >= 2 and (tonumber(nums[2]) or 0) ~= 0 then
                    tinsert(tb, {szKind = "task", nTaskId = tonumber(nums[1]) or 0, nCount = tonumber(nums[2]) or 0})
                end
            elseif kind == "money" or kind == "xu" or kind == "exp" or kind == "repute" then
                local n = tonumber(val) or 0
                if n > 0 then
                    tinsert(tb, {szKind = kind, nCount = n})
                end
            end
        end
    end
    return tb
end

-- mo ta cho client (uimail.lua PackAwardInfo): item giu nguyen thuoc tinh, tien/xu/exp thanh bieu tuong
function MailManager_AwardInfo(tbAward)
    local tb = {}
    for i = 1, getn(tbAward) do
        local a = tbAward[i]
        if a.szKind == "item" or a.szKind == "aucitem" then
            tinsert(tb, a)
        elseif a.szKind == "money" then
            tinsert(tb, {szKind = "icon", szIcon = MAILMGR_ICON_MONEY, szName = "Ng©n l­îng", szDesc = a.nCount.." Ng©n l­îng", nCount = a.nCount})
        elseif a.szKind == "xu" then
            tinsert(tb, {szKind = "icon", szIcon = MAILMGR_ICON_XU, szName = "Xu", szDesc = a.nCount.." xu", nCount = a.nCount})
        elseif a.szKind == "exp" then
            tinsert(tb, {szKind = "icon", szIcon = MAILMGR_ICON_EXP, szName = "Kinh nghiÖm", szDesc = a.nCount.." kinh nghiÖm", nCount = a.nCount})
        elseif a.szKind == "gold" then
            tinsert(tb, {szKind = "icon", szIcon = MAILMGR_ICON_GOLD, szName = "Trang bÞ Hoµng Kim", szDesc = "Dßng "..a.nRecord.." x"..a.nCount, nCount = a.nCount})
        elseif a.szKind == "task" then
            tinsert(tb, {szKind = "icon", szIcon = MAILMGR_ICON_TASK, szName = MailManager_TaskName(a.nTaskId), szDesc = a.nCount.." "..MailManager_TaskName(a.nTaskId), nCount = a.nCount})
        elseif a.szKind == "repute" then
            tinsert(tb, {szKind = "icon", szIcon = MAILMGR_ICON_TASK, szName = "Danh väng", szDesc = a.nCount.." danh väng", nCount = a.nCount})
        end
    end
    return tb
end

function MailManager_SendTo(nPlayerIdx, szEnum, h)
    local nOk = SendScriptDataToPlayer(nPlayerIdx, ScriptProtocol[szEnum], h)
    -- [D4] chan doan: logs/hethong.log [MAIL] (SendScriptDataToPlayer tra 0 khi PlayerIndex/ket noi/goi hong)
    GhiLog("MAIL", format("gui %s -> player %d: %s", szEnum, nPlayerIdx, (nOk == 1) and "ok" or "THAT BAI"))
    return nOk
end

-- trang header: tbList[nId] = {szSender, szTitle, nState, nExpiredTime, nSendTime, nAwardCount}; nComplete = 1 neu het
function MailManager_Headers(szRole, nMinId)
    local tb = MailDB_Headers(szRole, nMinId or 0, MAILMGR_PAGE + 1)
    local n = getn(tb)
    local nComplete = 1
    if n > MAILMGR_PAGE then
        nComplete = 0
        n = MAILMGR_PAGE
    end
    local tbList = {}
    for i = 1, n do
        local r = tb[i]
        tbList[r.id] = {szSender = r.sender, szTitle = r.title, nState = r.state, nExpiredTime = r.expire,
            nSendTime = r.send, nAwardCount = r.award_count}
    end
    return tbList, nComplete
end

function MailManager_PushHeaders(nPlayerIdx, szRole, nMinId)
    if nPlayerIdx <= 0 then
        return
    end
    local tbList, nComplete = MailManager_Headers(szRole, nMinId)
    local h = OB_Create()
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nMinId or 0)
    ObjBuffer:PushByType(h, OBJTYPE_TABLE, tbList)
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nComplete)
    MailManager_SendTo(nPlayerIdx, "emSCRIPT_PROTOCOL_MAIL_HEADERLIST", h)
    OB_Release(h)
    -- [D4] thu chua giao (state 0) trong trang nay -> tra id lon nhat de bao NEWMAIL (client tu mo hop thu)
    local nNewId = 0
    for nId, tb in pairs(tbList) do
        if tb.nState == 0 and nId > nNewId then
            nNewId = nId
        end
    end
    return nNewId
end

function MailManager_ReplyState(nId, nToState, nOk)
    local h = OB_Create()
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nId)
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nToState)
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nOk)
    SendScriptData(ScriptProtocol["emSCRIPT_PROTOCOL_MAIL_STATECHANGE"], h)
    OB_Release(h)
end

function MailManager_ReplyDelete(nId, nReason)
    local h = OB_Create()
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nId)
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nReason)
    SendScriptData(ScriptProtocol["emSCRIPT_PROTOCOL_MAIL_DELETE"], h)
    OB_Release(h)
end

-- ======== handler (PlayerIndex da dat) ========
function MailManager_OnRequestHeaderList(nMinId)
    MailManager_PushHeaders(PlayerIndex, GetName(), nMinId or 0)
end

function MailManager_OnRequestWholeMail(nId)
    local szRole = GetName()
    local r = MailDB_Get(szRole, nId)
    if not r then
        Msg2Player("Th­ kh«ng tån t¹i!")
        return
    end
    local tbAward = MailManager_ParseAward(r.award)
    local tbMail = {
        nPrivateId = r.id, szRoleName = szRole, szSender = r.sender, szTitle = r.title,
        szDescribe = strsub(r.content or "", 1, MAILMGR_MAX_CONTENT),
        nSendTime = r.send, nCacheTime = r.send, nRecvTime = r.send, nExpiredTime = r.expire,
        nState = r.state, tbAward = {}, nAwardCount = r.award_count,
        tbAwardInfo = MailManager_AwardInfo(tbAward),
    }
    local h = OB_Create()
    ObjBuffer:PushByType(h, OBJTYPE_TABLE, tbMail)
    SendScriptData(ScriptProtocol["emSCRIPT_PROTOCOL_MAIL_WHOLEMAIL"], h)
    OB_Release(h)
end

-- [D9] khoa / han dung sau khi tao vat pham (giong tasklink_award.lua tl_linkaward_give)
function MailManager_ItemPost(nIdx, nLock, nExpSec)
    if nLock and nLock ~= 0 then
        SetPlayerItemLock(nIdx, nLock)
    end
    if nExpSec and nExpSec > 0 then
        AddTimeItem(nIdx, nExpSec)
    end
end

-- [D9] trao 1 muc item/gold; tra so mon da tao
function MailManager_GiveItem(a)
    local nOk = 0
    if a.szKind == "gold" then
        for c = 1, a.nCount do
            local nIdx = AddItem2(2, 0, a.nRecord, 0, 0, 0)
            if nIdx and nIdx > 0 then
                MailManager_ItemPost(nIdx, a.nLock, a.nExpSec)
                nOk = nOk + 1
            else
                GhiLog("MAIL", format("AddItem2 hoang kim dong %d that bai cho %s", a.nRecord, GetName()))
            end
        end
        return nOk
    end
    if a.nStack == 1 and a.nGenre ~= 0 then
        -- 1 chong n mon: AddItem 21 tham so nhu lib_ham.lua AddItemSL (13 = so luong, 20 = khoa)
        local nIdx = AddItem(a.nGenre, a.nDetail, a.nParticular, a.nLevel, a.nSeries, a.nLuck, a.nMagic, 0, 0, 0, 0, 0, a.nCount, 0, 0, 0, 0, 0, 0, a.nLock, 0)
        if nIdx and nIdx > 0 then
            if a.nExpSec > 0 then
                AddTimeItem(nIdx, a.nExpSec)
            end
            return a.nCount
        end
        GhiLog("MAIL", format("AddItem chong %d,%d,%d x%d that bai cho %s", a.nGenre, a.nDetail, a.nParticular, a.nCount, GetName()))
        return 0
    end
    for c = 1, a.nCount do
        local nIdx = AddItem(a.nGenre, a.nDetail, a.nParticular, a.nLevel, a.nSeries, a.nLuck, a.nMagic)
        if nIdx and nIdx > 0 then
            MailManager_ItemPost(nIdx, a.nLock, a.nExpSec)
            nOk = nOk + 1
        else
            GhiLog("MAIL", format("AddItem that bai %d,%d,%d cho %s", a.nGenre, a.nDetail, a.nParticular, GetName()))
        end
    end
    return nOk
end

-- [CL 04/09 V4] Dung lai chuoi cot `award` cho phan CON THIEU de gui lai qua thu.
-- Giu dung ngu phap ma MailManager_ParseAward doc duoc.
function MailManager_QuaConLai(a, nThieu)
    if nThieu == nil or nThieu <= 0 then
        return ""
    end
    if a.szKind == "gold" then
        return format("gold:%d,%d,%d,%d", a.nRecord or 0, nThieu, a.nLock or 0, a.nExpSec or 0)
    end
    if a.szKind == "item" then
        return format("item:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
            a.nGenre or 0, a.nDetail or 0, a.nParticular or 0, a.nLevel or 0,
            a.nSeries or 0, a.nLuck or 0, nThieu, a.nLock or 0, a.nExpSec or 0,
            a.nMagic or 0, a.nStack or 0)
    end
    return ""
end

function MailManager_GiveAward(tbAward)
    for i = 1, getn(tbAward) do
        local a = tbAward[i]
        if a.szKind == "aucitem" then
            local szRec = a.szRec
            if szRec == nil or szRec == "" then
                local ai = AUC_Get(a.nAucId)
                if ai then
                    szRec = ai.rec
                end
            end
            if szRec ~= nil and szRec ~= "" and AUC_GiveRec(szRec) > 0 then
                GhiLog("MAIL", format("%s nhan mon dau gia id %d", GetName(), a.nAucId))
            else
                -- [B4] trao hong: thu da danh dau da nhan roi nen KHONG duoc bo qua - gui lai bang thu moi.
                GhiLog("MAIL", format("LOI: %s KHONG nhan duoc mon dau gia id %d - gui lai qua thu", GetName(), a.nAucId))
                MailManager_SendMail(GetName(), MAILMGR_SENDER_SYS, "VËt phÈm ®Êu gi¸ göi l¹i",
                    "LÇn nhËn tr­íc hµnh trang kh«ng ®ñ chç, vËt phÈm ®­îc göi l¹i ®©y.",
                    "aucitem:"..a.nAucId, 30, "daugia", 1)
            end
        elseif a.szKind == "item" or a.szKind == "gold" then
            -- [CL 04/09 V4] TRUOC DAY vut bo gia tri tra ve: AddItem hong thi chi
            -- GhiLog roi thoi, ma thu DA danh dau da nhan -> mat do vinh vien.
            -- Nay lam giong nhanh aucitem: thieu bao nhieu thi gui lai bang thu moi.
            local nOk = MailManager_GiveItem(a) or 0
            local nThieu = (a.nCount or 1) - nOk
            if a.nStack == 1 and a.nGenre ~= 0 and nOk > 0 then
                nThieu = 0   -- mot chong n mon: tra ve nCount khi dat duoc
            end
            if nThieu > 0 then
                local szLai = MailManager_QuaConLai(a, nThieu)
                GhiLog("MAIL", format("LOI: %s KHONG nhan du (%d/%d) - gui lai: %s",
                    GetName(), nOk, a.nCount or 1, szLai))
                if szLai ~= "" then
                    MailManager_SendMail(GetName(), MAILMGR_SENDER_SYS, "VËt phÈm göi l¹i",
                        "LÇn nhËn tr­íc hµnh trang kh«ng ®ñ chç trèng liÒn, phÇn cßn thiÕu ®­îc göi l¹i ®©y.",
                        szLai, 30, "mail", nThieu)
                end
            end
        elseif a.szKind == "money" then
            Earn(a.nCount)
        elseif a.szKind == "xu" then
            SetTask(MAILMGR_XU_TASK, GetTask(MAILMGR_XU_TASK) + a.nCount)
        elseif a.szKind == "exp" then
            AddOwnExp(a.nCount)
        elseif a.szKind == "repute" then
            AddRepute(a.nCount)
        elseif a.szKind == "task" then
            SetTask(a.nTaskId, GetTask(a.nTaskId) + a.nCount)
        end
    end
end

function MailManager_OnRequestStateChange(nId, nToState)
    local szRole = GetName()
    if nToState == MAILDEF.tbState.READED then
        if MailDB_SetState(szRole, nId, MAILDEF.tbState.READED, MAILDEF.tbState.READED) == 1 then
            MailManager_ReplyState(nId, MAILDEF.tbState.READED, 1)
            if MailAutoDel[szRole] == 1 then
                local r = MailDB_Get(szRole, nId)
                if r and r.award_count == 0 and MailDB_Delete(szRole, nId) == 1 then
                    MailManager_ReplyDelete(nId, MAILDEF.tbDeleteReson.REQUEST)
                end
            end
        end
        return
    end
    if nToState ~= MAILDEF.tbState.DRAWED then
        return
    end
    local r = MailDB_Get(szRole, nId)
    if not r then
        Msg2Player("Th­ kh«ng tån t¹i!")
        return
    end
    local tbAward = MailManager_ParseAward(r.award)
    if getn(tbAward) == 0 then
        Msg2Player("Th­ nµy kh«ng cã ®Ýnh kÌm!")
        return
    end
    -- [D9] uoc luong O hanh trang: trang bi (genre 0) va hoang kim 6 o/mon; do khac 1 o/mon, chong = 1 o
    local nCells = 0
    local nBigCells = 0   -- [CL 04/09 V4] so mon doi mot khoi lien 2x3
    for i = 1, getn(tbAward) do
        local a = tbAward[i]
        if a.szKind == "item" then
            if a.nGenre == 0 then
                nCells = nCells + a.nCount * 6
                nBigCells = nBigCells + a.nCount
            elseif a.nStack == 1 then
                nCells = nCells + 1
            else
                nCells = nCells + a.nCount
            end
        elseif a.szKind == "aucitem" then
            nCells = nCells + (a.nCells or 6)
        elseif a.szKind == "gold" then
            nCells = nCells + a.nCount * 6
            nBigCells = nBigCells + a.nCount
        end
    end
    -- [B4 04/09] CalcFreeItemCellCount dem O ROI RAC 1x1, con dat do doi mot KHOI LIEN WxH:
    -- tui con 6 o roi van khong dat noi mot thanh vu khi 2x3. Voi mon dau gia phai THU DAT THAT
    -- truoc khi danh dau da nhan, khong thi thu mat dinh kem ma trong tui khong co gi.
    for i = 1, getn(tbAward) do
        local a = tbAward[i]
        if a.szKind == "aucitem" then
            local ai = AUC_Get(a.nAucId)
            if not ai or ai.rec == nil or ai.rec == "" then
                Msg2Player("VËt phÈm ®Êu gi¸ kh«ng cßn trong kho, h·y b¸o qu¶n trÞ!")
                return
            end
            a.szRec = ai.rec
            if AUC_CanGiveRec and AUC_CanGiveRec(ai.rec) ~= 1 then
                Msg2Player("Hµnh trang kh«ng cßn kho¶ng trèng liÒn ®ñ réng cho vËt phÈm nµy, h·y dän bít!")
                return
            end
        end
    end
    -- [CL 04/09 V4] Phep kiem cu chi dem O ROI RAC 1x1 nen tui con 6 o roi van
    -- "du cho" cho mot bo giap 2x3 ma dat that thi hong. Nay kiem HAI dieu:
    --   (a) du SO O tong cong (nhu cu), va
    --   (b) du SO KHOI LIEN 2x3 cho tung mon trang bi / hoang kim.
    -- LUU Y ham C++ co guard `Lua_GetTopIndex(L) > 2` (ScriptFuns.cpp:6042) nen goi
    -- HAI tham so la W/H bi BO QUA IM LANG - bat buoc truyen DU BA tham so.
    if nBigCells > 0 and CalcFreeItemCellCount(2, 3, 0) < nBigCells then
        Msg2Player("Hµnh trang kh«ng cßn kho¶ng trèng liÒn ®ñ réng, h·y dän bít råi nhËn l¹i!")
        return
    end
    if nCells > 0 and CalcFreeItemCellCount(1, 1, 0) < nCells then
        Msg2Player("Hµnh trang kh«ng ®ñ chç trèng (cÇn "..nCells.." «), h·y dän bít råi nhËn l¹i!")
        return
    end
    -- nguyen tu: chi mot lan doi duoc state < 3 -> 3
    if MailDB_SetState(szRole, nId, MAILDEF.tbState.DRAWED, MAILDEF.tbState.DRAWED) ~= 1 then
        Msg2Player("§Ýnh kÌm ®· ®­îc nhËn råi!")
        return
    end
    MailManager_GiveAward(tbAward)
    MailManager_ReplyState(nId, MAILDEF.tbState.DRAWED, 1)
    Msg2Player("§· nhËn ®Ýnh kÌm trong th­.")
    GhiLog("MAIL", format("%s nhan dinh kem thu %d: %s", szRole, nId, r.award or ""))
end

-- [A29b 04/09] Vong quet hop thu TRUOC NAY KHONG CHAY: mailpoll.lua khong duoc tep nao nap,
-- va chot dang ky cua no dung GlbValue (luu vinh vien) nen dang ky mot lan roi thoi.
-- Nay dat ngay trong tep nay (playerlogin.lua da Include no) theo kieu NHIP TIM.
MAILPOLL_FRAMES = 30 * 18
MAILPOLL_GLB    = 9001
MAILPOLL_LASTID = MAILPOLL_LASTID or 0
MAILPOLL_TICKS  = MAILPOLL_TICKS or 0
function MailPoll_Tick(nParam, nTimerId)
    SetGlbValue(MAILPOLL_GLB, GetCurrentTime())
    if MAILPOLL_LASTID == 0 then
        MAILPOLL_LASTID = MailDB_MaxId() or 0
    end
    local tb = MailDB_PollNew(MAILPOLL_LASTID, 50)
    for i = 1, getn(tb) do
        local r = tb[i]
        if r.id > MAILPOLL_LASTID then
            MAILPOLL_LASTID = r.id
        end
        MailManager_NotifyNew(r.role, r.id)
    end
    MAILPOLL_TICKS = MAILPOLL_TICKS + 1
    if MAILPOLL_TICKS >= 20 then
        MAILPOLL_TICKS = 0
        MailDB_Sweep()
    end
    return MAILPOLL_FRAMES
end
if MAILPOLL_DANGKY ~= 1 then
    local nNhip = GetGlbValue(MAILPOLL_GLB) or 0
    if GetCurrentTime() - nNhip > 120 then
        MAILPOLL_DANGKY = 1
        SetGlbValue(MAILPOLL_GLB, GetCurrentTime())
        AddTimer(MAILPOLL_FRAMES, "MailPoll_Tick", 0)
    end
end

function MailManager_OnRequestDelete(nId)
    if MailDB_Delete(GetName(), nId) == 1 then
        MailManager_ReplyDelete(nId, MAILDEF.tbDeleteReson.REQUEST)
    end
end

function MailManager_OnRequestAutoDelete(nHandle)
    local szRole = GetName()
    if MailAutoDel[szRole] == 1 then
        MailAutoDel[szRole] = 0
    else
        MailAutoDel[szRole] = 1
    end
end

function MailManager_OnRequestOpenUrl(szUrl)
    -- chu chot: bo tinh nang mo URL
end

-- ======== goi tu NPC / dang nhap / script khac (PlayerIndex da dat) ========
function MailManager_OpenWindow()
    local szRole = GetName()
    GhiLog("MAIL", format("%s mo hop thu (OPENWINDOW, player %d)", szRole, PlayerIndex))
    MailManager_PushHeaders(PlayerIndex, szRole, 0)
    local h = OB_Create()
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, 1)
    SendScriptData(ScriptProtocol["emSCRIPT_PROTOCOL_MAIL_OPENWINDOW"], h)
    OB_Release(h)
end

function MailManager_OnLogin()
    -- [D7] bot (PB_IsBot) khong co ket noi: bo qua, khoi ghi 'THAT BAI' vao hethong.log
    if IsBot and IsBot(PlayerIndex) == 1 then
        return
    end
    local szRole = GetName()
    local nCount = MailDB_Count(szRole)
    GhiLog("MAIL", format("%s dang nhap: %d thu", szRole, nCount))
    -- [D4] LUON gui trang header dau (co the rong) de client tao bieu tuong thu duoi Bau Cua
    local nNewId = MailManager_PushHeaders(PlayerIndex, szRole, 0)
    if type(nNewId) == "number" and nNewId > 0 then
        -- thu giao luc offline = thu moi: bao NEWMAIL de client tu mo hop thu
        local h = OB_Create()
        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nNewId)
        MailManager_SendTo(PlayerIndex, "emSCRIPT_PROTOCOL_MAIL_NEWMAIL", h)
        OB_Release(h)
    end
end

-- thu moi cho nguoi choi dang online (web admin / SendMail): bao NEWMAIL + gui header thu do
function MailManager_NotifyNew(szRole, nId)
    local nIdx = FindPlayer(szRole)
    if type(nIdx) ~= "number" or nIdx <= 0 then
        return 0
    end
    local h = OB_Create()
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nId)
    MailManager_SendTo(nIdx, "emSCRIPT_PROTOCOL_MAIL_NEWMAIL", h)
    OB_Release(h)
    MailManager_PushHeaders(nIdx, szRole, nId - 1)
    return 1
end

-- API cho hoat dong / top tuan-thang / dua top: MailManager_SendMail(ten, nguoi gui, tieu de, noi dung, dinh kem, so ngay, nguon)
-- noi dung xuong dong bang <enter>; dinh kem theo dinh dang o dau tep; tra id thu (0 = loi)
-- [D11 04/09] Dem so dinh kem KHONG dung ParseAward (ParseAward tao bang long nhau, goi tu day sau
-- nhu ThuongTongKimQuaThu thi TRAN NGAN XEP Lua va lam dut ca ham thuong). Chi dem dau ';'.
function MailManager_CountAward(szAward)
    if type(szAward) ~= "string" or szAward == "" then
        return 0
    end
    local n = 1
    local i = 1
    while 1 do
        local a = strfind(szAward, ";", i, 1)
        if not a then
            break
        end
        n = n + 1
        i = a + 1
    end
    return n
end

-- [D12 04/09] 1 = ten nay la BOT dang trong the gioi (FindPlayer + IsBot, hai loi goi C++, khong tao bang)
function MailManager_IsBotName(szRole)
    if type(szRole) ~= "string" or szRole == "" then
        return 0
    end
    local nIdx = FindPlayer(szRole)
    if type(nIdx) ~= "number" or nIdx <= 0 then
        return 0
    end
    if IsBot and IsBot(nIdx) == 1 then
        return 1
    end
    return 0
end

-- nAwardCount: so muc dinh kem (nguoi goi biet san thi truyen vao cho re; nil = tu dem)
function MailManager_SendMail(szRole, szSender, szTitle, szContent, szAward, nExpireDays, szSource, nAwardCount)
    -- [D12 04/09] chu chot: KHONG gui thu cho bot (moi duong: hoat dong, dau gia, luong bang).
    if MailManager_IsBotName(szRole) == 1 then
        return 0
    end
    local nCount = nAwardCount or MailManager_CountAward(szAward or "")
    local nId = MailDB_Send(szRole, szSender or MAILMGR_SENDER_SYS, szTitle or "", szContent or "", szAward or "",
        nCount, (nExpireDays or 30) * 86400, szSource or "script")
    if nId > 0 then
        -- [D9b] bao thu moi trong STATE RIENG cua mailmanager.lua: goi truc tiep o day thi ngan xep Lua
        -- cua state goi (vd Da Tau: finish_exp > Prise_Chon > ... > tl_linkaward_mail) da sau, ObjBuffer
        -- PushTable long nhau lam TRAN (ScriptError "stack Overflow" 03/09 23:12 - mat luon phan thuong con lai).
        DynamicExecuteByPlayer(PlayerIndex, "\\script\\mail\\mailmanager.lua", "MailManager_NotifyNew", szRole, nId)
    else
        GhiLog("MAIL", format("MailDB_Send that bai cho %s: %s", szRole or "?", szTitle or ""))
    end
    return nId
end

-- lenh bai admin: gui thu thu cho chinh minh (1 = tien/xu/exp, 2 = them vat pham 6,1,4139)
-- [D8] nguoi gui theo HOAT DONG (chu 03/09: 'moi loai hoat dong ten khac nhau'); them hoat dong = them 1 dong
MAILMGR_ACTIVITY = {
    tongkim   = "Tèng Kim",
    congthanh = "C«ng Thµnh ChiÕn",
    liendau   = "Liªn §Êu",
    datau     = "D· TÈu",
    boss      = "Boss Hoµng Kim",
    viemde    = "Viªm §Õ",
    hoatdong  = "Ho¹t §éng",
    toptuan   = "Th­ëng Top TuÇn",
    topthang  = "Th­ëng Top Th¸ng",
    duatop    = "Sù KiÖn §ua Top",
    phonglangdo = "Phong L¨ng §é",
    vuotai    = "V­ît ¶i",
    tinsu     = "TÝn Sø",
    bangluong = "Bang héi",
    daugia    = "Ch­ëng QuÇy Khu §Êu Gi¸",
    web       = "Nhµ ph¸t hµnh",
}

-- MailManager_SendReward(szActivity, szRole, szTitle, szContent, szAward, nDays) -> id thu (0 = loi)
--   szActivity: khoa MAILMGR_ACTIVITY (tongkim, congthanh, liendau, datau, boss, viemde, hoatdong, toptuan, topthang,
--               duatop, web) hoac ten nguoi gui tu do; cung ghi vao cot `source` de web/log loc theo hoat dong.
--   szRole nil/"" = nguoi choi dang goi script (PlayerIndex); szAward theo dinh dang dau tep; nDays mac dinh 30.
-- Vi du (trong script hoat dong, sau Include("\\script\\mail\\mailmanager.lua")):
--   MailManager_SendReward("tongkim", nil, "Thuong Tong Kim", "Chuc mung...<enter>Tran trong", "money:5000;exp:100000")
function MailManager_SendReward(szActivity, szRole, szTitle, szContent, szAward, nDays, nAwardCount)
    local szSender = MAILMGR_ACTIVITY[szActivity or ""] or szActivity or MAILMGR_SENDER_SYS
    if not szRole or szRole == "" then
        -- [D9b] bot khong can thu (khoi tao hang nghin dong rac trong bang mail)
        if IsBot and IsBot(PlayerIndex) == 1 then
            return 0
        end
        szRole = GetName()
    end
    return MailManager_SendMail(szRole, szSender, szTitle or szSender, szContent or "", szAward or "", nDays or 30, szActivity or "script", nAwardCount)
end

-- [D9] Dung chuoi award tu bang kieu templet (awardtemplet/item_jx1): moi phan tu la mot trong:
--   {tbProp={g,d,p,l,s,k}, nCount, nBindState(-2 = khoa), nExpSec, nMagic, nStack(1 = 1 chong)}
--   {szKind="gold", nRecord, nCount, nBindState, nExpSec}   {szKind="task", nTaskId, nCount}
--   {szKind="money"|"xu"|"exp"|"repute", nCount}
function MailManager_BuildAward(tbList)
    local tb = {}
    for i = 1, getn(tbList) do
        local a = tbList[i]
        if type(a) == "table" then
            if a.tbProp then
                local p = a.tbProp
                tinsert(tb, format("item:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", p[1] or 0, p[2] or 0, p[3] or 0, p[4] or 0, p[5] or 0, p[6] or 0,
                    floor(a.nCount or 1), a.nBindState or 0, floor(a.nExpSec or 0), a.nMagic or 0, a.nStack or 0))
            elseif a.szKind == "gold" then
                tinsert(tb, format("gold:%d,%d,%d,%d", a.nRecord or 0, floor(a.nCount or 1), a.nBindState or 0, floor(a.nExpSec or 0)))
            elseif a.szKind == "task" then
                tinsert(tb, format("task:%d,%d", a.nTaskId or 0, floor(a.nCount or 0)))
            elseif a.szKind and a.nCount and floor(a.nCount) > 0 then
                tinsert(tb, format("%s:%d", a.szKind, floor(a.nCount)))
            end
        end
    end
    local sz = ""
    for i = 1, getn(tb) do
        if i > 1 then
            sz = sz..";"
        end
        sz = sz..tb[i]
    end
    return sz, getn(tb)
end

-- [D9] Gui thu thuong hoat dong tu bang kieu templet. szRole nil = nguoi choi dang goi. Tra id thu (0 = loi/rong).
function MailManager_SendRewardTemplet(szActivity, szRole, szTitle, szContent, tbList, nDays)
    -- [D12 04/09] bot: thoat NGAY, khoi dung chuoi dinh kem (Tong Kim co hang tram bot moi tran)
    if not szRole or szRole == "" then
        if IsBot and IsBot(PlayerIndex) == 1 then
            return 0
        end
    elseif MailManager_IsBotName(szRole) == 1 then
        return 0
    end
    local szAward, n = MailManager_BuildAward(tbList or {})
    if n <= 0 then
        return 0
    end
    return MailManager_SendReward(szActivity, szRole, szTitle, szContent, szAward, nDays, n)
end

function MailManager_SendTest(nKind)
    local szAward = "money:10000;xu:10;exp:50000"
    if nKind == 2 then
        szAward = "item:6,1,4139,0,0,0,1;"..szAward
    end
    local szContent = "§¹i hiÖp th©n mÕn,<enter>"
        .."§©y lµ th­ thö cña hÖ thèng th­ míi. H·y bÊm NhËn ®Ó lÊy ®Ýnh kÌm.<enter>"
        .."Tr©n träng"
    local nId = MailManager_SendMail(GetName(), MAILMGR_SENDER_NPH, "Th­ thö hÖ thèng th­", szContent, szAward, 30, "gm")
    Msg2Player("§· göi th­ thö (id "..nId.."). Hép th­ sÏ tù më; hoÆc bÊm biÓu t­îng th­ d­íi BÇu Cua.")
end
