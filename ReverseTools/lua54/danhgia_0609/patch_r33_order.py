import os, ast
SP = os.path.dirname(os.path.abspath(__file__))
p = os.path.join(SP, "r33_sapxep.py")
s = open(p, encoding="utf-8", errors="surrogateescape").read()
# cat 2 ham to_disk_bytes / ansi_str o cuoi, dua len truoc dong HAN = ...
i = s.index("def to_disk_bytes(s):")
j = s.index("if DO:", i)
block = s[i:j]
s = s[:i] + s[j:]
k = s.index("HAN = {ansi_str(k)")
s = s[:k] + block + "\n" + s[k:]
ast.parse(s)
open(p, "w", encoding="utf-8", errors="surrogateescape").write(s)
lines = s.split("\n")
for n, ln in enumerate(lines, 1):
    if ln.startswith("def ") or ln.startswith("HAN = ") or ln.startswith("if DO:"):
        print(n, ln[:60])
print("OK")
