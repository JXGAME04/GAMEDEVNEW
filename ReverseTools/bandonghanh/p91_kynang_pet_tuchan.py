# -*- coding: utf-8 -*-
r"""[PETKN 31/08] Che lai ky nang pet theo yeu cau chu:
 1. Ky nang MAC DINH 1600..1603 -> DON DANH vat ly (skills.txt doi sang
    attack skill khuon skill 53; dame theo cap trong aura.lua; C sPetFight
    do_skill bang no - va rieng o KPlayerPet.cpp).
 2. Ky nang THUOC TINH (18 bi kip 1670..1687) nang bang DIEM TU CHAN:
    toi da 30 cap, cap N -> N+1 ton N x 10 diem (menu op 8 xiuzhen.lua);
    cap luu o task 5166..5169 (di cap voi o id 5139..5142).
 3. Moi buff ky nang ap len PET, khong ap len nhan vat (va C).
Chay lai duoc (co marker [PETKN); sao luu .truoc_petkn lan dau.
"""
import io, os, shutil, sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn  # tieng Viet -> chuoi latin-1 mang byte TCVN3

SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
MARK = "[PETKN"


def doc(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def ghi(p, s):
    if not os.path.exists(p + ".truoc_petkn"):
        shutil.copy2(p, p + ".truoc_petkn")
    io.open(p, "w", encoding="latin-1", newline="").write(s)


# =========================================================================
# 1. skills.txt: 1600..1603 -> attack skill (khuon skill 53); 1670..1687
#    MaxLevel 5 -> 30. Ap cho CA server + client.
# =========================================================================
TEN_MOI = {  # (ten skill, mo ta) - chu co the doi lai trong skills.txt cot 1
    1600: (vn("Pet: Trảm Kích"), vn("Đòn đánh của Bạn Đồng Hành") + r"\n"),
    1601: (vn("Pet: Cuồng Kích"), vn("Đòn đánh của Bạn Đồng Hành (uy lực lớn)") + r"\n"),
    1602: (vn("Pet: Liệt Kích"), vn("Đòn đánh của Bạn Đồng Hành") + r"\n"),
    1603: (vn("Pet: Phá Kích"), vn("Đòn đánh của Bạn Đồng Hành") + r"\n"),
}
PROPERTY_MOI = vn("Công kích ngoại công")

# cot (0-based) -> gia tri; khuon tu dong skill 53 (chieu vat ly don muc
# tieu cua quai thuong): style 0 missles, dan line ChildSkillId 63, melee.
ATK_COLS = {
    4: "0",    # SkillStyle: 3 (state) -> 0 (Missles = danh thuong)
    9: "0", 10: "0", 11: "0", 12: "0", 13: "0",
    14: "75",  # AttackRadius
    15: "0", 16: "0", 17: "0", 18: "0",
    19: "1",   # MisslesForm: Line
    20: "63",  # ChildSkillId: dong missle cua skill 53
    21: "-1", 22: "1", 23: "0",
    24: "9",   # CharAnimId: hoat anh danh
    25: "0",
    26: "1",   # IsMelee
    27: "5",   # WaitTime
    29: "0", 30: "0", 31: "0", 32: "0", 33: "0",
    34: "1",   # IsPhysical
    35: "1",   # TargetOnly
    36: "1",   # TargetEnemy
    37: "0", 38: "0", 39: "0", 40: "0", 41: "0", 42: "0",
    43: "1",   # IsUseAR
    44: "0", 45: "0", 46: "0", 47: "0", 48: "0", 49: "0", 50: "0",
    51: "0", 52: "0",
    53: "1",
    54: "63",  # MaxLevel (cap skill = (cap pet+1)/2, tran 63)
    55: "-2", 56: "0",
    57: "80",  # DoHurt (nhu skill 53)
    58: "0", 59: "0", 61: "0",
    66: "0", 67: "0", 68: "0",
    69: "-1", 70: "1",
    # 71 LvlSetScript giu nguyen \script\skill\petsys\aura.lua
    72: "attackrating_v",     # do chinh xac theo cap (template pet AR ~100)
    74: "physicsdamage_v",    # dame min/max theo cap
}


def va_skills_txt(path):
    s = doc(path)
    eol = "\r\n" if "\r\n" in s[:2000] else "\n"
    lines = s.split(eol)
    n_atk = n_max = 0
    for idx, line in enumerate(lines):
        if not line:
            continue
        c = line.split("\t")
        if len(c) < 60:
            continue
        try:
            sid = int(c[2])
        except ValueError:
            continue
        if 1600 <= sid <= 1603:
            while len(c) < 114:
                c.append("")
            k = sid - 1600 + 1
            c[0] = TEN_MOI[sid][0]
            c[1] = PROPERTY_MOI
            for col, val in ATK_COLS.items():
                c[col] = val
            c[73] = "pet_aura_%d" % k   # LvlData1 (attackrating_v)
            c[75] = "pet_aura_%d" % k   # LvlData2 (physicsdamage_v)
            for col in range(76, 112):  # xoa attrib cu con sot
                c[col] = "0"
            c[112] = ""
            c[113] = TEN_MOI[sid][1]
            lines[idx] = "\t".join(c)
            n_atk += 1
        elif 1670 <= sid <= 1687:
            if c[54] != "30":
                c[54] = "30"        # MaxLevel 5 -> 30
                lines[idx] = "\t".join(c)
            n_max += 1
    assert n_atk == 4, "%s: chi thay %d/4 dong 1600..1603" % (path, n_atk)
    assert n_max == 18, "%s: chi thay %d/18 dong 1670..1687" % (path, n_max)
    ghi(path, eol.join(lines))
    print("  skills.txt OK (4 don danh + 18 MaxLevel=30):", path)


# =========================================================================
# 2. aura.lua: bang SKILLS -> duong cong DON DANH (ar + dame min/max).
# =========================================================================
AURA_MOI = """SKILLS={
-- [PETKN 31/08] 4 skill MAC DINH cua pet = DON DANH VAT LY (truoc la buff
-- len chu). Cap skill = (cap pet + 1)/2, toi da 63 (C: sPetSkillLevel).
-- attackrating_v: [1] = do chinh xac. physicsdamage_v: [1] = dame min,
-- [3] = dame max. DOI DAME PET CHI CAN SUA SO O DAY (server + client cung
-- sua roi "Nap lai script"/vao lai game, khong can build C).
-- pet_aura_2 (loai hiem 10%) dame cao hon ~20%.
	pet_aura_1={
		attackrating_v={ {{1,300},{63,5200}}, {{1,0},{63,0}}, {{1,0},{63,0}} },
		physicsdamage_v={ {{1,80},{63,4000}}, {{1,0},{63,0}}, {{1,120},{63,6000}} }
	},
	pet_aura_2={
		attackrating_v={ {{1,300},{63,5200}}, {{1,0},{63,0}}, {{1,0},{63,0}} },
		physicsdamage_v={ {{1,96},{63,4800}}, {{1,0},{63,0}}, {{1,144},{63,7200}} }
	},
	pet_aura_3={
		attackrating_v={ {{1,300},{63,5200}}, {{1,0},{63,0}}, {{1,0},{63,0}} },
		physicsdamage_v={ {{1,80},{63,4000}}, {{1,0},{63,0}}, {{1,120},{63,6000}} }
	},
	pet_aura_4={
		attackrating_v={ {{1,300},{63,5200}}, {{1,0},{63,0}}, {{1,0},{63,0}} },
		physicsdamage_v={ {{1,80},{63,4000}}, {{1,0},{63,0}}, {{1,120},{63,6000}} }
	},
}"""


def va_aura(path):
    s = doc(path)
    if MARK in s:
        print("  aura.lua da va roi, bo qua:", path)
        return
    i = s.find("SKILLS={")
    assert i >= 0, path
    j = s.find("\r\n}", i)
    if j < 0:
        j = s.find("\n}", i)
        assert j >= 0, path
        j += 2
    else:
        j += 3
    moi = AURA_MOI.replace("\n", "\r\n" if "\r\n" in s[:500] else "\n")
    ghi(path, s[:i] + moi + s[j:])
    print("  aura.lua OK:", path)


# =========================================================================
# 3. petskill.lua: keo dai 18 duong cong 20 -> 30 (noi tuyen tinh theo doan
#    cuoi; rieng 5 khang de bang 100 = kich tran).
# =========================================================================
DUOI = [
    ("{15,80},{16,83},{20,95}}",            "{15,80},{16,83},{20,95},{30,125}}", 2),
    ("{5,100},{15,300},{16,310},{20,350}}", "{5,100},{15,300},{16,310},{20,350},{30,450}}", 1),
    ("{10,20},{15,30},{16,31},{20,35}}",    "{10,20},{15,30},{16,31},{20,35},{30,45}}", 4),
    ("{10,10},{15,15},{16,16},{20,20}}",    "{10,10},{15,15},{16,16},{20,20},{30,30}}", 4),
    ("{10,4600},{15,6600},{16,6800},{20,8400}}", "{10,4600},{15,6600},{16,6800},{20,8400},{30,12400}}", 1),
    ("{10,2500},{15,3500},{16,3700},{20,4500}}", "{10,2500},{15,3500},{16,3700},{20,4500},{30,6500}}", 1),
    ("{10,50},{15,75},{16,80},{20,100}}",   "{10,50},{15,75},{16,80},{20,100},{30,100}}", 5),
]


def va_petskill(path):
    s = doc(path)
    if "{30," in s:
        print("  petskill.lua da va roi, bo qua:", path)
        return
    tong = 0
    for cu, moi, dem in DUOI:
        n = s.count(cu)
        assert n == dem, "%s: '%s' xuat hien %d lan (mong %d)" % (path, cu, n, dem)
        s = s.replace(cu, moi)
        tong += n
    assert tong == 18, tong
    s = s.replace(
        "-- bang SKILLS NGUYEN VAN tu petskill.lua VLTK (pak).",
        "-- bang SKILLS tu petskill.lua VLTK (pak); [PETKN 31/08] keo dai cap\r\n"
        "-- 20 -> 30 (nang bang diem Tu Chan) - diem {30,x} noi tuyen tinh doan\r\n"
        "-- cuoi, rieng 5 khang giu 100 (kich tran). Chinh so tai day duoc.",
        1)
    ghi(path, s)
    print("  petskill.lua OK (18 duong cong -> 30):", path)


# =========================================================================
# 4. head.lua: doi ten 4 loai skill mac dinh (hien o thong bao tao pet)
# =========================================================================
def va_head():
    p = os.path.join(SV, r"script\petsys\head.lua")
    s = doc(p)
    if MARK in s:
        print("  head.lua da va roi, bo qua")
        return
    cu = s[s.index("PetSys.tbPetSkill = {"):]
    cu = cu[:cu.index("}\r\n\r\n") + 1] if "}\r\n\r\n" in cu else cu[:cu.index("}\n\n") + 1]
    moi = (
        "PetSys.tbPetSkill = {\r\n"
        "\t-- [PETKN 31/08] 4 loai skill mac dinh gio la DON DANH (ten khop\r\n"
        "\t-- skills.txt 1600..1603); loai 2 hiem (10%) dame cao hon 20%.\r\n"
        '\t[1] ={szSkillName="' + vn("Trảm Kích") + '", nRate = 30},\r\n'
        '\t[2] ={szSkillName="' + vn("Cuồng Kích") + '",nRate = 10},\r\n'
        '\t[3] = {szSkillName="' + vn("Liệt Kích") + '",nRate = 30},\r\n'
        '\t[4] = {szSkillName="' + vn("Phá Kích") + '", nRate = 30},\r\n'
        "}"
    )
    ghi(p, s.replace(cu, moi, 1))
    print("  head.lua OK (4 ten skill)")


# =========================================================================
# 5. common.lua: hang so nang cap bi kip + don BIKIP_SKILLS ve day
# =========================================================================
def va_common_bikip():
    pc = os.path.join(SV, r"script\petsys\common.lua")
    pb = os.path.join(SV, r"script\petsys\bikip.lua")
    sc = doc(pc)
    sb = doc(pb)
    if MARK in sc:
        print("  common/bikip da va roi, bo qua")
        return
    # rut bang BIKIP_SKILLS tu bikip.lua
    i = sb.index("BIKIP_SKILLS = {")
    j = sb.index("}", i) + 1
    bang = sb[i:j]
    sb = (sb[:i]
          + "-- [PETKN 31/08] BIKIP_SKILLS chuyen sang common.lua (xiuzhen.lua dung chung)"
          + sb[j:])
    # bikip.lua: hoc xong dat cap 1 + ap buff len pet ngay
    cu = ('\tConsumeEquiproomItem(1, tbProp[1], tbProp[2], tbProp[3])\r\n'
          '\tSetTask(nSlot, nSkillId)\r\n')
    assert cu in sb, "bikip.lua: khong thay diem chen HocBiKip"
    sb = sb.replace(cu, cu.replace(
        "\tSetTask(nSlot, nSkillId)\r\n",
        "\tSetTask(nSlot, nSkillId)\r\n"
        "\tSetTask(BIKIP_LEVEL_TASK0 + nSlot - 5139, 1)\t-- [PETKN 31/08] cap khoi diem 1\r\n"
        "\tPET_SetSkill(1, GetTask(5124))\t-- [PETKN 31/08] ap buff len PET ngay\r\n"), 1)
    # common.lua: chen truoc tbPetSkillIDList
    moc = "tbPetSkillIDList ="
    assert moc in sc
    khoi = (
        "-- [PETKN 31/08] nang cap ky nang bi kip bang DIEM TU CHAN (chu chot):\r\n"
        "-- moi ky nang toi da 30 cap; cap N -> N+1 ton N x 10 diem Tu Chan;\r\n"
        "-- buff ky nang ap len PET (khong ap len nhan vat nua).\r\n"
        "BIKIP_LEVEL_TASK0 = 5166\t-- 4 o CAP di cap voi 4 o id 5139..5142\r\n"
        'BIKIP_MAX_LEVEL = BDH_CFG("BDH_KYNANG_BIKIP_CAP_TOI_DA", 30)\r\n'
        'BIKIP_TUCHAN_MOI_CAP = BDH_CFG("BDH_TUCHAN_MOI_CAP_KYNANG", 10)\r\n'
        "\r\n" + bang + "\r\n\r\n"
    )
    sc = sc.replace(moc, khoi + moc, 1)
    ghi(pb, sb)
    ghi(pc, sc)
    print("  common.lua + bikip.lua OK")


# =========================================================================
# 6. xiuzhen.lua: menu op 8 them nang cap ky nang bi kip
# =========================================================================
def va_xiuzhen():
    p = os.path.join(SV, r"script\petsys\xiuzhen.lua")
    s = doc(p)
    if MARK in s:
        print("  xiuzhen.lua da va roi, bo qua")
        return
    i = s.index("function PetSys:XiuzhenPointDlg()")
    j = s.index("function PetSys:XiuzhenConfirm()")
    moi = (
        "function PetSys:XiuzhenPointDlg()\r\n"
        "\tif PET_IsCreate() ~= 1 then\r\n"
        "\t\treturn\r\n"
        "\tend\r\n"
        "\tlocal nCo = GetTask(TASK_CHANGNGUYENDAN)\r\n"
        '\tPLOG("Xiuzhen: channguyen=" .. nCo .. " diem=" .. PET_GetXiuzhenPoint())\r\n'
        "\tlocal tbOpt = {}\r\n"
        "\tif PET_GetXiuzhenPoint() < XIUZHEN_MAX and nCo >= ZHENYUAN_RATE then\r\n"
        '\t\ttinsert(tbOpt, {format("' + vn("Đổi %d chân nguyên lấy 1 điểm Tu Chân") + '", ZHENYUAN_RATE), self.XiuzhenConfirm, {self}})\r\n'
        "\tend\r\n"
        "\t-- [PETKN 31/08] nang cap 4 ky nang bi kip bang diem Tu Chan:\r\n"
        "\t-- cap N -> N+1 ton N x BIKIP_TUCHAN_MOI_CAP diem, tran BIKIP_MAX_LEVEL\r\n"
        "\tlocal i\r\n"
        "\tfor i = 0, 3 do\r\n"
        "\t\tlocal nId = GetTask(5139 + i)\r\n"
        "\t\tif nId > 0 and BIKIP_SKILLS[nId] ~= nil then\r\n"
        "\t\t\tlocal nLv = GetTask(BIKIP_LEVEL_TASK0 + i)\r\n"
        "\t\t\tif nLv < 1 then\r\n"
        "\t\t\t\tnLv = 1\r\n"
        "\t\t\tend\r\n"
        "\t\t\tif nLv < BIKIP_MAX_LEVEL then\r\n"
        '\t\t\t\ttinsert(tbOpt, {format("' + vn("Nâng [%s] cấp %d lên %d (tốn %d điểm Tu Chân)") + '", BIKIP_SKILLS[nId], nLv, nLv + 1, nLv * BIKIP_TUCHAN_MOI_CAP), self.BiKipNangCap, {self, i}})\r\n'
        "\t\t\telse\r\n"
        '\t\t\t\ttinsert(tbOpt, {format("' + vn("[%s] đã đạt cấp tối đa %d") + '", BIKIP_SKILLS[nId], nLv)})\r\n'
        "\t\t\tend\r\n"
        "\t\tend\r\n"
        "\tend\r\n"
        "\ttinsert(tbOpt, {%CANCEL})\r\n"
        '\tCreateNewSayEx(format("' + vn("Điểm Tu Chân: %d - chân nguyên: %d") + '", PET_GetXiuzhenPoint(), nCo), tbOpt)\r\n'
        "end\r\n"
        "\r\n"
        "-- [PETKN 31/08] nO = 0..3 (o bi kip)\r\n"
        "function PetSys:BiKipNangCap(nO)\r\n"
        "\tlocal nId = GetTask(5139 + nO)\r\n"
        "\tif nId <= 0 or BIKIP_SKILLS[nId] == nil then\r\n"
        "\t\treturn\r\n"
        "\tend\r\n"
        "\tlocal nLv = GetTask(BIKIP_LEVEL_TASK0 + nO)\r\n"
        "\tif nLv < 1 then\r\n"
        "\t\tnLv = 1\r\n"
        "\tend\r\n"
        "\tif nLv >= BIKIP_MAX_LEVEL then\r\n"
        "\t\treturn\r\n"
        "\tend\r\n"
        "\tlocal nGia = nLv * BIKIP_TUCHAN_MOI_CAP\r\n"
        "\tif PET_GetXiuzhenPoint() < nGia then\r\n"
        '\t\tTalk(1, "", format("' + vn("Cần %d điểm Tu Chân để nâng cấp (ngươi đang có %d)") + '", nGia, PET_GetXiuzhenPoint()))\r\n'
        "\t\treturn\r\n"
        "\tend\r\n"
        "\tPET_SetXiuzhenPoint(PET_GetXiuzhenPoint() - nGia)\r\n"
        "\tSetTask(BIKIP_LEVEL_TASK0 + nO, nLv + 1)\r\n"
        "\tPET_SetSkill(1, GetTask(5124))\t-- ap lai buff tren PET ngay\r\n"
        '\tPLOG("BiKipNangCap: o=" .. nO .. " skill=" .. nId .. " cap=" .. (nLv + 1) .. " tru=" .. nGia)\r\n'
        '\tMsg2Player(format("' + vn("Đã nâng [%s] lên cấp %d, điểm Tu Chân còn %d") + '", BIKIP_SKILLS[nId], nLv + 1, PET_GetXiuzhenPoint()))\r\n'
        "end\r\n"
        "\r\n"
    )
    ghi(p, s[:i] + moi + s[j:])
    print("  xiuzhen.lua OK (menu nang cap)")


# =========================================================================
# 7. bdh_admin.lua: XoaKN don ca 4 o cap
# =========================================================================
def va_admin():
    p = os.path.join(SV, r"script\item\bdh_admin.lua")
    s = doc(p)
    if "5166 + i" in s:
        print("  bdh_admin.lua da va roi, bo qua")
        return
    cu = "\tfor i = 0, 3 do\r\n\t\tSetTask(5139 + i, 0)\r\n\tend\r\n"
    assert cu in s, "bdh_admin.lua: khong thay khoi XoaKN"
    s = s.replace(cu,
        "\tfor i = 0, 3 do\r\n\t\tSetTask(5139 + i, 0)\r\n"
        "\t\tSetTask(5166 + i, 0)\t-- [PETKN 31/08] don ca o cap\r\n\tend\r\n", 1)
    ghi(p, s)
    print("  bdh_admin.lua OK (XoaKN don o cap)")


# =========================================================================
# 8. ch_chung.lua: 2 khoa cau hinh moi
# =========================================================================
def va_chchung():
    p = os.path.join(SV, r"script\cauhinh\ch_chung.lua")
    s = doc(p)
    if "BDH_KYNANG_BIKIP_CAP_TOI_DA" in s:
        print("  ch_chung.lua da va roi, bo qua")
        return
    moc = "BDH_BUOC_CAP_MO_O_KYNANG  = 5           ,\t-- cu moi bao nhieu cap thi mo them mot o ky nang\r\n"
    assert moc in s, "ch_chung.lua: khong thay moc BDH_BUOC_CAP_MO_O_KYNANG"
    s = s.replace(moc, moc +
        "BDH_KYNANG_BIKIP_CAP_TOI_DA= 30         ,\t-- tran cap MOI ky nang bi kip (nang bang diem Tu Chan)\r\n"
        "BDH_TUCHAN_MOI_CAP_KYNANG = 10          ,\t-- ky nang cap N len N+1 ton N x (so nay) diem Tu Chan\r\n", 1)
    ghi(p, s)
    print("  ch_chung.lua OK (2 khoa)")


# =========================================================================
if __name__ == "__main__":
    print("== p91: skills.txt ==")
    va_skills_txt(os.path.join(SV, r"settings\skills.txt"))
    va_skills_txt(os.path.join(CL, r"settings\skills.txt"))
    print("== p91: aura.lua ==")
    va_aura(os.path.join(SV, r"script\skill\petsys\aura.lua"))
    va_aura(os.path.join(CL, r"script\skill\petsys\aura.lua"))
    print("== p91: petskill.lua ==")
    va_petskill(os.path.join(SV, r"script\skill\petskill.lua"))
    va_petskill(os.path.join(CL, r"script\skill\petskill.lua"))
    print("== p91: script petsys ==")
    va_head()
    va_common_bikip()
    va_xiuzhen()
    va_admin()
    va_chchung()
    print("== p91 XONG ==")
