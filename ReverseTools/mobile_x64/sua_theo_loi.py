# -*- coding: utf-8 -*-
"""Sua ban tu dong loi cat cut con tro theo log MSBuild (byte-safe, chi doi ASCII tren DUNG dong compiler chi):
  C4311 'type cast': pointer truncation from 'T *' to 'unsigned int'/'int'/'DWORD'/'UINT'/'long'
  C4302 'type cast': truncation from 'T *' to 'int'/'unsigned int'/...
  -> tren dong do, doi cast (unsigned int)/(UINT)/(DWORD)/(unsigned long) -> (KUPARAM); (int)/(long) -> (KNPARAM)
     chi cho cast dung TRUOC bieu thuc con tro (&x, this, pXxx, m_pXxx, (T*)..., szXxx, lpXxx) hoac cast dau tien neu chi co mot.
  C4312 conversion from 'int'/'unsigned int' to 'T *' of greater size: KHONG tu sua (phai doi kieu bien) -> chi liet ke.
Dung: python sua_theo_loi.py <log> [--apply]
"""
import io, re, sys, os, collections
log = sys.argv[1]; apply = "--apply" in sys.argv
rx = re.compile(r"^\s*(.+?)\((\d+)(?:,\d+)?\):\s+error\s+(C4311|C4302|C4312):\s+(.*?)(?:\s+\[[^\]]+\])?\s*$")
todo = collections.OrderedDict()
for line in io.open(log, encoding="utf-8", errors="replace"):
    m = rx.match(line)
    if not m: continue
    f, ln, code, msg = m.group(1), int(m.group(2)), m.group(3), m.group(4)
    todo.setdefault(f, {})[ln] = (code, msg)

ptr_after = re.compile(rb"^\s*\(?\s*(&|this\b|[a-z]?p[A-Z]\w*|m_p[A-Z]\w*|sz\w*|lp\w*|\(\s*\w+\s*\*\s*\))")
cast_u = re.compile(rb"\(\s*(?:unsigned\s+int|UINT|DWORD|unsigned\s+long|ULONG)\s*\)")
cast_s = re.compile(rb"\(\s*(?:int|long|LONG|INT)\s*\)")

changed = 0; skipped = []
for f, lines in todo.items():
    if not os.path.exists(f): continue
    b = io.open(f, "rb").read()
    parts = b.split(b"\n"); mod = False
    for ln, (code, msg) in sorted(lines.items()):
        if ln - 1 >= len(parts): continue
        src = parts[ln - 1]
        if code == "C4312":
            skipped.append((f, ln, code, msg[:100], src.strip()[:120])); continue
        # tim cast dung truoc bieu thuc con tro
        cands = []
        for rxc, rep in ((cast_u, b"(KUPARAM)"), (cast_s, b"(KNPARAM)")):
            for m in rxc.finditer(src):
                rest = src[m.end():]
                if ptr_after.match(rest): cands.append((m.start(), m.end(), rep))
        if not cands:
            allc = [(m.start(), m.end(), b"(KUPARAM)") for m in cast_u.finditer(src)] + [(m.start(), m.end(), b"(KNPARAM)") for m in cast_s.finditer(src)]
            if len(allc) == 1: cands = allc
        if not cands:
            skipped.append((f, ln, code, msg[:100], src.strip()[:120])); continue
        # doi tat ca cast hop le tren dong (thuong 1)
        out = bytearray(); last = 0
        for s, e, rep in sorted(cands):
            out += src[last:s] + rep; last = e
        out += src[last:]
        parts[ln - 1] = bytes(out); mod = True; changed += 1
        print(("SUA " if apply else "se sua ") + f.replace("D:\\GAMEDEVNEW_wt_mobile\\Sources\\", "") + f":{ln}: " + src.strip()[:110].decode("latin-1") + "  ->  " + bytes(out).strip()[:110].decode("latin-1"))
    if mod and apply:
        nb = b"\n".join(parts)
        assert sum(1 for c in b if c > 127) == sum(1 for c in nb if c > 127), f
        io.open(f, "wb").write(nb)
print("doi", changed, "dong; bo qua", len(skipped))
for s in skipped[:40]:
    print("  BO QUA", s[0].replace("D:\\GAMEDEVNEW_wt_mobile\\Sources\\", ""), s[1], s[2], "|", s[3], "|", s[4].decode("latin-1") if isinstance(s[4], bytes) else s[4])
