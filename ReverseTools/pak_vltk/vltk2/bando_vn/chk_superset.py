# -*- coding: utf-8 -*-
"""Kiem Game.exe MOI cua toi co chua moi chuoi ma Game.exe.moi (phien khac, 22:10) them so voi Game.exe dang chay.
Dung: python chk_superset.py <Game.exe dang chay> <Game.exe.moi phien khac> <Game.exe cua toi>"""
import re, sys, hashlib
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
run, theirs, mine = [open(p, "rb").read() for p in sys.argv[1:4]]
def strs(d):
    s = set(m.group(0) for m in re.finditer(rb"[\x20-\x7e]{6,}", d))
    return set(x for x in s if not re.match(rb"^[0-9<>=;:!#$%&'()*+,\-./?@\[\]^_`{|}~ ]+$", x))  # bo bang nhay/rac
a, b, c = strs(run), strs(theirs), strs(mine)
new_theirs = b - a
missing = [x for x in new_theirs if x not in c and b".pdb" not in x.lower()]
print("chuoi MOI cua Game.exe.moi (22:10) so voi Game.exe dang chay: %d" % len(new_theirs))
print("trong do THIEU o Game.exe cua toi (bo qua .pdb): %d" % len(missing))
for x in sorted(missing, key=len, reverse=True)[:15]:
    print("   -", x.decode("latin-1")[:100])
print("chuoi MOI cua Game.exe cua toi so voi .moi 22:10: %d (BANDO20)" % len(c - b))
for p, d in zip(("dang chay", ".moi 22:10", "cua toi"), (run, theirs, mine)):
    print("  %-10s %8d B  md5 %s" % (p, len(d), hashlib.md5(d).hexdigest()[:8]))
print("KET LUAN:", "SUPERSET OK - thay .moi duoc" if not missing else "!! CHUA superset - KHONG thay .moi")
