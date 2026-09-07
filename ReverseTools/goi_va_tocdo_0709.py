#!/usr/bin/env python
# -*- coding: ascii -*-
"""
[TOCDO 07/09] Toc do danh ngoai/noi nhu Linux (khong them kenh Am/Duong):
  H1 KNpcSet.h   : them nCastFrame + GetPlayerCastFrame()
  H2 KNpcSet.cpp : doc "CastFrame" tu BaseValue.ini (mac dinh 20 = gia tri cu)
  H3 KNpc.cpp    : nguoi choi gan m_CastFrame tu ini (2 cho: 1008, 6326) - truoc day ket 20
  H4 KNpc.cpp    : bo lam chan khung trong DoSkill (2 cho) - Linux khong lam chan
  H5 KNpc.cpp    : DoAttack / DoBlurAttack / DoJumpAttack chan mau so <= 0 va khung <= 0 nhu Linux (3 cho)
Chay: python goi_va_tocdo_0709.py <goc cay nguon>   (doc/ghi latin-1, khong dung Edit tool vi tep TCVN3/GBK)
"""
import sys, os, re, io

root = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW"
SRC = os.path.join(root, "Sources", "Core", "Src")

def rd(p):
    with io.open(p, "r", encoding="latin-1", newline="") as f:
        return f.read()

def wr(p, s):
    with io.open(p, "w", encoding="latin-1", newline="") as f:
        f.write(s)

def hb(s):
    return sum(1 for ch in s if ord(ch) > 127)

def apply(path, hunks):
    s = rd(path); before = hb(s)
    for name, old, new, count in hunks:
        n = s.count(old)
        if n != count:
            raise SystemExit("%s %s: mong %d, thay %d lan neo" % (os.path.basename(path), name, count, n))
        s = s.replace(old, new)
    if hb(s) != before + sum(hb(new) - hb(old) for _, old, new, c in hunks for _ in range(c)):
        raise SystemExit("%s: so byte cao lech" % path)
    wr(path, s)
    print("OK", os.path.basename(path), ", ".join(h[0] for h in hunks))

# ---------- H1 KNpcSet.h ----------
apply(os.path.join(SRC, "KNpcSet.h"), [
    ("H1a", "\tint\t\tnAttackFrame;\r\n",
            "\tint\t\tnAttackFrame;\r\n"
            "\tint\t\tnCastFrame;\t\t// [TOCDO 07/09] CastFrame tu BaseValue.ini nhu Linux\r\n", 1),
    ("H1b", "\tint\t\t\t\tGetPlayerAttackFrame() { return m_cPlayerBaseValue.nAttackFrame; };\r\n",
            "\tint\t\t\t\tGetPlayerAttackFrame() { return m_cPlayerBaseValue.nAttackFrame; };\r\n"
            "\tint\t\t\t\tGetPlayerCastFrame() { return m_cPlayerBaseValue.nCastFrame; };\t// [TOCDO 07/09]\r\n", 1),
])

# ---------- H2 KNpcSet.cpp ----------
apply(os.path.join(SRC, "KNpcSet.cpp"), [
    ("H2", "\tFile.GetInteger(\"Common\", \"AttackFrame\", 20, &m_cPlayerBaseValue.nAttackFrame);\r\n",
           "\tFile.GetInteger(\"Common\", \"AttackFrame\", 20, &m_cPlayerBaseValue.nAttackFrame);\r\n"
           "\tFile.GetInteger(\"Common\", \"CastFrame\", 20, &m_cPlayerBaseValue.nCastFrame);\t// [TOCDO 07/09] nhu Linux (ini 18); truoc day nguoi choi ket 20\r\n", 1),
])

# ---------- H3/H4/H5 KNpc.cpp ----------
p = os.path.join(SRC, "KNpc.cpp")
s = rd(p); before = hb(s)

old3 = "\t\tm_AttackFrame = NpcSet.GetPlayerAttackFrame();\r\n"
new3 = old3 + "\t\tm_CastFrame = NpcSet.GetPlayerCastFrame();\t// [TOCDO 07/09] nhu Linux: nguoi choi nap CastFrame tu ini (truoc day ket 20 cua ham dung)\r\n"
if s.count(old3) != 2: raise SystemExit("H3: neo %d lan" % s.count(old3))
s = s.replace(old3, new3)

old4 = "\t\t\tm_Frames.nTotalFrame = nTotalFrame - nTotalFrame % 2;\r\n"
new4 = "\t\t\tm_Frames.nTotalFrame = nTotalFrame;\t// [TOCDO 07/09] bo lam chan khung, nhu Linux\r\n"
if s.count(old4) != 2: raise SystemExit("H4: neo %d lan" % s.count(old4))
s = s.replace(old4, new4)

pat5 = re.compile(r"^(\t+)m_Frames\.nTotalFrame = m_AttackFrame \* 100 / \(100 \+ m_CurrentAttackSpeed\);\r\n", re.M)
if len(pat5.findall(s)) != 3: raise SystemExit("H5: neo %d lan" % len(pat5.findall(s)))
def rep5(m):
    ind = m.group(1)
    return (ind + "{\r\n"
            + ind + "\t// [TOCDO 07/09] nhu Linux: chan mau so <= 0 va khung <= 0 (DoAttack/DoBlurAttack/DoJumpAttack)\r\n"
            + ind + "\tint nTocDoChia = 100 + m_CurrentAttackSpeed;\r\n"
            + ind + "\tif (nTocDoChia <= 0) nTocDoChia = 1;\r\n"
            + ind + "\tm_Frames.nTotalFrame = m_AttackFrame * 100 / nTocDoChia;\r\n"
            + ind + "\tif (m_Frames.nTotalFrame <= 0) m_Frames.nTotalFrame = 1;\r\n"
            + ind + "}\r\n")
s = pat5.sub(rep5, s)
if hb(s) != before: raise SystemExit("KNpc.cpp: so byte cao lech")
wr(p, s)
print("OK KNpc.cpp H3 (2), H4 (2), H5 (3)")
