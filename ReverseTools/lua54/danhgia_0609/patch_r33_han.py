import os, re
SP = os.path.dirname(os.path.abspath(__file__))
# ---- r33: HAN_GBK bang danh sach so (khong dung escape trong literal)
p = os.path.join(SP, "r33_sapxep.py")
s = open(p, encoding="utf-8", errors="surrogateescape").read()
i = s.index("HAN_GBK = {")
j = s.index("# tep le o goc", i)
new_han = ('HAN_GBK = {bytes([0xd6, 0xd0, 0xd4, 0xad, 0xb1, 0xb1, 0xc7, 0xf8]): "tinhnang/trapcu/trungnguyenbac",   # trung nguyen bac\n'
           '           bytes([0xce, 0xf7, 0xc4, 0xcf, 0xc4, 0xcf, 0xc7, 0xf8]): "tinhnang/trapcu/taynamnam",        # tay nam nam\n'
           '           bytes([0xc1, 0xbd, 0xba, 0xfe, 0xc7, 0xf8]): "tinhnang/trapcu/luongho"}                    # luong ho\n')
s = s[:i] + new_han + s[j:]
# ---- r33: don rac trong script/ (khong phai .lua/.txt) -> _luutru/0609/rac_trong_script
old = '    # ScriptError.log rai rac trong cay -> _luutru'
new = '''    # rac trong cay script (.bak, .dat, .log, back/...) -> _luutru/0609/rac_trong_script (giu .lua va .txt)
    nrac = 0
    for dp, dn, fn in os.walk(SCRIPT):
        for f in fn:
            fl = f.lower()
            if fl.endswith(".lua") or fl.endswith(".txt") or f.startswith("_"):
                continue
            src = os.path.join(dp, f); rel = os.path.relpath(src, ROOT)
            dst = os.path.join(LUU, "rac_trong_script", rel); os.makedirs(os.path.dirname(dst), exist_ok=True)
            shutil.move(src, dst); nrac += 1
    print("rac trong script/ da don: %d" % nrac)
    # ScriptError.log rai rac trong cay -> _luutru'''
assert s.count(old) == 1
s = s.replace(old, new)
open(p, "w", encoding="utf-8", errors="surrogateescape").write(s)
import ast; ast.parse(s); print("r33_sapxep.py OK")
# ---- 3 tool phu: decode mbcs -> ansi_str
for f, var in (("kiem_duongdan_cu.py", "P"), ("r33_lui.py", "P"), ("so_boot.py", "alias_p")):
    q = os.path.join(SP, f)
    t = open(q, encoding="utf-8").read()
    old = 'open(%s, "rb").read().decode("mbcs", errors="replace")' % var
    if old in t:
        t = t.replace(old, 'ansi_str(open(%s, "rb").read())' % var)
    open(q, "w", encoding="utf-8").write(t)
    ast.parse(t); print(f, "OK", "ansi_str(" in t)
