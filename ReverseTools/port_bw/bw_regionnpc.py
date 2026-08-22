#!/usr/bin/env python
# -*- coding: ascii -*-
"""bw_regionnpc.py - liet ke NPC trong du lieu region (maps.pak) cua mot/nhieu map,
loc theo chuoi trong ten script (vd "bw"), in tpl/x/y/ten/kind/script.
Dung lai bo giai nen + doc region cua gen_datau_spots.py (KHONG chay main cua no).

Dung:
  python bw_regionnpc.py <thu-muc-pak> <maplist.ini> <map_id[,map_id...]> [loc-script] [loc-ten]
  vd: python bw_regionnpc.py D:/ServerLinux/server1/pak D:/ServerLinux/server1/settings/maplist.ini 80,78,11 bw
"""
import os, sys, struct
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
import gen_datau_spots as G

BS = chr(92)

def doc_npc_region_full(blob):
    if len(blob) < 4:
        return None
    (num_sect,) = struct.unpack_from("<I", blob, 0)
    if num_sect != 6:
        return None
    head_size = 4 + 8 * num_sect
    if len(blob) < head_size:
        return None
    muc = []
    for k in range(num_sect):
        off, length = struct.unpack_from("<II", blob, 4 + 8 * k)
        if head_size + off + length > len(blob):
            return None
        muc.append((off, length))
    off, length = muc[2]
    beg = head_size + off
    if length < 12:
        return []
    (num_npc,) = struct.unpack_from("<I", blob, beg)
    if num_npc > 4096:
        return None
    p = beg + 12
    out = []
    for _ in range(num_npc):
        if p + 60 > len(blob):
            break
        tpl, px, py = struct.unpack_from("<iii", blob, p)
        special = blob[p + 12]
        name = blob[p + 16:p + 48].split(b"\x00")[0]
        lvl, frame, head_img, kind = struct.unpack_from("<hhhh", blob, p + 48)
        camp, series, slen = struct.unpack_from("<BBH", blob, p + 56)
        script = blob[p + 60:p + 60 + slen].split(b"\x00")[0]
        p += 60 + slen
        out.append(dict(tpl=tpl, x=px, y=py, special=special, name=name, lvl=lvl,
                        kind=kind, camp=camp, series=series, script=script))
    return out

def main():
    pakdir, maplist, mapids = sys.argv[1], sys.argv[2], sys.argv[3]
    loc_script = sys.argv[4].lower().encode() if len(sys.argv) > 4 else b""
    loc_ten = sys.argv[5].encode("latin-1") if len(sys.argv) > 5 else b""
    paks = []
    for f in sorted(os.listdir(pakdir)):
        if f.lower().endswith((".pak", ".mps")):
            try:
                paks.append(G.Pak(os.path.join(pakdir, f)))
            except Exception as e:
                print("bo qua", f, e)
    ml = G.doc_maplist(maplist)
    for mid in [int(x) for x in mapids.split(",")]:
        duongdan = ml.get(mid)
        if duongdan is None:
            print("map %d khong co trong maplist" % mid); continue
        duongdan = duongdan.replace(b"\\\\", b"\\")
        print("=== map %d  %s" % (mid, duongdan.decode("latin-1")))
        st0 = G.hstep(0, 0, b"\\maps\\" + duongdan + b"\\v_")
        nreg = 0
        for ny in range(0, G.DAI_QUET):
            u1, i1 = G.hstep(st0[0], st0[1], b"%03d\\" % ny)
            for nx in range(0, G.DAI_QUET):
                uid, _ = G.hstep(u1, i1, b"%03d_region_s.dat" % nx)
                uid ^= 0x12345678
                pk = None
                for p in paks:
                    if p.info(uid):
                        pk = p; break
                if pk is None:
                    continue
                off, size, cflag = pk.info(uid)
                if size < G.CHI_VATCAN:
                    continue
                try:
                    blob = pk.doc(uid)
                except Exception:
                    continue
                ds = doc_npc_region_full(blob)
                if ds is None:
                    continue
                if ds:
                    nTrong = sum(1 for d in ds if nx * 512 <= d["x"] < (nx + 1) * 512 and ny * 1024 <= d["y"] < (ny + 1) * 1024)
                    if nTrong * 2 < len(ds):
                        continue
                nreg += 1
                for d in ds:
                    if loc_script and loc_script not in d["script"].lower():
                        continue
                    if loc_ten and loc_ten not in d["name"]:
                        continue
                    print("  pak=%s region(%d,%d) tpl=%d o=(%d,%d) mps=(%d,%d) kind=%d lvl=%d camp=%d ten=%s script=%s" % (
                        pk.name, nx, ny, d["tpl"], d["x"] // 32, d["y"] // 32, d["x"], d["y"], d["kind"], d["lvl"], d["camp"],
                        d["name"].decode("latin-1"), d["script"].decode("latin-1")))
        print("  (region hop le: %d)" % nreg)

if __name__ == "__main__":
    main()
