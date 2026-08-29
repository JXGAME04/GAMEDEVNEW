# -*- coding: utf-8 -*-
# [TK-DEU 28/08] Chu game: "van ket trong doanh trai nhieu khong ra ngoai duoc".
# Do that (18:56-18:58, ban chong-camp DANG chay):
#   - thoi gian DI QUA trai: p50=16s max=23s -> day chuyen trai KHONG tac;
#   - chet ~436-535 mang/PHUT (500 bot => moi con chet ~60s/lan);
#   - song sau khi ra cua: p50=24s p90=39s; vong doi chet-hoi sinh ~45s
#     => ~40% quan so LUON dung trong trai o moi thoi diem = canh "ket trong trai".
# Goc: doi bo nham sang "gan nhat tuyet doi" (H5 tk_san_doithu) lam MOI bot quanh
# mot khu cung dam dung MOT nan nhan -> tap trung hoa luc xoa 1-2 giay/mang theo
# day chuyen. (Xoay vong 23/08 truoc day ton tai chinh de tranh viec nay.)
# Va: van "gan" nhung CHIA HOA LUC TRONG NHOM 4 GAN NHAT theo chi so bot; het 4
# con do moi duyet tiep theo khoang cach. TTK cham lai ~4 lan -> bot song lau hon
# -> quan so trong trai giam han.
# AP SAU goi_va_tk_chongcamp.py. Chi KPlayerBot.cpp.
import io, sys
P = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerBot.cpp"

s = io.open(P, "r", encoding="latin-1", newline="").read()
truoc = sum(1 for c in s if ord(c) > 127)
crlf = "\r\n" in s
n = 0

def ap(ten, cu, moi):
    global s, n
    if crlf:
        cu = cu.replace("\n", "\r\n"); moi = moi.replace("\n", "\r\n")
    if moi in s:
        print("  [=] %s da ap tu truoc" % ten); return
    if s.count(cu) != 1:
        print("LOI: neo %s khop %d cho (can 1)" % (ten, s.count(cu))); sys.exit(1)
    s = s.replace(cu, moi); n += 1
    print("  [+] %s" % ten)

ap("H1 chia hoa luc top-4",
 '\tif (bTrongTK)\n'
 '\t{\n'
 '\t\t// [TK-SAN 28/08] (chu game) "bat muc tieu GAN NHAT": aId da sap tang dan theo\n'
 '\t\t// khoang cach -> duyet tu dau la gan-nhat-nhin-thay-duoc. Bo loi xoay vong\n'
 '\t\t// theo chi so bot (chong dan don cuc 23/08) vi trai y chu game.\n'
 '\t\tfor (int v = 0; v < nCand; v++)\n'
 '\t\t{\n'
 '\t\t\tconst int i3 = aId[(unsigned)v % (unsigned)nCand];\n',
 '\tif (bTrongTK)\n'
 '\t{\n'
 '\t\t// [TK-DEU 28/08] "gan nhat TUYET DOI" lam moi bot quanh mot khu cung dam dung\n'
 '\t\t// MOT nan nhan -> tap trung hoa luc xoa 1-2 giay/mang, ~500 mang/phut, ~40%\n'
 '\t\t// quan so nam trong trai cho hoi sinh (chu game: "ket trong doanh trai").\n'
 '\t\t// Van la "gan" nhung CHIA trong NHOM 4 GAN NHAT theo chi so bot; het nhom do\n'
 '\t\t// moi duyet tiep theo khoang cach (aId da sap tang dan).\n'
 '\t\tconst int nChia = (nCand < 4) ? nCand : 4;\n'
 '\t\tfor (int v = 0; v < nCand; v++)\n'
 '\t\t{\n'
 '\t\t\tconst int i9 = (v < nChia)\n'
 '\t\t\t             ? (int)(((unsigned)nLech + (unsigned)v) % (unsigned)nChia)\n'
 '\t\t\t             : v;\n'
 '\t\t\tconst int i3 = aId[i9];\n')

sau = sum(1 for c in s if ord(c) > 127)
if truoc != sau:
    print("LOI: high-byte doi %d -> %d" % (truoc, sau)); sys.exit(1)
if n:
    io.open(P, "w", encoding="latin-1", newline="").write(s)
print("Tong hunk ap: %d" % n)
