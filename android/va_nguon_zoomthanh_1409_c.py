# -*- coding: latin-1 -*-
# [CHUNET 14/09 c] + [ZOOMTHANH 14/09 c] - chu 14/09 15:0x (anh Fold 7): "bi loi ho qua xa voi thanh mau" + "khong co nut tat thanh zoom?"
#   1. Lop chu: chieu cao neo (z nho nhat cua nhom = chieu cao nhan vat, nHeightOff) phai CO THEO ZOOM nhu than sprite; chi phan xep dong
#      (z - zMin, do lech y) giu co that. Truoc (b) giu ca chieu cao 1:1 -> khi nhin rong chu + thanh mau treo cao hon dau nhan vat.
#   2. Cong tac 14 "Thanh zoom" trong Cai dat > Toi uu (mac dinh BAT): JxZoomThanh_DatBat -> thanh khong ve + cham xuyen qua.
# Idempotent, latin-1, neo CRLF. Chay SAU va_nguon_zoomthanh_1409_b.py. Dung: python android/va_nguon_zoomthanh_1409_c.py [goc]
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
        raise SystemExit('LOI: neo gap %d lan (can %d) trong %s: %r' % (n, so_lan, p, cu[:100]))
    s2 = s.replace(cu, moi)
    if cao(s2) != cao(s):
        raise SystemExit('LOI: so byte cao doi o %s' % p)
    ghi(p, s2)
    KQ.append('da va: %s : %s' % (os.path.basename(p), dau_hieu[:60]))

RC = os.path.join(GOC, 'Sources', 'Represent', 'Represent3', 'KRepresentShell3.cpp')
ZC = os.path.join(GOC, 'Sources', 'S3Client', 'Ui', 'UiCase', 'UiZoomThanh.cpp')
OH = os.path.join(GOC, 'Sources', 'S3Client', 'Ui', 'UiCase', 'UiOptions2.h')
OC = os.path.join(GOC, 'Sources', 'S3Client', 'Ui', 'UiCase', 'UiOptions2.cpp')

# ============================================================ A. Represent3: chieu cao neo co theo zoom ============================================================
# gom nhom: them z nho nhat cua nhom
va(RC,
   T + 'static std::vector<int> vCx, vY, vNhom, vCxN, vChan; static std::vector<char> vCoChu;' + R +
   T + 'vCx.resize(n); vY.resize(n); vNhom.resize(n); vCxN.clear(); vChan.clear(); vCoChu.clear();' + R,
   T + 'static std::vector<int> vCx, vY, vZ, vNhom, vCxN, vChan, vZMin; static std::vector<char> vCoChu;' + T + '// [CHUNET 14/09 c] + vZ / vZMin' + R +
   T + 'vCx.resize(n); vY.resize(n); vZ.resize(n); vNhom.resize(n); vCxN.clear(); vChan.clear(); vZMin.clear(); vCoChu.clear();' + R,
   '[CHUNET 14/09 c] + vZ / vZMin')

va(RC,
   T + T + T + 'vCx[k] = (e.Bong.oPosition.nX + e.Bong.oEndPos.nX) / 2; vY[k] = (e.Bong.oPosition.nY + e.Bong.oEndPos.nY) / 2;' + R,
   T + T + T + 'vCx[k] = (e.Bong.oPosition.nX + e.Bong.oEndPos.nX) / 2; vY[k] = (e.Bong.oPosition.nY + e.Bong.oEndPos.nY) / 2;' + R +
   T + T + T + 'vZ[k] = (e.Bong.oPosition.nZ < e.Bong.oEndPos.nZ) ? e.Bong.oPosition.nZ : e.Bong.oEndPos.nZ;' + T + '// [CHUNET 14/09 c]' + R,
   T + T + T + 'vZ[k] = (e.Bong.oPosition.nZ < e.Bong.oEndPos.nZ)')

va(RC,
   T + T + 'else if (e.nLoai == 2) { vCx[k] = e.Param.nX + e.nLineWidth / 2; vY[k] = e.Param.nY; }' + R,
   T + T + 'else if (e.nLoai == 2) { vCx[k] = e.Param.nX + e.nLineWidth / 2; vY[k] = e.Param.nY; vZ[k] = e.Param.nZ; }' + T + '// [CHUNET 14/09 c] + z' + R,
   'vZ[k] = e.Param.nZ;')

va(RC,
   T + T + T + 'vCx[k] = e.nX + e.nFontId * nDai / 4; vY[k] = e.nY;' + R,
   T + T + T + 'vCx[k] = e.nX + e.nFontId * nDai / 4; vY[k] = e.nY; vZ[k] = e.nZ;' + T + '// [CHUNET 14/09 c] + z' + R,
   'vY[k] = e.nY; vZ[k] = e.nZ;')

va(RC,
   T + T + T + 'vCxN.push_back(vCx[k]); vChan.push_back(vY[k]); vCoChu.push_back((nLoai != 3) ? 1 : 0);' + R +
   T + T + '}' + R +
   T + T + 'else if (nLoai != 3 && (!vCoChu[nNhom] || vY[k] > vChan[nNhom])) { vChan[nNhom] = vY[k]; vCoChu[nNhom] = 1; }' + R,
   T + T + T + 'vCxN.push_back(vCx[k]); vChan.push_back(vY[k]); vCoChu.push_back((nLoai != 3) ? 1 : 0); vZMin.push_back(vZ[k]);' + T + '// [CHUNET 14/09 c] zMin = chieu cao nhan vat' + R +
   T + T + '}' + R +
   T + T + 'else' + R +
   T + T + '{' + R +
   T + T + T + 'if (nLoai != 3 && (!vCoChu[nNhom] || vY[k] > vChan[nNhom])) { vChan[nNhom] = vY[k]; vCoChu[nNhom] = 1; }' + R +
   T + T + T + 'if (vZ[k] < vZMin[nNhom]) vZMin[nNhom] = vZ[k];' + R +
   T + T + '}' + R,
   '[CHUNET 14/09 c] zMin = chieu cao nhan vat')

# ve: chieu cao chan (zMin) co theo zoom, phan xep dong (z - zMin) giu 1:1
va(RC,
   T + T + 'const int g = vNhom[k], cxN = vCxN[g], yChan = vChan[g];' + R +
   T + T + 'int ax = cxN, ay = yChan;' + R +
   T + T + 'CoordinateTransform(ax, ay, 0);' + T + '// chan nhom -> khung (qua cua zoom / lac)' + R,
   T + T + 'const int g = vNhom[k], cxN = vCxN[g], yChan = vChan[g], zMin = vZMin[g];' + R +
   T + T + 'int ax = cxN, ay = yChan;' + R +
   T + T + 'CoordinateTransform(ax, ay, 0);' + T + '// chan nhom -> khung (qua cua zoom / lac)' + R +
   T + T + 'ay -= (int)((float)((zMin * 887) >> 10) * 1000.0f / (float)m_nTgZoom + 0.5f);' + T + '// [CHUNET 14/09 c] chieu cao nhan vat (z nho nhat cua nhom) CO THEO ZOOM nhu than sprite -> chu / thanh mau bam sat dau' + R,
   '[CHUNET 14/09 c] chieu cao nhan vat (z nho nhat cua nhom) CO THEO ZOOM')

va(RC,
   T + T + T + 'b.oPosition.nY = ay + (e.Bong.oPosition.nY - yChan) / 2 - ((e.Bong.oPosition.nZ * 887) >> 10);' + R +
   T + T + T + 'b.oEndPos.nY = ay + (e.Bong.oEndPos.nY - yChan) / 2 - ((e.Bong.oEndPos.nZ * 887) >> 10);' + R,
   T + T + T + 'b.oPosition.nY = ay + (e.Bong.oPosition.nY - yChan) / 2 - (((e.Bong.oPosition.nZ - zMin) * 887) >> 10);' + T + '// [CHUNET 14/09 c] z - zMin' + R +
   T + T + T + 'b.oEndPos.nY = ay + (e.Bong.oEndPos.nY - yChan) / 2 - (((e.Bong.oEndPos.nZ - zMin) * 887) >> 10);' + R,
   '(((e.Bong.oPosition.nZ - zMin) * 887) >> 10)')

va(RC,
   'p.nY = ay + (e.Param.nY - yChan) / 2 - ((e.Param.nZ * 887) >> 10); p.nZ = TEXT_IN_SINGLE_PLANE_COORD;',
   'p.nY = ay + (e.Param.nY - yChan) / 2 - (((e.Param.nZ - zMin) * 887) >> 10); p.nZ = TEXT_IN_SINGLE_PLANE_COORD;',
   '(((e.Param.nZ - zMin) * 887) >> 10)')

va(RC,
   T + T + T + 'int x = ax + (e.nX - cxN), y = ay + (e.nY - yChan) / 2 - ((e.nZ * 887) >> 10);' + R,
   T + T + T + 'int x = ax + (e.nX - cxN), y = ay + (e.nY - yChan) / 2 - (((e.nZ - zMin) * 887) >> 10);' + T + '// [CHUNET 14/09 c] z - zMin' + R,
   '(((e.nZ - zMin) * 887) >> 10)')

# ============================================================ B. Cong tac "Thanh zoom" (nut 14) ============================================================
va(ZC,
   'KUiZoomThanh* KUiZoomThanh::m_pSelf = NULL;' + R,
   'KUiZoomThanh* KUiZoomThanh::m_pSelf = NULL;' + R +
   'static int s_nThanhZoomBat = 1;' + T + '// [ZOOMTHANH 14/09 c] cong tac "Thanh zoom" (Cai dat > Toi uu), mac dinh BAT' + R +
   R +
   '// [ZOOMTHANH 14/09 c] UiOptions2: bat / tat thanh keo zoom (tat = khong ve, cham xuyen qua; van zoom duoc bang chum neu bat "Chum zoom")' + R +
   'extern "C" void JxZoomThanh_DatBat(int nBat)' + R +
   '{' + R +
   T + 's_nThanhZoomBat = nBat ? 1 : 0;' + R +
   '}' + R,
   '[ZOOMTHANH 14/09 c] cong tac "Thanh zoom"')

va(ZC,
   T + 'int nBat = JxLia_ZoomGioiHan(&nMin, &nMax, &nBuoc);' + R,
   T + 'int nBat = JxLia_ZoomGioiHan(&nMin, &nMax, &nBuoc) && s_nThanhZoomBat;' + T + '// [ZOOMTHANH 14/09 c] + cong tac Thanh zoom' + R,
   '[ZOOMTHANH 14/09 c] + cong tac Thanh zoom')

va(OH,
   T + 'OPTION_I_CHUMZOOM,' + T + T + '// [ZOOMTHANH 14/09] chum hai ngon zoom (mac dinh TAT: da co thanh keo zoom KUiZoomThanh, tranh nham khi bam man choi)',
   T + 'OPTION_I_CHUMZOOM,' + T + T + '// [ZOOMTHANH 14/09] chum hai ngon zoom (mac dinh TAT: da co thanh keo zoom KUiZoomThanh, tranh nham khi bam man choi)' + R +
   T + 'OPTION_I_THANHZOOM,' + T + '// [ZOOMTHANH 14/09 c] hien thanh keo zoom (mac dinh BAT)',
   'OPTION_I_THANHZOOM,')

va(OH,
   '#define MAX_TOGGLE_BTN_COUNT 13' + T + '// [ZOOMTHANH 14/09] 12 -> 13',
   '#define MAX_TOGGLE_BTN_COUNT 14' + T + '// [ZOOMTHANH 14/09 c] 13 -> 14 (+ Thanh zoom, du 7 hang x 2); [ZOOMTHANH 14/09] 12 -> 13',
   'MAX_TOGGLE_BTN_COUNT 14' + T + '// [ZOOMTHANH 14/09 c]')

va(OC,
   '#define MAX_TOGGLE_BTN_COUNT 13' + T + '// [ZOOMTHANH 14/09] 13 nut',
   '#define MAX_TOGGLE_BTN_COUNT 14' + T + '// [ZOOMTHANH 14/09 c] 14 nut (+ Thanh zoom); [ZOOMTHANH 14/09] 13 nut',
   'MAX_TOGGLE_BTN_COUNT 14' + T + '// [ZOOMTHANH 14/09 c]')

va(OC,
   T + '"ChumZoom",' + T + '// [ZOOMTHANH 14/09]',
   T + '"ChumZoom", "ThanhZoom",' + T + '// [ZOOMTHANH 14/09] (c: + ThanhZoom)',
   '"ChumZoom", "ThanhZoom",')

va(OC,
   'extern "C" void JxVatRoi_DatBat(int nBat);',
   'extern "C" void JxVatRoi_DatBat(int nBat);' + R +
   'extern "C" void JxZoomThanh_DatBat(int nBat);' + T + '// [ZOOMTHANH 14/09 c] Ui/UiCase/UiZoomThanh.cpp: hien / an thanh keo zoom',
   'extern "C" void JxZoomThanh_DatBat(int nBat);')

va(OC,
   T + 'case OPTION_I_CHUMZOOM:' + T + '// [ZOOMTHANH 14/09] ap ngay' + R +
   T + T + 'JxLia_DatChum(bEnable);' + R +
   T + T + 'break;' + R,
   T + 'case OPTION_I_CHUMZOOM:' + T + '// [ZOOMTHANH 14/09] ap ngay' + R +
   T + T + 'JxLia_DatChum(bEnable);' + R +
   T + T + 'break;' + R +
   T + 'case OPTION_I_THANHZOOM:' + T + '// [ZOOMTHANH 14/09 c]' + R +
   T + T + 'JxZoomThanh_DatBat(bEnable);' + R +
   T + T + 'break;' + R,
   'case OPTION_I_THANHZOOM:')

va(OC,
   T + T + T + 'pSetting->GetInteger(OPTIONS_SAVE_SECTION2, ls_ToggleOptionName2[OPTION_I_LAC], true, &bOptionsEnable[OPTION_I_LAC]);',
   T + T + T + 'pSetting->GetInteger(OPTIONS_SAVE_SECTION2, ls_ToggleOptionName2[OPTION_I_LAC], true, &bOptionsEnable[OPTION_I_LAC]);' + R +
   T + T + T + 'pSetting->GetInteger(OPTIONS_SAVE_SECTION2, ls_ToggleOptionName2[OPTION_I_THANHZOOM], true, &bOptionsEnable[OPTION_I_THANHZOOM]);' + T + '// [ZOOMTHANH 14/09 c] thanh zoom mac dinh BAT',
   'ls_ToggleOptionName2[OPTION_I_THANHZOOM], true')

va(OC,
   T + T + 'JxLia_DatChum(bOptionsEnable[OPTION_I_CHUMZOOM]);' + T + '// [ZOOMTHANH 14/09] mac dinh TAT (GetInteger mac dinh false)',
   T + T + 'JxLia_DatChum(bOptionsEnable[OPTION_I_CHUMZOOM]);' + T + '// [ZOOMTHANH 14/09] mac dinh TAT (GetInteger mac dinh false)' + R +
   T + T + 'JxZoomThanh_DatBat(bOptionsEnable[OPTION_I_THANHZOOM]);' + T + '// [ZOOMTHANH 14/09 c]',
   'JxZoomThanh_DatBat(bOptionsEnable[OPTION_I_THANHZOOM]);')

# ============================================================ C. uioptions2.ini: ten 13 = Thanh zoom (ASCII) ============================================================
def them_ten(p):
    if not os.path.isfile(p):
        KQ.append('khong thay: ' + p); return
    s = doc(p)
    if '13=Thanh zoom' in s:
        KQ.append('bo qua (da co): ' + p); return
    nl = '\r\n' if s.count('\r\n') > s.count('\n') // 2 else '\n'
    i = s.index('[ToggleOptionsName]'); j = s.find(nl + '12=', i)
    if j < 0:
        raise SystemExit('khong thay 12= o ' + p)
    k = s.find(nl, j + 1)
    if k < 0: k = len(s)
    s2 = s[:k] + nl + '; [ZOOMTHANH 14/09 c] cong tac 14 = hien thanh keo zoom (mac dinh BAT)' + nl + '13=Thanh zoom' + s[k:]
    ghi(p, s2); KQ.append('da them ten 13: ' + p)

for g in [os.path.join(GOC, 'android', 'du_lieu_ghi_de'), r'D:\jx1_android_data', r'D:\jx1_android_data_dt_v4']:
    them_ten(os.path.join(g, 'ui', 'ui3', 'uioptions2.ini'))

for k in KQ:
    print(k)
