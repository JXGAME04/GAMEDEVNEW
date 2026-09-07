import re, os
SP = os.path.dirname(os.path.abspath(__file__))
NEW = 'ROOT = os.environ.get("SAPXEP_ROOT", r"E:' + chr(92) + 'SourceTuanLe' + chr(92) + 'SourceVs22' + chr(92) + 'TESTLOFFF_ONLINE' + chr(92) + 'bin' + chr(92) + 'server")'
for f in ("r33_sapxep.py", "boot_gia.py", "kiem_duongdan_cu.py", "r33_lui.py"):
    p = os.path.join(SP, f)
    s = open(p, encoding="utf-8").read()
    if "SAPXEP_ROOT" in s:
        print("da co:", f); continue
    s2, n = re.subn(r'^ROOT = r"E:[^"]*server"$', lambda m: NEW, s, flags=re.M)
    if n != 1:
        raise SystemExit("khong thay ROOT trong %s (%d)" % (f, n))
    if not re.search(r"^import .*\bos\b", s2, re.M):
        s2 = "import os\n" + s2
    open(p, "w", encoding="utf-8").write(s2)
    print("ROOT env:", f)
