# -*- coding: utf-8 -*-
"""C7 - YEU CAU 3: them muc "San Boss Sat Thu" len Chi nam nhiem vu (F11) phia CLIENT.
Theo dung khuon BuildBachNhanText/BuildTinSuText (cac dot 21-24/08):
  1) UiTaskGuide.cpp : define TASKGUIDE_SATTHU_TASKID 10 + 2 nhanh re + body BuildSatThuText
  2) UiTaskGuide.h   : khai bao BuildSatThuText
  3) UiTaskGuideStr.h: 7 chuoi TCVN3 (sinh bang unicode_to_tcvn3_bytes)
  4) bin\\client\\Ui\\uitasklist.ini : muc [5] TaskId=10
Duong dong bo du lieu DA CO SAN: killer dung nt_setTask -> SyncTaskValue
(newtask_head.lua:31-33) -> client UI_TASKVALUE -> OnTaskValueChanged.
"""
import io, os, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes

SRC = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase"
CLIENT = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
MIRROR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"


def V(s):
    return unicode_to_tcvn3_bytes(s).decode("latin-1")


def rw(p, fn):
    d = io.open(p, encoding="latin-1", newline="").read()
    d2 = fn(d)
    if d2 != d:
        io.open(p, "w", encoding="latin-1", newline="").write(d2)
        print("  OK", os.path.basename(p))
    else:
        print("  (khong doi)", os.path.basename(p))


# ---------- 1) UiTaskGuide.cpp ----------
def f_cpp(d):
    if "TASKGUIDE_SATTHU_TASKID" in d:
        return d
    nl = "\r\n" if "\r\n" in d else "\n"
    # define
    a = "#define TASKGUIDE_BACHNHAN_TASKID	9	// [CHI NAM 24/08] bairenleitai: task 2709 (luot exp/ngay)"
    assert d.count(a) == 1
    d = d.replace(a, a + nl + "#define TASKGUIDE_SATTHU_TASKID	10	// [3HD 25/08] san boss sat thu: task 1082/1192/1193/1217 (nt_setTask co SyncTaskValue)")
    # nhanh OnTaskValueChanged
    a2 = ("\telse if (m_pSelf->m_Entries[m_pSelf->m_nCurEntry].nTaskId == TASKGUIDE_BACHNHAN_TASKID)" + nl +
          "\t{" + nl +
          "\t\tif (nTaskId == 2709)" + nl +
          "\t\t\tm_pSelf->BuildBachNhanText();" + nl +
          "\t}")
    assert d.count(a2) == 1
    d = d.replace(a2, a2 + nl +
          "\telse if (m_pSelf->m_Entries[m_pSelf->m_nCurEntry].nTaskId == TASKGUIDE_SATTHU_TASKID)" + nl +
          "\t{" + nl +
          "\t\tif (nTaskId == 1082 || nTaskId == 1192 || nTaskId == 1193 || nTaskId == 1217)" + nl +
          "\t\t\tm_pSelf->BuildSatThuText();" + nl +
          "\t}")
    # nhanh chon entry
    a3 = ("\telse if (pEntry->nTaskId == TASKGUIDE_BACHNHAN_TASKID)" + nl +
          "\t{" + nl +
          "\t\tBuildBachNhanText();" + nl +
          "\t}")
    assert d.count(a3) == 1
    d = d.replace(a3, a3 + nl +
          "\telse if (pEntry->nTaskId == TASKGUIDE_SATTHU_TASKID)" + nl +
          "\t{" + nl +
          "\t\tBuildSatThuText();" + nl +
          "\t}")
    # body sau BuildBachNhanText
    a4 = "void KUiTaskGuide::BuildTinSuText()"
    assert d.count(a4) == 1
    body = nl.join([
        "// [3HD 25/08] San boss Sat Thu (ban Linux): task 1082 = chi so boss dang nhan",
        "// (1..160, moi nhom 20 chi so = mot bac cap 20..90; chi nhom 90 = 141..160 con",
        "// phat thuong), 1193 = so lan giet hom nay (tran 8).",
        "void KUiTaskGuide::BuildSatThuText()",
        "{",
        "\tm_Content.Clear();",
        "\tAddLine(ST3_INFO);",
        "\tAddLine(ST3_HOWTO);",
        "\tint nBoss = DTG_TaskVal(1082);",
        "\tchar szLine[512];",
        "\tif (nBoss >= 1 && nBoss <= 160)",
        "\t{",
        "\t\tint nCapNhom = 20 + ((nBoss - 1) / 20) * 10;\t// 1-20=cap20 ... 141-160=cap90",
        "\t\tsprintf(szLine, ST3_CUR_FMT, nBoss, nCapNhom);",
        "\t\tAddLine(szLine);",
        "\t\tif (nBoss >= 141)",
        "\t\t\tAddLine(ST3_GROUP90);",
        "\t\telse",
        "\t\t\tAddLine(ST3_GROUPLOW);",
        "\t}",
        "\telse",
        "\t{",
        "\t\tAddLine(ST3_NOTASK);",
        "\t}",
        "\tint nUsed = DTG_TaskVal(1193);",
        "\tif (nUsed < 0) nUsed = 0;",
        "\tsprintf(szLine, ST3_LIMIT_FMT, nUsed);",
        "\tAddLine(szLine);",
        "}",
        "",
    ])
    d = d.replace(a4, body + a4)
    return d
rw(os.path.join(SRC, "UiTaskGuide.cpp"), f_cpp)

# ---------- 2) UiTaskGuide.h ----------
def f_h(d):
    if "BuildSatThuText" in d:
        return d
    nl = "\r\n" if "\r\n" in d else "\n"
    a = "\tvoid\t\t\t\t\tBuildBachNhanText();"
    i = d.index(a)
    eol = d.index(nl, i) + len(nl)
    return d[:eol] + "\tvoid\t\t\t\t\tBuildSatThuText();\t// [3HD 25/08] san boss sat thu (TaskId 10)" + nl + d[eol:]
rw(os.path.join(SRC, "UiTaskGuide.h"), f_h)

# ---------- 3) UiTaskGuideStr.h ----------
def f_str(d):
    if "ST3_INFO" in d:
        return d
    nl = "\r\n" if "\r\n" in d else "\n"
    block = nl.join([
        "// [3HD 25/08] San boss Sat Thu (TaskId 10) - chuoi TCVN3 tho",
        '#define ST3_INFO       "' + V("Gặp Nhiếp Thí Trần ở thất đại thành thị để nhận nhiệm vụ truy nã sát thủ (mỗi ngày tối đa 8 lần).") + '"',
        '#define ST3_HOWTO      "' + V("Tìm và tiêu diệt đúng tên sát thủ được giao; gộp 5 Sát Thủ lệnh cùng cấp thành Sát Thủ Giản - vé vào Thách thức thời gian.") + '"',
        '#define ST3_CUR_FMT    "' + V("Đang truy nã sát thủ thứ <color=yellow>%d<color> (nhóm cấp %d).") + '"',
        '#define ST3_GROUP90    "' + V("Nhóm cấp 90: hạ xong có thưởng lớn và Sát Thủ lệnh cấp 90.") + '"',
        '#define ST3_GROUPLOW   "' + V("Nhóm cấp thấp: bản gốc chỉ còn nhóm 90 phát thưởng - hãy nhận nhiệm vụ cấp 90.") + '"',
        '#define ST3_NOTASK     "' + V("Chưa nhận nhiệm vụ sát thủ nào.") + '"',
        '#define ST3_LIMIT_FMT  "' + V("Số lần đã giết hôm nay: <color=yellow>%d<color> / 8 .") + '"',
        "",
    ])
    return d.rstrip(nl) + nl + block
rw(os.path.join(SRC, "UiTaskGuideStr.h"), f_str)

# ---------- 4) uitasklist.ini ----------
def f_ini(d):
    if "TaskId=10" in d:
        return d
    nl = "\r\n" if "\r\n" in d else "\n"
    a = "[4]" + nl + "Name=+" + V("Bách Nhân Lôi đài") + nl + "TaskId=9" + nl
    assert d.count(a) == 1, "anchor [4] khong khop"
    return d.replace(a, a + nl + "[5]" + nl + "Name=+" + V("Săn Boss Sát Thủ") + nl + "TaskId=10" + nl)
p_ini = os.path.join(CLIENT, "Ui", "uitasklist.ini")
rw(p_ini, f_ini)
# guong
import shutil
m = os.path.join(MIRROR, "bin_client", "Ui", "uitasklist.ini")
os.makedirs(os.path.dirname(m), exist_ok=True)
shutil.copyfile(p_ini, m)
print("xong C7 (nguon client + uitasklist.ini)")
