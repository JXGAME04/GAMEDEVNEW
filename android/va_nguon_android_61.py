# -*- coding: utf-8 -*-
#
# [ANDROID 10/09 ANICON] "lam them nut an cac icon phia tren va nut an kenh chat nhu vnku" (chu).
#
# VNKU co HidenButton (an_icon.spr, mui ten) thu gon hang icon tron. Ta them nut [HideIcons] vao
# KUiPlayerBar: bam -> an/hien 7 icon tron (Nhan vat, Hanh trang, Vo cong, Bang hoi, To doi, Hao huu,
# Cai dat) + nut Ngoi (nam o KUiToolsControlBar, tim theo ten muc ini "Sit"). Nut an chat thi da co san
# ([HideChat], WndProc) - chi doi anh + cho trong ini. Khong guard JX_ANDROID: ini PC khong co
# [HideIcons] thi nut rong 0x0, khong anh huong gi.
# Khung: frame 1 = "<" (dang hien, bam de thu), frame 0 = ">" (dang an, bam de mo) - giong HideChat.

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
NL = "\r\n"
DAU = "[ANDROID 10/09 ANICON]"

# ---------------------------------------------------------------- .h
P = "Sources/S3Client/Ui/UiCase/UiPlayerBar.h"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = "\tKWndButton\t\tm_HideChat;" + NL
    assert s.count(CU) == 1, "khong tim thay m_HideChat trong .h (%d)" % s.count(CU)
    MOI = CU + "\tKWndButton\t\tm_AnIcon;\t// %s nut mui ten an/hien hang icon tron (ini [HideIcons])" % DAU + NL
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)

# ---------------------------------------------------------------- .cpp
P = "Sources/S3Client/Ui/UiCase/UiPlayerBar.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
    raise SystemExit

# include
CU = '#include "UiToolsControlBar.h"'
if s.count(CU) == 0:
    CU2 = '#include "UiPlayerBar.h"'
    assert s.count(CU2) == 1, "khong tim thay include UiPlayerBar.h (%d)" % s.count(CU2)
    s = s.replace(CU2, CU2 + NL + '#include "UiToolsControlBar.h"\t// %s tim nut Ngoi' % DAU, 1)

# Init
CU = '\tm_HideChat.Init(pIni, "HideChat");'
assert s.count(CU) == 1, "khong tim thay Init HideChat (%d)" % s.count(CU)
s = s.replace(CU, CU + NL + '\tm_AnIcon.Init(pIni, "HideIcons");\t// %s' % DAU)

# AddChild
CU = "\tAddChild(&m_HideChat);"
assert s.count(CU) == 1, "khong tim thay AddChild HideChat (%d)" % s.count(CU)
s = s.replace(CU, CU + NL + "\tAddChild(&m_AnIcon);\t// %s" % DAU)

# WndProc: truoc nhanh HideChat
CU = "\t\telse if ((KWndWindow*)uParam == (KWndWindow*)&m_HideChat)" + NL + "\t\t{"
assert s.count(CU) == 1, "khong tim thay nhanh HideChat trong WndProc (%d)" % s.count(CU)
MOI = NL.join([
    "\t\telse if ((KWndWindow*)uParam == (KWndWindow*)&m_AnIcon)",
    "\t\t{",
    "\t\t\t// %s bam mui ten: an / hien hang icon tron (VNKU: HidenButton)" % DAU,
    "\t\t\tAnHienIconTron(!s_bIconTronAn);",
    "\t\t}",
]) + NL + CU
s = s.replace(CU, MOI)

# ham + bien tinh: dat truoc KUiPlayerBar::PaintWindow
CU = "void KUiPlayerBar::PaintWindow()" + NL + "{"
assert s.count(CU) == 1, "khong tim thay PaintWindow (%d)" % s.count(CU)
MOI = NL.join([
    "//\t%s Hang icon tron dang an hay hien (mot thanh, mot nut - de tinh cho gon)" % DAU,
    "static int s_bIconTronAn = 0;",
    "",
    "void KUiPlayerBar::AnHienIconTron(int bAn)",
    "{",
    "\tKWndWindow*\taO[] = { &m_Status, &m_Items, &m_Skills, &m_Faction, &m_Team, &m_Friend, &m_Options };",
    "\tint\t\t\ti;",
    "",
    "\ts_bIconTronAn = bAn ? 1 : 0;",
    "\tfor (i = 0; i < (int)(sizeof(aO) / sizeof(aO[0])); i++)",
    "\t{",
    "\t\tif (bAn)\taO[i]->Hide();",
    "\t\telse\t\taO[i]->Show();",
    "\t}",
    "\t//\tnut Ngoi nam o thanh cong cu (KUiToolsControlBar), khong co ten bien -> tim theo muc ini",
    "\tKUiToolsControlBar* pCC = KUiToolsControlBar::GetSelf();",
    "\tif (pCC)",
    "\t{",
    "\t\tfor (KWndWindow* p = pCC->GetFirstChild(); p; p = p->GetNextWnd())",
    "\t\t{",
    "\t\t\tif (strcmp(p->GetMucIni(), \"Sit\") == 0)",
    "\t\t\t{",
    "\t\t\t\tif (bAn)\tp->Hide();",
    "\t\t\t\telse\t\tp->Show();",
    "\t\t\t}",
    "\t\t}",
    "\t}",
    "\tm_AnIcon.SetFrame(bAn ? 0 : 1);\t// 0 = \">\" (dang an, bam de mo), 1 = \"<\" (dang hien)",
    "}",
    "",
]) + CU
s = s.replace(CU, MOI)
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("da va:", P)

# khai bao ham trong .h (public)
P = "Sources/S3Client/Ui/UiCase/UiPlayerBar.h"
s = io.open(P, encoding="latin-1", newline="").read()
CU = "\tKWndButton\t\tm_AnIcon;"
if "AnHienIconTron" not in s:
    i = s.index(CU)
    s = s[:i] + "\tvoid\t\t\tAnHienIconTron(int bAn);\t// %s" % DAU + NL + s[i:]
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da them khai bao AnHienIconTron:", P)
