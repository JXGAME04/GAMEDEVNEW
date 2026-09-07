"""so_boot.py - so tap loi _ALERT cua boot_gia TRUOC (duong dan cu) va SAU khi doi cho (duong dan moi).
   Quy chuan: moi duong dan .lua -> chi con TEN TEP (Lua cat ngan chunkname khac nhau khi goc dai/ngan), bo dong traceback.
   python so_boot.py <truoc.txt> <sau.txt> <script/_duongdan_cu.txt>   (exit 0 = giong het, 2 = khac)"""
import sys, collections, re
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
truoc, sau, alias_p = sys.argv[1:4]
BS = chr(92)

def ansi_str(b):
    # byte ANSI -> unicode dung nhu Windows (cp1252; 5 byte khong dinh nghia -> U+0081...)
    return "".join(chr(x) if (x < 128 or x in (0x81, 0x8D, 0x8F, 0x90, 0x9D)) else bytes([x]).decode("cp1252") for x in b)

alias = {}
for ln in ansi_str(open(alias_p, "rb").read()).split("\n"):
    ln = ln.strip()
    if ln.startswith("--@") and "=" in ln:
        cu, moi = ln[3:].split("=", 1)
        alias[cu.strip().lower().replace("/", BS)] = moi.strip().lower().replace("/", BS)

# moi duong dan (co the bat dau bang "..." do Lua cat) -> chi giu ten tep .lua
RX_PATH = re.compile("[^\t :|\"']*" + re.escape(BS) + "([^\t :|\"'" + re.escape(BS) + "]+" + re.escape(".lua") + ")")

def norm_line(ln):
    low = ln.lower()
    for cu, moi in alias.items():        # cot rel (duong dan tuong doi, co the co dau cach) -> ten moi
        low = low.replace(cu, moi)
    low = RX_PATH.sub(lambda m: m.group(1), low)
    return low

def load(p):
    c = collections.Counter()
    for ln in open(p, encoding="utf-8", errors="replace"):
        ln = ln.rstrip("\n")
        if not ln or ln.startswith("\t"):
            continue                       # bo dong traceback tiep dien
        i = ln.find("stack traceback")
        if i >= 0:
            ln = ln[:i]
        c[norm_line(ln)] += 1
    return c

A, B = load(truoc), load(sau)
chi_truoc = A - B
chi_sau = B - A
print("dong loi truoc: %d, sau: %d; chi co TRUOC: %d; chi co SAU: %d  (%s)" % (
    sum(A.values()), sum(B.values()), sum(chi_truoc.values()), sum(chi_sau.values()),
    "GIONG HET" if (not chi_truoc and not chi_sau) else "KHAC - xem duoi"))
for k in list(chi_truoc)[:15]:
    print("  TRUOC-only:", k[:220])
for k in list(chi_sau)[:15]:
    print("  SAU-only:  ", k[:220])
sys.exit(0 if (not chi_truoc and not chi_sau) else 2)
