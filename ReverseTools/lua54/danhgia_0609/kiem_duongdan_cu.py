"""kiem_duongdan_cu.py - sau khi sap xep: bao tep XUAT HIEN LAI o duong dan cu (phien khac ghi nham) va tep bi danh thieu dich.
   python kiem_duongdan_cu.py [sua]   (sua = chuyen tep o duong dan cu sang duong dan moi, de tep moi neu da co -> giu ban moi nhat)"""
import os, sys, shutil
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
ROOT = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
P = os.path.join(ROOT, "script", "_duongdan_cu.txt")
SUA = len(sys.argv) > 1 and sys.argv[1] == "sua"
if not os.path.exists(P):
    print("khong co _duongdan_cu.txt"); sys.exit(0)
alias = []
for ln in open(P, "rb").read().decode("mbcs", errors="replace").split("\n"):
    ln = ln.strip()
    if ln.startswith("--@") and "=" in ln:
        cu, moi = ln[3:].split("=", 1)
        alias.append((cu.strip(), moi.strip()))
lai, thieu = [], []
for cu, moi in alias:
    pc = os.path.join(ROOT, cu); pm = os.path.join(ROOT, moi)
    if os.path.exists(pc):
        lai.append((cu, moi))
        if SUA:
            if os.path.exists(pm):
                if os.path.getmtime(pc) > os.path.getmtime(pm):
                    shutil.move(pc, pm); print("  GHI DE ban moi bang ban o duong dan cu (moi hon):", cu)
                else:
                    os.remove(pc); print("  XOA ban cu (cu hon ban moi):", cu)
            else:
                os.makedirs(os.path.dirname(pm), exist_ok=True); shutil.move(pc, pm); print("  CHUYEN:", cu, "->", moi)
    elif not os.path.exists(pm):
        thieu.append((cu, moi))
print("bi danh: %d dong; tep XUAT HIEN LAI o duong dan cu: %d; dich khong ton tai: %d" % (len(alias), len(lai), len(thieu)))
for cu, moi in lai[:20]: print("  CU LAI:", cu)
for cu, moi in thieu[:20]: print("  THIEU :", moi)
sys.exit(1 if (lai and not SUA) else 0)
