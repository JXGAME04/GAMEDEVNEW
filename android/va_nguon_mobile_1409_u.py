# -*- coding: latin-1 -*-
r"""[NENNGOAI 14/09] Vung KHONG CO du lieu nen -> to o nen mac dinh thay vi de DEN.

TRIEU CHUNG (chu gui anh chup 23:4x, ban do Tien Cuc dong): mot mang vuong DEN o goc tren-trai
man hinh, dung yen bao lau cung khong hien, nhan vat / cay / da van ve binh thuong de len.

DA TRA TAN DU LIEU (khong doan):
  - Moi dong [PGND-V] cua phien do deu "bo 0" -> khong o nen nao bi bo khi ghep. Nen KHONG phai
    thieu tep anh (bug do da va bang \system\spr\RegionTileDefault.spr, muc 10.12).
  - Doc luoi vung that cua map 93 tu pak: cum day 107 vung (x 94..109, y 93..104), ria cum RANG CUA
    - goc tren-trai KHONG CO tep vung nao. Dung cho mang den trong anh.
  - KScenePlaceRegionC::Load: mo khong duoc tep vung thi VAN dat m_Status = REGION_S_STANDBY, nen
    vung do VAN vao process area va VAN duoc cap mot anh nen 512x512.
  - PrerenderGround: xoa anh do ve DEN (ClearImageData) roi... khong ve gi ca, vi uNumGrunode = 0
    (`if (nNum) JX_NEN_VE(nNum)` khong chay). => anh nen giu nguyen mau den, vinh vien.
  - Khop nhat ky: [PGND-X] co ghi "xoa nen _*PlaceGround*_#~24~#_ / #~33~#_" nhung [PGND-V] KHONG
    co dong nao cho hai o do - dung la "xoa xong roi khong ghep gi".

VI SAO BAN PC KHONG THAY: khung ve PC 800x600, mobile 1040x936 (gap 2,03 lan dien tich) nen mobile
nhin xa hon nua vung moi ben va cham toi ria du lieu ban do, cho PC hiem khi thay.

CACH SUA: vung khong co lop nen thi TO DAY bang o nen mac dinh (8x8 o 64x64 = 512x512), di qua DUNG
duong ghep cu (DrawPrimitivesOnImage) nen lien mach voi vung ben canh, khong co duong noi.
Rao #ifdef JX_MOBILE: ban PC giu nguyen tung dong. Tat bang [Client] NenNgoaiBanDo=0 trong Config.ini.

Chay:  PYTHONIOENCODING=latin-1 python android\va_nguon_mobile_1409_u.py
"""
import io
import os

GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TAG = "[NENNGOAI 14/09]"
DUONG = os.path.join("Sources", "Core", "Src", "Scene", "KScenePlaceRegionC.cpp")

# --- 1. cho du cho 64 o khi vung RONG (uJxDem dang la 1)
CU1 = "\tconst unsigned uJxDem = uJxTong ? (uJxTong <= uJxTran ? uJxTong : uJxTran) : 1;\r\n"
MOI1 = ("\t// [NENNGOAI 14/09] vung RONG (khong co lop nen): can 8x8 = 64 cho de to o nen mac dinh, khong phai 1.\r\n"
        "\tconst unsigned uJxDem = uJxTong ? (uJxTong <= uJxTran ? uJxTong : uJxTran) : (unsigned)(JX_NENNGOAI_CANH * JX_NENNGOAI_CANH);\r\n")

# --- 2. hang so + ten tep + cong tac, dat ngay truoc ham PrerenderGround
CU2 = "bool KScenePlaceRegionC::PrerenderGround(bool bForce)\r\n"
MOI2 = ("#ifdef JX_MOBILE\r\n"
        "// [NENNGOAI 14/09] Vung khong co du lieu nen (ria ngoai luoi ban do) truoc day bi xoa den roi de nguyen.\r\n"
        "// To day bang o nen mac dinh - CHINH tep ma trinh soan ban do dat cho o chua ve, nen mau khop voi\r\n"
        "// phan ria ma chinh ban do da to san. 8x8 o 64x64 phu kin anh nen 512x512 cua mot vung.\r\n"
        "// Tat: [Client] NenNgoaiBanDo=0 trong Config.ini (doc mot lan).\r\n"
        "#define JX_NENNGOAI_CANH 8\r\n"
        "static const char JX_NENNGOAI_TEP[] = \"\\\\system\\\\spr\\\\RegionTileDefault.spr\";\r\n"
        "static int g_nJxNenNgoai = -1;\r\n"
        "static bool JxNenNgoaiBat()\r\n"
        "{\r\n"
        "\tif (g_nJxNenNgoai < 0)\r\n"
        "\t\tg_nJxNenNgoai = GetPrivateProfileIntA(\"Client\", \"NenNgoaiBanDo\", 1, \".\\\\Config.ini\") ? 1 : 0;\r\n"
        "\treturn g_nJxNenNgoai != 0;\r\n"
        "}\r\n"
        "#endif\r\n"
        "bool KScenePlaceRegionC::PrerenderGround(bool bForce)\r\n")

# --- 3. to day khi vung rong, ngay sau khi dat pGi = &ImgList[0]
CU3 = ("\tint\t\t\tnNum = 0;\r\n"
       "\tpGi = &ImgList[0];\r\n"
       "\r\n"
       "\t//--------\r\n"
       "\tKSPRCrunode* pGrunode = m_GroundLayerData.pGrunodes;\r\n")
MOI3 = ("\tint\t\t\tnNum = 0;\r\n"
        "\tpGi = &ImgList[0];\r\n"
        "\r\n"
        "#ifdef JX_MOBILE\r\n"
        "\t// [NENNGOAI 14/09] Vung khong co lop nen: to day o nen mac dinh thay vi de den.\r\n"
        "\t// Hai vong duoi chay 0 lan (uNumGrunode = uNumObject = 0), nen 64 muc nay se duoc\r\n"
        "\t// ve boi dung mot loi goi JX_NEN_VE(nNum) o cuoi ham - cung duong ghep nhu vung binh thuong.\r\n"
        "\t// Dieu kien la uJxTong == 0 chu KHONG phai uNumGrunode == 0: chi luc do nJxMaxImg moi bang 64.\r\n"
        "\t// Vung co vat the ma khong co lop nen (hiem) thi nJxMaxImg = so vat the, co the < 64 -> tran mang.\r\n"
        "\tif (uJxTong == 0 && JxNenNgoaiBat())\r\n"
        "\t{\r\n"
        "\t\tconst size_t uJxLen = sizeof(JX_NENNGOAI_TEP) - 1;\r\n"
        "\t\tfor (int jy = 0; jy < JX_NENNGOAI_CANH; jy++)\r\n"
        "\t\t{\r\n"
        "\t\t\tfor (int jx = 0; jx < JX_NENNGOAI_CANH; jx++, pGi++, nNum++)\r\n"
        "\t\t\t{\r\n"
        "\t\t\t\tpGi->bRenderStyle = IMAGE_RENDER_STYLE_OPACITY;\r\n"
        "\t\t\t\tpGi->nType = ISI_T_SPR;\r\n"
        "\t\t\t\tpGi->oPosition.nX = (jx * 2) * CellWidth;\r\n"
        "\t\t\t\tpGi->oPosition.nY = (jy * 2) * CellHeight;\r\n"
        "\t\t\t\tmemcpy(pGi->szImage, JX_NENNGOAI_TEP, uJxLen);\r\n"
        "\t\t\t\tpGi->szImage[uJxLen] = 0;\r\n"
        "\t\t\t\tpGi->nFrame = 0;\r\n"
        "\t\t\t\tpGi->uImage = 0;\r\n"
        "\t\t\t\tpGi->nISPosition = IMAGE_IS_POSITION_INIT;\r\n"
        "\t\t\t}\r\n"
        "\t\t}\r\n"
        "\t}\r\n"
        "#endif\r\n"
        "\t//--------\r\n"
        "\tKSPRCrunode* pGrunode = m_GroundLayerData.pGrunodes;\r\n")

p = os.path.join(GOC, DUONG)
s = io.open(p, "r", encoding="latin-1", newline="").read()
cao0 = sum(1 for c in s if ord(c) >= 128)
lf0 = s.count("\n") - s.count("\r\n")
if TAG in s:
    print("bo qua (da co %s)" % TAG)
else:
    for cu, moi in ((CU1, MOI1), (CU2, MOI2), (CU3, MOI3)):
        if s.count(cu) != 1:
            raise SystemExit("HONG: neo xuat hien %d lan:\n%s" % (s.count(cu), cu[:160]))
        s = s.replace(cu, moi, 1)
    cao1 = sum(1 for c in s if ord(c) >= 128)
    lf1 = s.count("\n") - s.count("\r\n")
    if cao1 != cao0:
        raise SystemExit("HONG: byte cao %d -> %d" % (cao0, cao1))
    if lf1 != lf0:
        raise SystemExit("HONG: %d dong LF don (truoc %d)" % (lf1, lf0))
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("da va: %s" % DUONG)
print("xong.")
