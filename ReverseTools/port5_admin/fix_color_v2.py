# -*- coding: utf-8 -*-
r"""[24/08 v2] SUA LAI cho dung sau khi chu game bao "van con <color> va diem 66770944".

SAI LAM CUA TOI: quet UiTaskGuideStr.h thay "chi 1 chuoi hong" nen bo ham va, sua thang
chuoi. NHUNG do la quet KHUON MAU: trong "<color=yellow>%s<color>" thi truoc '<' la ky tu
's' (an toan). Chi SAU KHI thay %s bang "Dai Ly" / "chua co" (ket thuc bang chu CO DAU)
thi the moi hong => phai xu ly SAU KHI ghep chuoi, tuc trong AddLine.

=> KHOI PHUC ham nhung voi logic DUNG: MO PHONG buoc nhay cua TEncodeText (byte >0x80 nuot
   luon byte ke tiep) va CHI chen dau cach khi '<' that su roi vao vi tri byte-duoi.
   Khac ban dau: ban dau chen truoc MOI '<' co byte truoc >0x80 (sai, vi con phu thuoc
   vi tri chan/le cua ca chuoi tinh tu dau).

Kem: diem tich luy Tin Su van hien 66770944 vi ham don rac o may chu chi chay khi noi
chuyen Dich Quan. Them chan o CLIENT: gia tri >= 1.000.000 chac chan la rac (moc doi
thuong cao nhat chi 1500) => hien 0.
"""
import io, os, shutil

UI = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiTaskGuide.cpp"
def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)

s = rd(UI)
NL = "\r\n" if "\r\n" in s else "\n"
if not os.path.isfile(UI + ".truoc_colorv2"):
    shutil.copyfile(UI, UI + ".truoc_colorv2")

# ---------- 1. khoi phuc ham voi logic mo phong ----------
if "static void DTG_FixTagAfterVn" in s:
    print("1: ham da co")
else:
    old_note = ("// [24/08 hau phan bien] Da BO ham DTG_FixTagAfterVn: no chen dau cach truoc MOI '<'" + NL +
                "// dung sau byte >0x80 nen thua nua o trang o cac cho VON KHONG hong. Mo phong" + NL +
                "// TEncodeText cho thay toan bo UiTaskGuideStr.h chi co DUNG 1 chuoi that su hong" + NL +
                "// (TS_ST10) - da sua thang chuoi do (doi cho dau cach) thay vi xu ly o day." + NL)
    assert s.count(old_note) == 1, "khong thay chu thich cu"

    ham = NL.join([
    "// [FIX 24/08 v2] TEncodeText (Engine\\Src\\Text.cpp:468) coi MOI byte > 0x80 la chu Han 2 byte",
    "// va nuot LUON byte ke tiep. Tieng Viet TCVN3 la 1 byte => neu dau '<' cua mot the roi dung",
    "// vao vi tri byte-duoi cua mot cap nhu vay thi the KHONG con duoc phan tich, phan \"color>\"",
    "// in thang ra man hinh.",
    "//",
    "// Phai xu ly SAU KHI ghep chuoi: trong khuon mau \"<color=yellow>%s<color>\" thi truoc '<' la",
    "// ky tu 's' (an toan), chi khi %s duoc thay bang ten ket thuc bang chu CO DAU (vd \"Dai Ly\",",
    "// \"chua co\") thi the moi hong. Vi vay quet rieng tep chuoi la KHONG DU.",
    "//",
    "// Cach lam: MO PHONG dung buoc nhay cua TEncodeText tu dau chuoi; chi khi '<' roi vao vi tri",
    "// byte-duoi moi chen 1 dau cach lam byte-duoi thay the, day '<' thanh byte dau.",
    "static void DTG_FixTagAfterVn(char* pBuf, size_t nCap)",
    "{",
    "\tif (!pBuf)",
    "\t\treturn;",
    "\tchar szTmp[2048];",
    "\tsize_t i = 0, o = 0;",
    "\tsize_t nLen = strlen(pBuf);",
    "\twhile (i < nLen && o + 3 < sizeof(szTmp))",
    "\t{",
    "\t\tunsigned char c = (unsigned char)pBuf[i];",
    "\t\tif (c > 0x80 && i + 1 < nLen)",
    "\t\t{",
    "\t\t\tif (pBuf[i + 1] == '<')",
    "\t\t\t{",
    "\t\t\t\tszTmp[o++] = pBuf[i];",
    "\t\t\t\tszTmp[o++] = ' ';\t// byte-duoi gia, hy sinh thay cho '<'",
    "\t\t\t\ti++;\t\t\t\t// '<' se thanh byte dau o vong sau",
    "\t\t\t}",
    "\t\t\telse",
    "\t\t\t{",
    "\t\t\t\tszTmp[o++] = pBuf[i];",
    "\t\t\t\tszTmp[o++] = pBuf[i + 1];",
    "\t\t\t\ti += 2;",
    "\t\t\t}",
    "\t\t}",
    "\t\telse",
    "\t\t{",
    "\t\t\tszTmp[o++] = pBuf[i];",
    "\t\t\ti++;",
    "\t\t}",
    "\t}",
    "\twhile (i < nLen && o + 1 < sizeof(szTmp))",
    "\t\tszTmp[o++] = pBuf[i++];",
    "\tszTmp[o] = 0;",
    "\tstrncpy(pBuf, szTmp, nCap - 1);",
    "\tpBuf[nCap - 1] = 0;",
    "}",
    "", ""])
    s = s.replace(old_note, ham, 1)

    # goi lai trong AddLine
    anchor = "\tszBuf[sizeof(szBuf) - 1] = 0;" + NL + "\tint nLen = TEncodeText(szBuf, strlen(szBuf));"
    assert s.count(anchor) == 1, "anchor AddLine = %d" % s.count(anchor)
    s = s.replace(anchor,
        "\tszBuf[sizeof(szBuf) - 1] = 0;" + NL +
        "\tDTG_FixTagAfterVn(szBuf, sizeof(szBuf));\t// [FIX 24/08 v2]" + NL +
        "\tint nLen = TEncodeText(szBuf, strlen(szBuf));", 1)
    wr(UI, s)
    print("1: da khoi phuc ham (logic mo phong) + goi trong AddLine")

# ---------- 2. chan diem rac 1205 o client ----------
s = rd(UI)
if "DTG_TinSuPoint" in s:
    print("2: da co")
else:
    helper = NL.join([
    "// [FIX 24/08 v2] Diem tich luy Tin Su (task 1205) tung bi bo dem ngay ghi de thanh",
    "// yymmdd*256 (~66 trieu). May chu da tach bo dem sang 4128 va co ham don rac, nhung ham do",
    "// chi chay khi nguoi choi noi chuyen Dich Quan. Chan them o day: moc doi thuong cao nhat",
    "// chi 1500 nen gia tri >= 1.000.000 chac chan la rac -> hien 0.",
    "static int DTG_TinSuPoint()",
    "{",
    "\tint nPoint = DTG_TaskVal(1205);",
    "\tif (nPoint >= 1000000 || nPoint < 0)",
    "\t\treturn 0;",
    "\treturn nPoint;",
    "}",
    "", ""])
    anchor = "void KUiTaskGuide::BuildBangChienText()"
    assert s.count(anchor) == 1
    s = s.replace(anchor, helper + anchor, 1)
    old = "DTG_TaskVal(1205), s_szTitle[nTitle]"
    assert s.count(old) == 1, "anchor diem = %d" % s.count(old)
    s = s.replace(old, "DTG_TinSuPoint(), s_szTitle[nTitle]", 1)
    wr(UI, s)
    print("2: da chan diem rac o client (>= 1.000.000 -> 0)")

print("XONG")
