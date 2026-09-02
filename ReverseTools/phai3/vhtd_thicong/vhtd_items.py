# -*- coding: ascii -*-
"""vhtd_items.py - 10 vat pham Vu Hon / Tieu Dao tu client VLTK (magicscript.txt 36 cot) vao JX1 magicscript.txt (server + client),
khuon hs_port2013.py items() (Hoa Son 4938..4966). REMAP id VLTK -> id JX1 noi duoi (magicscript: ParticularType = so dong - 1):
  5103 Vu Hon Tin Vat -> 4967 | 5106/5107 sach 90 Vu Hon -> 4968/4969 | 5215 Bi Kip Tieu Dao 150 -> 4970 | 5216-5218 Bi Kip 150 Lv21-23 -> 4971-4973
  5230 Tin Vat Tieu Dao -> 4974 | 5231/5232 sach 90 Tieu Dao -> 4975/4976
Icon thieu trong pak JX1 -> rut tu pak VLTK ra dia client; script khong co o JX1 -> "0" va bao cao.
Doc/ghi latin-1, giu CRLF, ban luu .truoc_vhtd_0209, idempotent. DUNG: python vhtd_items.py [--kiem]
"""
import io, os, sys, shutil
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
import pakdump as P
BIN = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin"
SRV, CLI = os.path.join(BIN, "server"), os.path.join(BIN, "client")
RAW = r"D:\GAMEDEVNEW\ReverseTools\phai3\vltk_raw"
P3 = r"D:\GAMEDEVNEW\ReverseTools\phai3"
KIEM = "--kiem" in sys.argv
BAK = ".truoc_vhtd_0209"
BS = chr(92); CRLF = "\r\n"
REMAP = {5103: 4967, 5106: 4968, 5107: 4969, 5215: 4970, 5216: 4971, 5217: 4972, 5218: 4973, 5230: 4974, 5231: 4975, 5232: 4976}
BAO_CAO = []
def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, d):
    if KIEM: return
    if not os.path.exists(p + BAK): shutil.copy2(p, p + BAK)
    io.open(p, "w", encoding="latin-1", newline="").write(d)
def alow(s): return "".join(ch.lower() if ord(ch) < 128 else ch for ch in s)

def main():
    rows = {}
    for l in rd(os.path.join(RAW, "settings__item__004__magicscript.txt")).replace(CRLF, "\n").split("\n"):
        c = l.split("\t")
        if len(c) > 20 and c[1] == "6" and c[2] == "1" and c[3].isdigit(): rows[int(c[3])] = c
    thieu = [k for k in REMAP if k not in rows]
    if thieu: raise SystemExit("VLTK magicscript thieu %r" % thieu)
    VROOT = r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky\data"
    vfiles = {}
    for dp, ds, fs in os.walk(VROOT):
        for f in fs:
            if f.lower().endswith((".pak", ".mps")): vfiles[f.lower()] = os.path.join(dp, f)
    idx = {}
    for l in open(os.path.join(P3, "phantich", "jx1_client_pak_index.tsv"), "rb").read().split(b"\n"):
        c = l.split(b"\t")
        if len(c) >= 2:
            try: idx[int(c[1].strip(), 16)] = c[0].decode("latin-1")
            except: pass
    vidx = {}
    vp = os.path.join(P3, r"phantich\vltk_pak_index_all.tsv")
    if os.path.exists(vp):
        for l in open(vp, "rb").read().split(b"\n"):
            c = l.split(b"\t")
            if len(c) >= 2:
                try: vidx[int(c[1].strip(), 16)] = c[0].decode("latin-1")
                except: pass
    new_lines = []
    for old in sorted(REMAP, key=lambda k: REMAP[k]):
        c = rows[old]; nid = REMAP[old]
        img = c[4]
        u = P.name2id(alow(img))
        dst = os.path.join(CLI, img.strip(BS))
        if u not in idx and not os.path.exists(dst):
            ok = False
            if u in vidx:
                try:
                    import ucl
                    pk = vfiles.get(os.path.basename(vidx[u]).lower(), vidx[u])
                    f, es = P.entries(pk)
                    for e in es:
                        if e[0] == u:
                            cs = e[3] & 0xFFFFFF; fl = e[3] >> 24
                            f.seek(e[1]); raw = f.read(cs if cs else e[2])
                            blob = raw if fl == 0 else ucl.nrv2b_decompress_8(raw, e[2])
                            if not KIEM:
                                os.makedirs(os.path.dirname(dst), exist_ok=True)
                                open(dst, "wb").write(blob)
                            ok = True; BAO_CAO.append("icon %s rut tu VLTK %s -> dia client (%d byte)" % (img, os.path.basename(pk), len(blob)))
                            break
                    f.close()
                except Exception as ex:
                    BAO_CAO.append("icon %s: loi rut %r" % (img, ex))
            if not ok:
                BAO_CAO.append("icon %s KHONG co (pak JX1 + VLTK) -> dung obj_item_lection.spr cho item %d" % (img, nid))
                img = BS + "spr" + BS + "item" + BS + "questkey" + BS + "obj_item_lection.spr"
        elif u not in idx:
            BAO_CAO.append("icon %s da co tren dia client" % img)
        script = c[13]
        if script.lower().endswith("noscript.lua") or script.strip() == "":
            script = "0"
        else:
            sp = os.path.join(SRV, script.strip(BS))
            if not os.path.exists(sp):
                BAO_CAO.append("item %d (%s): script %s KHONG co o JX1 -> dat 0" % (nid, alow(c[0]).encode("ascii", "replace").decode(), script)); script = "0"
        obj, w, h = c[5], c[6], c[7]
        price = c[10] if c[10].strip().isdigit() else "0"
        maxstack = c[20] if c[20].strip().isdigit() else "0"
        row = "\t".join([c[0], "6", "1", str(nid), img, obj, w, h, c[8], script, price, "1", maxstack, "0"] + [""] * 16)
        new_lines.append((nid, row))
    for p in (os.path.join(SRV, r"settings\item\magicscript.txt"), os.path.join(CLI, r"settings\item\magicscript.txt")):
        d = rd(p); lines = d.split(CRLF)
        if lines[-1] == "": lines = lines[:-1]
        last = lines[-1].split("\t")
        assert last[3].isdigit(), "magicscript dong cuoi la?"
        last_id = int(last[3])
        assert len(lines) == last_id + 2, "magicscript: so dong %d != id cuoi %d + 2 (bay ParticularType = so dong - 1)" % (len(lines), last_id)
        if last_id >= 4976:
            print("  [=] magicscript da co toi %d: %s" % (last_id, p)); continue
        assert last_id == 4966, "magicscript id cuoi %d != 4966 (Hoa Son)" % last_id
        for nid, row in new_lines:
            assert nid == len(lines) - 1, "lech dong: id %d vs dong %d" % (nid, len(lines))
            lines.append(row)
        wr(p, CRLF.join(lines) + CRLF)
        print("  [+] %s: +%d vat pham (4967..4976)" % (p, len(new_lines)))
    for nid, row in new_lines:
        f = row.split("\t"); print("   %d %s | %s | script %s" % (nid, alow(f[0]).encode("ascii", "replace").decode(), f[4], f[9]))
    for b in BAO_CAO: print("  * " + b)
    print("XONG%s." % (" (KIEM)" if KIEM else ""))
main()
