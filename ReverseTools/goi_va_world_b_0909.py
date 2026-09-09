# -*- coding: utf-8 -*-
"""goi_va_world_b_0909.py - [WORLD 09/09 b] Chia nho chi phi TICK LOGIC client theo NPC va theo pha.

Bang chung: [TICK] world 38-81 ms/tick luc dong (world = ca KSubWorldSet::MainLoop, xem CoreShell.cpp:24516),
trong khi [WORLD] quet_vung chi 0,4-7,1 ms khi 89-165 NPC/tick (~45 us/NPC). Tran TK ~900 NPC x 45 us = 40 ms
=> dich la CHI PHI MOI NPC trong KNpc::Activate (client). Tren client, KSubWorld::Activate chi con: xoa co +
vong quet vung + tu chay duong; MainLoop them m_cMusic.Play + CheckBalance (cac khoi NPC-khong-vung / thoi tiet /
nhiem vu deu #ifdef _SERVER).

DO (chi khi [Client] PaintLog=1, moi ma chen trong #ifndef _SERVER, ~5 QPC/NPC ~ 0,1 ms/tick o 900 NPC):
  - KRegion::Activate: thoi gian tung Npc[].Activate() -> tong, so lan, NPC nang nhat (idx).
  - KNpc::Activate: 4 pha ProcessState / NpcAI / ProcCommand / ProcStatus (NpcDoPha).
  - KSubWorldSet::MainLoop: m_cMusic.Play.
  - KSubWorld::Activate: moi tick co quet_vung >= 20 ms in mot dong [WORLD-TICK] rieng (bat dung cu giat).
  - WorldInDong: dong [WORLD b] trung binh moi 10 s.
Khong doi hanh vi. Doc trong tran TK roi moi quyet cat cho nao.
"""
import io
import sys

NL = "\r\n"
T = "\t"
ROOT = r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src"
F_NPC = ROOT + r"\KNpc.cpp"
F_REG = ROOT + r"\KRegion.cpp"
F_SW = ROOT + r"\KSubWorld.cpp"
F_SWS = ROOT + r"\KSubWorldSet.cpp"


def doc(p):
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    return s, s.count("\r\n"), s.count("\n") - s.count("\r\n"), sum(1 for c in s if ord(c) >= 0x80)


def ghi(p, s, crlf0, lf0, h0, ten):
    if crlf0 == 0:
        s = s.replace("\r\n", "\n"); ok = (s.count("\r\n") == 0)
    else:
        ok = (s.count("\n") - s.count("\r\n") == lf0)
    if not ok or sum(1 for c in s if ord(c) >= 0x80) != h0 or "�" in s:
        print("FAIL ma hoa " + ten); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("OK " + ten)


def rep(s, old, new, ten):
    n = s.count(old)
    if n != 1:
        print("FAIL neo %s: %d" % (ten, n)); sys.exit(1)
    return s.replace(old, new)


# ------------------------------------------------------------------ KSubWorld.cpp: bien + ham do + in tick
s, c0, l0, h0 = doc(F_SW)
if "[WORLD 09/09 b]" not in s:
    a = s.find("double WorldMs(const LARGE_INTEGER& a, const LARGE_INTEGER& b)")
    if a < 0:
        print("FAIL khong thay WorldMs"); sys.exit(1)
    b = s.find(NL + "}" + NL, a) + len(NL + "}" + NL)
    khoi = NL.join([
        "#ifndef _SERVER",
        "// [WORLD 09/09 b] Chia nho chi phi tick client theo NPC va theo pha (xem ReverseTools/goi_va_world_b_0909.py).",
        "// Chi ghi so khi PaintLog=1. g_* = tich luy ky 10 s (in o WorldInDong), t_* = tich luy MOT tick (in [WORLD-TICK] khi >= 20 ms).",
        "double   g_dWorldNhac = 0.0;",
        "double   g_dNpcPha[4] = { 0.0, 0.0, 0.0, 0.0 };" + T + "// ProcessState / NpcAI / ProcCommand / ProcStatus",
        "double   g_dNpcTong = 0.0, g_dNpcMax = 0.0;",
        "unsigned g_uNpcLan = 0;",
        "int      g_nNpcMaxIdx = 0;",
        "static double   t_dNpcTong = 0.0, t_dNpcMax = 0.0, t_dPha[4] = { 0.0, 0.0, 0.0, 0.0 };",
        "static unsigned t_uNpc = 0;",
        "static int      t_nNpcMaxIdx = 0;",
        "void NpcDoPha(int nPha, LARGE_INTEGER& t0)",
        "{",
        T + "LARGE_INTEGER t1; QueryPerformanceCounter(&t1);",
        T + "const double d = WorldMs(t0, t1);",
        T + "if (nPha >= 0 && nPha < 4) { g_dNpcPha[nPha] += d; t_dPha[nPha] += d; }",
        T + "t0 = t1;",
        "}",
        "void WorldNpcXong(int nIdx, const LARGE_INTEGER& a, const LARGE_INTEGER& b)",
        "{",
        T + "const double d = WorldMs(a, b);",
        T + "g_dNpcTong += d; g_uNpcLan++; t_dNpcTong += d; t_uNpc++;",
        T + "if (d > g_dNpcMax) { g_dNpcMax = d; g_nNpcMaxIdx = nIdx; }",
        T + "if (d > t_dNpcMax) { t_dNpcMax = d; t_nNpcMaxIdx = nIdx; }",
        "}",
        "void WorldTickXong(double dQuet)",
        "{",
        T + "extern int g_nCorePaintLog;",
        T + "if (g_nCorePaintLog > 0 && dQuet >= 20.0)",
        T + "{",
        T + T + "FILE* pLog = fopen(\"jx_paint.log\", \"a\");",
        T + T + "if (pLog)",
        T + T + "{",
        T + T + T + "const int i = t_nNpcMaxIdx;",
        T + T + T + "const bool bCo = (i > 0 && i < MAX_NPC);",
        T + T + T + "fprintf(pLog, \"[WORLD-TICK] t=%u quet_vung %.1f ms | npc %u tong %.1f ms (khac %.1f) | pha PS %.1f AI %.1f PC %.1f ST %.1f\"",
        T + T + T + T + "\" | nang nhat %.2f ms idx %d kind %d doing %d\\n\",",
        T + T + T + T + "(unsigned)GetTickCount(), dQuet, t_uNpc, t_dNpcTong, dQuet - t_dNpcTong, t_dPha[0], t_dPha[1], t_dPha[2], t_dPha[3],",
        T + T + T + T + "t_dNpcMax, i, bCo ? (int)Npc[i].m_Kind : -1, bCo ? (int)Npc[i].m_Doing : -1);" + T + "// m_ProcessAI la private -> khong in",
        T + T + T + "fclose(pLog);",
        T + T + "}",
        T + "}",
        T + "t_dNpcTong = t_dNpcMax = 0.0; t_uNpc = 0; t_nNpcMaxIdx = 0;",
        T + "t_dPha[0] = t_dPha[1] = t_dPha[2] = t_dPha[3] = 0.0;",
        "}",
        "#endif",
        "",
    ])
    s = s[:b] + khoi + s[b:]
    s = rep(s, T*2 + "if (dQuet > g_dWorldMaxTick) g_dWorldMaxTick = dQuet;",
            T*2 + "if (dQuet > g_dWorldMaxTick) g_dWorldMaxTick = dQuet;" + NL +
            T*2 + "WorldTickXong(dQuet);" + T + "// [WORLD 09/09 b] in rieng tick >= 20 ms, dat lai bo dem tick",
            "SW tick")
    ghi(F_SW, s, c0, l0, h0, "KSubWorld.cpp")
else:
    print("KSubWorld.cpp da co")

# ------------------------------------------------------------------ KNpc.cpp: 4 pha trong Activate
s, c0, l0, h0 = doc(F_NPC)
if "[WORLD 09/09 b]" not in s:
    old = NL.join([
        T + "m_LoopFrames++;",
        "",
        T + "//g_DebugLog(\"[DEATH] m_ProcessState: %d\", m_ProcessState);",
        T + "if (m_ProcessState)",
        T + "{",
        T + T + "if (ProcessState())",
        T + T + T + "return;",
        T + "}",
        "",
        T + "if (m_ProcessAI)",
        T + "{",
        T + T + "NpcAI.Activate(m_Index);",
        T + "}",
        T + "else",
        T + "{",
        T + T + "NpcAI.NotActivate(m_Index);",
        T + "}",
        "",
        T + "ProcCommand(m_ProcessAI);",
        T + "ProcStatus();",
    ])
    new = NL.join([
        T + "m_LoopFrames++;",
        "#ifndef _SERVER",
        T + "// [WORLD 09/09 b] do 4 pha cua mot NPC (client, chi khi PaintLog=1): ProcessState / NpcAI / ProcCommand / ProcStatus",
        T + "extern int g_nCorePaintLog; extern void NpcDoPha(int nPha, LARGE_INTEGER& t0);",
        T + "LARGE_INTEGER liNa0; const bool bNaDo = (g_nCorePaintLog > 0);",
        T + "if (bNaDo) QueryPerformanceCounter(&liNa0);",
        "#endif",
        "",
        T + "//g_DebugLog(\"[DEATH] m_ProcessState: %d\", m_ProcessState);",
        T + "if (m_ProcessState)",
        T + "{",
        T + T + "if (ProcessState())",
        T + T + "{",
        "#ifndef _SERVER",
        T + T + T + "if (bNaDo) NpcDoPha(0, liNa0);",
        "#endif",
        T + T + T + "return;",
        T + T + "}",
        T + "}",
        "#ifndef _SERVER",
        T + "if (bNaDo) NpcDoPha(0, liNa0);",
        "#endif",
        "",
        T + "if (m_ProcessAI)",
        T + "{",
        T + T + "NpcAI.Activate(m_Index);",
        T + "}",
        T + "else",
        T + "{",
        T + T + "NpcAI.NotActivate(m_Index);",
        T + "}",
        "#ifndef _SERVER",
        T + "if (bNaDo) NpcDoPha(1, liNa0);",
        "#endif",
        "",
        T + "ProcCommand(m_ProcessAI);",
        "#ifndef _SERVER",
        T + "if (bNaDo) NpcDoPha(2, liNa0);",
        "#endif",
        T + "ProcStatus();",
        "#ifndef _SERVER",
        T + "if (bNaDo) NpcDoPha(3, liNa0);",
        "#endif",
    ])
    s = rep(s, old, new, "NPC 4 pha")
    ghi(F_NPC, s, c0, l0, h0, "KNpc.cpp")
else:
    print("KNpc.cpp da co")

# ------------------------------------------------------------------ KRegion.cpp: thoi gian tung NPC
s, c0, l0, h0 = doc(F_REG)
if "[WORLD 09/09 b]" not in s:
    old = NL.join([
        T*3 + "// Always activate",
        T*3 + "Npc[nNpcIdx].Activate();",
        T*3 + "currentIndex++;",
    ])
    new = NL.join([
        T*3 + "// Always activate",
        "#ifndef _SERVER",
        T*3 + "{" + T + "// [WORLD 09/09 b] thoi gian tung NPC (client, chi khi PaintLog=1)",
        T*4 + "extern int g_nCorePaintLog; extern void WorldNpcXong(int nIdx, const LARGE_INTEGER& a, const LARGE_INTEGER& b);",
        T*4 + "if (g_nCorePaintLog > 0)",
        T*4 + "{",
        T*5 + "LARGE_INTEGER liN0, liN1; QueryPerformanceCounter(&liN0);",
        T*5 + "Npc[nNpcIdx].Activate();",
        T*5 + "QueryPerformanceCounter(&liN1);",
        T*5 + "WorldNpcXong(nNpcIdx, liN0, liN1);",
        T*4 + "}",
        T*4 + "else",
        T*5 + "Npc[nNpcIdx].Activate();",
        T*3 + "}",
        "#else",
        T*3 + "Npc[nNpcIdx].Activate();",
        "#endif",
        T*3 + "currentIndex++;",
    ])
    s = rep(s, old, new, "REG npc")
    ghi(F_REG, s, c0, l0, h0, "KRegion.cpp")
else:
    print("KRegion.cpp da co")

# ------------------------------------------------------------------ KSubWorldSet.cpp: nhac + dong [WORLD b]
s, c0, l0, h0 = doc(F_SWS)
if "[WORLD 09/09 b]" not in s:
    old = T + "this->m_cMusic.Play(SubWorld[0].m_SubWorldID, SubWorld[0].m_dwCurrentTime, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode);"
    new = NL.join([
        T + "{" + T + "// [WORLD 09/09 b] do nhac (client)",
        T + T + "extern double g_dWorldNhac; extern double WorldMs(const LARGE_INTEGER& a, const LARGE_INTEGER& b);",
        T + T + "LARGE_INTEGER a, b; const bool bDo = (g_nCorePaintLog > 0);",
        T + T + "if (bDo) QueryPerformanceCounter(&a);",
        T + T + "this->m_cMusic.Play(SubWorld[0].m_SubWorldID, SubWorld[0].m_dwCurrentTime, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode);",
        T + T + "if (bDo) { QueryPerformanceCounter(&b); g_dWorldNhac += WorldMs(a, b); }",
        T + "}",
    ])
    s = rep(s, old, new, "SWS nhac")
    old = T + "g_dWorldXoaCo = g_dWorldQuetVung = g_dWorldMaxTick = g_dWorldCanBang = 0.0;"
    new = NL.join([
        T + "{" + T + "// [WORLD 09/09 b] dong trung binh theo NPC / pha",
        T + T + "extern double g_dWorldNhac, g_dNpcPha[4], g_dNpcTong, g_dNpcMax; extern unsigned g_uNpcLan; extern int g_nNpcMaxIdx;",
        T + T + "FILE* p2 = fopen(\"jx_paint.log\", \"a\");",
        T + T + "if (p2)",
        T + T + "{",
        T + T + T + "fprintf(p2, \"[WORLD b] t=%u tick=%u | nhac %.2f ms/tick | npc %.1f/tick, tong %.2f ms/tick = %.1f us/NPC\"",
        T + T + T + T + "\" | pha PS %.2f AI %.2f PC %.2f ST %.2f ms/tick | nang nhat %.2f ms idx %d\\n\",",
        T + T + T + T + "dwNow, g_uWorldTick, g_dWorldNhac / g_uWorldTick, (double)g_uNpcLan / g_uWorldTick, g_dNpcTong / g_uWorldTick,",
        T + T + T + T + "g_uNpcLan ? g_dNpcTong * 1000.0 / g_uNpcLan : 0.0,",
        T + T + T + T + "g_dNpcPha[0] / g_uWorldTick, g_dNpcPha[1] / g_uWorldTick, g_dNpcPha[2] / g_uWorldTick, g_dNpcPha[3] / g_uWorldTick,",
        T + T + T + T + "g_dNpcMax, g_nNpcMaxIdx);",
        T + T + T + "fclose(p2);",
        T + T + "}",
        T + T + "g_dWorldNhac = 0.0; g_dNpcPha[0] = g_dNpcPha[1] = g_dNpcPha[2] = g_dNpcPha[3] = 0.0;",
        T + T + "g_dNpcTong = 0.0; g_dNpcMax = 0.0; g_uNpcLan = 0; g_nNpcMaxIdx = 0;",
        T + "}",
        old,
    ])
    s = rep(s, old, new, "SWS in")
    ghi(F_SWS, s, c0, l0, h0, "KSubWorldSet.cpp")
else:
    print("KSubWorldSet.cpp da co")
print("XONG WORLD b")
