# -*- coding: utf-8 -*-
"""goi_va_world1_do_tick_the_gioi_0809.py - [WORLD 08/09 a] DO TRUOC: chu bao "fps 144 nhung dong giam manh".
Da xac dinh bang [LOGIC]/[TICK] co san trong jx_paint.log: khung nang KHONG phai do ve ma do TICK LOGIC,
va trong tick thi 100 % nam o 'world' = KSubWorldSet::MainLoop -> SubWorld[0].Activate() (38-81 ms mot tick, 18 tick/giay).
Ban nay chia nho tiep de biet cat o dau: xoa co NPC / quet vung / trong vung (theo so NPC) / can bang cache NPC.
In dong [WORLD] vao jx_paint.log moi 10 giay khi [Client] PaintLog=1 (dung g_nCorePaintLog co san, khong them khoa cau hinh).
Tep: Core/Src/KSubWorld.cpp, KRegion.cpp, KSubWorldSet.cpp (latin-1 CRLF, KHONG dung Edit tool vi nguon TCVN3/GBK)."""
import io, re, sys
SRC = r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src"
def load(ten):
    p = SRC + "\\" + ten
    return p, io.open(p, "r", encoding="latin-1", newline="").read()
def save(p, s, h0):
    if sum(1 for ch in s if ord(ch) >= 0x80) != h0: print("FAIL byte cao", p); sys.exit(1)
    if re.search(r"[^\r]\n", s): print("FAIL LF", p); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
def rep(s, old, new, n=1):
    c = s.count(old)
    if c != n: print("FAIL neo %r: thay %d, can %d" % (old[:80], c, n)); sys.exit(1)
    return s.replace(old, new)
def hi(s): return sum(1 for ch in s if ord(ch) >= 0x80)

# ---------------- KSubWorld.cpp: bien do + dong ho quanh xoa co va vong quet vung
p, s = load("KSubWorld.cpp"); h0 = hi(s)
if "g_dWorldQuetVung" not in s:
    s = rep(s, "void KSubWorld::Activate()\r\n{\r\n",
        "#ifndef _SERVER\r\n"
        "// [WORLD 08/09 a] Do tick the gioi phia CLIENT. [TICK]/[LOGIC] cho biet khung nang nam o 'world'\r\n"
        "// (KSubWorldSet::MainLoop) chu khong phai o ve; day chia nho tiep de biet cat o dau.\r\n"
        "double   g_dWorldXoaCo = 0.0, g_dWorldQuetVung = 0.0, g_dWorldMaxTick = 0.0;\r\n"
        "unsigned g_uWorldTick = 0, g_uWorldVung = 0, g_uWorldNpc = 0, g_uWorldVungTong = 0;\r\n"
        "static LARGE_INTEGER s_liWorldFreq = {0};\r\n"
        "double WorldMs(const LARGE_INTEGER& a, const LARGE_INTEGER& b)\r\n"
        "{\r\n\tif (!s_liWorldFreq.QuadPart) QueryPerformanceFrequency(&s_liWorldFreq);\r\n"
        "\treturn s_liWorldFreq.QuadPart ? (double)(b.QuadPart - a.QuadPart) * 1000.0 / (double)s_liWorldFreq.QuadPart : 0.0;\r\n}\r\n"
        "extern int g_nCorePaintLog;\r\n"
        "#endif\r\n"
        "void KSubWorld::Activate()\r\n{\r\n")
    s = rep(s, "\tg_ScenePlace.SetCurrentTime(m_dwCurrentTime);\r\n\tNpcSet.ClearActivateFlagOfAllNpc();\r\n",
        "\tg_ScenePlace.SetCurrentTime(m_dwCurrentTime);\r\n"
        "\tLARGE_INTEGER liW0, liW1, liW2;\t// [WORLD 08/09 a]\r\n"
        "\tconst bool bWorldDo = (g_nCorePaintLog > 0);\r\n"
        "\tif (bWorldDo) QueryPerformanceCounter(&liW0);\r\n"
        "\tNpcSet.ClearActivateFlagOfAllNpc();\r\n"
        "\tif (bWorldDo) { QueryPerformanceCounter(&liW1); g_dWorldXoaCo += WorldMs(liW0, liW1); g_uWorldTick++; g_uWorldVungTong += (unsigned)m_nTotalRegion; }\r\n")
    old = ("#else\r\n"
           "\tfor (int i = 0; i < m_nTotalRegion; i++)\r\n"
           "\t{\r\n"
           "\t\tif (m_Region[i].IsActive())\r\n"
           "\t\t{\r\n"
           "//\t\t\tg_DebugLog(\"[Region]%d Activating\", i);\r\n"
           "\t\t\tm_Region[i].Activate();\r\n"
           "\t\t\tnActiveRegionCount++;\r\n"
           "\t\t}\r\n"
           "\t}\r\n"
           "#endif\r\n")
    new = ("#else\r\n"
           "\tfor (int i = 0; i < m_nTotalRegion; i++)\r\n"
           "\t{\r\n"
           "\t\tif (m_Region[i].IsActive())\r\n"
           "\t\t{\r\n"
           "//\t\t\tg_DebugLog(\"[Region]%d Activating\", i);\r\n"
           "\t\t\tm_Region[i].Activate();\r\n"
           "\t\t\tnActiveRegionCount++;\r\n"
           "\t\t\tif (bWorldDo) g_uWorldVung++;\t// [WORLD 08/09 a]\r\n"
           "\t\t}\r\n"
           "\t}\r\n"
           "\tif (bWorldDo)\r\n"
           "\t{\t// [WORLD 08/09 a] thoi gian ca vong quet vung (gom moi thu chay ben trong vung: NPC, dan, dong bo)\r\n"
           "\t\tQueryPerformanceCounter(&liW2);\r\n"
           "\t\tconst double dQuet = WorldMs(liW1, liW2);\r\n"
           "\t\tg_dWorldQuetVung += dQuet;\r\n"
           "\t\tif (dQuet > g_dWorldMaxTick) g_dWorldMaxTick = dQuet;\r\n"
           "\t}\r\n"
           "#endif\r\n")
    s = rep(s, old, new)
save(p, s, h0); print("OK KSubWorld.cpp")

# ---------------- KRegion.cpp: dem NPC phia client
p, s = load("KRegion.cpp"); h0 = hi(s)
if "g_uWorldNpc" not in s:
    s = rep(s, "#ifdef _SERVER\r\n\textern int nActiveNpcCount;\t// [PerfLog 24/08] khoi luong tick (KSubWorldSet.cpp)\r\n\tnActiveNpcCount += npcCount;\r\n#endif\r\n",
        "#ifdef _SERVER\r\n\textern int nActiveNpcCount;\t// [PerfLog 24/08] khoi luong tick (KSubWorldSet.cpp)\r\n\tnActiveNpcCount += npcCount;\r\n"
        "#else\r\n\t{\t// [WORLD 08/09 a] dem NPC chay trong tick nay (client)\r\n"
        "\t\textern unsigned g_uWorldNpc; extern int g_nCorePaintLog;\r\n"
        "\t\tif (g_nCorePaintLog > 0) g_uWorldNpc += (unsigned)npcCount;\r\n\t}\r\n#endif\r\n")
save(p, s, h0); print("OK KRegion.cpp")

# ---------------- KSubWorldSet.cpp: do CheckBalance + in dong [WORLD] moi 10 s
p, s = load("KSubWorldSet.cpp"); h0 = hi(s)
if "[WORLD 08/09 a]" not in s:
    old = ("\t\tfor (int i = 0; i < MAX_SUBWORLD; i++)\r\n"
           "\t\t{\r\n"
           "\t\t\tif (SubWorld[i].m_SubWorldID >= 0)\r\n"
           "\t\t\t{\r\n"
           "\t\t\t\tSubWorld[i].Activate();\r\n"
           "#ifndef _SERVER\r\n"
           "\t\t\t\tNpcSet.CheckBalance();\r\n"
           "#endif\r\n"
           "\t\t\t}\r\n"
           "\t\t}\r\n")
    new = ("\t\tfor (int i = 0; i < MAX_SUBWORLD; i++)\r\n"
           "\t\t{\r\n"
           "\t\t\tif (SubWorld[i].m_SubWorldID >= 0)\r\n"
           "\t\t\t{\r\n"
           "\t\t\t\tSubWorld[i].Activate();\r\n"
           "#ifndef _SERVER\r\n"
           "\t\t\t\t{\t// [WORLD 08/09 a] do rieng can bang cache NPC\r\n"
           "\t\t\t\t\textern double WorldMs(const LARGE_INTEGER& a, const LARGE_INTEGER& b);\r\n"
           "\t\t\t\t\textern double g_dWorldCanBang; extern int g_nCorePaintLog;\r\n"
           "\t\t\t\t\tLARGE_INTEGER a, b;\r\n"
           "\t\t\t\t\tconst bool bDo = (g_nCorePaintLog > 0);\r\n"
           "\t\t\t\t\tif (bDo) QueryPerformanceCounter(&a);\r\n"
           "\t\t\t\t\tNpcSet.CheckBalance();\r\n"
           "\t\t\t\t\tif (bDo) { QueryPerformanceCounter(&b); g_dWorldCanBang += WorldMs(a, b); }\r\n"
           "\t\t\t\t}\r\n"
           "#endif\r\n"
           "\t\t\t}\r\n"
           "\t\t}\r\n"
           "#ifndef _SERVER\r\n"
           "\tWorldInDong();\t// [WORLD 08/09 a] in [WORLD] moi 10 giay\r\n"
           "#endif\r\n")
    s = rep(s, old, new)
    s = rep(s, "void KSubWorldSet::MainLoop()\r\n{\r\n",
        "#ifndef _SERVER\r\n"
        "// [WORLD 08/09 a] gom so do tu KSubWorld.cpp/KRegion.cpp roi in moi 10 giay vao jx_paint.log\r\n"
        "double g_dWorldCanBang = 0.0;\r\n"
        "extern double g_dWorldXoaCo, g_dWorldQuetVung, g_dWorldMaxTick;\r\n"
        "extern unsigned g_uWorldTick, g_uWorldVung, g_uWorldNpc, g_uWorldVungTong;\r\n"
        "extern int g_nCorePaintLog;\r\n"
        "static void WorldInDong()\r\n"
        "{\r\n"
        "\tif (g_nCorePaintLog <= 0 || g_uWorldTick == 0)\r\n\t\treturn;\r\n"
        "\tstatic DWORD s_dwLan = 0;\r\n"
        "\tconst DWORD dwNow = timeGetTime();\r\n"
        "\tif (s_dwLan == 0) { s_dwLan = dwNow; return; }\r\n"
        "\tif (dwNow - s_dwLan < 10000)\r\n\t\treturn;\r\n"
        "\ts_dwLan = dwNow;\r\n"
        "\tFILE* pLog = fopen(\"jx_paint.log\", \"a\");\r\n"
        "\tif (pLog)\r\n\t{\r\n"
        "\t\tfprintf(pLog, \"[WORLD] t=%u tick=%u | xoa_co %.2f ms | quet_vung %.2f ms (max %.1f) | can_bang %.2f ms | vung %.1f/%.0f dang chay | npc %.0f/tick\\n\",\r\n"
        "\t\t\tdwNow, g_uWorldTick,\r\n"
        "\t\t\tg_dWorldXoaCo / g_uWorldTick, g_dWorldQuetVung / g_uWorldTick, g_dWorldMaxTick, g_dWorldCanBang / g_uWorldTick,\r\n"
        "\t\t\t(double)g_uWorldVung / g_uWorldTick, (double)g_uWorldVungTong / g_uWorldTick, (double)g_uWorldNpc / g_uWorldTick);\r\n"
        "\t\tfclose(pLog);\r\n\t}\r\n"
        "\tg_dWorldXoaCo = g_dWorldQuetVung = g_dWorldMaxTick = g_dWorldCanBang = 0.0;\r\n"
        "\tg_uWorldTick = g_uWorldVung = g_uWorldNpc = g_uWorldVungTong = 0;\r\n"
        "}\r\n"
        "#endif\r\n"
        "void KSubWorldSet::MainLoop()\r\n{\r\n")
save(p, s, h0); print("OK KSubWorldSet.cpp")
print("XONG WORLD a")
