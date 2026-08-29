# -*- coding: utf-8 -*-
r"""[PETSYS 29/08] Dong bo TRAN CAP theo ban private VLTK (chu: "cap toi da 20
khong the tang cap ban keu tang len 21 moi dung duoc?"):
- pet_skill_def.txt: thay bang ban PRIVATE VLTK (130 cap - rut tu pak).
- levelup.txt: giu nguyen 20 dong goc Linux + noi 21..130 THEO DUNG QUY LUAT
  TUYEN TINH cua bang goc (do o console: Up=80*(n-5), Grown=2*Up,
  Tame=250*(n-5) tu cap 9; ItemCost +1 moi 4 cap; Rate giu 70 nhu doan cuoi).
- common.lua MAX_LEVEL 20 -> 130 (khop client_common private).
- C PET_MAX_LEVEL 20 -> 130 (clamp aura).
- chay lai p25 de aura.lua SKILLS phu 130 cap.
"""
import io
import os
import shutil
import subprocess
import sys

CR = chr(13)
SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"

# 1. pet_skill_def private
src = r"D:\GAMEDEVNEW\ReverseTools\bandonghanh\ra_pet\vltk_pet_skill_def.txt"
dst = SV + r"\settings\petsys\pet_skill_def.txt"
if not os.path.exists(dst + ".goc_linux"):
    shutil.copyfile(dst, dst + ".goc_linux")
shutil.copyfile(src, dst)
print("1. pet_skill_def = ban private 130 cap")

# 2. levelup noi 21..130
p = SV + r"\settings\petsys\levelup.txt"
s = io.open(p, "r", encoding="latin-1", newline="").read()
lf = s.replace(CR + "\n", "\n")
dong = [d for d in lf.split("\n") if d.strip()]
if len(dong) >= 131:
    print("2. da co 130 cap")
else:
    if not os.path.exists(p + ".goc_linux"):
        shutil.copyfile(p, p + ".goc_linux")
    ra = dong[:]
    for n in range(21, 131):
        up = 80 * (n - 5)
        grown = 160 * (n - 5)
        tame = 250 * (n - 5)
        cost = min(30, 7 + (n - 19) // 4)
        ra.append("%d\t%d\t%d\t%d\t%d" % (up, grown, tame, cost, 70))
    io.open(p, "w", encoding="latin-1", newline="").write(("\n".join(ra) + "\n").replace("\n", CR + "\n"))
    print("2. levelup noi toi 130 (quy luat tuyen tinh bang goc)")

# 3. common.lua MAX_LEVEL
p = SV + r"\script\petsys\common.lua"
s = io.open(p, "r", encoding="latin-1", newline="").read()
if "MAX_LEVEL = 130" in s:
    print("3. da co")
else:
    assert s.count("MAX_LEVEL = 20") == 1
    s = s.replace("MAX_LEVEL = 20", "MAX_LEVEL = 130", 1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("3. MAX_LEVEL 130")

# 4. C PET_MAX_LEVEL
p = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerPet.h"
s = io.open(p, "r", encoding="latin-1", newline="").read()
if "PET_MAX_LEVEL      130" in s:
    print("4. da co")
else:
    assert s.count("#define PET_MAX_LEVEL      20") == 1
    s = s.replace("#define PET_MAX_LEVEL      20",
                  "#define PET_MAX_LEVEL      130\t// ban private VLTK (client_common MAX_LEVEL=130)", 1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("4. C PET_MAX_LEVEL 130")

# 5. sinh lai aura SKILLS theo bang moi
r = subprocess.run([sys.executable, r"D:\GAMEDEVNEW\ReverseTools\bandonghanh\p25_aura_dungkhuon.py"],
                   capture_output=True, text=True)
print("5. p25:", (r.stdout or r.stderr).strip()[-120:])
print("XONG p45")
