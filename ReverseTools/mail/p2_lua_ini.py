# -*- coding: utf-8 -*-
"""[MAIL 03/09] Dot 2 - script client + ini cua so thu.
  - \\script\\ui\\uimail.lua      : ban 2.0 (slistcl 9565EFB1) + vai sua cho JX1 (phan thuong do may chu mo ta,
                                   Include, ClientSay/Msg2Player qua C++)
  - \\script\\mail\\maildef.lua   : ban 2.0 (1F41E8E9) + shim (tblen, dump2str, WriteLog)
  - \\ui\\Ui3\\mail\\*.ini         : 6 tep cua client JX1 cu (ReverseTools\\pak_vltk\\vltk2\\mail_jx1cu), nhan
                                   "Nhan vat pham" -> "Nhan" nhu anh 2.0
Doc/ghi latin-1. Chay: python p2_lua_ini.py [--check]
"""
import io, os, re, shutil, sys

SRC = r"D:\GAMEDEVNEW\ReverseTools\pak_vltk\vltk2"
CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
MIRROR = r"D:\GAMEDEVNEW_wt_mail\serverscript_jx2\mail\client"
CHECK = "--check" in sys.argv
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes


def V(s):
    return unicode_to_tcvn3_bytes(s).decode("latin-1")


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


def rep1(s, old, new, label):
    n = s.count(old)
    assert n == 1, "%s: neo khop %d lan: %r" % (label, n, old[:60])
    return s.replace(old, new)


# ---------------------------------------------------------------- uimail.lua
def build_uimail():
    s = rd(os.path.join(SRC, "mail_vltk2", "uimail.lua"))
    e = "\r\n" if "\r\n" in s else "\n"
    # 1) Include: bo common.lua cua 2.0 (khong co o JX1), maildef.lua chu thuong
    s = rep1(s, 'Include("\\\\script\\\\mail\\\\MailDef.lua")', 'Include("\\\\script\\\\mail\\\\maildef.lua")', "include maildef")
    s = rep1(s, 'Include("\\\\script\\\\lib\\\\common.lua")' + e, "", "include common")
    # 2) WholeMailArrival: phan thuong do MAY CHU mo ta san (tbAwardInfo = {{szIcon, szName, szDesc, nCount}, ...}),
    #    dong goi thanh chuoi "icon|ten|mo ta|so luong\n..." cho UpdateMailDetail (C++ JX1 khong doc bang Lua)
    old = ("        local tbItemInfoList = {}" + e +
           "        local nAwardCount = 0" + e +
           "        if tbCurMail.nAwardCount > 0 and tbCurMail.nState < MAILDEF.tbState.DRAWED then" + e +
           "            tbItemInfoList  = self:GetAwardItemInfo(tbCurMail.tbAward)" + e +
           "            nAwardCount     = tbCurMail.nAwardCount" + e +
           "        end" + e +
           "        UpdateMailDetail(self.nCurrentId, tbCurMail.szSender, tbCurMail.szTitle, tbCurMail.szDescribe, 1, nAwardCount > 0 and 1 or 0, FormatTime2String(\"%Y/%m/%d %H:%M:%S\", tbCurMail.nSendTime), nAwardCount, tbItemInfoList)")
    new = ("        -- [MAIL 03/09 JX1] phan thuong do may chu mo ta san (tbAwardInfo), dong goi chuoi cho C++" + e +
           "        local szAwards = \"\"" + e +
           "        local nAwardCount = 0" + e +
           "        if tbCurMail.nAwardCount > 0 and tbCurMail.nState < MAILDEF.tbState.DRAWED then" + e +
           "            szAwards        = self:PackAwardInfo(tbCurMail.tbAwardInfo)" + e +
           "            nAwardCount     = tbCurMail.nAwardCount" + e +
           "        end" + e +
           "        UpdateMailDetail(self.nCurrentId, tbCurMail.szSender, tbCurMail.szTitle, tbCurMail.szDescribe, 1, nAwardCount > 0 and 1 or 0, FormatTime2String(\"%Y/%m/%d %H:%M:%S\", tbCurMail.nSendTime), nAwardCount, szAwards)")
    s = rep1(s, old, new, "WholeMailArrival")
    # 3) them PackAwardInfo truoc GetAwardItemInfo (giu nguyen GetAwardItemInfo/NewItemTmp cua 2.0, khong dung)
    anchor = "function UIMail:GetAwardItemInfo(tbAward)"
    pack = e.join([
        "-- [MAIL 03/09 JX1] tbAwardInfo do mailmanager.lua may chu gui, moi phan tu la bang:",
        "--   {szKind=\"item\", nGenre, nDetail, nParticular, nLevel, nSeries, nLuck, nCount}  -> vat pham (client dung lai tam)",
        "--   {szKind=\"icon\", szIcon, szName, szDesc, nCount}                              -> Ngan luong / xu / EXP",
        "-- -> \"item|g|d|p|l|s|k|n\\n\" hoac \"icon|spr|ten|mo ta|n\\n\" (C++ KMailClient.cpp tach).",
        "function UIMail:PackAwardInfo(tbAwardInfo)",
        "    local szAll = \"\"",
        "    if type(tbAwardInfo) ~= \"table\" then",
        "        return szAll",
        "    end",
        "    for _, tbInfo in tbAwardInfo do",
        "        if type(tbInfo) == \"table\" then",
        "            if tbInfo.szKind == \"item\" then",
        "                szAll = szAll..\"item|\"..(tbInfo.nGenre or 0)..\"|\"..(tbInfo.nDetail or 0)..\"|\"..(tbInfo.nParticular or 0)",
        "                    ..\"|\"..(tbInfo.nLevel or 0)..\"|\"..(tbInfo.nSeries or 0)..\"|\"..(tbInfo.nLuck or 0)..\"|\"..(tbInfo.nCount or 1)..\"\\n\"",
        "            else",
        "                local szDesc = gsub(gsub(tbInfo.szDesc or \"\", \"|\", \"/\"), \"\\n\", \"<enter>\")",
        "                szAll = szAll..\"icon|\"..(tbInfo.szIcon or \"\")..\"|\"..(tbInfo.szName or \"\")..\"|\"..szDesc..\"|\"..(tbInfo.nCount or 1)..\"\\n\"",
        "            end",
        "        end",
        "    end",
        "    return szAll",
        "end",
        "",
    ])
    s = rep1(s, anchor, pack + anchor, "PackAwardInfo")
    # 4) ClientSay -> MailConfirm (C++ hop xac nhan; ham dong y goi lai qua MAILUI_OP_CONFIRM_RESULT)
    #    2.0: ClientSay(szTitle, getn(tbOption), tbOption) voi tbOption = {"Dong y/g_ConfirmDeleteMail", "De ta suy nghi/no"}
    # 3 cho: xoa 1 thu, xoa cac thu da chon, va bam bieu tuong (den Tin Su)
    n = s.count("ClientSay(szTitle, getn(tbOption), tbOption)")
    assert n == 3, "ClientSay x%d" % n
    s = s.replace("ClientSay(szTitle, getn(tbOption), tbOption)", "MailConfirm(szTitle, tbOption[1], tbOption[2])")
    # 5) OnMailIconClick: JX1 chua co AutoCrossMapFindPath phia client -> chi nhac (giu bang map de sau)
    s = rep1(s, "    AutoCrossMapFindPath(tbAutoFindPath[1], tbAutoFindPath[2], tbAutoFindPath[3])",
             "    -- [MAIL 03/09 JX1] client JX1 chua co AutoCrossMapFindPath: chi nhac, khong tu chay", "findpath")
    # 7) [D4 03/09] chu: bo Tin Su -> bam bieu tuong thu la mo hop thu ngay (thay ca than OnMailIconClick;
    #    g_ConfirmFindMessager + bang map giu nguyen, khong ai goi)
    i0 = s.find("function UIMail:OnMailIconClick()")
    assert i0 >= 0, "OnMailIconClick"
    i1 = s.find(e + "end", i0)
    assert i1 > i0, "OnMailIconClick end"
    s = s[:i0] + e.join([
        "function UIMail:OnMailIconClick()",
        "    -- [MAIL 03/09 JX1 D4] bam bieu tuong thu (duoi Bau Cua) -> mo hop thu ngay, khong can den Tin Su",
        "    self.bHaveNewMail = 0",
        "    self:OpenMailWindow(1)",
        "    self:RequestMailHeaderList()",
    ]) + s[i1:]
    # 8) [D4 03/09] chu: co thu moi (NEWMAIL, ke ca thu giao luc dang nhap) -> hien hop thu luon
    s = rep1(s, "    self.bHaveNewMail = 1" + e + "    self:ReCheckMailIconState()" + e + "end",
             "    self.bHaveNewMail = 1" + e + "    self:ReCheckMailIconState()" + e +
             "    self:OpenMailWindow(1)   -- [MAIL 03/09 JX1 D4] co thu moi -> hien hop thu ngay" + e + "end",
             "NewMail auto open")
    # 6) dau dau tep
    s = ("-- [MAIL 03/09] uimail.lua = ban client VLTK 2.0 (slistcl.pak uid 9565EFB1) + sua cho JX1 (tim \"[MAIL 03/09 JX1]\")." + e +
         "-- Cac ham C++ (KMailClient.cpp): OpenMailWindow AddMailHeader SetMailHeader DeleteOneMail CleanMailAll CleanMailList" + e +
         "-- CleanMailDetail UpdateMailCount UpdateMailDetail SetMailBntStatus SetMailIconVisible NewMailUIEventArrival SelectMail" + e +
         "-- SwitchMailManager SetFilterText FormatTime2String MailConfirm Msg2Player." + e + s)
    return s


# ---------------------------------------------------------------- maildef.lua
def build_maildef():
    s = rd(os.path.join(SRC, "mail_vltk2", "MailDef.lua"))
    e = "\r\n" if "\r\n" in s else "\n"
    tail = e.join([
        "",
        "-- [MAIL 03/09 JX1] shim cho client JX1 (2.0 lay tu common.lua / C++)",
        "if (tblen == nil) then",
        "    function tblen(tb)",
        "        local n = 0",
        "        if type(tb) == \"table\" then",
        "            for _, _ in tb do",
        "                n = n + 1",
        "            end",
        "        end",
        "        return n",
        "    end",
        "end",
        "if (dump2str == nil) then",
        "    function dump2str(v)",
        "        if type(v) == \"table\" then",
        "            local s = \"{\"",
        "            for k, x in v do",
        "                s = s..tostring(k)..\"=\"..dump2str(x)..\",\"",
        "            end",
        "            return s..\"}\"",
        "        end",
        "        return tostring(v)",
        "    end",
        "end",
        "if (WriteLog == nil) then",
        "    function WriteLog(szLog)",
        "        if (GhiLog) then",
        "            GhiLog(\"MAIL\", szLog)",
        "        end",
        "    end",
        "end",
        "",
    ])
    if not s.endswith(e):
        s += e
    return s + tail


# ---------------------------------------------------------------- ini
INI = ["mail_manager.ini", "mail_list.ini", "mail_header.ini", "mail_detail.ini", "mail_icon.ini", "mail_award_item.ini"]


def build_ini(name):
    s = rd(os.path.join(SRC, "mail_jx1cu", "ui3_" + name))
    if name == "mail_list.ini":
        s = rep1(s, "Label=" + V("Nhận vật phẩm"), "Label=" + V("Nhận"), "nhan")
    if name == "mail_icon.ini":
        # [D4 03/09] chu: dat duoi bieu tuong Bau Cua = UiPlayerBar.ini [SpringGame] Left=765 Top=243 50x50 (800x600);
        # 1024: UiMail.cpp neo x = 1024 - 30 nhu UiPlayerBar.cpp
        s = rep1(s, "Left=648", "Left=765", "icon left")
        s = rep1(s, "Top=72", "Top=296", "icon top")
    return s


if __name__ == "__main__":
    out = {
        os.path.join(CL, r"script\ui\uimail.lua"): build_uimail(),
        os.path.join(CL, r"script\mail\maildef.lua"): build_maildef(),
    }
    for nm in INI:
        out[os.path.join(CL, r"ui\Ui3\mail", nm)] = build_ini(nm)
    for p, s in out.items():
        wr(p, s)
        rel = p[len(CL) + 1:]
        wr(os.path.join(MIRROR, rel), s)
    print("XONG" + (" (chi kiem tra)" if CHECK else ""))
