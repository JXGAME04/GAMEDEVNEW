# -*- coding: utf-8 -*-
r"""[GOMNEN 12/09] Gom CA mot vung nen vao MOT luot ve len anh thay vi ~7 luot.

Chuoi truy (moi buoc deu co so, khong doan):
  1. [VE-GIAT]  khung giat 28,6 ms = trong lop ve 8,7 + NGOAI 19,9 (70 %)
  2. [PDET]     ca 20 ms nam gon trong MOT lenh goi Wnd_RenderWindows
  3. [PHAVE]    trong do VE THE GIOI chiem 78,6 %, dan dau 177/202 khung
  4. [PHACANH2] trong ve the gioi: PrerenderGround 61,7 % (dan dau 204/311 khung), CHO KHOA 0,00 ms
  5. phan bo   : 115 lan >= 15 ms mot phien, TRUNG VI 17 ms cho MOT vung - ma mot khung chi co 8,3 ms

Vi sao mot vung ton 17 ms: LOCAL_MAX_IMG_NUM = 80 (dong 28) trong khi mot vung co toi ~512 nut nen (luoi 16x16,
trung binh hai lop moi o) cong cac vat phu nen. Hai vong trong PrerenderGround deu gom toi 80 roi goi
DrawPrimitivesOnImage -> ~7 luot, MOI LUOT mot lan GetRenderTarget + SetRenderTarget sang texture + tra lai.
Tren GPU xep o cua dien thoai, moi lan doi dich ve la mot lan XA O (tile flush), rat dat.
BANG CHUNG TU LOG: khung binh thuong 1 pass; khung giat trung vi 2 pass, DINH 8 (126/199 khung giat co > 1 pass).

Sua: chi trong ham PrerenderGround, cap dem du cho CA vung roi goi DrawPrimitivesOnImage MOT lan o cuoi.
KET QUA VE RA Y HET: DrawPrimitivesOnImage chi duyet va ve tung phan tu, khong co gioi han ben trong; goi mot lan voi
560 phan tu hay bay lan voi 80 phan tu deu ra dung nhung phan tu do, dung thu tu do, len dung anh do - chi khac so lan
doi dich ve. Mang cu nam tren NGAN XEP; ban moi cap tren DONG de khong phinh ngan xep cua luong ve.

Kem bo do: thoi gian nam trong cac lan goi DrawPrimitivesOnImage, so lan goi, so anh - in vao dong [PGND].
Neu [PGND] trung vi tut khoi 17 ms thi nghi pham "doi dich ve" dung; neu khong tut thi thu pham la "nap dong bo"
(DrawPrimitivesOnImage mo dau bang Rep3NapDongBo) va toi di tiep huong do.

CHI sua trong than ham PrerenderGround (ham khac o dong 605 cung dung ten ImgList - khong dung toi).
Rao JX_MOBILE nen ban PC va may chu khong doi mot byte.
Dung:  python android\va_nguon_android_gomnen.py
"""
import io
import os

GOC = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[GOMNEN 12/09]"


def doc(p):
    s = io.open(p, encoding="latin-1", newline="").read()
    crlf = s.count("\r\n")
    if crlf and crlf != s.count("\n"):
        raise SystemExit("xuong dong lan lon: " + p)
    return s.replace("\r\n", "\n"), ("\r\n" if crlf else "\n"), sum(1 for c in s if ord(c) >= 0x80)


def ghi(p, s, nl, cao):
    if sum(1 for c in s if ord(c) >= 0x80) != cao:
        raise SystemExit("byte cao doi: " + p)
    io.open(p, "w", encoding="latin-1", newline="").write(s.replace("\n", nl))
    print("da va:", os.path.relpath(p, GOC))


def thay1(s, cu, moi, ten):
    n = s.count(cu)
    if n != 1:
        raise SystemExit("moc '%s' khop %d cho (can 1)" % (ten, n))
    return s.replace(cu, moi)


p = os.path.join(GOC, "Sources", "Core", "Src", "Scene", "KScenePlaceRegionC.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi")
else:
    # ---- khoanh dung than ham PrerenderGround ----
    dau = "bool KScenePlaceRegionC::PrerenderGround(bool bForce)\n{\n"
    i0 = s.index(dau)
    i1 = s.index("\n\treturn true;\n}\n", i0) + len("\n\treturn true;\n}\n")
    than = s[i0:i1]
    assert than.count("ImgList") >= 5, "khoanh nham than ham"

    # 1. dem du ca vung, cap tren dong
    than = thay1(than, "\tKRUImage\tImgList[LOCAL_MAX_IMG_NUM];\n",
        "#ifdef JX_MOBILE\n"
        "\t// " + DAU + " dem du CA vung -> chi MOT lan doi dich ve thay vi ~7 lan (moi lan la mot lan xa o tren GPU dien thoai)\n"
        "\tconst unsigned uJxTong = m_GroundLayerData.uNumGrunode + m_GroundLayerData.uNumObject;\n"
        "\tstd::vector<KRUImage> jxBuf(uJxTong ? (size_t)uJxTong : (size_t)1);\n"
        "\tKRUImage* const ImgList = &jxBuf[0];\n"
        "\tconst int nJxMaxImg = (int)(uJxTong ? uJxTong : 1);\n"
        "\tg_dJxNenVeMs = 0.0; g_uJxNenVeLan = 0; g_uJxNenSoAnh = uJxTong;\n"
        "#else\n"
        "\tKRUImage\tImgList[LOCAL_MAX_IMG_NUM];\n"
        "\tconst int nJxMaxImg = LOCAL_MAX_IMG_NUM;\n"
        "#endif\n",
        "khai bao ImgList")
    than = thay1(than, "\tmemset(&ImgList, 0, sizeof(ImgList));\n",
        "#ifdef JX_MOBILE\n"
        "\tmemset(ImgList, 0, sizeof(KRUImage) * (size_t)nJxMaxImg);\t// " + DAU + "\n"
        "#else\n"
        "\tmemset(&ImgList, 0, sizeof(ImgList));\n"
        "#endif\n",
        "memset ImgList")

    # 2. dieu kien gom: dung nJxMaxImg (mobile = ca vung -> khong bao gio xa giua chung)
    n = than.count("\t\tif (nNum < LOCAL_MAX_IMG_NUM)\n")
    if n != 2:
        raise SystemExit("moc 'dieu kien gom' khop %d cho (can 2)" % n)
    than = than.replace("\t\tif (nNum < LOCAL_MAX_IMG_NUM)\n", "\t\tif (nNum < nJxMaxImg)\t// " + DAU + "\n")

    # 3. hai lenh goi giua vong + lenh goi cuoi -> macro co do thoi gian
    goi_giua = ("\t\t\tg_pRepresent->DrawPrimitivesOnImage(LOCAL_MAX_IMG_NUM, &ImgList[0], RU_T_IMAGE,\n"
                "\t\t\t\tm_pPrerenderGroundImg->szImage, m_pPrerenderGroundImg->uImage,\n"
                "\t\t\t\tm_pPrerenderGroundImg->nISPosition);\n")
    n = than.count(goi_giua)
    if n != 2:
        raise SystemExit("moc 'goi giua vong' khop %d cho (can 2)" % n)
    than = than.replace(goi_giua, "\t\t\tJX_NEN_VE(nJxMaxImg);\t// " + DAU + "\n")
    than = thay1(than,
        "\t\tg_pRepresent->DrawPrimitivesOnImage(nNum, &ImgList[0], RU_T_IMAGE,\n"
        "\t\t\tm_pPrerenderGroundImg->szImage, m_pPrerenderGroundImg->uImage,\n"
        "\t\t\tm_pPrerenderGroundImg->nISPosition);\n",
        "\t\tJX_NEN_VE(nNum);\t// " + DAU + "\n",
        "lenh goi cuoi")

    s = s[:i0] + than + s[i1:]

    # 4. bien do + macro, dat truoc ham
    s = thay1(s, "#define\tLOCAL_MAX_IMG_NUM\t80",
        "#define\tLOCAL_MAX_IMG_NUM\t80\n"
        "#ifdef JX_MOBILE\n"
        "#include <vector>\n"
        "// " + DAU + " do: thoi gian nam trong cac lan goi DrawPrimitivesOnImage cua mot vung, so lan goi, so anh\n"
        "double g_dJxNenVeMs = 0.0;\tunsigned g_uJxNenVeLan = 0, g_uJxNenSoAnh = 0;\n"
        "#endif",
        "LOCAL_MAX_IMG_NUM")
    s = thay1(s, "bool KScenePlaceRegionC::PrerenderGround(bool bForce)\n{\n",
        "#ifdef JX_MOBILE\n"
        "// " + DAU + " goi ve len anh, kem do thoi gian va dem so lan\n"
        "#define JX_NEN_VE(n) do { LARGE_INTEGER jxA, jxB, jxF; QueryPerformanceFrequency(&jxF); QueryPerformanceCounter(&jxA); "
        "g_pRepresent->DrawPrimitivesOnImage((n), &ImgList[0], RU_T_IMAGE, m_pPrerenderGroundImg->szImage, "
        "m_pPrerenderGroundImg->uImage, m_pPrerenderGroundImg->nISPosition); QueryPerformanceCounter(&jxB); "
        "if (jxF.QuadPart) g_dJxNenVeMs += (double)(jxB.QuadPart - jxA.QuadPart) * 1000.0 / (double)jxF.QuadPart; "
        "g_uJxNenVeLan++; } while (0)\n"
        "#else\n"
        "#define JX_NEN_VE(n) g_pRepresent->DrawPrimitivesOnImage((n), &ImgList[0], RU_T_IMAGE, "
        "m_pPrerenderGroundImg->szImage, m_pPrerenderGroundImg->uImage, m_pPrerenderGroundImg->nISPosition)\n"
        "#endif\n"
        "\n"
        "bool KScenePlaceRegionC::PrerenderGround(bool bForce)\n{\n",
        "dau ham PrerenderGround")
    ghi(p, s, nl, cao)

# ---------------------------------------------------------------- dong [PGND]
p = os.path.join(GOC, "Sources", "Core", "Src", "Scene", "KScenePlaceC.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi")
else:
    s = thay1(s, "\t\t\t\tfprintf(pPgLog, \"[PGND] ms=%u | tieu diem %.1f/%d, ke ben %.1f/%d, XA %.1f/%d",
        "\t\t\t\textern double g_dJxNenVeMs; extern unsigned g_uJxNenVeLan, g_uJxNenSoAnh;\t// " + DAU + "\n"
        "\t\t\t\tfprintf(pPgLog, \"[PGND] ms=%u | tieu diem %.1f/%d, ke ben %.1f/%d, XA %.1f/%d | ve len anh %.1f ms / %u lan / %u anh",
        "dong PGND dinh dang")
    s = thay1(s, "g_dJxNenNhanh[2], g_nJxNenSo[2]);\n",
        "g_dJxNenNhanh[2], g_nJxNenSo[2], g_dJxNenVeMs, g_uJxNenVeLan, g_uJxNenSoAnh);\n",
        "dong PGND tham so")
    ghi(p, s, nl, cao)

print("xong")
