# -*- coding: utf-8 -*-
"""patch_level90.py [VTCN 06/09] - chu game: "cap ha xuong 90" -> moi nguong cap cua van tieu = 90.
Sua theo DONG (chi thay so ASCII '150'/'120' -> '90' trong dung dong co CheckTotalLevel / MIN_LEVEL /
cau bao), byte cao giu nguyen. Sao luu .truoc_cap90_0609. Chay: python patch_level90.py [--dry]
"""
import io, os, shutil, sys
sys.stdout.reconfigure(encoding="utf-8")
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
DRY = "--dry" in sys.argv
BAK = ".truoc_cap90_0609"

# (tep, [(chuoi neo ASCII co trong dong, so cu, so lan mong doi trong dong)])
VIEC = [
    (r"tinhnang\activitysys\config\129\item_addproperty.lua", [("PlayerFunLib:CheckTotalLevel(150,", "150", 2)]),
    (r"tinhnang\activitysys\config\129\npc_consigner.lua",   [("PlayerFunLib:CheckTotalLevel(150,", "150", 2)]),
    (r"tinhnang\activitysys\config\129\npc_lmbiaoqi.lua",    [("PlayerFunLib:CheckTotalLevel(150,", "150", 2)]),
    (r"tinhnang\activitysys\config\129\npc_lmbiaowu.lua",    [("PlayerFunLib:CheckTotalLevel(150,", "150", 2)]),
    (r"tinhnang\sukien\longmenbiaoju\dialog.lua",            [("PlayerFunLib:CheckTotalLevel(120,", "120", 1)]),
    (r"tinhnang\sukien\longmenbiaoju\lmbj_config.lua",       [("LongMenBiaoJu.MIN_LEVEL = 120", "120", 1)]),
    (r"tinhnang\sukien\longmenbiaoju\lang.lua",              [("DLG_TOO_YONG = ", "120", 1), ("MSG_CAN_NOT_PICK = ", "120", 1)]),
]

def hb(s):
    return sum(1 for c in s if ord(c) >= 0x80)

tong = 0
for rel, ds in VIEC:
    p = os.path.join(SRV, rel)
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    # tach theo "\n" (giu "\r" trong dong) -> chay dung ca tep LF, CRLF lan tron (lmbj_config.lua da tron sau P6)
    eol = "\n"
    lines = s.split(eol)
    s0 = s
    for neo, socu, mong in ds:
        hit = [i for i, l in enumerate(lines) if neo in l]
        if len(hit) == 0 and any(neo.replace(socu, "90") in l for l in lines):
            print("  %s: da doi tu truoc (%r)" % (rel, neo))
            continue
        if len(hit) != 1:
            sys.exit("LOI %s: neo %r khop %d dong" % (rel, neo, len(hit)))
        i = hit[0]
        n = lines[i].count(socu)
        if n != mong:
            sys.exit("LOI %s dong %d: '%s' xuat hien %d lan (mong %d): %r" % (rel, i + 1, socu, n, mong, lines[i][:90]))
        lines[i] = lines[i].replace(socu, "90") + "\t-- [VTCN 06/09] chu: cap ha xuong 90 (Linux %s)" % socu
        tong += n
    s1 = eol.join(lines)
    assert hb(s1) == hb(s0), rel
    print("%-60s OK (byte cao %d giu nguyen)" % (rel, hb(s0)))
    if not DRY:
        if not os.path.isfile(p + BAK):
            shutil.copyfile(p, p + BAK)
        io.open(p, "w", encoding="latin-1", newline="").write(s1)
print("da doi %d cho%s" % (tong, " (dry)" if DRY else ""))
