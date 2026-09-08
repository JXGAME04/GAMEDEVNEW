# -*- coding: utf-8 -*-
"""harness_patch_fps_0809.py - them nhip khung cho rep_harness (REP_FPS=63: moi khung cho du 1000/63 ms) de do Present/bo khung
nhu game that (PaintFps 63) ma khong can chu restart. Va tren ban trong D:\\GAMEDEVNEW_wt_rep3 (noi co Lib\\release\\engine.lib)."""
import io, sys
p = r"D:\GAMEDEVNEW_wt_rep3\ReverseTools\represent3\rep_harness.cpp"
s = io.open(p, "r", encoding="latin-1", newline="").read()
if "REP_FPS" in s:
    print("da va"); sys.exit(0)
old = "\t\trs->RepresentEnd();\r\n\t\tPump();\r\n\t}\r\n\tQueryPerformanceCounter(&t1);\r\n"
if s.count(old) != 1:
    old = old.replace("\r\n", "\n")
    if s.count(old) != 1: print("FAIL neo vong PERF"); sys.exit(1)
nl = "\r\n" if "\r\n" in old else "\n"
new = ("\t\trs->RepresentEnd();" + nl + "\t\tPump();" + nl +
       "\t\tif (g_dFrameMs > 0.0)" + nl +
       "\t\t{\t// [08/09] nhip khung nhu game (REP_FPS): ngu roi quay cho du thoi gian khung" + nl +
       "\t\t\tLARGE_INTEGER tn; QueryPerformanceCounter(&tn);" + nl +
       "\t\t\tdouble dNext = (double)t0.QuadPart + (double)(fr + 1) * g_dFrameMs * (double)f.QuadPart / 1000.0;" + nl +
       "\t\t\tdouble dLeft = (dNext - (double)tn.QuadPart) * 1000.0 / (double)f.QuadPart;" + nl +
       "\t\t\tif (dLeft > 2.0) Sleep((DWORD)(dLeft - 1.0));" + nl +
       "\t\t\tdo { QueryPerformanceCounter(&tn); } while ((double)tn.QuadPart < dNext);" + nl +
       "\t\t}" + nl +
       "\t}" + nl + "\tQueryPerformanceCounter(&t1);" + nl)
s = s.replace(old, new)
# bien toan cuc + doc env
old2 = "static int RepMode() { const char* e = getenv(\"REP_OFFSCREEN\"); return e ? atoi(e) : 0; }"
if s.count(old2) != 1: print("FAIL neo RepMode"); sys.exit(1)
s = s.replace(old2, old2 + nl + "static double g_dFrameMs = (getenv(\"REP_FPS\") && atof(getenv(\"REP_FPS\")) > 0.0) ? 1000.0 / atof(getenv(\"REP_FPS\")) : 0.0;\t// [08/09] REP_FPS=63")
io.open(p, "w", encoding="latin-1", newline="").write(s)
print("OK harness REP_FPS")
