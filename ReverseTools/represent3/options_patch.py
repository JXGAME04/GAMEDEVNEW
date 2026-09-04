# -*- coding: utf-8 -*-
"""[REP3 03/09] Tuy chon chong dong (theo client VLTK 2.0 [Options] NpcTheSame / MissleIndex), doc tu [Client] config.ini:
  NpcTheSame=1          : nguoi choi KHAC mac cung mot bo (armor NpcTheSameArmor, mu NpcTheSameHelm, khong phi phong,
                          ngua NpcTheSameHorse neu >=0, vu khi NpcTheSameWeapon neu >=0) -> dung chung sprite/texture.
  MissleIndex=N (N>0)   : moi dan ky nang dung bo sprite/am thanh cua dong N trong missles (giu nguyen tham so bay).
Chi phia client (#ifndef _SERVER). Doc/ghi latin-1, giu kieu xuong dong."""
import io, re, os

def rd(p):
    with io.open(p, "r", encoding="latin-1", newline="") as f: return f.read()
def wr(p, s):
    with io.open(p, "w", encoding="latin-1", newline="") as f: f.write(s)
def nl(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"
def sub1(text, pattern, repl, name, count=1):
    n = len(re.findall(pattern, text, re.S))
    assert n == count, "%s: found %d (expected %d)" % (name, n, count)
    return re.sub(pattern, repl, text, count=count, flags=re.S)

MARK = "[REP3 03/09]"
ROOT = r"D:\GAMEDEVNEW_wt_rep3\Sources\Core\Src"

# ---------------- KProtocolProcess.cpp : NpcTheSame
p = os.path.join(ROOT, "KProtocolProcess.cpp")
s = rd(p); N = nl(s)
if "REP3_NpcTheSame" not in s:
    helper = N.join([
        "",
        "#ifndef _SERVER",
        "// %s [Client] NpcTheSame=1 : nguoi choi KHAC mac cung mot bo (theo tuy chon NpcTheSame cua client VLTK 2.0)" % MARK,
        "//   -> ca dam dong dung chung vai bo sprite/texture, giam nap tu pak va giam cache khi Tong Kim / cong thanh.",
        "//   NpcTheSameArmor (mac dinh 0), NpcTheSameHelm (mac dinh 0), NpcTheSameHorse (-1 = giu ngua rieng), NpcTheSameWeapon (-1 = giu).",
        "static int REP3_ClientIni(const char* szKey, int nDef)",
        "{",
        "\treturn (int)GetPrivateProfileIntA(\"Client\", szKey, nDef, \".\\\\config.ini\");",
        "}",
        "static void REP3_NpcTheSame(int nIdx)",
        "{",
        "\tstatic int s_nOn = -1, s_nArmor = 0, s_nHelm = 0, s_nHorse = -1, s_nWeapon = -1;",
        "\tif (s_nOn < 0)",
        "\t{",
        "\t\ts_nOn     = REP3_ClientIni(\"NpcTheSame\", 0);",
        "\t\ts_nArmor  = REP3_ClientIni(\"NpcTheSameArmor\", 0);",
        "\t\ts_nHelm   = REP3_ClientIni(\"NpcTheSameHelm\", 0);",
        "\t\ts_nHorse  = REP3_ClientIni(\"NpcTheSameHorse\", -1);",
        "\t\ts_nWeapon = REP3_ClientIni(\"NpcTheSameWeapon\", -1);",
        "\t}",
        "\tif (s_nOn <= 0 || nIdx <= 0 || nIdx >= MAX_NPC)",
        "\t\treturn;",
        "\tif (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)\t// khong dung cho chinh minh",
        "\t\treturn;",
        "\tif (s_nArmor >= 0)  Npc[nIdx].m_ArmorType  = s_nArmor;",
        "\tif (s_nHelm >= 0)   Npc[nIdx].m_HelmType   = s_nHelm;",
        "\tNpc[nIdx].m_MantleType = 0;",
        "\tNpc[nIdx].m_byMantleLevel = 0;",
        "\tif (s_nWeapon >= 0) Npc[nIdx].m_WeaponType = s_nWeapon;",
        "\tif (s_nHorse >= 0 && Npc[nIdx].m_HorseType >= 0) Npc[nIdx].m_HorseType = (char)s_nHorse;",
        "}",
        "#endif",
        "",
    ])
    s = sub1(s, r"(#include <iostream>[^\r\n]*" + N + ")", lambda m: m.group(1) + helper, "include anchor")
    # goi truoc 'Npc[nIdx].m_Kind = kind_player;' o SyncPlayer va SyncPlayerMin
    s = sub1(s, r"(\tNpc\[nIdx\]\.m_Kind\s+= kind_player;)",
             lambda m: "#ifndef _SERVER" + N + "\tREP3_NpcTheSame(nIdx);\t// " + MARK + N + "#endif" + N + m.group(1), "kind_player", count=2)
    wr(p, s); print("KProtocolProcess.cpp OK")
else:
    print("KProtocolProcess.cpp da va")

# ---------------- KMissle.cpp : MissleIndex
p = os.path.join(ROOT, "KMissle.cpp")
s = rd(p); N = nl(s)
if "REP3_MissleIndex" not in s:
    helper = N.join([
        "",
        "#ifndef _SERVER",
        "// %s [Client] MissleIndex=N : moi dan ky nang dung sprite/am thanh cua dong N (giu tham so bay cua dan goc)" % MARK,
        "//   theo tuy chon MissleOpen+MissleIndex cua client VLTK 2.0 - mot hieu ung cho moi chieu khi dong nguoi.",
        "static int REP3_MissleIndex()",
        "{",
        "\tstatic int s_nIdx = -1;",
        "\tif (s_nIdx < 0)",
        "\t\ts_nIdx = (int)GetPrivateProfileIntA(\"Client\", \"MissleIndex\", 0, \".\\\\config.ini\");",
        "\treturn s_nIdx;",
        "}",
        "#endif",
        "",
    ])
    s = sub1(s, r"(#include \"\.\./\.\./Engine/Src/KSG_StringProcess\.h\"[^\r\n]*" + N + ")", lambda m: m.group(1) + helper, "include anchor")
    old_loop_head = r"\tfor \(int i  = 0; i < MAX_MISSLE_STATUS; i\+\+\)" + N + r"\t\{" + N + r"\t\tsprintf\(AnimFileCol, \"AnimFile%d\", i \+ 1\);"
    new_loop_head = ("\tint nResRow = nRow;" + N +
        "#ifndef _SERVER" + N +
        "\t// %s MissleIndex: lay sprite/am thanh cua dong mau neu dong do co AnimFile1" % MARK + N +
        "\tif (REP3_MissleIndex() > 0 && REP3_MissleIndex() != nRow)" + N +
        "\t{" + N +
        "\t\tchar szProbe[64] = \"\";" + N +
        "\t\tpMisslesSetting->GetString(REP3_MissleIndex(), \"AnimFile1\", \"\", szProbe, 64, TRUE);" + N +
        "\t\tif (szProbe[0])" + N +
        "\t\t\tnResRow = REP3_MissleIndex();" + N +
        "\t}" + N +
        "#endif" + N +
        "\tfor (int i  = 0; i < MAX_MISSLE_STATUS; i++)" + N + "\t{" + N + "\t\tsprintf(AnimFileCol, \"AnimFile%d\", i + 1);")
    s = sub1(s, old_loop_head, lambda m: new_loop_head, "loop head")
    # chi thay 3 GetString trong vong lap NGAY SAU 'int nResRow = nRow;' (file co vong lap tuong tu o ham khac)
    i0 = s.index("\tint nResRow = nRow;")
    i1 = s.index(N + "}" + N, i0)          # het ham GetInfoFromTabFile (ca 2 vong lap doc AnimFile/SndFile)
    blk = s[i0:i1]
    for col in ("AnimFileCol", "SndFileCol", "AnimFileInfoCol"):
        n = blk.count("pMisslesSetting->GetString(nRow, " + col + ",")
        assert n >= 1, col
        blk = blk.replace("pMisslesSetting->GetString(nRow, " + col + ",", "pMisslesSetting->GetString(nResRow, " + col + ",")
        print("  ", col, "x", n)
    s = s[:i0] + blk + s[i1:]
    wr(p, s); print("KMissle.cpp OK")
else:
    print("KMissle.cpp da va")
print("DONE")
