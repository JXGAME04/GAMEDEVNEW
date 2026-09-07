"""r33_lui.py - LUI sap xep: doc script/_duongdan_cu.txt, chuyen tep tu duong dan moi ve duong dan cu, xoa tep bi danh.
   Chay khi GameServer DA TAT:  python r33_lui.py"""
import os, sys, shutil

def ansi_str(b):
    # byte ANSI -> unicode dung nhu Windows (cp1252; 5 byte khong dinh nghia -> U+0081...)
    return "".join(chr(x) if (x < 128 or x in (0x81, 0x8D, 0x8F, 0x90, 0x9D)) else bytes([x]).decode("cp1252") for x in b)
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
ROOT = os.environ.get("SAPXEP_ROOT", r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server")
P = os.path.join(ROOT, "script", "_duongdan_cu.txt")
if not os.path.exists(P):
    print("khong co _duongdan_cu.txt - khong co gi de lui"); sys.exit(0)
n = 0
for ln in ansi_str(open(P, "rb").read()).split("\n"):
    ln = ln.strip()
    if not ln.startswith("--@") or "=" not in ln: continue
    cu, moi = [x.strip() for x in ln[3:].split("=", 1)]
    pc, pm = os.path.join(ROOT, cu), os.path.join(ROOT, moi)
    if os.path.exists(pm) and not os.path.exists(pc):
        os.makedirs(os.path.dirname(pc), exist_ok=True); shutil.move(pm, pc); n += 1
os.rename(P, P + ".da_lui")
removed = 0
for dp, dn, fn in os.walk(os.path.join(ROOT, "script"), topdown=False):
    if dp != os.path.join(ROOT, "script") and not os.listdir(dp):
        os.rmdir(dp); removed += 1
print("DA LUI %d tep ve duong dan cu; xoa %d thu muc rong; bi danh -> _duongdan_cu.txt.da_lui" % (n, removed))
