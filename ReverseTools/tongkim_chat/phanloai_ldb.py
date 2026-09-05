# -*- coding: ascii -*-
# Phan loai 12 tep trung "Lien Dau Bang" rut tu slistcl.pak 2.0: ini hay lua, section, ScriptFile/Image, ham, duong dan.
import glob, re, sys, os
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
import vn_to_octal as vo

# bang giai ma TCVN3 -> unicode: dao tu bang ma hoa cua vn_to_octal (tim dict co gia tri int/bytes)
rev = {}
for name in dir(vo):
    obj = getattr(vo, name)
    if isinstance(obj, dict) and obj:
        k0 = next(iter(obj)); v0 = obj[k0]
        if isinstance(k0, str) and isinstance(v0, int):
            for k, v in obj.items(): rev[v] = k
        elif isinstance(k0, int) and isinstance(v0, str):
            for k, v in obj.items(): rev[k] = v
def tcvn(b):
    out = []
    for ch in b:
        if ch < 0x80: out.append(chr(ch))
        else: out.append(rev.get(ch, "<%02x>" % ch))
    return "".join(out)

BS = chr(92)
key_ldb = vo.unicode_to_tcvn3_bytes("Li" + chr(0xEA) + "n " + chr(0x110) + chr(0x1EA5) + "u Bang")
for fn in sorted(glob.glob("ldb_*.bin")):
    d = open(fn, "rb").read()
    head = d.lstrip()[:1]
    kind = "ini" if head == b"[" else ("lua" if (b"function" in d[:6000] or b"local " in d[:6000]) else "?")
    print("=====", fn, len(d), kind)
    if kind == "ini":
        secs = re.findall(rb"^" + rb"\[" + rb"([^" + rb"\]" + rb"\r\n]+)" + rb"\]", d, re.M)
        print("  sections(%d): %s" % (len(secs), ", ".join(s.decode("gbk", "replace") for s in secs[:45])[:700]))
        for key in [rb"ScriptFile=[^\r\n]*", rb"Image=[^\r\n]*", rb"Text=[^\r\n]*", rb"Title=[^\r\n]*"]:
            vals = re.findall(key, d)
            print("  %-11s %d -> %s" % (key.decode()[:10], len(vals), [tcvn(v)[:60] for v in vals[:10]]))
    else:
        fns = re.findall(rb"function\s+([A-Za-z_0-9:.]+)", d)
        print("  functions(%d): %s" % (len(fns), ", ".join(f.decode("latin-1") for f in fns[:40])[:600]))
        uis = sorted(set(re.findall(rb"UI_[A-Z_0-9]+", d)))
        print("  UI_: %s" % [u.decode() for u in uis[:15]])
        paths = sorted(set(re.findall(rb"[A-Za-z0-9_" + rb"\\" + rb"/\x80-\xff]+" + rb"\.(?:ini|spr|lua)", d)))
        print("  paths: %s" % [p.decode("gbk", "replace")[:60] for p in paths[:15]])
    for m in list(re.finditer(re.escape(key_ldb), d))[:3]:
        s0 = d.rfind(b"\n", 0, m.start()) + 1; e = d.find(b"\n", m.start())
        if e < 0: e = len(d)
        print("  LDB:", tcvn(d[s0:e]).strip()[:160])
