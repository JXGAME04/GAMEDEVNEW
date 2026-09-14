# -*- coding: latin-1 -*-
# [CAMERA 13/09 TUYCHON] Huong 2 (chu 13/09 22:5x "lam 2 di"): CONG TAC NGUOI CHOI cho lia / nhin rong / lia ve nhanh trong
# Cai dat > Toi uu (KUiOptions2 - cua so co 3 hang trong phia tren 4 cong tac cu; anh may ao opt4.png), luu UiCommon.ini [Options2]
# LiaCanh / NhinRong / LiaVeNhanh qua co che ls_ToggleOptionName2 co san; ap xuong Platform/JxLiaCanh (JxLia_DatTuyChon).
# Hieu luc = config.ini/map (buoc 3) VA cong tac nguoi choi. Ini bo cuc: android/sinh_uioptions2_camera.py (Top 163 -> 101, ten muc 4..6).
# Idempotent, latin-1, neo CRLF. Dung: python android/va_nguon_camera_tuychon_1309.py [goc]
import io, os, sys

GOC = sys.argv[1] if len(sys.argv) > 1 and not sys.argv[1].startswith('--') else os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
KQ = []
R = '\r\n'

def doc(p):
    with io.open(p, 'r', encoding='latin-1', newline='') as f:
        return f.read()

def ghi(p, s):
    with io.open(p, 'w', encoding='latin-1', newline='') as f:
        f.write(s)

def va(p, cu, moi, dau_hieu, so_lan=1):
    s = doc(p)
    if dau_hieu in s:
        KQ.append('bo qua (da co): %s : %s' % (os.path.basename(p), dau_hieu[:50])); return
    n = s.count(cu)
    if n != so_lan:
        raise SystemExit('LOI: neo gap %d lan (can %d) trong %s: %r' % (n, so_lan, p, cu[:90]))
    cao_truoc = sum(1 for c in s if ord(c) >= 0x80)
    s2 = s.replace(cu, moi)
    cao_sau = sum(1 for c in s2 if ord(c) >= 0x80)
    if cao_truoc != cao_sau:
        raise SystemExit('LOI: so byte cao doi %d -> %d o %s' % (cao_truoc, cao_sau, p))
    ghi(p, s2)
    KQ.append('da va: %s : %s' % (os.path.basename(p), dau_hieu[:50]))

S3 = os.path.join(GOC, 'Sources', 'S3Client')
LH = os.path.join(S3, 'Platform', 'JxLiaCanh.h')
LC = os.path.join(S3, 'Platform', 'JxLiaCanh.cpp')
OH = os.path.join(S3, 'Ui', 'UiCase', 'UiOptions2.h')
OC = os.path.join(S3, 'Ui', 'UiCase', 'UiOptions2.cpp')

# ===================== JxLiaCanh.h =====================
va(LH,
   'void JxLia_Ve();' + R,
   'void JxLia_Ve();' + R + R +
   '// [CAMERA 13/09 TUYCHON] Cong tac nguoi choi trong Cai dat > Toi uu (KUiOptions2, luu UiCommon.ini [Options2] LiaCanh/NhinRong/LiaVeNhanh):' + R +
   '// nLia = 0 tat lia mot ngon (chum chi zoom), nZoom = 0 tat chum + ve 100 %, nVeNhanh = 1 lia ve nhanh (cho 1/3, toc do x2). -1 = giu nguyen.' + R +
   '// Hieu luc = config.ini / map (camera_mobile.ini) VA cong tac nay.' + R +
   'void JxLia_DatTuyChon(int nLia, int nZoom, int nVeNhanh);' + R,
   'void JxLia_DatTuyChon(int nLia, int nZoom, int nVeNhanh);')

# ===================== JxLiaCanh.cpp =====================
va(LC,
   'static void\t\t\tCamera_DocMap();' + R,
   'static void\t\t\tCamera_DocMap();' + R +
   '// [CAMERA 13/09 TUYCHON] cong tac nguoi choi (Cai dat > Toi uu); hieu luc = config/map VA nguoi choi' + R +
   'static int\t\t\ts_nNcLia = 1, s_nNcZoom = 1, s_nNcVeNhanh = 0;' + R +
   'static int ChoVeMs() { return s_nNcVeNhanh ? s_nChoVeMs / 3 : s_nChoVeMs; }\t// ve nhanh: cho 1/3' + R +
   'static int VeTocDo() { return s_nNcVeNhanh ? s_nVeTocDo * 2 : s_nVeTocDo; }\t// toc do x2' + R +
   'static int VeEm()    { return s_nNcVeNhanh ? s_nVeEm * 2 : s_nVeEm; }' + R,
   's_nNcLia = 1, s_nNcZoom = 1, s_nNcVeNhanh = 0;')
va(LC,
   '\treturn s_nBat != 0;' + R,
   '\treturn s_nBat && s_nNcLia;\t// [CAMERA 13/09 TUYCHON]' + R,
   '\treturn s_nBat && s_nNcLia;')
va(LC,
   '\treturn s_nBat && TrenBanDo(x, y);' + R,
   '\treturn s_nBat && s_nNcLia && TrenBanDo(x, y);\t// [CAMERA 13/09 TUYCHON] nguoi choi tat lia -> khong lia' + R,
   's_nBat && s_nNcLia && TrenBanDo(x, y)')
va(LC,
   '\tif (!s_nBat)' + R + '\t\treturn;\t// [CAMERA 13/09] LiaCanh=0 (config hay map): van doc map + troi zoom o tren, khong lia' + R,
   '\tif (!s_nBat || !s_nNcLia)' + R + '\t\treturn;\t// [CAMERA 13/09] LiaCanh=0 (config hay map) hay nguoi choi tat: van doc map + troi zoom o tren, khong lia' + R,
   '\tif (!s_nBat || !s_nNcLia)')
va(LC,
   '(unsigned int)s_nChoVeMs',
   '(unsigned int)ChoVeMs()',
   '(unsigned int)ChoVeMs()', so_lan=2)
va(LC,
   '\tfBuoc = (float)s_nVeTocDo * dt;' + R + '\tfEm = fLen * (float)s_nVeEm * dt;' + R,
   '\tfBuoc = (float)VeTocDo() * dt;\t// [CAMERA 13/09 TUYCHON] ve nhanh: x2' + R + '\tfEm = fLen * (float)VeEm() * dt;' + R,
   '(float)VeTocDo() * dt')
va(LC,
   '\tif (!s_nZoomBat) nDich = 100;' + R,
   '\tif (!s_nZoomBat || !s_nNcZoom) nDich = 100;\t// [CAMERA 13/09 TUYCHON] map tat / nguoi choi tat -> 100 %' + R,
   'if (!s_nZoomBat || !s_nNcZoom) nDich = 100;')
va(LC,
   '\tif (!s_nZoomBat)' + R + '\t\treturn false;' + R,
   '\tif (!s_nZoomBat || !s_nNcZoom)' + R + '\t\treturn false;\t// [CAMERA 13/09 TUYCHON]' + R,
   '\tif (!s_nZoomBat || !s_nNcZoom)' + R + '\t\treturn false;')
va(LC,
   '\tif (s_nBat)\t// [CAMERA 13/09] LiaCanh=0 thi chum chi zoom, khong lia' + R,
   '\tif (s_nBat && s_nNcLia)\t// [CAMERA 13/09] LiaCanh=0 / nguoi choi tat lia thi chum chi zoom, khong lia' + R,
   '\tif (s_nBat && s_nNcLia)\t// [CAMERA 13/09]')
va(LC,
   '\tif (s_nBat)' + R + '\t\tJxLia_Keo(',
   '\tif (s_nBat && s_nNcLia)' + R + '\t\tJxLia_Keo(',
   '\tif (s_nBat && s_nNcLia)' + R + '\t\tJxLia_Keo(')
va(LC,
   '\tif (s_nBat)' + R + '\t\tJxLia_Nha();',
   '\tif (s_nBat && s_nNcLia)' + R + '\t\tJxLia_Nha();',
   '\tif (s_nBat && s_nNcLia)' + R + '\t\tJxLia_Nha();')
va(LC,
   'void JxLia_Ve()' + R + '{' + R,
   '// [CAMERA 13/09 TUYCHON] cong tac nguoi choi tu Cai dat > Toi uu (KUiOptions2): goi luc vao the gioi (LoadSetting) va moi lan bam' + R +
   'void JxLia_DatTuyChon(int nLia, int nZoom, int nVeNhanh)' + R +
   '{' + R +
   '\tint nZoomCu = s_nNcZoom;' + R + R +
   '\tif (nLia >= 0) s_nNcLia = nLia ? 1 : 0;' + R +
   '\tif (nZoom >= 0) s_nNcZoom = nZoom ? 1 : 0;' + R +
   '\tif (nVeNhanh >= 0) s_nNcVeNhanh = nVeNhanh ? 1 : 0;' + R +
   '\tif (!s_nNcLia && (s_nTrangThai != LIA_KHONG || s_nDaBatCo))' + R +
   '\t\tKetThuc("nguoi choi tat lia");' + R +
   '\tif (s_nNcZoom != nZoomCu)' + R +
   '\t{\t// tat -> troi ve 100 %; bat lai -> doc lai muc map (zoom da nho / mac dinh map)' + R +
   '\t\tif (!s_nNcZoom) { s_nZoomDich = 100; s_fZoomTroi = (float)s_nZoom; }' + R +
   '\t\telse s_nCanDocMap = 1;' + R +
   '\t}' + R +
   '\tg_DebugLog("[CAMERA] tuy chon nguoi choi: lia=%d nhin rong=%d ve nhanh=%d", s_nNcLia, s_nNcZoom, s_nNcVeNhanh);' + R +
   '}' + R + R +
   'void JxLia_Ve()' + R + '{' + R,
   'void JxLia_DatTuyChon(int nLia, int nZoom, int nVeNhanh)' + R + '{')

# ===================== UiOptions2.h =====================
va(OH,
   '\tOPTION_I_GIAMSKILL,\t\t\t\t' + R + '\tOPTION_INDEX_COUNT2,' + R,
   '\tOPTION_I_GIAMSKILL,\t\t\t\t' + R +
   '#ifdef JX_MOBILE' + R +
   '\tOPTION_I_LIA,\t\t\t// [CAMERA 13/09 TUYCHON] lia canh (mot ngon keo) - Platform/JxLiaCanh' + R +
   '\tOPTION_I_NHINRONG,\t\t// chum hai ngon nhin rong' + R +
   '\tOPTION_I_VENHANH,\t\t// lia ve nhanh' + R +
   '#endif' + R +
   '\tOPTION_INDEX_COUNT2,' + R,
   '\tOPTION_I_LIA,')
va(OH,
   '\tvoid\tUpdateAllStatusImg();' + R + 'private:' + R + '\tstatic KUiOptions2* m_pSelf;' + R,
   '\tvoid\tUpdateAllStatusImg();' + R +
   '#ifdef JX_MOBILE' + R +
   '\tvoid\tPaintWindow();\t// [CAMERA 13/09 TUYCHON] nen mo sau cac hang cong tac (main2.spr trong suot o vung do)' + R +
   '#endif' + R +
   'private:' + R + '\tstatic KUiOptions2* m_pSelf;' + R,
   '\tvoid\tPaintWindow();\t// [CAMERA 13/09 TUYCHON]')
va(OH,
   '\t#define MAX_TOGGLE_BTN_COUNT 4' + R,
   '\t#define MAX_TOGGLE_BTN_COUNT 4' + R +
   '#ifdef JX_MOBILE' + R +
   '#undef MAX_TOGGLE_BTN_COUNT' + R +
   '#define MAX_TOGGLE_BTN_COUNT 7\t// [CAMERA 13/09 TUYCHON] = OPTION_INDEX_COUNT2 (4 cu + lia / nhin rong / ve nhanh): 4 hang tu Top=101 (ini lop ghi de)' + R +
   '\ttypedef char JxKiemSoMucToiUu[(MAX_TOGGLE_BTN_COUNT == OPTION_INDEX_COUNT2) ? 1 : -1];\t// khac nhau = vong khoi tao tran mang' + R +
   '#endif' + R,
   '#define MAX_TOGGLE_BTN_COUNT 7')
# BAY 13/09 22:46 (app sap khi mo Cai dat): UiOptions.cpp include UiOptions.h (KUiOptions mang 4) roi UiOptions2.h -> macro thanh 7
# -> cac vong for cua KUiOptions chay toi 7 tren mang 4 = tran bo nho. Cuoi header tra macro ve 4; UiOptions2.cpp dat lai 7 sau include.
va(OH,
   '\tint\t\t\t\t\tm_nToggleItemCount;' + R + '};' + R,
   '\tint\t\t\t\t\tm_nToggleItemCount;' + R + '};' + R +
   '#ifdef JX_MOBILE' + R +
   '#undef MAX_TOGGLE_BTN_COUNT' + R +
   '#define MAX_TOGGLE_BTN_COUNT 4\t// [CAMERA 13/09 TUYCHON] tra lai 4 cho KUiOptions (UiOptions.cpp include ca hai header, mang cua no chi 4 -> vong for tran); UiOptions2.cpp dat lai 7 sau include' + R +
   '#endif' + R,
   '#define MAX_TOGGLE_BTN_COUNT 4\t// [CAMERA 13/09 TUYCHON] tra lai 4')

# ===================== UiOptions2.cpp =====================
va(OC,
   '#include "../UiChatPhrase.h"' + R,
   '#include "../UiChatPhrase.h"' + R +
   '#ifdef JX_MOBILE' + R +
   '#include "../../Platform/JxLiaCanh.h"\t// [CAMERA 13/09 TUYCHON] JxLia_DatTuyChon' + R +
   'void JxNhip_VeNen(int nX, int nY, int nRong, int nCao);\t// Platform/JxPerfHudAndroid.cpp: nen mo (nhu hang FPS cua KUiOptions)' + R +
   '#undef MAX_TOGGLE_BTN_COUNT' + R +
   '#define MAX_TOGGLE_BTN_COUNT 7\t// [CAMERA 13/09 TUYCHON] lop KUiOptions2 co 7 nut (header tra lai 4 cho KUiOptions) - dat SAU include cuoi' + R +
   '#endif' + R,
   'JxLiaCanh.h"\t// [CAMERA 13/09 TUYCHON]')
# cay da va bang ban dau (chua co dong dat lai 7 trong .cpp)
va(OC,
   'void JxNhip_VeNen(int nX, int nY, int nRong, int nCao);\t// Platform/JxPerfHudAndroid.cpp: nen mo (nhu hang FPS cua KUiOptions)' + R + '#endif' + R,
   'void JxNhip_VeNen(int nX, int nY, int nRong, int nCao);\t// Platform/JxPerfHudAndroid.cpp: nen mo (nhu hang FPS cua KUiOptions)' + R +
   '#undef MAX_TOGGLE_BTN_COUNT' + R +
   '#define MAX_TOGGLE_BTN_COUNT 7\t// [CAMERA 13/09 TUYCHON] lop KUiOptions2 co 7 nut (header tra lai 4 cho KUiOptions) - dat SAU include cuoi' + R +
   '#endif' + R,
   '#define MAX_TOGGLE_BTN_COUNT 7\t// [CAMERA 13/09 TUYCHON] lop KUiOptions2')
va(OC,
   '\t"GiamSkill",' + R + '};' + R,
   '\t"GiamSkill",' + R +
   '#ifdef JX_MOBILE' + R +
   '\t"LiaCanh", "NhinRong", "LiaVeNhanh",\t// [CAMERA 13/09 TUYCHON] luu UiCommon.ini [Options2]' + R +
   '#endif' + R +
   '};' + R,
   '"LiaCanh", "NhinRong", "LiaVeNhanh"')
va(OC,
   '\tcase OPTION_I_GIAMSKILL:' + R +
   '\t\tif (g_pCoreShell)' + R +
   '\t\t\tg_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_QUALITY_GIAMSKILL, bEnable);' + R +
   '\t\tbreak;' + R +
   '\t}' + R +
   '\tUpdateAllStatusImg();' + R,
   '\tcase OPTION_I_GIAMSKILL:' + R +
   '\t\tif (g_pCoreShell)' + R +
   '\t\t\tg_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_QUALITY_GIAMSKILL, bEnable);' + R +
   '\t\tbreak;' + R +
   '#ifdef JX_MOBILE' + R +
   '\tcase OPTION_I_LIA:' + R +
   '\tcase OPTION_I_NHINRONG:' + R +
   '\tcase OPTION_I_VENHANH:\t// [CAMERA 13/09 TUYCHON] ap ngay; luu khi dong cua so (StoreSetting)' + R +
   '\t\tJxLia_DatTuyChon(m_ToggleItemList[OPTION_I_LIA].bEnable, m_ToggleItemList[OPTION_I_NHINRONG].bEnable, m_ToggleItemList[OPTION_I_VENHANH].bEnable);' + R +
   '\t\tbreak;' + R +
   '#endif' + R +
   '\t}' + R +
   '\tUpdateAllStatusImg();' + R,
   '\tcase OPTION_I_VENHANH:\t// [CAMERA 13/09 TUYCHON]')
va(OC,
   '\t\t\t\tpSetting->GetInteger(OPTIONS_SAVE_SECTION2, ls_ToggleOptionName2[i], false, &bOptionsEnable[i]);' + R,
   '\t\t\t\tpSetting->GetInteger(OPTIONS_SAVE_SECTION2, ls_ToggleOptionName2[i], false, &bOptionsEnable[i]);' + R +
   '#ifdef JX_MOBILE' + R +
   '\t\t\tpSetting->GetInteger(OPTIONS_SAVE_SECTION2, ls_ToggleOptionName2[OPTION_I_LIA], true, &bOptionsEnable[OPTION_I_LIA]);\t// [CAMERA 13/09 TUYCHON] mac dinh BAT (chua co khoa)' + R +
   '\t\t\tpSetting->GetInteger(OPTIONS_SAVE_SECTION2, ls_ToggleOptionName2[OPTION_I_NHINRONG], true, &bOptionsEnable[OPTION_I_NHINRONG]);' + R +
   '#endif' + R,
   '[CAMERA 13/09 TUYCHON] mac dinh BAT')
va(OC,
   '\t\tg_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_QUALITY_GIAMSKILL, bOptionsEnable[OPTION_I_GIAMSKILL]);' + R + '\t}' + R,
   '\t\tg_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_QUALITY_GIAMSKILL, bOptionsEnable[OPTION_I_GIAMSKILL]);' + R +
   '#ifdef JX_MOBILE' + R +
   '\t\tJxLia_DatTuyChon(bOptionsEnable[OPTION_I_LIA], bOptionsEnable[OPTION_I_NHINRONG], bOptionsEnable[OPTION_I_VENHANH]);\t// [CAMERA 13/09 TUYCHON] luc vao the gioi + moi lan nap lai' + R +
   '#endif' + R +
   '\t}' + R,
   'JxLia_DatTuyChon(bOptionsEnable[OPTION_I_LIA]')
va(OC,
   'void KUiOptions2::StoreSetting()' + R,
   '#ifdef JX_MOBILE' + R +
   '// [CAMERA 13/09 TUYCHON] nen mo sau cac hang cong tac: main2.spr trong suot o vung tren (cho hang Do sang / Nhac / Am thanh cua cua so chinh)' + R +
   'void KUiOptions2::PaintWindow()' + R +
   '{' + R +
   '\tKWndImage::PaintWindow();' + R +
   '\tif (m_nToggleBtnValidCount > 0)' + R +
   '\t{' + R +
   '\t\tint nX0 = 0, nY0 = 0, nX1 = 0, nY1 = 0, nRong = 0, nCao = 0, nW = 0, nH = 0;' + R +
   '\t\tm_ToggleBtn[0].GetPosition(&nX0, &nY0);' + R +
   '\t\tm_ToggleBtn[m_nToggleBtnValidCount - 1].GetPosition(&nX1, &nY1);' + R +
   '\t\tm_ToggleBtn[0].GetSize(&nRong, &nCao);' + R +
   '\t\tGetSize(&nW, &nH);' + R +
   '\t\tJxNhip_VeNen(m_nAbsoluteLeft + 4, m_nAbsoluteTop + nY0 - 6, nW - 8, nY1 - nY0 + nCao + 12);' + R +
   '\t}' + R +
   '}' + R +
   '#endif' + R +
   'void KUiOptions2::StoreSetting()' + R,
   'void KUiOptions2::PaintWindow()')
# cay da va bang ban dau (them dong trong sau #endif -> ban Windows du mot dong trong, kiem --pc HONG) -> bo dong trong
va(OC,
   '#endif' + R + R + 'void KUiOptions2::StoreSetting()' + R,
   '#endif' + R + 'void KUiOptions2::StoreSetting()' + R,
   '#endif' + R + 'void KUiOptions2::StoreSetting()')
print('\n'.join(KQ))
