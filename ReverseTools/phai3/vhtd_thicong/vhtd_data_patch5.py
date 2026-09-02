# -*- coding: utf-8 -*-
"""vhtd_data_patch5.py [VHTD 02/09h] - du lieu dot 5 (E:\\...\\bin, KHONG trong git; ban luu <tep>.truoc_vhtd_0209h):
 1. npcs.txt (server + client)  dong 2186 'nangongjue' (NPC VLTK dau tien, id 2184) co 103 cot thay vi 87 -> KTabFile doc 87 o roi
    16 o thua + phan con lai thanh 1 hang rac => MOI mau NPC sau do LECH +1 (2607 'Tieu Dao Cu Si' -> 'Truong Tong Chinh' quai 164/165 288M mau,
    2467 'Hau Quan Vu Hon' -> hang rac 'phongdodatinh'). Cat ve 87 cot.
 2. skills.txt (server + client) 2119 'Tap Dap Luu Tinh_Khoi': MisslesForm 1->7 (tai nguoi phat), TargetEnemy 0->1, cap sat thuong = bang
    'wugoushuangxue' (Ngo Cau Suong Tuyet 2114 cung cap: seriesdamage_p / physicsenhance_p / lightingdamage_v) - GIA DINH, VLTK khong co.
 3. missles.txt (server + client) 638: CollidRange 0->2, DmgRange 0->2, DmgInterval 1->20 (danh 1 lan quanh diem den).
 4. wuhuntang.lua (server + client): 'Vu Muc Di Thu<color>' -> them dau cach truoc <color> (byte cao le nuot '<' -> hien chu <color>).
 5. MagicDesc.ini (server + client): 'Gioi han ton' -> 'Gioi han tang'.
 6. StatePos.ini: icon TRANG THAI 24x24 (\\spr\\Ui\\状态图标\\...) - 7 muc da co san trong updatejx14/15; 16 muc con lai TAO tu icon ky nang
    36x36 (thu nho) -> sprvuhontieudao3.pak (\\spr\\Ui\\状态图标\\vhtd\\<ten>.spr) + package.ini 39=.
DUNG: python vhtd_data_patch5.py [--kiem]
"""
import io, os, sys, shutil, re, struct
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\viemde")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\bandonghanh")
from vn_to_octal import unicode_to_tcvn3_bytes
from pak_id import file_name_to_id
import pakdump as P
import spr2png
from PIL import Image

KIEM = "--kiem" in sys.argv
B = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin"
MK = "[VHTD 02/09h]"
BAK = ".truoc_vhtd_0209h"
NL = "\r\n"
T = "\t"
TMP = r"C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\84112189-88a4-491d-b95a-6dce062de996\scratchpad\icon24"
os.makedirs(TMP, exist_ok=True)

def V(u):
    return unicode_to_tcvn3_bytes(u).decode("latin-1")

def rd(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()

def save(p, s, orig):
    if s == orig:
        print("  (khong doi) %s" % p); return
    if "\xef\xbf\xbd" in s:
        raise SystemExit("EF BF BD " + p)
    if KIEM:
        print("  => KIEM %s" % p); return
    if not os.path.exists(p + BAK):
        shutil.copy2(p, p + BAK)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("  => ghi %s" % p)

def both(rel):
    return [os.path.join(B, side, rel) for side in ("server", "client")]

# ------------------------------------------------------------------ 1. npcs.txt dong 2186
for p in both(r"settings\npcs.txt"):
    s = rd(p); o = s
    lines = s.split(NL)
    hdr = len(lines[0].split(T))
    bad = [i for i, l in enumerate(lines) if l and len(l.split(T)) != hdr]
    for i in bad:
        f = lines[i].split(T)
        if len(f) > hdr:
            print("  [+] npcs.txt dong %d: %d cot -> %d (%s)" % (i + 1, len(f), hdr, f[0][:20]))
            lines[i] = T.join(f[:hdr])
    s = NL.join(lines)
    save(p, s, o)

# ------------------------------------------------------------------ 2. skills.txt 2119
for p in both(r"settings\skills.txt"):
    s = rd(p); o = s
    lines = s.split(NL)
    h = lines[0].split(T); ix = {n: i for i, n in enumerate(h)}
    for i, l in enumerate(lines[1:], start=1):
        f = l.split(T)
        if len(f) > ix["SkillId"] and f[ix["SkillId"]] == "2119":
            if f[ix["MisslesForm"]] == "7" and f[ix["LvlSetting1"]] == "seriesdamage_p":
                print("  [=] skills 2119 da vá"); break
            f[ix["MisslesForm"]] = "7"
            f[ix["TargetEnemy"]] = "1"
            for k, name in ((1, "seriesdamage_p"), (2, "physicsenhance_p"), (3, "lightingdamage_v")):
                f[ix["LvlSetting%d" % k]] = name
                f[ix["LvlData%d" % k]] = "wugoushuangxue"
            lines[i] = T.join(f)
            print("  [+] skills 2119: form 7, TargetEnemy 1, sat thuong = wugoushuangxue (2114)")
            break
    save(p, NL.join(lines), o)

# ------------------------------------------------------------------ 3. missles.txt 638
for p in both(r"settings\missles.txt"):
    s = rd(p); o = s
    lines = s.split(NL)
    h = lines[0].split(T); ix = {n: i for i, n in enumerate(h)}
    for i, l in enumerate(lines[1:], start=1):
        f = l.split(T)
        if f and f[0] == "638":
            if f[ix["CollidRange"]] == "2":
                print("  [=] missle 638 da vá"); break
            f[ix["CollidRange"]] = "2"; f[ix["DmgRange"]] = "2"; f[ix["DmgInterval"]] = "20"
            lines[i] = T.join(f)
            print("  [+] missle 638: CollidRange/DmgRange 2, DmgInterval 20")
            break
    save(p, NL.join(lines), o)

# ------------------------------------------------------------------ 4. wuhuntang.lua <color>
for p in both(r"script\skill\wuhuntang.lua"):
    s = rd(p); o = s
    old = "Di Th\xad<color>"
    if old in s:
        s = s.replace(old, "Di Th\xad <color>"); print("  [+] wuhuntang.lua: dau cach truoc <color>")
    else:
        print("  [=] wuhuntang.lua")
    save(p, s, o)

# ------------------------------------------------------------------ 5. MagicDesc.ini ton -> tang
for p in both(r"settings\MagicDesc.ini"):
    s = rd(p); o = s
    old = V(u"Giới hạn tồn"); new = V(u"Giới hạn tầng")
    if old in s:
        s = s.replace(old, new); print("  [+] MagicDesc: 'Gioi han tang'")
    else:
        print("  [=] MagicDesc")
    save(p, s, o)

# ------------------------------------------------------------------ 6. icon trang thai
ZT = (b"\\spr\\Ui\\" + bytes([0xd7, 0xb4, 0xcc, 0xac, 0xcd, 0xbc, 0xb1, 0xea]) + b"\\").decode("latin-1")   # \spr\Ui\状态图标\
CO_SAN = {1965, 1966, 1968, 1971, 1973, 1982, 1987}          # \spr\Ui\状态图标\<ten>.spr da co trong updatejx14/15
TAO = {1976: 1976, 2116: 2116, 1989: 1976, 1991: 1976, 2128: 2128, 2130: 2130, 2131: 2131, 2133: 2133, 2134: 2134, 2139: 2139,
       1366: 1366, 1376: 1376, 1380: 1380, 1381: 1381, 1988: 1988}   # muc StatePos -> ky nang lay icon nguon
C = os.path.join(B, "client")
sk = {}
for l in io.open(os.path.join(C, "settings", "skills.txt"), encoding="latin-1"):
    r = l.rstrip("\r\n").split("\t")
    if len(r) > 5 and r[2].isdigit():
        sk[int(r[2])] = r[5].strip()
idx = {}
for l in io.open(r"D:\GAMEDEVNEW\ReverseTools\phai3\phantich\jx1_client_pak_index.tsv", encoding="latin-1"):
    q = l.rstrip("\n").split("\t")
    if len(q) >= 2: idx.setdefault(q[1].upper(), []).append(q[0])
def pak_uids(path):
    f = io.open(path, "rb"); hh = f.read(32); sig, count, ioff, doff = struct.unpack_from("<4sIII", hh, 0); f.seek(ioff); raw = f.read(count * 16)
    return set(struct.unpack_from("<I", raw, i * 16)[0] for i in range(count))
for pk in ("sprvuhontieudao.pak", "sprvuhontieudao2.pak"):
    for u in pak_uids(os.path.join(C, "data", pk)): idx.setdefault("%08X" % u, []).append(pk)

def doc_tep(path):
    u = file_name_to_id(path)
    paks = idx.get("%08X" % u)
    if not paks: raise SystemExit("khong thay trong pak: " + path.encode("latin-1").decode("gbk", "replace"))
    import ucl
    last = None
    for pk in paks:      # cung uid co the nam o nhieu pak; muc co flag 0x01 (UCL) / 0x20 (VNG = UCL, XPackFile::ExtractRead) thi giai NRV2B ca blob
        f, es = P.entries(os.path.join(C, "data", pk))
        for e in es:
            if e[0] == u:
                uid_, off_, size_, cf_ = e
                fl = cf_ >> 24; cs = cf_ & 0xFFFFFF
                f.seek(off_); d = f.read(cs if cs else size_); last = (pk, hex(fl), d[:4])
                if fl in (0x01, 0x20) and cs and cs != size_:
                    d = bytes(ucl.nrv2b_decompress_8(d, size_))
                elif fl == 0x10:
                    d = giai_spr_khung(d)
                if d[:4] == b"SPR\x00":
                    return d
    raise SystemExit("khong co ban SPR cho %s (%r)" % (path.encode("latin-1").decode("gbk", "replace"), last))

def spr_to_image(d):
    """SPR thuong (KSprite.h): SPRHEAD 32 byte, KPAL24 x Colors, SPROFFS{Offset,Length} x Frames, SPRFRAME{w,h,ox,oy} + RLE (count, alpha[, count chi so])."""
    W, H, cx, cy, frames, colors, dirs, itv = struct.unpack_from("<HHHHHHHH", d, 4)
    off = 32; pal = d[off:off + colors * 3]; off += colors * 3
    fo, fl = struct.unpack_from("<II", d, off); base = off + 8 * frames
    p = base + fo
    fw, fh, ox, oy = struct.unpack_from("<HHHH", d, p); p += 8
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0)); px = img.load()
    for y in range(fh):
        x = 0
        while x < fw:
            n, a = d[p], d[p + 1]; p += 2
            if a:
                for k in range(n):
                    i = d[p + k]; r, g, b = pal[i * 3], pal[i * 3 + 1], pal[i * 3 + 2]
                    if ox + x + k < W and oy + y < H:
                        px[ox + x + k, oy + y] = (r, g, b, min(255, a * 255 // 246))
                p += n
            x += n
    return img

def giai_spr_khung(d):
    """XPackFile::GetSprHeader/GetSprFrame (TYPE_FRAME): [SPRHEAD 28][KPAL24 x Colors][XPackSprFrameInfo{lCompressSize,lSize} x Frames][khung...]
    khung: lSize < 0 -> luu tho |lSize| byte; lSize > 0 -> UCL NRV2B lCompressSize byte -> lSize byte SPRFRAME. Ghep lai thanh SPR thuong."""
    import ucl
    W, H, cx, cy, frames, colors, dirs, itv = struct.unpack_from("<HHHHHHHH", d, 4)
    off = 28
    pal = d[off:off + colors * 3]; off += colors * 3
    infos = []
    for i in range(frames):
        cs, sz = struct.unpack_from("<ii", d, off); off += 8
        infos.append((cs, sz))
    fr = []
    for cs, sz in infos:
        if sz < 0:
            fr.append(d[off:off - sz]); off += -sz
        else:
            fr.append(bytes(ucl.nrv2b_decompress_8(d[off:off + cs], sz))); off += cs
    out = bytearray(b"SPR\x00" + struct.pack("<HHHHHHHH", W, H, cx, cy, frames, colors, dirs, itv) + bytes(8) + pal)
    o2 = 0
    for f_ in fr:
        out += struct.pack("<II", o2, len(f_)); o2 += len(f_)
    for f_ in fr:
        out += f_
    return bytes(out)

def encode_spr(img):
    """SPR 1 khung, 256 mau - bo cuc THAT cua client (KSprite.h + do tren updatejx15 'SPR\\0'): SPRHEAD 32 byte (Reserved[6] WORD),
    KPAL24 x 256, SPROFFS{0, len}, SPRFRAME{w,h,0,0} + RLE (count, alpha[, count chi so]); alpha mo = 0xF6 (246) nhu tep goc VNG,
    nua trong suot ti le theo 0xF6, < 32 = trong."""
    W, H = img.size
    q = img.convert("RGB").quantize(colors=256, method=Image.Quantize.MEDIANCUT)
    pal = q.getpalette()[:768]; pal += [0] * (768 - len(pal))
    qi = q.load(); a = img.split()[3].load()
    rle = bytearray()
    for y in range(H):
        x = 0
        while x < W:
            n = 0
            while x + n < W and a[x + n, y] < 32 and n < 255: n += 1
            if n:
                rle += bytes((n, 0)); x += n; continue
            al = max(1, min(246, a[x, y] * 246 // 255))
            n = 0
            while x + n < W and a[x + n, y] >= 32 and max(1, min(246, a[x + n, y] * 246 // 255)) == al and n < 255: n += 1
            rle += bytes((n, al))
            for k in range(n): rle.append(qi[x + k, y])
            x += n
    frame = struct.pack("<HHHH", W, H, 0, 0) + bytes(rle)
    out = bytearray()
    out += struct.pack("<4sHHHHHHHH", b"SPR\x00", W, H, 0, 0, 1, 256, 1, 0)
    out += bytes(12)
    out += bytes(pal)
    out += struct.pack("<II", 0, len(frame))
    out += frame
    return bytes(out)

def tao_icon24(sid):
    src = sk[sid]
    blob = doc_tep(src)
    io.open(os.path.join(TMP, "%d_in.spr" % sid), "wb").write(blob)
    im = spr_to_image(blob)
    im.save(os.path.join(TMP, "%d.png" % sid))
    if im.size != (24, 24):
        im = im.resize((24, 24), Image.LANCZOS)
    data = encode_spr(im)
    io.open(os.path.join(TMP, "%d_24.spr" % sid), "wb").write(data)
    spr_to_image(data).save(os.path.join(TMP, "%d_24.png" % sid))     # kiem: giai ma lai duoc bang chinh bo cuc that
    return data

sp_path = os.path.join(C, "ui", "StatePos.ini")
sp = rd(sp_path); sp_o = sp
def set_image(sid, newpath):
    global sp
    m = re.search(r"(Buff_(\d+)_ID=%d\r\n[^\r\n]*\r\n)Buff_\2_Image=([^\r\n]*)\r\n" % sid, sp)
    if not m: print("  [?] StatePos khong co muc %d" % sid); return
    if m.group(3) == newpath: print("  [=] StatePos %d" % sid); return
    sp = sp[:m.start()] + m.group(1) + "Buff_%s_Image=%s\r\n" % (m.group(2), newpath) + sp[m.end():]
    print("  [+] StatePos %d -> %s" % (sid, newpath.encode("latin-1")[-34:]))
for sid in sorted(CO_SAN):
    base = sk[sid].split("\\")[-1]
    set_image(sid, ZT + base)
pak_items = {}
for sid, src_sid in sorted(TAO.items()):
    base = sk[src_sid].split("\\")[-1]
    newpath = ZT + "vhtd\\" + base
    u = file_name_to_id(newpath)
    if u not in pak_items:
        pak_items[u] = (newpath, tao_icon24(src_sid))
    set_image(sid, newpath)
save(sp_path, sp, sp_o)

pak_out = os.path.join(C, "data", "sprvuhontieudao3.pak")
if not KIEM:
    ents = sorted(pak_items.items())
    with open(pak_out, "wb") as w:
        w.write(b"\x00" * 32); off = 32; index = []
        for u, (path, data) in ents:
            w.write(data); index.append((u, off, len(data), len(data))); off += len(data)
        ioff = off
        for u, o_, size, cf in index: w.write(struct.pack("<IIiI", u, o_, size, cf))
        w.seek(0); w.write(struct.pack("<4sIIII", b"PACK", len(index), ioff, 32, 0) + b"\x00" * 12)
    print("  => ghi %s (%d tep)" % (pak_out, len(ents)))
else:
    print("  => KIEM pak %d tep" % len(pak_items))
pi = os.path.join(C, "package.ini")
s = rd(pi); o = s
if "sprvuhontieudao3.pak" not in s:
    s = s.rstrip("\r\n") + NL + "39=sprvuhontieudao3.pak" + NL
    print("  [+] package.ini 39=sprvuhontieudao3.pak")
save(pi, s, o)
print("XONG.")
