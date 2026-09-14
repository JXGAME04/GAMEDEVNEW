# -*- coding: latin-1 -*-
# [ZOOM3D 14/09] Chum hai ngon kieu game 3D + cong tac do nhay + phong to (chu 14/09 11:2x "lam 1-3" sau muc 11 PHANTICH_KIEMVONG_GIANGHO_3D_1309.md):
#   1) S3Client/Platform/JxLiaCanh.cpp/.h (chi JX_MOBILE): [Cham] ZoomKieu=1 -> zoom dich += -dpx x ZoomNhay/100 (3D: mWheelDistance += -0,01.dpx
#      x fWheelSpeed), bo qua dpx < ZoomChongRung (5 px), muot bang duoi ham mu zoom += (dich - zoom) x clamp01(dt x ZoomMuot=16) (3D: fDistanceSpeed 16)
#      - dung cho ca troi theo map; ZoomKieu=0 = ti le khoang cach hai ngon nhu cu. ZoomToiThieu (80) cho PHONG TO (< 100 %), chu "Phong to NNN%".
#      JxLia_DatNhay(zoom nhanh, zoom cham, lac): nhay x2 / x0,5, cong tac lac camera.
#   2) S3Client/Ui/UiCase/UiOptions2.h/.cpp (chi JX_MOBILE): 3 cong tac Zoom nhanh / Zoom cham (loai tru nhau) / Lac camera; MAX_TOGGLE_BTN_COUNT 9 -> 12.
#   3) phan Represent3 (phong to co loc net) o kich ban rieng va_nguon_zoom3d_1409_rep3.py (soi cheo voi phien do nhip).
# Idempotent, latin-1, neo CRLF. Dung: python android/va_nguon_zoom3d_1409.py [goc]
import io, os, sys

sys.path.insert(0, os.path.join(os.path.expanduser('~'), '.claude', 'skills', 'swordonline-dev', 'scripts'))
from vn_edit import vn

GOC = sys.argv[1] if len(sys.argv) > 1 and not sys.argv[1].startswith('--') else os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
KQ = []
R = '\r\n'

def doc(p):
    with io.open(p, 'r', encoding='latin-1', newline='') as f:
        return f.read()

def ghi(p, s):
    with io.open(p, 'w', encoding='latin-1', newline='') as f:
        f.write(s)

def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)

def va(p, cu, moi, dau_hieu, so_lan=1, cao_them=0):
    s = doc(p)
    if dau_hieu in s:
        KQ.append('bo qua (da co): %s : %s' % (os.path.basename(p), dau_hieu[:50])); return
    n = s.count(cu)
    if n != so_lan:
        raise SystemExit('LOI: neo gap %d lan (can %d) trong %s: %r' % (n, so_lan, p, cu[:90]))
    s2 = s.replace(cu, moi)
    if cao(s2) != cao(s) + cao_them:
        raise SystemExit('LOI: so byte cao doi %d -> %d (cho phep +%d) o %s' % (cao(s), cao(s2), cao_them, p))
    ghi(p, s2)
    KQ.append('da va: %s : %s' % (os.path.basename(p), dau_hieu[:50]))

LC = os.path.join(GOC, 'Sources', 'S3Client', 'Platform', 'JxLiaCanh.cpp')
LH = os.path.join(GOC, 'Sources', 'S3Client', 'Platform', 'JxLiaCanh.h')
OH = os.path.join(GOC, 'Sources', 'S3Client', 'Ui', 'UiCase', 'UiOptions2.h')
OC = os.path.join(GOC, 'Sources', 'S3Client', 'Ui', 'UiCase', 'UiOptions2.cpp')

# ---------------- JxLiaCanh.cpp ----------------
# a) bien tinh
va(LC,
   'static int\ts_nZoomThu = 0;\t\t// ZoomThu: GO LOI - % zoom tu dat 3 s sau khi vao the gioi (may ao khong co hai ngon)' + R,
   'static int\ts_nZoomThu = 0;\t\t// ZoomThu: GO LOI - % zoom tu dat 3 s sau khi vao the gioi (may ao khong co hai ngon)' + R +
   '// [ZOOM3D 14/09] chum hai ngon kieu game 3D (chu "lam 1-3" sau muc 11 mo zoom ban 3D): tuyen tinh theo px + chong rung + duoi ham mu' + R +
   'static int\ts_nZoomKieu = 1;\t\t// ZoomKieu: 1 = kieu 3D (zoom dich -= dpx x ZoomNhay/100, muot ZoomMuot), 0 = ti le khoang cach hai ngon (cu)' + R +
   'static int\ts_nZoomNhay = 10;\t\t// ZoomNhay: % zoom tren 100 px chum o muc "vua" (3D: 0,01 don vi/px tren dai 11 don vi ~ 9 %); Zoom nhanh x2, cham x0,5' + R +
   'static int\ts_nZoomChongRung = 5;\t// ZoomChongRung: khoang cach hai ngon doi it hon (px) thi bo qua (3D: 5 px)' + R +
   'static int\ts_nZoomMuot = 16;\t\t// ZoomMuot: zoom += (dich - zoom) x clamp01(dt x ZoomMuot) (3D: fDistanceSpeed = 16, 95 % sau ~0,18 s)' + R +
   'static int\ts_nZoomToiThieu = 80;\t// ZoomToiThieu: % nho nhat = PHONG TO (80 = to 1,25 lan; 3D cho toi 1,9 lan); 100 = tat phong to' + R +
   'static float\ts_fChumDCu = 0.f;\t\t// khoang cach hai ngon lan chap nhan gan nhat (kieu 3D)' + R +
   'static float\ts_fZoomDich3D = 0.f;\t// zoom dich lien tuc trong luc chum (kieu 3D)' + R +
   'static int\ts_nNcZoomNhanh = 0, s_nNcZoomCham = 0, s_nNcLac = 1;\t// cong tac Cai dat > Toi uu: Zoom nhanh / Zoom cham / Lac camera (JxLia_DatNhay)' + R,
   '[ZOOM3D 14/09] chum hai ngon kieu game 3D')

# b) doc cau hinh
va(LC,
   '\ts_nZoomThu     = GetPrivateProfileInt("Cham", "ZoomThu", 0, szCfg);' + R,
   '\ts_nZoomThu     = GetPrivateProfileInt("Cham", "ZoomThu", 0, szCfg);' + R +
   '\ts_nZoomKieu      = GetPrivateProfileInt("Cham", "ZoomKieu", 1, szCfg);\t// [ZOOM3D 14/09]' + R +
   '\ts_nZoomNhay      = GetPrivateProfileInt("Cham", "ZoomNhay", 10, szCfg);' + R +
   '\ts_nZoomChongRung = GetPrivateProfileInt("Cham", "ZoomChongRung", 5, szCfg);' + R +
   '\ts_nZoomMuot      = GetPrivateProfileInt("Cham", "ZoomMuot", 16, szCfg);' + R +
   '\ts_nZoomToiThieu  = GetPrivateProfileInt("Cham", "ZoomToiThieu", 80, szCfg);' + R +
   '\tif (s_nZoomNhay < 1) s_nZoomNhay = 1; if (s_nZoomNhay > 100) s_nZoomNhay = 100;' + R +
   '\tif (s_nZoomChongRung < 0) s_nZoomChongRung = 0; if (s_nZoomChongRung > 50) s_nZoomChongRung = 50;' + R +
   '\tif (s_nZoomMuot < 1) s_nZoomMuot = 1; if (s_nZoomMuot > 100) s_nZoomMuot = 100;' + R +
   '\tif (s_nZoomToiThieu < 50) s_nZoomToiThieu = 50; if (s_nZoomToiThieu > 100) s_nZoomToiThieu = 100;' + R,
   'GetPrivateProfileInt("Cham", "ZoomKieu"')

va(LC,
   '\tif (s_nZoomMacDinh < 100) s_nZoomMacDinh = 100; if (s_nZoomMacDinh > s_nZoomToiDa) s_nZoomMacDinh = s_nZoomToiDa;',
   '\tif (s_nZoomMacDinh < s_nZoomToiThieu) s_nZoomMacDinh = s_nZoomToiThieu; if (s_nZoomMacDinh > s_nZoomToiDa) s_nZoomMacDinh = s_nZoomToiDa;\t// [ZOOM3D 14/09] cho phep < 100',
   '[ZOOM3D 14/09] cho phep < 100')

va(LC,
   '\t\t\tif (s_nZoomNguoiChoi < 100 || s_nZoomNguoiChoi > 300) s_nZoomNguoiChoi = 0;',
   '\t\t\tif (s_nZoomNguoiChoi < 50 || s_nZoomNguoiChoi > 300) s_nZoomNguoiChoi = 0;\t// [ZOOM3D 14/09] 50..300',
   '[ZOOM3D 14/09] 50..300')

va(LC,
   '\tg_DebugLog("[LAC] camera lac nhe khi lia: ngang %d do, doc %d%%, le RT %d%%, thu %d do", s_nLacDo, s_nLacDoc, s_nLacLe, s_nLacThu);' + R,
   '\tg_DebugLog("[LAC] camera lac nhe khi lia: ngang %d do, doc %d%%, le RT %d%%, thu %d do", s_nLacDo, s_nLacDoc, s_nLacLe, s_nLacThu);' + R +
   '\tg_DebugLog("[ZOOM3D] chum kieu=%d nhay=%d%%/100px chong rung=%d px muot=%d toi thieu=%d%%", s_nZoomKieu, s_nZoomNhay, s_nZoomChongRung, s_nZoomMuot, s_nZoomToiThieu);' + R,
   '[ZOOM3D] chum kieu=%d')

# c) ZoomAp: kep duoi = ZoomToiThieu, chu "Phong to"
va(LC,
   '\tDocCaiDat();' + R +
   '\tif (nPhanTram < 100) nPhanTram = 100;' + R +
   '\tif (nPhanTram > s_nZoomToiDa) nPhanTram = s_nZoomToiDa;' + R +
   '\tnPhanTram = (nPhanTram + s_nZoomBuoc / 2) / s_nZoomBuoc * s_nZoomBuoc;' + R +
   '\tif (nPhanTram < 100) nPhanTram = 100;' + R,
   '\tDocCaiDat();' + R +
   '\tif (nPhanTram < s_nZoomToiThieu) nPhanTram = s_nZoomToiThieu;\t// [ZOOM3D 14/09] < 100 = phong to' + R +
   '\tif (nPhanTram > s_nZoomToiDa) nPhanTram = s_nZoomToiDa;' + R +
   '\tnPhanTram = (nPhanTram + s_nZoomBuoc / 2) / s_nZoomBuoc * s_nZoomBuoc;' + R +
   '\tif (nPhanTram < s_nZoomToiThieu) nPhanTram = s_nZoomToiThieu;' + R,
   '[ZOOM3D 14/09] < 100 = phong to')

CHU_CU = '\t\tsprintf(s_szZoomChu, "' + vn(u'Nh\u00ecn r\u1ed9ng') + ' %d%%", s_nZoom);' + R
CHU_MOI = ('\t\tif (s_nZoom < 100) sprintf(s_szZoomChu, "' + vn(u'Ph\u00f3ng to') + ' %d%%", 10000 / s_nZoom);\t// [ZOOM3D 14/09] 80 % = to 125 %' + R +
           '\t\telse sprintf(s_szZoomChu, "' + vn(u'Nh\u00ecn r\u1ed9ng') + ' %d%%", s_nZoom);' + R)
va(LC, CHU_CU, CHU_MOI, '[ZOOM3D 14/09] 80 % = to 125 %', cao_them=cao(vn(u'Ph\u00f3ng to')))

# d) Camera_DocMap: dich duoc < 100
va(LC,
   '\tif (nDich < 100) nDich = 100;' + R + '\tif (nDich > s_nZoomToiDa) nDich = s_nZoomToiDa;' + R,
   '\tif (nDich < s_nZoomToiThieu) nDich = s_nZoomToiThieu;\t// [ZOOM3D 14/09]' + R + '\tif (nDich > s_nZoomToiDa) nDich = s_nZoomToiDa;' + R,
   'if (nDich < s_nZoomToiThieu)')

# e) Nhip: duoi ham mu (kieu 3D) + troi ca trong luc chum
va(LC,
   '\tif (s_nZoomDich > 0 && !s_nChum)' + R +
   '\t{' + R +
   '\t\tfloat fBuoc = (float)s_nZoomTocDo * dt;' + R +
   '\t\tif (s_fZoomTroi < (float)s_nZoomDich) { s_fZoomTroi += fBuoc; if (s_fZoomTroi > (float)s_nZoomDich) s_fZoomTroi = (float)s_nZoomDich; }' + R +
   '\t\telse { s_fZoomTroi -= fBuoc; if (s_fZoomTroi < (float)s_nZoomDich) s_fZoomTroi = (float)s_nZoomDich; }' + R,
   '\tif (s_nZoomDich > 0 && (!s_nChum || s_nZoomKieu))\t// [ZOOM3D 14/09] kieu 3D: troi ca trong luc chum (dich do ChumKeo dat)' + R +
   '\t{' + R +
   '\t\tif (s_nZoomKieu)' + R +
   '\t\t{\t// [ZOOM3D 14/09] duoi ham mu nhu GameCamera 3D: t = clamp01(dt x ZoomMuot)' + R +
   '\t\t\tfloat t = dt * (float)s_nZoomMuot; if (t > 1.f) t = 1.f;' + R +
   '\t\t\ts_fZoomTroi += ((float)s_nZoomDich - s_fZoomTroi) * t;' + R +
   '\t\t\tif (s_fZoomTroi > (float)s_nZoomDich - 0.5f && s_fZoomTroi < (float)s_nZoomDich + 0.5f) s_fZoomTroi = (float)s_nZoomDich;' + R +
   '\t\t}' + R +
   '\t\telse' + R +
   '\t\t{' + R +
   '\t\tfloat fBuoc = (float)s_nZoomTocDo * dt;' + R +
   '\t\tif (s_fZoomTroi < (float)s_nZoomDich) { s_fZoomTroi += fBuoc; if (s_fZoomTroi > (float)s_nZoomDich) s_fZoomTroi = (float)s_nZoomDich; }' + R +
   '\t\telse { s_fZoomTroi -= fBuoc; if (s_fZoomTroi < (float)s_nZoomDich) s_fZoomTroi = (float)s_nZoomDich; }' + R +
   '\t\t}' + R,
   '[ZOOM3D 14/09] duoi ham mu nhu GameCamera 3D')

# f) chum: bat dau + keo + nha
va(LC,
   '\ts_nChumZoom0 = s_nZoom;' + R + '\ts_nChum = 1;' + R,
   '\ts_nChumZoom0 = s_nZoom;' + R + '\ts_nChum = 1;' + R +
   '\ts_fChumDCu = s_fChumD0; s_fZoomDich3D = (float)((s_nZoomDich > 0) ? s_nZoomDich : s_nZoom);\t// [ZOOM3D 14/09]' + R,
   's_fChumDCu = s_fChumD0;')

va(LC,
   '\tJxLia_ZoomDat((int)((float)s_nChumZoom0 * s_fChumD0 / d + 0.5f));\t// hai ngon gan nhau (d < d0) = zoom lon = thay rong hon' + R,
   '\tif (s_nZoomKieu)' + R +
   '\t{\t// [ZOOM3D 14/09] kieu 3D: dich -= dpx x nhay (hai ngon dang ra = phong to); dpx < ZoomChongRung bo qua; ap muot o JxLia_Nhip' + R +
   '\t\tfloat fD = d - s_fChumDCu, fNhay;' + R +
   '\t\tif (fD <= -(float)s_nZoomChongRung || fD >= (float)s_nZoomChongRung)' + R +
   '\t\t{' + R +
   '\t\t\ts_fChumDCu = d;' + R +
   '\t\t\tfNhay = (float)s_nZoomNhay / 100.f; if (s_nNcZoomNhanh) fNhay *= 2.f; else if (s_nNcZoomCham) fNhay *= 0.5f;' + R +
   '\t\t\ts_fZoomDich3D -= fD * fNhay;' + R +
   '\t\t\tif (s_fZoomDich3D < (float)s_nZoomToiThieu) s_fZoomDich3D = (float)s_nZoomToiThieu;' + R +
   '\t\t\tif (s_fZoomDich3D > (float)s_nZoomToiDa) s_fZoomDich3D = (float)s_nZoomToiDa;' + R +
   '\t\t\ts_nZoomDich = (int)(s_fZoomDich3D + 0.5f);' + R +
   '\t\t\tif (s_nNhatKy) g_DebugLog("[ZOOM3D] chum d=%.0f dpx=%.0f -> dich %d%% (dang %d%%)", d, fD, s_nZoomDich, s_nZoom);' + R +
   '\t\t}' + R +
   '\t}' + R +
   '\telse' + R +
   '\t\tJxLia_ZoomDat((int)((float)s_nChumZoom0 * s_fChumD0 / d + 0.5f));\t// hai ngon gan nhau (d < d0) = zoom lon = thay rong hon' + R,
   '[ZOOM3D 14/09] kieu 3D: dich -= dpx x nhay')

va(LC,
   '\ts_nChum = 0;' + R + '\tCamera_GhiNho();\t// [CAMERA 13/09] nho zoom nguoi choi' + R,
   '\ts_nChum = 0;' + R +
   '\tif (s_nZoomKieu && s_nZoomDich > 0) s_nZoomNguoiChoi = s_nZoomDich;\t// [ZOOM3D 14/09] nho dich (Nhip van troi tiep toi do)' + R +
   '\tCamera_GhiNho();\t// [CAMERA 13/09] nho zoom nguoi choi' + R,
   '[ZOOM3D 14/09] nho dich')

# g) cong tac lac camera
va(LC,
   '\tif ((s_nLacDo != 0 || s_nLacDoc != 0) && s_nTrangThai != LIA_KHONG)',
   '\tif (s_nNcLac && (s_nLacDo != 0 || s_nLacDoc != 0) && s_nTrangThai != LIA_KHONG)\t// [ZOOM3D 14/09] cong tac Lac camera',
   '[ZOOM3D 14/09] cong tac Lac camera')

# h) JxLia_DatNhay
va(LC,
   'void JxLia_Ve()' + R + '{' + R,
   '// [ZOOM3D 14/09] cong tac Cai dat > Toi uu: Zoom nhanh / Zoom cham (loai tru nhau, ca hai tat = vua) / Lac camera' + R +
   'void JxLia_DatNhay(int nZoomNhanh, int nZoomCham, int nLac)' + R +
   '{' + R +
   '\tif (nZoomNhanh >= 0) s_nNcZoomNhanh = nZoomNhanh ? 1 : 0;' + R +
   '\tif (nZoomCham >= 0) s_nNcZoomCham = nZoomCham ? 1 : 0;' + R +
   '\tif (nLac >= 0) s_nNcLac = nLac ? 1 : 0;' + R +
   '\tg_DebugLog("[ZOOM3D] tuy chon: zoom nhanh=%d cham=%d lac camera=%d", s_nNcZoomNhanh, s_nNcZoomCham, s_nNcLac);' + R +
   '}' + R +
   'void JxLia_Ve()' + R + '{' + R,
   '[ZOOM3D 14/09] cong tac Cai dat > Toi uu: Zoom nhanh')

# ---------------- JxLiaCanh.h ----------------
va(LH,
   'void JxLia_DatTuyChon(int nLia, int nZoom, int nVeNhanh);',
   'void JxLia_DatTuyChon(int nLia, int nZoom, int nVeNhanh);' + R +
   'void JxLia_DatNhay(int nZoomNhanh, int nZoomCham, int nLac);\t// [ZOOM3D 14/09] cong tac Zoom nhanh / Zoom cham / Lac camera',
   '[ZOOM3D 14/09] cong tac Zoom nhanh / Zoom cham / Lac camera')

# ---------------- UiOptions2.h ----------------
va(OH,
   '\tOPTION_I_SANGVATROI,\t// cot sang + loe vat pham roi (Core/Src/KObj.cpp, VATROI 14/09)' + R,
   '\tOPTION_I_SANGVATROI,\t// cot sang + loe vat pham roi (Core/Src/KObj.cpp, VATROI 14/09)' + R +
   '\tOPTION_I_ZOOMNHANH,\t\t// [ZOOM3D 14/09] chum hai ngon nhay gap doi (Platform/JxLiaCanh)' + R +
   '\tOPTION_I_ZOOMCHAM,\t\t// chum hai ngon nhay mot nua (loai tru voi ZOOMNHANH)' + R +
   '\tOPTION_I_LAC,\t\t\t// camera lac nhe khi lia' + R,
   '[ZOOM3D 14/09] chum hai ngon nhay gap doi')

va(OH,
   '#define MAX_TOGGLE_BTN_COUNT 9\t// [HAOQUANG 14/09 d] 10 -> 9 (bo Vong do mac theo chu);',
   '#define MAX_TOGGLE_BTN_COUNT 12\t// [ZOOM3D 14/09] 9 -> 12 (+ Zoom nhanh / Zoom cham / Lac camera): 6 hang tu Top=60 cach 27 (android/sinh_uioptions2_zoom.py); [HAOQUANG 14/09 d] 10 -> 9 (bo Vong do mac theo chu);',
   '[ZOOM3D 14/09] 9 -> 12')

# ---------------- UiOptions2.cpp ----------------
va(OC,
   '#define MAX_TOGGLE_BTN_COUNT 9\t// [HAOQUANG 14/09 d] 9 nut (bo Vong do mac);',
   '#define MAX_TOGGLE_BTN_COUNT 12\t// [ZOOM3D 14/09] 12 nut; [HAOQUANG 14/09 d] 9 nut (bo Vong do mac);',
   '[ZOOM3D 14/09] 12 nut')

va(OC,
   '\t"HaoQuangQuai", "SangVatRoi",\t// [HAOQUANG 14/09] (14/09 d: bo HaoQuangTrangBi)' + R,
   '\t"HaoQuangQuai", "SangVatRoi",\t// [HAOQUANG 14/09] (14/09 d: bo HaoQuangTrangBi)' + R +
   '\t"ZoomNhanh", "ZoomCham", "LacCamera",\t// [ZOOM3D 14/09]' + R,
   '"ZoomNhanh", "ZoomCham", "LacCamera"')

va(OC,
   '\tcase OPTION_I_SANGVATROI:' + R + '\t\tJxVatRoi_DatBat(bEnable);' + R + '\t\tbreak;' + R,
   '\tcase OPTION_I_SANGVATROI:' + R + '\t\tJxVatRoi_DatBat(bEnable);' + R + '\t\tbreak;' + R +
   '\tcase OPTION_I_ZOOMNHANH:\t// [ZOOM3D 14/09] nhanh / cham loai tru nhau; roi xuong OPTION_I_LAC de ap ca ba' + R +
   '\tcase OPTION_I_ZOOMCHAM:' + R +
   '\t\tif (bEnable) m_ToggleItemList[(nIndex == OPTION_I_ZOOMNHANH) ? OPTION_I_ZOOMCHAM : OPTION_I_ZOOMNHANH].bEnable = false;' + R +
   '\tcase OPTION_I_LAC:' + R +
   '\t\tJxLia_DatNhay(m_ToggleItemList[OPTION_I_ZOOMNHANH].bEnable, m_ToggleItemList[OPTION_I_ZOOMCHAM].bEnable, m_ToggleItemList[OPTION_I_LAC].bEnable);' + R +
   '\t\tbreak;' + R,
   '[ZOOM3D 14/09] nhanh / cham loai tru nhau')

va(OC,
   '\t\t\tpSetting->GetInteger(OPTIONS_SAVE_SECTION2, ls_ToggleOptionName2[OPTION_I_SANGVATROI], true, &bOptionsEnable[OPTION_I_SANGVATROI]);' + R,
   '\t\t\tpSetting->GetInteger(OPTIONS_SAVE_SECTION2, ls_ToggleOptionName2[OPTION_I_SANGVATROI], true, &bOptionsEnable[OPTION_I_SANGVATROI]);' + R +
   '\t\t\tpSetting->GetInteger(OPTIONS_SAVE_SECTION2, ls_ToggleOptionName2[OPTION_I_LAC], true, &bOptionsEnable[OPTION_I_LAC]);\t// [ZOOM3D 14/09] lac mac dinh BAT; zoom nhanh/cham mac dinh TAT' + R,
   '[ZOOM3D 14/09] lac mac dinh BAT')

va(OC,
   '\t\tJxVatRoi_DatBat(bOptionsEnable[OPTION_I_SANGVATROI]);' + R,
   '\t\tJxVatRoi_DatBat(bOptionsEnable[OPTION_I_SANGVATROI]);' + R +
   '\t\tJxLia_DatNhay(bOptionsEnable[OPTION_I_ZOOMNHANH], bOptionsEnable[OPTION_I_ZOOMCHAM], bOptionsEnable[OPTION_I_LAC]);\t// [ZOOM3D 14/09]' + R,
   'JxLia_DatNhay(bOptionsEnable[OPTION_I_ZOOMNHANH]')


# i) ZoomThu (go loi may ao) cho phep < 100 (thu phong to)
va(LC,
   '\tif (s_nZoomThu > 100 && !s_nZoomThuXong) { nDich = s_nZoomThu; s_nZoomThuXong = 1; }',
   '\tif (s_nZoomThu >= 50 && s_nZoomThu != 100 && !s_nZoomThuXong) { nDich = s_nZoomThu; s_nZoomThuXong = 1; }\t// [ZOOM3D 14/09] ZoomThu < 100 = thu phong to',
   '[ZOOM3D 14/09] ZoomThu < 100 = thu phong to')

print('\n'.join(KQ))
