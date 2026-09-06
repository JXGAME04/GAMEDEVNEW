"""so_boot.py - so tap loi _ALERT cua boot_gia TRUOC (duong dan cu) va SAU khi doi cho (duong dan moi), ap bi danh de quy ve mot ten.
   python so_boot.py <truoc.txt> <sau.txt> <script\_duongdan_cu.txt>"""
import sys, collections
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
truoc, sau, alias_p = sys.argv[1:4]
BS = chr(92)
alias = {}
for ln in open(alias_p, "rb").read().decode("mbcs", errors="replace").split("\n"):
    ln = ln.strip()
    if ln.startswith("--@"):
        cu, moi = ln[3:].split("=", 1)
        alias[cu.strip().lower().replace("/", BS)] = moi.strip().lower().replace("/", BS)
def norm_line(ln):
    # dong: rel \t n \t msg ; thay moi duong dan cu -> moi (chu thuong), bo phan duong dan tuyet doi truoc "script\"
    low = ln.lower()
    for cu, moi in alias.items():
        low = low.replace(cu, moi)
    # bo tien to tuyet doi (E:\...\bin\server\) trong thong diep
    i = low.find(BS + "bin" + BS + "server" + BS)
    while i >= 0:
        j = low.rfind("...", 0, i)
        low = low[:i] + low[i + len(BS + "bin" + BS + "server" + BS):]
        i = low.find(BS + "bin" + BS + "server" + BS)
    return low
def load(p):
    c = collections.Counter()
    for ln in open(p, encoding="utf-8", errors="replace"):
        ln = ln.rstrip("\n")
        if ln:
            c[norm_line(ln)] += 1
    return c
A, B = load(truoc), load(sau)
chi_truoc = A - B; chi_sau = B - A
print("dong loi truoc: %d, sau: %d; chi co TRUOC: %d; chi co SAU: %d" % (sum(A.values()), sum(B.values()), sum(chi_truoc.values()), sum(chi_sau.values())))
for k in list(chi_truoc)[:15]: print("  TRUOC-only:", k[:220])
for k in list(chi_sau)[:15]: print("  SAU-only:  ", k[:220])
