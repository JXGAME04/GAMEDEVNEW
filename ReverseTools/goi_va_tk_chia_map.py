# -*- coding: utf-8 -*-
# [TK-CHIAMAP 28/08] Chu game: "[BotTan] da oke NHUNG luc di chuyen phai chia ra
# NHIEU doi thu O XA NHAU -> nhieu nhom danh nhau; hien tai van gom 1 nhom, khong
# chia deu map".
# Goc: TOPK=25 GAN NHAT -> ca 25 ung vien deu nam trong chinh dam danh nhau
# (tien tuyen la MOT cum) -> chia hang kieu gi cung quanh mot cho.
# Va 2 num:
#   H1 TOPK 25 -> 60: hang cua moi bot (nLech % nCand) rai tu gan toi TAN CANH XA
#      -> mac dinh dan da phan tan theo toan bo pho khoang cach, moi dich van bi
#      tran 10 con san (slot cap) -> tu nhien hinh thanh nhieu o danh nhau.
#   H2 [BotTan] ban kinh ne khi ket dam >20: 20 o -> 60 o: tuyen sau phai nhan
#      dich o VUNG KHAC HAN (khong phai ria dam dong) -> lam moi keo tran tach o;
#      khong con ai ngoai 60 o thi roi ve lang-thang-doanh-trai (cung xuyen map).
# AP SAU goi_va_tk_tan_nhom.py. Chi KPlayerBot.cpp.
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

ap("H1 TOPK 25 -> 60",
 '#define PB_TK_SAN_TOPK 25\n',
 '// [TK-CHIAMAP 28/08] 25 -> 60: 25 ung vien GAN NHAT deu nam trong chinh dam danh\n'
 '// nhau -> chia hang van quanh mot cum. 60 ung vien phu toi tan canh xa: hang cua\n'
 '// moi bot rai theo toan pho khoang cach + tran 10 con/dich -> nhieu o danh nhau\n'
 '// khap map (chu game: "chia ra nhieu doi thu o xa nhau, nhieu nhom danh nhau").\n'
 '#define PB_TK_SAN_TOPK 60\n')

ap("H2 ban kinh ne dam dong 20 -> 60 o",
 '\t\t\tif (nDong > 20)\n'
 '\t\t\t{\n'
 '\t\t\t\tnTranhGan = 20 * 32;\n',
 '\t\t\tif (nDong > 20)\n'
 '\t\t\t{\n'
 '\t\t\t\t// [TK-CHIAMAP] 20 o chi day ra RIA dam dong (dam rong 20-30 o) - van\n'
 '\t\t\t\t// mot cum. 60 o ep tuyen sau nhan dich o vung khac han -> tach o.\n'
 '\t\t\t\tnTranhGan = 60 * 32;\n')

sau = sum(1 for c in s if ord(c) > 127)
if truoc != sau:
    print("LOI: high-byte doi %d -> %d" % (truoc, sau)); sys.exit(1)
if n:
    io.open(P, "w", encoding="latin-1", newline="").write(s)
print("Tong hunk ap: %d" % n)
