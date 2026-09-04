# -*- coding: utf-8 -*-
"""[MAIL 03/09] Dot 3 - script MAY CHU cua he thu (cay chay that E:\\...\\bin\\server) + guong worktree.
  Sinh : script\\mail\\mailmanager.lua (thu vien), script\\mail\\mailpoll.lua (timer do web), script\\mail\\maildef.lua
  Va   : script\\script_protocol\\protocol_def_gs.lua (6 handler MAIL), script\\player\\playerlogin.lua (moc dang nhap),
         script\\global\\npcchucnang\\dichquan.lua (menu "Nhan thu"), script\\item\\lenhbaiadmin.lua (2 muc thu)
Chu Viet = TCVN3 tho (vn_to_octal), tep ghi latin-1. Chay: python p3_lua.py [--check]
"""
import io, os, re, sys

SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
MIRROR = r"D:\GAMEDEVNEW_wt_mail\serverscript_jx2\mail\server"
CHECK = "--check" in sys.argv
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes

MARK = "[MAIL 03/09]"
E = "\r\n"


def V(s):
    return unicode_to_tcvn3_bytes(s).decode("latin-1")


def G(s):
    return s.encode("gbk").decode("latin-1")


def rd(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def wr(p, s):
    if CHECK:
        print("  (check) ghi", p, len(s))
        return
    d = os.path.dirname(p)
    if not os.path.isdir(d):
        os.makedirs(d)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("  ghi:", p, len(s))


def wr2(rel, s):
    wr(os.path.join(SV, rel), s)
    wr(os.path.join(MIRROR, rel), s)


def hb(s):
    return sum(1 for c in s if ord(c) >= 0x80)


# ---------------------------------------------------------------- mailmanager.lua
def build_mailmanager():
    L = [
        "-- mailmanager.lua - [MAIL 03/09] he THU phia MAY CHU (thay \\script\\mail\\MailManager.lua cua VLTK 2.0 - khong co o dau).",
        "-- Thu nam trong bang MySQL `mail` (KMailServer.cpp: MailDB_*). Nguon gui: web admin ghi thang bang (state 0),",
        "-- hoat dong / top tuan-thang / dua top goi MailManager_SendMail(...). Client = uimail.lua cua 2.0, giao thuc giu nguyen.",
        "-- Cac ham MailManager_OnRequest* duoc goi qua DynamicExecuteByPlayer (PlayerIndex da dat) tu protocol_def_gs.lua.",
        "-- Dinh kem (cot award): \"item:genre,detail,particular,level,series,luck,soluong;money:N;xu:N;exp:N\" (nhieu muc cach ;).",
        "Include(\"\\\\script\\\\protocol.lua\")",
        "Include(\"\\\\script\\\\mail\\\\maildef.lua\")",
        "",
        "MAILMGR_PAGE        = 12              -- so header moi goi (ObjBuffer 4 KB)",
        "MAILMGR_MAX_CONTENT = 2000            -- MAILUI_CONTENT_LEN client = 2048",
        "MAILMGR_ICON_MONEY  = \"\\\\spr\\\\Ui4\\\\email\\\\" + G("银两图标") + ".spr\"",
        "MAILMGR_ICON_XU     = \"\\\\spr\\\\item\\\\script\\\\jinding.spr\"",
        "MAILMGR_ICON_EXP    = \"\\\\spr\\\\item\\\\exp.spr\"",
        "MAILMGR_SENDER_SYS  = \"" + V("Thư hệ thống") + "\"",
        "MAILMGR_SENDER_NPH  = \"" + V("Nhà phát hành") + "\"",
        "MAILMGR_XU_TASK     = 251             -- xu hien o hanh trang = task 251 (petsys\\jx1_compat.lua)",
        "MAILMGR_ICON_GOLD   = MAILMGR_ICON_XU   -- [D9] trang bi hoang kim (gold:) - tam dung bieu tuong xu",
        "MAILMGR_ICON_TASK   = MAILMGR_ICON_EXP  -- [D9] diem nhiem vu (task:) / danh vong (repute:)",
        "MAILMGR_TASK_NAME   = {[337] = \"" + V("Điểm sự kiện") + "\"}   -- [D9] ten hien thi cho task:id",
        "function MailManager_TaskName(nTaskId)",
        "    return MAILMGR_TASK_NAME[nTaskId] or (\"" + V("Nhiệm vụ ") + "\"..nTaskId)",
        "end",
        "MailAutoDel = MailAutoDel or {}      -- [ten nhan vat] = 1: tu xoa thu khong dinh kem sau khi doc",
        "",
        "function MailManager_Split(s, sep)",
        "    local tb = {}",
        "    if type(s) ~= \"string\" then",
        "        return tb",
        "    end",
        "    local nPos = 1",
        "    while 1 do",
        "        local a, b = strfind(s, sep, nPos, 1)",
        "        if not a then",
        "            tinsert(tb, strsub(s, nPos))",
        "            break",
        "        end",
        "        tinsert(tb, strsub(s, nPos, a - 1))",
        "        nPos = b + 1",
        "    end",
        "    return tb",
        "end",
        "",
        "function MailManager_Trim(s)",
        "    local _, _, t = strfind(s or \"\", \"^%s*(.-)%s*$\")",
        "    return t or \"\"",
        "end",
        "",
        "-- \"item:6,1,4139,0,0,0,1;money:1000;xu:10;exp:5000\" -> bang cac muc",
        "function MailManager_ParseAward(szAward)",
        "    local tb = {}",
        "    if type(szAward) ~= \"string\" or szAward == \"\" then",
        "        return tb",
        "    end",
        "    local parts = MailManager_Split(szAward, \";\")",
        "    for i = 1, getn(parts) do",
        "        local p = MailManager_Trim(parts[i])",
        "        local _, _, kind, val = strfind(p, \"^(%a+)%s*:%s*(.*)$\")",
        "        if kind then",
        "            kind = strlower(kind)",
        "            val = MailManager_Trim(val)",
        "            if kind == \"item\" then",
                "                -- [D9] item:g,d,p,l,s,k,n[,lock][,expSec][,magic][,stack]: lock -2 = khoa vinh vien (SetPlayerItemLock),",
                "                -- expSec = het han sau N giay (AddTimeItem), magic = tham so 7 AddItem, stack 1 = giao 1 chong n mon (AddItemSL)",
        "                local nums = MailManager_Split(val, \",\")",
        "                if getn(nums) >= 6 then",
        "                    tinsert(tb, {szKind = \"item\", nGenre = tonumber(nums[1]) or 0, nDetail = tonumber(nums[2]) or 0,",
        "                        nParticular = tonumber(nums[3]) or 0, nLevel = tonumber(nums[4]) or 0, nSeries = tonumber(nums[5]) or 0,",
        "                        nLuck = tonumber(nums[6]) or 0, nCount = tonumber(nums[7]) or 1, nLock = tonumber(nums[8]) or 0,",
        "                        nExpSec = tonumber(nums[9]) or 0, nMagic = tonumber(nums[10]) or 0, nStack = tonumber(nums[11]) or 0})",
        "                end",
        "            elseif kind == \"gold\" then",
        "                -- [D9] gold:record,n[,lock][,expSec] = trang bi hoang kim theo dong goldequip.txt (AddItem2 NATURE_GOLD, nhu Da Tau)",
        "                local nums = MailManager_Split(val, \",\")",
        "                if getn(nums) >= 1 then",
        "                    tinsert(tb, {szKind = \"gold\", nRecord = tonumber(nums[1]) or 0, nCount = tonumber(nums[2]) or 1,",
        "                        nLock = tonumber(nums[3]) or 0, nExpSec = tonumber(nums[4]) or 0})",
        "                end",
        "            elseif kind == \"task\" then",
        "                -- [D9] task:id,n = cong n vao o nhiem vu id (337 = diem su kien Tong Kim)",
        "                local nums = MailManager_Split(val, \",\")",
        "                if getn(nums) >= 2 and (tonumber(nums[2]) or 0) ~= 0 then",
        "                    tinsert(tb, {szKind = \"task\", nTaskId = tonumber(nums[1]) or 0, nCount = tonumber(nums[2]) or 0})",
        "                end",
        "            elseif kind == \"money\" or kind == \"xu\" or kind == \"exp\" or kind == \"repute\" then",
        "                local n = tonumber(val) or 0",
        "                if n > 0 then",
        "                    tinsert(tb, {szKind = kind, nCount = n})",
        "                end",
        "            end",
        "        end",
        "    end",
        "    return tb",
        "end",
        "",
        "-- mo ta cho client (uimail.lua PackAwardInfo): item giu nguyen thuoc tinh, tien/xu/exp thanh bieu tuong",
        "function MailManager_AwardInfo(tbAward)",
        "    local tb = {}",
        "    for i = 1, getn(tbAward) do",
        "        local a = tbAward[i]",
        "        if a.szKind == \"item\" then",
        "            tinsert(tb, a)",
        "        elseif a.szKind == \"money\" then",
        "            tinsert(tb, {szKind = \"icon\", szIcon = MAILMGR_ICON_MONEY, szName = \"" + V("Ngân lượng") + "\", szDesc = a.nCount..\" " + V("Ngân lượng") + "\", nCount = a.nCount})",
        "        elseif a.szKind == \"xu\" then",
        "            tinsert(tb, {szKind = \"icon\", szIcon = MAILMGR_ICON_XU, szName = \"Xu\", szDesc = a.nCount..\" xu\", nCount = a.nCount})",
        "        elseif a.szKind == \"exp\" then",
        "            tinsert(tb, {szKind = \"icon\", szIcon = MAILMGR_ICON_EXP, szName = \"" + V("Kinh nghiệm") + "\", szDesc = a.nCount..\" " + V("kinh nghiệm") + "\", nCount = a.nCount})",
        "        elseif a.szKind == \"gold\" then",
        "            tinsert(tb, {szKind = \"icon\", szIcon = MAILMGR_ICON_GOLD, szName = \"" + V("Trang bị Hoàng Kim") + "\", szDesc = \"" + V("Dòng ") + "\"..a.nRecord..\" x\"..a.nCount, nCount = a.nCount})",
        "        elseif a.szKind == \"task\" then",
        "            tinsert(tb, {szKind = \"icon\", szIcon = MAILMGR_ICON_TASK, szName = MailManager_TaskName(a.nTaskId), szDesc = a.nCount..\" \"..MailManager_TaskName(a.nTaskId), nCount = a.nCount})",
        "        elseif a.szKind == \"repute\" then",
        "            tinsert(tb, {szKind = \"icon\", szIcon = MAILMGR_ICON_TASK, szName = \"" + V("Danh vọng") + "\", szDesc = a.nCount..\" " + V("danh vọng") + "\", nCount = a.nCount})",
        "        end",
        "    end",
        "    return tb",
        "end",
        "",
        "function MailManager_SendTo(nPlayerIdx, szEnum, h)",
        "    local nOk = SendScriptDataToPlayer(nPlayerIdx, ScriptProtocol[szEnum], h)",
        "    -- [D4] chan doan: logs/hethong.log [MAIL] (SendScriptDataToPlayer tra 0 khi PlayerIndex/ket noi/goi hong)",
        "    GhiLog(\"MAIL\", format(\"gui %s -> player %d: %s\", szEnum, nPlayerIdx, (nOk == 1) and \"ok\" or \"THAT BAI\"))",
        "    return nOk",
        "end",
        "",
        "-- trang header: tbList[nId] = {szSender, szTitle, nState, nExpiredTime, nSendTime, nAwardCount}; nComplete = 1 neu het",
        "function MailManager_Headers(szRole, nMinId)",
        "    local tb = MailDB_Headers(szRole, nMinId or 0, MAILMGR_PAGE + 1)",
        "    local n = getn(tb)",
        "    local nComplete = 1",
        "    if n > MAILMGR_PAGE then",
        "        nComplete = 0",
        "        n = MAILMGR_PAGE",
        "    end",
        "    local tbList = {}",
        "    for i = 1, n do",
        "        local r = tb[i]",
        "        tbList[r.id] = {szSender = r.sender, szTitle = r.title, nState = r.state, nExpiredTime = r.expire,",
        "            nSendTime = r.send, nAwardCount = r.award_count}",
        "    end",
        "    return tbList, nComplete",
        "end",
        "",
        "function MailManager_PushHeaders(nPlayerIdx, szRole, nMinId)",
        "    if nPlayerIdx <= 0 then",
        "        return",
        "    end",
        "    local tbList, nComplete = MailManager_Headers(szRole, nMinId)",
        "    local h = OB_Create()",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nMinId or 0)",
        "    ObjBuffer:PushByType(h, OBJTYPE_TABLE, tbList)",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nComplete)",
        "    MailManager_SendTo(nPlayerIdx, \"emSCRIPT_PROTOCOL_MAIL_HEADERLIST\", h)",
        "    OB_Release(h)",
        "    -- [D4] thu chua giao (state 0) trong trang nay -> tra id lon nhat de bao NEWMAIL (client tu mo hop thu)",
        "    local nNewId = 0",
        "    for nId, tb in tbList do",
        "        if tb.nState == 0 and nId > nNewId then",
        "            nNewId = nId",
        "        end",
        "    end",
        "    return nNewId",
        "end",
        "",
        "function MailManager_ReplyState(nId, nToState, nOk)",
        "    local h = OB_Create()",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nId)",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nToState)",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nOk)",
        "    SendScriptData(ScriptProtocol[\"emSCRIPT_PROTOCOL_MAIL_STATECHANGE\"], h)",
        "    OB_Release(h)",
        "end",
        "",
        "function MailManager_ReplyDelete(nId, nReason)",
        "    local h = OB_Create()",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nId)",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nReason)",
        "    SendScriptData(ScriptProtocol[\"emSCRIPT_PROTOCOL_MAIL_DELETE\"], h)",
        "    OB_Release(h)",
        "end",
        "",
        "-- ======== handler (PlayerIndex da dat) ========",
        "function MailManager_OnRequestHeaderList(nMinId)",
        "    MailManager_PushHeaders(PlayerIndex, GetName(), nMinId or 0)",
        "end",
        "",
        "function MailManager_OnRequestWholeMail(nId)",
        "    local szRole = GetName()",
        "    local r = MailDB_Get(szRole, nId)",
        "    if not r then",
        "        Msg2Player(\"" + V("Thư không tồn tại!") + "\")",
        "        return",
        "    end",
        "    local tbAward = MailManager_ParseAward(r.award)",
        "    local tbMail = {",
        "        nPrivateId = r.id, szRoleName = szRole, szSender = r.sender, szTitle = r.title,",
        "        szDescribe = strsub(r.content or \"\", 1, MAILMGR_MAX_CONTENT),",
        "        nSendTime = r.send, nCacheTime = r.send, nRecvTime = r.send, nExpiredTime = r.expire,",
        "        nState = r.state, tbAward = {}, nAwardCount = r.award_count,",
        "        tbAwardInfo = MailManager_AwardInfo(tbAward),",
        "    }",
        "    local h = OB_Create()",
        "    ObjBuffer:PushByType(h, OBJTYPE_TABLE, tbMail)",
        "    SendScriptData(ScriptProtocol[\"emSCRIPT_PROTOCOL_MAIL_WHOLEMAIL\"], h)",
        "    OB_Release(h)",
        "end",
        "",
        "-- [D9] khoa / han dung sau khi tao vat pham (giong tasklink_award.lua tl_linkaward_give)",
        "function MailManager_ItemPost(nIdx, nLock, nExpSec)",
        "    if nLock and nLock ~= 0 then",
        "        SetPlayerItemLock(nIdx, nLock)",
        "    end",
        "    if nExpSec and nExpSec > 0 then",
        "        AddTimeItem(nIdx, nExpSec)",
        "    end",
        "end",
        "",
        "-- [D9] trao 1 muc item/gold; tra so mon da tao",
        "function MailManager_GiveItem(a)",
        "    local nOk = 0",
        "    if a.szKind == \"gold\" then",
        "        for c = 1, a.nCount do",
        "            local nIdx = AddItem2(2, 0, a.nRecord, 0, 0, 0)",
        "            if nIdx and nIdx > 0 then",
        "                MailManager_ItemPost(nIdx, a.nLock, a.nExpSec)",
        "                nOk = nOk + 1",
        "            else",
        "                GhiLog(\"MAIL\", format(\"AddItem2 hoang kim dong %d that bai cho %s\", a.nRecord, GetName()))",
        "            end",
        "        end",
        "        return nOk",
        "    end",
        "    if a.nStack == 1 and a.nGenre ~= 0 then",
        "        -- 1 chong n mon: AddItem 21 tham so nhu lib_ham.lua AddItemSL (13 = so luong, 20 = khoa)",
        "        local nIdx = AddItem(a.nGenre, a.nDetail, a.nParticular, a.nLevel, a.nSeries, a.nLuck, a.nMagic, 0, 0, 0, 0, 0, a.nCount, 0, 0, 0, 0, 0, 0, a.nLock, 0)",
        "        if nIdx and nIdx > 0 then",
        "            if a.nExpSec > 0 then",
        "                AddTimeItem(nIdx, a.nExpSec)",
        "            end",
        "            return a.nCount",
        "        end",
        "        GhiLog(\"MAIL\", format(\"AddItem chong %d,%d,%d x%d that bai cho %s\", a.nGenre, a.nDetail, a.nParticular, a.nCount, GetName()))",
        "        return 0",
        "    end",
        "    for c = 1, a.nCount do",
        "        local nIdx = AddItem(a.nGenre, a.nDetail, a.nParticular, a.nLevel, a.nSeries, a.nLuck, a.nMagic)",
        "        if nIdx and nIdx > 0 then",
        "            MailManager_ItemPost(nIdx, a.nLock, a.nExpSec)",
        "            nOk = nOk + 1",
        "        else",
        "            GhiLog(\"MAIL\", format(\"AddItem that bai %d,%d,%d cho %s\", a.nGenre, a.nDetail, a.nParticular, GetName()))",
        "        end",
        "    end",
        "    return nOk",
        "end",
        "",
        "function MailManager_GiveAward(tbAward)",
        "    for i = 1, getn(tbAward) do",
        "        local a = tbAward[i]",
        "        if a.szKind == \"item\" or a.szKind == \"gold\" then",
        "            MailManager_GiveItem(a)",
        "        elseif a.szKind == \"money\" then",
        "            Earn(a.nCount)",
        "        elseif a.szKind == \"xu\" then",
        "            SetTask(MAILMGR_XU_TASK, GetTask(MAILMGR_XU_TASK) + a.nCount)",
        "        elseif a.szKind == \"exp\" then",
        "            AddOwnExp(a.nCount)",
        "        elseif a.szKind == \"repute\" then",
        "            AddRepute(a.nCount)",
        "        elseif a.szKind == \"task\" then",
        "            SetTask(a.nTaskId, GetTask(a.nTaskId) + a.nCount)",
        "        end",
        "    end",
        "end",
        "",
        "function MailManager_OnRequestStateChange(nId, nToState)",
        "    local szRole = GetName()",
        "    if nToState == MAILDEF.tbState.READED then",
        "        if MailDB_SetState(szRole, nId, MAILDEF.tbState.READED, MAILDEF.tbState.READED) == 1 then",
        "            MailManager_ReplyState(nId, MAILDEF.tbState.READED, 1)",
        "            if MailAutoDel[szRole] == 1 then",
        "                local r = MailDB_Get(szRole, nId)",
        "                if r and r.award_count == 0 and MailDB_Delete(szRole, nId) == 1 then",
        "                    MailManager_ReplyDelete(nId, MAILDEF.tbDeleteReson.REQUEST)",
        "                end",
        "            end",
        "        end",
        "        return",
        "    end",
        "    if nToState ~= MAILDEF.tbState.DRAWED then",
        "        return",
        "    end",
        "    local r = MailDB_Get(szRole, nId)",
        "    if not r then",
        "        Msg2Player(\"" + V("Thư không tồn tại!") + "\")",
        "        return",
        "    end",
        "    local tbAward = MailManager_ParseAward(r.award)",
        "    if getn(tbAward) == 0 then",
        "        Msg2Player(\"" + V("Thư này không có đính kèm!") + "\")",
        "        return",
        "    end",
        "    -- [D9] uoc luong O hanh trang: trang bi (genre 0) va hoang kim 6 o/mon; do khac 1 o/mon, chong = 1 o",
        "    local nCells = 0",
        "    for i = 1, getn(tbAward) do",
        "        local a = tbAward[i]",
        "        if a.szKind == \"item\" then",
        "            if a.nGenre == 0 then",
        "                nCells = nCells + a.nCount * 6",
        "            elseif a.nStack == 1 then",
        "                nCells = nCells + 1",
        "            else",
        "                nCells = nCells + a.nCount",
        "            end",
        "        elseif a.szKind == \"gold\" then",
        "            nCells = nCells + a.nCount * 6",
        "        end",
        "    end",
        "    if nCells > 0 and CalcFreeItemCellCount(1, 1) < nCells then",
        "        Msg2Player(\"" + V("Hành trang không đủ chỗ trống (cần ") + "\"..nCells..\"" + V(" ô), hãy dọn bớt rồi nhận lại!") + "\")",
        "        return",
        "    end",
        "    -- nguyen tu: chi mot lan doi duoc state < 3 -> 3",
        "    if MailDB_SetState(szRole, nId, MAILDEF.tbState.DRAWED, MAILDEF.tbState.DRAWED) ~= 1 then",
        "        Msg2Player(\"" + V("Đính kèm đã được nhận rồi!") + "\")",
        "        return",
        "    end",
        "    MailManager_GiveAward(tbAward)",
        "    MailManager_ReplyState(nId, MAILDEF.tbState.DRAWED, 1)",
        "    Msg2Player(\"" + V("Đã nhận đính kèm trong thư.") + "\")",
        "    GhiLog(\"MAIL\", format(\"%s nhan dinh kem thu %d: %s\", szRole, nId, r.award or \"\"))",
        "end",
        "",
        "function MailManager_OnRequestDelete(nId)",
        "    if MailDB_Delete(GetName(), nId) == 1 then",
        "        MailManager_ReplyDelete(nId, MAILDEF.tbDeleteReson.REQUEST)",
        "    end",
        "end",
        "",
        "function MailManager_OnRequestAutoDelete(nHandle)",
        "    local szRole = GetName()",
        "    if MailAutoDel[szRole] == 1 then",
        "        MailAutoDel[szRole] = 0",
        "    else",
        "        MailAutoDel[szRole] = 1",
        "    end",
        "end",
        "",
        "function MailManager_OnRequestOpenUrl(szUrl)",
        "    -- chu chot: bo tinh nang mo URL",
        "end",
        "",
        "-- ======== goi tu NPC / dang nhap / script khac (PlayerIndex da dat) ========",
        "function MailManager_OpenWindow()",
        "    local szRole = GetName()",
        "    GhiLog(\"MAIL\", format(\"%s mo hop thu (OPENWINDOW, player %d)\", szRole, PlayerIndex))",
        "    MailManager_PushHeaders(PlayerIndex, szRole, 0)",
        "    local h = OB_Create()",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, 1)",
        "    SendScriptData(ScriptProtocol[\"emSCRIPT_PROTOCOL_MAIL_OPENWINDOW\"], h)",
        "    OB_Release(h)",
        "end",
        "",
        "function MailManager_OnLogin()",
        "    -- [D7] bot (PB_IsBot) khong co ket noi: bo qua, khoi ghi 'THAT BAI' vao hethong.log",
        "    if IsBot and IsBot(PlayerIndex) == 1 then",
        "        return",
        "    end",
        "    local szRole = GetName()",
        "    local nCount = MailDB_Count(szRole)",
        "    GhiLog(\"MAIL\", format(\"%s dang nhap: %d thu\", szRole, nCount))",
        "    -- [D4] LUON gui trang header dau (co the rong) de client tao bieu tuong thu duoi Bau Cua",
        "    local nNewId = MailManager_PushHeaders(PlayerIndex, szRole, 0)",
        "    if type(nNewId) == \"number\" and nNewId > 0 then",
        "        -- thu giao luc offline = thu moi: bao NEWMAIL de client tu mo hop thu",
        "        local h = OB_Create()",
        "        ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nNewId)",
        "        MailManager_SendTo(PlayerIndex, \"emSCRIPT_PROTOCOL_MAIL_NEWMAIL\", h)",
        "        OB_Release(h)",
        "    end",
        "end",
        "",
        "-- thu moi cho nguoi choi dang online (web admin / SendMail): bao NEWMAIL + gui header thu do",
        "function MailManager_NotifyNew(szRole, nId)",
        "    local nIdx = FindPlayer(szRole)",
        "    if type(nIdx) ~= \"number\" or nIdx <= 0 then",
        "        return 0",
        "    end",
        "    local h = OB_Create()",
        "    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, nId)",
        "    MailManager_SendTo(nIdx, \"emSCRIPT_PROTOCOL_MAIL_NEWMAIL\", h)",
        "    OB_Release(h)",
        "    MailManager_PushHeaders(nIdx, szRole, nId - 1)",
        "    return 1",
        "end",
        "",
        "-- API cho hoat dong / top tuan-thang / dua top: MailManager_SendMail(ten, nguoi gui, tieu de, noi dung, dinh kem, so ngay, nguon)",
        "-- noi dung xuong dong bang <enter>; dinh kem theo dinh dang o dau tep; tra id thu (0 = loi)",
        "function MailManager_SendMail(szRole, szSender, szTitle, szContent, szAward, nExpireDays, szSource)",
        "    local tbAward = MailManager_ParseAward(szAward or \"\")",
        "    local nId = MailDB_Send(szRole, szSender or MAILMGR_SENDER_SYS, szTitle or \"\", szContent or \"\", szAward or \"\",",
        "        getn(tbAward), (nExpireDays or 30) * 86400, szSource or \"script\")",
        "    if nId > 0 then",
        "        MailManager_NotifyNew(szRole, nId)",
        "    else",
        "        GhiLog(\"MAIL\", format(\"MailDB_Send that bai cho %s: %s\", szRole or \"?\", szTitle or \"\"))",
        "    end",
        "    return nId",
        "end",
        "",
        "-- lenh bai admin: gui thu thu cho chinh minh (1 = tien/xu/exp, 2 = them vat pham 6,1,4139)",
        "-- [D8] nguoi gui theo HOAT DONG (chu 03/09: 'moi loai hoat dong ten khac nhau'); them hoat dong = them 1 dong",
        "MAILMGR_ACTIVITY = {",
        "    tongkim   = \"" + V("Tống Kim") + "\",",
        "    congthanh = \"" + V("Công Thành Chiến") + "\",",
        "    liendau   = \"" + V("Liên Đấu") + "\",",
        "    datau     = \"" + V("Dã Tẩu") + "\",",
        "    boss      = \"" + V("Boss Hoàng Kim") + "\",",
        "    viemde    = \"" + V("Viêm Đế") + "\",",
        "    hoatdong  = \"" + V("Hoạt Động") + "\",",
        "    toptuan   = \"" + V("Thưởng Top Tuần") + "\",",
        "    topthang  = \"" + V("Thưởng Top Tháng") + "\",",
        "    duatop    = \"" + V("Sự Kiện Đua Top") + "\",",
        "    phonglangdo = \"" + V("Phong Lăng Độ") + "\",",
        "    vuotai    = \"" + V("Vượt ải") + "\",",
        "    tinsu     = \"" + V("Tín Sứ") + "\",",
        "    bangluong = \"" + V("Bang hội") + "\",",
        "    web       = \"" + V("Nhà phát hành") + "\",",
        "}",
        "",
        "-- MailManager_SendReward(szActivity, szRole, szTitle, szContent, szAward, nDays) -> id thu (0 = loi)",
        "--   szActivity: khoa MAILMGR_ACTIVITY (tongkim, congthanh, liendau, datau, boss, viemde, hoatdong, toptuan, topthang,",
        "--               duatop, web) hoac ten nguoi gui tu do; cung ghi vao cot `source` de web/log loc theo hoat dong.",
        "--   szRole nil/\"\" = nguoi choi dang goi script (PlayerIndex); szAward theo dinh dang dau tep; nDays mac dinh 30.",
        "-- Vi du (trong script hoat dong, sau Include(\"\\\\script\\\\mail\\\\mailmanager.lua\")):",
        "--   MailManager_SendReward(\"tongkim\", nil, \"Thuong Tong Kim\", \"Chuc mung...<enter>Tran trong\", \"money:5000;exp:100000\")",
        "function MailManager_SendReward(szActivity, szRole, szTitle, szContent, szAward, nDays)",
        "    local szSender = MAILMGR_ACTIVITY[szActivity or \"\"] or szActivity or MAILMGR_SENDER_SYS",
        "    if not szRole or szRole == \"\" then",
        "        szRole = GetName()",
        "    end",
        "    return MailManager_SendMail(szRole, szSender, szTitle or szSender, szContent or \"\", szAward or \"\", nDays or 30, szActivity or \"script\")",
        "end",
        "",
        "-- [D9] Dung chuoi award tu bang kieu templet (awardtemplet/item_jx1): moi phan tu la mot trong:",
        "--   {tbProp={g,d,p,l,s,k}, nCount, nBindState(-2 = khoa), nExpSec, nMagic, nStack(1 = 1 chong)}",
        "--   {szKind=\"gold\", nRecord, nCount, nBindState, nExpSec}   {szKind=\"task\", nTaskId, nCount}",
        "--   {szKind=\"money\"|\"xu\"|\"exp\"|\"repute\", nCount}",
        "function MailManager_BuildAward(tbList)",
        "    local tb = {}",
        "    for i = 1, getn(tbList) do",
        "        local a = tbList[i]",
        "        if type(a) == \"table\" then",
        "            if a.tbProp then",
        "                local p = a.tbProp",
        "                tinsert(tb, format(\"item:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\", p[1] or 0, p[2] or 0, p[3] or 0, p[4] or 0, p[5] or 0, p[6] or 0,",
        "                    floor(a.nCount or 1), a.nBindState or 0, floor(a.nExpSec or 0), a.nMagic or 0, a.nStack or 0))",
        "            elseif a.szKind == \"gold\" then",
        "                tinsert(tb, format(\"gold:%d,%d,%d,%d\", a.nRecord or 0, floor(a.nCount or 1), a.nBindState or 0, floor(a.nExpSec or 0)))",
        "            elseif a.szKind == \"task\" then",
        "                tinsert(tb, format(\"task:%d,%d\", a.nTaskId or 0, floor(a.nCount or 0)))",
        "            elseif a.szKind and a.nCount and floor(a.nCount) > 0 then",
        "                tinsert(tb, format(\"%s:%d\", a.szKind, floor(a.nCount)))",
        "            end",
        "        end",
        "    end",
        "    local sz = \"\"",
        "    for i = 1, getn(tb) do",
        "        if i > 1 then",
        "            sz = sz..\";\"",
        "        end",
        "        sz = sz..tb[i]",
        "    end",
        "    return sz, getn(tb)",
        "end",
        "",
        "-- [D9] Gui thu thuong hoat dong tu bang kieu templet. szRole nil = nguoi choi dang goi. Tra id thu (0 = loi/rong).",
        "function MailManager_SendRewardTemplet(szActivity, szRole, szTitle, szContent, tbList, nDays)",
        "    local szAward, n = MailManager_BuildAward(tbList or {})",
        "    if n <= 0 then",
        "        return 0",
        "    end",
        "    return MailManager_SendReward(szActivity, szRole, szTitle, szContent, szAward, nDays)",
        "end",
        "",
        "function MailManager_SendTest(nKind)",
        "    local szAward = \"money:10000;xu:10;exp:50000\"",
        "    if nKind == 2 then",
        "        szAward = \"item:6,1,4139,0,0,0,1;\"..szAward",
        "    end",
        "    local szContent = \"" + V("Đại hiệp thân mến,") + "<enter>\"",
        "        ..\"" + V("Đây là thư thử của hệ thống thư mới. Hãy bấm Nhận để lấy đính kèm.") + "<enter>\"",
        "        ..\"" + V("Trân trọng") + "\"",
        "    local nId = MailManager_SendMail(GetName(), MAILMGR_SENDER_NPH, \"" + V("Thư thử hệ thống thư") + "\", szContent, szAward, 30, \"gm\")",
        "    Msg2Player(\"" + V("Đã gửi thư thử (id ") + "\"..nId..\"). " + V("Hộp thư sẽ tự mở; hoặc bấm biểu tượng thư dưới Bầu Cua.") + "\")",
        "end",
        "",
    ]
    return E.join(L)


# ---------------------------------------------------------------- mailpoll.lua
def build_mailpoll():
    L = [
        "-- mailpoll.lua - [MAIL 03/09] do thu moi do web admin ghi thang bang `mail` (state 0) moi 30 giay,",
        "-- quet thu het han moi 10 phut. CHI tep nay dang ky timer (AddTimer chay trong state cua tep dang ky,",
        "-- KJx2League.cpp: ham tra so khung thi timer tiep tuc). KHONG Include tep nay o noi khac.",
        "Include(\"\\\\script\\\\mail\\\\mailmanager.lua\")",
        "",
        "MAILPOLL_FRAMES = 30 * 18",
        "MAILPOLL_LASTID = 0",
        "MAILPOLL_TICKS  = 0",
        "MAILPOLL_GLB    = 9001   -- GlbValue: chi mot state dang ky timer",
        "",
        "function MailPoll_Tick(nParam, nTimerId)",
        "    if MAILPOLL_LASTID == 0 then",
        "        -- lan dau: chi bao thu ghi SAU khi may chu chay; thu cu giao luc nguoi choi dang nhap",
        "        MAILPOLL_LASTID = MailDB_MaxId() or 0",
        "    end",
        "    local tb = MailDB_PollNew(MAILPOLL_LASTID, 50)",
        "    for i = 1, getn(tb) do",
        "        local r = tb[i]",
        "        if r.id > MAILPOLL_LASTID then",
        "            MAILPOLL_LASTID = r.id",
        "        end",
        "        MailManager_NotifyNew(r.role, r.id)",
        "    end",
        "    MAILPOLL_TICKS = MAILPOLL_TICKS + 1",
        "    if MAILPOLL_TICKS >= 20 then",
        "        MAILPOLL_TICKS = 0",
        "        MailDB_Sweep()",
        "    end",
        "    return MAILPOLL_FRAMES",
        "end",
        "",
        "if GetGlbValue(MAILPOLL_GLB) ~= 1 then",
        "    SetGlbValue(MAILPOLL_GLB, 1)",
        "    AddTimer(MAILPOLL_FRAMES, \"MailPoll_Tick\", 0)",
        "end",
        "",
    ]
    return E.join(L)


# ---------------------------------------------------------------- va cac tep co san
def patch_def_gs():
    p = os.path.join(SV, r"script\script_protocol\protocol_def_gs.lua")
    s = rd(p)
    if "MAIL_REQUEST_HEADERLIST" in s:
        print("  da va:", p)
        return s
    e = "\r\n" if "\r\n" in s else "\n"
    anchor = "ScriptProtocol:RegProtocolSet(Def)"
    assert s.count(anchor) == 1
    i = s.index(anchor)
    # dong '}' dong bang Def ngay truoc anchor
    j = s.rfind(e + "}" + e, 0, i)
    assert j > 0
    block = e.join([
        "\t-- [MAIL 03/09] he THU (uimail.lua 2.0 gui) -> \\script\\mail\\mailmanager.lua",
        "\t{ \"emSCRIPT_PROTOCOL_MAIL_REQUEST_HEADERLIST\", \"\\\\script\\\\mail\\\\mailmanager.lua\", \"MailManager_OnRequestHeaderList\", {OBJTYPE_NUMBER} },",
        "\t{ \"emSCRIPT_PROTOCOL_MAIL_REQUEST_DELETE\", \"\\\\script\\\\mail\\\\mailmanager.lua\", \"MailManager_OnRequestDelete\", {OBJTYPE_NUMBER} },",
        "\t{ \"emSCRIPT_PROTOCOL_MAIL_REQUEST_WHOLEMAIL\", \"\\\\script\\\\mail\\\\mailmanager.lua\", \"MailManager_OnRequestWholeMail\", {OBJTYPE_NUMBER} },",
        "\t{ \"emSCRIPT_PROTOCOL_MAIL_REQUEST_STATECHANGE\", \"\\\\script\\\\mail\\\\mailmanager.lua\", \"MailManager_OnRequestStateChange\", {OBJTYPE_NUMBER, OBJTYPE_NUMBER} },",
        "\t{ \"emSCRIPT_PROTOCOL_MAIL_REQUEST_AUTODELETE\", \"\\\\script\\\\mail\\\\mailmanager.lua\", \"MailManager_OnRequestAutoDelete\", nil },",
        "\t{ \"emSCRIPT_PROTOCOL_MAIL_REQUEST_OPENURL\", \"\\\\script\\\\mail\\\\mailmanager.lua\", \"MailManager_OnRequestOpenUrl\", {OBJTYPE_STRING} },",
    ])
    s2 = s[:j] + e + block + s[j:]
    assert hb(s2) == hb(s)
    wr(p, s2)
    wr(os.path.join(MIRROR, r"script\script_protocol\protocol_def_gs.lua"), s2)
    return s2


def patch_playerlogin():
    p = os.path.join(SV, r"script\player\playerlogin.lua")
    s = rd(p)
    if MARK in s:
        print("  da va:", p)
        return
    e = "\r\n" if "\r\n" in s else "\n"
    a1 = 'Include("\\\\script\\\\global\\\\titlefuncs.lua")'
    assert s.count(a1) == 1, "playerlogin: include anchor"
    s = s.replace(a1, a1 + e + 'Include("\\\\script\\\\mail\\\\mailmanager.lua")\t-- ' + MARK + ' he thu')
    a2 = "\tAddSkillHoTro()"
    assert s.count(a2) == 1, "playerlogin: AddSkillHoTro x%d" % s.count(a2)
    s = s.replace(a2, a2 + e + "\tMailManager_OnLogin()\t-- " + MARK + " co thu -> gui header, client bat bieu tuong bo cau")
    wr(p, s)
    wr(os.path.join(MIRROR, r"script\player\playerlogin.lua"), s)


def patch_dichquan():
    # [D4 03/09] chu: 'khong can giao tiep qua NPC Tin Su' -> GO muc 'Nhan thu' + ham nhanthu da chen o dot 3
    # (idempotent: khong co dau [MAIL 03/09] thi thoi). Hop thu mo bang bieu tuong duoi Bau Cua / tu mo khi co thu.
    p = os.path.join(SV, r"script\global\npcchucnang\dichquan.lua")
    s = rd(p)
    if MARK not in s:
        print("  dichquan: khong con muc thu (da go):", p)
        return
    e = "\r\n" if "\r\n" in s else "\n"
    lines = [l for l in s.split(e) if not ("/nhanthu\"" in l and MARK in l)]
    s = e.join(lines)
    i0 = s.find("-- " + MARK + " hop thu: Tin Su")
    if i0 >= 0:
        i1 = s.find(e + "end", i0)
        assert i1 > i0, "dichquan: khong thay end cua nhanthu"
        i1 += len(e + "end")
        while s[i1:i1 + len(e)] == e:
            i1 += len(e)
        s = s[:i0].rstrip("\r\n") + e + s[i1:]
    assert MARK not in s and "nhanthu" not in s, "dichquan: van con dau vet"
    wr(p, s)
    wr(os.path.join(MIRROR, r"script\global\npcchucnang\dichquan.lua"), s)


def patch_lenhbai():
    p = os.path.join(SV, r"script\item\lenhbaiadmin.lua")
    s = rd(p)
    if "mailtest1" in s:
        print("  da va:", p)
        return
    e = "\r\n" if "\r\n" in s else "\n"
    a1 = '\t\t"Thu kenh ScriptProtocol (ECHO)/specho",\t-- [MAIL 03/09]'
    assert s.count(a1) == 1, "lenhbai: anchor ECHO"
    s = s.replace(a1, a1 + e +
        '\t\t"' + V("Gửi thư thử: tiền, xu, exp") + '/mailtest1",\t-- [MAIL 03/09]' + e +
        '\t\t"' + V("Gửi thư thử: có vật phẩm") + '/mailtest2",\t-- [MAIL 03/09]')
    tail = e.join([
        "",
        "-- [MAIL 03/09] gui thu thu cho chinh minh (mailmanager.lua)",
        "function mailtest1()",
        "\tInclude(\"\\\\script\\\\mail\\\\mailmanager.lua\")",
        "\tMailManager_SendTest(1)",
        "end",
        "",
        "function mailtest2()",
        "\tInclude(\"\\\\script\\\\mail\\\\mailmanager.lua\")",
        "\tMailManager_SendTest(2)",
        "end",
        "",
    ])
    if not s.endswith(e):
        s += e
    s += tail
    wr(p, s)
    wr(os.path.join(MIRROR, r"script\item\lenhbaiadmin.lua"), s)


if __name__ == "__main__":
    wr2(r"script\mail\mailmanager.lua", build_mailmanager())
    wr2(r"script\mail\mailpoll.lua", build_mailpoll())
    # maildef.lua may chu = ban client (2.0 + shim)
    wr2(r"script\mail\maildef.lua", rd(os.path.join(CL, r"script\mail\maildef.lua")))
    patch_def_gs()
    patch_playerlogin()
    patch_dichquan()
    patch_lenhbai()
    print("XONG" + (" (chi kiem tra)" if CHECK else ""))
