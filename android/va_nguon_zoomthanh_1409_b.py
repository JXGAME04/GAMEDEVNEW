# -*- coding: latin-1 -*-
# [ZOOMTHANH 14/09 b] + [CHUNET 14/09 b] - chu 14/09 14:1x: "chua thay cho zoom keo dieu chinh" / "zoom no chua co dinh theo kich co man hinh nen
# man hinh rong bi che boi kenh chat" / "khi zoom rong ra thi ten bang hoi voi danh hieu bi lech".
#   1. Thanh keo zoom NAM NGANG, neo GIUA MEP TREN (duoi hang icon menu, khong kenh chat nao cham toi) qua bang bo cuc hai ho man hinh
#      (ui\uitoado_macdinh.ini + _rong.ini: KUiZoomThanh|Main = 442,96,1000,0,1,0 -> NeoX = 1 giua); ini uizoomthanh.ini Type=0; ma doc Type
#      de dao chieu (trai = nhin rong, phai = phong to). Thanh doc (Type=1) van dung duoc.
#   2. Lop chu the gioi: tra hang theo NEO CHAN nhom (cung tam x nMpsX, gan nhau theo y) qua cua zoom / lac, cong do lech 1:1 KHONG thu phong
#      (Core can giua chu quanh nMpsX va xep dong theo do cao o toa do 1:1 -> qua zoom moi dong lech mot kieu). Thanh mau hai doan gop tam.
# Idempotent, latin-1, neo CRLF. Dung: python android/va_nguon_zoomthanh_1409_b.py [goc]
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

def thay_khoi(p, dau, cuoi, moi, dau_hieu):
    """Thay doan [dau, cuoi) (cuoi giu lai) bang moi."""
    s = doc(p)
    if dau_hieu in s:
        KQ.append('bo qua (da co): %s : %s' % (os.path.basename(p), dau_hieu[:60])); return
    i = s.find(dau); j = s.find(cuoi, i + 1)
    if i < 0 or j < 0 or s.count(dau) != 1:
        raise SystemExit('LOI: khong tim thay khoi %r .. %r trong %s' % (dau[:50], cuoi[:50], p))
    s2 = s[:i] + moi + s[j:]
    if cao(s2) != cao(s):
        raise SystemExit('LOI: so byte cao doi o %s' % p)
    ghi(p, s2)
    KQ.append('da thay khoi: %s : %s' % (os.path.basename(p), dau_hieu[:60]))

RC = os.path.join(GOC, 'Sources', 'Represent', 'Represent3', 'KRepresentShell3.cpp')
ZC = os.path.join(GOC, 'Sources', 'S3Client', 'Ui', 'UiCase', 'UiZoomThanh.cpp')
ZH = os.path.join(GOC, 'Sources', 'S3Client', 'Ui', 'UiCase', 'UiZoomThanh.h')

# ============================================================ A. Represent3: tra hang theo neo chan nhom ============================================================
va(RC,
   '// [CHUNET 14/09] dang xep hang chu the gioi? (chi trong luc ve RT va co zoom / lac; khong xep khi dang tra hang)' + R,
   'static bool Rep3ChuGiu(const char* psText, int nCount, int nFont, int& nX, int& nY);' + T + '// [CHUNET 14/09 b] dinh nghia phia duoi (CHUGIU 09/09)' + R +
   '// [CHUNET 14/09] dang xep hang chu the gioi? (chi trong luc ve RT va co zoom / lac; khong xep khi dang tra hang)' + R,
   '[CHUNET 14/09 b] dinh nghia phia duoi')

TGCHUVE = (
    '// [CHUNET 14/09 b] tra hang chu len khung (lenh 2 sau blit, m_nTgTrangThai == 0 -> cua toa do dang mo). Chu giu CO THAT nen KHONG bien doi thang' + R +
    '// toa do goc: Core da can giua moi dong quanh nMpsX theo be rong chu va xep dong theo do cao o toa do 1:1 -> qua zoom thi do lech bi thu phong,' + R +
    '// moi dong lech mot kieu, cac dong chong len nhau (chu 14/09: "ten bang hoi voi danh hieu bi lech"). Cach: gom cac phan tu LIEN TIEP cung tam x' + R +
    '// (+-2) va gan nhau theo y (150 don vi the gioi) thanh mot NHOM = mot NPC; neo nhom = (tam x, y chan = y lon nhat cua dong chu, z 0) di qua' + R +
    '// CoordinateTransform (cua zoom / lac); moi phan tu ve o neo + do lech 1:1 khong thu phong ((x - tam), (y - chan) / 2, - z x 887 / 1024).' + R +
    '// Thanh mau hai doan (RU_T_SHADOW ke nhau cung y / z) gop tam chung. Chu dat cham (Rep3ChuGiu) van ap nhu chu neo the gioi.' + R +
    'void KRepresentShell3::TgChuVe()' + R +
    '{' + R +
    T + 'if (s_TgChu.empty()) return;' + R +
    T + 'const int n = (int)s_TgChu.size();' + R +
    T + 'static std::vector<int> vCx, vY, vNhom, vCxN, vChan; static std::vector<char> vCoChu;' + R +
    T + 'vCx.resize(n); vY.resize(n); vNhom.resize(n); vCxN.clear(); vChan.clear(); vCoChu.clear();' + R +
    T + 'for (int k = 0; k < n; k++)' + R +
    T + '{' + T + '// tam x + y tham chieu (toa do the gioi) cua tung phan tu' + R +
    T + T + 'const KTgChu& e = s_TgChu[k];' + R +
    T + T + 'if (e.nLoai == 3)' + R +
    T + T + '{' + R +
    T + T + T + 'vCx[k] = (e.Bong.oPosition.nX + e.Bong.oEndPos.nX) / 2; vY[k] = (e.Bong.oPosition.nY + e.Bong.oEndPos.nY) / 2;' + R +
    T + T + T + 'if (k > 0 && s_TgChu[k - 1].nLoai == 3)' + R +
    T + T + T + '{' + T + '// chuoi bong ke nhau (thanh mau: doan do + doan xam) cung y / z, dau doan nay = cuoi doan truoc -> tam chung ca chuoi' + R +
    T + T + T + T + 'const KRUShadow& t = s_TgChu[k - 1].Bong;' + R +
    T + T + T + T + 'if (t.oPosition.nY == e.Bong.oPosition.nY && t.oEndPos.nY == e.Bong.oEndPos.nY && t.oPosition.nZ == e.Bong.oPosition.nZ && t.oEndPos.nZ == e.Bong.oEndPos.nZ && t.oEndPos.nX == e.Bong.oPosition.nX)' + R +
    T + T + T + T + '{' + R +
    T + T + T + T + T + 'int j = k - 1;' + R +
    T + T + T + T + T + 'while (j > 0 && s_TgChu[j - 1].nLoai == 3 && vCx[j - 1] == vCx[j] && s_TgChu[j - 1].Bong.oEndPos.nX == s_TgChu[j].Bong.oPosition.nX) j--;' + R +
    T + T + T + T + T + 'const int cx = (s_TgChu[j].Bong.oPosition.nX + e.Bong.oEndPos.nX) / 2;' + R +
    T + T + T + T + T + 'for (int m = j; m <= k; m++) vCx[m] = cx;' + R +
    T + T + T + T + '}' + R +
    T + T + T + '}' + R +
    T + T + '}' + R +
    T + T + 'else if (e.nLoai == 2) { vCx[k] = e.Param.nX + e.nLineWidth / 2; vY[k] = e.Param.nY; }' + R +
    T + T + 'else' + R +
    T + T + '{' + T + '// Core can giua: nX = nMpsX - font x byte / 4 -> tam = nX + font x byte / 4' + R +
    T + T + T + 'int nDai = e.nCount; if (nDai < 0) nDai = (e.nVb >= 0) ? (int)strlen(&s_TgChuVb[e.nVb]) : 0;' + R +
    T + T + T + 'vCx[k] = e.nX + e.nFontId * nDai / 4; vY[k] = e.nY;' + R +
    T + T + '}' + R +
    T + '}' + R +
    T + 'int nNhom = -1, nCxNhom = 0, nYNhom = 0;' + R +
    T + 'for (int k = 0; k < n; k++)' + R +
    T + '{' + T + '// gom nhom; y chan = y lon nhat cua dong CHU trong nhom (dong ten o nMpsY), chua co chu thi y dau tien' + R +
    T + T + 'const int nLoai = s_TgChu[k].nLoai;' + R +
    T + T + 'if (nNhom < 0 || vCx[k] < nCxNhom - 2 || vCx[k] > nCxNhom + 2 || vY[k] < nYNhom - 150 || vY[k] > nYNhom + 150)' + R +
    T + T + '{' + R +
    T + T + T + 'nNhom = (int)vCxN.size(); nCxNhom = vCx[k]; nYNhom = vY[k];' + R +
    T + T + T + 'vCxN.push_back(vCx[k]); vChan.push_back(vY[k]); vCoChu.push_back((nLoai != 3) ? 1 : 0);' + R +
    T + T + '}' + R +
    T + T + 'else if (nLoai != 3 && (!vCoChu[nNhom] || vY[k] > vChan[nNhom])) { vChan[nNhom] = vY[k]; vCoChu[nNhom] = 1; }' + R +
    T + T + 'vNhom[k] = nNhom;' + R +
    T + '}' + R +
    T + 's_bTgChuTra = true;' + R +
    T + 'for (int k = 0; k < n; k++)' + R +
    T + '{' + R +
    T + T + 'const KTgChu& e = s_TgChu[k];' + R +
    T + T + 'const int g = vNhom[k], cxN = vCxN[g], yChan = vChan[g];' + R +
    T + T + 'int ax = cxN, ay = yChan;' + R +
    T + T + 'CoordinateTransform(ax, ay, 0);' + T + '// chan nhom -> khung (qua cua zoom / lac)' + R +
    T + T + 'char* vb = (e.nVb >= 0) ? &s_TgChuVb[e.nVb] : NULL;' + R +
    T + T + 'if (e.nLoai == 3)' + R +
    T + T + '{' + R +
    T + T + T + 'KRUShadow b = e.Bong;' + R +
    T + T + T + 'b.oPosition.nX = ax + (e.Bong.oPosition.nX - cxN); b.oEndPos.nX = ax + (e.Bong.oEndPos.nX - cxN);' + R +
    T + T + T + 'b.oPosition.nY = ay + (e.Bong.oPosition.nY - yChan) / 2 - ((e.Bong.oPosition.nZ * 887) >> 10);' + R +
    T + T + T + 'b.oEndPos.nY = ay + (e.Bong.oEndPos.nY - yChan) / 2 - ((e.Bong.oEndPos.nZ * 887) >> 10);' + R +
    T + T + T + 'b.oPosition.nZ = b.oEndPos.nZ = 0;' + R +
    T + T + T + 'DrawPrimitives(1, (KRepresentUnit*)&b, RU_T_SHADOW, TRUE);' + R +
    T + T + '}' + R +
    T + T + 'else if (e.nLoai == 2)' + R +
    T + T + '{' + R +
    T + T + T + 'if (!vb) continue;' + R +
    T + T + T + 'KOutputTextParam p = e.Param;' + R +
    T + T + T + 'p.nX = ax + (e.Param.nX - cxN); p.nY = ay + (e.Param.nY - yChan) / 2 - ((e.Param.nZ * 887) >> 10); p.nZ = TEXT_IN_SINGLE_PLANE_COORD;' + R +
    T + T + T + 'OutputRichText(e.nFontId, &p, vb, e.nCount, e.nLineWidth);' + R +
    T + T + '}' + R +
    T + T + 'else' + R +
    T + T + '{' + R +
    T + T + T + 'if (!vb) continue;' + R +
    T + T + T + 'int x = ax + (e.nX - cxN), y = ay + (e.nY - yChan) / 2 - ((e.nZ * 887) >> 10);' + R +
    T + T + T + 'Rep3ChuGiu(vb, e.nCount, e.nFontId, x, y);' + R +
    T + T + T + 'if (e.nLoai == 0) OutputText(e.nFontId, vb, e.nCount, x, y, e.Color, e.nLineWidth, TEXT_IN_SINGLE_PLANE_COORD, e.BorderColor);' + R +
    T + T + T + 'else OutputVNText(e.nFontId, vb, e.nCount, x, y, e.Color, e.nLineWidth, TEXT_IN_SINGLE_PLANE_COORD, e.BorderColor);' + R +
    T + T + '}' + R +
    T + '}' + R +
    T + 's_bTgChuTra = false;' + R +
    '}' + R +
    R)
thay_khoi(RC,
          '// [CHUNET 14/09] tra hang chu len khung (goi o lenh 2 sau blit, m_nTgTrangThai == 0 -> cua toa do dang mo): goi lai dung ham, dung tham so goc' + R,
          'int KRepresentShell3::JxTheGioi(int nLenh, int nThamSo)' + R,
          TGCHUVE,
          '[CHUNET 14/09 b] tra hang chu len khung')

# ============================================================ B. Thanh keo: ngang / doc theo Type, dao chieu khi ngang ============================================================
va(ZH,
   'm_bAn;' + T + T + T + '// 1 = zoom tat (map / nguoi choi) -> khong ve, cham xuyen qua',
   'm_bAn;' + T + T + T + '// 1 = zoom tat (map / nguoi choi) -> khong ve, cham xuyen qua' + R +
   T + 'int' + T + T + T + T + 'm_bNgang;' + T + T + '// [ZOOMTHANH 14/09 b] 1 = thanh NGANG ([Thanh] Type=0): trai = nhin rong (-), phai = phong to (+); 0 = doc: tren = phong to',
   'm_bNgang;')

va(ZC,
   T + 'm_bDangDat = 0; m_bAn = 0; m_uKeoCuoi = 0;',
   T + 'm_bDangDat = 0; m_bAn = 0; m_uKeoCuoi = 0; m_bNgang = 0;',
   'm_uKeoCuoi = 0; m_bNgang = 0;')

va(ZC,
   T + T + T + 'm_pSelf->m_Thanh.Init(&Ini, "Thanh");' + R,
   T + T + T + 'm_pSelf->m_Thanh.Init(&Ini, "Thanh");' + R +
   T + T + T + '{ int nKieu = 0; Ini.GetInteger("Thanh", "Type", 0, &nKieu); m_pSelf->m_bNgang = (nKieu == 0) ? 1 : 0; }' + T + '// [ZOOMTHANH 14/09 b] Type=0 ngang' + R,
   '[ZOOMTHANH 14/09 b] Type=0 ngang')

va(ZC,
   T + 'int nGia = (JxLia_ZoomDich() - m_nMin + m_nBuoc / 2) / m_nBuoc;',
   T + 'int nGia = m_bNgang ? (m_nMax - JxLia_ZoomDich() + m_nBuoc / 2) / m_nBuoc : (JxLia_ZoomDich() - m_nMin + m_nBuoc / 2) / m_nBuoc;' + T + '// [ZOOMTHANH 14/09 b] ngang: 0 = nhin rong nhat (trai)',
   '[ZOOMTHANH 14/09 b] ngang: 0 = nhin rong nhat (trai)')

va(ZC,
   T + T + T + 'JxLia_ZoomDatMuot(m_nMin + (int)nParam * m_nBuoc);',
   T + T + T + 'JxLia_ZoomDatMuot(m_bNgang ? m_nMax - (int)nParam * m_nBuoc : m_nMin + (int)nParam * m_nBuoc);' + T + '// [ZOOMTHANH 14/09 b]',
   'JxLia_ZoomDatMuot(m_bNgang ?')

# ============================================================ C. Bo cuc mac dinh hai ho: neo giua mep tren ============================================================
def them_dong(p, dong, dau):
    if not os.path.isfile(p):
        KQ.append('khong thay: ' + p); return
    s = doc(p)
    if dau in s:
        KQ.append('bo qua (da co): ' + p); return
    nl = '\r\n' if s.count('\r\n') > s.count('\n') // 2 else '\n'
    if not s.endswith(nl): s += nl
    ghi(p, s + dong.replace('\n', nl))
    KQ.append('da them: ' + p)

for g in [os.path.join(GOC, 'android', 'du_lieu_ghi_de'), r'D:\jx1_android_data', r'D:\jx1_android_data_dt_v4']:
    for ten in ('uitoado_macdinh.ini', 'uitoado_macdinh_rong.ini'):
        them_dong(os.path.join(g, 'ui', ten),
                  '; [ZOOMTHANH 14/09 b] thanh keo zoom NGANG neo GIUA mep tren (duoi hang icon menu), khong bi kenh chat che; nguoi choi keo cho khac trong Chinh giao dien\n'
                  'KUiZoomThanh|Main=442,96,1000,0,1,0\n', 'KUiZoomThanh|Main=')

for k in KQ:
    print(k)
