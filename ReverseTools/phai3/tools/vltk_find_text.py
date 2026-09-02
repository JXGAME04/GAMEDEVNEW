# -*- coding: utf-8 -*-
"""vltk_find_text.py - tim chuoi TCVN3 trong MOI muc cua cac .pak client VLTK Level Up (giai UCL neu co co 0x01/0x20).
DUNG: python vltk_find_text.py "kiem xuat ra" "ty le xuat" ...  (tham so = chuoi Unicode co dau, ghi utf-8 qua argv)
In: pak, uid, kich co, doan ngu canh (giai TCVN3 -> Unicode). Bo qua muc bat dau bang 'SPR' / 'PACK' / co kich co > 2 MB.
"""
import io, os, sys, struct, importlib.util, glob

sys.stdout.reconfigure(encoding="utf-8")
sys.path.insert(0, r"D:/GAMEDEVNEW/ReverseTools/pak_vltk")
import ucl
spec = importlib.util.spec_from_file_location("vn_edit", r"C:/Users/nguye/.claude/skills/swordonline-dev/scripts/vn_edit.py")
vn = importlib.util.module_from_spec(spec); spec.loader.exec_module(vn)
T2U = vn._TCVN3_TO_UNICODE
dec = lambda s: "".join(T2U.get(c, c) for c in s)
CLIENT = r"C:/Users/nguye/Level Up Games/Vo Lam Truyen Ky"

def entries(path):
    f = io.open(path, "rb")
    head = f.read(32)
    if head[:4] != b"PACK":
        return f, []
    cnt, idx = struct.unpack_from("<II", head, 4)
    f.seek(idx)
    raw = f.read(16 * cnt)
    es = [struct.unpack_from("<IIiI", raw, 16 * k) for k in range(cnt)]
    return f, es

def main():
    kws = [vn.unicode_to_tcvn3_bytes(a) for a in sys.argv[1:]]
    if not kws:
        print("can it nhat 1 tu khoa"); return
    paks = sorted(glob.glob(os.path.join(CLIENT, "data", "*.pak")) + glob.glob(os.path.join(CLIENT, "*.pak")))
    for pk in paks:
        f, es = entries(pk)
        hit = 0
        for uid, off, size, cf in es:
            fl = cf >> 24; cs = cf & 0xFFFFFF
            if size <= 0 or size > 2 * 1024 * 1024: continue
            f.seek(off); d = f.read(cs if cs else size)
            # NRV2B bat dau bang literal -> blob nen cua SPR mo dau '\xfbSPR' / 'SPR' ; bo qua khong can giai (nhanh gap ~50 lan)
            if d[:3] == b"SPR" or d[1:4] == b"SPR" or d[:4] == b"PACK": continue
            if fl in (0x01, 0x20) and cs and cs != size:
                try:
                    d = bytes(ucl.nrv2b_decompress_8(d, size))
                except Exception:
                    continue
            if d[:4] in (b"SPR\x00", b"PACK") or d[:3] == b"SPR" or d[:1] == b"\xfb": continue
            for k in kws:
                i = d.find(k)
                if i >= 0:
                    hit += 1
                    s = max(0, i - 80); e = min(len(d), i + 120)
                    ctx = d[s:e].decode("latin-1").replace("\r", " ").replace("\n", " | ")
                    print("%s uid=%08x size=%d off=%d :: %s" % (os.path.basename(pk), uid, size, i, dec(ctx)))
                    break
        f.close()
        if hit: print("-- %s: %d muc" % (os.path.basename(pk), hit))
    print("XONG")

if __name__ == "__main__":
    main()
