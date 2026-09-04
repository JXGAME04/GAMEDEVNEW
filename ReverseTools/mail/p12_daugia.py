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
        "AUCTION_DEF.nPersonalPutOnCost     = 10             -- tien coc ky gui (%) tinh tren GIA CO BAN",
        "-- [A20 04/09 chu chot] Tien coc HOAN lai khi ban duoc va khi het han e; CHI MAT khi nguoi ban",
        "-- tu rut mon ve. Kem tran de mot lan go nham gia khong tru sach vi:",
        "AUCTION_DEF.nMaxDepositXu          = 20000",
        "AUCTION_DEF.nMaxDepositMoney       = 20000000",
        "-- [A22] PHAI bang AUCUI_ROW_COUNT (so hang ve duoc) - khong thi mon thua nam duoi thanh cuon",
        "-- ma nguoi choi khong biet. Tran that la bo dem goi script 4096 byte: moi dong ~665 byte",
        "-- sau khi them chuoi mo ta vat pham, nen 3 dong ~2000 byte con rong rai; qua 5 dong la sat.",
        "AUCTION_DEF.nMaxItemPerPage        = 3",
        "AUCTION_DEF.nMaxItemPerSeller      = 5              -- mot nguoi ky gui toi da",
        "AUCTION_DEF.nPersonalDuration      = 24 * 60 * 60   -- ky gui 24 gio",
        "-- [A14] Ky gui CUNG la dau gia kieu Ha Lan (dung ban goc 2.0: nPersonalFloatTimes = 1):",
        "-- mo ban o 150% gia nguoi ban muon, sau nua thoi gian ha ve dung gia do.",
        "-- Nho vay Mua ngay va Gia san khac nhau: ai muon chac thi mua ngay, ai chiu cho thi mua re.",
        "AUCTION_DEF.nPersonalFloatTimes    = 1",
        "AUCTION_DEF.nPersonalFloatInterval = 12 * 60 * 60   -- ha gia dung mot lan, o nua chang",
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
        "-- [B3] [PlayerIndex] = {nType, szAct, szName, nTong}: luu TEN de biet o nguoi choi da bi cap lai",
        "-- cho nguoi khac, va luu BANG de khong gui goi phien bang A cho nguoi bang B.",
        "AUC_Viewers = AUC_Viewers or {}",
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
        "-- [A19] AUC_List cat theo trang (nMax, nAfterId). Truoc day goi dung MOT lan voi 200 nen",
        "-- qua 200 mon la tong so va so trang bi cat AM THAM. Nay lap theo con tro id.",
        "AUC_ROWS_MAX = 2000",
        "function AUC_Rows(nType, nTong)",
        "    local tb = {}",
        "    local nAfter = 0",
        "    while getn(tb) < AUC_ROWS_MAX do",
        "        local lo = AUC_List(nType, 200, nAfter)",
        "        local n = getn(lo)",
        "        if n <= 0 then",
        "            break",
        "        end",
        "        for i = 1, n do",
        "            tinsert(tb, lo[i])",
        "        end",
        "        nAfter = lo[n].id",
        "        if n < 200 then",
        "            break",
        "        end",
        "    end",
        "    if getn(tb) >= AUC_ROWS_MAX then",
        "        AUC_Log(format(\"CANH BAO: loai %d da cham tran %d dong - danh sach bi cat\", nType, AUC_ROWS_MAX))",
        "    end",
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
        "    -- [A11] MAY CHU tinh san \"mon cua chinh minh\": client KHONG co ham GetName",
        "    -- (GetName chi dang ky trong khoi #ifdef _SERVER) - goi ben client nem loi va dut vong ve.",
        "    -- [A12] bo szCurrencyName + szBelongRole cho nhe goi (bo dem ObjBuffer chi 4096 byte).",
        "    tb.bMine = 0",
        "    if szMe ~= \"\" and r.seller == szMe then",
        "        tb.bMine = 1",
        "    end",
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
        "    -- [A16] Bo mat THAT cua mon nam trong cot item_rec chu khong phai item_desc (chi 6 so).",
        "    -- Hoang kim nhan dien bang CAP (nature = NATURE_GOLD, row); thieu no thi client dung lai",
        "    -- mot mon thuong khac han. Doc thang tu rec nen CA cac mon dat ban tu truoc cung hien dung,",
        "    -- khong phai doi cot hay chuyen doi CSDL.",
        "    local szInfo, nStk = AUC_RecDesc(r.rec)",
        "    tb.szInfo = szInfo or \"\"",
        "    tb.nCount = nStk or 1",
        "    if tb.nCount < 1 then",
        "        tb.nCount = 1",
        "    end",
        "    tb.nGuaranteedPrice = r.guar",
        "    tb.nCurPrice = r.cur",
        "    -- [A17] chua ai tra gia thi KHONG duoc bao \"gia cao nhat\": client lay nMaxPrice + buoc gia",
        "    -- lam gia de nghi, nen nguoi tra dau tien bi day len thua mot buoc.",
        "    tb.nMaxPrice = 0",
        "    if r.buyer ~= \"\" then",
        "        tb.nMaxPrice = r.cur",
        "    end",
        "    -- [A20b] gia MUA NGAY (chi dong ky gui ca nhan moi co) - client ve nut \"Mua ngay\"",
        "    tb.nBuyNow = 0",
        "    if r.atype == " + P + ".tbAuctionTypeEnum.eType_PERSONAL and r.kind == " + P + ".tbItemTypeEnum.eType_ENGLISH then",
        "        tb.nBuyNow = r.base or 0",
        "    end",
        "    -- buoc moi luot = 10% GIA CO BAN (dung y chu chot 04/09)",
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
        "    -- [A14b] ky gui ha THANG ve gia san mot nhip, phien the gioi/bang hoi ha 10% moi nhip.",
        "    -- O \"Gia ke tiep\" phai bao dung cai vong quet se lam, khong thi nguoi mua thay mot dang tra mot neo.",
        "    local bCaNhan = (r.atype == " + P + ".tbAuctionTypeEnum.eType_PERSONAL)",
        "    if bCaNhan then",
        "        tb.nFloatInterval = " + P + ".nPersonalFloatInterval",
        "        tb.nTotalFloatTimes = " + P + ".nPersonalFloatTimes",
        "        tb.nCurFloatTimes = " + P + ".nPersonalFloatTimes - r.dropleft",
        "    else",
        "        tb.nFloatInterval = " + P + ".nDutchFloatInterval",
        "        tb.nTotalFloatTimes = " + P + ".nDutchFloatTimes",
        "        tb.nCurFloatTimes = " + P + ".nDutchFloatTimes - r.dropleft",
        "    end",
        "    if r.kind == " + P + ".tbItemTypeEnum.eType_DUTCH and r.dropleft > 0 and r.nextdrop > 0 then",
        "        tb.nRemainingTime = r.nextdrop - nNow",
        "        local nNext = r.guar",
        "        if not bCaNhan then",
        "            nNext = floor(r.cur * (1 - " + P + ".nDutchFloatRange))",
        "            if nNext < r.guar then",
        "                nNext = r.guar",
        "            end",
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
        "-- nTongLoc: chi phien bang hoi moi dung (chi gui cho nguoi CUNG bang)",
        "function AUC_Broadcast(nType, szAct, szEnum, fnPush, nTongLoc)",
        "    for nIdx, v in AUC_Viewers do",
        "        if v.nType == nType and (nTongLoc == nil or nTongLoc == 0 or v.nTong == nTongLoc)",
        "            and (szAct == nil or v.szAct == szAct or v.szAct == \"\") then",
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
        "-- [B3] PHAI truyen nTong that: AUC_Rows loc theo bang, truyen 0 thi phien bang luon dem = 0",
        "function AUC_NotifyNewItem(nType, szAct, nId, nTong)",
        "    local nCount = 0",
        "    local tb = AUC_Activities(nType, nTong or 0)",
        "    if tb[szAct] then",
        "        nCount = tb[szAct].nTotalCount",
        "    end",
        "    AUC_Broadcast(nType, szAct, \"emSCRIPT_PROTOCOL_AUCTION_NEWITEM\", function(h)",
        "        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nType)",
        "        ObjBuffer:PushByType(h, OBJTYPE_STRING, %szAct)",
        "        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nId)",
        "        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nCount)",
        "    end, nTong)",
        "end",
        "",
        "-- [B3] PHAI truyen nTong that: AUC_Rows loc theo bang, truyen 0 thi phien bang luon dem = 0",
        "function AUC_NotifyEndItem(nType, szAct, nId, nTong)",
        "    local nCount = 0",
        "    local tb = AUC_Activities(nType, nTong or 0)",
        "    if tb[szAct] then",
        "        nCount = tb[szAct].nTotalCount",
        "    end",
        "    AUC_Broadcast(nType, szAct, \"emSCRIPT_PROTOCOL_AUCTION_ENDITEM\", function(h)",
        "        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nType)",
        "        ObjBuffer:PushByType(h, OBJTYPE_STRING, %szAct)",
        "        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nId)",
        "        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, %nCount)",
        "    end, nTong)",
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
        "    AUC_Viewers[PlayerIndex] = {nType = nType, szAct = \"\", szName = GetName(), nTong = AUC_MyTong()}",
        "    AUC_SendActivityList(PlayerIndex, nType, AUC_MyTong())",
        "end",
        "",
        "function AUC_OnRequestActivityContent(nType, szAct, nPage, tbIds)",
        "    AUC_Viewers[PlayerIndex] = {nType = nType, szAct = szAct, szName = GetName(), nTong = AUC_MyTong()}",
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
        "    -- [A20] Hai duong duoc phep mua dut:",
        "    --   dong Ha Lan (phien the gioi/bang): mua o gia dang ha dan",
        "    --   dong ky gui ca nhan: mua o GIA MUA NGAY (cot base_price)",
        "    -- Ngoai hai duong do thi tu choi, khong thi ghi de nguoi dang giu gia cao nhat cua dong",
        "    -- kieu Anh ma khong hoan tien ho (loi [B2] cu).",
        "    local nGiaMua = r.cur",
        "    local bKyGui = 0",
        "    if r.kind ~= " + P + ".tbItemTypeEnum.eType_DUTCH then",
        "        if r.atype == " + P + ".tbAuctionTypeEnum.eType_PERSONAL and (r.base or 0) > 0 then",
        "            nGiaMua = r.base",
        "            bKyGui = 1",
        "        else",
        "            Msg2Player(\"" + V("Món này đấu theo kiểu tăng giá, hãy dùng nút Báo giá.") + "\")",
        "            return",
        "        end",
        "    end",
        "    if r.seller == GetName() then",
        "        Msg2Player(\"" + V("Không thể mua món do chính mình ký gửi.") + "\")",
        "        return",
        "    end",
        "    -- [A20b] nGiaMua = gia phai tra: dong Ha Lan la gia dang ha dan, dong ky gui la GIA MUA NGAY",
        "    if nPrice < nGiaMua then",
        "        Msg2Player(\"" + V("Giá đã thay đổi, hãy xem lại.") + "\")",
        "        return",
        "    end",
        "    if AUC_GetMoney(r.currency) < nGiaMua then",
        "        Msg2Player(\"" + V("Không đủ ") + "\"..AUC_CurName(r.currency)..\"" + V(" (cần ") + "\"..nGiaMua..\").\")",
        "        return",
        "    end",
        "    -- nho nguoi dang giu gia cao nhat TRUOC khi AUC_Buy ghi de ten nguoi mua",
        "    local szGiuGia, nGiuGia = r.buyer, r.cur",
        "    if AUC_Buy(nId, GetName(), nGiaMua) ~= 1 then",
        "        Msg2Player(\"" + V("Có người vừa mua trước, xin lỗi đại hiệp.") + "\")",
        "        return",
        "    end",
        "    if AUC_PayMoney(r.currency, nGiaMua) ~= 1 then",
        "        -- tra lai trang thai (hiem: tien vua doi)",
        "        -- [B2] PHAI xoa luon nguoi mua, khong thi nguoi ban khong rut lai duoc va kieu Anh",
        "        -- se giao mon cho nguoi chua tra dong nao khi het gio.",
        "        AUC_Rollback(nId)",
        "        -- [A24] AUC_Rollback xoa nguoi mua nhung KHONG tra cur_price, nen AUC_Bid ben duoi",
        "        -- truot chot cur_price < ? va nguoi giu gia cu khong bao gio duoc khoi phuc.",
        "        if bKyGui == 1 and szGiuGia ~= \"\" and nGiuGia > 0 then",
        "            AUC_SetPrice(nId, 0, 0, 0, r.endtime)",
        "            AUC_Bid(nId, szGiuGia, nGiuGia, r.endtime)",	# tra dong ve cho nguoi dang giu gia
        "        end",
        "        Msg2Player(\"" + V("Trừ tiền thất bại.") + "\")",
        "        return",
        "    end",
        "    -- [A20b] Mua dut mot dong DANG CO nguoi tra gia: tien cua nguoi do da bi tru tu luc tra,",
        "    -- phai hoan NGAY qua thu, khong thi ho mat trang (chinh la loi [B2] o duong nguoc lai).",
        "    if szGiuGia ~= \"\" and nGiuGia > 0 then",
        "        AUC_MailMoney(szGiuGia, \"" + V("Hoàn tiền đấu giá") + "\", \"" + V("Có người mua ngay ") + "\"..r.name..\"" + V(", hoàn lại tiền đại hiệp đã trả.") + "\", r.currency, nGiuGia)",
        "    end",
        "    AUC_Settle(r, GetName(), nGiaMua)",
        "    AUC_ReplyOffer(nType, szAct, nId, nGiaMua)",
        "    AUC_NotifyEndItem(nType, AUC_ActName(nType, r), nId, r.tong)",
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
        "    -- [A24] TRA BANG HOAC VUOT GIA MUA NGAY = CHOT BAN NGAY o dung gia mua ngay.",
        "    -- Truoc day duong tra gia khong co tran tren nen gia cao nhat vuot duoc gia mua ngay,",
        "    -- roi bat ky ai bam Mua ngay cung lay duoc mon o gia THAP HON: nguoi ban mat phan chenh,",
        "    -- nguoi dang giu gia cao nhat bi cuop mon. Chot o day thi cur khong bao gio vuot base nua.",
        "    if (r.base or 0) > 0 and r.atype == " + P + ".tbAuctionTypeEnum.eType_PERSONAL and nNewPrice >= r.base then",
        "        return AUC_OnRequestOfferDutch(nType, szAct, nId, r.base)",
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
        "        -- [A17] AUC_Bid DA ghi ten nguoi mua moi roi. Return thang o day thi nguoi giu gia cu",
        "        -- khong duoc hoan, ma het gio lai giao mon cho nguoi vua tru tien hong.",
        "        -- [A24] AUC_Rollback o day la MA CHET: no doi WHERE state=1 ma duong tra gia khong he",
        "        -- dat state=1. Duong lui dung la ha cur_price ve 0 roi ghi lai nguoi cu - khong ha thi",
        "        -- AUC_Bid truot chot cur_price < ? (gia bang nhau) va nguoi cu mat sach tien.",
        "        AUC_SetPrice(nId, 0, 0, 0, nEnd)",
        "        if szOld ~= \"\" and nOld > 0 then",
        "            AUC_Bid(nId, szOld, nOld, nEnd)",	# tra lai nguoi giu gia cu
        "        end",
        "        Msg2Player(\"" + V("Trừ tiền thất bại.") + "\")",
        "        AUC_Log(format(\"LOI: bid id %d cua %s da ghi nhung tru tien that bai - da lui dong\", nId, GetName()))",
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
        "    -- [B2] gui thu TRUOC roi moi danh dau da xu ly",
        "    if AUC_MailItem(r.seller, \"" + V("Rút vật phẩm ký gửi") + "\", \"" + V("Đại hiệp đã rút ") + "\"..r.name..\"" + V(" khỏi khu đấu giá, phí ký gửi không hoàn.") + "\", nId) <= 0 then",
        "        Msg2Player(\"" + V("Không rút được, hãy thử lại.") + "\")",
        "        return",
        "    end",
        "    AUC_SetState(nId, 3, 1)",
        "    Msg2Player(\"" + V("Đã rút ") + "\"..r.name..\"" + V(", vật phẩm gửi về hộp thư.") + "\")",
        "    AUC_Log(format(\"%s rut lai id %d (%s)\", GetName(), nId, r.name))",
        "    AUC_NotifyEndItem(nType, AUC_ActName(nType, r), nId, r.tong)",
        "end",
        "",
        "-- ---------------------------------------------------------------- chot giao dich",
        "-- r: dong (truoc khi doi), szBuyer, nPrice: gia chot. Giao do cho nguoi mua, tien cho nguoi ban / quy bang.",
        "function AUC_Settle(r, szBuyer, nPrice)",
        "    local nTax = floor(nPrice * " + P + ".nAuctionTaxRate / 100)",
        "    local nNet = nPrice - nTax",
        "    -- [A24] PHAI kiem ket qua gui thu: truoc day bo qua nen thu hong la tien nguoi mua da tru",
        "    -- ma mon bien mat, con dong nam o state = 1 - khong vong quet nao nhin toi, ket vinh vien.",
        "    -- Nay gui hong thi hoan tien nguoi mua va tra dong ve dang ban de ban lai.",
        "    if AUC_MailItem(szBuyer, \"" + V("Đấu giá thành công") + "\", \"" + V("Đại hiệp đã mua được ") + "\"..r.name..\"" + V(" với giá ") + "\"..nPrice..\" \"..AUC_CurName(r.currency)..\"" + V(". Vật phẩm đính kèm trong thư.") + "\", r.id) <= 0 then",
        "        AUC_MailMoney(szBuyer, \"" + V("Hoàn tiền đấu giá") + "\", \"" + V("Không giao được ") + "\"..r.name..\"" + V(", hoàn lại tiền đại hiệp đã trả.") + "\", r.currency, nPrice)",
        "        AUC_Rollback(r.id)",
        "        AUC_SetPrice(r.id, 0, 0, 0, r.endtime)",
        "        AUC_Log(format(\"LOI: id %d ban cho %s nhung GUI THU HONG - da hoan tien va tra dong ve dang ban\", r.id, szBuyer))",
        "        return 0",
        "    end",
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
        "        -- [A20 chu chot] hoan luon TIEN COC khi ban duoc: coc chi mat khi nguoi ban tu rut mon ve",
        "        local nCoc = r.deposit or 0",
        "        AUC_MailMoney(r.seller, \"" + V("Tiền bán ký gửi") + "\", \"" + V("Món ") + "\"..r.name..\"" + V(" đã bán được ") + "\"..nPrice..\" \"..AUC_CurName(r.currency)..\"" + V(", trừ thuế ") + "\"..nTax..\"" + V(", hoàn cọc ") + "\"..nCoc..\"" + V(", đại hiệp nhận ") + "\"..(nNet + nCoc)..\".\", r.currency, nNet + nCoc)",
        "    end",
        "    AUC_SetState(r.id, 3, 3)",
        "    AUC_Log(format(\"BAN id %d %s: %s -> %s gia %d tien %d (thue %d)\", r.id, r.name, r.seller, szBuyer, nPrice, r.currency, nTax))",
        "end",
        "",
        "-- het han / khong ai mua: tra do ve nguoi ban",
        "function AUC_Expire(r)",
        "    -- [B2] GUI THU TRUOC roi moi doi trang thai: truoc day doi truoc, thu gui hong la mon ket",
        "    -- trong bang vinh vien (moi vong quet chi lay state = 0).",
        "    if r.atype == " + P + ".tbAuctionTypeEnum.eType_WORLD then",
        "        AUC_Log(format(\"LUU PHACH the gioi id %d %s\", r.id, r.name))",
        "        AUC_SetState(r.id, 3, 3)",
        "        return",
        "    end",
        "    if AUC_MailItem(r.seller, \"" + V("Vật phẩm chưa bán được") + "\", \"" + V("Món ") + "\"..r.name..\"" + V(" hết hạn mà chưa ai mua, trả lại đại hiệp.") + "\", r.id) <= 0 then",
        "        AUC_Log(format(\"gui thu tra mon id %d cho %s HONG - giu nguyen de vong quet sau thu lai\", r.id, r.seller))",
        "        return",
        "    end",
        "    AUC_SetState(r.id, 2, 1)",
        "    -- [A20 chu chot] het han e hang thi hoan luon tien coc",
        "    if (r.deposit or 0) > 0 then",
        "        AUC_MailMoney(r.seller, \"" + V("Hoàn cọc ký gửi") + "\", \"" + V("Món ") + "\"..r.name..\"" + V(" hết hạn chưa bán được, hoàn lại tiền cọc.") + "\", r.currency, r.deposit)",
        "    end",
        "    AUC_Log(format(\"HET HAN id %d %s tra %s (hoan coc %d)\", r.id, r.name, r.seller, r.deposit or 0))",
        "end",
        "",
        "-- kieu Anh ket thuc co nguoi tra gia",
        "function AUC_FinishEnglish(r)",
        "    -- [A24] PHAI kiem ca gia: neu vi mot duong lui nao do ma dong con ten nguoi mua trong khi",
        "    -- gia da ve 0 thi khong duoc ban mon voi gia 0 - tra mon ve nguoi ban moi dung.",
        "    if r.buyer == \"\" or (r.cur or 0) <= 0 then",
        "        return AUC_Expire(r)",
        "    end",
        "    if AUC_SetState(r.id, 1, 1) ~= 1 then",
        "        return",
        "    end",
        "    AUC_Settle(r, r.buyer, r.cur)",
        "end",
        "",
        "-- ---------------------------------------------------------------- DAT BAN (goi tu NPC / GM)",
        "-- nPrice = gia mua ngay (ky gui) hoac gia khoi diem (phien the gioi/bang); nBaseIn = gia co ban khi ky gui.",
        "function AUC_PutOnItem(nType, szAct, nKind, nCur, nPrice, nItemIdx, nTong, nBaseIn)",
        "    if nItemIdx == nil or nItemIdx <= 0 then",
        "        return 0",
        "    end",
        "    local szRec, szName, szDesc, nCells, nStack, nHetHan = AUC_ItemToRec(nItemIdx)",
        "    if szRec == nil or szRec == \"\" then",
        "        Msg2Player(\"" + V("Món này không thể ký gửi (quặng, nguyên liệu thô...).") + "\")",
        "        return 0",
        "    end",
        "    -- [B2] mon co HAN DUNG: han la MOC thoi gian tuyet doi nen mon se chet ngay trong kho dau gia",
        "    -- hoac trong hop thu, va ban ghi ky gui khong luu moc do. Cam ky gui.",
        "    -- [A15 04/09] nHetHan do AUC_ItemToRec tra ve (= KItem::GetExpireTime, 0 = khong co han).",
        "    -- Truoc day dung GetItemLife la SAI: ham do nhan MA SU KIEN, tra -1 khi khong thay,",
        "    -- nen chan sach moi lan ky gui (chu bao 04/09: \"bo do vao thi bao co han su dung\").",
        "    if (nHetHan or 0) > 0 then",
        "        Msg2Player(\"" + V("Vật phẩm có hạn sử dụng không thể ký gửi.") + "\")",
        "        return 0",
        "    end",
        "    if GetItemBindState and GetItemBindState(nItemIdx) ~= 0 then",
        "        Msg2Player(\"" + V("Vật phẩm khoá không thể ký gửi.") + "\")",
        "        return 0",
        "    end",
        "    local nNow = GetCurrentTime()",
        "    -- [A20 04/09 chu chot] KY GUI CA NHAN = dau gia TANG DAN co MUA NGAY:",
        "    --   guar = gia CO BAN (khoi diem)   cur = gia cao nhat dang co, 0 = chua ai tra",
        "    --   base = gia MUA NGAY             buoc moi luot = 10% gia co ban (AUC_RowToClient)",
        "    -- Het 24 gio: ai giu gia cao nhat thi duoc mon; khong ai tra thi tra mon ve nguoi ban.",
        "    local nGuar = nBaseIn or nPrice",
        "    local nCurP = 0",
        "    local nBase = nPrice",
        "    local nEnd = nNow + " + P + ".nPersonalDuration",
        "    local nNextDrop = 0",
        "    local nDropLeft = 0",
        "    if nType == " + P + ".tbAuctionTypeEnum.eType_PERSONAL then",
        "        nKind = " + P + ".tbItemTypeEnum.eType_ENGLISH",
        "    end",
        "    if nType ~= " + P + ".tbAuctionTypeEnum.eType_PERSONAL then",
        "        nGuar = nPrice",	# phien the gioi/bang: chi mot gia khoi diem
        "        if nKind == " + P + ".tbItemTypeEnum.eType_ENGLISH then",
        "            -- [A17] gia khoi diem = gia san, chua ai tra thi cur = 0 (khong phai 150%),",
        "            -- khong thi nguoi tra gia DAU TIEN luon bi tu choi vi cau SQL doi cur_price < gia tra.",
        "            nCurP = 0",
        "            nBase = nGuar",
        "            nNextDrop = 0",
        "            nDropLeft = 0",
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
        "        -- [A20] coc tinh tren GIA CO BAN, kep boi tran de mot lan go nham khong tru sach vi",
        "        nDeposit = floor(nGuar * " + P + ".nPersonalPutOnCost / 100)",
        "        local nTran = " + P + ".nMaxDepositMoney",
        "        if nCur == " + P + ".tbCurrency.XU then",
        "            nTran = " + P + ".nMaxDepositXu",
        "        end",
        "        if nDeposit > nTran then",
        "            nDeposit = nTran",
        "        end",
        "        if AUC_PayMoney(nCur, nDeposit) ~= 1 then",
        "            Msg2Player(\"" + V("Không đủ ") + "\"..AUC_CurName(nCur)..\"" + V(" để trả phí ký gửi ") + "\"..nDeposit..\".\")",
        "            return 0",
        "        end",
        "    end",
        "    -- [B2] XOA MON TRUOC roi moi ghi kho: truoc day ghi kho xong moi xoa va bo qua ket qua,",
        "    -- xoa hong la mon vua nam trong tui vua nam trong kho dau gia = nhan doi.",
        "    -- Dung so luong THO tu AUC_ItemToRec (GetItemStackCount bi kep theo tran chong nen xoa thieu).",
        "    if RemoveItemByIndex(nItemIdx, nStack or 1) ~= 1 then",
        "        Msg2Player(\"" + V("Không lấy được vật phẩm khỏi hành trang, hãy thử lại.") + "\")",
        "        if nDeposit > 0 then",
        "            if nCur == " + P + ".tbCurrency.XU then",
        "                SetTask(" + P + ".XU_TASK, GetTask(" + P + ".XU_TASK) + nDeposit)",
        "            else",
        "                Earn(nDeposit)",
        "            end",
        "        end",
        "        return 0",
        "    end",
        "    local nId = AUC_PutOn(nType, szAct or \"\", nKind, GetName(), nTong or 0, szName, szDesc, szRec, nCells or 1, nCur,",
        "        nBase, nCurP, nGuar, nDeposit, nNow, nEnd, nNextDrop, nDropLeft)",
        "    if nId <= 0 then",
        "        -- ghi kho hong: TRA MON LAI NGAY (AUC_GiveRec dung lai dung mon vua xoa)",
        "        if AUC_GiveRec(szRec) <= 0 then",
        "            AUC_Log(format(\"MAT DO: %s ky gui %s nhung ghi kho hong VA tra lai hong\", GetName(), szName))",
        "        end",
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
        "    AUC_Log(format(\"DAT BAN id %d loai %d '%s' %s gia %d tien %d nguoi %s\", nId, nType, szAct or \"\", szName, nPrice, nCur, GetName()))",
        "    local szActReal = szAct",
        "    if nType == " + P + ".tbAuctionTypeEnum.eType_PERSONAL then",
        "        szActReal = " + P + ".szPersonalActivity",
        "    end",
        "    AUC_NotifyNewItem(nType, szActReal, nId, nTong)",
        "    return nId",
        "end",
        "",
        "AUCPOLL_FRAMES = 30 * 18",
        "AUCPOLL_GLB    = 9002",
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
        "    -- [A21] noi ro luat cho nguoi ban truoc khi ho bam Dong y",
        "    -- [A23 04/09] Chu bao \"cho nhap gia bi de chu len\": o mo ta cua hop trai tu y 96 den 182,",
        "    -- dung cho hai hang nhap gia. Chu dai ba bon dong la de len chung (loi cua dot A21).",
        "    -- Nay trong hop chi de MOT dong ngan; luat day du noi qua khung chat, doc lai duoc.",
        "    if nType == " + P + ".tbAuctionTypeEnum.eType_PERSONAL then",	# [A23] chi ky gui moi co hai gia
        "    Msg2Player(\"" + V("Ký gửi: nhập giá mua ngay và giá cơ bản (cơ bản phải thấp hơn giá mua ngay).") + "\")",
        "    Msg2Player(\"" + V("Người mua trả giá lên từng lượt, mỗi lượt thêm 10% giá cơ bản, hoặc trả thẳng giá mua ngay.") + "\")",
        "    Msg2Player(\"" + V("Cọc ") + "\".." + P + ".nPersonalPutOnCost..\"" + V("% giá cơ bản, hoàn lại khi bán được hoặc hết hạn, chỉ mất khi tự rút món. Thuế ") + "\".." + P + ".nAuctionTaxRate..\"" + V("% khi bán được.") + "\")",
        "    end",
        "    local szNhac = \"" + V("Cọc ") + "\".." + P + ".nPersonalPutOnCost..\"" + V("%, thuế ") + "\".." + P + ".nAuctionTaxRate..\"" + V("% khi bán.") + "\"",
        "    GiveItemUI(\"" + V("Ký gửi đấu giá") + "\", szNhac, \"AUC_OnGiveOk\", \"AUC_OnGiveCancel\", 0, \"AUC_OnGiveCheck\", 0, AUC_SCRIPT)",
        "end",
        "",
        "-- [A6] client bao GIA + LOAI TIEN (o ngay trong hop dua vat pham) truoc khi bam Dong y",
        "-- [A20] nPrice = gia MUA NGAY, nBase = gia CO BAN (khoi diem, phai thap hon gia mua ngay)",
        "function AUC_OnRequestSetPrice(nPrice, nCur, nBase)",
        "    local t = AUC_TMP[PlayerIndex]",
        "    if not t then",
        "        return",
        "    end",
        "    t.nPrice = floor(nPrice or 0)",
        "    t.nBase = floor(nBase or 0)",
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
        "        Msg2Player(\"" + V("Chưa nhập giá mua ngay hợp lệ.") + "\")",
        "        return",
        "    end",
        "    -- [A20] ky gui ca nhan doi DU HAI gia va gia co ban phai THAP HON gia mua ngay,",
        "    -- khong thi khong con gi de dau (chu hoi dung cho nay hom 04/09).",
        "    if t.nType == " + P + ".tbAuctionTypeEnum.eType_PERSONAL then",
        "        if (t.nBase or 0) < 1 then",
        "            Msg2Player(\"" + V("Chưa nhập giá cơ bản.") + "\")",
        "            return",
        "        end",
        "        if t.nBase >= t.nPrice then",
        "            Msg2Player(\"" + V("Giá cơ bản phải thấp hơn giá mua ngay.") + "\")",
        "            return",
        "        end",
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
        "    local nId = AUC_PutOnItem(t.nType, szAct, t.nKind, t.nCur, t.nPrice, nIdx, t.nTong, t.nBase)",
        "    if nId > 0 then",
        "        Msg2Player(\"" + V("Đã đưa vào khu đấu giá, mã số ") + "\"..nId..\"" + V(". Tiền bán và vật phẩm trả về qua hộp thư.") + "\")",
        "        -- [A28] bao RIENG nguoi vua ky gui: dong hop dua vat pham va nap lai trang cho ho.",
        "        -- Khong bam vao goi NEWITEM (goi do phat cho MOI nguoi dang xem) vi nguoi khac ky gui",
        "        -- cung sinh ra no, se dong nham hop cua nguoi dang dat mon vao.",
        "        local hOk = OB_Create()",
        "        ObjBuffer:PushByType(hOk, OBJTYPE_NUMBER, t.nType)",
        "        ObjBuffer:PushByType(hOk, OBJTYPE_STRING, szAct)",
        "        AUC_SendTo(PlayerIndex, \"emSCRIPT_PROTOCOL_AUCTION_PUTONOK\", hOk)",
        "        OB_Release(hOk)",
        "    end",
        "end",
        "",
        "-- ---------------------------------------------------------------- QUET (moi 30 giay)",
        "-- [B3] Timer PHAI dang ky ngay trong tep nay: Include = lua_dofile vao CHINH state goi, nen dat o",
        "-- tep rieng thi AUC_Tick chay o state khac state giu AUC_Viewers -> moi thong bao tu dong khong toi ai.",
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
        "        AUC_NotifyEndItem(r.atype, szAct, r.id, r.tong)",
        "    end",
        "    -- [A14] 1b) ky gui ca nhan: ha mot nhip THANG ve gia san khi den moc.",
        "    -- De khoi rieng chu khong nhet vao vong duoi, vi vong duoi con lo bao KET THUC PHIEN -",
        "    -- tab Ca nhan la cho ban thuong truc, khong co phien nao de ket thuc.",
        "    local rowsP = AUC_List(" + P + ".tbAuctionTypeEnum.eType_PERSONAL, 200, 0)",
        "    for i = 1, getn(rowsP) do",
        "        local r = rowsP[i]",
        "        if r.dropleft > 0 and r.nextdrop > 0 and r.nextdrop <= nNow then",
        "            if AUC_SetPrice(r.id, r.guar, 0, 0, r.endtime) == 1 then",
        "                r.cur = r.guar",
        "                r.nextdrop = 0",
        "                r.dropleft = 0",
        "                AUC_NotifyPrice(" + P + ".tbAuctionTypeEnum.eType_PERSONAL, " + P + ".szPersonalActivity, r, nNow)",
        "            end",
        "        end",
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
        "    self.bTuBam = 0",
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
        "    -- [A26] danh dau: chinh ta vua bam lenh lam mon bien khoi danh sach, nen khi goi",
        "    -- ket thuc mon ve thi nap lai trang cho RIENG ta. Nguoi khac van giu nguyen cho",
        "    -- (ban goc co y khong don danh sach duoi tay nguoi dang bam - de bam nham mon).",
        "    self.bTuBam = 1",
        "    self:SendItemReq(\"emSCRIPT_PROTOCOL_AUCTION_REQUEST_OFFERDUTCHPRICE\", nId, nPrice)",
        "end",
        "",
        "function UIAuctionHouse:RequestGetBackItem(nId)",
        "    self.bTuBam = 1",	# [A26] xem chu thich o RequestOfferDutchPrice
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
        "-- [A28] may chu bao ky gui XONG: dong hop dua vat pham va nap lai trang dang xem.",
        "function UIAuctionHouse:OnPutOnOk(nType, szAct)",
        "    AuctionPutOnMode(2)",	# 2 = dong hop
        "    if AuctionUiIsOpen() == 1 and self.nCurTypeIndex == nType then",
        "        self:ResetPageInfo(nType, szAct)",
        "        self:RequestActivityContent(self.szCurActivityName, self.nCurPageIndex)",
        "    end",
        "end",
        "",
        "function UIAuctionHouse:OnPutOnClick(nType)",
        "    local h = OB_Create()",
        "    AuctionPutOnMode(1)",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nType or self.nCurTypeIndex)",
        "    ScriptProtocol:SendData(\"emSCRIPT_PROTOCOL_AUCTION_REQUEST_PUTON\", h)",
        "    OB_Release(h)",
        "end",
        "",
        "-- [A6] o gia + nut doi tien nam ngay trong hop dua vat pham; bam Dong y thi bao len truoc",
        "-- [A20c] ba so: gia mua ngay, loai tien, gia co ban",
        "function UIAuctionHouse:OnSetPrice(nPrice, nCur, nBase)",
        "    local h = OB_Create()",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nPrice or 0)",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nCur or 1)",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nBase or 0)",
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
        "    -- [A11] may chu da gui san bMine (client khong co GetName)",
        "    local bMine = tbItem.bMine or 0",
        "    local szCur = " + P + ".tbCurrencyName[tbItem.nCurrencyType] or \"\"",
        "    -- [A9] 6 so cua vat pham nam thang trong dong (khong con bang long)",
        "    if tbItem.nType == " + K + ".eType_ENGLISH then",
        "        if bAdd == 1 then",
        "            AuctionAddEnglishItem(nType, tbItem.nId, tbItem.nStartTime, tbItem.nGuaranteedPrice, tbItem.nRangePerOffer, tbItem.nCurrencyType,",
        "                tbItem.nMaxPrice, tbItem.nSelfPrice, tbItem.nRemainingTime, bMine,",
        "                tbItem.nG or 0, tbItem.nD or 0, tbItem.nP or 0, tbItem.nL or 0, tbItem.nS or 0, tbItem.nK or 0, tbItem.nCount or 1, tbItem.szName or \"\", szCur,",
        "                tbItem.szInfo or \"\", tbItem.nBuyNow or 0)",	# [A20c] so cuoi = gia mua ngay
# [A16] chuoi cuoi = bo so dung lai dung mon
        "        else",
        "            AuctionSetEnglishItem(nType, tbItem.nId, tbItem.nStartTime, tbItem.nGuaranteedPrice, tbItem.nRangePerOffer, tbItem.nCurrencyType,",
        "                tbItem.nMaxPrice, tbItem.nSelfPrice, tbItem.nRemainingTime, bMine, szCur)",
        "        end",
        "    else",
        "        if bAdd == 1 then",
        "            AuctionAddDutchItem(nType, tbItem.nId, tbItem.nStartTime, tbItem.nCurPrice, tbItem.nGuaranteedPrice, tbItem.nCurrencyType,",
        "                tbItem.nRemainingTime, tbItem.nNextPrice, tbItem.nTotalRemainingTime, bMine,",
        "                tbItem.nG or 0, tbItem.nD or 0, tbItem.nP or 0, tbItem.nL or 0, tbItem.nS or 0, tbItem.nK or 0, tbItem.nCount or 1, tbItem.szName or \"\", szCur,",
        "                tbItem.szInfo or \"\")",	# [A16] chuoi cuoi = bo so dung lai dung mon
        "        else",
        "            AuctionSetDutchItem(nType, tbItem.nId, tbItem.nStartTime, tbItem.nCurPrice, tbItem.nGuaranteedPrice, tbItem.nCurrencyType,",
        "                tbItem.nRemainingTime, tbItem.nNextPrice, tbItem.nTotalRemainingTime, bMine, szCur)",
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
        "-- [A19] so mon THUC SU dang nam tren trang dang xem (ban goc co ham nay, ban ta thieu)",
        "function UIAuctionHouse:GetCurPageSize()",
        "    local a = self.tbActivityList[self.nCurTypeIndex]",
        "    if not a then",
        "        return 0",
        "    end",
        "    a = a[self.szCurActivityName]",
        "    if not a then",
        "        return 0",
        "    end",
        "    local nTong = a.nTotalCount or 0",
        "    local nBo = (self.nCurPageIndex - 1) * " + P + ".nMaxItemPerPage",
        "    local nCon = nTong - nBo",
        "    if nCon < 0 then",
        "        nCon = 0",
        "    end",
        "    if nCon > " + P + ".nMaxItemPerPage then",
        "        nCon = " + P + ".nMaxItemPerPage",
        "    end",
        "    return nCon",
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
        "    -- [A19] chi hoi lai khi TRANG DANG XEM CHUA DAY (dung dieu kien thu tu cua ban goc,",
        "    -- uiauction_house.lua:542). Trang da day thi mon moi nam o trang sau, hoi lai chi to doi goi.",
        "    if self.nCurTypeIndex == nType and self.szCurActivityName == szAct and AuctionUiIsOpen() == 1",
        "        and self:GetCurPageSize() < " + P + ".nMaxItemPerPage then",
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
        "        -- [A19] CHI danh dau mon tai cho, KHONG nap lai ca trang.",
        "        -- Ban goc co y chu thich bo dong nap lai (uiauction_house.lua:568): don danh sach duoi",
        "        -- tay nguoi dang bam la mua nham mon, va loi goi nap lai con xoa luon ket qua cua",
        "        -- AuctionEndItem ngay dong tren no. Voi 2 mon mot trang thi moi mon ai do ban xong deu",
        "        -- bat MOI nguoi dang mo cua so hoi lai ca trang - dung kieu doi goi (bai hoc F4 04/09).",
        "        AuctionEndItem(nType, nEndId)",
        "        self:ResetPageInfo(nType, szAct)",
        "        -- [A26] chi nguoi VUA BAM moi duoc nap lai trang: ho dang cho danh sach doi,",
        "        -- khong ai bi giat tay. Nguoi khac van chi danh dau mon tai cho nhu ban goc.",
        "        if self.bTuBam == 1 then",
        "            self.bTuBam = 0",
        "            if AuctionUiIsOpen() == 1 then",
        "                self:RequestActivityContent(szAct, self.nCurPageIndex)",
        "            end",
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
    "emSCRIPT_PROTOCOL_AUCTION_PUTONOK",	# [A28] may chu bao RIENG nguoi vua ky gui xong
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
    ("emSCRIPT_PROTOCOL_AUCTION_PUTONOK", "UIAuctionHouse:OnPutOnOk", "{OBJTYPE_NUMBER, OBJTYPE_STRING}"),	# [A28]
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
    ("emSCRIPT_PROTOCOL_AUCTION_REQUEST_SETPRICE", "AUC_OnRequestSetPrice", "{OBJTYPE_NUMBER, OBJTYPE_NUMBER, OBJTYPE_NUMBER}"),	# [A20c] them gia co ban
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
        # [A14 04/09] chu: "cac chu chong len nhau". Nhan de Width=120 ma cot chi cach 86 nen nhan
        # de len gia tri (86+120 > 172, 258+120 > 344), cot phai con tran khoi khung 429 (344+120 = 464).
        # Dat lai luoi: nhan 86 (86..172 va 258..344), gia tri 84 (172..256 va 344..428), ba dong 26/46/66.
        # [A22] ha BUOC HANG 118 -> 107 de lot ba hang trong khung cao 321 ma khong de len nut
        # chuyen trang. UiAuction lay chieu cao nay lam buoc dat hang (m_nItemRowH).
        if n in ("auction_item_dutch_header", "auction_item_english_header",
                 "auction_item_personal_header", "auction_item_personal_myself_header"):
            txt = _setkey(txt, "Main", "Height", 107)
        if n in ("auction_item_dutch_header", "auction_item_english_header"):
            if n == "auction_item_dutch_header":
                cols = [("CurrentPriceTitle", "CurrentPrice", V("Mua ngay")),
                        ("BasePriceTitle", "BasePrice", V("Giá sàn")),
                        ("CurrencyTypeTitle", "CurrencyType", V("Loại tiền")),
                        ("FreshRemainingTimeTitle", "FreshRemainingTime", V("Hạ giá sau")),
                        ("NextPriceTitle", "NextPrice", V("Giá kế tiếp")),
                        ("TotalRemainingTimeTitle", "TotalRemainingTime", V("Hết hạn sau"))]
            else:
                cols = [("GuaranteedPriceTitle", "GuaranteedPrice", V("Khởi điểm")),
                        ("RangePerOfferTitle", "RangePerOffer", V("Bước giá")),
                        ("CurrencyTypeTitle", "CurrencyType", V("Loại tiền")),
                        ("MaxPriceTitle", "MaxPrice", V("Cao nhất")),
                        ("SelfPriceTitle", "SelfPrice", V("Giá của ta")),
                        ("RemainingTimeTitle", "RemainingTime", V("Còn lại"))]
            for i in range(6):
                szT, szV, szLabel = cols[i]
                nLeft = 86 if i < 3 else 258
                nTop = 26 + (i % 3) * 20
                txt = _setkey(txt, szT, "Left", nLeft)
                txt = _setkey(txt, szT, "Top", nTop)
                txt = _setkey(txt, szT, "Width", 86)
                txt = _setkey(txt, szT, "Height", 16)
                txt = _setkey(txt, szT, "Text", szLabel)
                txt = _setkey(txt, szV, "Left", nLeft + 86)
                txt = _setkey(txt, szV, "Top", nTop)
                txt = _setkey(txt, szV, "Width", 84)
                txt = _setkey(txt, szV, "Height", 16)
                txt = _setkey(txt, szV, "HAlign", 0)
        # [A21] nut tren dong Ha Lan (phien the gioi / bang hoi): bam la MUA DUT ngay o gia dang
        # hien, khong phai mo mot lan dau gia - nhan cu "Ta muon dau gia" lam nguoi choi bam nham.
        if n == "auction_item_dutch_header":
            txt = _relabel(txt, "btnBid", V("Mua ngay"))
        # [A25 04/09] nut "Bao gia": ANH cua nut rong 67 (chung sprite voi hai nut kia) ma o chi
        # de 46 -> anh tran sang phai va cham nut ben canh o 321 (chu: "2 nut bi xac nhau").
        if n == "auction_item_english_header":
            txt = _setkey(txt, "btnOfferPrice", "Left", 250)	# 250 + 67 = 317, cach nut ke 4 px
            txt = _setkey(txt, "btnOfferPrice", "Width", 67)
        # [A25] O VAT PHAM noi rong cho vua trang bi nhieu o. Engine KHONG thu nho duoc anh vat
        # pham: KItem::PaintItem ve nguyen co (rong x 26, cao x 26) roi DrawPrimitives, con
        # Width/Height truyen vao chi dung de CAN GIUA. Duong duy nhat engine co san la doi han
        # sang anh thay the (bResize) nhung anh do la tui chuyen van - mat luon hinh mon.
        # 58 x 78 du cho trang bi 2 x 3; cao hon 3 o thi van nho ra.
        # [A26 04/09] con so tien de o Left=31 trong khi ANH khung nhap trai tu 86 den 213,
        # nen no ve han ra ngoai ben trai khung (chu: "so tien phai nam trong o nhap").
        # Dat vao 101 - ngay sau dau nho o 89..98 - va rong 108 cho gon trong khung.
        if n == "auction_item_english_header":
            txt = _setkey(txt, "txtJinPai", "Left", 101)
            txt = _setkey(txt, "txtJinPai", "Width", 108)
            # [A27] chu: "so tien trong o nhap bi lech phia tren" - o chu cao 16 dat o Top=88
            # trong khung cao 20 o Top=86 va KHONG bat can giua doc. Cho trung khung roi bat VAlign.
            txt = _setkey(txt, "txtJinPai", "Top", 86)
            txt = _setkey(txt, "txtJinPai", "Height", 20)
            txt = _setkey(txt, "txtJinPai", "VAlign", 1)
        if n == "auction_item_icon":
            txt = _setkey(txt, "Main", "Left", 14)
            txt = _setkey(txt, "Main", "Top", 14)
            txt = _setkey(txt, "MailAwardItemSpr", "Left", 0)
            txt = _setkey(txt, "MailAwardItemSpr", "Top", 0)
            txt = _setkey(txt, "MailAwardItemSpr", "Width", 58)
            txt = _setkey(txt, "MailAwardItemSpr", "Height", 78)
            txt = _setkey(txt, "MailAwardItemCount", "Top", 64)
            txt = _setkey(txt, "MailAwardItemCount", "Width", 56)
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


def drop_poll():
    """[B3] timer da chuyen vao auction_manager.lua - xoa tep poll rieng"""
    for root in (SV, MIRROR_S):
        pth = os.path.join(root, r"script\auction_house\auctionpoll.lua")
        if os.path.exists(pth):
            if CHECK:
                print("  (check) xoa", pth)
            else:
                os.remove(pth)
                print("  xoa:", pth)


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
        ("go poll", drop_poll),
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
