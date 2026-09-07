r"""pak_region.py - quet moi entry trong bin\server\Pak\*.pak, nhan dien tep vung dang 'combined'
(DWORD count + {off,len}*count), lay ID trap (doan 1) va ten script NPC (doan 2).
Dung boi kiem_trap_map.py. Giai nen UCL NRV2B bang ReverseTools\pak_vltk\ucl.py (flag 0x01 / 0x20)."""
import os, sys, struct, collections
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
import ucl
BS = chr(92)

def _entries(p):
    f = open(p, "rb")
    sig, count, ioff, doff = struct.unpack("<IIII", f.read(16))
    f.seek(ioff)
    raw = f.read(count * 16)
    out = []
    for i in range(count):
        uid, off, size, cf = struct.unpack("<IIiI", raw[i * 16:i * 16 + 16])
        out.append((uid, off, size, cf))
    return f, out

def _read(f, e):
    uid, off, size, cf = e
    csize = cf & 0xFFFFFF
    flag = cf >> 24
    f.seek(off)
    blob = f.read(csize if csize else size)
    if flag == 0 or csize in (0, size):
        return blob[:size]
    if flag in (0x01, 0x20):
        try:
            return ucl.nrv2b_decompress_8(blob, size)
        except Exception:
            return None
    return None

def parse_region(data):
    """-> (list trap id, list ten script npc bytes) hoac None neu khong phai tep vung"""
    if len(data) < 52: return None
    cnt = struct.unpack_from("<I", data, 0)[0]
    if cnt < 2 or cnt > 64: return None
    head = 4 + 8 * cnt
    if len(data) < head: return None
    secs = [struct.unpack_from("<II", data, 4 + 8 * i) for i in range(cnt)]
    tong = head + sum(ln for off, ln in secs)
    if tong != len(data):
        # cho phep chenh lech nho (padding), nhung tung doan phai nam trong tep
        for off, ln in secs:
            if head + off + ln > len(data): return None
    traps = []; npcs = []
    off, ln = secs[1]
    if ln >= 12:
        n = struct.unpack_from("<I", data, head + off)[0]
        if 12 + n * 8 == ln:
            for i in range(n):
                cx, cy, ncell, res, tid = struct.unpack_from("<BBBBI", data, head + off + 12 + i * 8)
                if tid: traps.append(tid)
    if cnt > 2:
        off, ln = secs[2]
        if ln >= 12:
            n = struct.unpack_from("<I", data, head + off)[0]
            pos = head + off + 12
            end = head + off + ln
            for i in range(n):
                if pos + 60 > end: break
                slen = struct.unpack_from("<H", data, pos + 58)[0]
                name = data[pos + 60: pos + 60 + slen]
                pos += 60 + slen
                name = name.split(b"\0")[0].strip()
                if name: npcs.append(name)
    return traps, npcs

def scan_paks(pak_dir, max_size=131072, log=print):
    trap_ref = collections.defaultdict(set)
    npc_ref = collections.defaultdict(set)
    stats = {}
    for fn in sorted(os.listdir(pak_dir)):
        if not fn.lower().endswith(".pak"): continue
        p = os.path.join(pak_dir, fn)
        try:
            f, es = _entries(p)
        except Exception as e:
            log("  pak loi %s: %s" % (fn, e)); continue
        n_reg = 0; n_trap = 0; n_npc = 0; n_try = 0
        for e in es:
            uid, off, size, cf = e
            if size < 52 or size > max_size: continue
            d = _read(f, e)
            if not d: continue
            n_try += 1
            r = parse_region(d)
            if r is None: continue
            traps, npcs = r
            if not traps and not npcs: continue
            n_reg += 1
            for t in traps:
                trap_ref[t].add("pak:" + fn); n_trap += 1
            for nm in npcs:
                npc_ref[nm].add("pak:" + fn); n_npc += 1
        f.close()
        stats[fn] = (len(es), n_try, n_reg, n_trap, n_npc)
        log("  %-32s entry %6d, thu %6d, tep vung co trap/npc %5d, o trap %6d, npc %6d" % (fn, len(es), n_try, n_reg, n_trap, n_npc))
    return trap_ref, npc_ref, stats

if __name__ == "__main__":
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    t, n, s = scan_paks(sys.argv[1] if len(sys.argv) > 1 else r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\Pak")
    print("ID trap khac nhau:", len(t), " ten script NPC khac nhau:", len(n))
