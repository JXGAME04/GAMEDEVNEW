# -*- coding: utf-8 -*-
r"""[PETSYS 29/08] HE TRANG BI PET V1 (muc 7 - "trang bi van chua thay"):
Khong co nguon server private -> thiet ke toi gian KHOP UI + khuon JX1
(menu hop thoai nhu moi he), ghi ro cho de doi sau.

O TASK: 5143..5148 = particular item 6 o (Vu khi/Non/Ao/Dai/Giay/Phu =
4881..4886); 5150..5155 = phan tram pham chat 80..120 (duc lai re-roll);
5157/5158 = TONG bonus HP/MP (lua tinh -> C doc ap vao mau khi summon);
5160..5165 = pct TAM khi duc thu.

1. 6 item 4881..4886 (bang server+client, khuon 4874, Script=petequip.lua)
2. script\petsys\petequip.lua: deo (dung item -> vao o, tra do cu),
   EquipRebuildDlg (op 10): chon o -> duc thu (5 xu/lan) -> nhan/duc tiep
3. head map [10], protocol Include, lenhbai reload, bdh_admin cap do
4. C server: mau summon += bonus (5157/5158)
5. C client UiPet: 6 o ve anh item theo bang; nut "Duc lai" -> SendOp(10)
"""
import io
import sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn  # noqa: E402

CR = chr(13)
BS = chr(92)
SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"

TEN6 = [vn("Vũ khí Đồng Hành"), vn("Nón Đồng Hành"), vn("Y phục Đồng Hành"),
        vn("Đai Đồng Hành"), vn("Giày Đồng Hành"), vn("Hộ Phù Đồng Hành")]
INTRO6 = vn("Trang bị cho bạn đồng hành (dùng là đeo vào ô tương ứng)")


def doc_lf(p):
    return io.open(p, "r", encoding="latin-1", newline="").read().replace(CR + "\n", "\n")


def ghi_crlf(p, lf):
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))


# ---------- 1. bang item ----------
def tim_anh(bang_lf, tukhoa):
    for d in bang_lf.split("\n"):
        c = d.split("\t")
        if len(c) > 4:
            for t in tukhoa:
                if t in c[4].lower():
                    return c[4]
    return None


for pth in (SV + r"\settings\item\magicscript.txt", CL + r"\settings\item\magicscript.txt"):
    lf = doc_lf(pth)
    if "\t4881\t" in lf:
        print("1. da co:", pth[:2])
        continue
    khuon = None
    for d in lf.split("\n"):
        c = d.split("\t")
        if len(c) > 9 and c[3] == "4874":
            khuon = c[:]
            break
    assert khuon
    anh6 = [tim_anh(lf, ["sword", "dao0", "blade"]) or khuon[4],
            tim_anh(lf, ["hat", "helm", "mao"]) or khuon[4],
            tim_anh(lf, ["armor", "yifu", "clothes"]) or khuon[4],
            tim_anh(lf, ["belt", "yaodai"]) or khuon[4],
            tim_anh(lf, ["boot", "shoe", "xie"]) or khuon[4],
            tim_anh(lf, ["amulet", "hufu", "lingpai"]) or khuon[4]]
    them = []
    for k in range(6):
        c = khuon[:]
        c[0] = TEN6[k]
        c[3] = str(4881 + k)
        c[4] = anh6[k]
        c[8] = INTRO6
        c[9] = BS + "script" + BS + "petsys" + BS + "petequip.lua"
        them.append("\t".join(c))
    if not lf.endswith("\n"):
        lf += "\n"
    lf += "\n".join(them) + "\n"
    ghi_crlf(pth, lf)
    print("1. + 6 item 4881..4886:", pth[:2], "| anh vd:", anh6[0][:40])

# ---------- 2. petequip.lua ----------
noidung = "\n".join([
    'Include("' + BS * 2 + 'script' + BS * 2 + 'petsys' + BS * 2 + 'head.lua")',
    'Include("' + BS * 2 + 'script' + BS * 2 + 'petsys' + BS * 2 + 'lang.lua")',
    "",
    "-- [PETSYS 29/08] TRANG BI PET V1 - thiet ke toi gian khop UI (khong co",
    "-- nguon server private). O: 5143..5148 particular; 5150..5155 pct 80..120;",
    "-- 5157/5158 tong bonus HP/MP; 5160..5165 pct tam khi duc thu.",
    "PETEQUIP_GOC = {",
    "\t[4881] = {nSlot = 1, nHp = 500, nMp = 0},",
    "\t[4882] = {nSlot = 2, nHp = 300, nMp = 0},",
    "\t[4883] = {nSlot = 3, nHp = 400, nMp = 0},",
    "\t[4884] = {nSlot = 4, nHp = 0, nMp = 200},",
    "\t[4885] = {nSlot = 5, nHp = 200, nMp = 100},",
    "\t[4886] = {nSlot = 6, nHp = 0, nMp = 300},",
    "}",
    'PETEQUIP_TEN = {"' + vn("Vũ khí") + '", "' + vn("Nón") + '", "' + vn("Y phục") + '", "'
    + vn("Đai") + '", "' + vn("Giày") + '", "' + vn("Hộ Phù") + '"}',
    "DUC_PHI_XU = 5",
    "",
    "function PetEquip_TinhBonus()",
    "\tlocal nHp = 0",
    "\tlocal nMp = 0",
    "\tlocal i",
    "\tfor i = 1, 6 do",
    "\t\tlocal nP = GetTask(5143 + i - 1)",
    "\t\tlocal tb = PETEQUIP_GOC[nP]",
    "\t\tif tb then",
    "\t\t\tlocal nPct = GetTask(5150 + i - 1)",
    "\t\t\tif nPct < 80 or nPct > 120 then",
    "\t\t\t\tnPct = 100",
    "\t\t\tend",
    "\t\t\tnHp = nHp + floor(tb.nHp * nPct / 100)",
    "\t\t\tnMp = nMp + floor(tb.nMp * nPct / 100)",
    "\t\tend",
    "\tend",
    "\tSetTask(5157, nHp)",
    "\tSetTask(5158, nMp)",
    "end",
    "",
    "function main(nItemIndex)",
    "\tif PET_IsCreate() ~= 1 then",
    '\t\tTalk(1, "", "' + vn("Ngươi chưa có bạn đồng hành") + '")',
    "\t\treturn 1",
    "\tend",
    "\tlocal nG, nD, nP = GetItemProp(nItemIndex)",
    "\tlocal tb = PETEQUIP_GOC[nP]",
    "\tif not tb then",
    "\t\treturn 1",
    "\tend",
    "\tlocal nO = 5143 + tb.nSlot - 1",
    "\tlocal nCu = GetTask(nO)",
    "\tif nCu > 0 then",
    "\t\tAddItem(6, 1, nCu, 1, 0, 0)\t-- tra do cu ve tui",
    "\tend",
    "\tSetTask(nO, nP)",
    "\tSetTask(5150 + tb.nSlot - 1, 100)\t-- pham chat goc 100%",
    "\tPetEquip_TinhBonus()",
    '\tMsg2Player(format("' + vn("Đã trang bị [%s] cho bạn đồng hành") + '", PETEQUIP_TEN[tb.nSlot]))',
    '\tPLOG("PetEquip: deo slot=" .. tb.nSlot .. " p=" .. nP)',
    "\treturn",
    "end",
    "",
    "-- op 10: cua so duc lai (menu hop thoai server nhu moi he JX1)",
    "function PetSys:EquipRebuildDlg()",
    "\tif PET_IsCreate() ~= 1 then",
    "\t\treturn",
    "\tend",
    "\tlocal tbOpt = {}",
    "\tlocal i",
    "\tfor i = 1, 6 do",
    "\t\tlocal nP = GetTask(5143 + i - 1)",
    "\t\tif PETEQUIP_GOC[nP] then",
    '\t\t\ttinsert(tbOpt, {format("%s - ' + vn("phẩm chất") + ' %d%%", PETEQUIP_TEN[i], GetTask(5150 + i - 1)), PetSys.EquipTry, {PetSys, i}})',
    "\t\tend",
    "\tend",
    "\tif getn(tbOpt) == 0 then",
    '\t\tTalk(1, "", "' + vn("Bạn đồng hành chưa đeo trang bị nào (dùng item Trang Bị Đồng Hành để đeo)") + '")',
    "\t\treturn",
    "\tend",
    "\ttinsert(tbOpt, {%CANCEL})",
    '\tCreateNewSayEx(format("' + vn("Chọn trang bị muốn đúc lại (phí %d xu/lần)") + '", DUC_PHI_XU), tbOpt)',
    "end",
    "",
    "function PetSys:EquipTry(nSlot)",
    "\tif GetTask(251) < DUC_PHI_XU then",
    '\t\tTalk(1, "", format("' + vn("Không đủ %d xu") + '", DUC_PHI_XU))',
    "\t\treturn",
    "\tend",
    "\tSetTask(251, GetTask(251) - DUC_PHI_XU)",
    "\tlocal nMoi = random(80, 120)",
    "\tSetTask(5160 + nSlot - 1, nMoi)",
    '\tPLOG("PetEquip: duc thu slot=" .. nSlot .. " pct=" .. nMoi)',
    "\tlocal tbOpt = {}",
    '\ttinsert(tbOpt, {format("' + vn("Nhận phẩm chất mới %d%%") + '", nMoi), PetSys.EquipAccept, {PetSys, nSlot}})',
    '\ttinsert(tbOpt, {format("' + vn("Đúc tiếp (thêm %d xu)") + '", DUC_PHI_XU), PetSys.EquipTry, {PetSys, nSlot}})',
    '\ttinsert(tbOpt, {"' + vn("Giữ phẩm chất cũ") + '"})',
    '\tCreateNewSayEx(format("' + vn("Kết quả đúc: %d%% (hiện tại %d%%)") + '", nMoi, GetTask(5150 + nSlot - 1)), tbOpt)',
    "end",
    "",
    "function PetSys:EquipAccept(nSlot)",
    "\tlocal nMoi = GetTask(5160 + nSlot - 1)",
    "\tif nMoi < 80 or nMoi > 120 then",
    "\t\treturn",
    "\tend",
    "\tSetTask(5150 + nSlot - 1, nMoi)",
    "\tSetTask(5160 + nSlot - 1, 0)",
    "\tPetEquip_TinhBonus()",
    '\tMsg2Player(format("' + vn("Đúc thành công! %s phẩm chất %d%%") + '", PETEQUIP_TEN[nSlot], nMoi))',
    "end",
    "",
])
ghi_crlf(SV + r"\script\petsys\petequip.lua", noidung)
print("2. petequip.lua")

# ---------- 3. head/protocol/lenhbai/bdh_admin ----------
p = SV + r"\script\petsys\head.lua"
lf = doc_lf(p)
if "EquipRebuildDlg" not in lf:
    neo = '\t[PET_OPERATION_XIUZHEN_POINT] = "XiuzhenPointDlg",\n'
    assert lf.count(neo) == 1
    lf = lf.replace(neo, neo + '\t[10] = "EquipRebuildDlg",\t-- [29/08] trang bi pet (nut Duc lai)\n', 1)
    ghi_crlf(p, lf)
    print("3a. head [10]")
p = SV + r"\script\petsys\protocol_process_gs.lua"
lf = doc_lf(p)
if "petequip" not in lf:
    neo = 'Include("' + BS * 2 + 'script' + BS * 2 + 'petsys' + BS * 2 + 'xiuzhen.lua")\n'
    assert lf.count(neo) == 1
    lf = lf.replace(neo, neo + 'Include("' + BS * 2 + 'script' + BS * 2 + 'petsys' + BS * 2 + 'petequip.lua")\n', 1)
    ghi_crlf(p, lf)
    print("3b. protocol Include petequip")
p = SV + r"\script\item\lenhbaiadmin.lua"
lf = doc_lf(p)
if "petequip" not in lf:
    neo = 'ReLoadScript("' + BS * 2 + 'script' + BS * 2 + 'petsys' + BS * 2 + 'bikip.lua")'
    assert lf.count(neo) == 1
    lf = lf.replace(neo, neo + '\nReLoadScript("' + BS * 2 + 'script' + BS * 2 + 'petsys' + BS * 2 + 'petequip.lua")', 1)
    ghi_crlf(p, lf)
    print("3c. lenhbai reload petequip")
p = SV + r"\script\item\bdh_admin.lua"
lf = doc_lf(p)
if "BDH_P_TrangBi" not in lf:
    i = lf.find("/BDH_P_BiKip")
    dau = lf.rfind("\n", 0, i) + 1
    lf = lf[:dau] + '\t"Cap 6 trang bi pet/BDH_P_TrangBi",\n' + lf[dau:]
    them = "\n".join([
        "function BDH_P_TrangBi()",
        "\tlocal i",
        "\tfor i = 4881, 4886 do",
        "\t\tAddItem(6, 1, i, 1, 0, 0)",
        "\tend",
        '\tMsg2Player("Da cap 6 trang bi Dong Hanh")',
        "end",
        "",
    ])
    neo = "function BDH_P_BiKip()"
    assert lf.count(neo) == 1
    lf = lf.replace(neo, them + neo, 1)
    ghi_crlf(p, lf)
    print("3d. bdh_admin cap trang bi")

# ---------- 4. C server mau bonus ----------
p = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerPet.cpp"
lf = doc_lf(p)
if "5157" not in lf:
    cu = "\t\tint nHp = sPetG(nPlayerIdx, PET_TV_ATTRIB0 + 4);\n"
    moi = ("\t\t// [29/08] + bonus trang bi pet (petequip.lua tinh tong vao 5157/5158)\n"
           "\t\tint nHp = sPetG(nPlayerIdx, PET_TV_ATTRIB0 + 4) + sPetG(nPlayerIdx, 5157);\n")
    assert lf.count(cu) == 1
    lf = lf.replace(cu, moi, 1)
    cu2 = "\t\tint nMp = sPetG(nPlayerIdx, PET_TV_ATTRIB0 + 5);\n"
    moi2 = "\t\tint nMp = sPetG(nPlayerIdx, PET_TV_ATTRIB0 + 5) + sPetG(nPlayerIdx, 5158);\n"
    assert lf.count(cu2) == 1
    lf = lf.replace(cu2, moi2, 1)
    ghi_crlf(p, lf)
    print("4. C mau bonus equip")
else:
    print("4. da co")

# ---------- 5. C client: 6 o anh + nut Duc lai ----------
p = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiPet.cpp"
lf = doc_lf(p)
if "sPetItemImg" not in lf:
    # ham tra anh item theo particular (bang client)
    neo = "static void sPetResPath("
    i = lf.find(neo)
    assert i > 0
    them = (
        "// [29/08] tra ImageName cua item theo ParticularType (bang magicscript client)\n"
        "static void sPetItemImg(int nParticular, char* szOut, int nOutLen)\n"
        "{\n"
        "    szOut[0] = 0;\n"
        "    static KTabFile s_ItemTab;\n"
        "    static int s_bLoaded = 0;\n"
        "    if (!s_bLoaded)\n"
        "    {\n"
        "        s_bLoaded = 1;\n"
        "        s_ItemTab.Load((LPSTR)\"\\\\settings\\\\item\\\\magicscript.txt\");\n"
        "    }\n"
        "    char szNum[16];\n"
        "    int nRow = s_ItemTab.GetHeight();\n"
        "    for (int r = 2; r <= nRow; r++)\n"
        "    {\n"
        "        s_ItemTab.GetString(r, 4, (LPSTR)\"\", szNum, sizeof(szNum));\n"
        "        if (atoi(szNum) == nParticular)\n"
        "        {\n"
        "            s_ItemTab.GetString(r, 5, (LPSTR)\"\", szOut, nOutLen);\n"
        "            return;\n"
        "        }\n"
        "    }\n"
        "}\n\n")
    lf = lf[:i] + them + lf[i:]
    # UpdateData: 6 o
    cu5 = """    for (i = 0; i < PET_UI_EXTSKILL_NUM && i < 4; i++)"""
    assert lf.count(cu5) == 1
    them5 = (
        "    // [29/08] 6 o trang bi (task 5143..5148 = particular item)\n"
        "    for (i = 0; i < PET_UI_EQUIP_NUM && i < 6; i++)\n"
        "    {\n"
        "        int nP = sPetTV(5143 + i);\n"
        "        char szImg[128];\n"
        "        szImg[0] = 0;\n"
        "        if (nP > 0)\n"
        "            sPetItemImg(nP, szImg, sizeof(szImg));\n"
        "        if (szImg[0])\n"
        "        {\n"
        "            m_Equip[i].SetImage(ISI_T_SPR, szImg);\n"
        "            m_Equip[i].Show();\n"
        "        }\n"
        "    }\n\n")
    lf = lf.replace(cu5, them5 + cu5, 1)
    # nut Duc lai
    cu6 = '        // m_CompanionBtn ("Duc lai") - he trang bi pet chua mo (dot sau)'
    moi6 = ("        if (uParam == (unsigned int)(KWndWindow*)&m_CompanionBtn)\n"
            "            SendOp(10);\t// [29/08] mo menu duc lai trang bi (server)")
    assert lf.count(cu6) == 1
    lf = lf.replace(cu6, moi6, 1)
    ghi_crlf(p, lf)
    print("5. UiPet 6 o anh + nut Duc lai")
else:
    print("5. da co")
print("XONG p48")
