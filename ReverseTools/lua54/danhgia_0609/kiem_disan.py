"""kiem_disan.py - KIEM LAI 1.371 tep Han da chuyen vao _luutru/0609/disan_jx bang bo ma DUNG (cp1252 + 5 byte dieu khien),
   doi chieu voi: token .lua trong settings + script (byte), ID bam trap (Region_S.dat), C++.  [sua] = chuyen ve cay chay neu con tham chieu.
   python kiem_disan.py [sua]"""
import os, sys, re, struct, collections, shutil
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
ROOT = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
DISAN = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\_luutru\0609\disan_jx"
SUA = len(sys.argv) > 1 and sys.argv[1] == "sua"
BS = chr(92)

def ansi_bytes(s):
    out = bytearray()
    for ch in s:
        o = ord(ch)
        if o < 128 or o in (0x81, 0x8D, 0x8F, 0x90, 0x9D): out.append(o)
        else:
            try: out += ch.encode("cp1252")
            except UnicodeEncodeError: out += b"?"
    return bytes(out)
def key_of(rel_unicode):
    return ansi_bytes(rel_unicode.replace("/", BS)).lower()

def fid(b):
    Id = 0
    for i, ch in enumerate(b):
        c = ch if ch < 128 else ch - 256
        Id = ((Id + (i + 1) * c) & 0xFFFFFFFF) % 0x8000000B
        Id = (Id * 0xFFFFFFEF) & 0xFFFFFFFF
    return Id ^ 0x12345678

# 1) tap token .lua (byte, thuong) tu settings + script + scriptjx2 + C++
rx = re.compile(rb'([A-Za-z0-9_\-\.\x80-\xff/\\]+\.lua)', re.I)
toks = set()
def norm(t):
    t = t.replace(b"\\\\", b"\\").replace(b"/", b"\\").lower().lstrip(b"\\")
    i = t.find(b"script\\")
    return t[i:] if i >= 0 else None
def scan(d, maxmb=8):
    for dp, dn, fn in os.walk(d):
        for f in fn:
            p = os.path.join(dp, f)
            try:
                if os.path.getsize(p) > maxmb * 1048576: continue
                data = open(p, "rb").read()
            except Exception: continue
            if b".lua" not in data.lower(): continue
            for m in rx.finditer(data):
                k = norm(m.group(1))
                if k: toks.add(k)
scan(os.path.join(ROOT, "settings")); scan(os.path.join(ROOT, "script")); scan(os.path.join(ROOT, "scriptjx2"))
scan(r"D:\GAMEDEVNEW\Sources\Core\Src", 4)
# 2) ID trap
trap_ids = set()
for dp, dn, fn in os.walk(os.path.join(ROOT, "Maps")):
    for f in fn:
        if not f.lower().endswith("region_s.dat"): continue
        data = open(os.path.join(dp, f), "rb").read()
        if len(data) < 52: continue
        cnt = struct.unpack_from("<I", data, 0)[0]
        if cnt < 6 or cnt > 64: continue
        off, ln = struct.unpack_from("<II", data, 4 + 8 * 1)
        head = 4 + 8 * cnt
        if ln >= 12:
            n = struct.unpack_from("<I", data, head + off)[0]
            if 12 + n * 8 == ln:
                for i in range(n):
                    trap_ids.add(struct.unpack_from("<BBBBI", data, head + off + 12 + i * 8)[4])
print("token .lua (byte): %d; ID trap: %d" % (len(toks), len(trap_ids)))
# 3) doi chieu tung tep trong disan_jx
con_dung = []
n = 0
for dp, dn, fn in os.walk(DISAN):
    for f in fn:
        p = os.path.join(dp, f)
        rel = os.path.relpath(p, DISAN)                  # script\...\x.lua (unicode)
        n += 1
        k = key_of(rel)
        ref = []
        if k in toks: ref.append("token")
        if fid(b"\\" + k) in trap_ids: ref.append("trap")
        # dang GBK-that: neu ten chua ky tu dieu khien, thu them decode cp1252 loi
        if ref:
            con_dung.append((rel, ref))
print("Tep trong disan_jx: %d; CON DUOC THAM CHIEU (bo ma dung): %d" % (n, len(con_dung)))
for rel, ref in con_dung[:40]:
    print("  ", rel, "<-", ",".join(ref))
if SUA and con_dung:
    for rel, ref in con_dung:
        src = os.path.join(DISAN, rel); dst = os.path.join(ROOT, rel)
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        shutil.move(src, dst)
    print("DA CHUYEN VE cay chay: %d tep" % len(con_dung))
