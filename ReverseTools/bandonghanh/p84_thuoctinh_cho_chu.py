# -*- coding: utf-8 -*-
r"""[PETSYS 30/08] Chu hoi: "thuoc tinh trang bi pet mac vao tac dung vao
ai - nguoi choi hay pet?"

Cang cu tu NGUON:
 - Pet ban goc KHONG DANH (dich nguoc jx_linux_y: KPet chi follow; VLTK
   cung khong co AI danh) -> cong chi so cho PET la vo nghia.
 - Chuoi goc G_STR_COMPANION_SUIT_ATTRIB: "Van Khoi Long Tuong: khi cap
   Van Khoi Long Tuong cao hon cap doi phuong thi sat thuong phong dai
   10%" -> ro rang la buff PVP cua NGUOI CHOI.
 - Cac thuoc tinh tren mon (sat thuong chieu, phan don, do don, khang...)
   deu la chi so chien dau cua NGUOI CHOI.
=> Ap thuoc tinh trang bi + thuoc tinh BO len CHU (giong 4 vong sang),
   khong ap len NPC pet nua.
"""
import io

CR = chr(13)
p = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerPet.cpp"
lf = io.open(p, "r", encoding="latin-1", newline="").read().replace(CR + "\n", "\n")

# 1. sPetApplyEquip: ap len CHU
cu = "static void sPetApplyEquip(int nPlayerIdx, int nNpcIdx)\n{\n\tif (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC) return;"
moi = """// [30/08] AP LEN CHU (khong phai pet): pet ban goc khong danh nen chi so
// tren pet vo nghia; chuoi goc G_STR_COMPANION_SUIT_ATTRIB mo ta ro day la
// buff PVP cua NGUOI CHOI.
static void sPetApplyEquip(int nPlayerIdx, int nNpcIdx)
{
	nNpcIdx = Player[nPlayerIdx].m_nIndex;   // ap len CHU
	if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC) return;"""
if "ap len CHU" in lf:
    print("1. da co")
else:
    assert lf.count(cu) == 1, lf.count(cu)
    lf = lf.replace(cu, moi, 1)
    print("1. thuoc tinh trang bi -> ap len CHU")

# 2. thuoc tinh BO: dang cong vao mau PET -> chuyen thanh cong cho CHU
cu2 = "\t\tint nHp = sPetG(nPlayerIdx, PET_TV_ATTRIB0 + 4) + sPetSuitAttrib(nPlayerIdx);\n"
moi2 = "\t\tint nHp = sPetG(nPlayerIdx, PET_TV_ATTRIB0 + 4);\n"
if cu2 in lf:
    lf = lf.replace(cu2, moi2, 1)
    print("2. mau pet khong con cong bonus bo")

# 3. trong sPetApplyAura: cong bonus bo (sinh luc) cho CHU
cu3 = "\tKSkill* pSkill = (KSkill*)g_SkillManager.GetSkill(PET_AURA_SKILL0 + nKind - 1, nLevel);"
moi3 = """	// [30/08] thuoc tinh BO trang bi: cong sinh luc toi da cho CHU
	{
		int nSuitHp = sPetSuitAttrib(nPlayerIdx);
		if (nSuitHp > 0)
		{
			KMagicAttrib sSuit;
			memset(&sSuit, 0, sizeof(sSuit));
			sSuit.nAttribType = magic_lifemax_yan_v;
			sSuit.nValue[0] = nSuitHp;
			Npc[nOwnerNpc].ModifyAttrib(0, &sSuit);
		}
	}
	KSkill* pSkill = (KSkill*)g_SkillManager.GetSkill(PET_AURA_SKILL0 + nKind - 1, nLevel);"""
if "thuoc tinh BO trang bi: cong sinh luc" not in lf:
    assert lf.count(cu3) == 1
    lf = lf.replace(cu3, moi3, 1)
    print("3. bonus bo -> cong cho CHU")

# 4. goi sPetApplyEquip trong Pet_Breathe (moi lan re-cast aura) de trang bi
#    doi la an ngay, khong doi goi lai pet
cu4 = "\t\t\ts_dwAuraTick[i] = 0;\n\t\t\tsPetApplyAura(i);"
moi4 = "\t\t\ts_dwAuraTick[i] = 0;\n\t\t\tsPetApplyAura(i);\n\t\t\tsPetApplyEquip(i, 0);\t// [30/08] thuoc tinh trang bi cho CHU"
if "sPetApplyEquip(i, 0);" not in lf:
    assert lf.count(cu4) == 1
    lf = lf.replace(cu4, moi4, 1)
    print("4. re-ap trang bi moi nhip aura")

io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))
print("XONG p84")
