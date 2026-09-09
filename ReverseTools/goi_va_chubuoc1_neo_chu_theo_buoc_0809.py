# -*- coding: utf-8 -*-
"""goi_va_chubuoc1_neo_chu_theo_buoc_0809.py - [CHUBUOC 08/09] Chu da A/B xong: PaintInterp=0 thi HET nhoe/am mau
nhung di chuyen bi giat; PaintInterp=1 thi muot nhung CHU ten/danh hieu bi nhoe khi di chuyen.
Da loai bang so do: mau theo phe dung, khong ai bi ve hai lan, he chieu sang tat van bi, cu nhay noi suy da het
(57/115 -> 2/114) ma van bi. Con lai dung mot dieu: net chu MANH khi truot lien tuc 144 lan/giay thi khong bao gio
dung yen, man hinh giu-mau (sample and hold) lam no nhoe; khi nhich 18 lan/giay thi giua hai buoc chu DUNG YEN 55 ms
nen net.
SUA DUNG CHO: chi lam CHU (va cac lop phu khac) nhich theo BUOC, than nguoi van muot nhu cu.
KNpc::GetDrawPos la duong DUY NHAT ma ten, danh hieu, ten bang, thanh mau, chat, so sat thuong lay vi tri;
than nguoi ve thang tu m_DataRes nen khong bi anh huong.
 [Client] ChuBuoc = 0 tat (nhu hien nay) | N (2..16) neo vi tri lop phu ve boi so N diem anh.
N = 4: chu dung yen ~20 ms moi lan o toc do di bo -> net lai, ma lech than nguoi toi da 4 diem anh (khong thay).
Chi sua Core -> CoreClient.dll."""
import io, re, sys

P = r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src\KNpc.cpp"
NL = "\r\n"
s = io.open(P, "r", encoding="latin-1", newline="").read()
h0 = sum(1 for ch in s if ord(ch) >= 0x80)


def rep(s, old, new, n=1):
    c = s.count(old)
    if c != n:
        print("FAIL neo %r: thay %d, can %d" % (old[:80], c, n)); sys.exit(1)
    return s.replace(old, new)


if "g_nChuBuoc" in s:
    print("da va roi"); sys.exit(0)

old = ("void KNpc::GetDrawPos(int *pPosX, int *pPosY)" + NL +
       "{" + NL +
       "\tm_DataRes.GetPos(pPosX, pPosY);" + NL +
       "}" + NL)
new = ("// [CHUBUOC 08/09] Vi tri cho cac lop PHU (ten, danh hieu, ten bang, thanh mau, chat, so sat thuong)." + NL +
       "// Than nguoi ve thang tu m_DataRes nen KHONG di qua day - doi o day chi anh huong lop phu." + NL +
       "// Ly do: net chu manh truot lien tuc 144 lan/giay thi khong bao gio dung yen nen bi nhoe tren man hinh" + NL +
       "// giu-mau; nhich theo BUOC thi giua hai buoc chu dung yen nen net lai, ma lech than nguoi khong dang ke." + NL +
       "static int g_nChuBuoc = -1;" + NL +
       "void KNpc::GetDrawPos(int *pPosX, int *pPosY)" + NL +
       "{" + NL +
       "\tm_DataRes.GetPos(pPosX, pPosY);" + NL +
       "\tif (g_nChuBuoc < 0)" + NL +
       "\t{" + NL +
       "\t\tg_nChuBuoc = (int)GetPrivateProfileIntA(\"Client\", \"ChuBuoc\", 0, \".\\\\config.ini\");" + NL +
       "\t\tif (g_nChuBuoc < 0) g_nChuBuoc = 0;" + NL +
       "\t\tif (g_nChuBuoc > 16) g_nChuBuoc = 16;" + NL +
       "\t}" + NL +
       "\tif (g_nChuBuoc > 1 && pPosX && pPosY)" + NL +
       "\t{\t// neo ve boi so ChuBuoc (toa do mps luon duong)" + NL +
       "\t\tif (*pPosX > 0) *pPosX -= (*pPosX % g_nChuBuoc);" + NL +
       "\t\tif (*pPosY > 0) *pPosY -= (*pPosY % g_nChuBuoc);" + NL +
       "\t}" + NL +
       "}" + NL)
s = rep(s, old, new)

if sum(1 for ch in s if ord(ch) >= 0x80) != h0:
    print("FAIL byte cao"); sys.exit(1)
if re.search(r"[^\r]\n", s):
    print("FAIL LF"); sys.exit(1)
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("OK KNpc.cpp"); print("XONG CHUBUOC")
