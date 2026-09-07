"""patch_unicode_ten.py - r33: bo qua (va bao) tep co ten Unicode that (> U+00FF) - engine ANSI khong mo duoc, bi danh vo nghia;
   so_boot: ap bi danh cu->moi TRUOC khi rut ve ten tep (xu ly ten co dau cach)."""
import os, ast
SP = os.path.dirname(os.path.abspath(__file__))
p = os.path.join(SP, "r33_sapxep.py")
s = open(p, encoding="utf-8", errors="surrogateescape").read()
old = ('        if not (f.lower().endswith(".lua") or f.lower().endswith(".txt")):\n'
       '            continue                                  # rac (.bak/.truoc_*/.dat/.zip) don rieng, khong bi danh\n')
assert s.count(old) == 1
new = old + ('        if any(ord(ch) > 255 for ch in rel):\n'
             '            TEN_UNICODE.append(rel); continue        # ten Unicode that: engine ANSI (cp1252) khong mo duoc -> giu nguyen, bao rieng\n')
s = s.replace(old, new)
old2 = "plan = []   # (rel_cu, rel_moi)\n"
assert s.count(old2) == 1
s = s.replace(old2, old2 + "TEN_UNICODE = []\n")
old3 = 'print("Ke hoach: %d tep doi cho; dich trung: %d; dich da ton tai: %d" % (len(plan), len(trung), len(tontai)))\n'
assert s.count(old3) == 1
s = s.replace(old3, old3 + 'if TEN_UNICODE:\n    print("CANH BAO: %d tep ten Unicode that (engine ANSI khong mo duoc, khong doi cho, can doi ten): %s" % (len(TEN_UNICODE), "; ".join(TEN_UNICODE[:6])))\n')
# trong DO: chuyen tep ten Unicode vao _luutru/0609/ten_unicode (engine ANSI _findfirst doc thanh '?' -> khong mo duoc, chi gay loi boot)
old4 = "    # rac trong cay script (.bak, .dat, .log, back/...) -> _luutru/0609/rac_trong_script (giu .lua va .txt)\n"
assert s.count(old4) == 1
s = s.replace(old4, ('    for rel in TEN_UNICODE:\n'
                     '        src = os.path.join(SCRIPT, rel.replace("/", os.sep)); dst = os.path.join(LUU, "ten_unicode", rel.replace("/", os.sep))\n'
                     '        if os.path.exists(src):\n'
                     '            os.makedirs(os.path.dirname(dst), exist_ok=True); shutil.move(src, dst)\n'
                     '    if TEN_UNICODE: print("tep ten Unicode da chuyen vao _luutru/0609/ten_unicode: %d" % len(TEN_UNICODE))\n') + old4)
ast.parse(s)
open(p, "w", encoding="utf-8", errors="surrogateescape").write(s)
print("r33 unicode-ten OK")

p = os.path.join(SP, "so_boot.py")
s = open(p, encoding="utf-8").read()
old = ('def norm_line(ln):\n'
       '    low = ln.lower()\n'
       '    low = RX_PATH.sub(lambda m: m.group(1), low)\n'
       '    return low\n')
assert s.count(old) == 1
new = ('def norm_line(ln):\n'
       '    low = ln.lower()\n'
       '    for cu, moi in alias.items():        # cot rel (duong dan tuong doi, co the co dau cach) -> ten moi\n'
       '        low = low.replace(cu, moi)\n'
       '    low = RX_PATH.sub(lambda m: m.group(1), low)\n'
       '    return low\n')
s = s.replace(old, new)
ast.parse(s)
open(p, "w", encoding="utf-8").write(s)
print("so_boot alias-first OK")
