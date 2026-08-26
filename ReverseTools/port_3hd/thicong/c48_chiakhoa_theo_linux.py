# -*- coding: utf-8 -*-
"""C48 - chu game: "can lam Chia Khoa Nhu y (6,1,3362) va Chia khoa vang (6,1,2953)
theo ban linux".

DO TRONG BAN LINUX - hai chia khoa nay KHONG BAN O TIEM, KHONG co nguon co dinh.
Moi cho phat chung deu la SU KIEN CO THOI HAN:
  script\\activitysys\\config\\4\\award_ext.lua:40,46   (Event_Thang5 2012, bang Award_A)
  script\\activitysys\\config\\11\\award.lua:53         (Event_Thang4 2012)
  script\\activitysys\\config\\40\\award_ext.lua:37     (Event_Thang3 2012)
  script\\event\\great_night\\yubi_baoxiang.lua:78      (ruong su kien)
  script\\vng_event\\trongcaythang112011\\plant.lua:135 (trong cay 11/2011)
  script\\vng_event\\item\\goldkey_box.lua:10           (hop 3 chia vang)
  script\\activitysys\\config\\1032\\variables.lua:5    (Tri An Khach Hang 6/2014)

LUAT DUY NHAT CHO QUAI ROI THANG RA CHIA (khong qua hop) - config 1032/config.lua:
    szMessageType = "NpcOnDeath"
    tbCondition = { CheckTotalLevel {150,"",">="},  CheckInMap {"321,322,225,226,227"} }
    tbActition  = { NpcFunLib:DropSingleItem {CHIAKHOA_VANG, 1, "0.03"} }
  => quai o 5 map da ngoai cap 95 (Truong Bach son Nam/Bac + Sa Mac 1/2/3) roi
     Chia khoa vang ti le 0,03%, nguoi choi tong cap >= 150.

VA (khong dung engine, khong doi can bang map khac):
  JX1 co san he bang roi theo map: MapList.ini <map>_NormalDropRate tro toi mot
  tep .ini; KNpc::LoseSingleItem (KNpc.cpp:8483-8497) quay BANH XE CONG DON tren
  [Main] RandRange. Bang npcdroprate90.ini hien dung chung cho 20 map va tong
  RandRate moi 4300/200000 (97,85% la khong roi gi) nen con thua rat nhieu cho.
  => Tao BAN SAO RIENG npcdroprate90_chiakhoa.ini = y het ban goc + 2 muc chia
     khoa, RandRate 60/200000 = 0,03% dung ti le Linux; roi tro DUNG 5 map cua
     Linux sang tep moi. 15 map con lai giu nguyen npcdroprate90.ini.

Doi ti le: sua RandRate trong npcdroprate90_chiakhoa.ini (60 = 0,03%; 200 = 0,1%;
2000 = 1%). Doi map: sua cot <map>_NormalDropRate trong settings\\MapList.ini.
"""
import io, os, re, sys, shutil
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"

GOC = r"settings\droprate\npcdroprate90.ini"
MOI = r"settings\droprate\npcdroprate90_chiakhoa.ini"
MAPS = ["321", "322", "225", "226", "227"]   # dung danh sach cua config 1032
RATE = 60                                     # 60/200000 = 0,03% (ti le Linux)
CHIA = [
    (2953, "Chia khoa vang"),
    (3362, "Chia Khoa Nhu y"),
]


def sync(rel):
    dst = os.path.join(MIR, rel)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.copyfile(os.path.join(JX1, rel), dst)


# ------------------------------------------------------ 1) bang roi rieng
src = io.open(os.path.join(JX1, GOC), encoding="latin-1", newline="").read()
nl = "\r\n" if "\r\n" in src else "\n"
m = re.search(r"^Count=(\d+)\s*$", src, re.M)
assert m, "khong doc duoc Count"
nCount = int(m.group(1))
mr = re.search(r"^RandRange=(\d+)\s*$", src, re.M)
assert mr, "khong doc duoc RandRange"
nRange = int(mr.group(1))

d = src[:m.start()] + ("Count=%d" % (nCount + len(CHIA))) + src[m.end():]
d = d.rstrip("\r\n") + nl + nl
for k, (pid, ten) in enumerate(CHIA):
    idx = nCount + 1 + k
    d += nl.join([
        "; [3HD 25/08 C48] %s - theo luat ban Linux (activitysys/config/1032)." % ten,
        "; RandRate/RandRange = %d/%d = %.3f%%. Sua so nay de doi ti le roi." % (RATE, nRange, RATE * 100.0 / nRange),
        "[%d]" % idx,
        "Genre=6",
        "Detail=1",
        "Particular=%d" % pid,
        "RandRate=%d" % RATE,
        "",
        "",
    ])
p = os.path.join(JX1, MOI)
io.open(p, "w", encoding="latin-1", newline="").write(d)
sync(MOI)
print("1) da sinh %s (Count %d -> %d, moi chia %d/%d = %.3f%%)"
      % (MOI, nCount, nCount + len(CHIA), RATE, nRange, RATE * 100.0 / nRange))

# ------------------------------------------------------ 2) tro 5 map sang bang moi
rel = r"settings\MapList.ini"
p = os.path.join(JX1, rel)
d = io.open(p, encoding="latin-1", newline="").read()
if not os.path.exists(p + ".truoc_chiakhoa_2508"):
    shutil.copyfile(p, p + ".truoc_chiakhoa_2508")
nsua = 0
for mp in MAPS:
    cu = "%s_NormalDropRate=\\settings\\droprate\\npcdroprate90.ini" % mp
    moi = "%s_NormalDropRate=\\settings\\droprate\\npcdroprate90_chiakhoa.ini" % mp
    if moi in d:
        print("   (da tro roi) map", mp)
        continue
    if d.count(cu) != 1:
        print("   !! map %s: khong khop duy nhat (%d)" % (mp, d.count(cu)))
        continue
    d = d.replace(cu, moi)
    nsua += 1
if nsua:
    io.open(p, "w", encoding="latin-1", newline="").write(d)
    sync(rel)
print("2) da tro %d map (%s) sang bang co chia khoa" % (nsua, ",".join(MAPS)))
