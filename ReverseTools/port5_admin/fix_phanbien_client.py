# -*- coding: utf-8 -*-
r"""[24/08 hau phan bien] Va phia CLIENT (55 tac nhan / 46 phat hien / 8 that).

A. HOI QUY do chinh ban va mau chu: DTG_FixTagAfterVn chen dau cach truoc MOI '<' dung sau
   byte >0x80 => thua nua o trang o nhieu cho VON KHONG HONG. Mo phong TEncodeText cho thay
   toan bo UiTaskGuideStr.h chi co DUNG 1 chuoi hong (TS_ST10). => BO ham, sua thang chuoi do
   bang cach doi cho dau cach (dua dau cach co san ra TRUOC the) - hien thi y het, 0 rui ro.
B. Danh muc trai F11 mat chu cuoi ("Nhiem vu Tin S"): loi engine co san (Text.cpp:470,476-477
   vut byte cuoi neu > 0x80). Va re: them 1 dau cach cuoi Name trong uitasklist.ini.
C. Tin Su: dong "Hom nay da hoan thanh" doc task 1218 = so TRON DOI (khong bao gio reset).
   Bo dem NGAY that la 4128 (yymmdd*256 + dem) => doc 4128 va giai ma theo ngay.
D. Bang Chien: dieu kien "chua tham gia" bo qua 2373 (quan ham) va 2376 (tran mang) - hai
   truong duoc dat NGAY LUC BAO DANH => dang trong tran van bao "chua tham gia".
"""
import io, os, re, shutil

UI  = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiTaskGuide.cpp"
STR = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiTaskGuideStr.h"
INI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\Ui\uitasklist.ini"

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def bak(p, tag):
    q = p + ".truoc_" + tag
    if not os.path.isfile(q): shutil.copyfile(p, q)

# ================= A1. bo ham DTG_FixTagAfterVn + loi goi =================
s = rd(UI)
NL = "\r\n" if "\r\n" in s else "\n"
if "DTG_FixTagAfterVn" in s:
    bak(UI, "phanbien_2408")
    # xoa loi goi
    call = "\tDTG_FixTagAfterVn(szBuf, sizeof(szBuf));\t// [FIX 24/08]" + NL
    assert s.count(call) == 1, "loi goi = %d" % s.count(call)
    s = s.replace(call, "", 1)
    # xoa ca khoi ham + chu thich dan
    i = s.find("// [FIX 24/08] TEncodeText (Engine\\Src\\Text.cpp:468)")
    assert i >= 0, "khong thay khoi chu thich"
    j = s.find("void KUiTaskGuide::AddLine(const char* pText)", i)
    assert j > i, "khong thay AddLine sau khoi ham"
    s = s[:i] + ("// [24/08 hau phan bien] Da BO ham DTG_FixTagAfterVn: no chen dau cach truoc MOI '<'" + NL +
                 "// dung sau byte >0x80 nen thua nua o trang o cac cho VON KHONG hong. Mo phong" + NL +
                 "// TEncodeText cho thay toan bo UiTaskGuideStr.h chi co DUNG 1 chuoi that su hong" + NL +
                 "// (TS_ST10) - da sua thang chuoi do (doi cho dau cach) thay vi xu ly o day." + NL) + s[j:]
    wr(UI, s)
    print("A1: da bo DTG_FixTagAfterVn + loi goi")
else:
    print("A1: da bo truoc do")

# ================= A2. sua chuoi TS_ST10 (doi cho dau cach) =================
s = rd(STR)
b = s.encode("latin-1")
old = b"\x53\xf8<color> \xae\xd3"          # "Sứ<color> để"
new = b"\x53\xf8 <color>\xae\xd3"          # "Sứ <color>để"
if old in b:
    assert b.count(old) == 1, "TS_ST10 anchor = %d" % b.count(old)
    bak(STR, "phanbien_2408")
    b = b.replace(old, new, 1)
    wr(STR, b.decode("latin-1"))
    print("A2: da sua TS_ST10 (dua dau cach ra truoc the)")
else:
    print("A2: da sua truoc do / khong thay anchor")

# ================= B. uitasklist.ini: them dau cach cuoi Name co dau =================
s = rd(INI)
NLi = "\r\n" if "\r\n" in s else "\n"
out, n = [], 0
for line in s.split(NLi):
    m = re.match(r"^(Name\s*=\s*)(.+?)(\s*)$", line)
    if m and m.group(2):
        val = m.group(2)
        if ord(val[-1]) > 0x80:            # ky tu cuoi la chu co dau -> engine se vut mat
            line = m.group(1) + val + " "
            n += 1
    out.append(line)
if n:
    bak(INI, "phanbien_2408")
    wr(INI, NLi.join(out))
print("B: them dau cach cuoi %d muc Name (chong mat chu cuoi)" % n)

# ================= C. Tin Su: doc 4128 thay 1218 =================
s = rd(UI)
if "DTG_DailyCount" in s:
    print("C: da va")
else:
    # ham giai ma daily dung chung
    helper = ("// [24/08 hau phan bien] Task dang \"dem theo ngay\" cua may chu ma hoa la" + NL +
              "// yymmdd*256 + so_lan (script\\activitysys\\playerfunlib.lua). Neu phan ngay khac" + NL +
              "// hom nay thi so dem coi nhu 0. Engine giai ma y het o CoreShell.cpp." + NL +
              "static int DTG_DailyCount(int nRaw)" + NL +
              "{" + NL +
              "\tif (nRaw <= 0)" + NL +
              "\t\treturn 0;" + NL +
              "\tSYSTEMTIME st;" + NL +
              "\tGetLocalTime(&st);" + NL +
              "\tint nToday = (st.wYear % 100) * 10000 + st.wMonth * 100 + st.wDay;" + NL +
              "\tif ((nRaw / 256) != nToday)" + NL +
              "\t\treturn 0;" + NL +
              "\treturn nRaw % 256;" + NL +
              "}" + NL + NL)
    anchor = "void KUiTaskGuide::BuildBangChienText()"
    assert s.count(anchor) == 1
    s = s.replace(anchor, helper + anchor, 1)

    # doi cho doc 1218 -> 4128 (giai ma)
    old = "\tsprintf(szPt, TS_POINT_FMT, DTG_TaskVal(1205), s_szTitle[nTitle], DTG_TaskVal(1218));"
    assert s.count(old) == 1, "anchor TS_POINT_FMT = %d" % s.count(old)
    s = s.replace(old,
        "\t// [24/08 hau phan bien] 1218 la so TRON DOI (khong noi nao dat lai 0) - bo dem NGAY" + NL +
        "\t// that la 4128 (doi tu 1205 hom nay, dang yymmdd*256 + so_lan)." + NL +
        "\tsprintf(szPt, TS_POINT_FMT, DTG_TaskVal(1205), s_szTitle[nTitle], DTG_DailyCount(DTG_TaskVal(4128)));", 1)

    # bo loc lam moi: them 4128
    oldf = "\t\tif (nTaskId >= 1201 && nTaskId <= 1218)"
    assert s.count(oldf) == 1
    s = s.replace(oldf, "\t\tif ((nTaskId >= 1201 && nTaskId <= 1218) || nTaskId == 4128)\t// [24/08] 4128 = bo dem ngay", 1)
    wr(UI, s)
    print("C: Tin Su doc 4128 + giai ma ngay + them vao bo loc lam moi")

# ================= D. Bang Chien: dieu kien "chua tham gia" =================
s = rd(UI)
old = "\tif (nPoint == 0 && nKill == 0 && nTotal == 0 && nDeath == 0)"
if old in s:
    s = s.replace(old,
        "\t// [24/08 hau phan bien] phai xet CA quan ham (2373) va tran mang (2376) - hai truong" + NL +
        "\t// nay duoc dat NGAY LUC BAO DANH (tongwar_trap.lua:132,141) nen dang trong tran ma" + NL +
        "\t// chi xet 4 truong kia se bao nham \"chua tham gia\"." + NL +
        "\tif (nPoint == 0 && nKill == 0 && nTotal == 0 && nDeath == 0 && nRank == 0 && nMaxDie == 0)", 1)
    wr(UI, s)
    print("D: sua dieu kien 'chua tham gia' cua Bang Chien")
else:
    print("D: da va / khong thay anchor")

print("XONG PHIA CLIENT")
