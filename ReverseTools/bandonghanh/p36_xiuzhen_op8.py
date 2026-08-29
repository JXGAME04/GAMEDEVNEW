# -*- coding: utf-8 -*-
r"""[PETSYS 29/08] Nut "Diem Tu Chan" = op 8 (PET_OPERATION_XIUZHEN_POINT,
theo client VLTK private): doi 200 diem chan nguyen (task 362
TASK_CHANGNGUYENDAN) lay 1 diem Tu Chan (o 5115), tran diem 20000
(ZHENYUAN_TO_XIUZHEN_POINT_RATE/VALUE trong client_common.lua VLTK).
1. script\petsys\xiuzhen.lua (moi)
2. common.lua: + PET_OPERATION_XIUZHEN_POINT = 8
3. head.lua: map [8] -> XiuzhenPointDlg
4. protocol_process_gs.lua: Include xiuzhen.lua
5. lenhbaiadmin.lua: ReLoadScript xiuzhen.lua
6. UiPet.cpp: nut Xiuzhen op 0 -> 8 (build Game.exe dot client)
"""
import io
import sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn  # noqa: E402

SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CR = chr(13)
BS = chr(92)


def doc(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def ghi_crlf(p, lf):
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))


# ---- 1. xiuzhen.lua ----
p = SV + r"\script\petsys\xiuzhen.lua"
noidung = "\n".join([
    'Include("' + BS * 2 + 'script' + BS * 2 + 'petsys' + BS * 2 + 'head.lua")',
    'Include("' + BS * 2 + 'script' + BS * 2 + 'petsys' + BS * 2 + 'lang.lua")',
    'Include("' + BS * 2 + 'script' + BS * 2 + 'lib' + BS * 2 + 'lib_task.lua")',
    "",
    "-- [PETSYS 29/08] op 8 ban private VLTK: doi chan nguyen -> diem Tu Chan",
    "ZHENYUAN_RATE = 200",
    "XIUZHEN_MAX = 20000",
    "",
    "function PetSys:XiuzhenPointDlg()",
    "\tif PET_IsCreate() ~= 1 then",
    "\t\treturn",
    "\tend",
    "\tlocal nCo = GetTask(TASK_CHANGNGUYENDAN)",
    '\tPLOG("Xiuzhen: channguyen=" .. nCo .. " diem=" .. PET_GetXiuzhenPoint())',
    "\tif PET_GetXiuzhenPoint() >= XIUZHEN_MAX then",
    '\t\tTalk(1, "", "' + vn("Điểm Tu Chân đã đạt tối đa") + '")',
    "\t\treturn",
    "\tend",
    "\tif nCo < ZHENYUAN_RATE then",
    '\t\tTalk(1, "", format("' + vn("Cần %d điểm chân nguyên để đổi 1 điểm Tu Chân (ngươi đang có %d)") + '", ZHENYUAN_RATE, nCo))',
    "\t\treturn",
    "\tend",
    "\tlocal tbOpt = {}",
    '\ttinsert(tbOpt, {format("' + vn("Đổi %d chân nguyên lấy 1 điểm Tu Chân") + '", ZHENYUAN_RATE), self.XiuzhenConfirm, {self}})',
    "\ttinsert(tbOpt, {%CANCEL})",
    '\tCreateNewSayEx(format("' + vn("Ngươi đang có %d điểm chân nguyên") + '", nCo), tbOpt)',
    "end",
    "",
    "function PetSys:XiuzhenConfirm()",
    "\tlocal nCo = GetTask(TASK_CHANGNGUYENDAN)",
    "\tif nCo < ZHENYUAN_RATE then",
    "\t\treturn",
    "\tend",
    "\tif PET_GetXiuzhenPoint() >= XIUZHEN_MAX then",
    "\t\treturn",
    "\tend",
    "\tSetTask(TASK_CHANGNGUYENDAN, nCo - ZHENYUAN_RATE)",
    "\tPET_SetXiuzhenPoint(PET_GetXiuzhenPoint() + 1)",
    '\tPLOG("Xiuzhen: DOI XONG - con " .. GetTask(TASK_CHANGNGUYENDAN) .. " CN, diem=" .. PET_GetXiuzhenPoint())',
    '\tMsg2Player(format("' + vn("Đổi thành công! Điểm Tu Chân hiện tại: %d") + '", PET_GetXiuzhenPoint()))',
    "end",
    "",
])
ghi_crlf(p, noidung)
print("1. ghi xiuzhen.lua")

# ---- 2. common.lua ----
p = SV + r"\script\petsys\common.lua"
s = doc(p)
if "XIUZHEN_POINT = 8" not in s:
    lf = s.replace(CR + "\n", "\n")
    neo = "PET_OPERATION_TAME = 7\n"
    assert lf.count(neo) == 1
    lf = lf.replace(neo, neo + "PET_OPERATION_XIUZHEN_POINT = 8\n", 1)
    ghi_crlf(p, lf)
    print("2. common + op8")
else:
    print("2. da co")

# ---- 3. head.lua map ----
p = SV + r"\script\petsys\head.lua"
s = doc(p)
if "XiuzhenPointDlg" not in s:
    lf = s.replace(CR + "\n", "\n")
    neo = '\t[PET_OPERATION_TAME] = "TransferExp",\n'
    assert lf.count(neo) == 1
    lf = lf.replace(neo, neo + '\t[PET_OPERATION_XIUZHEN_POINT] = "XiuzhenPointDlg",\n', 1)
    ghi_crlf(p, lf)
    print("3. head map [8]")
else:
    print("3. da co")

# ---- 4. protocol Include ----
p = SV + r"\script\petsys\protocol_process_gs.lua"
s = doc(p)
if "xiuzhen.lua" not in s:
    lf = s.replace(CR + "\n", "\n")
    neo = 'Include("' + BS * 2 + 'script' + BS * 2 + 'petsys' + BS * 2 + 'transferexp.lua")'
    i = lf.find(neo)
    assert i > 0
    cuoi = lf.find("\n", i) + 1
    lf = lf[:cuoi] + 'Include("' + BS * 2 + 'script' + BS * 2 + 'petsys' + BS * 2 + 'xiuzhen.lua")\n' + lf[cuoi:]
    ghi_crlf(p, lf)
    print("4. protocol Include xiuzhen")
else:
    print("4. da co")

# ---- 5. lenhbai reload ----
p = SV + r"\script\item\lenhbaiadmin.lua"
s = doc(p)
if "xiuzhen.lua" not in s:
    lf = s.replace(CR + "\n", "\n")
    neo = 'ReLoadScript("' + BS * 2 + 'script' + BS * 2 + 'petsys' + BS * 2 + 'feed.lua")'
    assert lf.count(neo) == 1
    lf = lf.replace(neo, neo + '\nReLoadScript("' + BS * 2 + 'script' + BS * 2 + 'petsys' + BS * 2 + 'xiuzhen.lua")', 1)
    ghi_crlf(p, lf)
    print("5. lenhbai reload xiuzhen")
else:
    print("5. da co")

# ---- 6. UiPet.cpp op 0 -> 8 ----
p = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiPet.cpp"
s = doc(p)
lf = s.replace(CR + "\n", "\n")
cu6 = "PET_OP_LEVEL_UP, 0,"
moi6 = "PET_OP_LEVEL_UP, 8,\t// 8 = XIUZHEN_POINT (ban private VLTK)"
if moi6 in lf:
    print("6. da co")
else:
    assert lf.count(cu6) == 1, lf.count(cu6)
    lf = lf.replace(cu6, moi6, 1)
    ghi_crlf(p, lf)
    print("6. VA op Xiuzhen 0 -> 8")
print("XONG p36")
