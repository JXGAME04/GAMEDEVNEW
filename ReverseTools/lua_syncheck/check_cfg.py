# -*- coding: utf-8 -*-
r"""Kiem cau hinh trung tam:
1. Moi khoa HD_CFG("X", ...) dung trong script CO trong tbCHD cua cauhinh_hoatdong.lua khong
   (thieu = chinh trong config se KHONG an, script luon dung gia tri mac dinh).
2. Moi khoa khai trong tbCHD CO duoc dung o dau khong (thua = ghi cho vui).
3. Moi tep dung HD_CFG CO Include cauhinh_hoatdong.lua khong (khong Include = HD_CFG nil -> loi chay).
4. Danh sach HD_RELOAD_LIST trong menu co bo sot tep nao dang dung HD_CFG khong.
"""
import io, re, os

E = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CFG = os.path.join(E, r"script\header\cauhinh_hoatdong.lua")
ADMIN = os.path.join(E, r"script\item\hoatdong_admin.lua")

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()

cfg = rd(CFG)
body = cfg[cfg.index("tbCHD = {"): cfg.index("-- HAM DUNG CHUNG")]
declared = set(re.findall(r"^([A-Z][A-Z0-9_]*)\s*=", body, re.M))
print("Khoa khai bao trong tbCHD: %d" % len(declared))

used = {}
noinc = []
for dp, _, fs in os.walk(os.path.join(E, "script")):
    for f in fs:
        if not f.lower().endswith(".lua"): continue
        p = os.path.join(dp, f)
        try: s = rd(p)
        except Exception: continue
        ks = re.findall(r'HD_CFG\(\s*"([^"]+)"', s)
        if not ks: continue
        for k in ks: used.setdefault(k, []).append(p)
        # tep dung HD_CFG phai Include config (truc tiep hoac qua tep khac)
        if "cauhinh_hoatdong" not in s:
            noinc.append(p)

print("Khoa duoc dung trong script: %d" % len(used))

missing = sorted(k for k in used if k not in declared)
unused  = sorted(k for k in declared if k not in used)

print("\n--- 1) KHOA SCRIPT DUNG NHUNG KHONG CO TRONG CONFIG (chinh se khong an) ---")
print("  (khong co)" if not missing else "")
for k in missing:
    print("  %-24s <- %s" % (k, os.path.basename(used[k][0])))

print("\n--- 2) KHOA KHAI TRONG CONFIG NHUNG KHONG NOI NAO DUNG ---")
print("  (khong co)" if not unused else "")
for k in unused:
    print("  %s" % k)

print("\n--- 3) TEP DUNG HD_CFG MA KHONG Include config ---")
print("  (khong co)" if not noinc else "")
for p in noinc:
    print("  %s" % p.replace(E, ""))

# 4) HD_RELOAD_LIST
adm = rd(ADMIN)
blk = re.search(r"HD_RELOAD_LIST\s*=\s*\{(.*?)\}", adm, re.S)
lst = set()
if blk:
    for m in re.finditer(r'"([^"]+)"', blk.group(1)):
        lst.add(m.group(1).replace("\\\\", "\\").lstrip("\\").lower())
files_using = set()
for k, ps in used.items():
    for p in ps:
        rel = p.replace(E + "\\", "").lower()
        files_using.add(rel)
# bo config va menu (khong can reload theo danh sach)
files_using.discard(r"script\header\cauhinh_hoatdong.lua")
files_using.discard(r"script\item\hoatdong_admin.lua")
sot = sorted(files_using - lst)
print("\n--- 4) TEP DUNG HD_CFG BI SOT KHOI HD_RELOAD_LIST (nut 'Nap lai CONFIG' bo qua) ---")
print("  (khong sot)" if not sot else "")
for p in sot:
    print("  %s" % p)
