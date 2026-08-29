# -*- coding: utf-8 -*-
r"""[PETSYS 29/08] PET CHIEN DAU + BAM CHU + 4 KY NANG (dot theo yeu cau chu):
"pet di theo sau neu toi di nhanh qua pet se khong theo nua" +
"pet khong tu tan cong quai chac do chua co ky nang".

A. KPlayerPartner.cpp: bo static sPartnerPickTarget (tai dung cho pet).
B. KPlayerPet.cpp:
   - sPetSummon: toc do = chu (walk=run chu, run=run chu*5/4) -> bam kip.
   - Pet_ProcessAI: keo ve khi cach >800mps (25 o); sync FightMode voi chu;
     chu bat chien dau -> chon dich mode 22 (ke danh chu / gan nhat, vision
     480) -> do_skill (skill slot 1..4 tu TEMPLATE npc - boss co skill san);
     khong dich -> do_run bam sau chu (nguong 96).
   - sPetApplyAura: + ap 4 ky nang BI DONG da hoc (task 5139..5142, skill
     1670..1687 port tu VLTK) len PET moi ~30s nhu aura chu.
C. UiPet.cpp UpdateData: 4 o Ext_Skill hien icon tu task 5139+k.
D. Bang item: dong 4880 "Bi kip Dong Hanh" (server+client, khuon 4874,
   Script=\script\petsys\bikip.lua).
E. bikip.lua: menu 18 skill (ten trich tu bang) -> hoc vao slot trong.
F. bdh_admin: muc cap 5 Bi kip.
"""
import io
import sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn  # noqa: E402

CR = chr(13)
BS = chr(92)
SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"


def doc_lf(p):
    return io.open(p, "r", encoding="latin-1", newline="").read().replace(CR + "\n", "\n")


def ghi_crlf(p, lf):
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))


# ---------- A. bo static ----------
p = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerPartner.cpp"
lf = doc_lf(p)
cu = "static int sPartnerPickTarget(int nNpcIdx, int nOwnerNpcIdx, int nMode, int nVision)"
moi = ("// [PET 29/08] bo static: he pet PC tai dung chon-dich nay\n"
       "int sPartnerPickTarget(int nNpcIdx, int nOwnerNpcIdx, int nMode, int nVision)")
if "bo static: he pet" in lf:
    print("A. da co")
else:
    assert lf.count(cu) == 1
    ghi_crlf(p, lf.replace(cu, moi, 1))
    print("A. VA extern pick target")

# ---------- B. KPlayerPet ----------
p = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerPet.cpp"
lf = doc_lf(p)

# B0. extern
if "sPartnerPickTarget" not in lf:
    neo = "#define PET_AURA_RECAST"
    i = lf.find(neo)
    assert i > 0
    cuoi = lf.find("\n", i) + 1
    lf = lf[:cuoi] + "\nextern int sPartnerPickTarget(int nNpcIdx, int nOwnerNpcIdx, int nMode, int nVision);\n" + lf[cuoi:]
    print("B0. extern pick")

# B1. toc do khi summon
if "m_RunSpeed * 5 / 4" not in lf:
    cu = "\tpNpc->m_bNpcFollowFindPath = FALSE;\n"
    moi = ("\tpNpc->m_bNpcFollowFindPath = FALSE;\n"
           "\t// [29/08] chay bang chu de bam kip (\"di nhanh qua pet khong theo\")\n"
           "\tpNpc->m_WalkSpeed = pOwner->m_RunSpeed;\n"
           "\tpNpc->m_RunSpeed = pOwner->m_RunSpeed * 5 / 4;\n")
    assert lf.count(cu) == 1
    lf = lf.replace(cu, moi, 1)
    print("B1. toc do")

# B2. AI moi
cu_ai = """	int nPX = 0, nPY = 0, nOX = 0, nOY = 0;
	pNpc->GetMpsPos(&nPX, &nPY);
	pOwnerNpc->GetMpsPos(&nOX, &nOY);
	int nDis = abs(nPX - nOX) + abs(nPY - nOY);
	if (nDis > PET_FORCE_SYNC * 2)
	{
		pNpc->SetPos(nOX + 48, nOY + 48);
		return;
	}
	if (nDis > PET_FOLLOW_DIS)
	{
		int nXGo = nOX + 50 - rand() % 100;
		int nYGo = nOY + 50 - rand() % 100;
		pNpc->SendCommand(do_walk, nXGo, nYGo);
	}
}"""
moi_ai = """	int nPX = 0, nPY = 0, nOX = 0, nOY = 0;
	pNpc->GetMpsPos(&nPX, &nPY);
	pOwnerNpc->GetMpsPos(&nOX, &nOY);
	int nDis = abs(nPX - nOX) + abs(nPY - nOY);
	// [29/08] bo xa 25 o la keo ve ngay ("di nhanh qua pet khong theo nua")
	if (nDis > 800)
	{
		pNpc->SetPos(nOX + 48, nOY + 48);
		return;
	}
	// dong bo phe + che do chien dau voi chu (khuon Partner_ProcessAI)
	if (pNpc->m_CurrentCamp != pOwnerNpc->m_CurrentCamp)
		pNpc->SetCurrentCamp(pOwnerNpc->m_CurrentCamp);
	if (pNpc->m_FightMode != pOwnerNpc->m_FightMode)
		pNpc->m_FightMode = pOwnerNpc->m_FightMode;
	// chu bat chien dau -> danh ke dang danh chu / dich gan (mode 22)
	if (pOwnerNpc->m_FightMode)
	{
		int nTarget = sPartnerPickTarget(nNpcIdx, Player[nOwner].m_nIndex, 22, 480);
		if (nTarget > 0)
		{
			int nSkillId = 0;
			for (int s = 1; s <= 4; s++)
				if (pNpc->m_SkillList.m_Skills[s].SkillId > 0)
				{
					nSkillId = pNpc->m_SkillList.m_Skills[s].SkillId;
					if (rand() % 2) break;	// dao skill cho sinh dong
				}
			if (nSkillId > 0)
			{
				pNpc->SendCommand(do_skill, nSkillId, -1, nTarget);
				return;
			}
			// template khong co skill (npc thoai): thoi thi bam theo chu
		}
	}
	if (nDis > 96)
	{
		int nXGo = nOX + 32 - rand() % 64;
		int nYGo = nOY + 32 - rand() % 64;
		pNpc->SendCommand(do_run, nXGo, nYGo);
	}
}"""
if "keo ve ngay" in lf:
    print("B2. da co")
else:
    assert lf.count(cu_ai) == 1, "B2 anchor"
    lf = lf.replace(cu_ai, moi_ai, 1)
    print("B2. AI chien dau + bam")

# B3. ext passive trong sPetApplyAura
if "5139" not in lf:
    cu = """	KSkill* pSkill = (KSkill*)g_SkillManager.GetSkill(PET_AURA_SKILL0 + nKind - 1, nLevel);
	if (pSkill)
		pSkill->CastStateSkill(nOwnerNpc, 0, 0, PET_AURA_TIME, TRUE);
}"""
    moi = """	KSkill* pSkill = (KSkill*)g_SkillManager.GetSkill(PET_AURA_SKILL0 + nKind - 1, nLevel);
	if (pSkill)
		pSkill->CastStateSkill(nOwnerNpc, 0, 0, PET_AURA_TIME, TRUE);
	// [29/08] 4 ky nang BI DONG da hoc (task 5139..5142, bang 1670..1687
	// port tu VLTK) ap len PET, re-cast cung nhip aura
	int nPetNpc = s_nPetNpcIdx[nPlayerIdx];
	if (nPetNpc > 0 && nPetNpc < MAX_NPC)
	{
		for (int k = 0; k < 4; k++)
		{
			int nSk = sPetG(nPlayerIdx, 5139 + k);
			if (nSk <= 0) continue;
			KSkill* pExt = (KSkill*)g_SkillManager.GetSkill(nSk, 1);
			if (pExt)
				pExt->CastStateSkill(nPetNpc, 0, 0, PET_AURA_TIME, TRUE);
		}
	}
}"""
    assert lf.count(cu) == 1, "B3 anchor"
    lf = lf.replace(cu, moi, 1)
    print("B3. ext passive")
ghi_crlf(p, lf)

# ---------- C. UiPet 4 o ext ----------
p = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiPet.cpp"
lf = doc_lf(p)
if "5139" not in lf:
    cu = """    else
        m_Skill1.HoldObject(CGOG_NOTHING, 0, 0, 0);
}"""
    moi = """    else
        m_Skill1.HoldObject(CGOG_NOTHING, 0, 0, 0);

    // [29/08] 4 ky nang bi dong da hoc (task 5139..5142)
    for (i = 0; i < PET_UI_EXTSKILL_NUM && i < 4; i++)
    {
        int nSk = sPetTV(5139 + i);
        if (nSk > 0)
            m_ExtSkill[i].HoldObject(CGOG_SKILL_FIGHT, nSk, 1, 0);
        else
            m_ExtSkill[i].HoldObject(CGOG_NOTHING, 0, 0, 0);
    }
}"""
    assert lf.count(cu) == 1, "C anchor"
    lf = lf.replace(cu, moi, 1)
    ghi_crlf(p, lf)
    print("C. UiPet 4 o ext")
else:
    print("C. da co")

# ---------- D. item 4880 ----------
for p in (SV + r"\settings\item\magicscript.txt", CL + r"\settings\item\magicscript.txt"):
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    if "\t4880\t" in s:
        print("D. da co:", p[:2])
        continue
    lf2 = s.replace(CR + "\n", "\n")
    dong = None
    for d in lf2.split("\n"):
        c = d.split("\t")
        if len(c) > 9 and c[3] == "4874":
            dong = c[:]
            break
    assert dong, "khong thay khuon 4874"
    dong[0] = vn("Bí kíp Đồng Hành")
    dong[3] = "4880"
    dong[8] = vn("Dạy bạn đồng hành 1 kỹ năng bị động (chọn 1 trong 18)")
    dong[9] = BS * 2 + "script" + BS * 2 + "petsys" + BS * 2 + "bikip.lua"
    if not lf2.endswith("\n"):
        lf2 += "\n"
    lf2 += "\t".join(dong) + "\n"
    ghi_crlf(p, lf2)
    print("D. + item 4880:", p[:2])

# ---------- E. bikip.lua ----------
# trich ten 18 skill tu bang server (byte TCVN3 giu nguyen)
n = io.open(SV + r"\settings\skills.txt", "r", encoding="latin-1").read().replace(CR, "").split("\n")
ds = []
for d in n:
    c = d.split("\t")
    if len(c) > 5 and c[2].strip().isdigit() and 1670 <= int(c[2]) <= 1687:
        ds.append((int(c[2]), c[0].strip()))
assert len(ds) == 18, len(ds)
bang = ",\n".join('\t[%d] = "%s"' % (i, t) for i, t in ds)
noidung = "\n".join([
    'Include("' + BS * 2 + 'script' + BS * 2 + 'petsys' + BS * 2 + 'head.lua")',
    'Include("' + BS * 2 + 'script' + BS * 2 + 'petsys' + BS * 2 + 'lang.lua")',
    "",
    "-- [PETSYS 29/08] Bi kip Dong Hanh: day pet 1 trong 18 ky nang BI DONG",
    "-- (bang 1670..1687 port tu VLTK). Luu vao task 5139..5142 (4 o).",
    "BIKIP_SKILLS = {",
    bang,
    "}",
    "",
    "function main(nItemIndex)",
    "\tif PET_IsCreate() ~= 1 then",
    '\t\tTalk(1, "", "' + vn("Ngươi chưa có bạn đồng hành") + '")',
    "\t\treturn 1",
    "\tend",
    "\tif PET_GetLevel() < 21 then",
    '\t\tTalk(1, "", "' + vn("Bạn đồng hành đạt cấp 21 mới học được kỹ năng") + '")',
    "\t\treturn 1",
    "\tend",
    "\tlocal nSlot = 0",
    "\tlocal i",
    "\tfor i = 0, 3 do",
    "\t\tif GetTask(5139 + i) <= 0 then",
    "\t\t\tnSlot = 5139 + i",
    "\t\t\tbreak",
    "\t\tend",
    "\tend",
    "\tif nSlot == 0 then",
    '\t\tTalk(1, "", "' + vn("Bạn đồng hành đã học đủ 4 kỹ năng") + '")',
    "\t\treturn 1",
    "\tend",
    "\t-- so o duoc mo theo cap: (cap-21)/5+1, toi da 4",
    "\tlocal nChoPhep = floor((PET_GetLevel() - 21) / 5) + 1",
    "\tif nChoPhep > 4 then",
    "\t\tnChoPhep = 4",
    "\tend",
    "\tif nSlot - 5139 + 1 > nChoPhep then",
    '\t\tTalk(1, "", format("' + vn("Cấp hiện tại chỉ mở %d ô kỹ năng (5 cấp mở thêm 1 ô)") + '", nChoPhep))',
    "\t\treturn 1",
    "\tend",
    "\tlocal tbOpt = {}",
    "\tlocal nId",
    "\tfor nId = 1670, 1687 do",
    "\t\tlocal bDaHoc = 0",
    "\t\tfor i = 0, 3 do",
    "\t\t\tif GetTask(5139 + i) == nId then",
    "\t\t\t\tbDaHoc = 1",
    "\t\t\tend",
    "\t\tend",
    "\t\tif bDaHoc == 0 then",
    "\t\t\ttinsert(tbOpt, {BIKIP_SKILLS[nId], PetSys.HocBiKip, {PetSys, nId, nSlot, nItemIndex}})",
    "\t\tend",
    "\tend",
    "\ttinsert(tbOpt, {%CANCEL})",
    '\tCreateNewSayEx("' + vn("Chọn kỹ năng muốn dạy cho bạn đồng hành") + '", tbOpt)',
    "\treturn 1",
    "end",
    "",
    "function PetSys:HocBiKip(nSkillId, nSlot, nItemIndex)",
    "\tif GetTask(nSlot) > 0 then",
    "\t\treturn",
    "\tend",
    "\t-- tru 1 Bi kip (dem theo ma, bo level nhu thuoc)",
    "\tif CalcEquiproomItemCount(6, 1, 4880, -1) < 1 then",
    '\t\tTalk(1, "", "' + vn("Không còn Bí kíp trong hành trang") + '")',
    "\t\treturn",
    "\tend",
    "\tConsumeEquiproomItem(1, 6, 1, 4880)",
    "\tSetTask(nSlot, nSkillId)",
    '\tMsg2Player(format("' + vn("Bạn đồng hành đã học được kỹ năng [%s]") + '", BIKIP_SKILLS[nSkillId]))',
    "end",
    "",
])
ghi_crlf(SV + r"\script\petsys\bikip.lua", noidung)
print("E. ghi bikip.lua (18 skill)")

# reload trong lenh bai
p = SV + r"\script\item\lenhbaiadmin.lua"
lf = doc_lf(p)
if "bikip.lua" not in lf:
    neo = 'ReLoadScript("' + BS * 2 + 'script' + BS * 2 + 'petsys' + BS * 2 + 'xiuzhen.lua")'
    assert lf.count(neo) == 1
    lf = lf.replace(neo, neo + '\nReLoadScript("' + BS * 2 + 'script' + BS * 2 + 'petsys' + BS * 2 + 'bikip.lua")', 1)
    ghi_crlf(p, lf)
    print("E2. reload bikip")

# ---------- F. bdh_admin cap bi kip ----------
p = SV + r"\script\item\bdh_admin.lua"
lf = doc_lf(p)
if "BDH_P_BiKip" not in lf:
    i = lf.find("/BDH_P_ChanNguyen")
    assert i > 0
    dau = lf.rfind("\n", 0, i) + 1
    lf = lf[:dau] + '\t"Cap 5 Bi kip ky nang/BDH_P_BiKip",\n' + lf[dau:]
    them = "\n".join([
        "function BDH_P_BiKip()",
        "\tlocal i",
        "\tfor i = 1, 5 do",
        "\t\tAddItem(6, 1, 4880, 1, 0, 0)",
        "\tend",
        '\tMsg2Player("Da cap 5 Bi kip Dong Hanh")',
        "end",
        "",
    ])
    neo = "function BDH_P_ChanNguyen()"
    assert lf.count(neo) == 1
    lf = lf.replace(neo, them + neo, 1)
    ghi_crlf(p, lf)
    print("F. + cap Bi kip")
else:
    print("F. da co")
print("XONG p43")
