# -*- coding: latin-1 -*-
# [ZOOMTHANH 14/09] + [CHUNET 14/09] - chu 14/09 13:2x: "Lam nut keo dieu chinh zoom cho khoi anh huong toi luc kich vao man choi. Va luc zoom
# rong chu bi mo di khong nhin ro".
#   1. Thanh keo zoom (Ui/UiCase/UiZoomThanh.cpp/.h moi, JX_MOBILE): mo o UiStartGame, dong o roi game (UiShell.cpp); API JxLiaCanh
#      JxLia_ZoomGioiHan / JxLia_ZoomDich / JxLia_ZoomDatMuot; trinh chinh giao dien: khoa KUiZoomThanh|Main (UiToaDoMobile.inc + uitoado_danhsach.ini);
#      CMake Android + iOS them tep.
#   2. Cong tac "Chum zoom" (hai ngon) trong Cai dat > Toi uu, mac dinh TAT: UiOptions2 (13 cong tac, 7 hang) + JxLia_DatChum + JxLia_ChumDuoc.
#   3. Chu the gioi sac net khi nhin rong / phong to / lac ([CHUNET], Represent3 khoi [TG], soi cheo phien do nhip): OutputText / OutputVNText /
#      OutputRichText va RU_T_SHADOW toa do the gioi (nen mo sau ten, thanh mau) goi trong luc ve RT thi xep hang, lenh 2 sau blit goi lai
#      -> CoordinateTransform qua cua toa do (m_nTgTrangThai == 0) dung cho, co that. [Client] TheGioiRTChu=0 tat.
# Idempotent, latin-1, neo CRLF (nguon) / LF (CMake). Dung: python android/va_nguon_zoomthanh_1409.py [goc]
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

def crlf(p):
    s = doc(p)
    if '\r\n' not in s:
        ghi(p, s.replace('\n', '\r\n')); KQ.append('CRLF: ' + os.path.basename(p))

RC = os.path.join(GOC, 'Sources', 'Represent', 'Represent3', 'KRepresentShell3.cpp')
RH = os.path.join(GOC, 'Sources', 'Represent', 'Represent3', 'KRepresentShell3.h')
JL = os.path.join(GOC, 'Sources', 'S3Client', 'Platform', 'JxLiaCanh.cpp')
JH = os.path.join(GOC, 'Sources', 'S3Client', 'Platform', 'JxLiaCanh.h')
OH = os.path.join(GOC, 'Sources', 'S3Client', 'Ui', 'UiCase', 'UiOptions2.h')
OC = os.path.join(GOC, 'Sources', 'S3Client', 'Ui', 'UiCase', 'UiOptions2.cpp')
US = os.path.join(GOC, 'Sources', 'S3Client', 'Ui', 'UiShell.cpp')
TD = os.path.join(GOC, 'Sources', 'S3Client', 'Ui', 'Elem', 'UiToaDoMobile.inc')
ZC = os.path.join(GOC, 'Sources', 'S3Client', 'Ui', 'UiCase', 'UiZoomThanh.cpp')
ZH = os.path.join(GOC, 'Sources', 'S3Client', 'Ui', 'UiCase', 'UiZoomThanh.h')
CMA = os.path.join(GOC, 'android', 'CMakeLists.txt')
CMI = os.path.join(GOC, 'ios', 'CMakeLists.txt')

crlf(ZC); crlf(ZH)

# ============================================================ A. Represent3 [CHUNET 14/09] ============================================================
va(RH,
   '    bool JxTheGioiNet2();' + T + '// [ZOOM3D 14/09] RT2 co 2x cho phong to co loc net (RT -> RT2 POINT -> khung LINEAR)',
   '    bool JxTheGioiNet2();' + T + '// [ZOOM3D 14/09] RT2 co 2x cho phong to co loc net (RT -> RT2 POINT -> khung LINEAR)' + R +
   '    bool TgChuXep();' + T + '// [CHUNET 14/09] dang ve RT + co zoom / lac: chu the gioi + nen chu / thanh mau (RU_T_SHADOW toa do the gioi) xep hang, ve SAU blit len khung (co that, sac net)' + R +
   '    void TgChuVe();' + T + '// ve hang do (lenh 2 sau blit; khung chi blit ve lai hang cua khung truoc)',
   '[CHUNET 14/09] dang ve RT + co zoom / lac')

va(RC,
   'static int s_nTgDaChamRt = 0, s_nTgDaChamRt2 = 0;' + T + '// [TGCAP 14/09 b] da cham san RT / RT2 (pass rong CLEAR) sau khi cap; ve 0 khi huy',
   'static int s_nTgDaChamRt = 0, s_nTgDaChamRt2 = 0;' + T + '// [TGCAP 14/09 b] da cham san RT / RT2 (pass rong CLEAR) sau khi cap; ve 0 khi huy' + R +
   '#include <vector>' + T + '// [CHUNET 14/09]' + R +
   '// [CHUNET 14/09] lop chu the gioi ve SAU blit khi nhin rong / phong to / lac (chu 14/09: "luc zoom rong chu bi mo di khong nhin ro"): OutputText /' + R +
   '// OutputVNText / OutputRichText va RU_T_SHADOW toa do the gioi (nen mo sau ten, thanh mau) goi TRONG luc ve RT duoc xep hang; lenh 2 (sau blit)' + R +
   '// goi lai y het -> CoordinateTransform di qua cua toa do (m_nTgTrangThai == 0) nen dung cho, ve o co that (khong thu nho / phong to cung anh RT).' + R +
   '// Khung chi blit (K = 2) ve lai hang cua khung truoc (anh RT cung cu). Zoom 100 % khong lac: khong xep (RT 1:1 da net). [Client] TheGioiRTChu=0 tat.' + R +
   'struct KTgChu { int nLoai, nFontId, nX, nY, nZ, nLineWidth, nCount, nVb; unsigned int Color, BorderColor; KOutputTextParam Param; KRUShadow Bong; };' + R +
   'static std::vector<KTgChu> s_TgChu; static std::vector<char> s_TgChuVb; static int s_nTgChuNet = 1; static bool s_bTgChuTra = false;' + R +
   'static void TgChuThem(int nLoai, int nFontId, const char* psText, int nCount, int nX, int nY, int nZ, unsigned int Color, int nLineWidth, unsigned int BorderColor, const KOutputTextParam* pParam, const KRUShadow* pBong)' + R +
   '{' + R +
   T + 'KTgChu e; memset(&e, 0, sizeof(e));' + R +
   T + 'e.nLoai = nLoai; e.nFontId = nFontId; e.nX = nX; e.nY = nY; e.nZ = nZ; e.nLineWidth = nLineWidth; e.Color = Color; e.BorderColor = BorderColor; e.nCount = nCount; e.nVb = -1;' + R +
   T + 'if (pParam) e.Param = *pParam;' + R +
   T + 'if (pBong) e.Bong = *pBong;' + R +
   T + 'if (psText)' + R +
   T + '{' + T + '// chep chuoi (toi 1024 byte), nCount >= 0 thi thanh so byte da chep; < 0 (KRF_ZERO_END) giu nguyen' + R +
   T + T + 'int nDai = 0; const int nToiDa = (nCount < 0) ? 1024 : ((nCount < 1024) ? nCount : 1024);' + R +
   T + T + 'while (nDai < nToiDa && psText[nDai]) nDai++;' + R +
   T + T + 'e.nVb = (int)s_TgChuVb.size(); if (nCount >= 0) e.nCount = nDai;' + R +
   T + T + 's_TgChuVb.insert(s_TgChuVb.end(), psText, psText + nDai); s_TgChuVb.push_back(0);' + R +
   T + '}' + R +
   T + 'if (s_TgChu.size() < 8192) s_TgChu.push_back(e);' + R +
   '}',
   '[CHUNET 14/09] lop chu the gioi ve SAU blit')

va(RC,
   'int KRepresentShell3::JxTheGioi(int nLenh, int nThamSo)' + R + '{' + R,
   '// [CHUNET 14/09] dang xep hang chu the gioi? (chi trong luc ve RT va co zoom / lac; khong xep khi dang tra hang)' + R +
   'bool KRepresentShell3::TgChuXep()' + R +
   '{' + R +
   T + 'return s_nTgChuNet && m_nTgTrangThai == 1 && !s_bTgChuTra && (m_nTgZoomRt != 1000 || m_nTgXoay || m_nTgDoc != 1000);' + R +
   '}' + R +
   R +
   '// [CHUNET 14/09] tra hang chu len khung (goi o lenh 2 sau blit, m_nTgTrangThai == 0 -> cua toa do dang mo): goi lai dung ham, dung tham so goc' + R +
   'void KRepresentShell3::TgChuVe()' + R +
   '{' + R +
   T + 'if (s_TgChu.empty()) return;' + R +
   T + 's_bTgChuTra = true;' + R +
   T + 'for (size_t k = 0; k < s_TgChu.size(); k++)' + R +
   T + '{' + R +
   T + T + 'const KTgChu& e = s_TgChu[k];' + R +
   T + T + 'char* vb = (e.nVb >= 0) ? &s_TgChuVb[e.nVb] : NULL;' + R +
   T + T + 'switch (e.nLoai)' + R +
   T + T + '{' + R +
   T + T + 'case 0: if (vb) OutputText(e.nFontId, vb, e.nCount, e.nX, e.nY, e.Color, e.nLineWidth, e.nZ, e.BorderColor); break;' + R +
   T + T + 'case 1: if (vb) OutputVNText(e.nFontId, vb, e.nCount, e.nX, e.nY, e.Color, e.nLineWidth, e.nZ, e.BorderColor); break;' + R +
   T + T + 'case 2: if (vb) { KOutputTextParam p = e.Param; OutputRichText(e.nFontId, &p, vb, e.nCount, e.nLineWidth); } break;' + R +
   T + T + 'case 3: { KRUShadow b = e.Bong; DrawPrimitives(1, (KRepresentUnit*)&b, RU_T_SHADOW, FALSE); } break;' + R +
   T + T + '}' + R +
   T + '}' + R +
   T + 's_bTgChuTra = false;' + R +
   '}' + R +
   R +
   'int KRepresentShell3::JxTheGioi(int nLenh, int nThamSo)' + R + '{' + R,
   '[CHUNET 14/09] tra hang chu len khung')

va(RC,
   T + 'Rep3Log("[TGCAP] RT cap %d, RT2 cap 2 x %d phan nghin khung, cap san %d (TheGioiRTCap / TheGioiRT2Cap / TheGioiRTSan)", s_nTgRtCap, s_nTgRt2Cap, s_nTgRtSan);',
   T + 'Rep3Log("[TGCAP] RT cap %d, RT2 cap 2 x %d phan nghin khung, cap san %d (TheGioiRTCap / TheGioiRT2Cap / TheGioiRTSan)", s_nTgRtCap, s_nTgRt2Cap, s_nTgRtSan);' + R +
   T + 's_nTgChuNet = Rep3Ini("TheGioiRTChu", 1) ? 1 : 0; Rep3Log("[CHUNET] lop chu the gioi ve sau blit khi zoom / lac: %d (TheGioiRTChu)", s_nTgChuNet);' + T + '// [CHUNET 14/09]',
   's_nTgChuNet = Rep3Ini("TheGioiRTChu"')

va(RC,
   's_uTgRTVe = 0xFFFFFFFFu; s_nTgDaChamRt = s_nTgDaChamRt2 = 0;',
   's_uTgRTVe = 0xFFFFFFFFu; s_nTgDaChamRt = s_nTgDaChamRt2 = 0; s_TgChu.clear(); s_TgChuVb.clear();' + T + '// [CHUNET 14/09] huy hang',
   '[CHUNET 14/09] huy hang')

va(RC,
   T + T + 'm_nTgTrangThai = 1; m_nTgLeft = m_nLeft; m_nTgTop = m_nTop;',
   T + T + 'm_nTgTrangThai = 1; m_nTgLeft = m_nLeft; m_nTgTop = m_nTop;' + R +
   T + T + 's_TgChu.clear(); s_TgChuVb.clear();' + T + '// [CHUNET 14/09] hang moi cho khung nay',
   '[CHUNET 14/09] hang moi cho khung nay')

va(RC,
   T + T + 'if (m_pTgSB) m_pTgSB->Apply();' + R +
   T + T + 'return 1;',
   T + T + 'if (m_pTgSB) m_pTgSB->Apply();' + R +
   T + T + 'TgChuVe();' + T + '// [CHUNET 14/09] lop chu the gioi len khung (sau blit, truoc giao dien; khung chi blit ve lai hang khung truoc)' + R +
   T + T + 'return 1;',
   '[CHUNET 14/09] lop chu the gioi len khung')

va(RC,
   T + 'int i;' + R +
   T + 'if(!psText || !psText[0])' + R +
   T + T + 'return;' + R +
   R +
   T + 'if(m_bDeviceLost)' + R +
   T + T + 'return;' + R +
   T + R +
   T + 'Color |= 0xff000000;' + R,
   T + 'int i;' + R +
   T + 'if(!psText || !psText[0])' + R +
   T + T + 'return;' + R +
   R +
   T + 'if(m_bDeviceLost)' + R +
   T + T + 'return;' + R +
   '#ifdef JX_MOBILE' + R +
   T + 'if (TgChuXep()) { TgChuThem(0, nFontId, psText, nCount, nX, nY, nZ, Color, nLineWidth, BorderColor, NULL, NULL); return; }' + T + '// [CHUNET 14/09] chu the gioi: ve sau blit' + R +
   '#endif' + R +
   T + R +
   T + 'Color |= 0xff000000;' + R,
   '[CHUNET 14/09] chu the gioi: ve sau blit')

va(RC,
   T + 'int i = 0;' + R +
   R +
   T + 'Color |= 0xff000000;' + R,
   T + 'int i = 0;' + R +
   '#ifdef JX_MOBILE' + R +
   T + 'if (TgChuXep()) { TgChuThem(1, nFontId, psText, nCount, nX, nY, nZ, Color, nLineWidth, BorderColor, NULL, NULL); return; }' + T + '// [CHUNET 14/09] chu VN the gioi: ve sau blit' + R +
   '#endif' + R +
   R +
   T + 'Color |= 0xff000000;' + R,
   '[CHUNET 14/09] chu VN the gioi: ve sau blit')

va(RC,
   T + 'if(!pParam || !psText || !psText[0])' + R +
   T + T + 'return 0;' + R +
   R +
   T + 'if(m_bDeviceLost)' + R +
   T + T + 'return 0;' + R,
   T + 'if(!pParam || !psText || !psText[0])' + R +
   T + T + 'return 0;' + R +
   R +
   T + 'if(m_bDeviceLost)' + R +
   T + T + 'return 0;' + R +
   '#ifdef JX_MOBILE' + R +
   T + 'if (TgChuXep()) { TgChuThem(2, nFontId, psText, nCount, 0, 0, 0, 0, nLineWidth, 0, pParam, NULL); return 0; }' + T + '// [CHUNET 14/09] chat tren dau (PaintChat): ve sau blit' + R +
   '#endif' + R,
   '[CHUNET 14/09] chat tren dau (PaintChat): ve sau blit')

va(RC,
   T + 'int i = 0;' + R +
   T + R +
   T + 'switch(uGenre)' + R +
   T + '{' + R +
   T + 'case RU_T_IMAGE:' + R +
   T + 'case RU_T_IMAGE_4:' + R,
   T + 'int i = 0;' + R +
   '#ifdef JX_MOBILE' + R +
   T + 'if (uGenre == RU_T_SHADOW && !bSinglePlaneCoord && TgChuXep()) { for (i = 0; i < nPrimitiveCount; i++) TgChuThem(3, 0, NULL, 0, 0, 0, 0, 0, 0, 0, NULL, ((KRUShadow*)pPrimitives) + i); return; }' + T + '// [CHUNET 14/09] nen mo sau ten + thanh mau (KNpc VeNenChu / PaintInfo): ve sau blit cung chu' + R +
   '#endif' + R +
   T + R +
   T + 'switch(uGenre)' + R +
   T + '{' + R +
   T + 'case RU_T_IMAGE:' + R +
   T + 'case RU_T_IMAGE_4:' + R,
   '[CHUNET 14/09] nen mo sau ten + thanh mau')

# ============================================================ B. JxLiaCanh: cong tac chum + API thanh keo ============================================================
va(JL,
   'static int' + T + 's_nNcZoomNhanh = 0, s_nNcZoomCham = 0, s_nNcLac = 1;' + T + '// cong tac Cai dat > Toi uu: Zoom nhanh / Zoom cham / Lac camera (JxLia_DatNhay)',
   'static int' + T + 's_nNcZoomNhanh = 0, s_nNcZoomCham = 0, s_nNcLac = 1;' + T + '// cong tac Cai dat > Toi uu: Zoom nhanh / Zoom cham / Lac camera (JxLia_DatNhay)' + R +
   'static int' + T + 's_nNcChum = 0;' + T + '// [ZOOMTHANH 14/09] cong tac "Chum zoom" (hai ngon): mac dinh TAT vi da co thanh keo zoom (KUiZoomThanh), tranh nham khi bam man choi bang hai ngon',
   '[ZOOMTHANH 14/09] cong tac "Chum zoom"')

va(JL,
   T + 'if (!s_nZoomBat || !s_nNcZoom)' + R +
   T + T + 'return false;' + T + '// [CAMERA 13/09 TUYCHON]' + R +
   T + 'return TrenBanDo(x1, y1) && TrenBanDo(x2, y2);',
   T + 'if (!s_nZoomBat || !s_nNcZoom)' + R +
   T + T + 'return false;' + T + '// [CAMERA 13/09 TUYCHON]' + R +
   T + 'if (!s_nNcChum)' + R +
   T + T + 'return false;' + T + '// [ZOOMTHANH 14/09] chum tat: ngon 2 dat len ban do khong lam gi (khong huy cham cua ngon 1)' + R +
   T + 'return TrenBanDo(x1, y1) && TrenBanDo(x2, y2);',
   '[ZOOMTHANH 14/09] chum tat: ngon 2')

va(JL,
   T + 'g_DebugLog("[ZOOM3D] tuy chon: zoom nhanh=%d cham=%d lac camera=%d", s_nNcZoomNhanh, s_nNcZoomCham, s_nNcLac);' + R +
   '}' + R,
   T + 'g_DebugLog("[ZOOM3D] tuy chon: zoom nhanh=%d cham=%d lac camera=%d", s_nNcZoomNhanh, s_nNcZoomCham, s_nNcLac);' + R +
   '}' + R +
   R +
   '// [ZOOMTHANH 14/09] cong tac "Chum zoom" (Cai dat > Toi uu, mac dinh TAT) + API cho thanh keo zoom (Ui/UiCase/UiZoomThanh.cpp)' + R +
   'void JxLia_DatChum(int nBat)' + R +
   '{' + R +
   T + 'if (nBat >= 0) s_nNcChum = nBat ? 1 : 0;' + R +
   T + 'g_DebugLog("[ZOOMTHANH] tuy chon: chum hai ngon zoom=%d", s_nNcChum);' + R +
   '}' + R +
   R +
   'int JxLia_ZoomGioiHan(int* pMin, int* pMax, int* pBuoc)' + R +
   '{' + R +
   T + 'DocCaiDat();' + R +
   T + 'if (pMin) *pMin = s_nZoomToiThieu;' + R +
   T + 'if (pMax) *pMax = s_nZoomToiDa;' + R +
   T + 'if (pBuoc) *pBuoc = s_nZoomBuoc;' + R +
   T + 'return (TrongGame() && s_nZoomBat && s_nNcZoom) ? 1 : 0;' + R +
   '}' + R +
   R +
   'int JxLia_ZoomDich()' + R +
   '{' + R +
   T + 'return (s_nZoomDich > 0) ? s_nZoomDich : s_nZoom;' + R +
   '}' + R +
   R +
   'void JxLia_ZoomDatMuot(int nPhanTram)' + R +
   '{' + T + '// thanh keo dat: dat dich, JxLia_Nhip troi toi (duoi ham mu ZoomMuot nhu chum kieu 3D); nho lai nhu nguoi choi chum' + R +
   T + 'DocCaiDat();' + R +
   T + 'if (nPhanTram < s_nZoomToiThieu) nPhanTram = s_nZoomToiThieu;' + R +
   T + 'if (nPhanTram > s_nZoomToiDa) nPhanTram = s_nZoomToiDa;' + R +
   T + 'nPhanTram = (nPhanTram + s_nZoomBuoc / 2) / s_nZoomBuoc * s_nZoomBuoc;' + R +
   T + 'if (nPhanTram < s_nZoomToiThieu) nPhanTram = s_nZoomToiThieu;' + R +
   T + 'if (nPhanTram == s_nZoom && s_nZoomDich == 0)' + R +
   T + T + 'return;' + R +
   T + 'if (s_nZoomDich == 0) s_fZoomTroi = (float)s_nZoom;' + T + '// bat dau troi tu muc dang ap; dang troi thi noi tiep' + R +
   T + 's_nZoomDich = nPhanTram;' + R +
   T + 's_nZoomNguoiChoi = nPhanTram;' + R +
   T + 'Camera_GhiNho();' + R +
   T + 'if (s_nNhatKy) g_DebugLog("[ZOOMTHANH] dat zoom %d%% (dang %d%%)", nPhanTram, s_nZoom);' + R +
   '}' + R,
   '[ZOOMTHANH 14/09] cong tac "Chum zoom" (Cai dat > Toi uu, mac dinh TAT) + API')

va(JH,
   'void JxLia_DatNhay(int nZoomNhanh, int nZoomCham, int nLac);' + T + '// [ZOOM3D 14/09] cong tac Zoom nhanh / Zoom cham / Lac camera',
   'void JxLia_DatNhay(int nZoomNhanh, int nZoomCham, int nLac);' + T + '// [ZOOM3D 14/09] cong tac Zoom nhanh / Zoom cham / Lac camera' + R +
   R +
   '// [ZOOMTHANH 14/09] cong tac "Chum zoom" (hai ngon; mac dinh TAT, thanh keo thay the) + API cho thanh keo zoom KUiZoomThanh (Ui/UiCase/UiZoomThanh.cpp)' + R +
   'void JxLia_DatChum(int nBat);' + R +
   'int  JxLia_ZoomGioiHan(int* pMin, int* pMax, int* pBuoc);' + T + '// % phong to nhat (ZoomToiThieu) / nhin rong nhat (ZoomToiDa map) / nac; tra ve 1 = zoom dang duoc phep (trong game, map + nguoi choi bat)' + R +
   'int  JxLia_ZoomDich();' + T + T + T + '// % dang troi toi (hay dang ap khi khong troi)' + R +
   'void JxLia_ZoomDatMuot(int nPhanTram);' + T + '// thanh keo dat: troi muot toi muc do, nho lai (UserData\\CameraMobile.ini)',
   '[ZOOMTHANH 14/09] cong tac "Chum zoom" (hai ngon; mac dinh TAT, thanh keo thay the)')

# ============================================================ C. UiOptions2: cong tac 13 "Chum zoom" ============================================================
va(OH,
   T + 'OPTION_I_LAC,' + T + T + T + '// camera lac nhe khi lia',
   T + 'OPTION_I_LAC,' + T + T + T + '// camera lac nhe khi lia' + R +
   T + 'OPTION_I_CHUMZOOM,' + T + T + '// [ZOOMTHANH 14/09] chum hai ngon zoom (mac dinh TAT: da co thanh keo zoom KUiZoomThanh, tranh nham khi bam man choi)',
   'OPTION_I_CHUMZOOM,')

va(OH,
   '#define MAX_TOGGLE_BTN_COUNT 12' + T + '// [ZOOM3D 14/09] 9 -> 12',
   '#define MAX_TOGGLE_BTN_COUNT 13' + T + '// [ZOOMTHANH 14/09] 12 -> 13 (+ Chum zoom): 7 hang tu Top=58 cach 23 (android/sinh_uioptions2_chum.py); [ZOOM3D 14/09] 9 -> 12',
   'MAX_TOGGLE_BTN_COUNT 13' + T + '// [ZOOMTHANH 14/09]')

va(OC,
   '#define MAX_TOGGLE_BTN_COUNT 12' + T + '// [ZOOM3D 14/09] 12 nut;',
   '#define MAX_TOGGLE_BTN_COUNT 13' + T + '// [ZOOMTHANH 14/09] 13 nut (+ Chum zoom); [ZOOM3D 14/09] 12 nut;',
   'MAX_TOGGLE_BTN_COUNT 13' + T + '// [ZOOMTHANH 14/09] 13 nut')

va(OC,
   T + '"ZoomNhanh", "ZoomCham", "LacCamera",' + T + '// [ZOOM3D 14/09]',
   T + '"ZoomNhanh", "ZoomCham", "LacCamera",' + T + '// [ZOOM3D 14/09]' + R +
   T + '"ChumZoom",' + T + '// [ZOOMTHANH 14/09]',
   T + '"ChumZoom",' + T + '// [ZOOMTHANH 14/09]')

va(OC,
   T + T + 'JxLia_DatNhay(m_ToggleItemList[OPTION_I_ZOOMNHANH].bEnable, m_ToggleItemList[OPTION_I_ZOOMCHAM].bEnable, m_ToggleItemList[OPTION_I_LAC].bEnable);' + R +
   T + T + 'break;' + R +
   '#endif',
   T + T + 'JxLia_DatNhay(m_ToggleItemList[OPTION_I_ZOOMNHANH].bEnable, m_ToggleItemList[OPTION_I_ZOOMCHAM].bEnable, m_ToggleItemList[OPTION_I_LAC].bEnable);' + R +
   T + T + 'break;' + R +
   T + 'case OPTION_I_CHUMZOOM:' + T + '// [ZOOMTHANH 14/09] ap ngay' + R +
   T + T + 'JxLia_DatChum(bEnable);' + R +
   T + T + 'break;' + R +
   '#endif',
   'case OPTION_I_CHUMZOOM:')

va(OC,
   T + T + 'JxLia_DatNhay(bOptionsEnable[OPTION_I_ZOOMNHANH], bOptionsEnable[OPTION_I_ZOOMCHAM], bOptionsEnable[OPTION_I_LAC]);' + T + '// [ZOOM3D 14/09]',
   T + T + 'JxLia_DatNhay(bOptionsEnable[OPTION_I_ZOOMNHANH], bOptionsEnable[OPTION_I_ZOOMCHAM], bOptionsEnable[OPTION_I_LAC]);' + T + '// [ZOOM3D 14/09]' + R +
   T + T + 'JxLia_DatChum(bOptionsEnable[OPTION_I_CHUMZOOM]);' + T + '// [ZOOMTHANH 14/09] mac dinh TAT (GetInteger mac dinh false)',
   'JxLia_DatChum(bOptionsEnable[OPTION_I_CHUMZOOM]);')

# ============================================================ D. UiShell: mo / dong thanh keo ============================================================
va(US,
   '#include "../Platform/JxLiaCanh.h"' + T + '// [CAMERA 13/09] chu bao muc zoom',
   '#include "../Platform/JxLiaCanh.h"' + T + '// [CAMERA 13/09] chu bao muc zoom' + R +
   '#include "UiCase/UiZoomThanh.h"' + T + '// [ZOOMTHANH 14/09] thanh keo zoom o mep man choi',
   '#include "UiCase/UiZoomThanh.h"')

va(US,
   T + 'KUiToolsControlBar::OpenWindow();' + R,
   T + 'KUiToolsControlBar::OpenWindow();' + R +
   '#ifdef JX_MOBILE' + R +
   T + 'KUiZoomThanh::OpenWindow();' + T + '// [ZOOMTHANH 14/09] thanh keo zoom (tu an khi map / nguoi choi tat zoom)' + R +
   '#endif' + R,
   'KUiZoomThanh::OpenWindow();')

va(US,
   T + T + 'KUiToolsControlBar::CloseWindow();' + R,
   T + T + 'KUiToolsControlBar::CloseWindow();' + R +
   '#ifdef JX_MOBILE' + R +
   T + T + 'KUiZoomThanh::CloseWindow();' + T + '// [ZOOMTHANH 14/09]' + R +
   '#endif' + R,
   'KUiZoomThanh::CloseWindow();')

# ============================================================ E. Trinh chinh giao dien ============================================================
va(TD,
   T + '{ "IconNpc", SUAGD_CO_KHONGPHONG }, { "ThongTinGoc", SUAGD_CO_KHONGPHONG },',
   T + '{ "IconNpc", SUAGD_CO_KHONGPHONG }, { "ThongTinGoc", SUAGD_CO_KHONGPHONG },' + R +
   T + '{ "KUiZoomThanh|Main", SUAGD_CO_KHONGPHONG },' + T + '// [ZOOMTHANH 14/09] thanh keo zoom',
   '"KUiZoomThanh|Main"')

# [b] UiToaDoMobile.inc duoc bien dich ca tren PC (kiem --pc HONG) -> rao JX_MOBILE quanh muc moi
va(TD,
   T + '{ "KUiZoomThanh|Main", SUAGD_CO_KHONGPHONG },' + T + '// [ZOOMTHANH 14/09] thanh keo zoom',
   '#ifdef JX_MOBILE' + R +
   T + '{ "KUiZoomThanh|Main", SUAGD_CO_KHONGPHONG },' + T + '// [ZOOMTHANH 14/09] thanh keo zoom' + R +
   '#endif',
   '#ifdef JX_MOBILE' + R + T + '{ "KUiZoomThanh|Main"')

# [b] UiZoomThanh.cpp: KIniFile la kieu chua day du -> them include (nhu UiToolsControlBar.cpp)
va(ZC,
   '#include "../Elem/WndMessage.h"' + R,
   '#include "../Elem/WndMessage.h"' + R +
   '#include "KIniFile.h"' + R,
   '#include "KIniFile.h"')

# ============================================================ H. KSdlApp: vuot doc tren thanh keo = keo nut (khong cuon), bam lien tiep = khong nhay dup ============================================================
KS = os.path.join(GOC, 'Sources', 'S3Client', 'Platform', 'KSdlApp.cpp')

va(ZH,
   T + 'static KUiZoomThanh*' + T + 'GetSelf() { return m_pSelf; }',
   T + 'static KUiZoomThanh*' + T + 'GetSelf() { return m_pSelf; }' + R +
   T + 'static int' + T + T + T + 'TaiDiem(int x, int y);' + T + '// 1 = diem (toa do man) nam tren thanh dang hien (KSdlApp: vuot doc o day = keo nut, khong cuon)',
   'TaiDiem(int x, int y);')

va(ZC,
   '#endif' + T + '// JX_MOBILE' + R,
   'int KUiZoomThanh::TaiDiem(int x, int y)' + R +
   '{' + R +
   T + 'return (m_pSelf && m_pSelf->IsVisible() && m_pSelf->PtInWindow(x, y)) ? 1 : 0;' + T + '// PtInWindow tra 0 khi thanh dang an' + R +
   '}' + R +
   R +
   '// [ZOOMTHANH 14/09] KSdlApp.cpp: vuot DOC tren giao dien mac dinh la CUON danh sach; tren thanh keo zoom phai la KEO nut' + R +
   'extern "C" int JxZoomThanh_TaiDiem(int x, int y)' + R +
   '{' + R +
   T + 'return KUiZoomThanh::TaiDiem(x, y);' + R +
   '}' + R +
   '#endif' + T + '// JX_MOBILE' + R,
   'extern "C" int JxZoomThanh_TaiDiem(int x, int y)')

va(KS,
   'extern "C" int JxUi_CoGiaoDienTaiDiem(int x, int y);' + T + '// Wnds.cpp' + R,
   'extern "C" int JxUi_CoGiaoDienTaiDiem(int x, int y);' + T + '// Wnds.cpp' + R +
   'extern "C" int JxZoomThanh_TaiDiem(int x, int y);' + T + '// [ZOOMTHANH 14/09] UiZoomThanh.cpp: diem nam tren thanh keo zoom dang hien' + R,
   'extern "C" int JxZoomThanh_TaiDiem(int x, int y);')

va(KS,
   T + T + T + T + 'abs(m_nChamY - m_nChamY0) > abs(m_nChamX - m_nChamX0))' + R,
   T + T + T + T + 'abs(m_nChamY - m_nChamY0) > abs(m_nChamX - m_nChamX0) && !JxZoomThanh_TaiDiem(m_nChamX0, m_nChamY0))' + T + '// [ZOOMTHANH 14/09] vuot doc tren thanh keo zoom = keo nut (roi xuong CHAM_KEO), khong cuon' + R,
   '[ZOOMTHANH 14/09] vuot doc tren thanh keo zoom = keo nut')

va(KS,
   T + T + T + T + '(abs(m_nChamY0 - m_nChamNhaY) <= CHAM_HAI_XA);' + R,
   T + T + T + T + '(abs(m_nChamY0 - m_nChamNhaY) <= CHAM_HAI_XA) && !JxZoomThanh_TaiDiem(m_nChamX0, m_nChamY0);' + T + '// [ZOOMTHANH 14/09] bam lien tiep vao thanh keo zoom = tung nhay rieng (KWndScrollBar khong nhan DBLCLK)' + R,
   '[ZOOMTHANH 14/09] bam lien tiep vao thanh keo zoom')

# [c] soi cheo phien do nhip: Scene/KWeather.cpp:426/432 cung ve RU_T_SHADOW toa do the gioi ca LO hat mua / tuyet -> chi xep khi 1 hat (KNpc luon goi 1)
va(RC,
   'if (uGenre == RU_T_SHADOW && !bSinglePlaneCoord && TgChuXep()) { for (i = 0; i < nPrimitiveCount; i++) TgChuThem(3, 0, NULL, 0, 0, 0, 0, 0, 0, 0, NULL, ((KRUShadow*)pPrimitives) + i); return; }' + T + '// [CHUNET 14/09] nen mo sau ten + thanh mau (KNpc VeNenChu / PaintInfo): ve sau blit cung chu',
   'if (uGenre == RU_T_SHADOW && !bSinglePlaneCoord && nPrimitiveCount == 1 && TgChuXep()) { TgChuThem(3, 0, NULL, 0, 0, 0, 0, 0, 0, 0, NULL, (KRUShadow*)pPrimitives); return; }' + T + '// [CHUNET 14/09] nen mo sau ten + thanh mau (KNpc VeNenChu / PaintInfo, luon 1 hat): ve sau blit cung chu; KWeather ve ca lo hat mua / tuyet -> khong xep (soi cheo)',
   'nPrimitiveCount == 1 && TgChuXep()')

# ============================================================ F. CMake (LF) ============================================================
va(CMA,
   '    ${JX_SRC}/S3Client/Ui/UiCase/UiVatPham.cpp)        # [VATPHAM 12/09]',
   '    ${JX_SRC}/S3Client/Ui/UiCase/UiZoomThanh.cpp      # [ZOOMTHANH 14/09] thanh keo zoom o mep man choi (KUiZoomThanh)\n' +
   '    ${JX_SRC}/S3Client/Ui/UiCase/UiVatPham.cpp)        # [VATPHAM 12/09]',
   'UiZoomThanh.cpp')

va(CMI,
   '    ${JX_SRC}/S3Client/Ui/UiCase/UiVatPham.cpp       # bang thong tin vat pham khi cham o do',
   '    ${JX_SRC}/S3Client/Ui/UiCase/UiZoomThanh.cpp     # [ZOOMTHANH 14/09] thanh keo zoom o mep man choi\n' +
   '    ${JX_SRC}/S3Client/Ui/UiCase/UiVatPham.cpp       # bang thong tin vat pham khi cham o do',
   'UiZoomThanh.cpp')

# ============================================================ G. Du lieu: danh sach trinh chinh + config ============================================================
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

for g in [os.path.join(GOC, 'android', 'du_lieu_ghi_de'), r'D:\jx1_android_data', r'D:\jx1_android_data_dt_v4']:
    them_dong(os.path.join(g, 'ui', 'uitoado_danhsach.ini'), 'KUiZoomThanh|Main=Thanh zoom,1\n', 'KUiZoomThanh|Main=')
    them_sau(os.path.join(g, 'config.ini'), 'TheGioiRTSan=1',
             '; [CHUNET 14/09] 1 = chu the gioi (ten, chat tren dau, ten vat roi) + nen mo sau ten + thanh mau ve SAU blit khi nhin rong / phong to / lac\n'
             ';   -> chu sac net, co khong doi theo zoom (nhu nhan dau game 3D); 0 = ve vao anh RT roi thu nho / phong to cung anh (mo khi nhin rong).\n'
             'TheGioiRTChu=1\n', 'TheGioiRTChu=')

for k in KQ:
    print(k)
