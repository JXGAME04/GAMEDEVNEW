r"""phuchoi_disan.py - PHUC HOI script bi xep nham la 'chet' (su co trap 06/09 toi):
  1) chuyen TOAN BO tep trong _luutru\0609\disan_jx ve DUNG duong dan cu trong bin\server (script\...)
  2) chuyen script\tinhnang\trapcu\<pinyin>\... ve script\<khu GBK>\... (nhu truoc sap xep) va xoa dong bi danh tuong ung
  3) ghi nhat ky _luutru\0609\NHATKY_PHUCHOI_0609.txt
  python phuchoi_disan.py        -> chi kiem (khong doi gi)
  python phuchoi_disan.py sua    -> thuc hien
"""
import os, sys, shutil, io
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
ROOT = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
DISAN = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\_luutru\0609\disan_jx"
NHATKY = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\_luutru\0609\NHATKY_PHUCHOI_0609.txt"
ALIAS = os.path.join(ROOT, "script", "_duongdan_cu.txt")
SUA = len(sys.argv) > 1 and sys.argv[1] == "sua"
BS = chr(92)

def disk_name(b):
    """bytes ANSI (GBK) -> ten tep unicode nhu Windows cp1252 hien thi (5 byte khong dinh nghia -> U+0080+x)"""
    out = []
    for c in b:
        if c < 128 or c in (0x81, 0x8D, 0x8F, 0x90, 0x9D): out.append(chr(c))
        else: out.append(bytes([c]).decode("cp1252"))
    return "".join(out)

# pinyin -> ten thu muc GBK goc (r33_sapxep.py HAN_GBK)
PINYIN = {
    "trungnguyenbac": bytes([0xd6, 0xd0, 0xd4, 0xad, 0xb1, 0xb1, 0xc7, 0xf8]),
    "taynamnam":      bytes([0xce, 0xf7, 0xc4, 0xcf, 0xc4, 0xcf, 0xc7, 0xf8]),
    "luongho":        bytes([0xc1, 0xbd, 0xba, 0xfe, 0xc7, 0xf8]),
    "giangnam":       bytes([0xbd, 0xad, 0xc4, 0xcf, 0xc7, 0xf8]),
}
log = []
def L(s):
    log.append(s); print(s)

# ---- 1) disan_jx -> ROOT
n1 = 0; xung_dot = []
for dp, dn, fn in os.walk(DISAN):
    for f in fn:
        src = os.path.join(dp, f)
        rel = os.path.relpath(src, DISAN)
        dst = os.path.join(ROOT, rel)
        if os.path.exists(dst):
            xung_dot.append(rel); continue
        n1 += 1
        if SUA:
            os.makedirs(os.path.dirname(dst), exist_ok=True)
            shutil.move(src, dst)
            log.append("PHUC HOI: " + rel)
L("1) disan_jx -> cay chay: %d tep%s, xung dot (dich da co): %d" % (n1, "" if SUA else " (kiem)", len(xung_dot)))
for x in xung_dot[:10]: L("   XUNG DOT: " + x)

# ---- 2) trapcu -> khu GBK goc
TRAPCU = os.path.join(ROOT, "script", "tinhnang", "trapcu")
n2 = 0; xung2 = []
prefix_moi = {}   # 'script\tinhnang\trapcu\<pinyin>' (lower) -> 'script\<GBK>'
if os.path.isdir(TRAPCU):
    for py in os.listdir(TRAPCU):
        if py not in PINYIN:
            L("   ?? thu muc trapcu khong biet: " + py); continue
        gbk_dir = disk_name(PINYIN[py])
        base = os.path.join(TRAPCU, py)
        for dp, dn, fn in os.walk(base):
            for f in fn:
                src = os.path.join(dp, f)
                rel_in = os.path.relpath(src, base)
                dst = os.path.join(ROOT, "script", gbk_dir, rel_in)
                if os.path.exists(dst):
                    xung2.append(rel_in); continue
                n2 += 1
                if SUA:
                    os.makedirs(os.path.dirname(dst), exist_ok=True)
                    shutil.move(src, dst)
                    log.append("VE CHO CU: script%strapcu%s%s%s -> script%s%s%s%s" % (BS, BS, py, BS, BS, gbk_dir, BS, rel_in))
L("2) trapcu -> khu goc: %d tep, xung dot: %d" % (n2, len(xung2)))
if SUA and os.path.isdir(TRAPCU):
    # xoa thu muc rong
    for dp, dn, fn in os.walk(TRAPCU, topdown=False):
        try: os.rmdir(dp)
        except OSError: pass
    L("   da xoa thu muc rong trapcu: %s" % (not os.path.isdir(TRAPCU)))

# ---- 3) xoa dong bi danh tro vao trapcu
raw = open(ALIAS, "rb").read().split(b"\n")
giu = []; bo = 0
for line in raw:
    if line.startswith(b"--@") and b"=" in line:
        moi = line.split(b"=", 1)[1].strip().lower().replace(b"/", b"\\")
        if moi.startswith(b"script\\tinhnang\\trapcu\\"):
            bo += 1; continue
    giu.append(line)
L("3) bi danh: bo %d dong tro vao trapcu, con %d dong --@" % (bo, sum(1 for l in giu if l.startswith(b"--@"))))
if SUA:
    shutil.copy2(ALIAS, ALIAS + ".truoc_phuchoi_0609")
    open(ALIAS, "wb").write(b"\n".join(giu))
    with io.open(NHATKY, "a", encoding="utf-8", errors="replace") as f:
        f.write("\n".join(log) + "\n")
    L("Da ghi nhat ky: " + NHATKY)
