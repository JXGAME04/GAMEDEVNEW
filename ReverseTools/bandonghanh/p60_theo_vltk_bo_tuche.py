# -*- coding: utf-8 -*-
r"""[PETSYS 29/08] CHU CHOT: theo ban VLTK PC, BO het phan toi tu che.

GO (tu che 100%):
1. He TRANG BI pet (petequip.lua, item 4881..4887, 7 o PetEquip, nut Duc lai
   SendOp(10), map [10]) - se port lai tu nguon that (4 kenh COMPANIONEQUIP).
2. Pet TU DANH (sPetFight) - ca Linux lan VLTK deu khong co.
3. Co che "nang cap ky nang id*100+cap" - nguon VLTK khong co.

GIU + SUA CHO DUNG NGUON VLTK (client_common.lua rut tu pak):
- EXT_SKILL_OPEN_PET_LEVEL=21, EXT_SKILL_GET_NEW_LEVEL=5, EXT_SKILL_MAX_COUNT=4
- GetExtSkillCount() = (cap-21)/5 + 1, tran 4   <- nguyen van
- tbPetSkillIDList = 1670..1687 (18 skill)
- PET_MIJI_ITEM: VLTK id 4808 -> JX1 da co item khac o 4808 nen GIU id 4880
  (nan ma nhu quy trinh du an), ten/anh da theo VLTK.
- ZHENYUAN_TO_XIUZHEN_POINT_RATE=200 / VALUE=20000 (Tu Chan) - da dung.
- MAX_LEVEL=130 (VLTK) - da dung.
"""
import io
import os
import re

CR = chr(13)
BS = chr(92)
SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"


def doc_lf(p):
    return io.open(p, "r", encoding="latin-1", newline="").read().replace(CR + "\n", "\n")


def ghi_crlf(p, lf):
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))


# ---------- 1. GO he trang bi ----------
p = SV + r"\script\petsys\head.lua"
lf = doc_lf(p)
lf = re.sub(r'\t\[10\] = "EquipRebuildDlg",[^\n]*\n', "", lf)
ghi_crlf(p, lf)
p = SV + r"\script\petsys\protocol_process_gs.lua"
lf = doc_lf(p)
lf = re.sub(r'Include\("' + BS * 2 + BS * 2 + 'script' + BS * 2 + BS * 2 + 'petsys' + BS * 2 + BS * 2 + r'petequip\.lua"\)\n', "", lf)
ghi_crlf(p, lf)
p = SV + r"\script\item\lenhbaiadmin.lua"
lf = doc_lf(p)
lf = re.sub(r'ReLoadScript\("[^"]*petequip\.lua"\)\n?', "", lf)
ghi_crlf(p, lf)
old = SV + r"\script\petsys\petequip.lua"
if os.path.exists(old):
    os.replace(old, old + ".tuche_da_go")
print("1. GO trang bi tu che (script + map op10 + reload)")

# item 4881..4887 khoi bang
for pth in (SV + r"\settings\item\magicscript.txt", CL + r"\settings\item\magicscript.txt"):
    rows = [d.split("\t") for d in doc_lf(pth).split("\n") if d.strip()]
    giu = [c for c in rows if not (len(c) > 3 and c[3] in [str(x) for x in range(4881, 4888)])]
    ghi_crlf(pth, "\n".join("\t".join(c) for c in giu) + "\n")
    print("   bo", len(rows) - len(giu), "dong item trang bi:", pth[:2])

# ---------- 2. common.lua: hang VLTK nguyen van ----------
p = SV + r"\script\petsys\common.lua"
lf = doc_lf(p)
if "EXT_SKILL_OPEN_PET_LEVEL" not in lf:
    ds = "\n".join("\t[%d] = %d," % (i + 1, 1670 + i) for i in range(18))
    them = "\n".join([
        "",
        "-- [29/08] nguyen van client_common.lua ban VLTK PC (rut tu pak)",
        "EXT_SKILL_OPEN_PET_LEVEL = 21",
        "EXT_SKILL_MAX_COUNT = 4",
        "EXT_SKILL_GET_NEW_LEVEL = 5",
        "-- VLTK dung item 4808; JX1 da co item khac o 4808 -> nan ma sang 4880",
        "PET_MIJI_ITEM = {tbProp = {6, 1, 4880, 0, 0, 0}}",
        "ZHENYUAN_TO_XIUZHEN_POINT_RATE = 200",
        "ZHENYUAN_TO_XIUZHEN_POINT_VALUE = 20000",
        "",
        "tbPetSkillIDList =",
        "{",
        ds,
        "}",
        "",
        "function GetExtSkillCount()",
        "\tlocal nPetLevel = PET_GetLevel()",
        "\tlocal nExtSkillCount = floor((nPetLevel - EXT_SKILL_OPEN_PET_LEVEL) / EXT_SKILL_GET_NEW_LEVEL) + 1",
        "\tif nExtSkillCount > EXT_SKILL_MAX_COUNT then",
        "\t\tnExtSkillCount = EXT_SKILL_MAX_COUNT",
        "\tend",
        "\treturn nExtSkillCount",
        "end",
        "",
    ])
    ghi_crlf(p, lf + them)
    print("2. common.lua + hang VLTK + GetExtSkillCount")
else:
    print("2. da co")

# ---------- 3. bikip.lua viet lai theo VLTK ----------
p = SV + r"\script\petsys\bikip.lua"
lf = doc_lf(p)
# bo co che nang cap id*100+cap -> luu id tran; dieu kien o theo GetExtSkillCount
i = lf.find("function main(nItemIndex)")
j = lf.find("\nfunction PetSys:HocBiKip")
assert i > 0 and j > i, (i, j)
import sys
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn  # noqa: E402
moi = '''function main(nItemIndex)
	if PET_IsCreate() ~= 1 then
		Talk(1, "", "''' + vn("Ngươi chưa có bạn đồng hành") + '''")
		return 1
	end
	if PET_GetLevel() < EXT_SKILL_OPEN_PET_LEVEL then
		Talk(1, "", format("''' + vn("Bạn đồng hành đạt cấp %d mới học được kỹ năng") + '''", EXT_SKILL_OPEN_PET_LEVEL))
		return 1
	end
	-- so o mo theo cap - nguyen van cong thuc VLTK
	local nChoPhep = GetExtSkillCount()
	local nSlot = 0
	local i
	for i = 0, EXT_SKILL_MAX_COUNT - 1 do
		if GetTask(5139 + i) <= 0 then
			nSlot = 5139 + i
			break
		end
	end
	if nSlot == 0 then
		Talk(1, "", format("''' + vn("Bạn đồng hành đã học đủ %d kỹ năng") + '''", EXT_SKILL_MAX_COUNT))
		return 1
	end
	if nSlot - 5139 + 1 > nChoPhep then
		Talk(1, "", format("''' + vn("Cấp hiện tại chỉ mở %d ô kỹ năng (mỗi %d cấp mở thêm 1 ô)") + '''", nChoPhep, EXT_SKILL_GET_NEW_LEVEL))
		return 1
	end
	local tbOpt = {}
	local k
	for k = 1, getn(tbPetSkillIDList) do
		local nId = tbPetSkillIDList[k]
		local bDaHoc = 0
		for i = 0, EXT_SKILL_MAX_COUNT - 1 do
			if GetTask(5139 + i) == nId then
				bDaHoc = 1
			end
		end
		if bDaHoc == 0 then
			tinsert(tbOpt, {BIKIP_SKILLS[nId], PetSys.HocBiKip, {PetSys, nId, nSlot}})
		end
	end
	tinsert(tbOpt, {%CANCEL})
	CreateNewSayEx("''' + vn("Chọn kỹ năng muốn dạy cho bạn đồng hành") + '''", tbOpt)
	return 1
end
'''
lf = lf[:i] + moi + lf[j:]
# HocBiKip: bo cap
i2 = lf.find("function PetSys:HocBiKip")
j2 = lf.find("\nend", lf.find("Msg2Player", i2))
moi2 = '''function PetSys:HocBiKip(nSkillId, nSlot)
	if GetTask(nSlot) > 0 then
		return
	end
	local tbProp = PET_MIJI_ITEM.tbProp
	if CalcEquiproomItemCount(tbProp[1], tbProp[2], tbProp[3], -1) < 1 then
		Talk(1, "", "''' + vn("Không còn Bí kiếp trong hành trang") + '''")
		return
	end
	ConsumeEquiproomItem(1, tbProp[1], tbProp[2], tbProp[3])
	SetTask(nSlot, nSkillId)
	PLOG("HocBiKip: o=" .. nSlot .. " skill=" .. nSkillId)
	Msg2Player(format("''' + vn("Bạn đồng hành đã học được kỹ năng [%s]") + '''", BIKIP_SKILLS[nSkillId]))'''
lf = lf[:i2] + moi2 + lf[j2:]
ghi_crlf(p, lf)
print("3. bikip.lua theo cong thuc VLTK (bo nang cap tu che)")

# ---------- 4. C: bo pet tu danh + parse cap ----------
p = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerPet.cpp"
lf = doc_lf(p)
i = lf.find("static DWORD s_dwFightTick[MAX_PLAYER];")
if i > 0:
    j = lf.find("\n//---", i)
    lf = lf[:i] + "// [29/08] pet TU DANH da GO - ca Linux lan VLTK deu khong co\n" + lf[j + 1:]
    lf = lf.replace("\t\tsPetFight(i, nNpc);\n", "")
    print("4a. GO pet tu danh")
# parse: bo id*100+lv -> id tran (nguon khong co cap)
cu = """			int nV = sPetG(nPlayerIdx, 5139 + k);
			if (nV <= 0) continue;
			int nSk = (nV >= 100000) ? nV / 100 : nV;
			int nLv = (nV >= 100000) ? nV % 100 : 1;
			if (nLv < 1) nLv = 1;
			if (nLv > 5) nLv = 5;
			KSkill* pExt = (KSkill*)g_SkillManager.GetSkill(nSk, nLv);"""
moi = """			int nSk = sPetG(nPlayerIdx, 5139 + k);
			if (nSk <= 0) continue;
			KSkill* pExt = (KSkill*)g_SkillManager.GetSkill(nSk, 1);"""
if cu in lf:
    lf = lf.replace(cu, moi, 1)
    print("4b. bo parse cap tu che (server)")
ghi_crlf(p, lf)

p = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiPet.cpp"
lf = doc_lf(p)
cu = """        int nV = sPetTV(5139 + i);
        int nSk = (nV >= 100000) ? nV / 100 : nV;
        int nLv = (nV >= 100000) ? nV % 100 : 1;
        if (nSk > 0)
            m_ExtSkill[i].HoldObject(CGOG_SKILL_FIGHT, nSk, nLv, 0);"""
moi = """        int nSk = sPetTV(5139 + i);
        if (nSk > 0)
            m_ExtSkill[i].HoldObject(CGOG_SKILL_FIGHT, nSk, 1, 0);"""
if cu in lf:
    lf = lf.replace(cu, moi, 1)
    print("4c. bo parse cap (client)")
# go 7 o trang bi + nut Duc lai
i = lf.find("    // [29/08] 7 o trang bi hang tren")
if i > 0:
    j = lf.find("\n    }\n", i) + len("\n    }\n")
    lf = lf[:i] + "    // [29/08] 7 o trang bi: he tu che DA GO - se port lai tu nguon that\n" + lf[j:]
    print("4d. GO ve 7 o trang bi")
lf = lf.replace("        if (uParam == (unsigned int)(KWndWindow*)&m_CompanionBtn)\n            SendOp(10);\t// [29/08] mo menu duc lai trang bi (server)\n",
                "        // m_CompanionBtn (\"Duc lai\"): he trang bi chua port tu nguon that\n")
ghi_crlf(p, lf)
print("XONG p60")
