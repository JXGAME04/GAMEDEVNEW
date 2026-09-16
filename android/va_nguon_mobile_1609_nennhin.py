# -*- coding: latin-1 -*-
r"""[NENNHIN 16/09] Nen den + chop man khi di chuyen tren mobile: vung DANG TREN MAN HINH khong bao gio duoc ve bang duong bo o.

NGUYEN NHAN GOC (doc ma + log Fold 7 13-16/09, khong phai du lieu ban do):
  PC: KScenePlaceRegionC::PaintGround ve anh nen da ghep (GROUND_IMG_OK_FLAG); chua ghep thi PaintGroundDirect ve tung o.
      Hai duong cho CUNG diem anh vi PC nap sprite DONG BO.
  Mobile: khi dang ve (m_bVeDangDien) TextureResMgr::GetImage tra NULL cho anh dang nap o luong nen (TextureResMgr.cpp:522,
      :595-611) va PrepareFrameData bo khung khi het ngan sach / khung to / pak ban (TextureRes.cpp:641-690). Vay
      PaintGroundDirect tren mobile = BO o chua nap -> o den, khung sau o hien -> "den mang / chop mang".
  Vung nao roi vao PaintGroundDirect? Vung co OK_FLAG = false, tuc:
    (1) vung cach tieu diem >= 2 (nhanh XA cua KScenePlaceC::PrerenderGround: 1 vung/khung + [NENTRUOC 13/09] hoan toi 1,5 s
        cho luong nen chuan bi khung). Luat "8 vung ke ben = trong tam nhin" dung voi man PC 800x600; tren Fold 7 khung
        1040x936 + nhin rong 120-150 % (camera_mobile.ini ZoomMacDinh, chum ngon) nua tam nhin = 920 px = 1,8 vung ngang,
        1,5 vung doc -> vung cach 2 hien tren man hinh phan lon thoi gian. Man gap (1436x616) cao 616 px nen hang tren/duoi
        khong bao gio hien -> "man nho khong bi, mo rong ra la bi".
    (2) vung vua bi dat lai OK_FLAG: SetNestRegion (moi khi mot vung ke moi nap xong = moi lan qua ranh vung), ChangeProcessArea.
        Anh nen cu van la anh DUNG cua vung do, nhung PaintGround bo no ma ve truc tiep - va khung cua cac o nen da bi
        CheckBalance don sau 10 s nghi (anh ghep duoc dung, o goc khong) nen khong con gi de ve -> den roi hien lai = chop.
  O den TINH (RegionTileDefault.spr, 2958/6272 o cua Tien Coc Dong) co y het tren cay du lieu PC -> la thiet ke map, KHONG dung.

SUA (chi JX_MOBILE, PC bien dich y het - kiem bang ios/kiem_android_tuongduong.py --pc):
  a. KScenePlaceRegionC: co m_bJxNenAnhCu = "anh nen dang cam co noi dung cua CHINH vung nay" (bat sau moi lan ghep xong; tat khi
     bo anh / doi anh / Clear / RepresentShellReset). PaintGround ve anh nen khi OK_FLAG HOAC co anh cu -> dat lai OK_FLAG chi con
     nghia "can ghep lai", khong con nghia "ve truc tiep".
  b. KScenePlaceC::PrerenderGround: vung KE BEN hoac DANG TREN MAN HINH (giao m_RepresentArea +64 px, dung phep thu cua Paint())
     di nhanh 'ke ben'. Trong do vung dang tren man hinh ma CHUA co anh nao cua no thi ghep NGAY (khong chiu ngan sach 8 ms),
     vi hoan = khung nay ve bang duong bo o. Vung da co anh cu van chiu ngan sach nhu cu (PaintGround ve anh cu trong luc cho).
     Vung KHONG tren man hinh giu nguyen nhanh XA (1 vung/khung + hoan NENTRUOC) - toi uu 13/09 giu nguyen.
  Moc nhat ky: [NENNHIN] trong jx_paint.log (PaintLog=1), 24 dong dau.

Chay:  PYTHONIOENCODING=latin-1 python android\va_nguon_mobile_1609_nennhin.py
"""
import io
import os

GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TAG = "[NENNHIN 16/09]"


def va(duong, cap):
    p = os.path.join(GOC, duong)
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    cao0 = sum(1 for c in s if ord(c) >= 128)
    lf0 = s.count("\n") - s.count("\r\n")
    if TAG in s:
        print("  bo qua (da co %s): %s" % (TAG, duong))
        return
    for cu, moi in cap:
        if s.count(cu) != 1:
            raise SystemExit("HONG: neo xuat hien %d lan trong %s:\n%s" % (s.count(cu), duong, cu[:200]))
        s = s.replace(cu, moi, 1)
    cao1 = sum(1 for c in s if ord(c) >= 128)
    lf1 = s.count("\n") - s.count("\r\n")
    if cao1 != cao0 or lf1 != lf0:
        raise SystemExit("HONG: byte cao %d -> %d, LF le %d -> %d trong %s" % (cao0, cao1, lf0, lf1, duong))
    if any(ord(c) >= 128 for cu, moi in cap for c in moi):
        raise SystemExit("HONG: doan moi co byte >= 128")
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("  da va: %s" % duong)


# ---------------------------------------------------------------- KScenePlaceRegionC.h: co anh cu
va("Sources/Core/Src/Scene/KScenePlaceRegionC.h", [
    ("\tunsigned short m_jxNenCho[640];\t// chi so o (grunode roi object) con 'dang chuan bi', tang dan\r\n"
     "#endif\r\n",
     "\tunsigned short m_jxNenCho[640];\t// chi so o (grunode roi object) con 'dang chuan bi', tang dan\r\n"
     "\t// [NENNHIN 16/09] anh nen dang cam co noi dung cua CHINH vung nay (da ghep xong >= 1 lan). GROUND_IMG_OK_FLAG = false tu do chi con\r\n"
     "\t// nghia 'can ghep lai' (SetNestRegion / ChangeProcessArea), KHONG con nghia 've truc tiep': PaintGroundDirect tren mobile bo o chua nap\r\n"
     "\t// (GetImage tra NULL / bo ve khi dang ve) nen roi vao no la o den + chop; PC nap dong bo nen hai duong cho cung diem anh.\r\n"
     "\tbool JxNenCoAnhCu() const { return m_bJxNenAnhCu; }\r\n"
     "\tvoid JxNenAnhCuDat(bool b) { m_bJxNenAnhCu = b; }\r\n"
     "\tbool     m_bJxNenAnhCu;\r\n"
     "#endif\r\n"),
])

# ---------------------------------------------------------------- KScenePlaceRegionC.cpp
va("Sources/Core/Src/Scene/KScenePlaceRegionC.cpp", [
    # constructor
    ("\tJxNenDatLai();\t// [NENTRUOC 13/09] [NENTRUOC 13/09 b]\r\n"
     "#endif\r\n"
     "\r\n"
     "\tmemset(m_TrapInfo, 0, sizeof(m_TrapInfo));",
     "\tJxNenDatLai();\t// [NENTRUOC 13/09] [NENTRUOC 13/09 b]\r\n"
     "\tm_bJxNenAnhCu = false;\t// [NENNHIN 16/09]\r\n"
     "#endif\r\n"
     "\r\n"
     "\tmemset(m_TrapInfo, 0, sizeof(m_TrapInfo));"),
    # Clear(): het anh
    ("\tJxNenDatLai();\t// [NENTRUOC 13/09] [NENTRUOC 13/09 b]\r\n"
     "#endif\r\n"
     "\tm_Status = REGION_S_STANDBY;",
     "\tJxNenDatLai();\t// [NENTRUOC 13/09] [NENTRUOC 13/09 b]\r\n"
     "\tm_bJxNenAnhCu = false;\t// [NENNHIN 16/09] da bo anh nen\r\n"
     "#endif\r\n"
     "\tm_Status = REGION_S_STANDBY;"),
    # PrerenderGround(): ghep xong -> co anh
    ("\t\tJX_NEN_VE(nNum);\t// [GOMNEN 12/09]\r\n"
     "\t}\r\n"
     "#ifdef JX_MOBILE\r\n"
     "\t{\t// [NENDO 13/09 b]",
     "\t\tJX_NEN_VE(nNum);\t// [GOMNEN 12/09]\r\n"
     "\t}\r\n"
     "#ifdef JX_MOBILE\r\n"
     "\tm_bJxNenAnhCu = true;\t// [NENNHIN 16/09] tu day anh nen la anh dung cua vung nay: OK_FLAG bi dat lai thi van ve anh nay toi khi ghep lai xong\r\n"
     "\t{\t// [NENDO 13/09 b]"),
    # LeaveProcessArea(): tra anh
    ("\tm_pPrerenderGroundImg = NULL;\r\n"
     "\tif (m_BiosData.pLeafs)\r\n"
     "\t{\r\n"
     "\t\tfree(m_BiosData.pLeafs);",
     "\tm_pPrerenderGroundImg = NULL;\r\n"
     "#ifdef JX_MOBILE\r\n"
     "\tm_bJxNenAnhCu = false;\t// [NENNHIN 16/09] da tra anh nen\r\n"
     "#endif\r\n"
     "\tif (m_BiosData.pLeafs)\r\n"
     "\t{\r\n"
     "\t\tfree(m_BiosData.pLeafs);"),
    # EnterProcessArea(): doi anh
    ("\t\tif (m_pPrerenderGroundImg = pImage)\r\n",
     "#ifdef JX_MOBILE\r\n"
     "\t\tm_bJxNenAnhCu = false;\t// [NENNHIN 16/09] anh khac (hoac NULL): chua co noi dung cua vung nay\r\n"
     "#endif\r\n"
     "\t\tif (m_pPrerenderGroundImg = pImage)\r\n"),
    # PaintGround(): ve anh cu trong luc cho ghep lai
    ("\tif (m_pPrerenderGroundImg && bPrerenderGroundImg && m_pPrerenderGroundImg->GROUND_IMG_OK_FLAG)\r\n",
     "#ifdef JX_MOBILE\r\n"
     "\t// [NENNHIN 16/09] OK_FLAG = false chi la 'can ghep lai' (SetNestRegion moi khi vung ke moi nap xong, ChangeProcessArea): anh cu van la\r\n"
     "\t// anh DUNG cua vung nay -> van ve anh cu cho toi khi ghep lai xong. Roi xuong PaintGroundDirect tren mobile = bo o chua nap\r\n"
     "\t// (TextureResMgr::GetImage tra NULL khi dang ve, PrepareFrameData bo ve) = o den roi hien lai = chop; PC nap dong bo nen khong thay.\r\n"
     "\tif (m_pPrerenderGroundImg && bPrerenderGroundImg && (m_pPrerenderGroundImg->GROUND_IMG_OK_FLAG || m_bJxNenAnhCu))\r\n"
     "#else\r\n"
     "\tif (m_pPrerenderGroundImg && bPrerenderGroundImg && m_pPrerenderGroundImg->GROUND_IMG_OK_FLAG)\r\n"
     "#endif\r\n"),
])

# ---------------------------------------------------------------- KScenePlaceC.cpp
va("Sources/Core/Src/Scene/KScenePlaceC.cpp", [
    # ham phu: vung dang tren man hinh + moc nhat ky
    ("\treturn s_dF > 0.0 ? (double)(b.QuadPart - a.QuadPart) * 1000.0 / s_dF : 0.0;\r\n"
     "}\r\n"
     "#endif\r\n"
     "\r\n"
     "void KScenePlaceC::Paint()\r\n",
     "\treturn s_dF > 0.0 ? (double)(b.QuadPart - a.QuadPart) * 1000.0 / s_dF : 0.0;\r\n"
     "}\r\n"
     "#endif\r\n"
     "#ifdef JX_MOBILE\r\n"
     "// [NENNHIN 16/09] Vung co giao vung ve (m_RepresentArea + 64 px) khong - cung phep thu Paint() dung de chon vung ve nen.\r\n"
     "static inline bool JxNenVungNhin(const POINT& RgIdx, const RECT& rc)\r\n"
     "{\r\n"
     "\tconst LONG nL = RgIdx.x * (LONG)KScenePlaceRegionC::RWPP_AREGION_WIDTH, nT = RgIdx.y * (LONG)KScenePlaceRegionC::RWPP_AREGION_HEIGHT;\r\n"
     "\treturn !(nL >= rc.right || nL + (LONG)KScenePlaceRegionC::RWPP_AREGION_WIDTH <= rc.left || nT >= rc.bottom || nT + (LONG)KScenePlaceRegionC::RWPP_AREGION_HEIGHT <= rc.top);\r\n"
     "}\r\n"
     "// [NENNHIN 16/09] moc nhat ky (jx_paint.log, PaintLog=1): vung cach tieu diem >= 2 nhung dang tren man hinh duoc ghep theo nhanh ke ben; 24 dong dau\r\n"
     "static unsigned g_uJxNenNhinSo = 0, g_uJxNenNhinNgay = 0;\r\n"
     "static void JxNenNhinGhi(const POINT& RgIdx, int nDx, int nDy, bool bNgay)\r\n"
     "{\r\n"
     "\textern int g_nCorePaintLog;\r\n"
     "\tg_uJxNenNhinSo++;\r\n"
     "\tif (bNgay) g_uJxNenNhinNgay++;\r\n"
     "\tif (g_nCorePaintLog <= 0 || g_uJxNenNhinSo > 24) return;\r\n"
     "\tFILE* p = fopen(\"jx_paint.log\", \"a\");\r\n"
     "\tif (!p) return;\r\n"
     "\tfprintf(p, \"[NENNHIN] vung (%d,%d) cach tieu diem %d,%d dang tren man hinh -> ghep theo nhanh ke ben (%s) [tong %u, ghep ngay %u]\\n\",\r\n"
     "\t\tRgIdx.x, RgIdx.y, nDx, nDy, bNgay ? \"chua co anh: ghep ngay\" : \"co anh cu: theo ngan sach\", g_uJxNenNhinSo, g_uJxNenNhinNgay);\r\n"
     "\tfclose(p);\r\n"
     "}\r\n"
     "#endif\r\n"
     "\r\n"
     "void KScenePlaceC::Paint()\r\n"),
    # vung ve cua khung nay (mot lan moi vong PrerenderGround) - khoi rieng #ifdef JX_MOBILE, KHONG chen vao khoi
    # '#if defined(JX_MOBILE) && !defined(_SERVER)' co san (kiem_android_tuongduong khong danh gia duoc _SERVER nen giu ca hai nhanh)
    ("\tconst DWORD\tdwPgBudgetMs = 8;\r\n"
     "#if defined(JX_MOBILE) && !defined(_SERVER)\r\n",
     "\tconst DWORD\tdwPgBudgetMs = 8;\r\n"
     "#ifdef JX_MOBILE\r\n"
     "\tRECT rcJxNhin = m_RepresentArea;\t// [NENNHIN 16/09] vung ve cua khung nay (+64 px nhu rcGroundView trong Paint())\r\n"
     "\trcJxNhin.left -= 64; rcJxNhin.top -= 64; rcJxNhin.right += 64; rcJxNhin.bottom += 64;\r\n"
     "#endif\r\n"
     "#if defined(JX_MOBILE) && !defined(_SERVER)\r\n"),
    # nhanh ke ben -> ke ben HOAC dang tren man hinh
    ("\t\telse if (nDx <= 1 && nDy <= 1)\r\n"
     "\t\t{\r\n"
     "\t\t\t// 8 region ke ben (van trong tam nhin o ria): ve ngay NEU con ngan sach,\r\n"
     "\t\t\t// het ngan sach thi hoan sang khung sau thay vi keo dai khung nay.\r\n"
     "\t\t\tif (timeGetTime() - dwPgT0 < dwPgBudgetMs)\r\n"
     "#ifdef JX_MOBILE\r\n"
     "\t\t\t{\t// [NENTRUOC 13/09 c] vung KE BEN dang tren man hinh: ghep NGAY nhu cu, KHONG hoan (hoan = nen quanh nhan vat den toi 1,5 s luc vao/quay lai\r\n"
     "\t\t\t\t// map vi luong nen dang ngap - chu thay 22:13 13/09). Chi xin nap truoc: vung bi hoan vi ngan sach 8 ms se co khung san o khung sau.\r\n"
     "\t\t\t\tm_pInProcessAreaRegions[i]->JxNenTruoc();\r\n"
     "\t\t\t\tJX_NEN_DO(1, m_pInProcessAreaRegions[i]->PrerenderGround(false));\t// [NENDAT 11/09]\r\n"
     "\t\t\t}\r\n"
     "#else\r\n"
     "\t\t\t\tJX_NEN_DO(1, m_pInProcessAreaRegions[i]->PrerenderGround(false));\t// [NENDAT 11/09]\r\n"
     "#endif\r\n"
     "\t\t\telse\r\n"
     "\t\t\t\tnDeferred++;\r\n"
     "\t\t}\r\n"
     "\t\telse if (nFarBudget > 0)\r\n",
     "#ifdef JX_MOBILE\r\n"
     "\t\telse if ((nDx <= 1 && nDy <= 1) || JxNenVungNhin(RgIdx, rcJxNhin))\r\n"
     "\t\t{\t// [NENNHIN 16/09] ke ben HOAC DANG TREN MAN HINH. Luat cu chi coi 8 vung ke ben la 'trong tam nhin' - dung voi man PC 800x600 / 1024x768,\r\n"
     "\t\t\t// sai tren dien thoai: khung 1040x936 + nhin rong 120-150 % (camera_mobile.ini / chum ngon) cho thay ca vung cach tieu diem 2 (nua tam nhin\r\n"
     "\t\t\t// toi 920 px = 1,8 vung ngang, 1,5 vung doc). Vung do roi vao nhanh XA (1 vung/khung + hoan NENTRUOC toi 1,5 s), va trong luc cho\r\n"
     "\t\t\t// PaintGround ve bang PaintGroundDirect - tren mobile duong do BO o chua nap (GetImage tra NULL / bo ve khi dang ve: TextureResMgr.cpp:522,\r\n"
     "\t\t\t// TextureRes.cpp:655) = o den roi lan luot hien = 'den mang / chop mang', chi thay tren man rong. PC nap dong bo nen khong bao gio thay.\r\n"
     "\t\t\tconst bool bJxNhin = JxNenVungNhin(RgIdx, rcJxNhin);\r\n"
     "\t\t\tconst bool bJxNgay = bJxNhin && !m_pInProcessAreaRegions[i]->JxNenCoAnhCu();\t// dang tren man hinh ma CHUA co anh nao cua no: ghep NGAY, khong chiu ngan sach\r\n"
     "\t\t\tif (bJxNgay || timeGetTime() - dwPgT0 < dwPgBudgetMs)\r\n"
     "\t\t\t{\t// [NENTRUOC 13/09 c] vung dang tren man hinh: ghep NGAY nhu cu, KHONG hoan (hoan = nen quanh nhan vat den toi 1,5 s luc vao/quay lai\r\n"
     "\t\t\t\t// map vi luong nen dang ngap - chu thay 22:13 13/09). Chi xin nap truoc: vung bi hoan vi ngan sach 8 ms se co khung san o khung sau.\r\n"
     "\t\t\t\tm_pInProcessAreaRegions[i]->JxNenTruoc();\r\n"
     "\t\t\t\tbool bJxDa = false;\r\n"
     "\t\t\t\tJX_NEN_DO(1, bJxDa = m_pInProcessAreaRegions[i]->PrerenderGround(false));\t// [NENDAT 11/09]\r\n"
     "\t\t\t\tif (bJxDa && bJxNhin && (nDx > 1 || nDy > 1)) JxNenNhinGhi(RgIdx, nDx, nDy, bJxNgay);\t// [NENNHIN 16/09] moc nhat ky\r\n"
     "\t\t\t}\r\n"
     "\t\t\telse\r\n"
     "\t\t\t\tnDeferred++;\t// da co anh cu: PaintGround van ve anh cu, ghep lai o khung sau\r\n"
     "\t\t}\r\n"
     "#else\r\n"
     "\t\telse if (nDx <= 1 && nDy <= 1)\r\n"
     "\t\t{\r\n"
     "\t\t\t// 8 region ke ben (van trong tam nhin o ria): ve ngay NEU con ngan sach,\r\n"
     "\t\t\t// het ngan sach thi hoan sang khung sau thay vi keo dai khung nay.\r\n"
     "\t\t\tif (timeGetTime() - dwPgT0 < dwPgBudgetMs)\r\n"
     "\t\t\t\tJX_NEN_DO(1, m_pInProcessAreaRegions[i]->PrerenderGround(false));\t// [NENDAT 11/09]\r\n"
     "\t\t\telse\r\n"
     "\t\t\t\tnDeferred++;\r\n"
     "\t\t}\r\n"
     "#endif\r\n"
     "\t\telse if (nFarBudget > 0)\r\n"),
    # RepresentShellReset(): anh GPU co the mat -> khong con anh cu
    ("\tfor (int i = 0; i < m_nNumGroundImagesAvailable; i++)\r\n"
     "\t\tm_RegionGroundImages[i].GROUND_IMG_OK_FLAG = false;\r\n"
     "}\r\n",
     "\tfor (int i = 0; i < m_nNumGroundImagesAvailable; i++)\r\n"
     "\t\tm_RegionGroundImages[i].GROUND_IMG_OK_FLAG = false;\r\n"
     "#ifdef JX_MOBILE\r\n"
     "\tfor (int i = 0; i < SPWP_MAX_NUM_REGIONS; i++)\r\n"
     "\t\tif (m_pRegions[i]) m_pRegions[i]->JxNenAnhCuDat(false);\t// [NENNHIN 16/09] thiet bi dat lai: anh nen phai ghep lai tu dau, khong con anh cu de ve\r\n"
     "#endif\r\n"
     "}\r\n"),
])
print("xong %s" % TAG)
