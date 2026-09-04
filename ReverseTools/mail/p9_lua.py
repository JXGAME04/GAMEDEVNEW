# -*- coding: utf-8 -*-
"""[MAIL 03/09 D9] Thuong hoat dong GUI QUA THU (chu 03/09 toi): Tong Kim, Phong Lang Do, Vuot Ai, Cong Thanh, Da Tau (moc),
Tin Su - chi luc KET THUC hoat dong / tra nhiem vu; + phat luong bang hoi qua thu (NPC Tong quan hoat dong, bang chu).
Va tren cay chay that E:\\...\\bin\\server + guong worktree. Idempotent (dau [MAIL 03/09 D9]). Doc/ghi latin-1, chu Viet = V() TCVN3.
Chay: python p9_lua.py [--check]
"""
import io, os, sys

SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIRROR = r"D:\GAMEDEVNEW_wt_mail\serverscript_jx2\mail\server"
CHECK = "--check" in sys.argv
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes

MARK = "[MAIL 03/09 D9]"
INC = 'Include("\\\\script\\\\mail\\\\mailmanager.lua")'


def V(s):
    return unicode_to_tcvn3_bytes(s).decode("latin-1")


def rd(rel):
    return io.open(os.path.join(SV, rel), "r", encoding="latin-1", newline="").read()


def wr(rel, s):
    for root in (SV, MIRROR):
        p = os.path.join(root, rel)
        if CHECK:
            print("  (check) ghi", p, len(s))
            continue
        d = os.path.dirname(p)
        if not os.path.isdir(d):
            os.makedirs(d)
        io.open(p, "w", encoding="latin-1", newline="").write(s)
        print("  ghi:", p, len(s))


def eol(s):
    return "\r\n" if "\r\n" in s else "\n"


def rep1(s, old, new, label):
    n = s.count(old)
    if n != 1:
        raise AssertionError("%s: neo khop %d lan: %s" % (label, n, ascii(old[:80])))
    return s.replace(old, new)


def slice_replace(s, start_anchor, end_anchor, new, label, include_end=True):
    i0 = s.find(start_anchor)
    if i0 < 0:
        raise AssertionError("%s: khong thay neo dau %s" % (label, ascii(start_anchor[:60])))
    i1 = s.find(end_anchor, i0 + len(start_anchor))
    if i1 < 0:
        raise AssertionError("%s: khong thay neo cuoi %s" % (label, ascii(end_anchor[:60])))
    if include_end:
        i1 += len(end_anchor)
    return s[:i0] + new + s[i1:]


# ------------------------------------------------------------------ 1. Tong Kim (lib_tktc.lua + task03.lua)
def patch_tongkim():
    rel = r"script\tinhnang\tong_kim_tcap\lib_tktc.lua"
    s = rd(rel)
    if MARK in s:
        print("  da va:", rel)
    else:
        e = eol(s)
        new = e.join([
            "-- " + MARK + " Thuong KET THUC Tong Kim gui QUA THU (chu 03/09): vat pham + xu + diem su kien vao thu, kinh nghiem",
            "-- van cong ngay (AddSumExp). So luong giu nguyen ban cu (hoa / thang / thua). Vat pham genre 6 = 1 CHONG (nhu AddItemSL).",
            "MAILTK_ITEMS = {",
            "\thoa   = {{1023,10},{3846,2},{4728,2},{4844,100},{4831,20},{4850,2}},",
            "\tthang = {{1023,30},{3846,5},{4728,5},{4844,500},{4831,50},{4850,10}},",
            "\tthua  = {{1023,20},{3846,3},{4728,3},{4844,300},{4831,30},{4850,5}},",
            "}",
            "function ThuongTongKimQuaThu(szLoai, nRank, nXu, szTieuDe)",
            "\t" + INC,
            "\tlocal tb = {}",
            "\tlocal tbItems = MAILTK_ITEMS[szLoai] or {}",
            "\tfor i = 1, getn(tbItems) do",
            "\t\ttinsert(tb, {tbProp = {6, 1, tbItems[i][1], 0, 0, 0}, nCount = tbItems[i][2], nStack = 1})",
            "\tend",
            "\ttinsert(tb, {szKind = \"xu\", nCount = nXu})",
            "\ttinsert(tb, {szKind = \"task\", nTaskId = TASK_DSK, nCount = 50})",
            "\tlocal szND = \"" + V("Đại hiệp thân mến,<enter>Trận Tống Kim vừa kết thúc, phần thưởng của ngươi (hạng ") + "\"..nRank..\"" + V(") đính kèm trong thư.<enter>Trân trọng") + "\"",
            "\tlocal nId = MailManager_SendRewardTemplet(\"tongkim\", nil, szTieuDe, szND, tb, 30)",
            "\tif nId > 0 then",
            "\t\tMsg2Player(\"" + V("Phần thưởng Tống Kim đã gửi vào hộp thư (") + "\"..getn(tb)..\"" + V(" mục).") + "\")",
            "\telse",
            "\t\tMsg2Player(\"" + V("Gửi thư thưởng Tống Kim thất bại, hãy báo GM.") + "\")",
            "\t\tGhiLog(\"MAIL\", format(\"TongKim %s hang %d: gui thu that bai cho %s\", szLoai, nRank, GetName()))",
            "\tend",
            "\tlogRutXuTK(format(\"***Account [%s] nhan vat [%s] nhan %d Xu qua thu (Tong Kim %s hang %d).***\", GetAccount(), GetName(), nXu, szLoai, nRank))",
            "end",
            "function ThuongHaiPheHoaTongKim(nRank)",
            "\tAddSumExp(5000000 * nRank * 2)",
            "\tThuongTongKimQuaThu(\"hoa\", nRank, 2, \"" + V("Thưởng Tống Kim - hai phe hoà") + "\")",
            "end",
            "function ThuongPheThangTongKim(nRank)",
            "\tAddSumExp(15000000 * nRank * 3)",
            "\tThuongTongKimQuaThu(\"thang\", nRank, 5, \"" + V("Thưởng Tống Kim - phe thắng") + "\")",
            "end",
            "function ThuongPheThuaTongKim(nRank)",
            "\tAddSumExp(10000000 * nRank * 1)",
            "\tThuongTongKimQuaThu(\"thua\", nRank, 2, \"" + V("Thưởng Tống Kim - phe thua") + "\")",
            "end",
            "",
        ])
        s = slice_replace(s, "function ThuongHaiPheHoaTongKim(nRank)", "function reset_savetask_tongkim_tk()", new + "function reset_savetask_tongkim_tk()", "lib_tktc", True)
        wr(rel, s)
    # task03.lua: 3000 diem -> 2 Chan Nguyen Don
    rel2 = r"script\timertask\task03.lua"
    s2 = rd(rel2)
    if MARK in s2:
        print("  da va:", rel2)
        return
    e = eol(s2)
    old = "\t\tAddItemSL(4846, 2, 0)" + e + "\t\tMsg2Player(\"" + V("Đạt trên 3000 điểm tích lũy trận: nhận 2 Chân Nguyên Đơn (trung).") + "\")"
    new = e.join([
        "\t\t-- " + MARK + " 3000 diem tich luy tran -> 2 Chan Nguyen Don (trung) gui qua thu",
        "\t\t" + INC,
        "\t\tMailManager_SendRewardTemplet(\"tongkim\", nil, \"" + V("Thưởng 3000 điểm tích lũy Tống Kim") + "\", \"" + V("Đạt trên 3000 điểm tích lũy trận: 2 Chân Nguyên Đơn (trung) đính kèm trong thư.") + "\", {{tbProp = {6, 1, 4846, 0, 0, 0}, nCount = 2, nStack = 1}}, 30)",
        "\t\tMsg2Player(\"" + V("Đạt trên 3000 điểm tích lũy trận: 2 Chân Nguyên Đơn (trung) đã gửi vào hộp thư.") + "\")",
    ])
    s2 = rep1(s2, old, new, "task03 3000 diem")
    wr(rel2, s2)


# ------------------------------------------------------------------ 2. Phong Lang Do (mission.lua: cap ben)
def patch_pld():
    rel = r"script\missions\fengling_ferry\mission.lua"
    s = rd(rel)
    if MARK in s:
        print("  da va:", rel)
        return
    e = eol(s)
    old = "\t\t\ttbAwardTemplet:GiveAwardByList({tbProp={6,1,3361,1,0,0},}, \"fenglingdu_shuizeicaibao\", HD_CFG(\"HD3_PLD_THUONG_CAPBEN\", 2))"
    new = e.join([
        "\t\t\t-- " + MARK + " thuong cap ben gui qua thu (khong can cho trong tui)",
        "\t\t\t" + INC,
        "\t\t\tMailManager_SendRewardTemplet(\"phonglangdo\", nil, \"" + V("Thưởng cập bến Phong Lăng Độ") + "\", \"" + V("Đại hiệp đã cập bến an toàn, phần thưởng đính kèm trong thư.<enter>Trân trọng") + "\", {{tbProp = {6, 1, 3361, 1, 0, 0}, nCount = HD_CFG(\"HD3_PLD_THUONG_CAPBEN\", 2)}}, 30)",
        "\t\t\tMsg2Player(\"" + V("Phần thưởng cập bến Phong Lăng Độ đã gửi vào hộp thư.") + "\")",
    ])
    s = rep1(s, old, new, "PLD cap ben")
    wr(rel, s)


# ------------------------------------------------------------------ 3. Vuot Ai (award.lua: kinh nghiem hoan thanh + vat pham ai an)
def patch_vuotai():
    rel = r"script\missions\challengeoftime\award.lua"
    s = rd(rel)
    if MARK in s:
        print("  da va:", rel)
        return
    e = eol(s)
    old1 = "\t\tAddOwnExp(point);" + e + "\t\tMsg2Player(\"<#> " + V("Bạn nhận được ") + "\" .. point .. \"" + V(" điểm kinh nghiệm.") + "\");"
    new1 = e.join([
        "\t\t-- " + MARK + " kinh nghiem hoan thanh Vuot Ai gui qua thu",
        "\t\t" + INC,
        "\t\tMailManager_SendRewardTemplet(\"vuotai\", nil, \"" + V("Thưởng hoàn thành Vượt ải") + "\", \"" + V("Chúc mừng đại hiệp đã vượt ải, kinh nghiệm đính kèm trong thư.<enter>Trân trọng") + "\", {{szKind = \"exp\", nCount = floor(point)}}, 30)",
        "\t\tMsg2Player(\"<#> " + V("Phần thưởng ") + "\" .. floor(point) .. \"" + V(" điểm kinh nghiệm Vượt ải đã gửi vào hộp thư.") + "\");",
    ])
    s = rep1(s, old1, new1, "VA exp")
    old2 = "\t\tAddItem(item[2], item[3], item[4], item[5], item[6], item[7], 0);"
    new2 = e.join([
        "\t\t-- " + MARK + " vat pham (ai an / ket thuc) gui qua thu",
        "\t\t" + INC,
        "\t\tMailManager_SendRewardTemplet(\"vuotai\", nil, \"" + V("Thưởng Vượt ải: ") + "\"..name, \"" + V("Phần thưởng Vượt ải đính kèm trong thư.<enter>Trân trọng") + "\", {{tbProp = {item[2], item[3], item[4], item[5], item[6], item[7]}, nCount = 1}}, 30)",
    ])
    s = rep1(s, old2, new2, "VA item")
    wr(rel, s)


# ------------------------------------------------------------------ 4. Cong Thanh (lib_ctc.lua)
def patch_congthanh():
    rel = r"script\tinhnang\congthanhchien\lib_ctc.lua"
    s = rd(rel)
    if MARK in s:
        print("  da va:", rel)
        return
    e = eol(s)
    old = "\tAddItem(6,1,1075,1,1,0);"
    new = e.join([
        "\t-- " + MARK + " thuong ket thuc Cong Thanh gui qua thu",
        "\t" + INC,
        "\tMailManager_SendRewardTemplet(\"congthanh\", nil, \"" + V("Thưởng Công Thành Chiến") + "\", \"" + V("Đại hiệp đạt trên 1000 điểm công trạng, phần thưởng đính kèm trong thư.<enter>Trân trọng") + "\", {{tbProp = {6, 1, 1075, 1, 1, 0}, nCount = 1}}, 30)",
    ])
    s = rep1(s, old, new, "CTC item")
    wr(rel, s)


# ------------------------------------------------------------------ 5. Da Tau (tasklink_award.lua: moc; seasonnpc.lua: 40 nhiem vu)
def patch_datau():
    rel = r"script\task\newtask\tasklink\tasklink_award.lua"
    s = rd(rel)
    if MARK in s:
        print("  da va:", rel)
    else:
        e = eol(s)
        # tep tron CRLF/LF (dong tren CRLF, vong cuoi LF): neo bang regex \r?\n
        import re
        m0 = re.search(r"\tfor i=1, getn\(tbCol\) do\r?\n\t\tnCanO = nCanO \+ tl_linkaward_cells\(tbCol\[i\]\)\r?\n\tend", s)
        m1 = re.search(r"\tfor i=1, getn\(tbCol\) do\r?\n\t\ttl_linkaward_give\(tbCol\[i\], nTask\)\r?\n\tend\r?\n(\r?\n)?end", s)
        if not m0 or not m1 or m1.start() < m0.start():
            raise AssertionError("tasklink moc: khong thay neo (m0=%s m1=%s)" % (bool(m0), bool(m1)))
        new = e.join([
            "\t-- " + MARK + " thuong MOC Da Tau gui QUA THU: giu dung so luong / khoa / han tung dong, khong can cho trong tui",
            "\ttl_linkaward_mail(tbCol, nTask)",
            "end",
            "-- " + MARK + " gom cac dong cua moc thanh 1 thu (gold: hoang kim theo dong goldequip.txt nhu tl_linkaward_give)",
            "function tl_linkaward_mail(tbCol, nTask)",
            "\t" + INC,
            "\tlocal tb = {}",
            "\tfor i = 1, getn(tbCol) do",
            "\t\tlocal nCol = tbCol[i]",
            "\t\tlocal nQual   = tonumber(TabFile_GetCell(TL_AWARDLINK, nCol, \"Quality\"))",
            "\t\tlocal nGenre  = tonumber(TabFile_GetCell(TL_AWARDLINK, nCol, \"Genre\")) or 0",
            "\t\tlocal nDetail = tonumber(TabFile_GetCell(TL_AWARDLINK, nCol, \"Detail\")) or 0",
            "\t\tlocal nPart   = tonumber(TabFile_GetCell(TL_AWARDLINK, nCol, \"Particular\")) or 0",
            "\t\tlocal nLevel  = tonumber(TabFile_GetCell(TL_AWARDLINK, nCol, \"Level\")) or 0",
            "\t\tlocal nFive   = tonumber(TabFile_GetCell(TL_AWARDLINK, nCol, \"GoodsFive\")) or 0",
            "\t\tlocal nMagic  = tonumber(TabFile_GetCell(TL_AWARDLINK, nCol, \"Magiclevel\")) or 0",
            "\t\tlocal nCount  = tl_linkaward_count(nCol)",
            "\t\tlocal nLock   = tonumber(TabFile_GetCell(TL_AWARDLINK, nCol, \"LockType\")) or 0",
            "\t\tlocal nExpDay = tonumber(TabFile_GetCell(TL_AWARDLINK, nCol, \"ExpDay\")) or 0",
            "\t\tif (nQual == 1) then",
            "\t\t\ttinsert(tb, {szKind = \"gold\", nRecord = nGenre, nCount = nCount, nBindState = nLock, nExpSec = nExpDay * 86400})",
            "\t\telse",
            "\t\t\ttinsert(tb, {tbProp = {nGenre, nDetail, nPart, nLevel, nFive, 0}, nCount = nCount, nBindState = nLock, nExpSec = nExpDay * 86400, nMagic = nMagic})",
            "\t\tend",
            "\tend",
            "\tlocal nId = MailManager_SendRewardTemplet(\"datau\", nil, \"" + V("Thưởng mốc ") + "\"..nTask..\"" + V(" nhiệm vụ Dã Tẩu") + "\", \"" + V("Chúc mừng đại hiệp hoàn thành mốc ") + "\"..nTask..\"" + V(" nhiệm vụ Dã Tẩu, phần thưởng đính kèm trong thư.<enter>Trân trọng") + "\", tb, 30)",
            "\tif (nId > 0) then",
            "\t\tMsg2Player(\"" + V("Phần thưởng mốc ") + "\"..nTask..\"" + V(" Dã Tẩu đã gửi vào hộp thư (") + "\"..getn(tb)..\"" + V(" mục).") + "\")",
            "\t\tTaskLink_WriteLog(nTask, \"gui thu id \"..nId..\" (\"..getn(tb)..\" dong)\")",
            "\telse",
            "\t\tWriteLog(\"[DaTau moc \"..nTask..\"] GUI THU THAT BAI nguoi \"..(GetName() or \"?\"))",
            "\tend",
            "end",
        ])
        s = s[:m0.start()] + new + s[m1.end():]
        wr(rel, s)
    rel2 = r"script\global\seasonnpc.lua"
    s2 = rd(rel2)
    if MARK in s2:
        print("  da va:", rel2)
        return
    e = eol(s2)
    old = "\t\ttbAwardTemplet:GiveAwardByList({tbProp = {6, 1, 2383, 1, 0, 0}, nCount = 5}, \"seasonnpc_40task_ruong\")"
    new = e.join([
        "\t\t-- " + MARK + " 5 ruong moc 40 nhiem vu/ngay gui qua thu",
        "\t\t" + INC,
        "\t\tMailManager_SendRewardTemplet(\"datau\", nil, \"" + V("Thưởng 40 nhiệm vụ Dã Tẩu trong ngày") + "\", \"" + V("Hoàn thành 40 nhiệm vụ Dã Tẩu trong ngày không huỷ, 5 Bảo rương thần bí đính kèm trong thư.<enter>Trân trọng") + "\", {{tbProp = {6, 1, 2383, 1, 0, 0}, nCount = 5}}, 30)",
    ])
    s2 = rep1(s2, old, new, "seasonnpc 40 ruong")
    wr(rel2, s2)


# ------------------------------------------------------------------ 6. Tin Su (posthouse.lua: tra nhiem vu)
def patch_tinsu():
    rel = r"script\task\tollgate\messenger\posthouse.lua"
    s = rd(rel)
    if MARK in s:
        print("  da va:", rel)
        return
    e = eol(s)
    start = "\tif nTodayTaskCount == 1 then" + e + "\t\tfor i = 1, HD_CFG(\"TS_TRA_HANHHIEP\", 3) do"
    end_a = "\"TinSuNhanNguyenLieu\")" + e + "\tend"
    new = e.join([
        "\t-- " + MARK + " thuong tra nhiem vu Tin Su gui QUA THU (Hanh Hiep Lenh lan dau trong ngay, Tin Su Bao Ruong, nguyen lieu kinh mach)",
        "\t" + INC,
        "\tlocal tbThu = {}",
        "\tif nTodayTaskCount == 1 then",
        "\t\ttinsert(tbThu, {tbProp = {6, 1, 2575, 1, 0, 0}, nCount = HD_CFG(\"TS_TRA_HANHHIEP\", 3)})",
        "\tend",
        "\ttinsert(tbThu, {tbProp = {6, 1, 3430, 0, 0, 0}, nCount = HD_CFG(\"TS_TRA_BAORUONG\", 2)})",
        "\tif PlayerFunLib:CheckTaskDaily(3073,2,\"nomsg\",\"<\") == 1 then",
        "\t\tPlayerFunLib:AddTaskDaily(3073,1)",
        "\t\ttinsert(tbThu, {tbProp = {6, 1, 4847, 1, 0, 0}, nCount = 2, nBindState = -2})",
        "\tend",
        "\tlocal nIdThu = MailManager_SendRewardTemplet(\"tinsu\", nil, \"" + V("Thưởng nhiệm vụ Tín Sứ") + "\", \"" + V("Đại hiệp đã hoàn thành nhiệm vụ Tín Sứ lần thứ ") + "\"..nTodayTaskCount..\"" + V(" hôm nay, phần thưởng đính kèm trong thư.<enter>Trân trọng") + "\", tbThu, 30)",
        "\tif nIdThu > 0 then",
        "\t\tMsg2Player(\"" + V("Phần thưởng Tín Sứ đã gửi vào hộp thư.") + "\")",
        "\telse",
        "\t\tMsg2Player(\"" + V("Gửi thư thưởng Tín Sứ thất bại, hãy báo GM.") + "\")",
        "\tend",
    ])
    s = slice_replace(s, start, end_a, new, "posthouse tra nhiem vu", True)
    wr(rel, s)


# ------------------------------------------------------------------ 7. Bang hoi: phat luong (huodong_zongguan.lua + tong_luong.lua)
def build_tong_luong():
    e = "\r\n"
    return e.join([
        "-- tong_luong.lua - " + MARK + " PHAT LUONG BANG HOI qua thu (chu 03/09). Include vao state NPC Tong quan hoat dong",
        "-- (scriptjx2\\tong_vn\\npc\\huodong_zongguan.lua). Bang chu chon muc luong moi thanh vien, tru quy bang (TONG_GetMoney),",
        "-- moi thanh vien (ca offline) nhan 1 thu 'Luong bang hoi' dinh kem Ngan luong; 1 lan/ngay/bang (TONG task 1200 = yyyymmdd).",
        INC,
        "",
        "TONGLUONG_MUC       = {10000, 50000, 100000, 500000}   -- Ngan luong moi thanh vien (menu)",
        "TONGLUONG_TASK_NGAY = 1200                             -- TONG_GetTaskValue: ngay da phat (yyyymmdd)",
        "TONGLUONG_HAN_NGAY  = 30",
        "",
        "function TongLuong_Menu(nTongID)",
        "\tlocal nQuy = TONG_GetMoney(nTongID)",
        "\tlocal nTV = TONG_GetMemberCount(nTongID, -1)",
        "\tSay(\"<#>" + V("Tổng quản hoạt động: Bang chủ muốn làm gì?<enter>Quỹ bang: ") + "\"..nQuy..\"" + V(" Ngân lượng, thành viên: ") + "\"..nTV, 3,",
        "\t\t\"" + V("Phát lương bang hội (qua thư)") + "/TongLuong_ChonMuc\",",
        "\t\t\"" + V("Hoạt động bang") + "/TongLuong_HoatDongCu\",",
        "\t\t\"" + V("Rời khỏi") + "/OnCancelTongLuong\")",
        "end",
        "",
        "function OnCancelTongLuong()",
        "end",
        "",
        "-- duong cu cua NPC (ws_huodong.lua USE_G_1) cho bang chu",
        "function TongLuong_HoatDongCu()",
        "\tlocal _, nTongID = GetTongName()",
        "\tlocal nTypeID = wsGetTypeID(NAME_HUODONG)",
        "\tlocal nWorkshopID = TWS_GetFirstWorkshop(nTongID, nTypeID)",
        "\tUSE_G_1(nTongID, nWorkshopID)",
        "end",
        "",
        "function TongLuong_ChonMuc()",
        "\tlocal _, nTongID = GetTongName()",
        "\tlocal nTV = TONG_GetMemberCount(nTongID, -1)",
        "\tlocal m = TONGLUONG_MUC",
        "\tSay(\"<#>" + V("Chọn mức lương mỗi thành viên (") + "\"..nTV..\"" + V(" người). Tổng trừ quỹ = mức x số thành viên.") + "\", 5,",
        "\t\tm[1]..\"" + V(" Ngân lượng (tổng ") + "\"..(m[1] * nTV)..\")/#TongLuong_Phat(1)\",",
        "\t\tm[2]..\"" + V(" Ngân lượng (tổng ") + "\"..(m[2] * nTV)..\")/#TongLuong_Phat(2)\",",
        "\t\tm[3]..\"" + V(" Ngân lượng (tổng ") + "\"..(m[3] * nTV)..\")/#TongLuong_Phat(3)\",",
        "\t\tm[4]..\"" + V(" Ngân lượng (tổng ") + "\"..(m[4] * nTV)..\")/#TongLuong_Phat(4)\",",
        "\t\t\"" + V("Rời khỏi") + "/OnCancelTongLuong\")",
        "end",
        "",
        "function TongLuong_Phat(nMuc)",
        "\tlocal szTong, nTongID = GetTongName()",
        "\tif nTongID == 0 or TONG_GetMaster(nTongID) ~= GetName() then",
        "\t\tMsg2Player(\"" + V("Chỉ Bang chủ mới được phát lương.") + "\")",
        "\t\treturn",
        "\tend",
        "\tlocal nMoiNguoi = TONGLUONG_MUC[nMuc] or 0",
        "\tif nMoiNguoi <= 0 then",
        "\t\treturn",
        "\tend",
        "\tlocal nHomNay = tonumber(GetLocalDate(\"%Y%m%d\")) or 0",
        "\tif TONG_GetTaskValue(nTongID, TONGLUONG_TASK_NGAY) == nHomNay then",
        "\t\tMsg2Player(\"" + V("Hôm nay bang đã phát lương rồi, mai hãy phát tiếp.") + "\")",
        "\t\treturn",
        "\tend",
        "\tlocal tbTen = {}",
        "\tlocal nMem = TONG_GetFirstMember(nTongID, -1)",
        "\twhile nMem and nMem > 0 do",
        "\t\tlocal szTen = TONGM_GetName(nTongID, nMem)",
        "\t\tif szTen and szTen ~= \"\" then",
        "\t\t\ttinsert(tbTen, szTen)",
        "\t\tend",
        "\t\tnMem = TONG_GetNextMember(nTongID, nMem, -1)",
        "\tend",
        "\tif getn(tbTen) == 0 then",
        "\t\tMsg2Player(\"" + V("Bang chưa có thành viên.") + "\")",
        "\t\treturn",
        "\tend",
        "\tlocal nTong = nMoiNguoi * getn(tbTen)",
        "\tlocal nQuy = TONG_GetMoney(nTongID)",
        "\tif nQuy < nTong then",
        "\t\tMsg2Player(\"" + V("Quỹ bang không đủ: cần ") + "\"..nTong..\"" + V(" Ngân lượng, hiện có ") + "\"..nQuy..\".\")",
        "\t\treturn",
        "\tend",
        "\tif TONG_ApplyAddMoney(nTongID, -nTong) ~= 1 then",
        "\t\tMsg2Player(\"" + V("Trừ quỹ bang thất bại, hãy thử lại.") + "\")",
        "\t\treturn",
        "\tend",
        "\tTONG_ApplySetTaskValue(nTongID, TONGLUONG_TASK_NGAY, nHomNay)",
        "\tlocal nOk = 0",
        "\tlocal szSender = \"" + V("Bang hội ") + "\"..szTong",
        "\tlocal szND = \"" + V("Bang chủ ") + "\"..GetName()..\"" + V(" phát lương cho thành viên bang ") + "\"..szTong..\"" + V(".<enter>Lương kỳ này: ") + "\"..nMoiNguoi..\"" + V(" Ngân lượng, đính kèm trong thư.<enter>Trân trọng") + "\"",
        "\tfor i = 1, getn(tbTen) do",
        "\t\tlocal nId = MailManager_SendMail(tbTen[i], szSender, \"" + V("Lương bang hội") + "\", szND, \"money:\"..nMoiNguoi, TONGLUONG_HAN_NGAY, \"bangluong\")",
        "\t\tif nId > 0 then",
        "\t\t\tnOk = nOk + 1",
        "\t\tend",
        "\tend",
        "\tTONG_ApplyAddEventRecord(nTongID, \"" + V("Bang chủ phát lương ") + "\"..nMoiNguoi..\"" + V(" Ngân lượng cho ") + "\"..nOk..\"" + V(" thành viên (tổng ") + "\"..nTong..\")\")",
        "\tMsg2Player(\"" + V("Đã gửi lương ") + "\"..nMoiNguoi..\"" + V(" Ngân lượng cho ") + "\"..nOk..\"/\"..getn(tbTen)..\"" + V(" thành viên, trừ quỹ bang ") + "\"..nTong..\".\")",
        "\tGhiLog(\"MAIL\", format(\"Bang %s: bang chu %s phat luong %d x %d thanh vien (ok %d), tru quy %d\", szTong, GetName(), nMoiNguoi, getn(tbTen), nOk, nTong))",
        "end",
        "",
    ])


def patch_banghoi():
    wr(r"scriptjx2\tong_vn\tong_luong.lua", build_tong_luong())
    rel = r"scriptjx2\tong_vn\npc\huodong_zongguan.lua"
    s = rd(rel)
    if MARK in s:
        print("  da va:", rel)
        return
    e = eol(s)
    old = "\tlocal nTypeID = wsGetTypeID(NAME_HUODONG)" + e + "\tlocal nWorkshopID = TWS_GetFirstWorkshop(nTongID, nTypeID)" + e + "\tUSE_G_1(nTongID, nWorkshopID)"
    new = e.join([
        "\t-- " + MARK + " bang chu: them menu phat luong bang hoi qua thu (thanh vien khac di duong cu)",
        "\tif TONG_GetMaster(nTongID) == GetName() then",
        "\t\tInclude(\"\\\\scriptjx2\\\\tong_vn\\\\tong_luong.lua\")",
        "\t\tTongLuong_Menu(nTongID)",
        "\t\treturn",
        "\tend",
        old,
    ])
    s = rep1(s, old, new, "huodong_zongguan")
    wr(rel, s)


if __name__ == "__main__":
    ok = True
    for name, fn in [("tongkim", patch_tongkim), ("pld", patch_pld), ("vuotai", patch_vuotai), ("congthanh", patch_congthanh),
                     ("datau", patch_datau), ("tinsu", patch_tinsu), ("banghoi", patch_banghoi)]:
        try:
            print("==", name)
            fn()
        except AssertionError as ex:
            ok = False
            print("  LOI:", ex)
    print("XONG" if ok else "CO LOI")
