# -*- coding: utf-8 -*-
"""C51 - chu game: "moi lan danh boss sat thu se nhan duoc gi? toi thay co mot so
lenh bai ma kich dung khong duoc" + "ra nhung item do hinh nhu dang add nham item
dung khong?"

=== LOI THAT #1: 30,0024% BANG THUONG BOSS CAP 90 RA... KHONG GI CA ===
kill_level.lua OnFinishKillerTask co 2 muc:
    {szName="Dai Luc hoan", tbProp={6,0,3,1,0,0}, nRate=15}
    {szName="Phi Toc hoan", tbProp={6,0,6,1,0,0}, nRate=15}
Ban LINUX danh so khoi hoan duoc bang DETAIL = 0 (magicscript.txt:6 va :9),
JX1 danh so bang DETAIL = 1 (magicscript.txt:5 va :8 - dung ten, dung script
\\script\\item\\potion15.lua). => 6,0,3 va 6,0,6 KHONG TON TAI trong bang item JX1,
AddItem im lang khong tao gi. Cong 15% + 15% = 30,0024% moi lan giet boss cap 90
nguoi choi khong nhan duoc vat pham nao (van co 10 trieu exp co dinh).
VA: doi sang 6,1,3 va 6,1,6 - tra THEO TEN, dung 2 muc, khong dung toi ti le.

=== KHONG PHAI LOI: lenh bai bam chuot phai khong duoc ===
"Lenh bai Bac Dau" (4126) va "Lenh bai Bac Dau - Boss sat thu" (4138) LA DUNG ITEM
(Linux 3508 / 3520, doi chieu theo ten). O CA ban Linux chung deu de cot Script = 0
tuc VON DI khong bam duoc - phai nop cho NPC Bac Dau lao nhan. Sat Thu lenh (398)
va Sat thu gian (399) cung vay: 398 mang toi NPC Nhiep Thi Tran de hop thanh, 399
la VE VAO Vuot Ai (dragonboat_main.lua:136-142 tru 1 cai).
=> khong doi co che, chi THEM MOT SCRIPT CHI DAN: bam chuot phai se hien dang giu
bao nhieu lenh bai, nop o dau, moc doi thuong. Script KHONG tru item va KHONG nam
trong danh sach trang sIsJx2ItemScript nen engine cung khong tru.

=== CON LAI (bao cao, chua sua): 0,1635% van tro toi ma JX1 khong co ===
8 "Do Pho Dang Long ..." (30528, 30530-30536) 0,0003% moi cai, "Dang Long Thach -
Ha" (30538) 0,16%, 3 ngua Phong Van (0,10,19/20/21) 0,0011%. Quet ca 61 bang item
cua JX1: khong co ten nao khop. Chu game quyet bo han hay them item.
"""
import io, os, re, sys, shutil

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes

V = lambda s: unicode_to_tcvn3_bytes(s).decode("latin-1")

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
MIR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"


def sync(rel, goc=SRV):
    dst = os.path.join(MIR, rel)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.copyfile(os.path.join(goc, rel), dst)


# ============================================================ 1) va 30% thuong chet
rel = r"script\task\tollgate\killer\kill_level.lua"
p = os.path.join(SRV, rel)
d = io.open(p, encoding="latin-1", newline="").read()
if "[3HD 25/08 C51]" in d:
    print("1) kill_level.lua: da va roi")
else:
    nl = "\r\n" if "\r\n" in d else "\n"
    SUA = [
        ('{szName="' + V("Đại lực hoàn") + '",tbProp={6,0,3,1,0,0},nCount=1,nRate=15},',
         '{szName="' + V("Đại lực hoàn") + '",tbProp={6,1,3,1,0,0},nCount=1,nRate=15},'),
        ('{szName="' + V("Phi tốc hoàn") + '",tbProp={6,0,6,1,0,0},nCount=1,nRate=15},',
         '{szName="' + V("Phi tốc hoàn") + '",tbProp={6,1,6,1,0,0},nCount=1,nRate=15},'),
    ]
    n = 0
    for cu, moi in SUA:
        if d.count(cu) == 1:
            d = d.replace(cu, moi)
            n += 1
        else:
            # thu ban khong dau / khac hoa thuong
            m = re.search(r'(\{szName="[^"]*",tbProp=\{6,0,(3|6),1,0,0\},nCount=1,nRate=15\},)', d)
            if m:
                d = d.replace(m.group(1), m.group(1).replace("{6,0,", "{6,1,"))
                n += 1
            else:
                print("   !! khong khop:", cu[:60])
    if n:
        ghichu = nl.join([
            "\t\t-- [3HD 25/08 C51] Ban LINUX danh so khoi hoan duoc bang DETAIL = 0",
            "\t\t-- (magicscript.txt:6 Dai Luc hoan 6,0,3 / :9 Phi Toc hoan 6,0,6); JX1 danh",
            "\t\t-- so bang DETAIL = 1 (6,1,3 / 6,1,6, script potion15.lua). Giu nguyen 6,0,*",
            "\t\t-- thi AddItem IM LANG khong tao gi => 15% + 15% = 30,0024% moi lan giet boss",
            "\t\t-- cap 90 khong ra vat pham nao. Doi sang ma JX1 - tra THEO TEN.",
        ])
        neo = "\t\tlocal tbAward = {"
        assert d.count(neo) == 1, d.count(neo)
        d = d.replace(neo, ghichu + nl + neo)
        io.open(p, "w", encoding="latin-1", newline="").write(d)
        sync(rel)
        print("1) kill_level.lua: da doi %d muc 6,0,* -> 6,1,* (thu hoi 30,0024%% bang thuong)" % n)

# =========================================== 2) script chi dan cho lenh bai Bac Dau
LB = [
    (4126, "Lệnh bài Bắc Đẩu"),
    (4127, "Lệnh bài Bắc Đẩu - Phong Vân cấp 1"),
    (4128, "Lệnh bài Bắc Đẩu - Phong Vân cấp 2"),
    (4129, "Lệnh bài Bắc Đẩu - Phong Vân cấp 3"),
    (4130, "Lệnh bài Bắc Đẩu - Phong Vân cấp 4"),
    (4131, "Lệnh bài Bắc Đẩu - Vượt ải cấp 1"),
    (4132, "Lệnh bài Bắc Đẩu - Vượt ải cấp 2"),
    (4133, "Lệnh bài Bắc Đẩu - Viêm Đế cấp 1"),
    (4134, "Lệnh bài Bắc Đẩu - Viêm Đế cấp 2"),
    (4135, "Lệnh bài Bắc Đẩu - Phong Lăng Độ cấp 1"),
    (4136, "Lệnh bài Bắc Đẩu - Phong Lăng Độ cấp 2"),
    (4137, "Lệnh bài Bắc Đẩu - Tín Sứ"),
    (4138, "Lệnh bài Bắc Đẩu - Boss sát thủ"),
    (4139, "Lệnh bài Bắc Đẩu - Thiên Lộc Phúc"),
]
L = [
    "-- ============================================================================",
    "-- BD_LENHBAI_INFO.LUA - SINH boi ReverseTools/port_3hd/thicong/c51_va_thuong_satthu.py",
    "-- Bam chuot phai vao lenh bai Bac Dau -> chi dan nop o dau. KHONG tru item.",
    "--",
    "-- Vi sao can: o CA ban Linux, 14 ma lenh bai Bac Dau deu de cot Script = 0 nen",
    "-- bam chuot phai khong lam gi - nguoi choi tuong item hong. Script nay chi HIEN",
    "-- THONG TIN, khong doi co che (van phai nop cho NPC Bac Dau lao nhan).",
    "-- KHONG dua duong dan nay vao sIsJx2ItemScript de engine khong tru item.",
    "-- ============================================================================",
    'Include("\\\\script\\\\tinhnang\\\\3hoatdong\\\\beidou\\\\bd_activity.lua")',
    "",
    "function main(nItemIdx)",
    "\tlocal nCo = 0",
    "\tif (BD_DemLenhBai ~= nil) then",
    "\t\tnCo = BD_DemLenhBai()",
    "\tend",
    "\tlocal nRieng = 0",
    "\tif (tbBeidou ~= nil and tbBeidou.LENHBAI_HD ~= nil) then",
    "\t\tfor k, v in tbBeidou.LENHBAI_HD do",
    "\t\t\tnRieng = nRieng + CalcItemCount(3, 6, 1, v[1], -1)",
    "\t\tend",
    "\tend",
    '\tlocal n15 = 15',
    '\tlocal n20 = 20',
    "\tif (tbBeidou ~= nil) then",
    "\t\tn15 = tbBeidou.TOKEN_NUM_AWARD10",
    "\t\tn20 = tbBeidou.TOKEN_NUM_AWARD13",
    "\tend",
    '\tSay("' + V("Lệnh bài Bắc Đẩu không dùng trực tiếp. Hãy mang tới ")
    + '<color=yellow>' + V("Bắc Đẩu lão nhân") + '<color> '
    + V("ở Thành Đô, Phượng Tường, Biện Kinh, Lâm An, Đại Lý, Tương Dương, Dương Châu (8 giờ đến 22 giờ).") + '", 0)',
    '\tSay("' + V("Đang giữ ") + '<color=yellow>"..nCo.."<color> '
    + V("Lệnh bài Bắc Đẩu và ") + '<color=yellow>"..nRieng.."<color> '
    + V("lệnh bài của hoạt động. Đủ ") + '<color=yellow>"..n15.."<color> '
    + V("hoặc ") + '<color=yellow>"..n20.."<color> '
    + V("thì đổi được phần thưởng lớn.") + '", 0)',
    "end",
    "",
]
rel = r"script\item\bd_lenhbai_info.lua"
p = os.path.join(SRV, rel)
io.open(p, "w", encoding="latin-1", newline="").write("\r\n".join(L))
sync(rel)
print("2) da sinh %s (%d dong)" % (rel, len(L)))

SC = "\\script\\item\\bd_lenhbai_info.lua"
for goc in (SRV, CLI):
    q = os.path.join(goc, r"settings\item\magicscript.txt")
    if not os.path.exists(q + ".truoc_c51_2508"):
        shutil.copyfile(q, q + ".truoc_c51_2508")
    dd = io.open(q, encoding="latin-1", newline="").read().split("\n")
    n = 0
    for i, l in enumerate(dd):
        c = l.split("\t")
        if len(c) > 10 and c[1] == "6" and c[2] == "1":
            for pid, ten in LB:
                if c[3] == str(pid) and c[9] != SC:
                    c[9] = SC
                    dd[i] = "\t".join(c)
                    n += 1
    if n:
        io.open(q, "w", encoding="latin-1", newline="").write("\n".join(dd))
    print("   bang item %s: tro %d dong lenh bai sang script chi dan"
          % ("server" if goc == SRV else "client", n))
sync(r"settings\item\magicscript.txt")
