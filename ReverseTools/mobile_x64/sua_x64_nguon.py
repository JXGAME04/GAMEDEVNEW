# -*- coding: utf-8 -*-
"""Sua hang loat (byte-safe, latin-1, giu CRLF) cac kieu tham so mang con tro trong he KWnd / CoreDataChanged:
  unsigned int uParam, int nParam   -> KUPARAM uParam, KNPARAM nParam
  (unsigned int)(KWndWindow*)this   -> (KUPARAM)(KWndWindow*)this
  (unsigned int)&x / (DWORD)&x / (UINT)&x  -> (KUPARAM)&x
  (int)&x / (int)(&x)               -> (KNPARAM)&x
  (unsigned int)pXxx, (unsigned int)this, (DWORD)pXxx, (int)pXxx, (int)this ... (chi khi theo sau la , hoac ) )
Tren Win32: KUPARAM == unsigned int, KNPARAM == int (typedef trong KWin32.h) -> nhi phan khong doi.
Cach dung: python sua_x64_nguon.py <root> [--apply]   (mac dinh chi dem)
"""
import io, os, re, sys

root = sys.argv[1]
apply = "--apply" in sys.argv
dirs = [os.path.join(root, "Sources", "S3Client"), os.path.join(root, "Sources", "Core", "Src")]

PTR = r"(?:this|&\s*\w+|\(\s*&\s*\w+\s*\)|p[A-Z]\w*|m_p[A-Z]\w*|sz\w*|m_sz\w*|lp\w*|pc\w*|pb\w*|pw\w*|str\w*)"
rules = [
    (re.compile(r"unsigned\s+int\s+uParam\s*,\s*int\s+nParam"), "KUPARAM uParam, KNPARAM nParam"),
    (re.compile(r"\(\s*(?:unsigned\s+int|UINT|DWORD|unsigned\s+long)\s*\)\s*\(\s*KWndWindow\s*\*\s*\)"), "(KUPARAM)(KWndWindow*)"),
    (re.compile(r"\(\s*int\s*\)\s*\(\s*KWndWindow\s*\*\s*\)"), "(KNPARAM)(KWndWindow*)"),
    (re.compile(r"\(\s*(?:unsigned\s+int|UINT|DWORD|unsigned\s+long)\s*\)\s*(?=&\w|\(\s*&\w+\s*\))"), "(KUPARAM)"),
    (re.compile(r"\(\s*int\s*\)\s*(?=&\w|\(\s*&\w+\s*\))"), "(KNPARAM)"),
    (re.compile(r"\(\s*(?:unsigned\s+int|UINT|DWORD|unsigned\s+long)\s*\)\s*(?=\(?\s*" + PTR + r"\s*\)?\s*[,)])"), "(KUPARAM)"),
    (re.compile(r"\(\s*int\s*\)\s*(?=\(?\s*" + PTR + r"\s*\)?\s*[,)])"), "(KNPARAM)"),
]
# khong dong vao cac tep nay (khong lien quan UI param)
skip_names = {"vcpkg_installed"}
skip_files = {"KPlayerBot.cpp"}   # chi may chu: giu nhi phan CoreServer y nguyen

tot = {i: 0 for i in range(len(rules))}
files_changed = 0
report = []
for d in dirs:
    for dp, dn, fn in os.walk(d):
        if any(s in dp for s in skip_names): continue
        for f in fn:
            if not f.lower().endswith((".cpp", ".h")): continue
            if f in skip_files: continue
            p = os.path.join(dp, f)
            b = io.open(p, "rb").read()
            if b"\xef\xbf\xbd" in b:
                pass  # da hong san, van xu ly nhung khong tao them
            s = b.decode("latin-1")
            orig = s
            cnt = []
            for i, (rx, rep) in enumerate(rules):
                s, n = rx.subn(rep, s)
                cnt.append(n); tot[i] += n
            if s != orig:
                files_changed += 1
                report.append((os.path.relpath(p, root), cnt))
                if apply:
                    nb = s.encode("latin-1")
                    # kiem tra byte cao khong doi (chi doi ASCII)
                    hb_old = sum(1 for c in b if c > 127); hb_new = sum(1 for c in nb if c > 127)
                    assert hb_old == hb_new, p
                    io.open(p, "wb").write(nb)
print("tep doi:", files_changed, " | theo luat:", {i: tot[i] for i in tot}, " | apply=" + str(apply))
for r, c in report[:400]:
    print("  ", r, c)
