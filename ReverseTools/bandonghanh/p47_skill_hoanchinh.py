# -*- coding: utf-8 -*-
r"""[PETSYS 29/08] Hoan chinh 18 ky nang pet theo DUNG nguon VLTK:
1. Sinh \script\skill\petskill.lua (server+client) = KHUNG chuan JX1 (chep tu
   file mau resistance nhu p25) + bang SKILLS NGUYEN VAN tu petskill.lua VLTK
   -> skill co du lieu cap that (GetSkill an) - het "hoc ma khong co gi".
2. bikip.lua: luu o = SkillId*100+Level; hoc trung skill = +1 cap (max 5 -
   theo cot MaxLevel bang); menu hien cap hien tai.
3. INI: o ky nang 36x36 buoc 40 (icon skill chuan 36 - dang tran o 34).
4. bdh_admin: menu chinh BDH_Root -> vao thang he PET (xoa cac muc partner cu).
5. item Bi kip: anh sach random_taskbook.spr (co san JX1).
6. PLOG do dem truoc/sau Consume trong HocBiKip (nghi "khong tru item").
"""
import io
import glob
import re

CR = chr(13)
BS = chr(92)
SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"


def doc_lf(p):
    return io.open(p, "r", encoding="latin-1", newline="").read().replace(CR + "\n", "\n")


def ghi_crlf(p, lf):
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))


# ---------- 1. petskill.lua ----------
mau = glob.glob(SV + r"\script\skill\partner\resistance\*.lua")[0]
khung = open(mau, "rb").read().decode("latin-1").replace(CR + "\n", "\n")
i = khung.find("SKILLS=")
assert i > 0
j = i + khung[i:].find("{")
depth = 0
k = j
while k < len(khung):
    if khung[k] == "{":
        depth += 1
    elif khung[k] == "}":
        depth -= 1
        if depth == 0:
            break
    k += 1
k += 1
vl = io.open(r"D:\GAMEDEVNEW\ReverseTools\bandonghanh\ra_pet\vltk_petskill.lua", "r",
             encoding="latin-1").read().replace(CR + "\n", "\n").replace(CR, "\n")
iv = vl.find("SKILLS=")
jv = iv + vl[iv:].find("{")
depth = 0
kv = jv
while kv < len(vl):
    if vl[kv] == "{":
        depth += 1
    elif vl[kv] == "}":
        depth -= 1
        if depth == 0:
            break
    kv += 1
kv += 1
bang_vltk = vl[iv:kv]
moi = (khung[:i] + bang_vltk + khung[k:])
moi = ("-- [PETSYS 29/08] 18 ky nang bi dong pet - KHUNG chep nguyen tu file mau\n-- "
       + mau.split(BS)[-1] + "; bang SKILLS NGUYEN VAN tu petskill.lua VLTK (pak).\n" + moi)
for base in (SV, CL):
    ghi_crlf(base + r"\script\skill\petskill.lua", moi)
print("1. petskill.lua server+client, dai", len(moi))

# ---------- 2. bikip.lua nang cap ----------
p = SV + r"\script\petsys\bikip.lua"
lf = doc_lf(p)
if "*100" in lf:
    print("2. da co")
else:
    # menu: cho phep chon skill DA hoc de nang cap (bo loc bDaHoc), nhan kem cap
    cu = """	local tbOpt = {}
	local nId
	for nId = 1670, 1687 do
		local bDaHoc = 0
		for i = 0, 3 do
			if GetTask(5139 + i) == nId then
				bDaHoc = 1
			end
		end
		if bDaHoc == 0 then
			tinsert(tbOpt, {BIKIP_SKILLS[nId], PetSys.HocBiKip, {PetSys, nId, nSlot, nItemIndex}})
		end
	end"""
    moi2 = """	local tbOpt = {}
	local nId
	for nId = 1670, 1687 do
		local nCapCu = 0
		local nSlotCu = 0
		for i = 0, 3 do
			local v = GetTask(5139 + i)
			if floor(v / 100) == nId or v == nId then
				nSlotCu = 5139 + i
				nCapCu = v - nId * 100
				if nCapCu < 1 then
					nCapCu = 1
				end
			end
		end
		if nSlotCu == 0 then
			tinsert(tbOpt, {BIKIP_SKILLS[nId], PetSys.HocBiKip, {PetSys, nId, nSlot, 0}})
		elseif nCapCu < 5 then
			tinsert(tbOpt, {format("%s (cap %d)", BIKIP_SKILLS[nId], nCapCu), PetSys.HocBiKip, {PetSys, nId, nSlotCu, nCapCu}})
		end
	end"""
    assert lf.count(cu) == 1
    lf = lf.replace(cu, moi2, 1)
    cu3 = """function PetSys:HocBiKip(nSkillId, nSlot, nItemIndex)
	if GetTask(nSlot) > 0 then
		return
	end"""
    moi3 = """function PetSys:HocBiKip(nSkillId, nSlot, nCapCu)
	-- nCapCu = 0: hoc moi vao o trong; >0: nang cap skill dang o nSlot"""
    assert lf.count(cu3) == 1
    lf = lf.replace(cu3, moi3, 1)
    cu4 = """	ConsumeEquiproomItem(1, 6, 1, 4880)
	SetTask(nSlot, nSkillId)"""
    moi4 = """	local nTruoc = CalcEquiproomItemCount(6, 1, 4880, -1)
	ConsumeEquiproomItem(1, 6, 1, 4880)
	PLOG("HocBiKip: bikip " .. nTruoc .. " -> " .. CalcEquiproomItemCount(6, 1, 4880, -1))
	SetTask(nSlot, nSkillId * 100 + (nCapCu + 1))"""
    assert lf.count(cu4) == 1
    lf = lf.replace(cu4, moi4, 1)
    cu5 = 'Msg2Player(format("'
    i5 = lf.find(cu5, lf.find("HocBiKip"))
    # thay msg cuoi de kem cap
    ghi_crlf(p, lf)
    print("2. bikip id*100+cap, nang cap max 5")

# ---------- 3. INI o 36x36 ----------
p = CL + r"\Ui\Ui3\pet_main.ini"
s = io.open(p, "r", encoding="latin-1", newline="").read()


def doi_sec(s, sec, l, t, w, h):
    i = s.find("[" + sec + "]")
    j = s.find("[", i + 1)
    khoi = s[i:j]
    m2 = re.sub(r"Left=\d+", "Left=%d" % l, khoi, 1)
    m2 = re.sub(r"Top=\d+", "Top=%d" % t, m2, 1)
    m2 = re.sub(r"Width=\d+", "Width=%d" % w, m2, 1)
    m2 = re.sub(r"Height=\d+", "Height=%d" % h, m2, 1)
    return s.replace(khoi, m2, 1)


s = doi_sec(s, "Skill_1", 376, 168, 36, 36)
for kq in range(4):
    s = doi_sec(s, "Ext_Skill_%d" % (kq + 1), 416 + 40 * kq, 168, 36, 36)
io.open(p, "w", encoding="latin-1", newline="").write(s)
print("3. INI o skill 36x36")

# ---------- 4. bdh_admin don menu ----------
p = SV + r"\script\item\bdh_admin.lua"
lf = doc_lf(p)
if "BDH_A_CapKim" in lf and "-- [29/08] menu partner cu DA GO" not in lf:
    i4 = lf.find("function BDH_Root()")
    j4 = lf.find("end", lf.find("SayEx", i4))
    khoi = lf[i4:j4 + 3]
    moi4 = """function BDH_Root()
	-- [29/08] menu partner cu DA GO theo yeu cau chu ("cac dong pet cu khong
	-- dung thi xoa di") - vao thang he PET ban PC. Ham BDH_A_* giu lai duoi
	-- file de khoi pha nhung khong con duong vao.
	BDH_P_Root()
end"""
    lf = lf.replace(khoi, moi4, 1)
    ghi_crlf(p, lf)
    print("4. menu admin -> thang PET PC")
else:
    print("4. da co / khac dang")

# ---------- 5. anh Bi kip ----------
for pth in (SV + r"\settings\item\magicscript.txt", CL + r"\settings\item\magicscript.txt"):
    s = io.open(pth, "r", encoding="latin-1", newline="").read()
    cu = "huihuangzhiguo.spr"
    # chi dong 4880
    lf5 = s.replace(CR + "\n", "\n")
    n5 = lf5.split("\n")
    doi = 0
    for idx, d in enumerate(n5):
        c = d.split("\t")
        if len(c) > 4 and c[3] == "4880" and "random_taskbook" not in c[4]:
            c[4] = BS + "spr" + BS + "item" + BS + "questkey" + BS + "random_taskbook.spr"
            n5[idx] = "\t".join(c)
            doi = 1
    if doi:
        ghi_crlf(pth, "\n".join(n5))
        print("5. anh Bi kip = sach:", pth[:2])
    else:
        print("5. da co:", pth[:2])
print("XONG p47")
