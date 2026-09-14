# -*- coding: latin-1 -*-
# [CHUNET 14/09 d] + [ZOOMTHANH 14/09 e] - chu 14/09 15:1x: "ngoi sao chuyen sinh mau xanh chua co dinh theo ten nen bi lech" + "chum zoom la gi?
# nhung tinh nang nao khong dung don sach phan cai dat".
#   1. Icon canh ten (ngoi sao chuyen sinh, hang, mon phai... KNpc::PaintInfo ve RU_T_IMAGE toa do the gioi): Core mobile goi DrawPrimitives voi
#      bSinglePlaneCoord = 2 (= toa do the gioi + lop thong tin); Represent3 xep hang (loai 4) va ve sau blit cung nhom chu (co that, bam theo ten).
#   2. Cua so Toi uu con 11 cong tac: bo Zoom nhanh / Zoom cham / Chum zoom (chum hai ngon + do nhay chi con qua config [Cham] ZoomChum / ZoomNhay);
#      6 hang Top 60 cach 27 (android/sinh_uioptions2_don.py).
# Idempotent, latin-1, neo CRLF. Chay SAU _b va _c. Dung: python android/va_nguon_zoomthanh_1409_d.py [goc]
import io, os, sys

GOC = sys.argv[1] if len(sys.argv) > 1 and not sys.argv[1].startswith('--') else os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
KQ = []
R = '\r\n'
T = '\t'

def doc(p):
    with io.open(p, 'r', encoding='latin-1', newline='') as f:
        return f.read()

def ghi(p, s):
    with io.open(p, 'w', encoding='latin-1', newline='') as f:
        f.write(s)

def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)

def va(p, cu, moi, dau_hieu, so_lan=1):
    s = doc(p)
    if dau_hieu in s:
        KQ.append('bo qua (da co): %s : %s' % (os.path.basename(p), dau_hieu[:60])); return
    n = s.count(cu)
    if n != so_lan:
        raise SystemExit('LOI: neo gap %d lan (can %d) trong %s: %r' % (n, so_lan, p, cu[:110]))
    s2 = s.replace(cu, moi)
    if cao(s2) != cao(s):
        raise SystemExit('LOI: so byte cao doi o %s' % p)
    ghi(p, s2)
    KQ.append('da va: %s : %s' % (os.path.basename(p), dau_hieu[:60]))

RC = os.path.join(GOC, 'Sources', 'Represent', 'Represent3', 'KRepresentShell3.cpp')
KN = os.path.join(GOC, 'Sources', 'Core', 'Src', 'KNpc.cpp')
JL = os.path.join(GOC, 'Sources', 'S3Client', 'Platform', 'JxLiaCanh.cpp')
OH = os.path.join(GOC, 'Sources', 'S3Client', 'Ui', 'UiCase', 'UiOptions2.h')
OC = os.path.join(GOC, 'Sources', 'S3Client', 'Ui', 'UiCase', 'UiOptions2.cpp')

# ============================================================ A. KNpc.cpp: icon canh ten -> bSinglePlaneCoord = 2 (mobile) ============================================================
GC = '// [CHUNET 14/09 d] 2 = toa do the gioi + LOP THONG TIN: Represent3 xep hang, ve sau blit cung nhom chu (co that, bam theo ten khi zoom)'

def icon(so_tab, bien, duoi, so_lan=1):
    cu = R + T * so_tab + 'g_pRepresent->DrawPrimitives(1, &' + bien + ', RU_T_IMAGE, ' + duoi + ');'
    # dong goc ket thuc bang dau tab? -> nguoi goi truyen ca phan duoi cung
    return cu

def va_icon(cu_line, so_tab, bien, dau_hieu, so_lan=1):
    """cu_line = ca dong goc (khong co R dau / cuoi). Boc #ifdef JX_MOBILE / #else / #endif."""
    cu = R + cu_line + R
    moi = (R + '#ifdef JX_MOBILE' + R +
           T * so_tab + 'g_pRepresent->DrawPrimitives(1, &' + bien + ', RU_T_IMAGE, 2);' + T + GC + R +
           '#else' + R +
           cu_line + R +
           '#endif' + R)
    va(KN, cu, moi, dau_hieu, so_lan)

va_icon(T * 3 + 'g_pRepresent->DrawPrimitives(1, &RURank, RU_T_IMAGE, 0);' + T * 2, 3, 'RURank', '&RURank, RU_T_IMAGE, 2);')
va_icon(T * 3 + 'g_pRepresent->DrawPrimitives(1, &RUIconImage, RU_T_IMAGE, FALSE);' + T * 4, 3, 'RUIconImage', 'RUIconImage, RU_T_IMAGE, 2);' + T + GC + R + '#else' + R + T * 3 + 'g_pRepresent->DrawPrimitives(1, &RUIconImage, RU_T_IMAGE, FALSE);' + T * 4)
va_icon(T * 3 + 'g_pRepresent->DrawPrimitives(1, &RUIconImage, RU_T_IMAGE, FALSE);' + T, 3, 'RUIconImage', 'RUIconImage, RU_T_IMAGE, 2);' + T + GC + R + '#else' + R + T * 3 + 'g_pRepresent->DrawPrimitives(1, &RUIconImage, RU_T_IMAGE, FALSE);' + T + R)
# 3 tab (2 cho) TRUOC 4 tab: dau hieu 3 tab (co R dau) khong nam trong ban va 4 tab va nguoc lai
va_icon(T * 3 + 'g_pRepresent->DrawPrimitives(1, &RUIconImageR, RU_T_IMAGE, FALSE);', 3, 'RUIconImageR', R + T * 3 + 'g_pRepresent->DrawPrimitives(1, &RUIconImageR, RU_T_IMAGE, 2);', 2)
va_icon(T * 4 + 'g_pRepresent->DrawPrimitives(1, &RUIconImageR, RU_T_IMAGE, FALSE);', 4, 'RUIconImageR', R + T * 4 + 'g_pRepresent->DrawPrimitives(1, &RUIconImageR, RU_T_IMAGE, 2);')

# ============================================================ B. Represent3: loai 4 = icon ============================================================
va(RC,
   'KOutputTextParam Param; KRUShadow Bong; };',
   'KOutputTextParam Param; KRUShadow Bong; KRUImage Anh; };' + T + '// [CHUNET 14/09 d] + icon canh ten',
   'KRUShadow Bong; KRUImage Anh; };')

va(RC,
   'const KOutputTextParam* pParam, const KRUShadow* pBong)' + R +
   '{' + R +
   T + 'KTgChu e; memset(&e, 0, sizeof(e));',
   'const KOutputTextParam* pParam, const KRUShadow* pBong, const KRUImage* pAnh)' + R +
   '{' + R +
   T + 'KTgChu e; memset(&e, 0, sizeof(e));' + R +
   T + 'if (pAnh) e.Anh = *pAnh;' + T + '// [CHUNET 14/09 d]',
   'if (pAnh) e.Anh = *pAnh;')

va(RC, 'nX, nY, nZ, Color, nLineWidth, BorderColor, NULL, NULL); return; }' + T + '// [CHUNET 14/09] chu the gioi: ve sau blit',
       'nX, nY, nZ, Color, nLineWidth, BorderColor, NULL, NULL, NULL); return; }' + T + '// [CHUNET 14/09] chu the gioi: ve sau blit',
       'BorderColor, NULL, NULL, NULL); return; }' + T + '// [CHUNET 14/09] chu the gioi')
va(RC, 'nX, nY, nZ, Color, nLineWidth, BorderColor, NULL, NULL); return; }' + T + '// [CHUNET 14/09] chu VN the gioi: ve sau blit',
       'nX, nY, nZ, Color, nLineWidth, BorderColor, NULL, NULL, NULL); return; }' + T + '// [CHUNET 14/09] chu VN the gioi: ve sau blit',
       'BorderColor, NULL, NULL, NULL); return; }' + T + '// [CHUNET 14/09] chu VN')
va(RC, 'nLineWidth, 0, pParam, NULL); return 0; }',
       'nLineWidth, 0, pParam, NULL, NULL); return 0; }',
       'pParam, NULL, NULL); return 0; }')
va(RC, 'TgChuThem(3, 0, NULL, 0, 0, 0, 0, 0, 0, 0, NULL, (KRUShadow*)pPrimitives); return; }',
       'TgChuThem(3, 0, NULL, 0, 0, 0, 0, 0, 0, 0, NULL, (KRUShadow*)pPrimitives, NULL); return; }',
       '(KRUShadow*)pPrimitives, NULL); return; }')

# DrawPrimitives: 2 = lop thong tin; RU_T_IMAGE lop thong tin -> xep hang
va(RC,
   '#ifdef JX_MOBILE' + R +
   T + 'if (uGenre == RU_T_SHADOW && !bSinglePlaneCoord && nPrimitiveCount == 1 && TgChuXep())',
   '#ifdef JX_MOBILE' + R +
   T + 'int bLopChu = 0; if (bSinglePlaneCoord == 2) { bLopChu = 1; bSinglePlaneCoord = 0; }' + T + '// [CHUNET 14/09 d] KNpc::PaintInfo (mobile): icon canh ten = toa do the gioi + lop thong tin' + R +
   T + 'if (uGenre == RU_T_IMAGE && bLopChu && TgChuXep()) { for (i = 0; i < nPrimitiveCount; i++) TgChuThem(4, 0, NULL, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, ((KRUImage*)pPrimitives) + i); return; }' + R +
   T + 'if (uGenre == RU_T_SHADOW && !bSinglePlaneCoord && nPrimitiveCount == 1 && TgChuXep())',
   '[CHUNET 14/09 d] KNpc::PaintInfo (mobile): icon canh ten')

# TgChuVe: do luong + gom nhom + ve
va(RC,
   T + T + 'else if (e.nLoai == 2) { vCx[k] = e.Param.nX + e.nLineWidth / 2; vY[k] = e.Param.nY; vZ[k] = e.Param.nZ; }',
   T + T + 'else if (e.nLoai == 4) { vCx[k] = e.Anh.oPosition.nX; vY[k] = e.Anh.oPosition.nY; vZ[k] = e.Anh.oPosition.nZ; }' + T + '// [CHUNET 14/09 d] icon: goc anh (lech nMpsX toi ~100)' + R +
   T + T + 'else if (e.nLoai == 2) { vCx[k] = e.Param.nX + e.nLineWidth / 2; vY[k] = e.Param.nY; vZ[k] = e.Param.nZ; }',
   '[CHUNET 14/09 d] icon: goc anh')

va(RC,
   T + T + 'if (nNhom < 0 || vCx[k] < nCxNhom - 2 || vCx[k] > nCxNhom + 2 || vY[k] < nYNhom - 150 || vY[k] > nYNhom + 150)',
   T + T + 'const int nRongX = (nLoai == 4 || (nNhom >= 0 && !vCoChu[nNhom])) ? 200 : 2;' + T + '// [CHUNET 14/09 d] icon (hay nhom moi chi co icon) rong hon: icon lech nMpsX toi ~100' + R +
   T + T + 'if (nNhom < 0 || vCx[k] < nCxNhom - nRongX || vCx[k] > nCxNhom + nRongX || vY[k] < nYNhom - 150 || vY[k] > nYNhom + 150)',
   '[CHUNET 14/09 d] icon (hay nhom moi chi co icon) rong hon')

va(RC,
   T + T + T + 'if (nLoai != 3 && (!vCoChu[nNhom] || vY[k] > vChan[nNhom])) { vChan[nNhom] = vY[k]; vCoChu[nNhom] = 1; }' + R,
   T + T + T + 'if (nLoai != 3 && nLoai != 4 && !vCoChu[nNhom]) { vCxN[nNhom] = vCx[k]; nCxNhom = vCx[k]; }' + T + '// [CHUNET 14/09 d] dong chu dau tien dinh tam nhom (nhom mo bang icon)' + R +
   T + T + T + 'if (nLoai != 3 && nLoai != 4 && (!vCoChu[nNhom] || vY[k] > vChan[nNhom])) { vChan[nNhom] = vY[k]; vCoChu[nNhom] = 1; }' + R,
   '[CHUNET 14/09 d] dong chu dau tien dinh tam nhom')

va(RC,
   T + T + T + 'vCxN.push_back(vCx[k]); vChan.push_back(vY[k]); vCoChu.push_back((nLoai != 3) ? 1 : 0); vZMin.push_back(vZ[k]);',
   T + T + T + 'vCxN.push_back(vCx[k]); vChan.push_back(vY[k]); vCoChu.push_back((nLoai != 3 && nLoai != 4) ? 1 : 0); vZMin.push_back(vZ[k]);',
   'vCoChu.push_back((nLoai != 3 && nLoai != 4) ? 1 : 0)')

va(RC,
   T + T + 'else if (e.nLoai == 2)' + R +
   T + T + '{' + R +
   T + T + T + 'if (!vb) continue;' + R +
   T + T + T + 'KOutputTextParam p = e.Param;',
   T + T + 'else if (e.nLoai == 4)' + R +
   T + T + '{' + T + '// [CHUNET 14/09 d] icon canh ten: co that, lech 1:1 so voi tam nhom' + R +
   T + T + T + 'KRUImage a = e.Anh;' + R +
   T + T + T + 'a.oPosition.nX = ax + (e.Anh.oPosition.nX - cxN);' + R +
   T + T + T + 'a.oPosition.nY = ay + (e.Anh.oPosition.nY - yChan) / 2 - (((e.Anh.oPosition.nZ - zMin) * 887) >> 10);' + R +
   T + T + T + 'a.oPosition.nZ = 0;' + R +
   T + T + T + 'DrawPrimitives(1, (KRepresentUnit*)&a, RU_T_IMAGE, TRUE);' + R +
   T + T + '}' + R +
   T + T + 'else if (e.nLoai == 2)' + R +
   T + T + '{' + R +
   T + T + T + 'if (!vb) continue;' + R +
   T + T + T + 'KOutputTextParam p = e.Param;',
   '[CHUNET 14/09 d] icon canh ten: co that')

# ============================================================ C. Don cua so Toi uu: 11 cong tac ============================================================
va(OH,
   T + 'OPTION_I_ZOOMNHANH,' + T + T + '// [ZOOM3D 14/09] chum hai ngon nhay gap doi (Platform/JxLiaCanh)' + R +
   T + 'OPTION_I_ZOOMCHAM,' + T + T + '// chum hai ngon nhay mot nua (loai tru voi ZOOMNHANH)' + R +
   T + 'OPTION_I_LAC,' + T + T + T + '// camera lac nhe khi lia' + R +
   T + 'OPTION_I_CHUMZOOM,' + T + T + '// [ZOOMTHANH 14/09] chum hai ngon zoom (mac dinh TAT: da co thanh keo zoom KUiZoomThanh, tranh nham khi bam man choi)' + R,
   T + 'OPTION_I_LAC,' + T + T + T + '// camera lac nhe khi lia ([ZOOMTHANH 14/09 e] bo Zoom nhanh / Zoom cham / Chum zoom theo chu "don sach phan cai dat": chum hai ngon + do nhay chi con qua config [Cham] ZoomChum / ZoomNhay)' + R,
   '[ZOOMTHANH 14/09 e] bo Zoom nhanh / Zoom cham / Chum zoom')

va(OH,
   '#define MAX_TOGGLE_BTN_COUNT 14' + T + '// [ZOOMTHANH 14/09 c] 13 -> 14',
   '#define MAX_TOGGLE_BTN_COUNT 11' + T + '// [ZOOMTHANH 14/09 e] 14 -> 11 (bo Zoom nhanh / cham / Chum zoom): 6 hang Top 60 cach 27 (android/sinh_uioptions2_don.py); [ZOOMTHANH 14/09 c] 13 -> 14',
   'MAX_TOGGLE_BTN_COUNT 11' + T + '// [ZOOMTHANH 14/09 e]')

va(OC,
   '#define MAX_TOGGLE_BTN_COUNT 14' + T + '// [ZOOMTHANH 14/09 c] 14 nut',
   '#define MAX_TOGGLE_BTN_COUNT 11' + T + '// [ZOOMTHANH 14/09 e] 11 nut; [ZOOMTHANH 14/09 c] 14 nut',
   'MAX_TOGGLE_BTN_COUNT 11' + T + '// [ZOOMTHANH 14/09 e]')

va(OC,
   T + '"ZoomNhanh", "ZoomCham", "LacCamera",' + T + '// [ZOOM3D 14/09]' + R +
   T + '"ChumZoom", "ThanhZoom",' + T + '// [ZOOMTHANH 14/09] (c: + ThanhZoom)',
   T + '"LacCamera", "ThanhZoom",' + T + '// [ZOOM3D 14/09] [ZOOMTHANH 14/09 c] (e: bo ZoomNhanh / ZoomCham / ChumZoom)',
   '"LacCamera", "ThanhZoom",')

va(OC,
   T + 'case OPTION_I_ZOOMNHANH:' + T + '// [ZOOM3D 14/09] nhanh / cham loai tru nhau; roi xuong OPTION_I_LAC de ap ca ba' + R +
   T + 'case OPTION_I_ZOOMCHAM:' + R +
   T + T + 'if (bEnable) m_ToggleItemList[(nIndex == OPTION_I_ZOOMNHANH) ? OPTION_I_ZOOMCHAM : OPTION_I_ZOOMNHANH].bEnable = false;' + R +
   T + 'case OPTION_I_LAC:' + R +
   T + T + 'JxLia_DatNhay(m_ToggleItemList[OPTION_I_ZOOMNHANH].bEnable, m_ToggleItemList[OPTION_I_ZOOMCHAM].bEnable, m_ToggleItemList[OPTION_I_LAC].bEnable);' + R +
   T + T + 'break;' + R +
   T + 'case OPTION_I_CHUMZOOM:' + T + '// [ZOOMTHANH 14/09] ap ngay' + R +
   T + T + 'JxLia_DatChum(bEnable);' + R +
   T + T + 'break;' + R,
   T + 'case OPTION_I_LAC:' + T + '// [ZOOM3D 14/09] ([ZOOMTHANH 14/09 e] chi con Lac camera; nhanh / cham / chum theo config)' + R +
   T + T + 'JxLia_DatNhay(-1, -1, m_ToggleItemList[OPTION_I_LAC].bEnable);' + R +
   T + T + 'break;' + R,
   '[ZOOMTHANH 14/09 e] chi con Lac camera')

va(OC,
   T + T + 'JxLia_DatNhay(bOptionsEnable[OPTION_I_ZOOMNHANH], bOptionsEnable[OPTION_I_ZOOMCHAM], bOptionsEnable[OPTION_I_LAC]);' + T + '// [ZOOM3D 14/09]' + R +
   T + T + 'JxLia_DatChum(bOptionsEnable[OPTION_I_CHUMZOOM]);' + T + '// [ZOOMTHANH 14/09] mac dinh TAT (GetInteger mac dinh false)' + R,
   T + T + 'JxLia_DatNhay(-1, -1, bOptionsEnable[OPTION_I_LAC]);' + T + '// [ZOOM3D 14/09] ([ZOOMTHANH 14/09 e] nhanh / cham / chum theo config [Cham])' + R,
   '[ZOOMTHANH 14/09 e] nhanh / cham / chum theo config [Cham]')

# JxLiaCanh: chum hai ngon theo config
va(JL,
   T + 's_nZoomMuot      = GetPrivateProfileInt("Cham", "ZoomMuot", 16, szCfg);' + R,
   T + 's_nZoomMuot      = GetPrivateProfileInt("Cham", "ZoomMuot", 16, szCfg);' + R +
   T + 's_nNcChum        = GetPrivateProfileInt("Cham", "ZoomChum", 0, szCfg) ? 1 : 0;' + T + '// [ZOOMTHANH 14/09 e] chum hai ngon chi bat qua config (bo cong tac; mac dinh tat vi da co thanh keo)' + R,
   '[ZOOMTHANH 14/09 e] chum hai ngon chi bat qua config')

# config.ini: lop ghi de + may ao (KHONG dt_v4: phien do nhip dang sua config dt_v4, khoa mac dinh 0 trong ma)
def them_sau(p, neo, them, dau):
    if not os.path.isfile(p):
        KQ.append('khong thay: ' + p); return
    s = doc(p)
    if dau in s:
        KQ.append('bo qua (da co): ' + p); return
    nl = '\r\n' if s.count('\r\n') > s.count('\n') // 2 else '\n'
    a = neo + nl
    if s.count(a) != 1:
        raise SystemExit('LOI neo %r trong %s: %d' % (neo, p, s.count(a)))
    ghi(p, s.replace(a, a + them.replace('\n', nl), 1))
    KQ.append('da them: ' + p)

for g in [os.path.join(GOC, 'android', 'du_lieu_ghi_de'), r'D:\jx1_android_data']:
    them_sau(os.path.join(g, 'config.ini'), 'ZoomToiThieu=80',
             '; [ZOOMTHANH 14/09 e] ZoomChum=1 bat lai chum hai ngon zoom (mac dinh 0: da co thanh keo zoom, chum lam mat cham khi choi hai ngon cai);\n'
             ';   do nhay chum = ZoomNhay o tren (cong tac Zoom nhanh / cham / Chum zoom da bo khoi cua so Toi uu theo chu).\n'
             'ZoomChum=0\n', 'ZoomChum=')

for k in KQ:
    print(k)
