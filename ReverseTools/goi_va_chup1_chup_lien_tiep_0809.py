# -*- coding: utf-8 -*-
"""goi_va_chup1_chup_lien_tiep_0809.py - [CHUP 08/09] Chu bao: "luc chup vao anh thi thay binh thuong, con trong game
ai chay ngoai man hinh vao deu thay bi (am do chu ten)". Nghia la LOI KHONG NAM TRONG MOT KHUNG DA VE - no la loi
theo THOI GIAN (mot so khung bi, mot so khung khong), nen chup mot khung khong bao gio bat duoc.
Ban nay cho chup LIEN TIEP nhieu khung bang mot lan bam phim chup: [Client] ChupThem = N (mac dinh 0 = nhu cu).
Bam phim chup mot lan -> luu anh dau nhu cu, roi N khung ke tiep tu dong luu tiep, ten <ten goc>_kN.
Nho vay so duoc khung nao bi khung nao khong, va bi o dau tren man hinh. Chi Represent3, khong doi hanh vi ve."""
import io, re, sys

P = r"D:\GAMEDEVNEW_wt_delta\Sources\Represent\Represent3\KRepresentShell3.cpp"
NL = "\r\n"
s = io.open(P, "r", encoding="latin-1", newline="").read()
h0 = sum(1 for ch in s if ord(ch) >= 0x80)


def rep(s, old, new, n=1):
    c = s.count(old)
    if c != n:
        print("FAIL neo %r: thay %d, can %d" % (old[:80], c, n)); sys.exit(1)
    return s.replace(old, new)


if "g_nRep3ChupThem" in s:
    print("da va roi"); sys.exit(0)

# 1. bien cau hinh
s = rep(s, "int  g_nRep3VienChu   = 1;",
    "int  g_nRep3ChupThem  = 0;\t// [CHUP 08/09] so khung chup THEM sau moi lan bam phim chup (0 = nhu cu)\r\n"
    "int  g_nRep3VienChu   = 1;")
s = rep(s, "\tg_nRep3VienChu   = Rep3Ini(\"VienChu\", 1);\t// [CHU 08/09]" + NL,
    "\tg_nRep3VienChu   = Rep3Ini(\"VienChu\", 1);\t// [CHU 08/09]" + NL +
    "\tg_nRep3ChupThem  = Rep3Ini(\"ChupThem\", 0);\t// [CHUP 08/09] kep 0..30" + NL +
    "\tif (g_nRep3ChupThem < 0) g_nRep3ChupThem = 0;" + NL +
    "\tif (g_nRep3ChupThem > 30) g_nRep3ChupThem = 30;" + NL)

# 2. nho ten goc + so khung con lai khi bam phim chup
s = rep(s, "bool KRepresentShell3::SaveScreenToFile(const char* pszName, ScreenFileType eType, unsigned int nQuality)" + NL + "{" + NL +
       "\tif(!pszName || !pszName[0])" + NL + "\t\treturn 0;" + NL,
    "// [CHUP 08/09] chup lien tiep: giu ten goc + so khung con phai chup + kieu/chat luong" + NL +
    "static char  s_szChupGoc[260] = {0};" + NL +
    "static int   s_nChupConLai = 0, s_nChupKieu = 0, s_nChupChatLuong = 0, s_nChupSo = 0;" + NL +
    "bool KRepresentShell3::SaveScreenToFile(const char* pszName, ScreenFileType eType, unsigned int nQuality)" + NL + "{" + NL +
    "\tif(!pszName || !pszName[0])" + NL + "\t\treturn 0;" + NL +
    "\tif (g_nRep3ChupThem > 0 && s_nChupConLai == 0)" + NL +
    "\t{\t// [CHUP 08/09] lan bam dau: hen chup them g_nRep3ChupThem khung ke tiep" + NL +
    "\t\tstrncpy(s_szChupGoc, pszName, sizeof(s_szChupGoc) - 1); s_szChupGoc[sizeof(s_szChupGoc) - 1] = 0;" + NL +
    "\t\ts_nChupConLai = g_nRep3ChupThem; s_nChupKieu = (int)eType; s_nChupChatLuong = (int)nQuality; s_nChupSo = 0;" + NL +
    "\t}" + NL)

# 3. cuoi khung ve: neu con no thi chup tiep
s = rep(s, "\tif(m_bDeviceLost)" + NL + "\t\treturn;" + NL + NL + "\tchar cc[200];" + NL,
    "\tif(m_bDeviceLost)" + NL + "\t\treturn;" + NL + NL +
    "\tif (s_nChupConLai > 0 && s_szChupGoc[0])" + NL +
    "\t{\t// [CHUP 08/09] chup them khung ke tiep de so sanh giua cac khung (loi chi hien o mot so khung)" + NL +
    "\t\tchar szK[300]; const char* pCham = strrchr(s_szChupGoc, '.');" + NL +
    "\t\tint nCat = pCham ? (int)(pCham - s_szChupGoc) : (int)strlen(s_szChupGoc);" + NL +
    "\t\tif (nCat > 250) nCat = 250;" + NL +
    "\t\tmemcpy(szK, s_szChupGoc, nCat); szK[nCat] = 0;" + NL +
    "\t\tsprintf(szK + nCat, \"_k%02d%s\", ++s_nChupSo, pCham ? pCham : \".jpg\");" + NL +
    "\t\ts_nChupConLai--;" + NL +
    "\t\tconst int nGiu = s_nChupConLai; s_nChupConLai = 0;\t// tranh de quy hen lai" + NL +
    "\t\tSaveScreenToFile(szK, (ScreenFileType)s_nChupKieu, (unsigned int)s_nChupChatLuong);" + NL +
    "\t\ts_nChupConLai = nGiu;" + NL +
    "\t}" + NL + NL +
    "\tchar cc[200];" + NL)

if sum(1 for ch in s if ord(ch) >= 0x80) != h0:
    print("FAIL byte cao"); sys.exit(1)
if re.search(r"[^\r]\n", s):
    print("FAIL LF"); sys.exit(1)
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("OK KRepresentShell3.cpp"); print("XONG CHUP")
