# -*- coding: utf-8 -*-
"""[DAUGIA 04/09 A2+A3+B+C] Sinh/va script + ini cho DAU GIA (chu chot: tien xu + luong; A ky gui ca nhan,
B phien the gioi, C phien bang hoi). Cay chay that E:\\...\\bin\\{server,client} + guong worktree.
Idempotent (dau [DAUGIA 04/09]). Doc/ghi latin-1, chu Viet = V() TCVN3. Chay: python p12_daugia.py [--check]

Sinh:  server\\script\\auction_house\\auction_def.lua      (dung chung, cung chep sang client)
       server\\script\\auction_house\\auction_manager.lua  (bo may may chu)
       server\\script\\auction_house\\auction_npc.lua      (menu NPC: ky gui, phien bang hoi, phien the gioi cho GM)
       server\\script\\auction_house\\auctionpoll.lua      (timer 30 s: giam gia Ha Lan, ket thuc kieu Anh, het han ky gui)
       client\\script\\ui\\uiauction_house.lua             (viet lai tu client JX1 cu cho JX1)
       client\\ui\\Ui3\\auction\\*.ini                      (13 ini rut tu client JX1 cu, Image= them dau \\ dau)
Va:    protocol.lua (2 phia, +20 ten), protocol_def_gs.lua (+8), protocol_def_c.lua (+12), dichquan.lua (+menu),
       lenhbaiadmin.lua (+menu GM), p3_lua.py (thu: dinh kem aucitem:<id>) roi sinh lai mailmanager.lua.
"""
import io, os, sys, shutil, re

SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
MIRROR_S = r"D:\GAMEDEVNEW_wt_mail\serverscript_jx2\mail\server"
MIRROR_C = r"D:\GAMEDEVNEW_wt_mail\serverscript_jx2\mail\client"
INI_SRC = r"C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\c5fa9bc2-c353-4fb8-8907-29124052d92a\scratchpad\vltk2_dg\auction_jx1cu"
INI_KEEP = r"D:\GAMEDEVNEW_wt_mail\ReverseTools\pak_vltk\vltk2\auction_ini"
CHECK = "--check" in sys.argv
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes

MARK = "[DAUGIA 04/09]"
BS = chr(92)
E = "\r\n"


def V(s):
    return unicode_to_tcvn3_bytes(s).decode("latin-1")


def lp(*parts):
    """duong dan Lua dang chuoi trong file Lua: \\script\\x\\y.lua (2 gach trong nguon Lua)"""
    return BS * 2 + (BS * 2).join(parts)


def rd(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def wr_both(rel_from_root, s, root, mirror):
    for base in (root, mirror):
        p = os.path.join(base, rel_from_root)
        if CHECK:
            print("  (check) ghi", p, len(s))
            continue
        d = os.path.dirname(p)
        if not os.path.isdir(d):
            os.makedirs(d)
        io.open(p, "w", encoding="latin-1", newline="").write(s)
        print("  ghi:", p, len(s))


def wr_s(rel, s):
    wr_both(rel, s, SV, MIRROR_S)


def wr_c(rel, s):
    wr_both(rel, s, CL, MIRROR_C)


def rep1(s, old, new, label):
    n = s.count(old)
    if n != 1:
        raise AssertionError("%s: neo khop %d lan: %s" % (label, n, ascii(old[:80])))
    return s.replace(old, new)


def eol(s):
    return "\r\n" if "\r\n" in s else "\n"


# ============================================================================ auction_def.lua (dung chung)
def build_def():
    L = [
        "-- auction_def.lua - " + MARK + " tham so + hang so DAU GIA (dung chung may chu / client). Viet lai tu",
        "-- \\script\\auction_house\\auction_def.lua cua client JX1 cu (2.0). Chu chot 04/09: tien XU va NGAN LUONG.",
        "AUCTION_DEF = {}",
        "AUCTION_DEF.nAuctionTaxRate        = 5              -- thue giao dich (%) tru vao tien nguoi ban nhan",
        "AUCTION_DEF.nPersonalPutOnCost     = 10             -- phi ky gui (%) thu ngay khi dat ban, khong hoan",
        "AUCTION_DEF.nMaxItemPerPage        = 20             -- moi trang",
        "AUCTION_DEF.nMaxItemPerSeller      = 5              -- mot nguoi ky gui toi da",
        "AUCTION_DEF.nPersonalDuration      = 24 * 60 * 60   -- ky gui 24 gio",
        "AUCTION_DEF.nEnglishRemainingTime  = 30 * 60        -- kieu Anh: 30 phut",
        "AUCTION_DEF.nRefreshRemingTimeWhenOffer = 60        -- kieu Anh: moi lan tra gia gia han them 60 giay",
        "AUCTION_DEF.nDutchInitRate         = 1.5            -- kieu Ha Lan: gia mo = 150% gia bao dam",
        "AUCTION_DEF.nDutchFloatTimes       = 8              -- giam 8 lan",
        "AUCTION_DEF.nDutchFloatRange       = 0.1            -- moi lan 10%",
        "AUCTION_DEF.nDutchFloatInterval    = 5 * 60         -- cach 5 phut",
        "AUCTION_DEF.nMailDays              = 30             -- thu tra tien / do giu 30 ngay",
        "AUCTION_DEF.XU_TASK                = 251            -- xu = o nhiem vu 251 (nhu hop thu)",
        "AUCTION_DEF.tbAuctionTypeEnum = { eType_TONG = 1, eType_WORLD = 2, eType_PERSONAL = 3, eType_COUNT = 4 }",
        "AUCTION_DEF.tbItemTypeEnum    = { eType_ENGLISH = 1, eType_DUTCH = 2 }",
        "AUCTION_DEF.tbCurrency        = { MONEY = 1, XU = 2 }",
        "AUCTION_DEF.tbCurrencyName    = { [1] = \"" + V("Ngân lượng") + "\", [2] = \"Xu\" }",
        "AUCTION_DEF.szPersonalActivity = \"" + V("Ký gửi") + "\"       -- ten phien duy nhat cua tab Ca nhan",
        "AUCTION_DEF.szSender           = \"" + V("Chưởng Quầy Khu Đấu Giá") + "\"",
        "",
    ]
    return E.join(L)


# ============================================================================ auction_manager.lua (may chu)
def build_manager():
    P = "AUCTION_DEF"
    L = [
        "-- auction_manager.lua - " + MARK + " bo may DAU GIA phia may chu. Kho: bang MySQL auction_item (KAuctionServer.cpp,",
        "-- 14 ham AUC_*). Giao/tra VAT PHAM va TIEN deu qua THU (mailmanager.lua) nen nguoi offline khong mat gi; vat pham giu",
        "-- NGUYEN thuoc tinh (AUC_ItemToRec / AUC_GiveRec). Client: \\script\\ui\\uiauction_house.lua. Giao thuc: 20 ten AUCTION_*.",
        "-- Loai phien: 1 bang hoi (chi thanh vien bang, tien ban vao QUY BANG), 2 the gioi (GM mo, tien ban vao he thong),",
        "-- 3 ca nhan = ky gui (moi nguoi dat ban, 24 gio, gia co dinh, tien ve nguoi ban tru thue).",
        "Include(\"" + lp("script", "protocol.lua") + "\")",
        "Include(\"" + lp("script", "lib", "objbuffer_head.lua") + "\")",
        "Include(\"" + lp("script", "auction_house", "auction_def.lua") + "\")",
        "Include(\"" + lp("script", "mail", "mailmanager.lua") + "\")",
        "",
        "AUC_Viewers = AUC_Viewers or {}      -- [PlayerIndex] = {nType, szAct}  nguoi dang mo tab nao",
        "AUC_LastCount = AUC_LastCount or {}  -- [nType..'|'..szAct] = so mon lan quet truoc (bao ket thuc phien)",
        "",
        "function AUC_Log(sz)",
        "    GhiLog(\"DAUGIA\", sz)",
        "end",
        "",
        "function AUC_SendTo(nPlayerIdx, szEnum, h)",
        "    if nPlayerIdx == nil or nPlayerIdx <= 0 then",
        "        return 0",
        "    end",
        "    return SendScriptDataToPlayer(nPlayerIdx, ScriptProtocol[szEnum], h)",
        "end",
        "",
        "-- ---------------------------------------------------------------- tien",
        "function AUC_GetMoney(nCur)",
        "    if nCur == " + P + ".tbCurrency.XU then",
        "        return GetTask(" + P + ".XU_TASK)",
        "    end",
        "    return GetCash()",
        "end",
        "",
        "function AUC_PayMoney(nCur, n)",
        "    n = floor(n)",
        "    if n <= 0 then",
        "        return 1",
        "    end",
        "    if nCur == " + P + ".tbCurrency.XU then",
        "        local x = GetTask(" + P + ".XU_TASK)",
        "        if x < n then",
        "            return 0",
        "        end",
        "        SetTask(" + P + ".XU_TASK, x - n)",
        "        return 1",
        "    end",
        "    if GetCash() < n then",
        "        return 0",
        "    end",
        "    return Pay(n)",
        "end",
        "",
        "function AUC_MoneyAward(nCur, n)",
        "    n = floor(n)",
        "    if nCur == " + P + ".tbCurrency.XU then",
        "        return \"xu:\"..n",
        "    end",
        "    return \"money:\"..n",
        "end",
        "",
        "function AUC_CurName(nCur)",
        "    return " + P + ".tbCurrencyName[nCur] or \"?\"",
        "end",
        "",
        "-- ---------------------------------------------------------------- thu",
        "function AUC_MailItem(szRole, szTitle, szContent, nId)",
        "    return MailManager_SendMail(szRole, " + P + ".szSender, szTitle, szContent, \"aucitem:\"..nId, " + P + ".nMailDays, \"daugia\")",
        "end",
        "",
        "function AUC_MailMoney(szRole, szTitle, szContent, nCur, n)",
        "    if floor(n) <= 0 then",
        "        return 0",
        "    end",
        "    return MailManager_SendMail(szRole, " + P + ".szSender, szTitle, szContent, AUC_MoneyAward(nCur, n), " + P + ".nMailDays, \"daugia\")",
        "end",
        "",
        "-- ---------------------------------------------------------------- doc kho",
        "-- tra ve danh sach dong dang ban cua loai nType (bang hoi: chi bang nTong)",
        "function AUC_Rows(nType, nTong)",
        "    local tb = AUC_List(nType, 200, 0)",
        "    if nType ~= " + P + ".tbAuctionTypeEnum.eType_TONG then",
        "        return tb",
        "    end",
        "    local out = {}",
        "    for i = 1, getn(tb) do",
        "        if tb[i].tong == nTong then",
        "            tinsert(out, tb[i])",
        "        end",
        "    end",
        "    return out",
        "end",
        "",
        "function AUC_ActName(nType, r)",
        "    if nType == " + P + ".tbAuctionTypeEnum.eType_PERSONAL then",
        "        return " + P + ".szPersonalActivity",
        "    end",
        "    return r.activity",
        "end",
        "",
        "-- {[szAct] = {nTotalCount, nStartTime}}",
        "function AUC_Activities(nType, nTong)",
        "    local rows = AUC_Rows(nType, nTong)",
        "    local tb = {}",
        "    for i = 1, getn(rows) do",
        "        local szAct = AUC_ActName(nType, rows[i])",
        "        if szAct ~= \"\" then",
        "            if not tb[szAct] then",
        "                tb[szAct] = {nTotalCount = 0, nStartTime = rows[i].start}",
        "            end",
        "            tb[szAct].nTotalCount = tb[szAct].nTotalCount + 1",
        "            if rows[i].start < tb[szAct].nStartTime then",
        "                tb[szAct].nStartTime = rows[i].start",
        "            end",
        "        end",
        "    end",
        "    if nType == " + P + ".tbAuctionTypeEnum.eType_PERSONAL and not tb[" + P + ".szPersonalActivity] then",
        "        tb[" + P + ".szPersonalActivity] = {nTotalCount = 0, nStartTime = 0}",
        "    end",
        "    return tb",
        "end",
        "",
        "function AUC_SplitDesc(szDesc)",
        "    local t = {}",
        "    local s = szDesc or \"\"",
        "    while 1 do",
        "        local a, b, num = strfind(s, \"^(%-?%d+),?\")",
        "        if not a then",
        "            break",
        "        end",
        "        tinsert(t, tonumber(num) or 0)",
        "        s = strsub(s, b + 1)",
        "        if s == \"\" then",
        "            break",
        "        end",
        "    end",
        "    while getn(t) < 6 do",
        "        tinsert(t, 0)",
        "    end",
        "    return t",
        "end",
        "",
        "-- dong CSDL -> bang gui client (ten truong theo uiauction_house.lua 2.0)",
        "function AUC_RowToClient(r, szMe, nNow)",
        "    local tb = {}",
        "    tb.nId = r.id",
        "    tb.nType = r.kind",
        "    tb.nStartTime = r.start",
        "    tb.nCurrencyType = r.currency",
        "    tb.szCurrencyName = AUC_CurName(r.currency)",
        "    tb.szBelongRole = r.seller",
        "    -- [A9] KHONG long bang ba tang (tbPage > dong > tbItem): ObjBuffer khong dua qua duoc,",
        "    -- ben nhan ra bang RONG. Trai 6 so cua vat pham thanh 6 truong so.",
        "    local pr = AUC_SplitDesc(r.desc)",
        "    tb.nG = pr[1]",
        "    tb.nD = pr[2]",
        "    tb.nP = pr[3]",
        "    tb.nL = pr[4]",
        "    tb.nS = pr[5]",
        "    tb.nK = pr[6]",
        "    tb.szName = r.name",
        "    tb.nCount = 1",
        "    tb.nGuaranteedPrice = r.guar",
        "    tb.nCurPrice = r.cur",
        "    tb.nMaxPrice = r.cur",
        "    tb.nRangePerOffer = floor(r.guar / 10)",
        "    if tb.nRangePerOffer < 1 then",
        "        tb.nRangePerOffer = 1",
        "    end",
        "    tb.nSelfPrice = 0",
        "    if r.kind == " + P + ".tbItemTypeEnum.eType_ENGLISH and r.buyer == szMe then",
        "        tb.nSelfPrice = r.cur",
        "    end",
        "    tb.nRemainingTime = r.endtime - nNow",
        "    tb.nTotalRemainingTime = r.endtime - nNow",
        "    tb.nNextPrice = r.cur",
        "    tb.nFloatInterval = " + P + ".nDutchFloatInterval",
        "    tb.nTotalFloatTimes = " + P + ".nDutchFloatTimes",
        "    tb.nCurFloatTimes = " + P + ".nDutchFloatTimes - r.dropleft",
        "    if r.kind == " + P + ".tbItemTypeEnum.eType_DUTCH and r.dropleft > 0 and r.nextdrop > 0 then",
        "        tb.nRemainingTime = r.nextdrop - nNow",
        "        local nNext = floor(r.cur * (1 - " + P + ".nDutchFloatRange))",
        "        if nNext < r.guar then",
        "            nNext = r.guar",
        "        end",
        "        tb.nNextPrice = nNext",
        "    end",
        "    return tb",
        "end",
        "",
        "-- ---------------------------------------------------------------- gui xuong client",
        "function AUC_SendActivityList(nPlayerIdx, nType, nTong)",
        "    local h = OB_Create()",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nType)",
        "    local tb = AUC_Activities(nType, nTong)",
        "    tb[\"__money\"] = {nTotalCount = GetCash(), nStartTime = GetTask(" + P + ".XU_TASK)}",
        "    ObjBuffer:PushByType(h, OBJTYPE_TABLE, tb)",
        "    AUC_SendTo(nPlayerIdx, \"emSCRIPT_PROTOCOL_AUCTION_ACTIVITYLIST\", h)",
        "    OB_Release(h)",
        "end",
        "",
        "function AUC_SendActivityContent(nPlayerIdx, nType, nTong, szAct, nPage)",
        "    local rows = AUC_Rows(nType, nTong)",
        "    local lst = {}",
        "    for i = 1, getn(rows) do",
        "        if AUC_ActName(nType, rows[i]) == szAct then",
        "            tinsert(lst, rows[i])",
        "        end",
        "    end",
        "    local nTotal = getn(lst)",
        "    if nPage < 1 then",
        "        nPage = 1",
        "    end",
        "    local nFrom = (nPage - 1) * " + P + ".nMaxItemPerPage + 1",
        "    local nTo = nPage * " + P + ".nMaxItemPerPage",
        "    local tbPage = {}",
        "    local szMe = GetName()",
        "    local nNow = GetCurrentTime()",
        "    local k = 0",
        "    for i = nFrom, nTo do",
        "        if lst[i] then",
        "            k = k + 1",
        "            tbPage[k] = AUC_RowToClient(lst[i], szMe, nNow)",
        "        end",
        "    end",
        "    local h = OB_Create()",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nType)",
        "    ObjBuffer:PushByType(h, OBJTYPE_STRING, szAct)",
        "    ObjBuffer:PushByType(h, OBJTYPE_TABLE, tbPage)",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nTotal)",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nPage)",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, 1)",
        "    AUC_SendTo(nPlayerIdx, \"emSCRIPT_PROTOCOL_AUCTION_ACTIVITYINFO\", h)",
        "    OB_Release(h)",
        "end",
        "",
        "-- bao cho moi nguoi dang xem (nType, szAct): szEnum voi ham push tham so",
        "function AUC_Broadcast(nType, szAct, szEnum, fnPush)",
        "    for nIdx, v in AUC_Viewers do",
        "        if v.nType == nType and (szAct == nil or v.szAct == szAct or v.szAct == \"\") then",
        "            local h = OB_Create()",
        "            fnPush(h)",
        "            if AUC_SendTo(nIdx, szEnum, h) ~= 1 then",
        "                AUC_Viewers[nIdx] = nil",
        "            end",
        "            OB_Release(h)",
        "        end",
        "    end",
        "end",
        "",
        "function AUC_NotifyNewItem(nType, szAct, nId)",
        "    local nCount = 0",
        "    local tb = AUC_Activities(nType, 0)",
        "    if tb[szAct] then",
        "        nCount = tb[szAct].nTotalCount",
        "    end",
        "    AUC_Broadcast(nType, szAct, \"emSCRIPT_PROTOCOL_AUCTION_NEWITEM\", function(h)",
        "        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nType)",
        "        ObjBuffer:PushByType(h, OBJTYPE_STRING, %szAct)",
        "        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nId)",
        "        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nCount)",
        "    end)",
        "end",
        "",
        "function AUC_NotifyEndItem(nType, szAct, nId)",
        "    local nCount = 0",
        "    local tb = AUC_Activities(nType, 0)",
        "    if tb[szAct] then",
        "        nCount = tb[szAct].nTotalCount",
        "    end",
        "    AUC_Broadcast(nType, szAct, \"emSCRIPT_PROTOCOL_AUCTION_ENDITEM\", function(h)",
        "        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nType)",
        "        ObjBuffer:PushByType(h, OBJTYPE_STRING, %szAct)",
        "        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nId)",
        "        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nCount)",
        "    end)",
        "end",
        "",
        "function AUC_NotifyPrice(nType, szAct, r, nNow)",
        "    local tb = AUC_RowToClient(r, \"\", nNow)",
        "    AUC_Broadcast(nType, szAct, \"emSCRIPT_PROTOCOL_AUCTION_TPRICECHANGE\", function(h)",
        "        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nType)",
        "        ObjBuffer:PushByType(h, OBJTYPE_STRING, %szAct)",
        "        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %tb.nId)",
        "        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %tb.nCurPrice)",
        "        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %tb.nNextPrice)",
        "        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %tb.nRemainingTime)",
        "        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %tb.nCurFloatTimes)",
        "    end)",
        "end",
        "",
        "function AUC_NotifyNewActivity(nType, szAct, nStart)",
        "    AUC_Broadcast(nType, nil, \"emSCRIPT_PROTOCOL_AUCTION_NEWACTIVITY\", function(h)",
        "        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nType)",
        "        ObjBuffer:PushByType(h, OBJTYPE_STRING, %szAct)",
        "        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nStart)",
        "    end)",
        "end",
        "",
        "function AUC_NotifyEndActivity(nType, szAct)",
        "    AUC_Broadcast(nType, nil, \"emSCRIPT_PROTOCOL_AUCTION_ENDACTIVITY\", function(h)",
        "        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nType)",
        "        ObjBuffer:PushByType(h, OBJTYPE_STRING, %szAct)",
        "    end)",
        "end",
        "",
        "-- ---------------------------------------------------------------- nhan tu client (protocol_def_gs.lua)",
        "function AUC_MyTong()",
        "    local _, nTong = GetTongName()",
        "    return nTong or 0",
        "end",
        "",
        "function AUC_OnRequestActivityList(nType, nStartTime)",
        "    AUC_Viewers[PlayerIndex] = {nType = nType, szAct = \"\"}",
        "    AUC_SendActivityList(PlayerIndex, nType, AUC_MyTong())",
        "end",
        "",
        "function AUC_OnRequestActivityContent(nType, szAct, nPage, tbIds)",
        "    AUC_Viewers[PlayerIndex] = {nType = nType, szAct = szAct}",
        "    AUC_SendActivityContent(PlayerIndex, nType, AUC_MyTong(), szAct, nPage)",
        "end",
        "",
        "function AUC_OnRequestItemContent(nType, szAct, nId)",
        "    local r = AUC_Get(nId)",
        "    if not r then",
        "        return",
        "    end",
        "    local h = OB_Create()",
        "    ObjBuffer:PushByType(h, OBJTYPE_TABLE, AUC_RowToClient(r, GetName(), GetCurrentTime()))",
        "    AUC_SendTo(PlayerIndex, \"emSCRIPT_PROTOCOL_AUCTION_ITEMINFO\", h)",
        "    OB_Release(h)",
        "end",
        "",
        "function AUC_OnRequestMemberList(szAct)",
        "    local nTong = AUC_MyTong()",
        "    local szMem = \"\"",
        "    local nCount = 0",
        "    if nTong > 0 then",
        "        local nMem = TONG_GetFirstMember(nTong, -1)",
        "        while nMem and nMem > 0 and nCount < 60 do",
        "            local szTen = TONGM_GetName(nTong, nMem)",
        "            if szTen and szTen ~= \"\" then",
        "                -- [A9] noi thanh CHUOI (bang long ba tang khong qua duoc ObjBuffer)",
        "                if szMem ~= \"\" then",
        "                    szMem = szMem..\";\"",
        "                end",
        "                szMem = szMem..szTen..\",\"..((TONGM_GetLevel and TONGM_GetLevel(nTong, nMem)) or 0)..\",\"..(TONGM_GetFigure(nTong, nMem) or 3)..\",\"..(TONGM_GetOnline(nTong, nMem) or 0)",
        "                nCount = nCount + 1",
        "            end",
        "            nMem = TONG_GetNextMember(nTong, nMem, -1)",
        "        end",
        "    end",
        "    local tb = {szActivityName = szAct, szMemberList = szMem, nCount = nCount, nSalary = 0}",
        "    if nTong > 0 and nCount > 0 then",
        "        tb.nSalary = floor(TONG_GetMoney(nTong) / nCount)",
        "    end",
        "    local h = OB_Create()",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, " + P + ".tbAuctionTypeEnum.eType_TONG)",
        "    ObjBuffer:PushByType(h, OBJTYPE_TABLE, tb)",
        "    AUC_SendTo(PlayerIndex, \"emSCRIPT_PROTOCOL_AUCTION_MEMBERLIST\", h)",
        "    OB_Release(h)",
        "end",
        "",
        "function AUC_ReplyOffer(nType, szAct, nId, nPrice)",
        "    local h = OB_Create()",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nType)",
        "    ObjBuffer:PushByType(h, OBJTYPE_STRING, szAct)",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nId)",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nPrice)",
        "    AUC_SendTo(PlayerIndex, \"emSCRIPT_PROTOCOL_AUCTION_OFFERPRICERET\", h)",
        "    OB_Release(h)",
        "end",
        "",
        "-- kiem tra chung: dong con ban, dung loai, dung bang",
        "function AUC_CheckRow(r, nType)",
        "    if not r or r.state ~= 0 then",
        "        Msg2Player(\"" + V("Vật phẩm này đã bán hoặc đã kết thúc.") + "\")",
        "        return 0",
        "    end",
        "    if r.atype ~= nType then",
        "        return 0",
        "    end",
        "    if nType == " + P + ".tbAuctionTypeEnum.eType_TONG and r.tong ~= AUC_MyTong() then",
        "        Msg2Player(\"" + V("Phiên này của bang khác.") + "\")",
        "        return 0",
        "    end",
        "    return 1",
        "end",
        "",
        "-- MUA NGAY / ky gui / Ha Lan: tra dung gia hien tai",
        "function AUC_OnRequestOfferDutch(nType, szAct, nId, nPrice)",
        "    local r = AUC_Get(nId)",
        "    if AUC_CheckRow(r, nType) == 0 then",
        "        return",
        "    end",
        "    if r.seller == GetName() then",
        "        Msg2Player(\"" + V("Không thể mua món do chính mình ký gửi.") + "\")",
        "        return",
        "    end",
        "    if nPrice < r.cur then",
        "        Msg2Player(\"" + V("Giá đã thay đổi, hãy xem lại.") + "\")",
        "        return",
        "    end",
        "    if AUC_GetMoney(r.currency) < r.cur then",
        "        Msg2Player(\"" + V("Không đủ ") + "\"..AUC_CurName(r.currency)..\"" + V(" (cần ") + "\"..r.cur..\").\")",
        "        return",
        "    end",
        "    if AUC_Buy(nId, GetName(), r.cur) ~= 1 then",
        "        Msg2Player(\"" + V("Có người vừa mua trước, xin lỗi đại hiệp.") + "\")",
        "        return",
        "    end",
        "    if AUC_PayMoney(r.currency, r.cur) ~= 1 then",
        "        -- tra lai trang thai (hiem: tien vua doi)",
        "        AUC_SetState(nId, 0, 2)",
        "        Msg2Player(\"" + V("Trừ tiền thất bại.") + "\")",
        "        return",
        "    end",
        "    AUC_Settle(r, GetName(), r.cur)",
        "    AUC_ReplyOffer(nType, szAct, nId, r.cur)",
        "    AUC_NotifyEndItem(nType, AUC_ActName(nType, r), nId)",
        "end",
        "",
        "-- KIEU ANH: tra gia (giu tien nguoi tra; nguoi bi vuot duoc hoan qua thu)",
        "function AUC_OnRequestOfferEnglish(nType, szAct, nId, nNewPrice)",
        "    local r = AUC_Get(nId)",
        "    if AUC_CheckRow(r, nType) == 0 then",
        "        return",
        "    end",
        "    if r.kind ~= " + P + ".tbItemTypeEnum.eType_ENGLISH then",
        "        return AUC_OnRequestOfferDutch(nType, szAct, nId, nNewPrice)",
        "    end",
        "    if r.seller == GetName() then",
        "        Msg2Player(\"" + V("Không thể tự trả giá món của mình.") + "\")",
        "        return",
        "    end",
        "    local nStep = floor(r.guar / 10)",
        "    if nStep < 1 then",
        "        nStep = 1",
        "    end",
        "    local nMin = r.guar",
        "    if r.buyer ~= \"\" then",
        "        nMin = r.cur + nStep",
        "    end",
        "    if nNewPrice < nMin then",
        "        Msg2Player(\"" + V("Giá trả phải từ ") + "\"..nMin..\" \"..AUC_CurName(r.currency)..\".\")",
        "        return",
        "    end",
        "    if AUC_GetMoney(r.currency) < nNewPrice then",
        "        Msg2Player(\"" + V("Không đủ ") + "\"..AUC_CurName(r.currency)..\".\")",
        "        return",
        "    end",
        "    local nNow = GetCurrentTime()",
        "    local nEnd = r.endtime",
        "    if nEnd - nNow < " + P + ".nRefreshRemingTimeWhenOffer then",
        "        nEnd = nNow + " + P + ".nRefreshRemingTimeWhenOffer",
        "    end",
        "    local szOld, nOld = r.buyer, r.cur",
        "    if AUC_Bid(nId, GetName(), nNewPrice, nEnd) ~= 1 then",
        "        Msg2Player(\"" + V("Có người vừa trả giá cao hơn, hãy xem lại.") + "\")",
        "        return",
        "    end",
        "    if AUC_PayMoney(r.currency, nNewPrice) ~= 1 then",
        "        Msg2Player(\"" + V("Trừ tiền thất bại.") + "\")",
        "        AUC_Log(format(\"LOI: bid id %d cua %s da ghi nhung tru tien that bai\", nId, GetName()))",
        "        return",
        "    end",
        "    if szOld ~= \"\" and szOld ~= GetName() and nOld > 0 then",
        "        AUC_MailMoney(szOld, \"" + V("Hoàn tiền đấu giá") + "\", \"" + V("Có người trả giá cao hơn cho ") + "\"..r.name..\"" + V(", hoàn lại tiền đã trả.") + "\", r.currency, nOld)",
        "    elseif szOld == GetName() and nOld > 0 then",
        "        -- tu nang gia cua minh: hoan phan cu",
        "        AUC_MailMoney(szOld, \"" + V("Hoàn tiền đấu giá") + "\", \"" + V("Đại hiệp nâng giá cho ") + "\"..r.name..\"" + V(", hoàn lại mức trả trước.") + "\", r.currency, nOld)",
        "    end",
        "    Msg2Player(\"" + V("Đã trả giá ") + "\"..nNewPrice..\" \"..AUC_CurName(r.currency)..\"" + V(" cho ") + "\"..r.name..\".\")",
        "    AUC_ReplyOffer(nType, szAct, nId, nNewPrice)",
        "    local r2 = AUC_Get(nId)",
        "    if r2 then",
        "        AUC_NotifyPrice(nType, AUC_ActName(nType, r2), r2, nNow)",
        "    end",
        "end",
        "",
        "function AUC_OnRequestRefund(nType, szAct, nId)",
        "    local r = AUC_Get(nId)",
        "    if not r then",
        "        return",
        "    end",
        "    if r.state == 0 and r.buyer == GetName() then",
        "        Msg2Player(\"" + V("Đại hiệp đang giữ giá cao nhất, không thể rút lại.") + "\")",
        "    else",
        "        Msg2Player(\"" + V("Đại hiệp không có mức giá nào đang giữ ở món này.") + "\")",
        "    end",
        "    local h = OB_Create()",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nType)",
        "    ObjBuffer:PushByType(h, OBJTYPE_STRING, szAct)",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nId)",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, 0)",
        "    AUC_SendTo(PlayerIndex, \"emSCRIPT_PROTOCOL_AUCTION_REFUNDRET\", h)",
        "    OB_Release(h)",
        "end",
        "",
        "-- lay lai mon ky gui (khong hoan phi)",
        "function AUC_OnRequestGetBack(nType, szAct, nId)",
        "    local r = AUC_Get(nId)",
        "    if AUC_CheckRow(r, nType) == 0 then",
        "        return",
        "    end",
        "    if r.seller ~= GetName() then",
        "        Msg2Player(\"" + V("Không phải món của đại hiệp.") + "\")",
        "        return",
        "    end",
        "    if r.buyer ~= \"\" then",
        "        Msg2Player(\"" + V("Đã có người trả giá, không thể rút lại.") + "\")",
        "        return",
        "    end",
        "    if AUC_SetState(nId, 3, 1) ~= 1 then",
        "        Msg2Player(\"" + V("Không rút được, hãy thử lại.") + "\")",
        "        return",
        "    end",
        "    AUC_MailItem(r.seller, \"" + V("Rút vật phẩm ký gửi") + "\", \"" + V("Đại hiệp đã rút ") + "\"..r.name..\"" + V(" khỏi khu đấu giá, phí ký gửi không hoàn.") + "\", nId)",
        "    Msg2Player(\"" + V("Đã rút ") + "\"..r.name..\"" + V(", vật phẩm gửi về hộp thư.") + "\")",
        "    AUC_Log(format(\"%s rut lai id %d (%s)\", GetName(), nId, r.name))",
        "    AUC_NotifyEndItem(nType, AUC_ActName(nType, r), nId)",
        "end",
        "",
        "-- ---------------------------------------------------------------- chot giao dich",
        "-- r: dong (truoc khi doi), szBuyer, nPrice: gia chot. Giao do cho nguoi mua, tien cho nguoi ban / quy bang.",
        "function AUC_Settle(r, szBuyer, nPrice)",
        "    local nTax = floor(nPrice * " + P + ".nAuctionTaxRate / 100)",
        "    local nNet = nPrice - nTax",
        "    AUC_MailItem(szBuyer, \"" + V("Đấu giá thành công") + "\", \"" + V("Đại hiệp đã mua được ") + "\"..r.name..\"" + V(" với giá ") + "\"..nPrice..\" \"..AUC_CurName(r.currency)..\"" + V(". Vật phẩm đính kèm trong thư.") + "\", r.id)",
        "    if r.atype == " + P + ".tbAuctionTypeEnum.eType_TONG and r.tong > 0 then",
        "        if r.currency == " + P + ".tbCurrency.MONEY then",
        "            TONG_ApplyAddMoney(r.tong, nNet)",
        "            TONG_ApplyAddEventRecord(r.tong, \"" + V("Đấu giá bang: bán ") + "\"..r.name..\"" + V(" được ") + "\"..nNet..\"" + V(" Ngân lượng vào quỹ") + "\")",
        "        else",
        "            AUC_MailMoney(r.seller, \"" + V("Tiền bán đấu giá bang") + "\", \"" + V("Bán ") + "\"..r.name..\"" + V(" trong phiên bang, thu ") + "\"..nNet..\" Xu.\", r.currency, nNet)",
        "        end",
        "    elseif r.atype == " + P + ".tbAuctionTypeEnum.eType_WORLD then",
        "        -- phien the gioi do GM mo: tien vao he thong (khong tra ai)",
        "    else",
        "        AUC_MailMoney(r.seller, \"" + V("Tiền bán ký gửi") + "\", \"" + V("Món ") + "\"..r.name..\"" + V(" đã bán được ") + "\"..nPrice..\" \"..AUC_CurName(r.currency)..\"" + V(", trừ thuế ") + "\"..nTax..\"" + V(", đại hiệp nhận ") + "\"..nNet..\".\", r.currency, nNet)",
        "    end",
        "    AUC_SetState(r.id, 3, 3)",
        "    AUC_Log(format(\"BAN id %d %s: %s -> %s gia %d tien %d (thue %d)\", r.id, r.name, r.seller, szBuyer, nPrice, r.currency, nTax))",
        "end",
        "",
        "-- het han / khong ai mua: tra do ve nguoi ban",
        "function AUC_Expire(r)",
        "    if AUC_SetState(r.id, 2, 1) ~= 1 then",
        "        return",
        "    end",
        "    if r.atype == " + P + ".tbAuctionTypeEnum.eType_WORLD then",
        "        AUC_Log(format(\"LUU PHACH the gioi id %d %s\", r.id, r.name))",
        "        AUC_SetState(r.id, 3, 3)",
        "        return",
        "    end",
        "    AUC_MailItem(r.seller, \"" + V("Vật phẩm chưa bán được") + "\", \"" + V("Món ") + "\"..r.name..\"" + V(" hết hạn mà chưa ai mua, trả lại đại hiệp.") + "\", r.id)",
        "    AUC_Log(format(\"HET HAN id %d %s tra %s\", r.id, r.name, r.seller))",
        "end",
        "",
        "-- kieu Anh ket thuc co nguoi tra gia",
        "function AUC_FinishEnglish(r)",
        "    if r.buyer == \"\" then",
        "        return AUC_Expire(r)",
        "    end",
        "    if AUC_SetState(r.id, 1, 1) ~= 1 then",
        "        return",
        "    end",
        "    AUC_Settle(r, r.buyer, r.cur)",
        "end",
        "",
        "-- ---------------------------------------------------------------- DAT BAN (goi tu NPC / GM)",
        "-- nType, szAct, nKind, nCur, nPrice, nItemIdx, nTong -> id (0 = loi). Chinh nguoi choi hien tai la nguoi ban.",
        "function AUC_PutOnItem(nType, szAct, nKind, nCur, nPrice, nItemIdx, nTong)",
        "    if nItemIdx == nil or nItemIdx <= 0 then",
        "        return 0",
        "    end",
        "    local szRec, szName, szDesc, nCells = AUC_ItemToRec(nItemIdx)",
        "    if szRec == nil or szRec == \"\" then",
        "        Msg2Player(\"" + V("Món này không thể ký gửi (quặng, nguyên liệu thô...).") + "\")",
        "        return 0",
        "    end",
        "    if GetItemBindState and GetItemBindState(nItemIdx) ~= 0 then",
        "        Msg2Player(\"" + V("Vật phẩm khoá không thể ký gửi.") + "\")",
        "        return 0",
        "    end",
        "    local nNow = GetCurrentTime()",
        "    local nBase, nCurP, nGuar, nEnd, nNextDrop, nDropLeft = nPrice, nPrice, nPrice, nNow + " + P + ".nPersonalDuration, 0, 0",
        "    if nType ~= " + P + ".tbAuctionTypeEnum.eType_PERSONAL then",
        "        if nKind == " + P + ".tbItemTypeEnum.eType_ENGLISH then",
        "            nEnd = nNow + " + P + ".nEnglishRemainingTime",
        "        else",
        "            nCurP = floor(nPrice * " + P + ".nDutchInitRate)",
        "            nBase = nCurP",
        "            nNextDrop = nNow + " + P + ".nDutchFloatInterval",
        "            nDropLeft = " + P + ".nDutchFloatTimes",
        "            nEnd = nNow + " + P + ".nDutchFloatInterval * (" + P + ".nDutchFloatTimes + 1)",
        "        end",
        "    end",
        "    local nDeposit = 0",
        "    if nType == " + P + ".tbAuctionTypeEnum.eType_PERSONAL then",
        "        nDeposit = floor(nPrice * " + P + ".nPersonalPutOnCost / 100)",
        "        if AUC_PayMoney(nCur, nDeposit) ~= 1 then",
        "            Msg2Player(\"" + V("Không đủ ") + "\"..AUC_CurName(nCur)..\"" + V(" để trả phí ký gửi ") + "\"..nDeposit..\".\")",
        "            return 0",
        "        end",
        "    end",
        "    local nId = AUC_PutOn(nType, szAct or \"\", nKind, GetName(), nTong or 0, szName, szDesc, szRec, nCells or 1, nCur,",
        "        nBase, nCurP, nGuar, nDeposit, nNow, nEnd, nNextDrop, nDropLeft)",
        "    if nId <= 0 then",
        "        if nDeposit > 0 then",
        "            if nCur == " + P + ".tbCurrency.XU then",
        "                SetTask(" + P + ".XU_TASK, GetTask(" + P + ".XU_TASK) + nDeposit)",
        "            else",
        "                Earn(nDeposit)",
        "            end",
        "        end",
        "        Msg2Player(\"" + V("Kho đấu giá lỗi, hãy thử lại sau.") + "\")",
        "        return 0",
        "    end",
        "    RemoveItemByIndex(nItemIdx, GetItemStackCount(nItemIdx))",
        "    AUC_Log(format(\"DAT BAN id %d loai %d '%s' %s gia %d tien %d nguoi %s\", nId, nType, szAct or \"\", szName, nPrice, nCur, GetName()))",
        "    local szActReal = szAct",
        "    if nType == " + P + ".tbAuctionTypeEnum.eType_PERSONAL then",
        "        szActReal = " + P + ".szPersonalActivity",
        "    end",
        "    AUC_NotifyNewItem(nType, szActReal, nId)",
        "    return nId",
        "end",
        "",
        "-- ---------------------------------------------------------------- DAT BAN TU NUT TREN CUA SO (khong dung NPC)",
        "-- Chu 04/09: \"khong can lam npc ma lam nut tren box dau gia luon\". Nut goc phai moi trang gui",
        "-- REQUEST_PUTON; o day mo hai hop nhap so roi hop dat vat pham. CA BA hop deu CHI DINH duong dan script",
        "-- (OpenGetNumber 4 doi, GiveItemUI 8 doi) nen callback chay dung trong tep nay du khong thoai voi NPC nao.",
        "AUC_SCRIPT = \"\\\\script\\\\auction_house\\\\auction_manager.lua\"",
        "AUC_TMP = AUC_TMP or {}",
        "",
        "function AUC_OnRequestPutOn(nType)",
        "    local szTong, nTong = GetTongName()",
        "    nTong = nTong or 0",
        "    local nKind = " + P + ".tbItemTypeEnum.eType_ENGLISH",
        "    if nType == " + P + ".tbAuctionTypeEnum.eType_PERSONAL then",
        "        nKind = " + P + ".tbItemTypeEnum.eType_DUTCH",
        "        if AUC_CountSeller(GetName()) >= " + P + ".nMaxItemPerSeller then",
        "            Msg2Player(\"" + V("Đại hiệp đang ký gửi tối đa ") + "\".." + P + ".nMaxItemPerSeller..\"" + V(" món, hãy chờ bán xong hoặc rút bớt.") + "\")",
        "            return",
        "        end",
        "    elseif nType == " + P + ".tbAuctionTypeEnum.eType_WORLD then",
        "        if not (admincheck and admincheck() == 1) then",
        "            Msg2Player(\"" + V("Chỉ GM mới mở được phiên đấu giá thế giới.") + "\")",
        "            return",
        "        end",
        "    else",
        "        if nTong <= 0 then",
        "            Msg2Player(\"" + V("Đại hiệp chưa gia nhập bang hội.") + "\")",
        "            return",
        "        end",
        "        if TONG_GetMaster(nTong) ~= GetName() then",
        "            -- khong phai bang chu: nut nay xem DANH SACH THANH VIEN cua phien bang",
        "            AUC_OnRequestMemberList(szTong)",
        "            return",
        "        end",
        "    end",
        "    -- [A6] MOT hop duy nhat: hop dua vat pham da co o nhap gia + nut doi loai tien.",
        "    -- Client gui AUCTION_REQUEST_SETPRICE (gia, loai tien) NGAY TRUOC khi bam Dong y.",
        "    AUC_TMP[PlayerIndex] = {nType = nType, nKind = nKind, nTong = nTong, nCur = " + P + ".tbCurrency.MONEY, nPrice = 0}",
        "    GiveItemUI(\"" + V("Ký gửi đấu giá: đặt vật phẩm vào ô, nhập giá rồi bấm Đồng ý") + "\", \"" + V("Phí ký gửi ") + "\".." + P + ".nPersonalPutOnCost..\"" + V("% giá bán, thuế ") + "\".." + P + ".nAuctionTaxRate..\"" + V("% khi bán được") + "\", \"AUC_OnGiveOk\", \"AUC_OnGiveCancel\", 0, \"AUC_OnGiveCheck\", 0, AUC_SCRIPT)",
        "end",
        "",
        "-- [A6] client bao GIA + LOAI TIEN (o ngay trong hop dua vat pham) truoc khi bam Dong y",
        "function AUC_OnRequestSetPrice(nPrice, nCur)",
        "    local t = AUC_TMP[PlayerIndex]",
        "    if not t then",
        "        return",
        "    end",
        "    t.nPrice = floor(nPrice or 0)",
        "    if nCur == " + P + ".tbCurrency.XU then",
        "        t.nCur = " + P + ".tbCurrency.XU",
        "    else",
        "        t.nCur = " + P + ".tbCurrency.MONEY",
        "    end",
        "end",
        "",        "function AUC_OnGiveCheck(nCount)",
        "    return 1",
        "end",
        "",
        "function AUC_OnGiveCancel()",
        "    AUC_TMP[PlayerIndex] = nil",
        "end",
        "",
        "function AUC_OnGiveOk(nCount)",
        "    local t = AUC_TMP[PlayerIndex]",
        "    AUC_TMP[PlayerIndex] = nil",
        "    if not t then",
        "        return",
        "    end",
        "    if (t.nPrice or 0) < 1 or t.nPrice > 2000000000 then",
        "        Msg2Player(\"" + V("Chưa nhập giá bán hợp lệ.") + "\")",
        "        return",
        "    end",
        "    local nIdx = GetGiveItemUnit(1)",
        "    if nIdx == nil or nIdx <= 0 then",
        "        Msg2Player(\"" + V("Chưa đặt vật phẩm vào ô.") + "\")",
        "        return",
        "    end",
        "    local szAct = \"\"",
        "    if t.nType == " + P + ".tbAuctionTypeEnum.eType_TONG then",
        "        szAct = GetTongName()",
        "    elseif t.nType == " + P + ".tbAuctionTypeEnum.eType_WORLD then",
        "        szAct = \"" + V("Phiên ") + "\"..GetLocalDate(\"%H:%M %d/%m\")",
        "    end",
        "    local nId = AUC_PutOnItem(t.nType, szAct, t.nKind, t.nCur, t.nPrice, nIdx, t.nTong)",
        "    if nId > 0 then",
        "        Msg2Player(\"" + V("Đã đưa vào khu đấu giá, mã số ") + "\"..nId..\"" + V(". Tiền bán và vật phẩm trả về qua hộp thư.") + "\")",
        "    end",
        "end",
        "",
        "-- ---------------------------------------------------------------- QUET (auctionpoll.lua goi moi 30 giay)",
        "function AUC_Tick()",
        "    local nNow = GetCurrentTime()",
        "    -- 1) het han / ket thuc",
        "    local tb = AUC_Sweep(nNow, 30)",
        "    for i = 1, getn(tb) do",
        "        local r = tb[i]",
        "        local szAct = AUC_ActName(r.atype, r)",
        "        if r.kind == " + P + ".tbItemTypeEnum.eType_ENGLISH then",
        "            AUC_FinishEnglish(r)",
        "        else",
        "            AUC_Expire(r)",
        "        end",
        "        AUC_NotifyEndItem(r.atype, szAct, r.id)",
        "    end",
        "    -- 2) Ha Lan giam gia (the gioi / bang hoi)",
        "    for _, nType in {" + P + ".tbAuctionTypeEnum.eType_WORLD, " + P + ".tbAuctionTypeEnum.eType_TONG} do",
        "        local rows = AUC_List(nType, 200, 0)",
        "        for i = 1, getn(rows) do",
        "            local r = rows[i]",
        "            if r.kind == " + P + ".tbItemTypeEnum.eType_DUTCH and r.dropleft > 0 and r.nextdrop > 0 and r.nextdrop <= nNow then",
        "                local nNext = floor(r.cur * (1 - " + P + ".nDutchFloatRange))",
        "                if nNext < r.guar then",
        "                    nNext = r.guar",
        "                end",
        "                if AUC_SetPrice(r.id, nNext, nNow + " + P + ".nDutchFloatInterval, r.dropleft - 1, r.endtime) == 1 then",
        "                    r.cur = nNext",
        "                    r.nextdrop = nNow + " + P + ".nDutchFloatInterval",
        "                    r.dropleft = r.dropleft - 1",
        "                    AUC_NotifyPrice(nType, AUC_ActName(nType, r), r, nNow)",
        "                end",
        "            end",
        "        end",
        "        -- 3) phien het mon -> bao ket thuc phien",
        "        local tbAct = AUC_Activities(nType, 0)",
        "        for k, v in AUC_LastCount do",
        "            local a, b, t, n = strfind(k, \"^(%d+)|(.*)$\")",
        "            if a and tonumber(t) == nType and not tbAct[n] then",
        "                AUC_NotifyEndActivity(nType, n)",
        "                AUC_LastCount[k] = nil",
        "            end",
        "        end",
        "        for n, v in tbAct do",
        "            AUC_LastCount[nType..\"|\"..n] = v.nTotalCount",
        "        end",
        "    end",
        "end",
        "",
    ]
    return E.join(L)


# ============================================================================ auctionpoll.lua (timer)
def build_poll():
    L = [
        "-- auctionpoll.lua - " + MARK + " quet dau gia moi 30 giay (giam gia Ha Lan, ket thuc kieu Anh, het han ky gui).",
        "-- CHI tep nay dang ky timer (khuon mailpoll.lua). KHONG Include tep nay o noi khac.",
        "Include(\"" + lp("script", "auction_house", "auction_manager.lua") + "\")",
        "AUCPOLL_FRAMES = 30 * 18",
        "AUCPOLL_GLB    = 9002   -- GlbValue: chi mot state dang ky timer",
        "function AucPoll_Tick(nParam, nTimerId)",
        "    if AUC_Ready() == 1 then",
        "        AUC_Tick()",
        "    end",
        "    return AUCPOLL_FRAMES",
        "end",
        "if GetGlbValue(AUCPOLL_GLB) ~= 1 then",
        "    SetGlbValue(AUCPOLL_GLB, 1)",
        "    AddTimer(AUCPOLL_FRAMES, \"AucPoll_Tick\", 0)",
        "end",
        "",
    ]
    return E.join(L)


# ============================================================================ uiauction_house.lua (client)
def build_client_ui():
    P = "AUCTION_DEF"
    T = P + ".tbAuctionTypeEnum"
    K = P + ".tbItemTypeEnum"
    L = [
        "-- uiauction_house.lua - " + MARK + " cua so DAU GIA phia client. Viet lai tu \\script\\ui\\uiauction_house.lua cua",
        "-- client JX1 cu (2.0) cho JX1: van goi 21 ham C++ cung ten (KAuctionClient.cpp), nhung vat pham truyen bang 6 so",
        "-- (genre,detail,particular,level,series,luck) + so luong + ten + ten tien; hop xac nhan bo (hanh dong truc tiep).",
        "Include(\"" + lp("script", "protocol.lua") + "\")",
        "Include(\"" + lp("script", "lib", "objbuffer_head.lua") + "\")",
        "Include(\"" + lp("script", "auction_house", "auction_def.lua") + "\")",
        "",
        "UIAuctionHouse = UIAuctionHouse or {}",
        "UIAuctionHouse.tbActivityList = {}",
        "UIAuctionHouse.tbActivityList[" + T + ".eType_TONG] = {}",
        "UIAuctionHouse.tbActivityList[" + T + ".eType_WORLD] = {}",
        "UIAuctionHouse.tbActivityList[" + T + ".eType_PERSONAL] = {}",
        "UIAuctionHouse.nCurTypeIndex = " + T + ".eType_PERSONAL",
        "UIAuctionHouse.szCurActivityName = \"\"",
        "UIAuctionHouse.nCurPageIndex = 1",
        "",
        "function UIAuctionHouse:Reset()",
        "    self.tbActivityList = {}",
        "    self.tbActivityList[" + T + ".eType_TONG] = {}",
        "    self.tbActivityList[" + T + ".eType_WORLD] = {}",
        "    self.tbActivityList[" + T + ".eType_PERSONAL] = {}",
        "    self.szCurActivityName = \"\"",
        "    self.nCurPageIndex = 1",
        "end",
        "",
        "function UIAuctionHouse:TalkToPlayer(szMsg)",
        "    Msg2Player(szMsg)",
        "end",
        "",
        "function UIAuctionHouse:SwitchToAuctionWindow(nType)",
        "    self.nCurTypeIndex = nType",
        "    self.szCurActivityName = \"\"",
        "    self.nCurPageIndex = 1",
        "    AuctionClearAll()",
        "    SwitchAuctionWindow(nType)",
        "    for szName, tbInfo in self.tbActivityList[nType] do",
        "        AuctionAddActivity(nType, szName, tbInfo.nStartTime or 0)",
        "    end",
        "    self:RequestActivityList(nType)",
        "end",
        "",
        "function UIAuctionHouse:GetActivity(szAct)",
        "    return self.tbActivityList[self.nCurTypeIndex][szAct]",
        "end",
        "",
        "function UIAuctionHouse:GetActivityMaxPage(szAct)",
        "    local a = self:GetActivity(szAct)",
        "    if not a or not a.nTotalCount or a.nTotalCount <= 0 then",
        "        return 0",
        "    end",
        "    return floor((a.nTotalCount - 1) / " + P + ".nMaxItemPerPage) + 1",
        "end",
        "",
        "function UIAuctionHouse:ResetPageInfo(nType, szAct)",
        "    if szAct ~= \"\" then",
        "        local nMax = self:GetActivityMaxPage(szAct)",
        "        if nMax > 0 and self.nCurPageIndex > nMax then",
        "            self.nCurPageIndex = nMax",
        "        end",
        "        if nMax > 0 then",
        "            AuctionSetCurrentPageTxt(nType, format(\"%d/%d\", self.nCurPageIndex, nMax))",
        "            return",
        "        end",
        "    end",
        "    AuctionSetCurrentPageTxt(nType, \"-/-\")",
        "end",
        "",
        "function UIAuctionHouse:FirstActivity()",
        "    local szMin, nMin = \"\", 0",
        "    for szName, tbInfo in self.tbActivityList[self.nCurTypeIndex] do",
        "        if szMin == \"\" or (tbInfo.nStartTime or 0) < nMin then",
        "            szMin = szName",
        "            nMin = tbInfo.nStartTime or 0",
        "        end",
        "    end",
        "    return szMin",
        "end",
        "",
        "function UIAuctionHouse:GetItem(szAct, nId)",
        "    local a = self:GetActivity(szAct)",
        "    if not a or not a.tbItemList then",
        "        return nil",
        "    end",
        "    for nPage, tbPage in a.tbItemList do",
        "        for i, tbItem in tbPage do",
        "            if tbItem.nId == nId then",
        "                return tbItem, nPage",
        "            end",
        "        end",
        "    end",
        "    return nil",
        "end",
        "",
        "-- ---------------------------------------------------------------- gui len may chu",
        "function UIAuctionHouse:RequestActivityList(nType)",
        "    local h = OB_Create()",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nType)",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, 0)",
        "    ScriptProtocol:SendData(\"emSCRIPT_PROTOCOL_AUCTION_REQUEST_ACTIVITYLIST\", h)",
        "    OB_Release(h)",
        "end",
        "",
        "function UIAuctionHouse:RequestActivityContent(szAct, nPage)",
        "    local h = OB_Create()",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, self.nCurTypeIndex)",
        "    ObjBuffer:PushByType(h, OBJTYPE_STRING, szAct)",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nPage)",
        "    ObjBuffer:PushByType(h, OBJTYPE_TABLE, {})",
        "    ScriptProtocol:SendData(\"emSCRIPT_PROTOCOL_AUCTION_REQUEST_ACTIVITYCONTENT\", h)",
        "    OB_Release(h)",
        "end",
        "",
        "function UIAuctionHouse:RequestMemberList()",
        "    if self.szCurActivityName == \"\" then",
        "        return",
        "    end",
        "    local h = OB_Create()",
        "    ObjBuffer:PushByType(h, OBJTYPE_STRING, self.szCurActivityName)",
        "    ScriptProtocol:SendData(\"emSCRIPT_PROTOCOL_AUCTION_REQUEST_MEMBERLIST\", h)",
        "    OB_Release(h)",
        "end",
        "",
        "function UIAuctionHouse:SendItemReq(szEnum, nId, nPrice)",
        "    if self.szCurActivityName == \"\" then",
        "        self:TalkToPlayer(\"" + V("Chưa chọn phiên.") + "\")",
        "        return",
        "    end",
        "    local h = OB_Create()",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, self.nCurTypeIndex)",
        "    ObjBuffer:PushByType(h, OBJTYPE_STRING, self.szCurActivityName)",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nId)",
        "    if nPrice then",
        "        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nPrice)",
        "    end",
        "    ScriptProtocol:SendData(szEnum, h)",
        "    OB_Release(h)",
        "end",
        "",
        "function UIAuctionHouse:RequestOfferEnglishPrice(nId, nPrice)",
        "    self:SendItemReq(\"emSCRIPT_PROTOCOL_AUCTION_REQUEST_OFFERENGLISHPRICE\", nId, nPrice)",
        "end",
        "",
        "function UIAuctionHouse:RequestOfferDutchPrice(nId, nPrice)",
        "    self:SendItemReq(\"emSCRIPT_PROTOCOL_AUCTION_REQUEST_OFFERDUTCHPRICE\", nId, nPrice)",
        "end",
        "",
        "function UIAuctionHouse:RequestGetBackItem(nId)",
        "    self:SendItemReq(\"emSCRIPT_PROTOCOL_AUCTION_REQUEST_GETBACKITEM\", nId, nil)",
        "end",
        "",
        "function UIAuctionHouse:RequestRefund(nId)",
        "    self:SendItemReq(\"emSCRIPT_PROTOCOL_AUCTION_REQUEST_REFUND\", nId, nil)",
        "end",
        "",
        "-- ---------------------------------------------------------------- tu C++ (nguoi bam)",
        "function UIAuctionHouse:OnAuctionIconClick(nToOpen)",
        "    if nToOpen == 1 then",
        "        OpenAuctionWindow()",
        "        self:SwitchToAuctionWindow(self.nCurTypeIndex)",
        "    else",
        "        CloseAuctionWindow()",
        "    end",
        "end",
        "",
        "function UIAuctionHouse:OnOpenFromServer(nType)",
        "    OpenAuctionWindow()",
        "    self:SwitchToAuctionWindow(nType or " + T + ".eType_PERSONAL)",
        "end",
        "",
        "function UIAuctionHouse:OnPageBtnClick(nType)",
        "    if nType == self.nCurTypeIndex then",
        "        return",
        "    end",
        "    self:SwitchToAuctionWindow(nType)",
        "end",
        "",
        "-- [A4] nut goc phai: Ca nhan = ky gui, The gioi = GM mo phien, Bang hoi = bang chu dat mon / thanh vien xem danh sach",
        "function UIAuctionHouse:OnPutOnClick(nType)",
        "    local h = OB_Create()",
        "    AuctionPutOnMode(1)",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nType or self.nCurTypeIndex)",
        "    ScriptProtocol:SendData(\"emSCRIPT_PROTOCOL_AUCTION_REQUEST_PUTON\", h)",
        "    OB_Release(h)",
        "end",
        "",
        "-- [A6] o gia + nut doi tien nam ngay trong hop dua vat pham; bam Dong y thi bao len truoc",
        "function UIAuctionHouse:OnSetPrice(nPrice, nCur)",
        "    local h = OB_Create()",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nPrice or 0)",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nCur or 1)",
        "    ScriptProtocol:SendData(\"emSCRIPT_PROTOCOL_AUCTION_REQUEST_SETPRICE\", h)",
        "    OB_Release(h)",
        "end",
        "",
        "function UIAuctionHouse:OnWindowClosed()",
        "end",
        "",
        "function UIAuctionHouse:OnSelect(szAct)",
        "    if self.szCurActivityName == szAct then",
        "        return",
        "    end",
        "    self.szCurActivityName = szAct",
        "    self.nCurPageIndex = 1",
        "    AuctionOnActivitySelect(self.nCurTypeIndex, szAct)",
        "    AuctionClearItemList()",
        "    self:ResetPageInfo(self.nCurTypeIndex, szAct)",
        "    if szAct ~= \"\" then",
        "        self:RequestActivityContent(szAct, 1)",
        "    end",
        "end",
        "",
        "function UIAuctionHouse:OnClickPrevPageBtn()",
        "    if self.nCurPageIndex <= 1 then",
        "        Msg2Player(\"" + V("Đã là trang đầu.") + "\")",
        "        return",
        "    end",
        "    self.nCurPageIndex = self.nCurPageIndex - 1",
        "    self:RequestActivityContent(self.szCurActivityName, self.nCurPageIndex)",
        "end",
        "",
        "function UIAuctionHouse:OnClickNextPageBtn()",
        "    local nMax = self:GetActivityMaxPage(self.szCurActivityName)",
        "    if self.nCurPageIndex + 1 > nMax then",
        "        Msg2Player(\"" + V("Đã là trang cuối.") + "\")",
        "        return",
        "    end",
        "    self.nCurPageIndex = self.nCurPageIndex + 1",
        "    self:RequestActivityContent(self.szCurActivityName, self.nCurPageIndex)",
        "end",
        "",
        "-- ---------------------------------------------------------------- tu may chu (protocol_def_c.lua)",
        "function UIAuctionHouse:ShowItem(nType, tbItem, bAdd)",
        "    local bMine = 0",
        "    if nType == " + T + ".eType_PERSONAL and tbItem.szBelongRole == GetName() then",
        "        bMine = 1",
        "    end",
        "    -- [A9] 6 so cua vat pham nam thang trong dong (khong con bang long)",
        "    if tbItem.nType == " + K + ".eType_ENGLISH then",
        "        if bAdd == 1 then",
        "            AuctionAddEnglishItem(nType, tbItem.nId, tbItem.nStartTime, tbItem.nGuaranteedPrice, tbItem.nRangePerOffer, tbItem.nCurrencyType,",
        "                tbItem.nMaxPrice, tbItem.nSelfPrice, tbItem.nRemainingTime, bMine,",
        "                tbItem.nG or 0, tbItem.nD or 0, tbItem.nP or 0, tbItem.nL or 0, tbItem.nS or 0, tbItem.nK or 0, tbItem.nCount or 1, tbItem.szName or \"\", tbItem.szCurrencyName or \"\")",
        "        else",
        "            AuctionSetEnglishItem(nType, tbItem.nId, tbItem.nStartTime, tbItem.nGuaranteedPrice, tbItem.nRangePerOffer, tbItem.nCurrencyType,",
        "                tbItem.nMaxPrice, tbItem.nSelfPrice, tbItem.nRemainingTime, bMine, tbItem.szCurrencyName or \"\")",
        "        end",
        "    else",
        "        if bAdd == 1 then",
        "            AuctionAddDutchItem(nType, tbItem.nId, tbItem.nStartTime, tbItem.nCurPrice, tbItem.nGuaranteedPrice, tbItem.nCurrencyType,",
        "                tbItem.nRemainingTime, tbItem.nNextPrice, tbItem.nTotalRemainingTime, bMine,",
        "                tbItem.nG or 0, tbItem.nD or 0, tbItem.nP or 0, tbItem.nL or 0, tbItem.nS or 0, tbItem.nK or 0, tbItem.nCount or 1, tbItem.szName or \"\", tbItem.szCurrencyName or \"\")",
        "        else",
        "            AuctionSetDutchItem(nType, tbItem.nId, tbItem.nStartTime, tbItem.nCurPrice, tbItem.nGuaranteedPrice, tbItem.nCurrencyType,",
        "                tbItem.nRemainingTime, tbItem.nNextPrice, tbItem.nTotalRemainingTime, bMine, tbItem.szCurrencyName or \"\")",
        "        end",
        "    end",
        "end",
        "",
        "function UIAuctionHouse:OnActivityListResponse(nType, tbList)",
        "    if not tbList then",
        "        return",
        "    end",
        "    for szName, tbInfo in tbList do",
        "        if szName == \"__money\" then",
        "            AuctionSetMoney(tbInfo.nTotalCount or 0, tbInfo.nStartTime or 0)",
        "        else",
        "            local a = self.tbActivityList[nType][szName]",
        "            if not a then",
        "                self.tbActivityList[nType][szName] = {nTotalCount = tbInfo.nTotalCount or 0, nStartTime = tbInfo.nStartTime or 0}",
        "                if nType == self.nCurTypeIndex then",
        "                    AuctionAddActivity(nType, szName, tbInfo.nStartTime or 0)",
        "                end",
        "            else",
        "                a.nTotalCount = tbInfo.nTotalCount or 0",
        "                a.nStartTime = tbInfo.nStartTime or 0",
        "            end",
        "        end",
        "    end",
        "    if nType == self.nCurTypeIndex and self.szCurActivityName == \"\" then",
        "        local szFirst = self:FirstActivity()",
        "        if szFirst ~= \"\" then",
        "            self:OnSelect(szFirst)",
        "        end",
        "    end",
        "end",
        "",
        "function UIAuctionHouse:OnActivityContentResponse(nType, szAct, tbPage, nTotal, nPage, bNeedUpdate)",
        "    local a = self.tbActivityList[nType][szAct]",
        "    if not a then",
        "        a = {nTotalCount = 0, nStartTime = 0}",
        "        self.tbActivityList[nType][szAct] = a",
        "    end",
        "    a.nTotalCount = nTotal or 0",
        "    a.tbItemList = a.tbItemList or {}",
        "    local nNow = GetLocalTime()",
        "    tbPage = tbPage or {}",
        "    for i, tbItem in tbPage do",
        "        tbItem.nRemainingTime = (tbItem.nRemainingTime or 0) + nNow",
        "        tbItem.nTotalRemainingTime = (tbItem.nTotalRemainingTime or 0) + nNow",
        "    end",
        "    a.tbItemList[nPage] = tbPage",
        "    if nType == self.nCurTypeIndex and self.szCurActivityName == szAct and self.nCurPageIndex == nPage then",
        "        AuctionClearItemList()",
        "        self:ResetPageInfo(nType, szAct)",
        "        for i, tbItem in tbPage do",
        "            self:ShowItem(nType, tbItem, 1)",
        "        end",
        "    end",
        "end",
        "",
        "function UIAuctionHouse:OnItemContentResponse(tbItem)",
        "end",
        "",
        "function UIAuctionHouse:OnMemberListResponse(nType, tbInfo)",
        "    if not tbInfo then",
        "        return",
        "    end",
        "    AuctionClearMemberList()",
        "    -- [A9] danh sach thanh vien den duoi dang chuoi \"ten,cap,chuc,online;...\"",
        "    local sz = tbInfo.szMemberList or \"\"",
        "    local n = 0",
        "    while sz ~= \"\" do",
        "        local a, b, one = strfind(sz, \"^([^;]*);?\")",
        "        if not a then",
        "            break",
        "        end",
        "        sz = strsub(sz, b + 1)",
        "        local _, _, szTen, szCap, szChuc, szOn = strfind(one, \"^(.-),(%d+),(%d+),(%d+)$\")",
        "        if szTen then",
        "            n = n + 1",
        "            AuctionAddActivityMember(szTen, tonumber(szCap) or 0, tonumber(szChuc) or 0, tonumber(szOn) or 0)",
        "        end",
        "    end",
        "    AuctionSetSalaryAndCount(tbInfo.nCount or n, tbInfo.nSalary or 0)",
        "end",
        "",        "function UIAuctionHouse:OnOfferPriceResponse(nType, szAct, nId, nNewPrice)",
        "    local tbItem = self:GetItem(szAct, nId)",
        "    if tbItem then",
        "        tbItem.nSelfPrice = nNewPrice",
        "        if tbItem.nType == " + K + ".eType_DUTCH then",
        "            PopBlackTips(\"" + V("Chúc mừng đại hiệp đã mua được ") + "\"..(tbItem.szName or \"\")..\"" + V(", vật phẩm gửi vào hộp thư.") + "\")",
        "        else",
        "            tbItem.nMaxPrice = nNewPrice",
        "            if nType == self.nCurTypeIndex and self.szCurActivityName == szAct then",
        "                self:ShowItem(nType, tbItem, 0)",
        "            end",
        "        end",
        "    end",
        "end",
        "",
        "function UIAuctionHouse:OnRefundResponse(nType, szAct, nId, nOk)",
        "end",
        "",
        "function UIAuctionHouse:OnNewActivityEvent(nType, szAct, nStart)",
        "    if not self.tbActivityList[nType][szAct] then",
        "        self.tbActivityList[nType][szAct] = {nTotalCount = 0, nStartTime = nStart or 0}",
        "        if self.nCurTypeIndex == nType then",
        "            AuctionAddActivity(nType, szAct, nStart or 0)",
        "        end",
        "    end",
        "    NewAuctionEventArrival()",
        "end",
        "",
        "function UIAuctionHouse:OnEndActivityEvent(nType, szAct)",
        "    if self.tbActivityList[nType][szAct] then",
        "        AuctionDelActivity(nType, szAct)",
        "        self.tbActivityList[nType][szAct] = nil",
        "        if self.nCurTypeIndex == nType and self.szCurActivityName == szAct then",
        "            AuctionClearItemList()",
        "            self.szCurActivityName = \"\"",
        "            self:ResetPageInfo(nType, \"\")",
        "        end",
        "    end",
        "end",
        "",
        "function UIAuctionHouse:OnNewItemEvent(nType, szAct, nNewId, nTotal)",
        "    local a = self.tbActivityList[nType][szAct]",
        "    if not a then",
        "        a = {nTotalCount = 0, nStartTime = 0}",
        "        self.tbActivityList[nType][szAct] = a",
        "        if self.nCurTypeIndex == nType then",
        "            AuctionAddActivity(nType, szAct, 0)",
        "        end",
        "    end",
        "    a.nTotalCount = nTotal or 0",
        "    if self.nCurTypeIndex == nType and self.szCurActivityName == szAct and AuctionUiIsOpen() == 1 then",
        "        self:RequestActivityContent(szAct, self.nCurPageIndex)",
        "    end",
        "    NewAuctionEventArrival()",
        "end",
        "",
        "function UIAuctionHouse:OnEndItemEvent(nType, szAct, nEndId, nTotal)",
        "    local a = self.tbActivityList[nType][szAct]",
        "    if a then",
        "        a.nTotalCount = nTotal or 0",
        "    end",
        "    if self.nCurTypeIndex == nType and self.szCurActivityName == szAct then",
        "        AuctionEndItem(nType, nEndId)",
        "        self:ResetPageInfo(nType, szAct)",
        "        if AuctionUiIsOpen() == 1 then",
        "            self:RequestActivityContent(szAct, self.nCurPageIndex)",
        "        end",
        "    end",
        "end",
        "",
        "function UIAuctionHouse:OnPriceChangeEvent(nType, szAct, nId, nNewPrice, nNextPrice, nRemaining, nCurFloat)",
        "    local tbItem = self:GetItem(szAct, nId)",
        "    if tbItem then",
        "        tbItem.nMaxPrice = nNewPrice",
        "        tbItem.nCurPrice = nNewPrice",
        "        tbItem.nNextPrice = nNextPrice",
        "        tbItem.nRemainingTime = (nRemaining or 0) + GetLocalTime()",
        "        if nType == self.nCurTypeIndex and self.szCurActivityName == szAct then",
        "            self:ShowItem(nType, tbItem, 0)",
        "        end",
        "    end",
        "end",
        "",
        "SetAuctionIconVisible(0)",
        "",
    ]
    return E.join(L)


# ============================================================================ va cac tep co san
PROTO_NAMES = [
    "emSCRIPT_PROTOCOL_AUCTION_OFFERPRICERET", "emSCRIPT_PROTOCOL_AUCTION_REFUNDRET",
    "emSCRIPT_PROTOCOL_AUCTION_ACTIVITYLIST", "emSCRIPT_PROTOCOL_AUCTION_ACTIVITYINFO",
    "emSCRIPT_PROTOCOL_AUCTION_ITEMINFO", "emSCRIPT_PROTOCOL_AUCTION_MEMBERLIST",
    "emSCRIPT_PROTOCOL_AUCTION_NEWACTIVITY", "emSCRIPT_PROTOCOL_AUCTION_ENDACTIVITY",
    "emSCRIPT_PROTOCOL_AUCTION_NEWITEM", "emSCRIPT_PROTOCOL_AUCTION_ENDITEM",
    "emSCRIPT_PROTOCOL_AUCTION_TPRICECHANGE", "emSCRIPT_PROTOCOL_AUCTION_OPENWND",
    "emSCRIPT_PROTOCOL_AUCTION_REQUEST_OFFERENGLISHPRICE", "emSCRIPT_PROTOCOL_AUCTION_REQUEST_OFFERDUTCHPRICE",
    "emSCRIPT_PROTOCOL_AUCTION_REQUEST_REFUND", "emSCRIPT_PROTOCOL_AUCTION_REQUEST_ACTIVITYLIST",
    "emSCRIPT_PROTOCOL_AUCTION_REQUEST_ACTIVITYCONTENT", "emSCRIPT_PROTOCOL_AUCTION_REQUEST_ITEMCONTENT",
    "emSCRIPT_PROTOCOL_AUCTION_REQUEST_MEMBERLIST", "emSCRIPT_PROTOCOL_AUCTION_REQUEST_GETBACKITEM",
    "emSCRIPT_PROTOCOL_AUCTION_REQUEST_PUTON",
    "emSCRIPT_PROTOCOL_AUCTION_REQUEST_SETPRICE",
]

S2C = [  # (enum, ham, kieu tham so)
    ("emSCRIPT_PROTOCOL_AUCTION_OFFERPRICERET", "UIAuctionHouse:OnOfferPriceResponse", "{OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER, OBJTYPE_NUMBER}"),
    ("emSCRIPT_PROTOCOL_AUCTION_REFUNDRET", "UIAuctionHouse:OnRefundResponse", "{OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER, OBJTYPE_NUMBER}"),
    ("emSCRIPT_PROTOCOL_AUCTION_ACTIVITYLIST", "UIAuctionHouse:OnActivityListResponse", "{OBJTYPE_NUMBER, OBJTYPE_TABLE}"),
    ("emSCRIPT_PROTOCOL_AUCTION_ACTIVITYINFO", "UIAuctionHouse:OnActivityContentResponse", "{OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_TABLE, OBJTYPE_NUMBER, OBJTYPE_NUMBER, OBJTYPE_NUMBER}"),
    ("emSCRIPT_PROTOCOL_AUCTION_ITEMINFO", "UIAuctionHouse:OnItemContentResponse", "{OBJTYPE_TABLE}"),
    ("emSCRIPT_PROTOCOL_AUCTION_MEMBERLIST", "UIAuctionHouse:OnMemberListResponse", "{OBJTYPE_NUMBER, OBJTYPE_TABLE}"),
    ("emSCRIPT_PROTOCOL_AUCTION_NEWACTIVITY", "UIAuctionHouse:OnNewActivityEvent", "{OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER}"),
    ("emSCRIPT_PROTOCOL_AUCTION_ENDACTIVITY", "UIAuctionHouse:OnEndActivityEvent", "{OBJTYPE_NUMBER, OBJTYPE_STRING}"),
    ("emSCRIPT_PROTOCOL_AUCTION_NEWITEM", "UIAuctionHouse:OnNewItemEvent", "{OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER, OBJTYPE_NUMBER}"),
    ("emSCRIPT_PROTOCOL_AUCTION_ENDITEM", "UIAuctionHouse:OnEndItemEvent", "{OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER, OBJTYPE_NUMBER}"),
    ("emSCRIPT_PROTOCOL_AUCTION_TPRICECHANGE", "UIAuctionHouse:OnPriceChangeEvent", "{OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER, OBJTYPE_NUMBER, OBJTYPE_NUMBER, OBJTYPE_NUMBER, OBJTYPE_NUMBER}"),
    ("emSCRIPT_PROTOCOL_AUCTION_OPENWND", "UIAuctionHouse:OnOpenFromServer", "{OBJTYPE_NUMBER}"),
]

C2S = [
    ("emSCRIPT_PROTOCOL_AUCTION_REQUEST_OFFERENGLISHPRICE", "AUC_OnRequestOfferEnglish", "{OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER, OBJTYPE_NUMBER}"),
    ("emSCRIPT_PROTOCOL_AUCTION_REQUEST_OFFERDUTCHPRICE", "AUC_OnRequestOfferDutch", "{OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER, OBJTYPE_NUMBER}"),
    ("emSCRIPT_PROTOCOL_AUCTION_REQUEST_REFUND", "AUC_OnRequestRefund", "{OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER}"),
    ("emSCRIPT_PROTOCOL_AUCTION_REQUEST_ACTIVITYLIST", "AUC_OnRequestActivityList", "{OBJTYPE_NUMBER, OBJTYPE_NUMBER}"),
    ("emSCRIPT_PROTOCOL_AUCTION_REQUEST_ACTIVITYCONTENT", "AUC_OnRequestActivityContent", "{OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER, OBJTYPE_TABLE}"),
    ("emSCRIPT_PROTOCOL_AUCTION_REQUEST_ITEMCONTENT", "AUC_OnRequestItemContent", "{OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER}"),
    ("emSCRIPT_PROTOCOL_AUCTION_REQUEST_MEMBERLIST", "AUC_OnRequestMemberList", "{OBJTYPE_STRING}"),
    ("emSCRIPT_PROTOCOL_AUCTION_REQUEST_GETBACKITEM", "AUC_OnRequestGetBack", "{OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER}"),
    ("emSCRIPT_PROTOCOL_AUCTION_REQUEST_PUTON", "AUC_OnRequestPutOn", "{OBJTYPE_NUMBER}"),
    ("emSCRIPT_PROTOCOL_AUCTION_REQUEST_SETPRICE", "AUC_OnRequestSetPrice", "{OBJTYPE_NUMBER, OBJTYPE_NUMBER}"),
]


def _strip_auction(s, e):
    """[A4] bo moi dong cu cua khoi dau gia (ten giao thuc + muc dang ky) de chen lai tron bo"""
    out = []
    skip_block = 0
    for line in s.split(e):
        t = line.strip()
        if "emSCRIPT_PROTOCOL_AUCTION" in line or (MARK in line and "dau gia" in line):
            # muc dang ky nhieu dong cua protocol_def_c: bo tu dong "{" truoc do
            if out and out[-1].strip() == "{":
                out.pop()
                skip_block = 1
            continue
        if skip_block:
            # bo not phan con lai cua muc: duong dan, ten ham, kieu tham so, "},"
            if t.startswith('"') or t.startswith("{OBJTYPE") or t == "}," or t.startswith("},"):
                if t == "}," or t.startswith("},"):
                    skip_block = 0
                continue
            skip_block = 0
        out.append(line)
    return e.join(out)


def patch_protocol():
    """them 20 ten vao protocol.lua (server + client giong nhau)"""
    for root, mirror in ((SV, MIRROR_S), (CL, MIRROR_C)):
        rel = r"script\protocol.lua"
        s = io.open(os.path.join(root, rel), "r", encoding="latin-1", newline="").read()
        e = eol(s)
        s = _strip_auction(s, e)
        anchor = '\t"emSCRIPT_PROTOCOL_MAIL_REQUEST_AUTODELETE",'
        assert s.count(anchor) == 1, "protocol.lua neo MAIL_REQUEST_AUTODELETE"
        add = anchor + e + "\t-- " + MARK + " dau gia (2.0 + OPENWND rieng cua JX1)" + e + e.join('\t"%s",' % n for n in PROTO_NAMES)
        s = s.replace(anchor, add)
        wr_both(rel, s, root, mirror)


def patch_def_gs():
    rel = r"script\script_protocol\protocol_def_gs.lua"
    s = rd(os.path.join(SV, rel))
    e = eol(s)
    s = _strip_auction(s, e)
    lines = ["\t-- " + MARK + " dau gia -> \\script\\auction_house\\auction_manager.lua"]
    for en, fn, ty in C2S:
        lines.append('\t{ "%s", "%s", "%s", %s },' % (en, lp("script", "auction_house", "auction_manager.lua"), fn, ty))
    # chen truoc dau } cuoi cung
    i = s.rstrip().rfind("}")
    assert i > 0
    s = s[:i] + e.join(lines) + e + s[i:]
    wr_s(rel, s)


def patch_def_c():
    rel = r"script\script_protocol\protocol_def_c.lua"
    s = io.open(os.path.join(CL, rel), "r", encoding="latin-1", newline="").read()
    e = eol(s)
    s = _strip_auction(s, e)
    lines = ["\t-- " + MARK + " dau gia -> \\script\\ui\\uiauction_house.lua"]
    for en, fn, ty in S2C:
        lines.append("\t{" + e + '\t\t"%s",' % en + e + '\t\t"%s",' % lp("script", "ui", "uiauction_house.lua") + e + '\t\t"%s",' % fn + e + "\t\t%s," % ty + e + "\t},")
    i = s.rstrip().rfind("}")
    assert i > 0
    s = s[:i] + e.join(lines) + e + s[i:]
    wr_c(rel, s)


def patch_dichquan():
    rel = r"script\global\npcchucnang\dichquan.lua"
    s = rd(os.path.join(SV, rel))
    if MARK in s:
        print("  da co dichquan")
        return
    e = eol(s)
    anchor = '\t\t"' + V("Nhiệm vụ Tín Sứ") + '/especiallymessenger",'
    assert s.count(anchor) == 1, "dichquan: neo Tin Su"
    s = s.replace(anchor, anchor + e + '\t\t"' + V("Đấu giá (ký gửi, phiên bang, mở cửa sổ)") + '/auc_npc_main",\t-- ' + MARK)
    # Include o dau tep
    s = 'Include("' + lp("script", "auction_house", "auction_npc.lua") + '")\t-- ' + MARK + e + s
    # Say(...,2, -> 3 lua chon
    s = rep1(s, '\tSay("' + V("Ngươi tìm ta có việc gì?") + '",2,', '\tSay("' + V("Ngươi tìm ta có việc gì?") + '",3,', "dichquan so lua chon")
    wr_s(rel, s)


def patch_lenhbai():
    rel = r"script\item\lenhbaiadmin.lua"
    s = rd(os.path.join(SV, rel))
    if MARK in s:
        print("  da co lenhbaiadmin")
        return
    e = eol(s)
    anchor = '\t\t-- "' + V("Gửi thư thử: tiền, xu, exp") + '/mailtest1",\t-- [MAIL 03/09]'
    n = s.count(anchor)
    if n != 1:
        # neo ngan hon
        m = re.search(r'\t\t-- "[^"\r\n]*/mailtest1",[^\r\n]*', s)
        assert m, "lenhbaiadmin: neo mailtest1"
        anchor = m.group(0)
    s = s.replace(anchor, anchor + e + '\t\t"' + V("Đấu giá: mở phiên thế giới (GM)") + '/auc_admin_world",\t-- ' + MARK)
    s = 'Include("' + lp("script", "auction_house", "auction_npc.lua") + '")\t-- ' + MARK + e + s
    wr_s(rel, s)


def _setkey(txt, sec, key, val):
    """[A5] doi (hoac them) mot khoa trong muc [sec] cua noi dung ini"""
    e = chr(13) + chr(10)
    out = []
    cur = None
    done = False
    for line in txt.split(e):
        st = line.strip()
        if st.startswith("[") and st.endswith("]"):
            if cur == sec and not done:
                out.append(key + "=" + str(val))
                done = True
            cur = st[1:-1]
        elif cur == sec and st.startswith(key + "="):
            line = key + "=" + str(val)
            done = True
        out.append(line)
    if cur == sec and not done:
        out.append(key + "=" + str(val))
    return e.join(out)


def _relabel(txt, sec, label):
    """[A4] doi dong Label= trong mot muc [sec] cua noi dung ini (label None = khong doi gi)"""
    if label is None:
        return txt
    out = []
    cur = None
    for line in txt.split(chr(13) + chr(10)):
        st = line.strip()
        if st.startswith("[") and st.endswith("]"):
            cur = st[1:-1]
        elif cur == sec and st.startswith("Label="):
            line = "Label=" + label
        out.append(line)
    return (chr(13) + chr(10)).join(out)


def copy_ini():
    """13 ini + item icon -> client ui\\Ui3\\auction\\ ; Image= them dau \\ (bai hoc hop thu: g_FileName2Id khong chuan hoa)"""
    names = ["auction_manager", "auction_icon", "auction_page_personal", "auction_page_tong", "auction_page_world",
             "auction_item_dutch_header", "auction_item_english_header", "auction_item_personal_header",
             "auction_item_personal_myself_header", "auction_activity_header", "auction_member_header",
             "auction_memberlist_info", "auction_item_icon"]
    for n in names:
        src = os.path.join(INI_SRC, n + ".ini")
        if not os.path.exists(src):
            src2 = os.path.join(INI_KEEP, n + ".ini")
            if os.path.exists(src2):
                src = src2
            else:
                print("  THIEU ini:", n)
                continue
        s = rd(src)
        out = []
        for line in s.replace("\r\n", "\n").split("\n"):
            if line.startswith("Image=") and len(line) > 6 and line[6] not in ("\\", "/"):
                line = "Image=" + BS + line[6:]
            out.append(line)
        txt = "\r\n".join(out)
        # sprite "小按钮二字" khong co trong pak du an -> dung "小按钮四字" (co) cho nut Bao gia
        txt = txt.replace("小按钮二字.spr".encode("gbk").decode("latin-1"), "小按钮四字.spr".encode("gbk").decode("latin-1"))
        # [A5 04/09] sua 4 loi hien thi chu bao:
        # 1) ba the cua cua so chinh: nhan goc qua dai cho o rong 64 -> cat con "Dau gia ..."
        if n == "auction_manager":
            txt = _relabel(txt, "AuctionTong", V("Bang hội"))
            txt = _relabel(txt, "AuctionWorld", V("Thế giới"))
            txt = _relabel(txt, "AuctionPersonal", V("Cá nhân"))
            txt = _setkey(txt, "AuctionTong", "Left", 10)
            txt = _setkey(txt, "AuctionWorld", "Left", 78)
            txt = _setkey(txt, "AuctionPersonal", "Left", 146)
            for sec in ("AuctionTong", "AuctionWorld", "AuctionPersonal"):
                txt = _setkey(txt, sec, "Width", 66)
            txt = _setkey(txt, "Title", "Left", 300)
            txt = _setkey(txt, "Title", "Width", 120)
        # 2) tieu de trang bi cat ("Loai dau g") + 3) hai dong tien DE LEN NHAU (Top 13 va 19)
        if n in ("auction_page_personal", "auction_page_world", "auction_page_tong"):
            txt = _setkey(txt, "Title", "Width", 150)
            txt = _relabel(txt, "Title", None)
            txt = _setkey(txt, "BindingSilverTitle", "Text", V("Ngân lượng"))
            txt = _setkey(txt, "BindingGoldTitle", "Text", "Xu")
            txt = _setkey(txt, "BindingSilverTitle", "Top", 5)
            txt = _setkey(txt, "BindingSilverValue", "Top", 5)
            txt = _setkey(txt, "BindingGoldTitle", "Top", 23)
            txt = _setkey(txt, "BindingGoldValue", "Top", 23)
            txt = _setkey(txt, "BindingSilverTitle", "Width", 70)
            txt = _setkey(txt, "BindingGoldTitle", "Width", 70)
            txt = _setkey(txt, "BindingSilverValue", "Left", 265)
            txt = _setkey(txt, "BindingGoldValue", "Left", 265)
            txt = _setkey(txt, "tip", "Text", "")
        # [A4] doi nhan nut goc phai: khong con NPC, nut nay la loi vao dat ban
        if n == "auction_page_personal":
            txt = _relabel(txt, "AuctionPersonalTip", V("Ký gửi vật phẩm"))
        elif n == "auction_page_world":
            txt = _relabel(txt, "AuctionWorldTip", V("Mở phiên (GM)"))
        elif n == "auction_page_tong":
            txt = _relabel(txt, "seeMemberList", V("Đặt món / Thành viên"))
        if n == "auction_icon":
            # bieu tuong: nam ngay duoi bieu tuong thu (mail_icon.ini Left=765 Top=296 -> dau gia Top=322)
            # [A10 04/09] chu: "cho icon dau gia xuong giua bau cua va mail"
            # Bau Cua = UiPlayerBar.ini [SpringGame] Top=243 (cao 50) -> dau gia 296, thu doi xuong 322
            txt = _setkey(txt, "Main", "Left", 765)
            txt = _setkey(txt, "Main", "Top", 296)
        wr_c(os.path.join("ui", "Ui3", "auction", n + ".ini"), txt)
        # luu ban goc lau dai
        if not CHECK:
            if not os.path.isdir(INI_KEEP):
                os.makedirs(INI_KEEP)
            shutil.copy(src, os.path.join(INI_KEEP, n + ".ini"))


def patch_mail_generator():
    """p3_lua.py: dinh kem aucitem:<id> (vat pham dau gia giu nguyen thuoc tinh) roi sinh lai mailmanager.lua"""
    p = r"D:\GAMEDEVNEW_wt_mail\ReverseTools\mail\p3_lua.py"
    s = io.open(p, "r", encoding="utf-8", newline="").read()
    if "aucitem" in s:
        print("  p3_lua.py da co aucitem")
    else:
        nl = "\r\n" if "\r\n" in s else "\n"
        # 1) ParseAward
        old = '        "            elseif kind == \\"task\\" then",'
        assert s.count(old) == 1, "p3 ParseAward task"
        new = nl.join([
            '        "            elseif kind == \\"aucitem\\" then",',
            '        "                -- [DAUGIA 04/09] aucitem:<id> = mon trong bang auction_item (giu nguyen thuoc tinh, AUC_GiveRec)",',
            '        "                local nAid = tonumber(val) or 0",',
            '        "                if nAid > 0 then",',
            '        "                    local ai = AUC_Get(nAid)",',
            '        "                    local pr = {0, 0, 0, 0, 0, 0}",',
            '        "                    if ai then",',
            '        "                        local kx = 0",',
            '        "                        local sd = ai.desc",',
            '        "                        while kx < 6 do",',
            '        "                            local a1, b1, num = strfind(sd, \\"^(%-?%d+),?\\")",',
            '        "                            if not a1 then break end",',
            '        "                            kx = kx + 1",',
            '        "                            pr[kx] = tonumber(num) or 0",',
            '        "                            sd = strsub(sd, b1 + 1)",',
            '        "                        end",',
            '        "                    end",',
            '        "                    tinsert(tb, {szKind = \\"aucitem\\", nAucId = nAid, nGenre = pr[1], nDetail = pr[2], nParticular = pr[3],",',
            '        "                        nLevel = pr[4], nSeries = pr[5], nLuck = pr[6], nCount = 1, nCells = (ai and ai.cells) or 6})",',
            '        "                end",',
            old,
        ])
        s = s.replace(old, new)
        # 2) AwardInfo: hien nhu item (client dung lai bieu tuong tu 6 so)
        old2 = '        "        if a.szKind == \\"item\\" then",' + nl + '        "            tinsert(tb, a)",'
        assert s.count(old2) == 1, "p3 AwardInfo item"
        s = s.replace(old2, '        "        if a.szKind == \\"item\\" or a.szKind == \\"aucitem\\" then",' + nl + '        "            tinsert(tb, a)",')
        # 3) GiveAward
        old3 = '        "        if a.szKind == \\"item\\" or a.szKind == \\"gold\\" then",' + nl + '        "            MailManager_GiveItem(a)",'
        assert s.count(old3) == 1, "p3 GiveAward"
        s = s.replace(old3, nl.join([
            '        "        if a.szKind == \\"aucitem\\" then",',
            '        "            local ai = AUC_Get(a.nAucId)",',
            '        "            if ai and AUC_GiveRec(ai.rec) > 0 then",',
            '        "                GhiLog(\\"MAIL\\", format(\\"%s nhan mon dau gia id %d (%s)\\", GetName(), a.nAucId, ai.name))",',
            '        "            else",',
            '        "                GhiLog(\\"MAIL\\", format(\\"LOI: %s KHONG nhan duoc mon dau gia id %d\\", GetName(), a.nAucId))",',
            '        "            end",',
            old3,
        ]))
        # 4) kiem o trong tui
        old4 = '        "        elseif a.szKind == \\"gold\\" then",' + nl + '        "            nCells = nCells + a.nCount * 6",'
        assert s.count(old4) == 1, "p3 cells"
        s = s.replace(old4, '        "        elseif a.szKind == \\"aucitem\\" then",' + nl + '        "            nCells = nCells + (a.nCells or 6)",' + nl + old4)
        # 5) MAILMGR_ACTIVITY: daugia
        old5 = '        "    bangluong = \\"" + V("Bang hội") + "\\",",'
        assert s.count(old5) == 1, "p3 activity"
        s = s.replace(old5, old5 + nl + '        "    daugia    = \\"" + V("Chưởng Quầy Khu Đấu Giá") + "\\",",')
        io.open(p, "w", encoding="utf-8", newline="").write(s)
        print("  p3_lua.py: them aucitem")
    if not CHECK:
        os.system('python "%s"' % p)


def drop_npc():
    """[A4] khong con duong NPC: xoa auction_npc.lua o ca hai noi"""
    for root in (SV, MIRROR_S):
        pth = os.path.join(root, r"script\auction_house\auction_npc.lua")
        if os.path.exists(pth):
            if CHECK:
                print("  (check) xoa", pth)
            else:
                os.remove(pth)
                print("  xoa:", pth)


def _revert(rel, root, mirror):
    """go moi dong mang dau MARK + dong Include auction_npc"""
    p = os.path.join(root, rel)
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    if MARK not in s:
        print("  da sach:", rel)
        return
    e = eol(s)
    out = []
    for line in s.split(e):
        if MARK in line and ("auc_npc_main" in line or "auc_admin_world" in line or "auction_npc.lua" in line):
            continue
        out.append(line)
    s2 = e.join(out)
    # tra so lua chon cua Say ve 2 (dichquan)
    s2 = s2.replace('Say("' + V("Ngươi tìm ta có việc gì?") + '",3,', 'Say("' + V("Ngươi tìm ta có việc gì?") + '",2,')
    if MARK in s2:
        raise AssertionError(rel + ": van con dau vet " + MARK)
    wr_both(rel, s2, root, mirror)


def revert_dichquan():
    _revert(r"script\global\npcchucnang\dichquan.lua", SV, MIRROR_S)


def revert_lenhbai():
    _revert(r"script\item\lenhbaiadmin.lua", SV, MIRROR_S)


def main():
    steps = [
        ("def (server)", lambda: wr_s(r"script\auction_house\auction_def.lua", build_def())),
        ("def (client)", lambda: wr_c(r"script\auction_house\auction_def.lua", build_def())),
        ("manager", lambda: wr_s(r"script\auction_house\auction_manager.lua", build_manager())),
        ("go npc", drop_npc),
        ("poll", lambda: wr_s(r"script\auction_house\auctionpoll.lua", build_poll())),
        ("client ui", lambda: wr_c(r"script\ui\uiauction_house.lua", build_client_ui())),
        ("protocol.lua", patch_protocol),
        ("protocol_def_gs", patch_def_gs),
        ("protocol_def_c", patch_def_c),
        ("tra dichquan", revert_dichquan),
        ("tra lenhbaiadmin", revert_lenhbai),
        ("ini", copy_ini),
        ("mail aucitem", patch_mail_generator),
    ]
    ok = True
    for name, fn in steps:
        print("==", name)
        try:
            fn()
        except AssertionError as ex:
            ok = False
            print("  LOI:", ex)
    print("XONG" if ok else "CO LOI")


if __name__ == "__main__":
    main()
