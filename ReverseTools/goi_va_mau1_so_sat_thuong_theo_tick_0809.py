# -*- coding: utf-8 -*-
"""goi_va_mau1_so_sat_thuong_theo_tick_0809.py - [MAU 08/09] Chu bao (PaintFps=120): "so mat mau troi nhanh hon".
Goc: KNpc::PaintBlood giam m_nBloodTime[i] MOI KHUNG VE (ban goc ve = tick 18/s nen dung); PaintFps 60 -> troi nhanh 3,3 lan,
120 -> 6,7 lan. Sua: giam MOT lan moi tick logic (chot m_dwBloodTick vs SubWorld[0].m_dwCurrentTime, cung kieu m_dwLastDirTick),
do cao troi cong them phan le tick (g_nPaintAlpha 0..1000 do POSSHIFT dat) de van muot giua hai tick. KNpc them 1 DWORD ->
CoreClient + Game.exe phai doi cung luc.
Kem [NHIP b] S3Client: [SUM] them TB/max thoi gian ve va tick; neo lai moc ve khi tre > 2 ms (khong ve don 1 ms sau khung 16 ms).
Tep: Core/Src/KNpc.h, KNpc.cpp, CoreShell.cpp, S3Client/S3Client.cpp (latin-1 CRLF, chu thich TCVN3/GBK giu nguyen)."""
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

p, s = load(r"Core\Src\KNpc.h"); h0 = hi(s)
if "m_dwBloodTick" not in s:
    s = rep(s, "\tDWORD\t\t\t\tm_dwLastDirTick;\t// tick logic cuoi da lam muot m_ResDir\r\n",
        "\tDWORD\t\t\t\tm_dwLastDirTick;\t// tick logic cuoi da lam muot m_ResDir\r\n"
        "\tDWORD\t\t\t\tm_dwBloodTick;\t\t// [MAU 08/09] tick logic cuoi da giam m_nBloodTime (so sat thuong troi theo tick, khong theo khung ve)\r\n")
save(p, s, h0); print("OK KNpc.h")

p, s = load(r"Core\Src\KNpc.cpp"); h0 = hi(s)
if "m_dwBloodTick" not in s:
    s = rep(s, "\tm_dwLastDirTick\t\t\t\t= 0;\r\n", "\tm_dwLastDirTick\t\t\t\t= 0;\r\n\tm_dwBloodTick\t\t\t\t= 0;\t// [MAU 08/09]\r\n")
    s = rep(s, "extern BOOL\t\t\tg_bPaintInterpFocus;\t// CoreShell.cpp: PaintFps interpolation drives the camera\r\n",
        "extern BOOL\t\t\tg_bPaintInterpFocus;\t// CoreShell.cpp: PaintFps interpolation drives the camera\r\n"
        "extern int\t\t\tg_nPaintAlpha;\t\t\t// [MAU 08/09] CoreShell.cpp: phan le tick (0..1000) cua khung ve hien tai, 0 khi tat noi suy\r\n")
    s = rep(s, "int\tKNpc::PaintBlood(int nHeightOffset)\r\n{\r\n\tint addbloodspace = 50;\r\n",
        "int\tKNpc::PaintBlood(int nHeightOffset)\r\n{\r\n\tint addbloodspace = 50;\r\n"
        "\t// [MAU 08/09] giam bo dem MOT lan moi tick logic (18/s) nhu ban goc (ban goc ve = tick). Truoc day giam moi khung ve:\r\n"
        "\t// PaintFps 60 -> so troi nhanh 3,3 lan, 120 -> 6,7 lan. Dat trong ham ve (khong phai Activate) cung ly do m_dwLastDirTick.\r\n"
        "\tconst BOOL bTickMoi = (m_dwBloodTick != SubWorld[0].m_dwCurrentTime);\r\n"
        "\tif (bTickMoi)\r\n\t\tm_dwBloodTick = SubWorld[0].m_dwCurrentTime;\r\n")
    s = rep(s, "\t\tint nHeightOff = (int)(nHeightOffset + (defMAX_SHOW_BLOOD_TIME - m_nBloodTime[i]) * defSHOW_BLOOD_MOVE_SPEED / 3);\r\n",
        "\t\tint nHeightOff = (int)(nHeightOffset + ((defMAX_SHOW_BLOOD_TIME - m_nBloodTime[i]) * 1000 + g_nPaintAlpha) * defSHOW_BLOOD_MOVE_SPEED / 3000);\t// [MAU 08/09] + phan le tick: troi muot giua hai tick\r\n")
    s = rep(s, "NOTHING :\r\n\t\tm_nBloodTime[i]--;\r\n\t\tif (m_nBloodTime[i] <= 0)\r\n",
        "NOTHING :\r\n\t\tif (!bTickMoi)\r\n\t\t\treturn;\t// [MAU 08/09] chua sang tick moi: chi ve, khong giam\r\n\t\tm_nBloodTime[i]--;\r\n\t\tif (m_nBloodTime[i] <= 0)\r\n")
save(p, s, h0); print("OK KNpc.cpp")

p, s = load(r"Core\Src\CoreShell.cpp"); h0 = hi(s)
if "g_nPaintAlpha" not in s:
    s = rep(s, "BOOL\tg_bPaintInterpFocus = FALSE;\r\n",
        "BOOL\tg_bPaintInterpFocus = FALSE;\r\n"
        "int\t\tg_nPaintAlpha = 0;\t// [MAU 08/09] phan le tick (0..1000) cua khung ve hien tai, POSSHIFT dat moi khung; 0 khi tat noi suy (KNpc::PaintBlood dung)\r\n")
    s = rep(s, "\t\tif (nAlpha > 1000)\r\n\t\t\tnAlpha = 1000;\r\n",
        "\t\tif (nAlpha > 1000)\r\n\t\t\tnAlpha = 1000;\r\n\t\tg_nPaintAlpha = nAlpha;\t// [MAU 08/09]\r\n")
save(p, s, h0); print("OK CoreShell.cpp")

p, s = load(r"S3Client\S3Client.cpp"); h0 = hi(s)
if "s_LogPaintSum" not in s:
    s = rep(s, "\t\t\t\tif (s_dNextPaint < (double)nPaintElapse - dPaintStep)\r\n\t\t\t\t\ts_dNextPaint = (double)nPaintElapse + dPaintStep;\r\n",
        "\t\t\t\tif (s_dNextPaint < (double)nPaintElapse - 2.0)\r\n\t\t\t\t\ts_dNextPaint = (double)nPaintElapse + dPaintStep;\t// [NHIP 08/09 b] tre > 2 ms (tick/khung nang): neo lai, khong ve don (khung 1 ms ngay sau khung 16 ms = giat kep)\r\n")
    s = rep(s, "\t\tstatic DWORD s_LogSum = 0, s_LogCnt = 0, s_LogMax = 0, s_LogSpk = 0, s_LogCross = 0, s_LogLast = 0;\r\n",
        "\t\tstatic DWORD s_LogSum = 0, s_LogCnt = 0, s_LogMax = 0, s_LogSpk = 0, s_LogCross = 0, s_LogLast = 0;\r\n"
        "\t\tstatic DWORD s_LogPaintSum = 0, s_LogPaintMax = 0, s_LogPaintCnt = 0, s_LogTickSum = 0, s_LogTickMax = 0, s_LogTickCnt = 0;\t// [NHIP 08/09 b] thoi gian ve / tick moi luot\r\n"
        "\t\tif (bPainted) { s_LogPaintSum += nLogPaint; s_LogPaintCnt++; if (nLogPaint > s_LogPaintMax) s_LogPaintMax = nLogPaint; }\r\n"
        "\t\tif (m_GameCounter != nLogCntBefore) { s_LogTickSum += nLogTick; s_LogTickCnt++; if (nLogTick > s_LogTickMax) s_LogTickMax = nLogTick; }\r\n")
    s = rep(s, "| span tick %u..%u ms | PaintFps=%d vsync=%d\\n\",\r\n",
        "| span tick %u..%u ms | ve %u/%u ms tick %u/%u ms (TB/max) | PaintFps=%d vsync=%d\\n\",\r\n")
    s = rep(s, "s_LogGapCnt ? s_LogGapSum / s_LogGapCnt : 0, s_LogGapMax, s_LogSpanMin, s_LogSpanMax, g_nPaintFps, g_nPaintVsync);\t// [NHIP 08/09]\r\n",
        "s_LogGapCnt ? s_LogGapSum / s_LogGapCnt : 0, s_LogGapMax, s_LogSpanMin, s_LogSpanMax,\r\n"
        "\t\t\t\t\ts_LogPaintCnt ? s_LogPaintSum / s_LogPaintCnt : 0, s_LogPaintMax, s_LogTickCnt ? s_LogTickSum / s_LogTickCnt : 0, s_LogTickMax, g_nPaintFps, g_nPaintVsync);\t// [NHIP 08/09 a/b]\r\n"
        "\t\t\t\ts_LogPaintSum = s_LogPaintMax = s_LogPaintCnt = s_LogTickSum = s_LogTickMax = s_LogTickCnt = 0;\r\n")
save(p, s, h0); print("OK S3Client.cpp")
print("XONG MAU")
