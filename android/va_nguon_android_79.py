# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 OSO / OTRONG / KHOAMT / ICON] Bon yeu cau cua chu (11/09 toi), chi mobile, PC khong doi:
#  1. O vat pham so 1-4 (UiPlayerBar.cpp): cham (chuot trai, tay khong) = DUNG ngay; giu lau (chuot phai) = NHAC LEN TAY
#     de bo vao ruong. PC: trai = nhac, phai = dung (giu nguyen, rao JX_ANDROID).
#  2. O ky nang phu: "Go khoi o" -> o de TRONG that (Trong<i>=1 trong KyNangMobile.ini), khong tu lay ky nang ke tiep
#     theo danh sach (KyNang_CuaNut) nua. Go ca ky nang dang hien tu danh sach mac dinh. Gan lai -> bo co trong.
#  3. Cham nguoi choi/NPC de xem thong tin: khoa muc tieu da cham (KPlayer.cpp, JX_ANDROID) - keo ngon di cho khac
#     (cam can, keo cua so, hover gia lap) khong doi muc tieu; cham cho khac moi doi/bo.
#  4. Icon tren dau NPC doi thoai gan nhat: dung nut "Giao tiep" cua kho VNKU (\spr\UiNew\InterRact\giao_tiep.spr, ve
#     rong IconNpcRong=112), cham vao icon = bam vao than NPC (di toi + mo thoai) ke ca khi icon nam ngoai hinh NPC.
# Chay lai vo hai. Doc/ghi latin-1, giu CRLF, chi them dong ASCII: byte cao khong doi.

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 11/09 OSO]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def nl_cua(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def thay(s, cu, moi, ten, so=1):
    nl = nl_cua(s)
    c = nl.join(cu)
    if s.count(c) == so:
        return s.replace(c, nl.join(moi))
    raise SystemExit("khong tim thay dung %d cho (%d): %s" % (so, s.count(c), ten))


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


def va(p, dau, ham):
    s = doc(p)
    if dau in s:
        print("da va roi, bo qua:", p)
        return
    s2 = ham(s)
    if cao(s) != cao(s2):
        raise SystemExit("so byte cao doi o " + p)
    ghi(p, s2)
    print("da va:", p)


# ---------------------------------------------------------------- 1. UiPlayerBar.cpp
def va_playerbar(s):
    s = thay(s, ["\tcase WND_N_ITEM_PICKDROP:",
                 "\t\tif (g_UiBase.IsOperationEnable(UIS_O_MOVE_ITEM))",
                 "\t\t\tOnObjPickedDropped((ITEM_PICKDROP_PLACE*)uParam, (ITEM_PICKDROP_PLACE*)nParam);",
                 "\t\tbreak;"],
                ["\tcase WND_N_ITEM_PICKDROP:",
                 "#ifdef JX_ANDROID",
                 "\t\t// %s Cham (chuot trai) o vat pham so 1-4 khi tay khong = DUNG ngay (PC: nhac len tay); giu lau (chuot phai)" % DAU,
                 "\t\t// = nhac len tay de bo vao ruong (WND_N_RIGHT_CLICK_ITEM). Chu: \"bo item vao phim so thi cham vao la su dung luon",
                 "\t\t// khong can de vai giay - de vao vai giay vao o phim so la lay ra tren tay de bo vao ruong\". Bo xuong o (tay dang cam) nhu cu.",
                 "\t\tif (uParam && !nParam)",
                 "\t\t{",
                 "\t\t\tint nOSo = -1;",
                 "\t\t\tfor (int i = 0; i < UPB_IMMEDIA_ITEM_COUNT; i++)",
                 "\t\t\t\tif (((ITEM_PICKDROP_PLACE*)uParam)->pWnd == (KWndWindow*)&m_ImmediaItem[i])",
                 "\t\t\t\t\tnOSo = i;",
                 "\t\t\tif (nOSo >= 0)",
                 "\t\t\t{",
                 "\t\t\t\tif (g_UiBase.IsOperationEnable(UIS_O_USE_ITEM))",
                 "\t\t\t\t\tOnUseItem(nOSo);",
                 "\t\t\t\tbreak;",
                 "\t\t\t}",
                 "\t\t}",
                 "#endif",
                 "\t\tif (g_UiBase.IsOperationEnable(UIS_O_MOVE_ITEM))",
                 "\t\t\tOnObjPickedDropped((ITEM_PICKDROP_PLACE*)uParam, (ITEM_PICKDROP_PLACE*)nParam);",
                 "\t\tbreak;"], "UiPlayerBar.cpp: PICKDROP")
    s = thay(s, ["\tcase WND_N_RIGHT_CLICK_ITEM:",
                 "\t\tif (g_UiBase.IsOperationEnable(UIS_O_USE_ITEM))",
                 "\t\t{",
                 "\t\t\tfor (int i = 0; i < UPB_IMMEDIA_ITEM_COUNT; i++)",
                 "\t\t\t\tif (nParam == (KNPARAM)(KWndWindow*)&m_ImmediaItem[i])",
                 "\t\t\t\t\tOnUseItem(i);",
                 "\t\t}",
                 "\t\tbreak;"],
                ["\tcase WND_N_RIGHT_CLICK_ITEM:",
                 "#ifdef JX_ANDROID",
                 "\t\t// %s giu lau (chuot phai) o vat pham so = NHAC LEN TAY (PC: dung); tay dang cam gi thi bo qua" % DAU,
                 "\t\tif (g_UiBase.IsOperationEnable(UIS_O_MOVE_ITEM) && !Wnd_GetDragObj(NULL))",
                 "\t\t{",
                 "\t\t\tfor (int i = 0; i < UPB_IMMEDIA_ITEM_COUNT; i++)",
                 "\t\t\t\tif (nParam == (KNPARAM)(KWndWindow*)&m_ImmediaItem[i])",
                 "\t\t\t\t{",
                 "\t\t\t\t\tITEM_PICKDROP_PLACE Pick;",
                 "\t\t\t\t\tPick.pWnd = &m_ImmediaItem[i];",
                 "\t\t\t\t\tPick.h = 0;",
                 "\t\t\t\t\tPick.v = 0;",
                 "\t\t\t\t\tOnObjPickedDropped(&Pick, NULL);",
                 "\t\t\t\t}",
                 "\t\t}",
                 "\t\tbreak;",
                 "#else",
                 "\t\tif (g_UiBase.IsOperationEnable(UIS_O_USE_ITEM))",
                 "\t\t{",
                 "\t\t\tfor (int i = 0; i < UPB_IMMEDIA_ITEM_COUNT; i++)",
                 "\t\t\t\tif (nParam == (KNPARAM)(KWndWindow*)&m_ImmediaItem[i])",
                 "\t\t\t\t\tOnUseItem(i);",
                 "\t\t}",
                 "\t\tbreak;",
                 "#endif"], "UiPlayerBar.cpp: RIGHT_CLICK_ITEM")
    return s


# ---------------------------------------------------------------- 2 + 4. JxCanDieuKhien.cpp
def va_can(s):
    D2 = "[ANDROID 11/09 OTRONG]"
    D4 = "[ANDROID 11/09 ICON b]"
    s = thay(s, ["static KUiGameObject s_KNGan[KYNANG_SO_PHU];\t// ky nang nguoi choi tu gan cho tung o"],
             ["static KUiGameObject s_KNGan[KYNANG_SO_PHU];\t// ky nang nguoi choi tu gan cho tung o",
              "static int\t\t\ts_nKNTrong[KYNANG_SO_PHU];\t// %s 1 = nguoi choi da GO ky nang khoi o nay -> o TRONG that, khong tu lay theo danh sach (Trong<i>=1)" % D2],
             "JxCanDieuKhien.cpp: khai bao s_KNGan")
    s = thay(s, ["\tmemset(s_KNGan, 0, sizeof(s_KNGan));",
                 "\tmemset(&s_KNChinhGan, 0, sizeof(s_KNChinhGan));\t// [ANDROID 10/09 BANGCHON]"],
             ["\tmemset(s_KNGan, 0, sizeof(s_KNGan));",
              "\tmemset(s_nKNTrong, 0, sizeof(s_nKNTrong));\t// %s" % D2,
              "\tmemset(&s_KNChinhGan, 0, sizeof(s_KNChinhGan));\t// [ANDROID 10/09 BANGCHON]"], "DocGan memset")
    s = thay(s, ["\t\telse if (sscanf(szDong, \"Chinh=%d,%d\", &nLoai, &nMa) == 2)"],
             ["\t\telse if (sscanf(szDong, \"Trong%d=%d\", &nO, &nMa) == 2 && nO >= 0 && nO < KYNANG_SO_PHU)",
              "\t\t\ts_nKNTrong[nO] = nMa ? 1 : 0;\t// %s" % D2,
              "\t\telse if (sscanf(szDong, \"Chinh=%d,%d\", &nLoai, &nMa) == 2)"], "DocGan Trong")
    s = thay(s, ["\t\tif (s_KNGan[i].uId)",
                 "\t\t\tfprintf(pTep, \"O%d=%u,%u\\n\", i, s_KNGan[i].uGenre, s_KNGan[i].uId);"],
             ["\t\tif (s_KNGan[i].uId)",
              "\t\t\tfprintf(pTep, \"O%d=%u,%u\\n\", i, s_KNGan[i].uGenre, s_KNGan[i].uId);",
              "\t\telse if (s_nKNTrong[i])",
              "\t\t\tfprintf(pTep, \"Trong%d=1\\n\", i);\t// " + D2], "GhiGan Trong")
    s = thay(s, ["\tif (s_KNGan[nNut - 1].uId)",
                 "\t{",
                 "\t\t*pRa = s_KNGan[nNut - 1];",
                 "\t\treturn true;",
                 "\t}"],
             ["\tif (s_KNGan[nNut - 1].uId)",
              "\t{",
              "\t\t*pRa = s_KNGan[nNut - 1];",
              "\t\treturn true;",
              "\t}",
              "\tif (s_nKNTrong[nNut - 1])",
              "\t{\t// %s nguoi choi da go -> o TRONG that (chu: \"go ky nang la no tu add ky nang khac vao o da go\")" % D2,
              "\t\tmemset(pRa, 0, sizeof(*pRa));",
              "\t\treturn false;",
              "\t}"], "CuaNut o trong")
    s = thay(s, ["\ts_KNGan[s_nKNOChon].uGenre = uGenre;",
                 "\ts_KNGan[s_nKNOChon].uId    = uId;"],
             ["\ts_KNGan[s_nKNOChon].uGenre = uGenre;",
              "\ts_KNGan[s_nKNOChon].uId    = uId;",
              "\ts_nKNTrong[s_nKNOChon] = 0;\t// %s gan lai thi het trong" % D2], "gan ky nang bo co trong")
    s = thay(s, ["\t\tif (s_KNGan[i].uId == p->uId && s_KNGan[i].uGenre == p->uGenre)",
                 "\t\t{",
                 "\t\t\tmemset(&s_KNGan[i], 0, sizeof(s_KNGan[i]));",
                 "\t\t\tnGo++;",
                 "\t\t}"],
             ["\t\tKUiGameObject oO;",
              "\t\t// %s go ca ky nang dang hien o o do theo danh sach mac dinh (chua tu gan) - o nao dang HIEN ky nang nay thi go" % D2,
              "\t\tif ((s_KNGan[i].uId == p->uId && s_KNGan[i].uGenre == p->uGenre)",
              "\t\t\t|| (s_KNGan[i].uId == 0 && !s_nKNTrong[i] && KyNang_CuaNut(i + 1, &oO) && oO.uId == p->uId && oO.uGenre == p->uGenre))",
              "\t\t{",
              "\t\t\tmemset(&s_KNGan[i], 0, sizeof(s_KNGan[i]));",
              "\t\t\ts_nKNTrong[i] = 1;\t// %s o de trong, khong tu lay ky nang ke tiep" % D2,
              "\t\t\tnGo++;",
              "\t\t}"], "GoKhoiO")
    # ---- 4. icon NPC
    s = thay(s, ["static char\ts_szIconAnh[128] = \"\\\\spr\\\\obj\\\\box\\\\YellowPoint.spr\";"],
             ["static char\ts_szIconAnh[128] = \"\\\\spr\\\\UiNew\\\\InterRact\\\\giao_tiep.spr\";\t// %s nut \"Giao tiep\" cua kho VNKU (224x50)" % D4,
              "static int\ts_nIconRong = 112;\t\t// %s be rong ve icon (px); cao theo ti le anh. [Cham] IconNpcRong" % D4,
              "static int\ts_nIconVeX0 = 0, s_nIconVeY0 = 0, s_nIconVeX1 = 0, s_nIconVeY1 = 0;\t// %s khung icon vua ve (man hinh)" % D4,
              "static int\ts_nIconNpcX = 0, s_nIconNpcY = 0;\t// %s chan NPC (man hinh) cua icon vua ve" % D4,
              "static unsigned int\ts_uIconVeLuc = 0;\t// %s luc ve icon gan nhat (0 = chua ve)" % D4],
             "khai bao icon NPC")
    s = thay(s, ["\ts_nIconCao   = GetPrivateProfileInt(\"Cham\", \"IconNpcCao\", 62, szCfg);"],
             ["\ts_nIconCao   = GetPrivateProfileInt(\"Cham\", \"IconNpcCao\", 72, szCfg);\t// %s mep TREN cua icon cao hon chan NPC bao nhieu" % D4,
              "\ts_nIconRong  = GetPrivateProfileInt(\"Cham\", \"IconNpcRong\", 112, szCfg);\t// %s" % D4,
              "\tif (s_nIconRong < 24) s_nIconRong = 24;",
              "\tif (s_nIconRong > 400) s_nIconRong = 400;"], "DocCaiDat IconNpcCao")
    # VeAnhRong sau VeAnhCo
    s = thay(s, ["\tg_pRepresentShell->DrawPrimitives(1, &a, RU_T_IMAGE_STRETCH, true);",
                 "}",
                 "",
                 "//\tSo khung cua mot anh .spr (mui ten danh co mot khung cho moi huong)."],
             ["\tg_pRepresentShell->DrawPrimitives(1, &a, RU_T_IMAGE_STRETCH, true);",
              "}",
              "",
              "//\t%s Ve anh keo can theo rong x cao (khong vuong) - cho nut \"Giao tiep\" tren dau NPC." % D4,
              "static void VeAnhRong(const char* pszAnh, int nX, int nY, int nRong, int nCao)",
              "{",
              "\tKRUImage a;",
              "",
              "\tif (g_pRepresentShell == NULL || nRong < 2 || nCao < 2)",
              "\t\treturn;",
              "\tmemset(&a, 0, sizeof(a));",
              "\ta.nType = ISI_T_SPR;",
              "\ta.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;",
              "\ta.Color.Color_dw = 0xffffffff;",
              "\ta.nISPosition = IMAGE_IS_POSITION_INIT;",
              "\ta.nFrame = 0;",
              "\tstrncpy(a.szImage, pszAnh, sizeof(a.szImage) - 1);",
              "\ta.oPosition.nX = nX;",
              "\ta.oPosition.nY = nY;",
              "\ta.oPosition.nZ = 0;",
              "\ta.oEndPos.nX = nX + nRong;",
              "\ta.oEndPos.nY = nY + nCao;",
              "\ta.oEndPos.nZ = 0;",
              "\tg_pRepresentShell->DrawPrimitives(1, &a, RU_T_IMAGE_STRETCH, true);",
              "}",
              "",
              "//\tSo khung cua mot anh .spr (mui ten danh co mot khung cho moi huong)."], "VeAnhRong")
    # than ve icon: thay tu 'static KRUImage s_Icon;' toi DrawPrimitives
    s = thay(s, ["\tstatic KRUImage s_Icon;",
                 "\tif (s_Icon.szImage[0] == 0)",
                 "\t{",
                 "\t\tmemset(&s_Icon, 0, sizeof(s_Icon));",
                 "\t\ts_Icon.nType = ISI_T_SPR;",
                 "\t\ts_Icon.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;",
                 "\t\ts_Icon.Color.Color_dw = 0xffffffff;",
                 "\t\ts_Icon.nISPosition = IMAGE_IS_POSITION_INIT;",
                 "\t\ts_Icon.nFrame = 0;",
                 "\t\tstrncpy(s_Icon.szImage, s_szIconAnh, sizeof(s_Icon.szImage) - 1);",
                 "\t}",
                 "\tKRPosition2 oOffI = { 0, 0 }, oCoI = { 0, 0 };",
                 "\tint nLui = 0;",
                 "\tif (g_pRepresentShell->GetImageFrameParam(s_Icon.szImage, 0, &oOffI, &oCoI, s_Icon.nType) && oCoI.nX > 0)",
                 "\t\tnLui = oCoI.nX / 2;",
                 "\ts_Icon.oPosition.nX = x - nLui;",
                 "\ts_Icon.oPosition.nY = y;",
                 "\tg_pRepresentShell->DrawPrimitives(1, &s_Icon, RU_T_IMAGE, true);\t// true = toa do MAN HINH"],
             ["\t// %s ve nut \"Giao tiep\" keo can rong s_nIconRong, mep tren tai y (= chan - IconNpcCao); nho khung de cham" % D4,
              "\tKRPosition2 oOffI = { 0, 0 }, oCoI = { 0, 0 };",
              "\tint nRong = s_nIconRong, nCao = s_nIconRong / 4;",
              "\tif (g_pRepresentShell->GetImageFrameParam(s_szIconAnh, 0, &oOffI, &oCoI, ISI_T_SPR) && oCoI.nX > 0 && oCoI.nY > 0)",
              "\t\tnCao = nRong * oCoI.nY / oCoI.nX;",
              "\tVeAnhRong(s_szIconAnh, x - nRong / 2, y, nRong, nCao);",
              "\ts_nIconVeX0 = x - nRong / 2 - 8; s_nIconVeX1 = x + nRong / 2 + 8;",
              "\ts_nIconVeY0 = y - 8;             s_nIconVeY1 = y + nCao + 8;",
              "\ts_nIconNpcX = x; s_nIconNpcY = y + s_nIconCao;",
              "\ts_uIconVeLuc = (unsigned int)GetTickCount();"], "than JxIconNpc_Ve")
    # ham cham icon: them truoc 'void JxCan_Ve()'
    s = thay(s, ["void JxCan_Ve()", "{"],
             ["//\t%s Cham trung icon \"Giao tiep\" vua ve (trong 300 ms)? -> tra toa do THAN NPC (chan - 28) de KSdlApp bam vao" % D4,
              "//\tnhu cham thang NPC: di toi + mo thoai. Chu: \"khi bam vao icon do thi se tu di chuyen toi npc mo hoi thoai\".",
              "int JxIconNpc_Cham(int x, int y, int* pnX, int* pnY)",
              "{",
              "\tif (!s_nIconBat || s_uIconVeLuc == 0 || (unsigned int)GetTickCount() - s_uIconVeLuc > 300)",
              "\t\treturn 0;",
              "\tif (x < s_nIconVeX0 || x > s_nIconVeX1 || y < s_nIconVeY0 || y > s_nIconVeY1)",
              "\t\treturn 0;",
              "\t*pnX = s_nIconNpcX;",
              "\t*pnY = s_nIconNpcY - 28;",
              "\treturn 1;",
              "}",
              "",
              "void JxCan_Ve()", "{"], "JxIconNpc_Cham")
    return s


def va_can_h(s):
    return thay(s, ["void JxIconNpc_Ve();"],
                ["void JxIconNpc_Ve();",
                 "int  JxIconNpc_Cham(int x, int y, int* pnX, int* pnY);\t// [ANDROID 11/09 ICON b] cham trung icon Giao tiep -> toa do than NPC de bam"],
                "JxCanDieuKhien.h")


# ---------------------------------------------------------------- 4. KSdlApp.cpp: cham icon
def va_sdlapp(s):
    return thay(s, ["\t\t\tif (JxKyNang_ChamBangChon(m_nChamX0, m_nChamY0))",
                    "\t\t\t{",
                    "\t\t\t\tm_nCham = CHAM_KHONG;",
                    "\t\t\t\treturn true;",
                    "\t\t\t}"],
                   ["\t\t\tif (JxKyNang_ChamBangChon(m_nChamX0, m_nChamY0))",
                    "\t\t\t{",
                    "\t\t\t\tm_nCham = CHAM_KHONG;",
                    "\t\t\t\treturn true;",
                    "\t\t\t}",
                    "\t\t\t{\t// [ANDROID 11/09 ICON b] cham icon \"Giao tiep\" tren dau NPC = bam vao than NPC (di toi + mo thoai),",
                    "\t\t\t\t// ke ca khi icon nam ngoai hinh NPC. Chu: \"khi bam vao icon do thi se tu di chuyen toi npc mo hoi thoai\".",
                    "\t\t\t\tint nNpcX = 0, nNpcY = 0;",
                    "\t\t\t\tif (!UiToaDo_DangSua() && JxIconNpc_Cham(m_nChamX0, m_nChamY0, &nNpcX, &nNpcY))",
                    "\t\t\t\t{",
                    "\t\t\t\t\tLPARAM lNpc = MAKELPARAM(nNpcX, nNpcY);",
                    "\t\t\t\t\tGhiChuot(0, lNpc);",
                    "\t\t\t\t\tMsgProc(hWnd, WM_MOUSEMOVE, 0, lNpc);",
                    "\t\t\t\t\tGhiChuot(MK_LBUTTON, lNpc);",
                    "\t\t\t\t\tMsgProc(hWnd, WM_LBUTTONDOWN, MK_LBUTTON, lNpc);",
                    "\t\t\t\t\tGhiChuot(0, lNpc);",
                    "\t\t\t\t\tMsgProc(hWnd, WM_LBUTTONUP, 0, lNpc);",
                    "\t\t\t\t\tg_DebugLog(\"[ICON] cham icon giao tiep -> bam NPC tai %d,%d\", nNpcX, nNpcY);",
                    "\t\t\t\t\tm_nCham = CHAM_KHONG;",
                    "\t\t\t\t\treturn true;",
                    "\t\t\t\t}",
                    "\t\t\t}"], "KSdlApp.cpp: cham icon NPC")


# ---------------------------------------------------------------- 3. KPlayer.cpp: khoa muc tieu da cham
def va_kplayer(s):
    D3 = "[ANDROID 11/09 KHOAMT]"
    s = thay(s, ["void KPlayer::OnButtonDown(int x,int y, int Key, MOUSE_BUTTON nButton)",
                 "{",
                 "\tm_MouseDown[(int)nButton] = TRUE;",
                 "\t",
                 "\tFindSelectNpc(x, y, relation_all);",
                 "\tFindSelectObject(x, y);"],
                ["#ifdef JX_ANDROID",
                 "int g_nJxMucTieuKhoa = 0;\t// %s NPC/nguoi choi da CHAM (OnButtonDown); 0 = khong khoa. Chi client Android." % D3,
                 "#endif",
                 "",
                 "void KPlayer::OnButtonDown(int x,int y, int Key, MOUSE_BUTTON nButton)",
                 "{",
                 "\tm_MouseDown[(int)nButton] = TRUE;",
                 "\t",
                 "\tFindSelectNpc(x, y, relation_all);",
                 "\tFindSelectObject(x, y);",
                 "#ifdef JX_ANDROID",
                 "\tg_nJxMucTieuKhoa = m_nPeapleIdx;\t// %s cham trung ai thi KHOA muc tieu do; cham dat = bo khoa" % D3,
                 "#endif"], "KPlayer.cpp: OnButtonDown")
    s = thay(s, ["void KPlayer::OnMouseMove(int x,int y)",
                 "{",
                 "\tm_nPeapleIdx = 0;",
                 "\tm_nObjectIdx = 0;",
                 "\tFindSelectNpc(x, y, relation_all);",
                 "\tFindSelectObject(x, y);"],
                ["void KPlayer::OnMouseMove(int x,int y)",
                 "{",
                 "\tm_nPeapleIdx = 0;",
                 "\tm_nObjectIdx = 0;",
                 "\tFindSelectNpc(x, y, relation_all);",
                 "\tFindSelectObject(x, y);",
                 "#ifdef JX_ANDROID",
                 "\t// %s dien thoai khong co \"re chuot\": ngon tay keo di (cam can, keo cua so, hover gia lap) KHONG doi muc tieu" % D3,
                 "\t// dang xem; giu muc tieu da cham cho toi khi cham cho khac (OnButtonDown). Chu: \"kich vao hien thong tin",
                 "\t// neu lo di chuot di cho khac la no doi muc tieu\".",
                 "\tif (g_nJxMucTieuKhoa > 0)",
                 "\t{",
                 "\t\tif (g_nJxMucTieuKhoa >= MAX_NPC || Npc[g_nJxMucTieuKhoa].m_dwID == 0 || Npc[g_nJxMucTieuKhoa].m_RegionIndex < 0)",
                 "\t\t\tg_nJxMucTieuKhoa = 0;",
                 "\t\telse",
                 "\t\t\tm_nPeapleIdx = g_nJxMucTieuKhoa;",
                 "\t}",
                 "#endif"], "KPlayer.cpp: OnMouseMove")
    return s


# ---------------------------------------------------------------- config lop ghi de
def va_config(s):
    return thay(s, ["LuanChuyenMs=300"],
                ["LuanChuyenMs=300",
                 "; [ANDROID 11/09 ICON b] icon tren dau NPC doi thoai gan nhat: nut \"Giao tiep\" cua kho VNKU; cham vao = di toi NPC + mo thoai.",
                 ";   IconNpcRong = be rong ve (px, cao theo ti le anh); IconNpcCao = mep tren icon cao hon chan NPC bao nhieu px; IconNpc=0 de tat.",
                 "IconNpcAnh=\\spr\\UiNew\\InterRact\\giao_tiep.spr",
                 "IconNpcRong=112",
                 "IconNpcCao=72"], "config: [Cham] icon NPC")


va("Sources/S3Client/Ui/UiCase/UiPlayerBar.cpp", DAU, va_playerbar)
va("Sources/S3Client/Platform/JxCanDieuKhien.cpp", "[ANDROID 11/09 OTRONG]", va_can)
va("Sources/S3Client/Platform/JxCanDieuKhien.h", "[ANDROID 11/09 ICON b]", va_can_h)
va("Sources/S3Client/Platform/KSdlApp.cpp", "[ANDROID 11/09 ICON b]", va_sdlapp)
va("Sources/Core/Src/KPlayer.cpp", "[ANDROID 11/09 KHOAMT]", va_kplayer)
va("android/du_lieu_ghi_de/config.ini", "[ANDROID 11/09 ICON b]", va_config)
print("xong")
