# -*- coding: utf-8 -*-
"""goi_va_nhipve1_ghi_vitri_tung_khung_0809.py - [NHIPVE 08/09] Chu chot duoc dieu kien: CHI BI KHI CO CHUYEN DONG.
"dung yen thi mau dung; kich chuot di chuyen la mau am; di chuyen den noi thi mau lai dung; nguoi khac di chuyen thi
cung bi; ca hai dung yen thi dung mau". Anh chup mot khung van dung => moi khung ve ra deu dung, cai sai la CHUOI khung.
Neu vi tri VE cua nhan vat khong tang deu ma nhay toi/lui giua cac khung thi mat se thay nhoe va am mau du tung khung deu dung.
Ban nay GHI LOG vi tri ve tung khung de nhin thay chuoi do:
 [Client] DoNhipVe = N (mac dinh 0 = tat) -> ghi N khung ve dau tien sau khi vao game, moi khung mot dong [NHIPVE]
 vao jx_paint.log: so thu tu khung, alpha noi suy, vi tri TICK truoc/sau va vi tri VE cua NHAN VAT CUA MINH,
 kem vi tri ve cua NPC dang di chuyen nhanh nhat trong khung do.
Doc log: cot ve_x phai TANG DEU theo thoi gian. Neu no tang roi lui roi tang lai = giat, do la nguyen nhan nhoe mau.
Chi sua Core -> CoreClient.dll, khong doi hanh vi ve."""
import io, re, sys

P = r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src\CoreShell.cpp"
NL = "\r\n"
s = io.open(P, "r", encoding="latin-1", newline="").read()
h0 = sum(1 for ch in s if ord(ch) >= 0x80)


def rep(s, old, new, n=1):
    c = s.count(old)
    if c != n:
        print("FAIL neo %r: thay %d, can %d" % (old[:80], c, n)); sys.exit(1)
    return s.replace(old, new)


if "g_nDoNhipVe" in s:
    print("da va roi"); sys.exit(0)

s = rep(s, "unsigned g_uPaintFrameSeq = 0;",
    "static int g_nDoNhipVe = -1;\t// [NHIPVE 08/09] so khung con phai ghi (-1 = chua doc cau hinh)" + NL +
    "unsigned g_uPaintFrameSeq = 0;")

# ghi mot dong moi khung, ngay sau khi tinh xong vi tri ve cua nhan vat cua minh
old = ("\t\tg_nPaintAlpha = nAlpha;\t// [MAU 08/09]" + NL +
       "\t\tg_uPaintFrameSeq++;\t// [VETRUNG 08/09]" + NL)
new = ("\t\tg_nPaintAlpha = nAlpha;\t// [MAU 08/09]" + NL +
       "\t\tg_uPaintFrameSeq++;\t// [VETRUNG 08/09]" + NL +
       "\t\tif (g_nDoNhipVe < 0)" + NL +
       "\t\t\tg_nDoNhipVe = (int)GetPrivateProfileIntA(\"Client\", \"DoNhipVe\", 0, \".\\\\config.ini\");" + NL +
       "\t\tif (g_nDoNhipVe > 0)" + NL +
       "\t\t{\t// [NHIPVE 08/09] ghi vi tri VE cua nhan vat cua minh tung khung: cot ve_x phai TANG DEU" + NL +
       "\t\t\tg_nDoNhipVe--;" + NL +
       "\t\t\tconst int nMe = Player[CLIENT_PLAYER_INDEX].m_nIndex;" + NL +
       "\t\t\tif (nMe > 0 && nMe < MAX_NPC && s_InterpValid[nMe])" + NL +
       "\t\t\t{" + NL +
       "\t\t\t\tconst int nVeX = s_InterpFrom[nMe].x + (s_InterpTo[nMe].x - s_InterpFrom[nMe].x) * nAlpha / 1000;" + NL +
       "\t\t\t\tconst int nVeY = s_InterpFrom[nMe].y + (s_InterpTo[nMe].y - s_InterpFrom[nMe].y) * nAlpha / 1000;" + NL +
       "\t\t\t\tFILE* pLog = fopen(\"jx_paint.log\", \"a\");" + NL +
       "\t\t\t\tif (pLog)" + NL +
       "\t\t\t\t{" + NL +
       "\t\t\t\t\tfprintf(pLog, \"[NHIPVE] t=%u khung=%u alpha=%4d | tick truoc (%d,%d) sau (%d,%d) | ve (%d,%d)\\n\"," + NL +
       "\t\t\t\t\t\tGetTickCount(), g_uPaintFrameSeq, nAlpha," + NL +
       "\t\t\t\t\t\ts_InterpFrom[nMe].x, s_InterpFrom[nMe].y, s_InterpTo[nMe].x, s_InterpTo[nMe].y, nVeX, nVeY);" + NL +
       "\t\t\t\t\tfclose(pLog);" + NL +
       "\t\t\t\t}" + NL +
       "\t\t\t}" + NL +
       "\t\t}" + NL)
s = rep(s, old, new)

if sum(1 for ch in s if ord(ch) >= 0x80) != h0:
    print("FAIL byte cao"); sys.exit(1)
if re.search(r"[^\r]\n", s):
    print("FAIL LF"); sys.exit(1)
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("OK CoreShell.cpp"); print("XONG NHIPVE")
