# -*- coding: utf-8 -*-
"""C38 - THEM LOG CHAN DOAN (khong doan nua - DO THUC TE).
Chu bao van khong danh duoc quai trong PLD du da co C37. Thay vi doan tiep, dat
log tai DUNG 2 DIEM QUYET DINH:

 (1) SERVER - LuaSetFightState: co duoc goi khong, cho AI, gia tri gi, gui sync
     duoc khong. => biet mission co that su keo nguoi choi vao trang thai chien
     dau khong.
 (2) CLIENT - KPlayer::FindSelectNpc: khi bam chuot ma KHONG chon duoc muc tieu,
     in ra NPC gan nhat cung voi kind/camp/fight/relation cua CA HAI ben.
     => biet CHINH XAC dieu kien nao loai bo muc tieu.

Log di vao jx_auto_server.log / jx_auto.log (co san, bat bang g_AutoLogSet).
Sau khi doc duoc nguyen nhan se GO log nay ra.
"""
import io, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

CORE = r"D:\GAMEDEVNEW\Sources\Core\Src"
T = "\t"


def rw(p, fn):
    d = io.open(p, encoding="latin-1", newline="").read()
    d2 = fn(d)
    if d2 != d:
        io.open(p, "w", encoding="latin-1", newline="").write(d2)
        print("  OK", p.rsplit("\\", 1)[-1])
    else:
        print("  (khong doi)", p.rsplit("\\", 1)[-1])


# ---------- 1) SERVER: LuaSetFightState ----------
def f_sf(d):
    if "FIGHT-SET" in d:
        return d
    a = "\tNpc[Player[nPlayerIndex].m_nIndex].SetFightMode(Lua_ValueToNumber(L, 1) != 0);"
    assert d.count(a) == 1
    i = d.index(a) + len(a)
    nl = "\r\n" if d[i:i + 2] == "\r\n" else "\n"
    ins = nl.join([
        "",
        "#ifdef _SERVER",
        "\t// [3HD C38] LOG CHAN DOAN - go ra sau khi tim duoc nguyen nhan",
        "\tg_AutoLog(\"[FIGHT-SET] player=%d val=%d net=%d npc=%d camp=%d curcamp=%d msgrp=%d\",",
        "\t\tnPlayerIndex, (int)(Lua_ValueToNumber(L, 1) != 0), Player[nPlayerIndex].m_nNetConnectIdx,",
        "\t\tPlayer[nPlayerIndex].m_nIndex, (int)Npc[Player[nPlayerIndex].m_nIndex].m_Camp,",
        "\t\t(int)Npc[Player[nPlayerIndex].m_nIndex].m_CurrentCamp,",
        "\t\tNpc[Player[nPlayerIndex].m_nIndex].m_nMissionGroup);",
        "#endif",
    ])
    return d[:i] + ins + d[i:]
rw(CORE + r"\ScriptFuns.cpp", f_sf)


# ---------- 2) CLIENT: FindSelectNpc ----------
def f_kp(d):
    if "SEL-NPC-FAIL" in d:
        return d
    a = "\t\tm_nPeapleIdx = 0;\n\t}\n\t//\n\tif (Npc[nNpcIdx].m_HideState.nTime > 0)"
    nl = "\n"
    if a not in d:
        a = a.replace("\n", "\r\n")
        nl = "\r\n"
    assert a in d, "anchor FindSelectNpc"
    ins = nl.join([
        "\t\tm_nPeapleIdx = 0;",
        "#ifndef _SERVER",
        "\t\t// [3HD C38] LOG CHAN DOAN: bam ma KHONG chon duoc muc tieu -> in ly do.",
        "\t\t// Quet NPC gan nhat quanh diem bam va in kind/camp/fight/relation ca hai ben.",
        "\t\t{",
        "\t\t\tint nSelf = m_nIndex;",
        "\t\t\tint nBest = 0, nBestD = 0x7fffffff, sx, sy, ex, ey;",
        "\t\t\tNpc[nSelf].GetMpsPos(&sx, &sy);",
        "\t\t\tfor (int k = 1; k < MAX_NPC; k++)",
        "\t\t\t{",
        "\t\t\t\tif (k == nSelf || Npc[k].m_dwID == 0 || Npc[k].m_RegionIndex < 0)",
        "\t\t\t\t\tcontinue;",
        "\t\t\t\tif (Npc[k].m_Kind != kind_normal)",
        "\t\t\t\t\tcontinue;",
        "\t\t\t\tNpc[k].GetMpsPos(&ex, &ey);",
        "\t\t\t\tint dd = g_GetDistance(sx, sy, ex, ey);",
        "\t\t\t\tif (dd < nBestD) { nBestD = dd; nBest = k; }",
        "\t\t\t}",
        "\t\t\tif (nBest)",
        "\t\t\t\tAUTOLOG_EVERY(700, \"[SEL-NPC-FAIL] tim=%d self(kind=%d camp=%d cur=%d fight=%d msgrp=%d) npc=%d(%s kind=%d camp=%d cur=%d fight=%d msgrp=%d alive=%d) dist=%d rel=0x%X\",",
        "\t\t\t\t\tnRelation, (int)Npc[nSelf].m_Kind, (int)Npc[nSelf].m_Camp, (int)Npc[nSelf].m_CurrentCamp,",
        "\t\t\t\t\t(int)Npc[nSelf].m_FightMode, Npc[nSelf].m_nMissionGroup,",
        "\t\t\t\t\tnBest, Npc[nBest].Name, (int)Npc[nBest].m_Kind, (int)Npc[nBest].m_Camp,",
        "\t\t\t\t\t(int)Npc[nBest].m_CurrentCamp, (int)Npc[nBest].m_FightMode, Npc[nBest].m_nMissionGroup,",
        "\t\t\t\t\t(int)Npc[nBest].IsAlive(), nBestD, (unsigned int)NpcSet.GetRelation(nSelf, nBest));",
        "\t\t}",
        "#endif",
        "\t}",
        "\t//",
        "\tif (Npc[nNpcIdx].m_HideState.nTime > 0)",
    ])
    return d.replace(a, ins)
rw(CORE + r"\KPlayer.cpp", f_kp)
print("xong C38 - build lai CoreServer + CoreClient")
