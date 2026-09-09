# -*- coding: utf-8 -*-
"""goi_va_nhip2_dongho_tick_muot_0809.py - [NHIP 08/09 c] Chu bao "fps cao thay nguoi choi va bot di chuyen nhu toc bien".
Hai goc do duoc trong jx_paint.log:
 (1) MOC TICK lay SAU Breathe()+UiHeartBeat() nen xe dich theo thoi gian chay logic; va tick chi duoc kiem MOI VONG BOM nen tre
     theo do dai vong (canh nang: vong 8 ms) -> khoang tick do duoc nhay 45..69 ms (nhe: 53..58). Noi suy chia cho khoang TRUOC do,
     nen khi khoang that ngan hon: alpha chua toi 1000 da sang tick moi -> NHAY MOT DOAN (toc bien); khi dai hon: dong bang cuoi chu ky.
     Sau mot cu giat (span 474 ms) thi so chia sai gap 8 lan -> nhay rat manh.
 (2) Sua: moc tick lay TRUOC Breathe (nTickAt); so chia = TRUNG BINH TRUOT cua khoang tick (kep 40..90 ms, mac dinh 1000/18)
     nhan 0,97 (toi 1000 hoi som: tha dong bang 2-4 ms con hon nhay). Cong tac lui: [Client] PaintSmooth=0.
 [SUM] them 'kep N' = so khung bi kep tran alpha (do truc tiep trieu chung).
Chi sua S3Client.cpp -> chi Game.exe doi (CoreClient giu nguyen)."""
import io, re, sys
p = r"D:\GAMEDEVNEW_wt_delta\Sources\S3Client\S3Client.cpp"
s = io.open(p, "r", encoding="latin-1", newline="").read()
h0 = sum(1 for ch in s if ord(ch) >= 0x80)
def rep(s, old, new, n=1):
    c = s.count(old)
    if c != n: print("FAIL neo %r: thay %d, can %d" % (old[:90], c, n)); sys.exit(1)
    return s.replace(old, new)
if "g_nPaintSmooth" in s:
    print("da va roi"); sys.exit(0)

# 1. bien cau hinh
s = rep(s, "static int\tg_nPaintVsync = 0;",
    "static int\tg_nPaintSmooth = 1;\t\t// [NHIP 08/09 c] 1 = so chia noi suy dung trung binh truot cua khoang tick (het nhay/dong bang moi tick); 0 = nhu cu\r\n"
    "static int\tg_nPaintVsync = 0;")
s = rep(s, "\tIniFile.GetInteger(\"Client\", \"PaintVsync\", 0, &g_nPaintVsync);",
    "\tIniFile.GetInteger(\"Client\", \"PaintSmooth\", 1, &g_nPaintSmooth);\t// [NHIP 08/09 c]\r\n"
    "\tIniFile.GetInteger(\"Client\", \"PaintVsync\", 0, &g_nPaintVsync);")

# 2. bien trang thai
s = rep(s, "\tstatic DWORD\ts_dwLastPaintAt = 0, s_LogGapMin = 0,",
    "\tstatic double\ts_dSpanAvg = 0.0;\t// [NHIP 08/09 c] trung binh truot khoang tick (ms)\r\n"
    "\tstatic DWORD\ts_LogKep = 0;\t\t// [NHIP 08/09 c] so khung bi kep tran alpha trong ky\r\n"
    "\tstatic DWORD\ts_dwLastPaintAt = 0, s_LogGapMin = 0,")

# 3. moc tick lay TRUOC Breathe
s = rep(s, "\t\tDWORD\tdwLgT0 = g_nPaintLog > 0 ? timeGetTime() : 0;\r\n\t\tBOOL\tbLgBre = g_pCoreShell->Breathe();\r\n",
    "\t\t// [NHIP 08/09 c] Moc tick phai lay TRUOC Breathe/UiHeartBeat: lay sau thi moc xe dich theo\r\n"
    "\t\t// thoi gian chay logic (0-25 ms) va lam khoang tick do duoc nhay len xuong.\r\n"
    "\t\tconst DWORD\tnTickAt = (DWORD)m_Timer.GetElapse();\r\n"
    "\t\tDWORD\tdwLgT0 = g_nPaintLog > 0 ? timeGetTime() : 0;\r\n\t\tBOOL\tbLgBre = g_pCoreShell->Breathe();\r\n")

# 4. chup moc + trung binh truot
old = ("\t\t\tif (s_dwLastTickAt && (DWORD)nElapse > s_dwLastTickAt)\r\n"
       "\t\t\t{\r\n"
       "\t\t\t\ts_dwTickSpan = (DWORD)nElapse - s_dwLastTickAt;\r\n")
new = ("\t\t\tif (s_dwLastTickAt && nTickAt > s_dwLastTickAt)\r\n"
       "\t\t\t{\r\n"
       "\t\t\t\ts_dwTickSpan = nTickAt - s_dwLastTickAt;\t// [NHIP 08/09 c] theo moc truoc Breathe\r\n"
       "\t\t\t\tif (s_dwTickSpan >= 30 && s_dwTickSpan <= 120)\r\n"
       "\t\t\t\t{\t// [NHIP 08/09 c] trung binh truot: mot khoang le (giat, nap map) khong duoc lam sai so chia ca chu ky sau\r\n"
       "\t\t\t\t\tif (s_dSpanAvg <= 0.0)\r\n\t\t\t\t\t\ts_dSpanAvg = 1000.0 / (double)GAME_FPS;\r\n"
       "\t\t\t\t\ts_dSpanAvg += ((double)s_dwTickSpan - s_dSpanAvg) * 0.125;\r\n"
       "\t\t\t\t\tif (s_dSpanAvg < 40.0) s_dSpanAvg = 40.0;\r\n\t\t\t\t\tif (s_dSpanAvg > 90.0) s_dSpanAvg = 90.0;\r\n"
       "\t\t\t\t}\r\n")
s = rep(s, old, new)
s = rep(s, "\t\t\ts_dwLastTickAt = (DWORD)nElapse;\r\n", "\t\t\ts_dwLastTickAt = nTickAt;\t// [NHIP 08/09 c]\r\n")

# 5. alpha theo trung binh truot
old = ("\t\t\t\tint\tnAlpha;\r\n"
       "\t\t\t\tif (s_dwTickSpan >= 20 && s_dwTickSpan <= 200 && nPaintElapse >= s_dwLastTickAt)\r\n"
       "\t\t\t\t\tnAlpha = (int)((nPaintElapse - s_dwLastTickAt) * 1000 / s_dwTickSpan);\r\n"
       "\t\t\t\telse\r\n"
       "\t\t\t\t\tnAlpha = (int)(nPaintElapse * (DWORD)GAME_FPS - (m_GameCounter - 1) * 1000);\r\n"
       "\t\t\t\tif (nAlpha < 0)\r\n\t\t\t\t\tnAlpha = 0;\r\n")
new = ("\t\t\t\tint\tnAlpha;\r\n"
       "\t\t\t\t// [NHIP 08/09 c] So chia = TRUNG BINH TRUOT chu khong phai khoang tick lien truoc: khoang do duoc\r\n"
       "\t\t\t\t// leo theo do dai vong bom (53..58 ms canh nhe, 45..69 canh nang, 474 khi nap map). Chia cho mot\r\n"
       "\t\t\t\t// khoang le -> alpha chua toi 1000 da sang tick moi (nhay mot doan = \"toc bien\") hoac toi som\r\n"
       "\t\t\t\t// roi dong bang. Nhan 0,97 de toi 1000 hoi som: dong bang 2-4 ms de chiu hon mot cu nhay.\r\n"
       "\t\t\t\tif (g_nPaintSmooth > 0 && s_dSpanAvg > 0.0 && nPaintElapse >= s_dwLastTickAt)\r\n"
       "\t\t\t\t\tnAlpha = (int)((double)(nPaintElapse - s_dwLastTickAt) * 1000.0 / (s_dSpanAvg * 0.97));\r\n"
       "\t\t\t\telse if (s_dwTickSpan >= 20 && s_dwTickSpan <= 200 && nPaintElapse >= s_dwLastTickAt)\r\n"
       "\t\t\t\t\tnAlpha = (int)((nPaintElapse - s_dwLastTickAt) * 1000 / s_dwTickSpan);\r\n"
       "\t\t\t\telse\r\n"
       "\t\t\t\t\tnAlpha = (int)(nPaintElapse * (DWORD)GAME_FPS - (m_GameCounter - 1) * 1000);\r\n"
       "\t\t\t\tif (nAlpha < 0)\r\n\t\t\t\t\tnAlpha = 0;\r\n"
       "\t\t\t\tif (nAlpha > 1000)\r\n\t\t\t\t{\r\n\t\t\t\t\tnAlpha = 1000;\r\n\t\t\t\t\tif (g_nPaintLog > 0) s_LogKep++;\t// [NHIP 08/09 c] khung dong bang cuoi chu ky\r\n\t\t\t\t}\r\n")
s = rep(s, old, new)

# 6. [SUM]
s = rep(s, "| ve %u/%u ms tick %u/%u ms (TB/max) | PaintFps=%d vsync=%d\\n\",\r\n",
    "| ve %u/%u ms tick %u/%u ms (TB/max) | kep %u | PaintFps=%d smooth=%d vsync=%d\\n\",\r\n")
s = rep(s, "s_LogTickCnt ? s_LogTickSum / s_LogTickCnt : 0, s_LogTickMax, g_nPaintFps, g_nPaintVsync);\t// [NHIP 08/09 a/b]\r\n",
    "s_LogTickCnt ? s_LogTickSum / s_LogTickCnt : 0, s_LogTickMax, s_LogKep, g_nPaintFps, g_nPaintSmooth, g_nPaintVsync);\t// [NHIP 08/09 a/b/c]\r\n"
    "\t\t\t\ts_LogKep = 0;\r\n")

if sum(1 for ch in s if ord(ch) >= 0x80) != h0: print("FAIL byte cao"); sys.exit(1)
if re.search(r"[^\r]\n", s): print("FAIL LF"); sys.exit(1)
io.open(p, "w", encoding="latin-1", newline="").write(s)
print("OK S3Client.cpp"); print("XONG NHIP c")
