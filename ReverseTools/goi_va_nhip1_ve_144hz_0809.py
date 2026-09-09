# -*- coding: utf-8 -*-
"""goi_va_nhip1_ve_144hz_0809.py - [NHIP 08/09] Viec #3 KEHOACH_D3D11_TIEP_0809.md: ve 120-144 Hz voi noi suy san co (PaintFps/PaintInterp 15/08).
 1. S3Client: tran PaintFps 60 -> 240; PaintFps=-1 = tu theo tan so man hinh (59 -> 60); PaintVsync=1 = ve moi vong bom, Represent3 Present(1)
    (vblank dan nhip, khong jitter dong ho); luoi vong bom 1 ms (g_SetLoopInterval) khi > 60 fps hoac vsync; nhip > 60 dung moc phan le (trung binh
    dung 6,94 ms o 144), <= 60 giu cach cu (neo luc ve that tren luoi 8 ms); PaintLog [SUM] them khoang cach khung (min/TB/max) + span tick min/max.
 2. Engine KWin32App: g_SetLoopInterval(ms) (ham xuat, khong doi bo cuc lop), Run() doc moi vong.
 3. Represent3: PaintVsync=1 -> g_nRep3Vsync=1; Rep3Latency mac dinh 1 khi vsync (khong thi 3 nhu cu), ini van ghi de.
Tep: Engine/Src/KWin32App.h/.cpp, S3Client/S3Client.cpp, Represent3/KRepresentShell3.cpp (latin-1 CRLF)."""
import io, re, sys
SRC = r"D:\GAMEDEVNEW_wt_delta\Sources"
def load(rel):
    p = SRC + "\\" + rel
    return p, io.open(p, "r", encoding="latin-1", newline="").read()
def save(p, s, h0):
    if sum(1 for ch in s if ord(ch) >= 0x80) != h0: print("FAIL byte cao", p); sys.exit(1)
    if re.search(r"[^\r]\n", s): print("FAIL LF", p); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
def rep(s, old, new, n=1):
    c = s.count(old)
    if c != n: print("FAIL neo %r: thay %d, can %d" % (old[:90], c, n)); sys.exit(1)
    return s.replace(old, new)
def hi(s): return sum(1 for ch in s if ord(ch) >= 0x80)

# ---------------- Engine
p, s = load(r"Engine\Src\KWin32App.h"); h0 = hi(s)
if "g_SetLoopInterval" not in s:
    s = rep(s, "};\r\n//---------------------------------------------------------------------------\r\n#endif",
        "};\r\n// [NHIP 08/09] luoi co hoi chay GameLoop trong KWin32App::Run (ms): mac dinh 8; S3Client dat 1 khi PaintFps > 60 / PaintVsync\r\nENGINE_API void g_SetLoopInterval(unsigned int uMs);\r\n//---------------------------------------------------------------------------\r\n#endif")
save(p, s, h0); print("OK KWin32App.h")

p, s = load(r"Engine\Src\KWin32App.cpp"); h0 = hi(s)
if "s_nLoopInterval" not in s:
    s = rep(s, "#include \"KWin32Wnd.h\"\r\n",
        "#include \"KWin32Wnd.h\"\r\n"
        "// [NHIP 08/09] luoi vong bom: 8 ms giu nhip tick deu (56 ms); 1 ms khi ve > 60 fps (144 Hz = 6,94 ms/khung) - tick van deu (56 ms)\r\n"
        "static DWORD s_nLoopInterval = 8;\r\n"
        "ENGINE_API void g_SetLoopInterval(unsigned int uMs) { s_nLoopInterval = (uMs < 1) ? 1 : (uMs > 16 ? 16 : uMs); }\r\n")
    s = rep(s, "\tDWORD nInterval = 8;\r\n", "\tDWORD nInterval = s_nLoopInterval;\t// [NHIP 08/09] doc lai moi vong (g_SetLoopInterval)\r\n")
    s = rep(s, "\twhile (TRUE)\r\n\t{\r\n\t\tif (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))\r\n",
        "\twhile (TRUE)\r\n\t{\r\n\t\tnInterval = s_nLoopInterval;\t// [NHIP 08/09]\r\n\t\tif (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))\r\n")
save(p, s, h0); print("OK KWin32App.cpp")

# ---------------- S3Client
p, s = load(r"S3Client\S3Client.cpp"); h0 = hi(s)
if "g_nPaintVsync" not in s:
    if "s_dwNextPaint" in s.replace("static DWORD s_dwNextPaint = 0;", "").replace("s_dwNextPaint = nPaintElapse + nPaintStep;", "").replace("(int)(nPaintElapse + PAINT_LEAD_MS - s_dwNextPaint) >= 0", ""):
        print("FAIL: s_dwNextPaint con dung o cho khac"); sys.exit(1)
    s = rep(s, "static int\tg_nPaintInterp = 1;", "static int\tg_nPaintVsync = 0;\t\t// [NHIP 08/09] config.ini [Client] PaintVsync; 1 = ve moi vong bom, Represent3 Present(1) (vblank dan nhip)\r\nstatic int\tg_nPaintInterp = 1;")
    s = rep(s, "\tIniFile.GetInteger(\"Client\", \"PaintFps\", 30, &g_nPaintFps);\r\n\tif (g_nPaintFps < 0)\r\n\t\tg_nPaintFps = 0;\r\n\tif (g_nPaintFps > 60)\r\n\t\tg_nPaintFps = 60;\r\n",
        "\tIniFile.GetInteger(\"Client\", \"PaintFps\", 30, &g_nPaintFps);\r\n"
        "\tif (g_nPaintFps == -1)\r\n\t{\t// [NHIP 08/09] -1 = tu theo tan so man hinh hien tai (59 -> 60)\r\n"
        "\t\tDEVMODEA dmNhip; memset(&dmNhip, 0, sizeof(dmNhip)); dmNhip.dmSize = sizeof(dmNhip);\r\n"
        "\t\tint nHz = EnumDisplaySettingsA(NULL, ENUM_CURRENT_SETTINGS, &dmNhip) ? (int)dmNhip.dmDisplayFrequency : 60;\r\n"
        "\t\tif (nHz == 59) nHz = 60;\r\n\t\tif (nHz < 30) nHz = 60;\r\n\t\tg_nPaintFps = nHz;\r\n\t}\r\n"
        "\tif (g_nPaintFps < 0)\r\n\t\tg_nPaintFps = 0;\r\n"
        "\tif (g_nPaintFps > 240)\r\n\t\tg_nPaintFps = 240;\t// [NHIP 08/09] tran 60 -> 240 (man hinh 120/144/240 Hz)\r\n"
        "\tIniFile.GetInteger(\"Client\", \"PaintVsync\", 0, &g_nPaintVsync);\t// [NHIP 08/09] 1 = ve theo vblank (Represent3 doc cung khoa -> Present(1)); PaintFps khi do chi la nhan\r\n"
        "\tif (g_nPaintFps > 60 || g_nPaintVsync > 0)\r\n\t\tg_SetLoopInterval(1);\t// [NHIP 08/09] luoi vong bom 1 ms: luoi 8 ms chi cho toi da ~125 khung/giay\r\n")
    s = rep(s, "\tif (g_nPaintFps > 30)\r\n\t\ttimeBeginPeriod(1);", "\tif (g_nPaintFps > 30 || g_nPaintVsync > 0)\r\n\t\ttimeBeginPeriod(1);")
    s = rep(s, "\tif (g_nPaintFps > 30)\r\n\t\ttimeEndPeriod(1);", "\tif (g_nPaintFps > 30 || g_nPaintVsync > 0)\r\n\t\ttimeEndPeriod(1);")
    # thong ke nhip (PaintLog)
    s = rep(s, "\tstatic DWORD\ts_dwLastTickAt = 0;\r\n\tstatic DWORD\ts_dwTickSpan = 0;\r\n",
        "\tstatic DWORD\ts_dwLastTickAt = 0;\r\n\tstatic DWORD\ts_dwTickSpan = 0;\r\n"
        "\tstatic DWORD\ts_dwLastPaintAt = 0, s_LogGapMin = 0, s_LogGapMax = 0, s_LogGapSum = 0, s_LogGapCnt = 0, s_LogSpanMin = 0, s_LogSpanMax = 0;\t// [NHIP 08/09] PaintLog: khoang cach khung ve + span tick\r\n")
    s = rep(s, "\t\t\tif (s_dwLastTickAt && (DWORD)nElapse > s_dwLastTickAt)\r\n\t\t\t\ts_dwTickSpan = (DWORD)nElapse - s_dwLastTickAt;\r\n",
        "\t\t\tif (s_dwLastTickAt && (DWORD)nElapse > s_dwLastTickAt)\r\n\t\t\t{\r\n\t\t\t\ts_dwTickSpan = (DWORD)nElapse - s_dwLastTickAt;\r\n"
        "\t\t\t\tif (g_nPaintLog > 0) { if (!s_LogSpanMin || s_dwTickSpan < s_LogSpanMin) s_LogSpanMin = s_dwTickSpan; if (s_dwTickSpan > s_LogSpanMax) s_LogSpanMax = s_dwTickSpan; }\t// [NHIP 08/09]\r\n\t\t\t}\r\n")
    # nhip ve
    s = rep(s, "\t\tstatic DWORD s_dwNextPaint = 0;\r\n\t\tDWORD\tnPaintElapse = m_Timer.GetElapse();\r\n\t\tDWORD\tnPaintStep = 1000 / (DWORD)g_nPaintFps;\r\n\t\tif (nPaintStep < 1)\r\n\t\t\tnPaintStep = 1;\r\n\t\tif ((int)(nPaintElapse + PAINT_LEAD_MS - s_dwNextPaint) >= 0)\r\n\t\t{\r\n\t\t\ts_dwNextPaint = nPaintElapse + nPaintStep;\r\n",
        "\t\tstatic double s_dNextPaint = 0.0;\t// [NHIP 08/09] moc ve ke tiep (phan le)\r\n"
        "\t\tDWORD\tnPaintElapse = m_Timer.GetElapse();\r\n"
        "\t\tconst bool bLuoi1ms = (g_nPaintFps > 60 || g_nPaintVsync > 0);\r\n"
        "\t\tconst double dPaintStep = 1000.0 / (double)g_nPaintFps;\r\n"
        "\t\tconst int nLead = bLuoi1ms ? 0 : PAINT_LEAD_MS;\r\n"
        "\t\tif (g_nPaintVsync > 0 || (double)nPaintElapse + nLead >= s_dNextPaint)\r\n\t\t{\r\n"
        "\t\t\tif (bLuoi1ms)\r\n\t\t\t{\t// [NHIP 08/09] luoi 1 ms: cong deu tung buoc (144 fps = 6,94 ms, nhip trung binh dung); tut xa hon 1 khung thi dat lai\r\n"
        "\t\t\t\ts_dNextPaint += dPaintStep;\r\n"
        "\t\t\t\tif (s_dNextPaint < (double)nPaintElapse - dPaintStep)\r\n\t\t\t\t\ts_dNextPaint = (double)nPaintElapse + dPaintStep;\r\n\t\t\t}\r\n"
        "\t\t\telse\r\n\t\t\t\ts_dNextPaint = (double)nPaintElapse + (double)(1000 / (DWORD)g_nPaintFps);\t// nhu cu (<= 60): neo vao luc ve that, luoi 8 ms cho khoang cach deu\r\n")
    s = rep(s, "\t\t\tUiPaint(nGameFps);\r\n\t\t\tbPainted = TRUE;\r\n",
        "\t\t\tUiPaint(nGameFps);\r\n\t\t\tbPainted = TRUE;\r\n"
        "\t\t\tif (g_nPaintLog > 0)\r\n\t\t\t{\t// [NHIP 08/09] khoang cach giua hai lan ve\r\n"
        "\t\t\t\tDWORD tVe = timeGetTime();\r\n"
        "\t\t\t\tif (s_dwLastPaintAt) { DWORD g = tVe - s_dwLastPaintAt; s_LogGapSum += g; s_LogGapCnt++; if (!s_LogGapMin || g < s_LogGapMin) s_LogGapMin = g; if (g > s_LogGapMax) s_LogGapMax = g; }\r\n"
        "\t\t\t\ts_dwLastPaintAt = tVe;\r\n\t\t\t}\r\n")
    s = rep(s, "\t\t\t\tfprintf(pLog, \"[SUM] t=%u passes=%u avg=%u max=%u spikes=%u cross=%u\\n\",\r\n\t\t\t\t\tnLogT0, s_LogCnt, s_LogCnt ? s_LogSum / s_LogCnt : 0, s_LogMax, s_LogSpk, s_LogCross);\r\n",
        "\t\t\t\tfprintf(pLog, \"[SUM] t=%u passes=%u avg=%u max=%u spikes=%u cross=%u | ve: %u khung, cach %u/%u/%u ms (min/TB/max) | span tick %u..%u ms | PaintFps=%d vsync=%d\\n\",\r\n"
        "\t\t\t\t\tnLogT0, s_LogCnt, s_LogCnt ? s_LogSum / s_LogCnt : 0, s_LogMax, s_LogSpk, s_LogCross,\r\n"
        "\t\t\t\t\ts_LogGapCnt, s_LogGapMin, s_LogGapCnt ? s_LogGapSum / s_LogGapCnt : 0, s_LogGapMax, s_LogSpanMin, s_LogSpanMax, g_nPaintFps, g_nPaintVsync);\t// [NHIP 08/09]\r\n"
        "\t\t\t\ts_LogGapMin = s_LogGapMax = s_LogGapSum = s_LogGapCnt = s_LogSpanMin = s_LogSpanMax = 0;\r\n")
save(p, s, h0); print("OK S3Client.cpp")

# ---------------- Represent3
p, s = load(r"Represent\Represent3\KRepresentShell3.cpp"); h0 = hi(s)
if "PaintVsync" not in s:
    s = rep(s, "\tg_nRep3Vsync     = Rep3Ini(\"Rep3Vsync\", 0);\r\n",
        "\tg_nRep3Vsync     = Rep3Ini(\"Rep3Vsync\", 0);\r\n"
        "\tif (Rep3Ini(\"PaintVsync\", 0) > 0) g_nRep3Vsync = 1;\t// [NHIP 08/09] Game.exe ve theo vblank -> Present(1)\r\n")
    m = re.search(r'\tg_nRep3Latency   = Rep3Ini\("Rep3Latency", (\d+)\);([^\r\n]*)\r\n', s)
    if not m: print("FAIL: khong thay dong Rep3Latency"); sys.exit(1)
    s = s.replace(m.group(0), "\t{ int nLat = Rep3Ini(\"Rep3Latency\", -1); g_nRep3Latency = (nLat >= 0) ? nLat : (g_nRep3Vsync ? 1 : %s); }\t// [NHIP 08/09] vsync: hang 1 khung (do tre thap); khong vsync: %s nhu cu%s\r\n" % (m.group(1), m.group(1), m.group(2)))
save(p, s, h0); print("OK KRepresentShell3.cpp")
print("XONG NHIP")
