# -*- coding: utf-8 -*-
r"""[PETSYS 29/08] Logic he TRANG BI DONG HANH - chi lam phan CO NGUON:
- deo/thao 10 o (bang goc: DetailType 0..9 = vi tri)
- thuoc tinh BO theo settings\petsys\suitattrib.txt (3 bac)
- ruong 1/2/3 + chia khoa (item goc) -> ra trang bi dung bo
- nut "Duc lai": nguon KHONG co luat roll (logic server private) ->
  bao ro "dang hoan thien", KHONG tu che so.
O task: 5143..5152 = 10 o trang bi (id item); 5163 = so mon cung bo dang
mac (de C ap thuoc tinh bo).
"""
import io
import sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn  # noqa: E402

CR = chr(13)
BS = chr(92)
SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"


def doc_lf(p):
    return io.open(p, "r", encoding="latin-1", newline="").read().replace(CR + "\n", "\n")


def ghi_crlf(p, lf):
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))


# ---------- petequip.lua ----------
noi = "\n".join([
    'Include("' + BS * 2 + 'script' + BS * 2 + 'petsys' + BS * 2 + 'head.lua")',
    'Include("' + BS * 2 + 'script' + BS * 2 + 'petsys' + BS * 2 + 'lang.lua")',
    'Include("' + BS * 2 + 'script' + BS * 2 + 'petsys' + BS * 2 + 'petequip_def.lua")',
    "",
    "-- [PETSYS 29/08] TRANG BI DONG HANH - port theo bang goc VLTK.",
    "-- 10 o: task 5143..5152 (luu ParticularType cua mon dang deo)",
    "-- 5163: so mon cung bo dang mac (C doc de ap thuoc tinh bo)",
    "PETEQUIP_O_DAU = 5143",
    "PETEQUIP_O_SO = 10",
    "PETEQUIP_O_BOCOUNT = 5163",
    "",
    "-- dem so mon cung bo -> ghi 5163 (bo*100 + so mon) cho C doc",
    "function PetEquip_CapNhatBo()",
    "\tlocal tbDem = {}",
    "\tlocal i",
    "\tfor i = 0, PETEQUIP_O_SO - 1 do",
    "\t\tlocal nId = GetTask(PETEQUIP_O_DAU + i)",
    "\t\tlocal tb = PETEQUIP_DEF[nId]",
    "\t\tif tb then",
    "\t\t\ttbDem[tb.nSuit] = (tbDem[tb.nSuit] or 0) + 1",
    "\t\tend",
    "\tend",
    "\tlocal nBoTot = -1",
    "\tlocal nSoTot = 0",
    "\tfor k, v in tbDem do",
    "\t\tif v > nSoTot then",
    "\t\t\tnSoTot = v",
    "\t\t\tnBoTot = k",
    "\t\tend",
    "\tend",
    "\tif nBoTot < 0 then",
    "\t\tSetTask(PETEQUIP_O_BOCOUNT, 0)",
    "\telse",
    "\t\tSetTask(PETEQUIP_O_BOCOUNT, nBoTot * 100 + nSoTot)",
    "\tend",
    "\treturn nBoTot, nSoTot",
    "end",
    "",
    "-- dung item trang bi = DEO vao dung o (tra mon cu ve tui)",
    "function main(nItemIndex)",
    "\tif PET_IsCreate() ~= 1 then",
    '\t\tTalk(1, "", "' + vn("Ngươi chưa có bạn đồng hành") + '")',
    "\t\treturn 1",
    "\tend",
    "\tlocal nG, nD, nP = GetItemProp(nItemIndex)",
    "\tlocal tb = PETEQUIP_DEF[nP]",
    "\tif not tb then",
    "\t\treturn 1",
    "\tend",
    "\tlocal nO = PETEQUIP_O_DAU + tb.nSlot - 1",
    "\tlocal nCu = GetTask(nO)",
    "\tif nCu > 0 then",
    "\t\tAddItem(6, 1, nCu, 1, 0, 0)",
    "\tend",
    "\tSetTask(nO, nP)",
    "\tlocal nBo, nSo = PetEquip_CapNhatBo()",
    '\tMsg2Player(format("' + vn("Bạn đồng hành đã trang bị [%s] - bộ %s %d món") + '",',
    "\t\tPETEQUIP_VITRI[tb.nSlot], PETEQUIP_SUIT_TEN[tb.nSuit] or \"?\", nSo))",
    '\tPLOG("PetEquip: deo o=" .. tb.nSlot .. " id=" .. nP .. " bo=" .. nBo .. " so=" .. nSo)',
    "\treturn",
    "end",
    "",
    "-- op 10: cua so trang bi (menu server) - xem / thao / duc lai",
    "function PetSys:EquipRebuildDlg()",
    "\tif PET_IsCreate() ~= 1 then",
    "\t\treturn",
    "\tend",
    "\tlocal tbOpt = {}",
    "\tlocal i",
    "\tfor i = 1, PETEQUIP_O_SO do",
    "\t\tlocal nId = GetTask(PETEQUIP_O_DAU + i - 1)",
    "\t\tlocal tb = PETEQUIP_DEF[nId]",
    "\t\tif tb then",
    '\t\t\ttinsert(tbOpt, {format("%s: %s - ' + vn("tháo ra") + '", PETEQUIP_VITRI[i],',
    "\t\t\t\tPETEQUIP_SUIT_TEN[tb.nSuit] or \"?\"), PetSys.EquipThao, {PetSys, i}})",
    "\t\tend",
    "\tend",
    "\tif getn(tbOpt) == 0 then",
    '\t\tTalk(1, "", "' + vn("Bạn đồng hành chưa mặc trang bị nào (dùng vật phẩm trang bị Đồng Hành để mặc)") + '")',
    "\t\treturn",
    "\tend",
    '\ttinsert(tbOpt, {"' + vn("Đúc lại trang bị (đang hoàn thiện)") + '", PetSys.EquipDucLai, {PetSys}})',
    "\ttinsert(tbOpt, {%CANCEL})",
    "\tlocal nBo, nSo = PetEquip_CapNhatBo()",
    '\tCreateNewSayEx(format("' + vn("Trang bị Đồng Hành - bộ %s %d món") + '",',
    "\t\t(nBo >= 0 and PETEQUIP_SUIT_TEN[nBo]) or \"-\", nSo), tbOpt)",
    "end",
    "",
    "function PetSys:EquipThao(nSlot)",
    "\tlocal nO = PETEQUIP_O_DAU + nSlot - 1",
    "\tlocal nId = GetTask(nO)",
    "\tif nId <= 0 then",
    "\t\treturn",
    "\tend",
    "\tAddItem(6, 1, nId, 1, 0, 0)",
    "\tSetTask(nO, 0)",
    "\tPetEquip_CapNhatBo()",
    '\tMsg2Player(format("' + vn("Đã tháo trang bị ở vị trí %s") + '", PETEQUIP_VITRI[nSlot]))',
    "end",
    "",
    "function PetSys:EquipDucLai()",
    '\tTalk(1, "", "' + vn("Đúc lại trang bị cần luật đúc của máy chủ gốc - đang hoàn thiện, chưa mở") + '")',
    "end",
    "",
])
ghi_crlf(SV + r"\script\petsys\petequip.lua", noi)
print("1. petequip.lua (deo 10 o + bo + thao)")

# ---------- petbox.lua: ruong + chia khoa ----------
noi2 = "\n".join([
    'Include("' + BS * 2 + 'script' + BS * 2 + 'petsys' + BS * 2 + 'head.lua")',
    'Include("' + BS * 2 + 'script' + BS * 2 + 'petsys' + BS * 2 + 'petequip_def.lua")',
    "",
    "-- [PETSYS 29/08] Ruong trang bi Dong Hanh 1/2/3 (goc 5064..5066):",
    "-- can Chia Khoa Ruong Dong Hanh, mo ra 1 mon ngau nhien cua bo tuong ung.",
    "PETBOX_RUONG_BO = {",
    "\t[4929] = 0,\t-- Ruong 1 -> bo Bich Huyet",
    "\t[4930] = 1,\t-- Ruong 2 -> bo Kim Lan",
    "\t[4931] = 1,\t-- Ruong 3 -> bo Kim Lan (ban goc chua co bo 3 Dan Tam)",
    "}",
    "",
    "function main(nItemIndex)",
    "\tlocal nG, nD, nP = GetItemProp(nItemIndex)",
    "\tlocal nBo = PETBOX_RUONG_BO[nP]",
    "\tif nBo == nil then",
    "\t\treturn 1",
    "\tend",
    "\tif CalcEquiproomItemCount(6, 1, PETEQUIP_CHIAKHOA, -1) < 1 then",
    '\t\tTalk(1, "", "' + vn("Cần Chìa Khóa Rương Đồng Hành mới mở được") + '")',
    "\t\treturn 1",
    "\tend",
    "\tlocal tbCo = {}",
    "\tlocal k",
    "\tfor k, v in PETEQUIP_DEF do",
    "\t\tif v.nSuit == nBo then",
    "\t\t\ttinsert(tbCo, k)",
    "\t\tend",
    "\tend",
    "\tif getn(tbCo) == 0 then",
    "\t\treturn 1",
    "\tend",
    "\tConsumeEquiproomItem(1, 6, 1, PETEQUIP_CHIAKHOA)",
    "\tlocal nChon = tbCo[random(1, getn(tbCo))]",
    "\tAddItem(6, 1, nChon, 1, 0, 0)",
    '\tMsg2Player(format("' + vn("Mở rương nhận được trang bị Đồng Hành bộ %s") + '", PETEQUIP_SUIT_TEN[nBo]))',
    '\tPLOG("PetBox: ruong=" .. nP .. " ra=" .. nChon)',
    "\treturn",
    "end",
    "",
])
ghi_crlf(SV + r"\script\petsys\petbox.lua", noi2)
print("2. petbox.lua (ruong + chia khoa)")

# ---------- head map op 10 + include ----------
p = SV + r"\script\petsys\head.lua"
lf = doc_lf(p)
if "EquipRebuildDlg" not in lf:
    neo = '\t[PET_OPERATION_XIUZHEN_POINT] = "XiuzhenPointDlg",\n'
    assert lf.count(neo) == 1
    lf = lf.replace(neo, neo + '\t[10] = "EquipRebuildDlg",\t-- trang bi Dong Hanh\n', 1)
    ghi_crlf(p, lf)
    print("3. head map [10]")
p = SV + r"\script\petsys\protocol_process_gs.lua"
lf = doc_lf(p)
if "petequip.lua" not in lf:
    neo = 'Include("' + BS * 2 + 'script' + BS * 2 + 'petsys' + BS * 2 + 'xiuzhen.lua")\n'
    assert lf.count(neo) == 1
    lf = lf.replace(neo, neo + 'Include("' + BS * 2 + 'script' + BS * 2 + 'petsys' + BS * 2 + 'petequip.lua")\n', 1)
    ghi_crlf(p, lf)
    print("4. protocol Include petequip")
p = SV + r"\script\item\lenhbaiadmin.lua"
lf = doc_lf(p)
for f in ("petequip", "petbox", "petequip_def"):
    if f + ".lua" not in lf:
        neo = 'ReLoadScript("' + BS * 2 + 'script' + BS * 2 + 'petsys' + BS * 2 + 'bikip.lua")'
        lf = lf.replace(neo, neo + '\nReLoadScript("' + BS * 2 + 'script' + BS * 2 + 'petsys' + BS * 2 + f + '.lua")', 1)
ghi_crlf(p, lf)
print("5. lenhbai reload 3 file moi")

# ---------- admin: cap trang bi goc ----------
p = SV + r"\script\item\bdh_admin.lua"
lf = doc_lf(p)
i = lf.find("function BDH_P_CapKN()")
j = lf.find("\nend", i)
moi = '''function BDH_P_CapKN()
	local i
	for i = 1, 5 do
		AddItem(6, 1, 4880, 1, 0, 0)
	end
	for i = 4907, 4916 do
		AddItem(6, 1, i, 1, 0, 0)
	end
	for i = 1, 5 do
		AddItem(6, 1, 4929, 1, 0, 0)
		AddItem(6, 1, 4930, 1, 0, 0)
		AddItem(6, 1, 4932, 1, 0, 0)
		AddItem(6, 1, 4927, 1, 0, 0)
	end
	Msg2Player("Da cap 5 Bi kiep + bo Bich Huyet 10 mon + 5 ruong/chia khoa/ket tinh")'''
lf = lf[:i] + moi + lf[j:]
ghi_crlf(p, lf)
print("6. admin cap trang bi goc")
print("XONG p64")
