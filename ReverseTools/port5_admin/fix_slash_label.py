# -*- coding: utf-8 -*-
r"""[24/08] Bo dau '/' khoi NHAN lua chon menu admin.
Engine tach nhan/ham bang strstr(pAnswer,"/") = dau '/' DAU TIEN
(ScriptFuns.cpp:716) nen nhan CO '/' se lam ten ham sai -> bam khong ra gi + loi script.
"""
import io, shutil, os

E = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIR = r"D:\GAMEDEVNEW\serverscript_jx2\port5_admin"

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)

# TCVN3: '/' la ASCII nen thao tac tren byte an toan
FIX = [
    (os.path.join(E, r"script\item\lenhbaiadmin.lua"),
     "23-24/08 (BC-BN-TV-TB)",
     "23-24.08 (BC-BN-TV-TB)"),
    (os.path.join(E, r"script\item\hoatdong_admin.lua"),
     "(task 200/2340-2342)",
     "(task 200 va 2340-2342)"),
]

for p, old, new in FIX:
    s = rd(p)
    c = s.count(old)
    if c == 0:
        print("da sua truoc do: %s" % p.split("\\")[-1]); continue
    assert c == 1, "%s: %d" % (p, c)
    s = s.replace(old, new, 1)
    wr(p, s)
    print("OK %s: %r -> %r" % (p.split("\\")[-1], old, new))
    shutil.copyfile(p, os.path.join(MIR, os.path.basename(p)))

# sua luon generator de chay lai khong tai phat
g = r"D:\GAMEDEVNEW\ReverseTools\port5_admin\hd_menu_tcvn3.py"
if os.path.isfile(g):
    s = io.open(g, "r", encoding="utf-8").read()
    n = 0
    if "23-24/08 (BC-BN-TV-TB)" in s:
        s = s.replace("23-24/08 (BC-BN-TV-TB)", "23-24.08 (BC-BN-TV-TB)"); n += 1
    if "(task 200/2340-2342)" in s:
        s = s.replace("(task 200/2340-2342)", "(task 200 va 2340-2342)"); n += 1
    if n:
        io.open(g, "w", encoding="utf-8").write(s)
    print("generator: sua %d cho" % n)

# sua nguon goc hd_hook_lenhbai.py (dong menu ban ASCII)
h = r"D:\GAMEDEVNEW\ReverseTools\port5_admin\hd_hook_lenhbai.py"
if os.path.isfile(h):
    s = io.open(h, "r", encoding="utf-8").read()
    if "23-24/08 (BC-BN-TV-TB)" in s:
        s = s.replace("23-24/08 (BC-BN-TV-TB)", "23-24.08 (BC-BN-TV-TB)")
        io.open(h, "w", encoding="utf-8").write(s)
        print("hook: da sua")
print("XONG")
