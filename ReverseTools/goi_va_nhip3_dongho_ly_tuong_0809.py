# -*- coding: utf-8 -*-
"""goi_va_nhip3_dongho_ly_tuong_0809.py - [NHIP 08/09 d] SUA THEO LOG.
Do 900 khung lien tiep ([NHIPVE], chu dung yen): khoang cach giua hai tick logic KHONG phai 55,6 ms ma nhay
47 ms (39 lan) / 46 (11) / 62 (27) / 63 (37) - tuc lech +-15 %. Ly do: tick chi duoc kiem MOI VONG BOM, ma vong
bom dang khoa theo vsync 144 Hz = 6,94 ms, nen tick roi vao 7, 8 hoac 9 khung = 48,6 / 55,5 / 62,5 ms.
Hau qua do duoc: **57 tren 115 lan sang tick moi xay ra khi alpha CHUA TOI 900** (nho nhat 18), tuc mot nua so tick
lam vi tri VE NHAY TOI mot doan (toi 80 % mot buoc), 18 lan/giay -> nguoi va TEN dang di chuyen bi nhoe.
(Nguoc lai chi 8 lan alpha cham tran 1000 = dong bang.)
SUA: them che do dong ho LY TUONG - alpha tinh tu MOC LY TUONG cua tick (m_GameCounter - 1) * 1000/18 chu khong
tu moc tick THAT, va chia cho nhip danh dinh 1000/18. Khi do alpha chay deu 0..1000 dung 55,56 ms roi giu 1000
trong phan tick den muon (toi da mot khung 6,94 ms) - KHONG con cu nhay nao.
Cong tac: [Client] PaintSmooth = 0 nhu truoc 08/09 | 1 trung binh truot (hien nay) | 2 dong ho ly tuong (moi).
Chi sua S3Client -> chi Game.exe."""
import io, re, sys

P = r"D:\GAMEDEVNEW_wt_delta\Sources\S3Client\S3Client.cpp"
NL = "\r\n"
s = io.open(P, "r", encoding="latin-1", newline="").read()
h0 = sum(1 for ch in s if ord(ch) >= 0x80)


def rep(s, old, new, n=1):
    c = s.count(old)
    if c != n:
        print("FAIL neo %r: thay %d, can %d" % (old[:80], c, n)); sys.exit(1)
    return s.replace(old, new)


if "PaintSmooth = 2" in s or "dong ho LY TUONG" in s:
    print("da va roi"); sys.exit(0)

old = ("\t\t\t\tif (g_nPaintSmooth > 0 && s_dSpanAvg > 0.0 && nPaintElapse >= s_dwLastTickAt)" + NL +
       "\t\t\t\t\tnAlpha = (int)((double)(nPaintElapse - s_dwLastTickAt) * 1000.0 / (s_dSpanAvg * 0.97));" + NL)
new = ("\t\t\t\t// [NHIP 08/09 d] PaintSmooth = 2: DONG HO LY TUONG. Do 900 khung cho thay khoang cach hai tick" + NL +
       "\t\t\t\t// KHONG deu (47 / 62 / 63 ms vi tick chi duoc kiem moi vong bom, ma vong bom khoa theo vsync 6,94 ms)," + NL +
       "\t\t\t\t// nen 57/115 lan sang tick moi xay ra khi alpha chua toi 900 => vi tri VE nhay toi mot doan, 18 lan/giay" + NL +
       "\t\t\t\t// -> nguoi va ten dang di chuyen bi nhoe. Neo vao MOC LY TUONG cua tick va chia cho nhip danh dinh thi" + NL +
       "\t\t\t\t// alpha chay deu 0..1000 dung 55,56 ms, tick den muon chi lam giu 1000 toi da mot khung - het nhay." + NL +
       "\t\t\t\tif (g_nPaintSmooth == 2 && m_GameCounter > 0)" + NL +
       "\t\t\t\t\tnAlpha = (int)((double)nPaintElapse * (double)GAME_FPS - (double)(m_GameCounter - 1) * 1000.0);" + NL +
       "\t\t\t\telse if (g_nPaintSmooth > 0 && s_dSpanAvg > 0.0 && nPaintElapse >= s_dwLastTickAt)" + NL +
       "\t\t\t\t\tnAlpha = (int)((double)(nPaintElapse - s_dwLastTickAt) * 1000.0 / (s_dSpanAvg * 0.97));" + NL)
s = rep(s, old, new)

if sum(1 for ch in s if ord(ch) >= 0x80) != h0:
    print("FAIL byte cao"); sys.exit(1)
if re.search(r"[^\r]\n", s):
    print("FAIL LF"); sys.exit(1)
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("OK S3Client.cpp"); print("XONG NHIP d")
