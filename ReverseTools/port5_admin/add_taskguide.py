# -*- coding: utf-8 -*-
r"""[24/08] Them 2 muc vao bang CHI NAM NHIEM VU (F11):
  TaskId 8 = BANG CHIEN (tongwar)  - task 2369..2378 ghi qua nt_setTask (SetTask+SyncTaskValue)
                                     => client doc duoc, khong can dung server.
  TaskId 9 = BACH NHAN LOI DAI     - task 2709 (luot exp/ngay, ma hoa yymmdd*256).
Chuoi TCVN3. Sua: UiTaskGuideStr.h (chuoi), UiTaskGuide.h (khai bao), UiTaskGuide.cpp
(hang so + dispatch + 2 ham dung noi dung + loc task khi doi gia tri), va uitasklist.ini.
"""
import io, os, shutil

SRC = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase"
INI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\Ui\uitasklist.ini"

def rd(p, enc="latin-1"): return io.open(p, "r", encoding=enc, newline="").read()
def wr(p, s, enc="latin-1"): io.open(p, "w", encoding=enc, newline="").write(s)
def bak(p):
    if not os.path.isfile(p + ".truoc_them2muc_2408"):
        shutil.copyfile(p, p + ".truoc_them2muc_2408")

# ---- bang chuyen TCVN3 ----
M = {"á":0xB8,"à":0xB5,"ả":0xB6,"ã":0xB7,"ạ":0xB9,"ă":0xA8,"ắ":0xBE,"ằ":0xBB,"ẳ":0xBC,"ẵ":0xBD,"ặ":0xC6,
     "â":0xA9,"ấ":0xCA,"ầ":0xC7,"ẩ":0xC8,"ẫ":0xC9,"ậ":0xCB,"đ":0xAE,"é":0xD0,"è":0xCC,"ẻ":0xCE,"ẽ":0xCF,
     "ẹ":0xD1,"ê":0xAA,"ế":0xD5,"ề":0xD2,"ể":0xD3,"ễ":0xD4,"ệ":0xD6,"í":0xDD,"ì":0xD7,"ỉ":0xD8,"ĩ":0xDC,
     "ị":0xDE,"ó":0xE3,"ò":0xDF,"ỏ":0xE1,"õ":0xE2,"ọ":0xE4,"ô":0xAB,"ố":0xE8,"ồ":0xE5,"ổ":0xE6,"ỗ":0xE7,
     "ộ":0xE9,"ơ":0xAC,"ớ":0xED,"ờ":0xEA,"ở":0xEB,"ỡ":0xEC,"ợ":0xEE,"ú":0xF3,"ù":0xEF,"ủ":0xF1,"ũ":0xF2,
     "ụ":0xF4,"ư":0xAD,"ứ":0xF8,"ừ":0xF5,"ử":0xF6,"ữ":0xF7,"ự":0xF9,"ý":0xFD,"ỳ":0xFA,"ỷ":0xFB,"ỹ":0xFC,"ỵ":0xFE}
for k, v in list(M.items()):
    if k.upper() != k and k.upper() not in M: M[k.upper()] = v
def V(u):
    return "".join(chr(M[c]) if c in M else c for c in u)

# ================= 1) UiTaskGuideStr.h =================
p = os.path.join(SRC, "UiTaskGuideStr.h")
s = rd(p)
if "TW_NOTASK" in s:
    print("Str.h: da co")
else:
    add = "\n".join([
    "",
    "// ---- [CHI NAM 24/08] Bang Chien (tongwar) - task 2369..2378 ----",
    '#define TW_NOTASK      "' + V("Ngươi chưa tham gia trận Bang Chiến nào. Mùa giải diễn ra vào cuối tháng 10 - đầu tháng 11, mỗi tối một trận.") + '"',
    '#define TW_INFO_FMT    "' + V("Điểm trận này: <color=yellow>%d<color> . Tổng điểm cả mùa: <color=yellow>%d<color> ." ) + '"',
    '#define TW_KILL_FMT    "' + V("Đã hạ: <color=yellow>%d<color> người . Chuỗi hạ cao nhất: <color=yellow>%d<color> ." ) + '"',
    '#define TW_DEATH_FMT   "' + V("Số lần tử vong: <color=yellow>%d<color> / <color=yellow>%d<color> . Hết số mạng sẽ bị đưa khỏi chiến trường." ) + '"',
    '#define TW_RANK_FMT    "' + V("Quân hàm hiện tại: <color=yellow>%s<color> ." ) + '"',
    '#define TW_RANK_0      "' + V("chưa có") + '"',
    '#define TW_RANK_1      "' + V("Binh Sĩ") + '"',
    '#define TW_RANK_2      "' + V("Hiệu Úy") + '"',
    '#define TW_RANK_3      "' + V("Thống Lĩnh") + '"',
    '#define TW_RANK_4      "' + V("Phó Tướng") + '"',
    '#define TW_RANK_5      "' + V("Đại Tướng") + '"',
    "",
    "// ---- [CHI NAM 24/08] Bach Nhan Loi Dai - task 2709 (yymmdd*256 + dem) ----",
    '#define BR_INFO        "' + V("Bách Nhân Lôi Đài mở mỗi ngày từ 12:00 đến 24:00 tại Lôi Đài Hoàng Thành Từ (vào bằng Quan nhạc nhỏ ở Lâm An).") + '"',
    '#define BR_HOWTO       "' + V("Dùng khinh công nhảy lên lôi đài để làm Lôi Chủ. Ở trong bản đồ được cộng kinh nghiệm mỗi 5 phút; giữ được đài thì cộng thêm.") + '"',
    '#define BR_LIMIT_FMT   "' + V("Lượt nhận kinh nghiệm hôm nay: <color=yellow>%d<color> / <color=yellow>%d<color> ." ) + '"',
    '#define BR_LIMIT_FULL  "' + V("Hôm nay đã dùng hết lượt nhận kinh nghiệm. Hãy quay lại vào ngày mai.") + '"',
    "",
    ])
    # tep dung #pragma once (khong co #endif) -> chen vao CUOI tep
    s = s.rstrip() + "\n" + add + "\n"
    bak(p); wr(p, s)
    print("Str.h: da them chuoi")

# ================= 2) UiTaskGuide.h =================
p = os.path.join(SRC, "UiTaskGuide.h")
s = rd(p)
if "BuildBangChienText" in s:
    print("h: da co")
else:
    old = "\tvoid\t\t\t\t\tBuildTinSuText();"
    if old not in s:
        import re
        m = re.search(r"[ \t]*void[ \t]+BuildTinSuText\(\);", s)
        assert m, "khong thay khai bao BuildTinSuText"
        old = m.group(0)
    s = s.replace(old, old + "\n\tvoid\t\t\t\t\tBuildBangChienText();\t// [CHI NAM 24/08]\n\tvoid\t\t\t\t\tBuildBachNhanText();\t// [CHI NAM 24/08]", 1)
    bak(p); wr(p, s)
    print("h: da khai bao 2 ham")

# ================= 3) UiTaskGuide.cpp =================
p = os.path.join(SRC, "UiTaskGuide.cpp")
s = rd(p)
NL = "\r\n" if "\r\n" in s else "\n"
if "TASKGUIDE_BANGCHIEN_TASKID" in s:
    print("cpp: da co")
else:
    # 3a. hang so
    old = "#define TASKGUIDE_TINSU_TASKID\t7"
    i = s.index(old); j = s.index("\n", i)
    s = s[:j+1] + ("#define TASKGUIDE_BANGCHIEN_TASKID\t8\t// [CHI NAM 24/08] tongwar: task 2369..2378 (nt_setTask co SyncTaskValue)" + NL +
                   "#define TASKGUIDE_BACHNHAN_TASKID\t9\t// [CHI NAM 24/08] bairenleitai: task 2709 (luot exp/ngay)" + NL) + s[j+1:]

    # 3b. dispatch
    old = ("\telse if (pEntry->nTaskId == TASKGUIDE_TINSU_TASKID)" + NL +
           "\t{" + NL +
           "\t\tBuildTinSuText();" + NL +
           "\t}" + NL)
    assert s.count(old) == 1, "anchor dispatch"
    s = s.replace(old, old +
        "\telse if (pEntry->nTaskId == TASKGUIDE_BANGCHIEN_TASKID)" + NL +
        "\t{" + NL + "\t\tBuildBangChienText();" + NL + "\t}" + NL +
        "\telse if (pEntry->nTaskId == TASKGUIDE_BACHNHAN_TASKID)" + NL +
        "\t{" + NL + "\t\tBuildBachNhanText();" + NL + "\t}" + NL, 1)

    # 3c. loc task khi doi gia tri
    old = ("\t\tif (nTaskId >= 1201 && nTaskId <= 1218)" + NL +
           "\t\t\tm_pSelf->BuildTinSuText();" + NL +
           "\t}" + NL)
    assert s.count(old) == 1, "anchor filter"
    s = s.replace(old, old +
        "\telse if (m_pSelf->m_Entries[m_pSelf->m_nCurEntry].nTaskId == TASKGUIDE_BANGCHIEN_TASKID)" + NL +
        "\t{" + NL + "\t\tif (nTaskId >= 2369 && nTaskId <= 2378)" + NL +
        "\t\t\tm_pSelf->BuildBangChienText();" + NL + "\t}" + NL +
        "\telse if (m_pSelf->m_Entries[m_pSelf->m_nCurEntry].nTaskId == TASKGUIDE_BACHNHAN_TASKID)" + NL +
        "\t{" + NL + "\t\tif (nTaskId == 2709)" + NL +
        "\t\t\tm_pSelf->BuildBachNhanText();" + NL + "\t}" + NL, 1)

    # 3d. hai ham dung noi dung - chen ngay truoc dinh nghia BuildTinSuText
    anchor = "void KUiTaskGuide::BuildTinSuText()"
    assert s.count(anchor) == 1
    body = NL.join([
    "// [CHI NAM 24/08] Bang Chien: doc task 2369 diem tran, 2370 so lan chet, 2371 so nguoi ha,",
    "// 2373 quan ham 1..5, 2375 chuoi ha cao nhat, 2376 tran mang, 2378 tong diem ca mua.",
    "void KUiTaskGuide::BuildBangChienText()",
    "{",
    "\tm_Content.Clear();",
    "\tint nPoint  = DTG_TaskVal(2369);",
    "\tint nDeath  = DTG_TaskVal(2370);",
    "\tint nKill   = DTG_TaskVal(2371);",
    "\tint nRank   = DTG_TaskVal(2373);",
    "\tint nSeries = DTG_TaskVal(2375);",
    "\tint nMaxDie = DTG_TaskVal(2376);",
    "\tint nTotal  = DTG_TaskVal(2378);",
    "\tif (nPoint == 0 && nKill == 0 && nTotal == 0 && nDeath == 0)",
    "\t{",
    "\t\tAddLine(TW_NOTASK);",
    "\t\treturn;",
    "\t}",
    "\tstatic const char* s_szRank[6] = { TW_RANK_0, TW_RANK_1, TW_RANK_2, TW_RANK_3, TW_RANK_4, TW_RANK_5 };",
    "\tif (nRank < 0 || nRank > 5)",
    "\t\tnRank = 0;",
    "\tchar szLine[512];",
    "\tsprintf(szLine, TW_INFO_FMT, nPoint, nTotal);",
    "\tAddLine(szLine);",
    "\tsprintf(szLine, TW_KILL_FMT, nKill, nSeries);",
    "\tAddLine(szLine);",
    "\tsprintf(szLine, TW_DEATH_FMT, nDeath, nMaxDie);",
    "\tAddLine(szLine);",
    "\tsprintf(szLine, TW_RANK_FMT, s_szRank[nRank]);",
    "\tAddLine(szLine);",
    "}",
    "",
    "// [CHI NAM 24/08] Bach Nhan Loi Dai: task 2709 ma hoa yymmdd*256 + so luot da dung.",
    "// Tran 50 luot/ngay (BR_TRAN_LUOT_NGAY trong cauhinh_hoatdong.lua).",
    "void KUiTaskGuide::BuildBachNhanText()",
    "{",
    "\tm_Content.Clear();",
    "\tAddLine(BR_INFO);",
    "\tAddLine(BR_HOWTO);",
    "\tconst int nLimit = 50;",
    "\tint nRaw = DTG_TaskVal(2709);",
    "\tint nUsed = (nRaw > 0) ? (nRaw % 256) : 0;",
    "\tif (nUsed > nLimit)",
    "\t\tnUsed = nLimit;",
    "\tchar szLine[256];",
    "\tif (nUsed >= nLimit)",
    "\t\tAddLine(BR_LIMIT_FULL);",
    "\telse",
    "\t{",
    "\t\tsprintf(szLine, BR_LIMIT_FMT, nUsed, nLimit);",
    "\t\tAddLine(szLine);",
    "\t}",
    "}",
    "", ""])
    s = s.replace(anchor, body + anchor, 1)
    bak(p); wr(p, s)
    print("cpp: da them hang so + dispatch + loc + 2 ham")

# ================= 4) uitasklist.ini =================
s = rd(INI)
if "TaskId=8" in s:
    print("ini: da co")
else:
    NL = "\r\n" if "\r\n" in s else "\n"
    add = (NL + "[3]" + NL + "Name=+" + V("Bang Chiến") + NL + "TaskId=8" + NL +
           NL + "[4]" + NL + "Name=+" + V("Bách Nhân Lôi Đài") + NL + "TaskId=9" + NL)
    s = s.rstrip() + NL + add
    # cap nhat so muc neu co khoa Count
    import re
    m = re.search(r"(?im)^(\s*Count\s*=\s*)(\d+)", s)
    if m:
        s = s[:m.start(2)] + str(int(m.group(2)) + 2) + s[m.end(2):]
        print("ini: cap nhat Count -> %d" % (int(m.group(2)) + 2))
    bak(INI); wr(INI, s)
    print("ini: da them 2 muc")

print("XONG")
