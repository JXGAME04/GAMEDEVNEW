# -*- coding: utf-8 -*-
r"""[ANDROID 11/09 DULIEU] Phan tich D:\jx1_android_data: cai gi BO DUOC (khong can cho ban mobile) va bao nhieu MB.

Muc:
  A. Rac: log o goc, tep sao luu (.truoc_*, .moi, .bak, .old, __tmp__), Thumbs.db.
  B. Ban do khong co trong MapList.ini (client): thu muc maps2\<khu>\<ban do> va tep maps\<id>.fp khong dung -> bo.
  C. Pak bi che hoan toan theo thu tu tim cua KPakList (package.ini): bo ca tep; pak bi che mot phan: chi giam khi dong lai.
  D. Tep trong data\ khong phai pak / khong co trong package.ini.
  E. spr roi: dung luong theo thu muc con (de chu quyet).
Dung: python android\phan_tich_du_lieu.py > bao_cao.txt
"""
import io
import os
import re
import sys
import glob

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
GOC = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
sys.path.insert(0, os.path.join(GOC, "ReverseTools", "pak_vltk"))
import pakdump  # noqa: E402

AN = r"D:\jx1_android_data"
MB = 1048576.0


def kich(p):
    try:
        return os.path.getsize(p)
    except Exception:
        return 0


def kich_thu_muc(p):
    s = 0; n = 0
    for root, ds, fs in os.walk(p):
        for f in fs:
            s += kich(os.path.join(root, f)); n += 1
    return s, n


def ha_ascii(s):
    return "".join(c.lower() if ord(c) < 128 else c for c in s)


def muc_a():
    print("== A. RAC (bo ngay) ==")
    tong = 0
    log = [p for p in glob.glob(os.path.join(AN, "*.log"))]
    s = sum(kich(p) for p in log)
    print("  log o goc: %d tep, %.0f MB" % (len(log), s / MB)); tong += s
    mau = re.compile(r"(\.truoc_[^\\/]*|\.moi|\.bak|\.old|__tmp__|\.orig|\.tmp)$", re.I)
    sl = []; sls = 0
    thumbs = []; ts = 0
    for root, ds, fs in os.walk(AN):
        for f in fs:
            p = os.path.join(root, f)
            if mau.search(f):
                sl.append(p); sls += kich(p)
            elif f.lower() in ("thumbs.db", "desktop.ini"):
                thumbs.append(p); ts += kich(p)
    print("  sao luu (.truoc_* .moi .bak .old __tmp__): %d tep, %.1f MB" % (len(sl), sls / MB)); tong += sls
    for p in sorted(sl, key=kich, reverse=True)[:8]:
        print("     %8.2f MB  %s" % (kich(p) / MB, os.path.relpath(p, AN)))
    print("  Thumbs.db/desktop.ini: %d tep, %.1f MB" % (len(thumbs), ts / MB)); tong += ts
    print("  -> A tong: %.0f MB" % (tong / MB))
    return tong


def doc_maplist():
    p = os.path.join(AN, "settings", "maplist.ini")
    ds = {}
    loai = {}
    for ln in io.open(p, encoding="latin-1"):
        ln = ln.strip()
        if "=" not in ln:
            continue
        k, v = ln.split("=", 1)
        k = k.strip(); v = v.strip()
        if k.isdigit():
            ds[int(k)] = v
        elif k.endswith("_MapType") and k[:-8].isdigit():
            loai[int(k[:-8])] = v
    return ds, loai


def muc_b():
    print("== B. BAN DO khong co trong MapList.ini (client) ==")
    ds, loai = doc_maplist()
    dung = set(ha_ascii(v.replace("\\\\", "\\")).lower() for v in ds.values())
    print("  MapList: %d ban do; loai: %s" % (len(ds), ", ".join("%s=%d" % (t, list(loai.values()).count(t)) for t in sorted(set(loai.values())))))
    tong = 0
    # maps2\<khu>\<ban do>\ (thu muc) + <ban do>.wor + <ban do>24.jpg
    goc2 = os.path.join(AN, "maps2")
    bo = []; giu_s = 0; giu_n = 0
    for khu in os.listdir(goc2):
        pk = os.path.join(goc2, khu)
        if not os.path.isdir(pk):
            continue
        for ten in os.listdir(pk):
            p = os.path.join(pk, ten)
            if os.path.isdir(p):
                rel = (khu + "\\" + ten).lower()
                s, n = kich_thu_muc(p)
                # tep di kem cung ten (.wor, 24.jpg ...)
                kem = [os.path.join(pk, f) for f in os.listdir(pk) if not os.path.isdir(os.path.join(pk, f)) and f.lower().startswith(ten.lower())]
                s += sum(kich(f) for f in kem); n += len(kem)
                if rel in dung:
                    giu_s += s; giu_n += n
                else:
                    bo.append((s, rel, n))
    bo.sort(reverse=True)
    sb = sum(x[0] for x in bo)
    print("  maps2: giu %d ban do (%.0f MB, %d tep); BO %d ban do (%.0f MB, %d tep)" % (len(dung), giu_s / MB, giu_n, len(bo), sb / MB, sum(x[2] for x in bo)))
    for s, rel, n in bo[:12]:
        print("     %7.1f MB %6d tep  maps2\\%s" % (s / MB, n, rel))
    tong += sb
    # maps\<id>.fp
    goc1 = os.path.join(AN, "maps")
    fp_bo = []; fp_giu = 0
    for f in os.listdir(goc1):
        p = os.path.join(goc1, f)
        m = re.match(r"^(\d+)\.fp$", f, re.I)
        if m:
            if int(m.group(1)) in ds:
                fp_giu += kich(p)
            else:
                fp_bo.append((kich(p), f))
        elif os.path.isdir(p):
            pass
    sf = sum(x[0] for x in fp_bo)
    print("  maps\\*.fp: giu %.0f MB; BO %d tep khong co trong MapList (%.0f MB): %s" % (fp_giu / MB, len(fp_bo), sf / MB, ", ".join(x[1] for x in sorted(fp_bo, reverse=True)[:10])))
    tong += sf
    khac = [(kich_thu_muc(os.path.join(goc1, d)), d) for d in os.listdir(goc1) if os.path.isdir(os.path.join(goc1, d))]
    for (s, n), d in sorted(khac, reverse=True)[:6]:
        print("  maps\\%s\\: %.0f MB, %d tep (xet rieng)" % (d, s / MB, n))
    print("  -> B tong bo duoc: %.0f MB" % (tong / MB))
    return tong


def muc_c():
    print("== C. PAK theo thu tu tim (package.ini): bi che hoan toan -> bo tep ==")
    ini = io.open(os.path.join(AN, "package.ini"), encoding="latin-1").read()
    thu = re.search(r"Path=(.*)", ini).group(1).strip().strip("\\").replace("\\", "/")
    ds = sorted((int(m.group(1)), m.group(2).strip()) for m in re.finditer(r"^(\d+)=(.*)$", ini, re.M))
    thang = {}
    bang = []
    for _, ten in ds:
        p = os.path.join(AN, thu, ten)
        if not os.path.isfile(p):
            bang.append((ten, 0, 0, 0, 0, "KHONG CO TEP (xoa dong trong package.ini)"))
            continue
        f, ents = pakdump.entries(p); f.close()
        s_all = 0; s_win = 0
        for uid, off, size, cf in ents:
            csz = (cf & 0xFFFFFF) or size
            s_all += csz
            if uid not in thang:
                thang[uid] = ten; s_win += csz
        bang.append((ten, len(ents), kich(p), s_all, s_win, ""))
    tong_bo = 0; tong_che = 0
    for ten, n, sz, sa, sw, ghi in bang:
        if ghi:
            print("  %-26s %s" % (ten, ghi)); continue
        che = sa - sw
        if sw == 0:
            print("  %-26s %6d muc %8.0f MB  BI CHE 100%% -> BO" % (ten, n, sz / MB)); tong_bo += sz
        elif che > 5 * MB:
            print("  %-26s %6d muc %8.0f MB  bi che %.0f MB (%.0f%%) -> dong lai pak thi giam" % (ten, n, sz / MB, che / MB, 100.0 * che / max(1, sa))); tong_che += che
    print("  -> C: bo tep pak che 100%%: %.0f MB; dong lai cac pak che mot phan: them %.0f MB" % (tong_bo / MB, tong_che / MB))
    return tong_bo, tong_che


def muc_d():
    print("== D. Tep trong data\\ ngoai package.ini ==")
    ini = io.open(os.path.join(AN, "package.ini"), encoding="latin-1").read()
    ke = set(m.group(2).strip().lower() for m in re.finditer(r"^(\d+)=(.*)$", ini, re.M))
    tong = 0
    for f in sorted(os.listdir(os.path.join(AN, "data"))):
        p = os.path.join(AN, "data", f)
        if os.path.isfile(p) and f.lower() not in ke:
            print("  %-30s %8.1f MB  (khong ke trong package.ini)" % (f, kich(p) / MB)); tong += kich(p)
    print("  -> D tong: %.0f MB" % (tong / MB))
    return tong


def muc_e():
    print("== E. spr roi theo thu muc con (giu; chi ghi de pak) ==")
    goc = os.path.join(AN, "spr")
    ds = []
    for d in os.listdir(goc):
        p = os.path.join(goc, d)
        if os.path.isdir(p):
            s, n = kich_thu_muc(p); ds.append((s, n, d))
        else:
            ds.append((kich(p), 1, d))
    ds.sort(reverse=True)
    for s, n, d in ds[:14]:
        print("  %8.0f MB %6d tep  spr\\%s" % (s / MB, n, d))
    print("  tong spr roi: %.0f MB, %d tep" % (sum(x[0] for x in ds) / MB, sum(x[1] for x in ds)))


if __name__ == "__main__":
    a = muc_a()
    b = muc_b()
    c1, c2 = muc_c()
    d = muc_d()
    muc_e()
    tong, n = kich_thu_muc(AN)
    print("== TONG KET ==")
    print("  hien tai: %.0f MB (%d tep)" % (tong / MB, n))
    print("  bo ngay (A rac + B ban do thua + C pak che 100%% + D): %.0f MB -> con %.0f MB" % ((a + b + c1 + d) / MB, (tong - a - b - c1 - d) / MB))
    print("  dong lai pak che mot phan: them %.0f MB -> con %.0f MB" % (c2 / MB, (tong - a - b - c1 - d - c2) / MB))
