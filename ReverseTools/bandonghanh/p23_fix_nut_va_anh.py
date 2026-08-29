# -*- coding: utf-8 -*-
r"""[PETSYS 28/08 chieu] 3 viec:
1. wrapper PetSys_Protocol ghi log petops.log (phan doi client/server khi nut im)
2. reloadfileEvent (lenh bai admin) + ReLoadScript 9 file petsys -> "Nap lai script"
   la an ngay khong can restart
3. UiPet.cpp: sua lech 1 hang khi tra npcs.txt (data dau = row 2 => id N = row N+2)
"""
import io
import os

BS = chr(92)
CR = chr(13)

# ---- 1. wrapper log ----
p = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\petsys\protocol_process_gs.lua"
s = io.open(p, "r", encoding="latin-1", newline="").read()
if "petops.log" not in s:
    cu = "function PetSys_Protocol(nOp)"
    them = ('function PetSys_Protocol(nOp)\r\n'
            '\tappendto("petops.log")\r\n'
            '\twrite(GetLocalDate("%H:%M:%S") .. " op=" .. nOp .. " nguoi=" .. GetName() .. "\\'
            'n")\r\n'
            '\twriteto()')
    assert s.count(cu) == 1
    s = s.replace(cu, them, 1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("1. wrapper da ghi log petops.log")
else:
    print("1. da co log")

# ---- 2. reloadfileEvent + petsys ----
p = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\lenhbaiadmin.lua"
s = io.open(p, "r", encoding="latin-1", newline="").read()
if "petsys" not in s:
    cu = 'ReLoadScript("' + BS*2 + 'script' + BS*2 + 'item' + BS*2 + 'test_loren_admin.lua")'
    assert s.count(cu) == 1
    them = cu + "\r\n-- [PETSYS 28/08] nap lai tron bo he Ban Dong Hanh PC\r\n"
    for f in ["protocol_process_gs", "head", "common", "dataload", "summon",
              "levelup", "name", "feature", "delete", "transferexp", "lang"]:
        them += ('ReLoadScript("' + BS*2 + 'script' + BS*2 + 'petsys' + BS*2 +
                 f + '.lua")\r\n')
    them = them.rstrip("\r\n")
    s = s.replace(cu, them, 1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("2. reloadfileEvent + 11 file petsys")
else:
    print("2. da co")

# ---- 3. UiPet.cpp: nTpl + 1 -> nTpl + 2 ----
p = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiPet.cpp"
s = io.open(p, "r", encoding="latin-1", newline="").read()
if "nTpl + 2" not in s:
    cu = 's_NpcTab.GetString(nTpl + 1, 12, (LPSTR)"", szRes, sizeof(szRes));'
    assert s.count(cu) == 1
    s = s.replace(cu,
        '// data dau cua KTabFile = row 2 (row 1 la header) => id N = row N + 2\r\n'
        '    s_NpcTab.GetString(nTpl + 2, 12, (LPSTR)"", szRes, sizeof(szRes));', 1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("3. da sua nTpl + 2")
else:
    print("3. da sua roi")
