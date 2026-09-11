# -*- coding: utf-8 -*-
"""goi_va_anhnen_1009.py - [ANHNEN 10/09] ve LOP ANH CANH nhu ban VLTK 2.0.

Do duoc o ban 2.0 (bao cao BANGIAO_CANH_VLTK2_1009.md):
  - gamecl.exe doc \\Maps\\ScrollSetting\\<map_id>_Scroll.ini (chuoi VA 0x7B0BE6, ham nap 0x6747F0).
  - Moi muc "0","1",... co Type = 1 (anh nen), 2 (tien canh), 3 (lop phu nen); Type khac -> ngung doc.
  - Area{Left,Right} x 512 va Area{Top,Bottom} x 1024 (shl 9 / shl 0xA o 0x67499C..0x6749B3) = vung kich
    hoat theo CHI SO O VUNG doi ra diem anh; chi ve khi tieu diem nam HAN trong vung (so sanh chat hai dau).
  - PaintRect L/T mac dinh 0, R mac dinh 0x400, B mac dinh 0x300; Rating mac dinh 2; Speed mac dinh 2.
  - Rating: chu thich goc trong ini "canh dich Rating diem thi nen moi dich 1 diem".
  - Kieu 2/3 dung Speed + Angle (thang 64, 0 = thang len, thuan kim dong ho) + Count + Image0..Image8.
Du an da co san du lieu: 26 tep _Scroll.ini, 11 anh nen jpg, 9 cloud*.spr trong pak (trung byte voi 2.0),
chi thieu ma doc/ve. Tat bang [Client] AnhNenCanh=0.
"""
import io
import sys

NL = "\r\n"
T = "\t"
D = "D:/GAMEDEVNEW_wt_delta/Sources/Core/Src/Scene/"
TAG = "[ANHNEN 10/09]"


def doc(p):
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    return s, sum(1 for c in s if ord(c) >= 0x80), s.count("\n") - s.count("\r\n"), s.count("\r\n")


def ghi(p, s, h0, lf0, crlf0, ten):
    ok = (s.count("\n") - s.count("\r\n") == lf0) if crlf0 else (s.count("\r\n") == 0)
    if not ok or sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s:
        print("FAIL ma hoa " + ten); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("OK " + ten)


def rep(s, old, new, ten, n_mong=1):
    n = s.count(old)
    if n != n_mong:
        print("FAIL neo %s: %d (mong %d)" % (ten, n, n_mong)); sys.exit(1)
    return s.replace(old, new)


# ------------------------------------------------------------------ KScenePlaceC.h
P = D + "KScenePlaceC.h"
s, h0, lf0, crlf0 = doc(P)
if TAG not in s:
    neo = T + "RECT" + T + "BGArea[MAX_BACKGROUND_IMAGE];" + NL
    them = NL.join([
        "",
        "public:",
        T + "// " + TAG + " lop anh canh nhu ban 2.0: \\Maps\\ScrollSetting\\<id>_Scroll.ini",
        T + "enum { LC_MAX_LOP = 24, LC_MAX_ANH = 9, LC_MAX_MAY = 32 };",
        T + "struct KLopCanh",
        T + "{",
        T*2 + "int" + T*2 + "nKieu;" + T*3 + "// 1 = anh nen, 2 = tien canh, 3 = lop phu nen",
        T*2 + "RECT" + T + "rcVung;" + T*2 + "// Area* doi ra diem anh (x * 512, y * 1024)",
        T*2 + "RECT" + T + "rcMan;" + T*3 + "// PaintRect* (toa do man hinh)",
        T*2 + "int" + T*2 + "nTamCanhX, nTamCanhY;" + T + "// SceneCenterPoint (kieu 1)",
        T*2 + "int" + T*2 + "nTamAnhX, nTamAnhY;" + T + "// PicCenterPoint (kieu 1)",
        T*2 + "int" + T*2 + "nTiLe;" + T*3 + "// Rating: canh dich nTiLe diem thi nen dich 1",
        T*2 + "RECT" + T + "rcTam;" + T*3 + "// ViewArea* (kieu 2/3), diem anh",
        T*2 + "int" + T*2 + "nToc, nGoc, nSo;" + T + "// Speed, Angle (thang 64), Count",
        T*2 + "int" + T*2 + "nSoAnh;",
        T*2 + "char" + T + "szAnh[LC_MAX_ANH][64];",
        T*2 + "int" + T*2 + "bDaDat;" + T*2 + "// da rai may chua",
        T*2 + "DWORD" + T + "dwMoc;" + T*3 + "// moc thoi gian lan ve truoc",
        T*2 + "int" + T*2 + "nMayX[LC_MAX_MAY], nMayY[LC_MAX_MAY], nMayAnh[LC_MAX_MAY];" + T + "// x, y tinh theo 1/16 diem anh",
        T + "};",
        T + "void" + T + "NapLopCanh(int nPlaceIndex);",
        T + "void" + T + "VeLopCanh(int nKieu);",
        T + "void" + T + "VeLopNen(KLopCanh* p);",
        T + "void" + T + "VeLopMay(KLopCanh* p);",
        T + "KLopCanh" + T + "m_LopCanh[LC_MAX_LOP];",
        T + "int" + T*2 + "m_nLopCanh;",
        "private:",
        ""])
    s = rep(s, neo, neo + them, "H lop canh")
    ghi(P, s, h0, lf0, crlf0, "KScenePlaceC.h")
else:
    print("KScenePlaceC.h da co")

# ------------------------------------------------------------------ KScenePlaceC.cpp
P = D + "KScenePlaceC.cpp"
s, h0, lf0, crlf0 = doc(P)
if TAG not in s:
    # 1. dat lai luc khoi tao
    s = rep(s, T + "bPaintMode = 0;" + NL + T + "bFlagMode = false;" + NL,
            T + "bPaintMode = 0;" + NL + T + "bFlagMode = false;" + NL + T + "m_nLopCanh = 0;" + T + "// " + TAG + NL, "C ctor")
    # 2. nap khi mo map
    s = rep(s, T + "m_nSceneId = nPlaceIndex;" + NL,
            T + "m_nSceneId = nPlaceIndex;" + NL + T + "NapLopCanh(nPlaceIndex);" + T + "// " + TAG + NL, "C nap")
    # 3. xoa khi dong map
    s = rep(s, T + "m_nNumGroundImagesAvailable = 0;" + NL + T + "m_bInited = false;" + NL,
            T + "m_nNumGroundImagesAvailable = 0;" + NL + T + "m_nLopCanh = 0;" + T + "// " + TAG + NL + T + "m_bInited = false;" + NL, "C dong")
    # 4. ve: kieu 1 truoc nen dat, kieu 3 sau nen dat truoc vat the, kieu 2 sau cung
    moc = "BOOL bPrerenderGroundImg = PaintBackGround();"
    if s.count(moc) != 1:
        print("FAIL neo C ve kieu 1: %d" % s.count(moc)); sys.exit(1)
    a = s.find(moc)
    b = s.find(NL, a)
    s = s[:b + len(NL)] + T + "VeLopCanh(1);" + T + "// " + TAG + " anh nen ve TRUOC nen dat" + NL + s[b + len(NL):]
    s = rep(s, T + "m_ObjectsTree.Paint(&m_RepresentArea, IPOT_RL_COVER_GROUND);" + NL,
            T + "VeLopCanh(3);" + T + "// " + TAG + " lop phu nen (may xa) ve sau nen dat, truoc vat the" + NL +
            T + "m_ObjectsTree.Paint(&m_RepresentArea, IPOT_RL_COVER_GROUND);" + NL, "C ve kieu 3")
    s = rep(s, T + "m_ObjectsTree.Paint(&m_RepresentArea, IPOT_RL_INFRONTOF_ALL);" + NL,
            T + "m_ObjectsTree.Paint(&m_RepresentArea, IPOT_RL_INFRONTOF_ALL);" + NL +
            T + "VeLopCanh(2);" + T + "// " + TAG + " tien canh ve sau cung" + NL, "C ve kieu 2")
    # 5. than ba ham, dat truoc PaintBackGround
    neo = "BOOL KScenePlaceC::PaintBackGround()"
    W = "KScenePlaceRegionC::RWPP_AREGION_WIDTH"
    H = "KScenePlaceRegionC::RWPP_AREGION_HEIGHT"
    than = NL.join([
        "// " + TAG + " ---------------------------------------------------------------------",
        "// Ve lop anh canh y nhu ban VLTK 2.0 (xem BANGIAO_CANH_VLTK2_1009.md):",
        "//   tep  \\Maps\\ScrollSetting\\<map_id>_Scroll.ini, moi muc mot lop.",
        "//   Type 1 = anh nen JPG chay thi sai (canh dich Rating diem thi nen dich 1 diem),",
        "//   Type 3 = lop phu nen (may xa) ve sau nen dat truoc vat the,",
        "//   Type 2 = tien canh (may gan) ve sau cung.",
        "//   Area* la CHI SO O VUNG -> doi ra diem anh (x * 512, y * 1024) nhu 2.0 lam luc nap.",
        "// Tat: [Client] AnhNenCanh=0 (khi do khong doc tep, khong ve gi - y het truoc day).",
        "static int l_nAnhNenCanh = -1;",
        "",
        "void KScenePlaceC::NapLopCanh(int nPlaceIndex)",
        "{",
        T + "m_nLopCanh = 0;",
        T + "if (l_nAnhNenCanh < 0)",
        T*2 + "l_nAnhNenCanh = GetPrivateProfileIntA(\"Client\", \"AnhNenCanh\", 1, \".\\\\config.ini\");",
        T + "if (l_nAnhNenCanh <= 0 || nPlaceIndex < 0)",
        T*2 + "return;",
        "",
        T + "char szTep[128];",
        T + "sprintf(szTep, \"\\\\Maps\\\\ScrollSetting\\\\%d_Scroll.ini\", nPlaceIndex);",
        T + "KIniFile Ini;",
        T + "if (Ini.Load(szTep) == FALSE)",
        T*2 + "return;",
        "",
        T + "int nDem[4] = { 0, 0, 0, 0 };",
        T + "for (int i = 0; i < LC_MAX_LOP; i++)",
        T + "{",
        T*2 + "char szMuc[16];",
        T*2 + "itoa(i, szMuc, 10);",
        T*2 + "int nKieu = 0;",
        T*2 + "Ini.GetInteger(szMuc, \"Type\", 0, &nKieu);",
        T*2 + "if (nKieu < 1 || nKieu > 3)",
        T*3 + "break;" + T + "// het muc - ban 2.0 cung ngung o day",
        "",
        T*2 + "KLopCanh* p = &m_LopCanh[m_nLopCanh];",
        T*2 + "memset(p, 0, sizeof(*p));",
        T*2 + "p->nKieu = nKieu;",
        T*2 + "int v = 0;",
        T*2 + "Ini.GetInteger(szMuc, \"AreaLeft\",   0, &v); p->rcVung.left   = v * " + W + ";",
        T*2 + "Ini.GetInteger(szMuc, \"AreaTop\",    0, &v); p->rcVung.top    = v * " + H + ";",
        T*2 + "Ini.GetInteger(szMuc, \"AreaRight\",  0, &v); p->rcVung.right  = v * " + W + ";",
        T*2 + "Ini.GetInteger(szMuc, \"AreaBottom\", 0, &v); p->rcVung.bottom = v * " + H + ";",
        T*2 + "Ini.GetInteger(szMuc, \"PaintRectLeft\",   0,    &v); p->rcMan.left   = v;",
        T*2 + "Ini.GetInteger(szMuc, \"PaintRectTop\",    0,    &v); p->rcMan.top    = v;",
        T*2 + "Ini.GetInteger(szMuc, \"PaintRectRight\",  1024, &v); p->rcMan.right  = v;",
        T*2 + "Ini.GetInteger(szMuc, \"PaintRectBottom\", 768,  &v); p->rcMan.bottom = v;",
        T*2 + "if (nKieu == 1)",
        T*2 + "{",
        T*3 + "Ini.GetInteger(szMuc, \"SceneCenterPointX\", 0, &p->nTamCanhX);",
        T*3 + "Ini.GetInteger(szMuc, \"SceneCenterPointY\", 0, &p->nTamCanhY);",
        T*3 + "Ini.GetInteger(szMuc, \"PicCenterPointX\",   0, &p->nTamAnhX);",
        T*3 + "Ini.GetInteger(szMuc, \"PicCenterPointY\",   0, &p->nTamAnhY);",
        T*3 + "Ini.GetInteger(szMuc, \"Rating\", 2, &p->nTiLe);",
        T*3 + "if (p->nTiLe <= 0) p->nTiLe = 1;",
        T*3 + "Ini.GetString(szMuc, \"Image\", \"\", p->szAnh[0], sizeof(p->szAnh[0]));",
        T*3 + "p->nSoAnh = p->szAnh[0][0] ? 1 : 0;",
        T*2 + "}",
        T*2 + "else",
        T*2 + "{",
        T*3 + "Ini.GetInteger(szMuc, \"ViewAreaLeft\",   0, &v); p->rcTam.left   = v * " + W + ";",
        T*3 + "Ini.GetInteger(szMuc, \"ViewAreaTop\",    0, &v); p->rcTam.top    = v * " + H + ";",
        T*3 + "Ini.GetInteger(szMuc, \"ViewAreaRight\",  0, &v); p->rcTam.right  = v * " + W + ";",
        T*3 + "Ini.GetInteger(szMuc, \"ViewAreaBottom\", 0, &v); p->rcTam.bottom = v * " + H + ";",
        T*3 + "Ini.GetInteger(szMuc, \"Speed\", 2, &p->nToc);",
        T*3 + "Ini.GetInteger(szMuc, \"Angle\", 0, &p->nGoc);",
        T*3 + "Ini.GetInteger(szMuc, \"Count\", 0, &p->nSo);",
        T*3 + "if (p->nSo > LC_MAX_MAY) p->nSo = LC_MAX_MAY;",
        T*3 + "for (int k = 0; k < LC_MAX_ANH; k++)",
        T*3 + "{",
        T*4 + "char szKhoa[16];",
        T*4 + "sprintf(szKhoa, \"Image%d\", k);",
        T*4 + "Ini.GetString(szMuc, szKhoa, \"\", p->szAnh[k], sizeof(p->szAnh[k]));",
        T*4 + "if (p->szAnh[k][0] == 0) break;",
        T*4 + "p->nSoAnh = k + 1;",
        T*3 + "}",
        T*2 + "}",
        T*2 + "if (p->nSoAnh <= 0)",
        T*3 + "continue;" + T + "// khong co anh thi bo lop nay",
        T*2 + "nDem[nKieu]++;",
        T*2 + "m_nLopCanh++;",
        T + "}",
        "",
        T + "extern int g_nCorePaintLog;",
        T + "if (g_nCorePaintLog > 0 && m_nLopCanh > 0)",
        T + "{",
        T*2 + "FILE* pLog = fopen(\"jx_paint.log\", \"a\");",
        T*2 + "if (pLog)",
        T*2 + "{",
        T*3 + "fprintf(pLog, \"[ANHNEN] map %d: %d lop (nen %d, phu nen %d, tien canh %d) | %s\\n\",",
        T*4 + "nPlaceIndex, m_nLopCanh, nDem[1], nDem[3], nDem[2], szTep);",
        T*3 + "fclose(pLog);",
        T*2 + "}",
        T + "}",
        "}",
        "",
        "void KScenePlaceC::VeLopCanh(int nKieu)",
        "{",
        T + "if (m_nLopCanh <= 0 || g_pRepresent == NULL)",
        T*2 + "return;",
        T + "for (int i = 0; i < m_nLopCanh; i++)",
        T + "{",
        T*2 + "KLopCanh* p = &m_LopCanh[i];",
        T*2 + "if (p->nKieu != nKieu)",
        T*3 + "continue;",
        T*2 + "// ban 2.0 so sanh chat hai dau: chi ve khi tieu diem nam HAN trong vung kich hoat",
        T*2 + "if (p->rcVung.left >= m_FocusPosition.x || p->rcVung.right  <= m_FocusPosition.x ||",
        T*3 + "p->rcVung.top  >= m_FocusPosition.y || p->rcVung.bottom <= m_FocusPosition.y)",
        T*2 + "{",
        T*3 + "p->bDaDat = 0;",
        T*3 + "continue;",
        T*2 + "}",
        T*2 + "if (nKieu == 1)",
        T*3 + "VeLopNen(p);",
        T*2 + "else",
        T*3 + "VeLopMay(p);",
        T + "}",
        "}",
        "",
        "void KScenePlaceC::VeLopNen(KLopCanh* p)",
        "{",
        T + "KRUImage Img;",
        T + "memset(&Img, 0, sizeof(Img));",
        T + "Img.nType = ISI_T_BITMAP16;",
        T + "Img.bRenderStyle = IMAGE_RENDER_STYLE_OPACITY;",
        T + "Img.bRenderFlag = 0;",
        T + "Img.Color.Color_dw = 0xffffffff;",
        T + "Img.nISPosition = IMAGE_IS_POSITION_INIT;",
        T + "Img.nFrame = 0;",
        T + "Img.uImage = 0;",
        T + "strncpy(Img.szImage, p->szAnh[0], sizeof(Img.szImage) - 1);",
        "",
        T + "// thi sai: canh dich nTiLe diem thi nen dich 1 diem (chu thich goc trong _Scroll.ini)",
        T + "int nDoiX = (m_FocusPosition.x - p->rcVung.left - p->nTamCanhX) / p->nTiLe;",
        T + "int nDoiY = (m_FocusPosition.y - p->rcVung.top  - p->nTamCanhY) / p->nTiLe;",
        T + "int x = p->nTamAnhX - nDoiX;",
        T + "int y = p->nTamAnhY - nDoiY;",
        "",
        T + "// khong de ho vien: neu anh lon hon khung ve thi keo lai cho phu kin, nho hon thi dat giua",
        T + "KImageParam Param;",
        T + "memset(&Param, 0, sizeof(Param));",
        T + "if (g_pRepresent->GetImageParam(Img.szImage, &Param, ISI_T_BITMAP16) && Param.nWidth > 0 && Param.nHeight > 0)",
        T + "{",
        T*2 + "int nRong = p->rcMan.right - p->rcMan.left;",
        T*2 + "int nCao  = p->rcMan.bottom - p->rcMan.top;",
        T*2 + "if ((int)Param.nWidth >= nRong)",
        T*2 + "{",
        T*3 + "if (x > p->rcMan.left) x = p->rcMan.left;",
        T*3 + "if (x + (int)Param.nWidth < p->rcMan.right) x = p->rcMan.right - (int)Param.nWidth;",
        T*2 + "}",
        T*2 + "else",
        T*3 + "x = p->rcMan.left + (nRong - (int)Param.nWidth) / 2;",
        T*2 + "if ((int)Param.nHeight >= nCao)",
        T*2 + "{",
        T*3 + "if (y > p->rcMan.top) y = p->rcMan.top;",
        T*3 + "if (y + (int)Param.nHeight < p->rcMan.bottom) y = p->rcMan.bottom - (int)Param.nHeight;",
        T*2 + "}",
        T*2 + "else",
        T*3 + "y = p->rcMan.top + (nCao - (int)Param.nHeight) / 2;",
        T + "}",
        T + "Img.oPosition.nX = x;",
        T + "Img.oPosition.nY = y;",
        T + "Img.oPosition.nZ = 0;",
        T + "g_pRepresent->DrawPrimitives(1, &Img, RU_T_IMAGE, true);" + T + "// true = toa do man hinh",
        "}",
        "",
        "void KScenePlaceC::VeLopMay(KLopCanh* p)",
        "{",
        T + "if (p->nSo <= 0 || p->nSoAnh <= 0)",
        T*2 + "return;",
        T + "int nRong = p->rcMan.right - p->rcMan.left;",
        T + "int nCao  = p->rcMan.bottom - p->rcMan.top;",
        T + "if (nRong <= 0 || nCao <= 0)",
        T*2 + "return;",
        "",
        T + "DWORD dwNay = timeGetTime();",
        T + "if (p->bDaDat == 0)",
        T + "{",
        T*2 + "p->bDaDat = 1;",
        T*2 + "p->dwMoc = dwNay;",
        T*2 + "for (int k = 0; k < p->nSo; k++)",
        T*2 + "{",
        T*3 + "p->nMayX[k] = (p->rcMan.left + (rand() % nRong)) << 4;",
        T*3 + "p->nMayY[k] = (p->rcMan.top  + (rand() % nCao))  << 4;",
        T*3 + "p->nMayAnh[k] = rand() % p->nSoAnh;",
        T*2 + "}",
        T + "}",
        "",
        T + "// Angle thang 64: 0 = thang len, thuan kim dong ho. Toc do: nToc * 8 diem anh moi giay.",
        T + "DWORD dwCach = dwNay - p->dwMoc;",
        T + "if (dwCach > 200) dwCach = 200;" + T + "// bo qua khung dai (doi map, nap anh)",
        T + "p->dwMoc = dwNay;",
        T + "double dGoc = (double)p->nGoc * 3.14159265358979 / 32.0;",
        T + "int nDiX = (int)(sin(dGoc) * (double)p->nToc * 128.0 * (double)dwCach / 1000.0);",
        T + "int nDiY = (int)(-cos(dGoc) * (double)p->nToc * 128.0 * (double)dwCach / 1000.0);",
        T + "int nLe = 512 << 4;" + T + "// vien ra ngoai khung de may khong bi nhay",
        "",
        T + "KRUImage aImg[LC_MAX_MAY];",
        T + "int nSo = 0;",
        T + "for (int k = 0; k < p->nSo; k++)",
        T + "{",
        T*2 + "p->nMayX[k] += nDiX;",
        T*2 + "p->nMayY[k] += nDiY;",
        T*2 + "if (p->nMayX[k] < (p->rcMan.left << 4) - nLe)  p->nMayX[k] += (nRong << 4) + nLe;",
        T*2 + "if (p->nMayX[k] > (p->rcMan.right << 4) + nLe) p->nMayX[k] -= (nRong << 4) + nLe;",
        T*2 + "if (p->nMayY[k] < (p->rcMan.top << 4) - nLe)    p->nMayY[k] += (nCao << 4) + nLe;",
        T*2 + "if (p->nMayY[k] > (p->rcMan.bottom << 4) + nLe) p->nMayY[k] -= (nCao << 4) + nLe;",
        "",
        T*2 + "KRUImage* q = &aImg[nSo];",
        T*2 + "memset(q, 0, sizeof(*q));",
        T*2 + "q->nType = ISI_T_SPR;",
        T*2 + "q->bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;",
        T*2 + "q->bRenderFlag = 0;",
        T*2 + "q->Color.Color_dw = 0xffffffff;",
        T*2 + "q->nISPosition = IMAGE_IS_POSITION_INIT;",
        T*2 + "q->nFrame = 0;",
        T*2 + "q->uImage = 0;",
        T*2 + "strncpy(q->szImage, p->szAnh[p->nMayAnh[k]], sizeof(q->szImage) - 1);",
        T*2 + "q->oPosition.nX = p->nMayX[k] >> 4;",
        T*2 + "q->oPosition.nY = p->nMayY[k] >> 4;",
        T*2 + "q->oPosition.nZ = 0;",
        T*2 + "nSo++;",
        T + "}",
        T + "if (nSo > 0)",
        T*2 + "g_pRepresent->DrawPrimitives(nSo, &aImg[0], RU_T_IMAGE, true);",
        "}",
        "",
        neo])
    s = rep(s, neo, than, "C than ham")
    ghi(P, s, h0, lf0, crlf0, "KScenePlaceC.cpp")
else:
    print("KScenePlaceC.cpp da co")
print("XONG " + TAG)
