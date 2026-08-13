# -*- coding: utf-8 -*-
"""v3 - dung DUNG dinh dang XPack (header 32B, moi muc chi muc 16B).
Header: cSignature[4]='PACK', uCount, uIndexTableOffset, uDataOffset, uCrc32, cReserved[12]
Index : uId, uOffset, lSize, lCompressSizeFlag   (16 byte)
"""
import os, re, struct, glob, sys

CLIENT = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
PATCHU = r"D:\ServerLinux\Patch\ui"
M32 = 0xFFFFFFFF

def mk_hash(keep_slash=True, signed=True):
    """KPakList::FileNameToId. g_GetPackPath bo dau '\\' roi ha chu thuong,
    NHUNG KPakList gan lai '\\' o dau TRUOC khi bam (KPakList.cpp:93-108)."""
    def h(pb):
        p = pb.replace(b"/", b"\\")
        if p[:1] == b"\\": p = p[1:]
        p = bytes((c + 32) if 0x41 <= c <= 0x5A else c for c in p)   # g_StrLower
        if keep_slash: p = b"\\" + p
        Id = 0
        for i, c in enumerate(p):
            cc = (c - 256) if (signed and c >= 128) else c
            v = (Id + (i + 1) * cc) & M32
            Id = ((v % 0x8000000b) * 0xffffffef) & M32
        return Id ^ 0x12345678
    return h

VAR = {"co-slash+signed":   mk_hash(1, 1), "co-slash+unsigned": mk_hash(1, 0),
       "khong-slash+signed": mk_hash(0, 1), "khong-slash+unsigned": mk_hash(0, 0)}

def read_xpack(path):
    sz = os.path.getsize(path)
    with open(path, "rb") as f:
        hdr = f.read(32)
        if len(hdr) < 32 or hdr[:4] != b"PACK": return None, "sig %r" % hdr[:4]
        uCount, uIdxOff, uDataOff, uCrc = struct.unpack_from("<IIII", hdr, 4)
        if not (0 < uCount < 5_000_000) or not (32 <= uIdxOff < sz):
            return None, "header la (count=%d idx=%d size=%d)" % (uCount, uIdxOff, sz)
        f.seek(uIdxOff)
        blob = f.read(uCount * 16)
        if len(blob) < uCount * 16: return None, "chi muc cut"
        ids, d = [], {}
        for i in range(uCount):
            uid, off, lsz, flag = struct.unpack_from("<IIii", blob, i*16)
            d[uid] = (os.path.basename(path), off, lsz); ids.append(uid)
        return (d, ids), None

print("=== NAP CHI MUC (XPack) ===")
INDEX, sorted_ok, npak = {}, 0, 0
for p in sorted(glob.glob(os.path.join(CLIENT, "data", "*.pak"))):
    r, err = read_xpack(p)
    if err:
        print("  %-28s LOI: %s" % (os.path.basename(p), err)); continue
    d, ids = r; npak += 1
    if ids == sorted(ids): sorted_ok += 1
    INDEX.update({k: v for k, v in d.items() if k not in INDEX})
print("pak doc duoc: %d | chi muc sap xep tang: %d/%d  <= phai gan het thi moi dung"
      % (npak, sorted_ok, npak))
print("tong muc duy nhat: %d" % len(INDEX))

print("\n=== KIEM CHUNG: bam file roi ngoai dia ===")
loose = []
for sub in ("Spr", "Ui", "settings", "script"):
    b = os.path.join(CLIENT, sub)
    if not os.path.isdir(b): continue
    for dp, dn, fn in os.walk(b):
        for f in fn:
            rel = "\\" + os.path.relpath(os.path.join(dp, f), CLIENT)
            loose.append(rel.encode("latin-1", errors="replace"))
print("mau: %d file" % len(loose))
best = None
for name, h in VAR.items():
    hits = sum(1 for x in loose if h(x) in INDEX)
    print("  %-16s trung %6d  (%.1f%%)" % (name, hits, 100.0*hits/max(1,len(loose))))
    if best is None or hits > best[1]: best = (name, hits, h)
print("-> bien the tot nhat: %s (%d trung)" % (best[0], best[1]))
if best[1] == 0:
    print("\n!!! Van 0 trung -> CHUA ket luan duoc gi. Dung.")
    sys.exit(0)
H = best[2]

print("\n=== ANH GIAO DIEN BANG HOI JX2 CAN ===")
img_re = re.compile(rb'(?mi)^[ \t]*Image[ \t]*=[ \t]*([^\r\n;]+?)[ \t\r]*$')
inis, imgs = [], set()
for d in ("ui3_1024", "ui3_800"):
    dd = os.path.join(PATCHU, d)
    if not os.path.isdir(dd): continue
    for f in os.listdir(dd):
        try: nm = f.encode("latin-1").decode("gbk")
        except Exception: nm = f
        if any(k in nm for k in ("帮会", "帮派", "作坊", "城市管理")):
            inis.append(os.path.join(dd, f))
for p in inis:
    for m in img_re.finditer(open(p, "rb").read()):
        v = m.group(1).strip()
        if v.lower().endswith(b".spr"): imgs.add(v)
print("file .ini bang hoi: %d | duong dan .spr can: %d" % (len(inis), len(imgs)))

co, thieu = [], []
for img in sorted(imgs):
    (co if H(img) in INDEX else thieu).append(img.decode("gbk", errors="replace"))
print("\n=== KET QUA ===")
print("  CO san trong pak client : %d" % len(co))
print("  THIEU                   : %d" % len(thieu))
print("  ty le co                : %.1f%%" % (100.0*len(co)/max(1,len(imgs))))
print("\n--- CO (toi da 15) ---")
for x in co[:15]: print("   ", x)
print("--- THIEU (toi da 25) ---")
for x in thieu[:25]: print("   ", x)
base = os.path.dirname(os.path.abspath(__file__))
open(os.path.join(base,"anh_banghoi_thieu.txt"),"w",encoding="utf-8").write("\n".join(thieu))
open(os.path.join(base,"anh_banghoi_co.txt"),"w",encoding="utf-8").write("\n".join(co))
print("\n[wrote anh_banghoi_thieu.txt / anh_banghoi_co.txt]")
