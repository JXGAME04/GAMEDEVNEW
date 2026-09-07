# -*- coding: utf-8 -*-
"""[BANDO20 06/09] Doi ban do the gioi + son dong cua client JX1 sang ban tieng Viet cua VLTK 2.0.
  1) Dat 2 tep SPR `_vn` len DIA tai \\Spr\\Ui3\\<小地图>\\ (KPakFile che do 0 = dia truoc pak).
     Ten thu muc/tep GBK tren dia = byte GBK doc theo cp1252 (da chung minh bang 4 thu muc co san).
  2) Sao luu + sua DUNG 2 dong `WorldMapImage=` / `CaveMapImage=` trong settings\\MapList.ini
     (chi them `_vn` truoc `.spr`; doc/ghi latin-1 de giu nguyen byte).
  3) Kiem: tep ini tro toi phai ton tai tren dia, md5 khop ban rut tu 2.0.
Chay lai duoc (idempotent). KHONG dung code, KHONG build, KHONG swap.
"""
import os, sys, io, shutil, hashlib, re
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

CLIENT = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
SRC = r"C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\46ee1ca2-572c-4f11-8090-719731e2eab0\scratchpad\v20big"
MIRROR = r"D:\GAMEDEVNEW\clientdata_jx2"

def gbk2disk(s):
    """ten GBK -> ten Unicode ma Windows luu khi client ANSI (cp1252) mo bang CreateFileA"""
    return s.encode("gbk").decode("cp1252")

def md5(p):
    return hashlib.md5(open(p, "rb").read()).hexdigest()

# ---------- 1) dat SPR len dia ----------
files = [
    ("update_43841ADC_752x576.spr", "剑侠大地图2_vn.spr", "the gioi"),
    ("update_88D08A38_752x576.spr", "打怪向导山洞图_vn.spr", "son dong"),
]
ddir = os.path.join(CLIENT, "Spr", "Ui3", gbk2disk("小地图"))
os.makedirs(ddir, exist_ok=True)
print("thu muc:", ddir)
for src, gbkname, lab in files:
    s = os.path.join(SRC, src)
    d = os.path.join(ddir, gbk2disk(gbkname))
    if os.path.exists(d) and md5(d) == md5(s):
        print("  [=] %-9s da co, md5 khop  %s" % (lab, gbkname))
    else:
        shutil.copyfile(s, d)
        print("  [+] %-9s -> %s  (%d B, md5 %s)" % (lab, gbkname, os.path.getsize(d), md5(d)[:12]))
    assert md5(d) == md5(s)

# ---------- 2) sua MapList.ini ----------
ini = os.path.join(CLIENT, "settings", "MapList.ini")
bak = ini + ".truoc_bando20_0609"
raw = open(ini, "rb").read()
hb_before = sum(1 for c in raw if c >= 0x80)
if not os.path.exists(bak):
    shutil.copyfile(ini, bak)
    print("sao luu:", bak)
else:
    print("sao luu da co:", bak)

changed = 0
for key in (b"WorldMapImage", b"CaveMapImage"):
    pat = re.compile(rb"^(" + key + rb"=[^\r\n]*?)(_vn)?\.spr(\r?\n)", re.M)
    ms = pat.findall(raw)
    assert len(ms) == 1, "%s: mong 1 dong, thay %d" % (key.decode(), len(ms))
    m = pat.search(raw)
    if m.group(2):
        print("  [=] %s da la _vn" % key.decode())
        continue
    raw = raw[:m.start()] + m.group(1) + b"_vn.spr" + m.group(3) + raw[m.end():]
    changed += 1
    print("  [~] %s -> them _vn" % key.decode())
if changed:
    io.open(ini, "wb").write(raw)
hb_after = sum(1 for c in raw if c >= 0x80)
print("high-byte truoc/sau: %d / %d  %s" % (hb_before, hb_after, "OK" if hb_before == hb_after else "!!! LECH"))
assert hb_before == hb_after

# ---------- 3) kiem: ini tro toi dau, tep co ton tai? ----------
print("\nKIEM:")
for key in (b"WorldMapImage", b"CaveMapImage"):
    m = re.search(rb"^" + key + rb"=([^\r\n]+)", raw, re.M)
    val = m.group(1)
    disk = os.path.join(CLIENT, val.decode("cp1252").lstrip("\\"))
    ok = os.path.exists(disk)
    print("  %-14s = %s" % (key.decode(), val.decode("gbk", "replace")))
    print("     tren dia: %s  %s" % ("CO" if ok else "KHONG CO !!!", "%d B" % os.path.getsize(disk) if ok else ""))
    assert ok

# ---------- 4) guong git ----------
mdir = os.path.join(MIRROR, "settings")
os.makedirs(mdir, exist_ok=True)
shutil.copyfile(ini, os.path.join(mdir, "MapList.ini"))
mspr = os.path.join(MIRROR, "Spr", "Ui3", gbk2disk("小地图"))
os.makedirs(mspr, exist_ok=True)
for src, gbkname, lab in files:
    shutil.copyfile(os.path.join(ddir, gbk2disk(gbkname)), os.path.join(mspr, gbkname))   # guong: ten Unicode that
print("\nguong:", os.path.join(mdir, "MapList.ini"), "+", mspr)
print("XONG.")
