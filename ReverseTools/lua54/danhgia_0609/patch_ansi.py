"""patch_ansi.py - sua bo ma ANSI trong cac tool: chi 5 byte cp1252 KHONG DINH NGHIA (0x81 0x8D 0x8F 0x90 0x9D) la ky tu
   dieu khien U+0081..; con lai theo cp1252 that (0x80 = euro...). Dong thoi so_boot chi so dong loi chinh (bo traceback)."""
import os, ast, re
SP = os.path.dirname(os.path.abspath(__file__))
UNDEF = "(0x81, 0x8D, 0x8F, 0x90, 0x9D)"
ANSI_STR = ('def ansi_str(b):\n'
            '    # byte ANSI -> unicode dung nhu Windows (cp1252; 5 byte khong dinh nghia -> U+0081...)\n'
            '    return "".join(chr(x) if (x < 128 or x in ' + UNDEF + ') else bytes([x]).decode("cp1252") for x in b)\n')
ANSI_BYTES_BODY = ('    out = bytearray()\n'
                   '    for ch in s:\n'
                   '        o = ord(ch)\n'
                   '        if o < 128 or o in ' + UNDEF + ': out.append(o)\n'
                   '        else:\n'
                   '            try: out += ch.encode("cp1252")\n'
                   '            except UnicodeEncodeError: out += b"?"\n'
                   '    return bytes(out)\n')

def sub_func(src, name, new_body_or_func):
    """thay toan bo ham def name(...) (den dong trong dau tien / dong khong thut le) bang new"""
    m = re.search(r"^def " + name + r"\(.*?\):\n", src, re.M)
    if not m:
        return src, False
    i = m.start()
    j = m.end()
    # ket thuc ham: dong dau tien sau j khong bat dau bang khoang trang va khong rong
    lines = src[j:].split("\n")
    k = 0
    for k, ln in enumerate(lines):
        if ln.strip() and not ln.startswith((" ", "\t")):
            break
    else:
        k = len(lines)
    end = j + len("\n".join(lines[:k])) + (1 if k < len(lines) else 0)
    return src[:i] + new_body_or_func + src[end:], True

for f in ("r33_sapxep.py", "kiem_disan.py", "kiem_duongdan_cu.py", "r33_lui.py", "so_boot.py"):
    p = os.path.join(SP, f)
    s = open(p, encoding="utf-8", errors="surrogateescape").read()
    s, ok1 = sub_func(s, "ansi_str", ANSI_STR)
    if f == "r33_sapxep.py":
        s, ok2 = sub_func(s, "to_disk_bytes", "def to_disk_bytes(s):\n" + ANSI_BYTES_BODY)
    elif f == "kiem_disan.py":
        s, ok2 = sub_func(s, "ansi_bytes", "def ansi_bytes(s):\n" + ANSI_BYTES_BODY)
    else:
        ok2 = None
    ast.parse(s)
    open(p, "w", encoding="utf-8", errors="surrogateescape").write(s)
    print(f, "ansi_str:", ok1, "bytes:", ok2)

# so_boot: chi so dong loi chinh, cat o 'stack traceback', bo dong tiep dien (bat dau bang tab)
p = os.path.join(SP, "so_boot.py")
s = open(p, encoding="utf-8").read()
old = ('def load(p):\n'
       '    c = collections.Counter()\n'
       '    for ln in open(p, encoding="utf-8", errors="replace"):\n'
       '        ln = ln.rstrip("\\n")\n'
       '        if ln:\n'
       '            c[norm_line(ln)] += 1\n'
       '    return c\n')
new = ('def load(p):\n'
       '    c = collections.Counter()\n'
       '    for ln in open(p, encoding="utf-8", errors="replace"):\n'
       '        ln = ln.rstrip("\\n")\n'
       '        if not ln or ln.startswith("\\t"):\n'
       '            continue                       # bo dong traceback tiep dien (bi cat ngan o 300 ky tu -> gia khac)\n'
       '        i = ln.find("stack traceback")\n'
       '        if i >= 0:\n'
       '            ln = ln[:i]\n'
       '        c[norm_line(ln)] += 1\n'
       '    return c\n')
assert s.count(old) == 1, "so_boot load"
s = s.replace(old, new)
ast.parse(s)
open(p, "w", encoding="utf-8").write(s)
print("so_boot: chi dong loi chinh OK")
