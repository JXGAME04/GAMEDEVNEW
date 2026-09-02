# -*- coding: ascii -*-
"""vhtd_engine_patch1b.py - sua loi build CLIENT (Client Release|Win32) sau patch1:
  KNpc.h: khoi khai bao ham HS_* nam trong #ifdef _SERVER (852-897) -> chuyen ra sau khoi truong m_nHSUnravel (khong dieu kien);
  KNpc.cpp: HS_SpGet/HS_SpAdd/HS_SpCost nam trong #ifdef _SERVER nhung KNpcAttribModify::SpecialPointAdd (ca 2 ben) goi -> dua #endif len truoc HS_SpGet.
Doc/ghi latin-1. Idempotent. DUNG: python vhtd_engine_patch1b.py [--kiem]
"""
import io, os, sys
ROOT = r"D:\GAMEDEVNEW\Sources\Core\Src"
KIEM = "--kiem" in sys.argv
MARK = "[VHTD 02/09]"
def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s):
    if not KIEM: io.open(p, "w", encoding="latin-1", newline="").write(s)

# ---- KNpc.h
p = os.path.join(ROOT, "KNpc.h"); s = rd(p); orig = s
a = s.find("\r\n\tvoid\t\t\t\tHS_AutoCastTick();")
b_key = "HS_ResetVhtd();\t\t\t\t\t// " + MARK + " reset truong tinh lai thuoc tinh (khong reset so tang)"
b = s.find(b_key)
if a > 0 and b > a:
    b += len(b_key)
    blk = s[a:b]                       # bat dau bang \r\n, ket thuc khong co \r\n
    s = s[:a] + s[b:]                  # go khoi (dong CastAutoSkillAt giu lai comment HOASON cu)
    anchor = "\tint\t\t\t\t\tm_nHSUnravel;\t\t\t// " + MARK + " unravel_effect (chua co co che)\r\n"
    i = s.find(anchor)
    if i < 0: raise SystemExit("KNpc.h: khong thay m_nHSUnravel")
    i += len(anchor)
    s = s[:i] + "\t// " + MARK + " ham dung ca 2 ben (KNpcAttribModify goi HS_SpAdd) - khai bao ngoai #ifdef _SERVER" + blk + "\r\n" + s[i:]
    print("  [+] KNpc.h: chuyen khai bao HS_* ra ngoai khoi _SERVER")
elif "m_nHSUnravel;" in s and s.find("HS_AutoCastTick();") < s.find("m_CurrentIgnoreNegativeStateP;"):
    print("  [=] KNpc.h da chuyen")
else:
    raise SystemExit("KNpc.h: khong thay khoi HS_*")
if s != orig: wr(p, s); print("  => ghi KNpc.h%s" % (" KIEM" if KIEM else ""))

# ---- KNpc.cpp
p = os.path.join(ROOT, "KNpc.cpp"); s = rd(p); orig = s
old1 = "}\r\n\r\nint KNpc::HS_SpGet(int nKey)\r\n"
new1 = "}\r\n#endif\t// _SERVER (HS_AutoCastTick/HS_OnStateRemoved/HS_ResetBuffTime chi may chu)\r\n\r\nint KNpc::HS_SpGet(int nKey)\r\n"
old2 = "\treturn FALSE;\r\n}\r\n#endif\r\n\r\nvoid KNpc::HS_ResetVhtd()"
new2 = "\treturn FALSE;\r\n}\r\n\r\nvoid KNpc::HS_ResetVhtd()"
if old1 in s and old2 in s:
    s = s.replace(old1, new1, 1).replace(old2, new2, 1); print("  [+] KNpc.cpp: HS_SpGet/SpAdd/SpCost ra ngoai #ifdef _SERVER")
elif new1 in s: print("  [=] KNpc.cpp da chuyen")
else: raise SystemExit("KNpc.cpp: khong thay anchor #endif")
if s != orig: wr(p, s); print("  => ghi KNpc.cpp%s" % (" KIEM" if KIEM else ""))
print("XONG%s." % (" (KIEM)" if KIEM else ""))
