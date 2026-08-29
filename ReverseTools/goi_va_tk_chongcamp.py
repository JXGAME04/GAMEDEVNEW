# -*- coding: utf-8 -*-
# [TK-CHONGCAMP 28/08] Chu game: "dot nay fix bot KET TRONG DOANH TRAI khong ra
# ngoai duoc rat nhieu".
# Do that (bot.log sau restart 18:15): KET pha 3 = 0 (het mass-quit) NHUNG 7.8k
# luot "RA TRAN"/gio = moi bot ~15 lan; vet VoLam258: ra cua 18:22:40 -> CHET
# 18:22:56 (16 giay), ra 18:23:16 -> chet 18:23:26 (10 giay). [BotSan] hai phe
# cam ngay CUM DIEM RA CUA cua dich (tum san (1542-1549,3215-3231) hom truoc =
# dung cum kimratrai (1544-1592,3173-3227)) -> xay thit nguoi vua buoc ra ->
# phan lon vong doi bot nam trong trai cho hoi sinh/mua thuoc = "ket trong trai".
# Va: VUNG CAM SAN theo phe cua UNG VIEN - khong san muc tieu con dung:
#   - quanh hau doanh phe no (R=45 o, anchor TKPOS_GO_HDOANH + dao the tran), va
#   - quanh cum diem SetPos ra trai phe no (R=25 o; tam cum tu RANDOM_POS cua
#     tongratrai.lua ~(1331,3442) / kimratrai.lua ~(1568,3200)).
# Ra khoi vung nay la bi san binh thuong. Danh gan (pb_Fight) khong doi - chi
# chan viec DINH VI/keo dan toi cua trai dich.
# AP SAU goi_va_tk_san_tanra.py. Chi KPlayerBot.cpp.
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

# ---- H1: hang so vung cam (sau #define TOPK) ----
ap("H1 vung cam san",
 '#define PB_TK_SAN_TOPK 25\n'
 '\tint     aC[PB_TK_SAN_TOPK];\n',
 '#define PB_TK_SAN_TOPK 25\n'
 '\t// [TK-CHONGCAMP 28/08] (chu game: "bot ket trong doanh trai khong ra duoc")\n'
 '\t// khong san muc tieu con trong VUNG RA QUAN cua phe no: quanh hau doanh R=45 o\n'
 '\t// + quanh cum diem SetPos ra trai R=25 o (tam cum tu RANDOM_POS tongratrai/\n'
 '\t// kimratrai.lua). Truoc do [BotSan] hai phe cam ngay cua dich xay thit nguoi\n'
 '\t// vua buoc ra (chet sau 10-16 giay) -> ca dan quan het vong doi trong trai.\n'
 '\tstatic const int aZHx[2] = { 1229, 1689 };   // hau doanh (o) - khop aKHx pha 4\n'
 '\tstatic const int aZHy[2] = { 3561, 3074 };\n'
 '\tconst int iTongZ = pb_TkDaoTheTran(nSub);\n'
 '\tconst int hz = (nCampDich == 1) ? iTongZ : (1 - iTongZ);\n'
 '\tconst int nZCampX = aZHx[hz] * 32, nZCampY = aZHy[hz] * 32;\n'
 '\tconst int nZRaX = ((nCampDich == 1) ? 1331 : 1568) * 32;\n'
 '\tconst int nZRaY = ((nCampDich == 1) ? 3442 : 3200) * 32;\n'
 '\tint     aC[PB_TK_SAN_TOPK];\n')

# ---- H2: loc trong vong quet ----
ap("H2 loc ung vien trong vung cam",
 '\t\tint ex = 0, ey = 0;\n'
 '\t\tNpc[nn].GetMpsPos(&ex, &ey);\n'
 '\t\tconst __int64 dx = (__int64)ex - bx;\n',
 '\t\tint ex = 0, ey = 0;\n'
 '\t\tNpc[nn].GetMpsPos(&ex, &ey);\n'
 '\t\t{\n'
 '\t\t\tint zdx = ex - nZCampX;  if (zdx < 0) zdx = -zdx;\n'
 '\t\t\tint zdy = ey - nZCampY;  if (zdy < 0) zdy = -zdy;\n'
 '\t\t\tif (zdx <= 45 * 32 && zdy <= 45 * 32)\n'
 '\t\t\t\tcontinue;          // con trong khu hau doanh phe no - chua duoc san\n'
 '\t\t\tzdx = ex - nZRaX;  if (zdx < 0) zdx = -zdx;\n'
 '\t\t\tzdy = ey - nZRaY;  if (zdy < 0) zdy = -zdy;\n'
 '\t\t\tif (zdx <= 25 * 32 && zdy <= 25 * 32)\n'
 '\t\t\t\tcontinue;          // vua buoc ra cua trai - cho di han ra ngoai\n'
 '\t\t}\n'
 '\t\tconst __int64 dx = (__int64)ex - bx;\n')

sau = sum(1 for c in s if ord(c) > 127)
if truoc != sau:
    print("LOI: high-byte doi %d -> %d" % (truoc, sau)); sys.exit(1)
if n:
    io.open(P, "w", encoding="latin-1", newline="").write(s)
print("Tong hunk ap: %d" % n)
