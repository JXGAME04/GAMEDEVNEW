# -*- coding: latin-1 -*-
# [ZOOM 13/09] Va nguon cho ZOOM NHIN RONG (buoc 2 cua lia canh): chum hai ngon tren ban do = the gioi ve vao RT to hon khung roi
# thu nho (cach C, tai dung duong [TG] Rep3_JxTheGioi), giao dien giu nguyen. Idempotent, latin-1, neo CRLF (nguon) / LF (ini).
# Dung: python android/va_nguon_zoom_1309.py [goc worktree] [--config <config.ini> ...]
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

REP = os.path.join(GOC, 'Sources', 'Represent', 'Represent3')
S3 = os.path.join(GOC, 'Sources', 'S3Client')

# ================= A. KRepresentShell3.h: thanh vien zoom (trong khoi JX_MOBILE cua [TG]) =================
va(os.path.join(REP, 'KRepresentShell3.h'),
   '    DWORD m_dwTgMauXoa;                  // mau xoa cua RepresentBegin khung nay' + R + '#endif' + R,
   '    DWORD m_dwTgMauXoa;                  // mau xoa cua RepresentBegin khung nay' + R +
   '    // [ZOOM 13/09] nhin rong ra: zoom phan nghin (1000 = tat). RT = khung x zoom; goc RT lui (m_nZoomDx/Dy) de tieu diem van o giua.' + R +
   '    // Luc ve RT: g_nScreenWidth/Height + m_nLeft/m_nTop tam doi sang RT (cull/cat theo RT), lenh 2 tra lai (m_nTgKhung*, m_nTg*Khung).' + R +
   '    // Cua toa do: CoordinateTransform (the gioi -> khung, chia zoom) va ViewPortCoordToSpaceCoord (khung -> the gioi, nhan zoom).' + R +
   '    int   m_nTgZoom;' + R +
   '    int   m_nZoomDx, m_nZoomDy;' + R +
   '    int   m_nTgKhungW, m_nTgKhungH;' + R +
   '    int   m_nTgLeftKhung, m_nTgTopKhung;' + R +
   '#endif' + R,
   'm_nTgZoom;')

# ================= B. KRepresentShell3.cpp =================
RC = os.path.join(REP, 'KRepresentShell3.cpp')
# B1 khoi tao
va(RC,
   '\tm_pTgTex = NULL; m_pTgSurf = NULL; m_pTgSurfCu = NULL; m_pTgSB = NULL; m_nTgW = m_nTgH = 0; m_nTgTrangThai = 0; m_nTgLeft = m_nTgTop = 0; m_dwTgMauXoa = 0xff000000;\t// [TG 13/09]' + R,
   '\tm_pTgTex = NULL; m_pTgSurf = NULL; m_pTgSurfCu = NULL; m_pTgSB = NULL; m_nTgW = m_nTgH = 0; m_nTgTrangThai = 0; m_nTgLeft = m_nTgTop = 0; m_dwTgMauXoa = 0xff000000;\t// [TG 13/09]' + R +
   '\tm_nTgZoom = 1000; m_nZoomDx = m_nZoomDy = 0; m_nTgKhungW = m_nTgKhungH = 0; m_nTgLeftKhung = m_nTgTopKhung = 0;\t// [ZOOM 13/09]' + R,
   'm_nTgZoom = 1000;')
# B2 lenh 4/5
va(RC,
   '\tif (nLenh == 3) { g_nJxTheGioiEp = 1; return 0; }' + R,
   '\tif (nLenh == 3) { g_nJxTheGioiEp = 1; return 0; }' + R +
   '\tif (nLenh == 4)' + R +
   '\t{\t// [ZOOM 13/09] dat zoom phan nghin (1000 = tat, toi da 3000) - S3Client (JxLiaCanh) goi khi chum hai ngon; doi thi khung sau ve that' + R +
   '\t\tint nZ = nThamSo; if (nZ < 1000) nZ = 1000; if (nZ > 3000) nZ = 3000;' + R +
   '\t\tif (nZ != m_nTgZoom) { Rep3Log("[ZOOM] zoom %d -> %d (phan nghin)", m_nTgZoom, nZ); m_nTgZoom = nZ; if (nZ == 1000) { m_nZoomDx = m_nZoomDy = 0; } g_nJxTheGioiEp = 1; }' + R +
   '\t\treturn m_nTgZoom;' + R +
   '\t}' + R +
   '\tif (nLenh == 5) return m_nTgZoom;\t// [ZOOM 13/09] hoi zoom hien tai' + R,
   'if (nLenh == 4)')
# B3 lenh 0: zoom -> luon ve RT (K=1), RT = khung x zoom
va(RC,
   '\t\tif (!s_nTgBat || (nK != 2 && s_nTgEp != 1)) { s_uTgVeThat = s_uTgKhung; s_uTgDemThuong++; return 0; }' + R +
   '\t\tif (!m_pTgTex || m_nTgW != g_nScreenWidth || m_nTgH != g_nScreenHeight)' + R +
   '\t\t{\t// RT phai dung co khung logic (gap / mo, doi ho khung)' + R +
   '\t\t\tJxTheGioiHuy();' + R +
   '\t\t\tm_nTgW = g_nScreenWidth; m_nTgH = g_nScreenHeight;' + R,
   '\t\tconst bool bZoom = (m_nTgZoom > 1000);\t// [ZOOM 13/09] dang nhin rong: luon ve the gioi vao RT to hon khung roi thu nho (K = 1)' + R +
   '\t\tif (bZoom) nK = 1;' + R +
   '\t\tif (!bZoom && (!s_nTgBat || (nK != 2 && s_nTgEp != 1))) { s_uTgVeThat = s_uTgKhung; s_uTgDemThuong++; return 0; }' + R +
   '\t\tconst int nRtW = bZoom ? ((g_nScreenWidth * m_nTgZoom / 1000 + 1) & ~1) : g_nScreenWidth;' + R +
   '\t\tconst int nRtH = bZoom ? ((g_nScreenHeight * m_nTgZoom / 1000 + 1) & ~1) : g_nScreenHeight;' + R +
   '\t\tif (!m_pTgTex || m_nTgW != nRtW || m_nTgH != nRtH)' + R +
   '\t\t{\t// RT phai dung co khung logic (gap / mo, doi ho khung); [ZOOM 13/09] hoac khung x zoom' + R +
   '\t\t\tJxTheGioiHuy();' + R +
   '\t\t\tm_nTgW = nRtW; m_nTgH = nRtH;' + R,
   'const bool bZoom = (m_nTgZoom > 1000);')
# B4 lenh 1: goc + co khung tam doi sang RT
va(RC,
   '\t\tm_nTgTrangThai = 1; m_nTgLeft = m_nLeft; m_nTgTop = m_nTop;' + R,
   '\t\tm_nTgLeftKhung = m_nLeft; m_nTgTopKhung = m_nTop; m_nTgKhungW = g_nScreenWidth; m_nTgKhungH = g_nScreenHeight;\t// [ZOOM 13/09] goc + co khung that' + R +
   '\t\tif (m_nTgZoom > 1000)' + R +
   '\t\t{\t// [ZOOM 13/09] RT to hon khung: goc RT lui de tieu diem van o giua; cull / cat trong shell theo co RT trong luc ve' + R +
   '\t\t\tm_nZoomDx = (m_nTgW - g_nScreenWidth) / 2; m_nZoomDy = (m_nTgH - g_nScreenHeight) / 2;' + R +
   '\t\t\tm_nLeft -= m_nZoomDx; m_nTop -= m_nZoomDy; g_nScreenWidth = m_nTgW; g_nScreenHeight = m_nTgH;' + R +
   '\t\t}' + R +
   '\t\telse { m_nZoomDx = m_nZoomDy = 0; }' + R +
   '\t\tm_nTgTrangThai = 1; m_nTgLeft = m_nLeft; m_nTgTop = m_nTop;' + R,
   'm_nTgLeftKhung = m_nLeft;')
# B5 lenh 2: tra lai co khung + goc khung
va(RC,
   '\t\t\tPD3DDEVICE->SetRenderTarget(0, m_pTgSurfCu); SAFE_RELEASE(m_pTgSurfCu); m_nTgTrangThai = 0;' + R,
   '\t\t\tPD3DDEVICE->SetRenderTarget(0, m_pTgSurfCu); SAFE_RELEASE(m_pTgSurfCu); m_nTgTrangThai = 0;' + R +
   '\t\t\tif (m_nTgZoom > 1000) { g_nScreenWidth = m_nTgKhungW; g_nScreenHeight = m_nTgKhungH; m_nLeft = m_nTgLeftKhung; m_nTop = m_nTgTopKhung; }\t// [ZOOM 13/09] tra co khung + goc khung' + R,
   '[ZOOM 13/09] tra co khung')
# B6 blit: loc tuyen tinh khi thu nho
va(RC,
   '\t\tPD3DDEVICE->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT); PD3DDEVICE->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);' + R,
   '\t\t{ const DWORD dwLoc = (m_nTgZoom > 1000) ? D3DTEXF_LINEAR : D3DTEXF_POINT; PD3DDEVICE->SetSamplerState(0, D3DSAMP_MINFILTER, dwLoc); PD3DDEVICE->SetSamplerState(0, D3DSAMP_MAGFILTER, dwLoc); }\t// [ZOOM 13/09] thu nho thi loc tuyen tinh' + R,
   'const DWORD dwLoc')
# B6b blit: dich = khung (khong phai co RT) - RT to hon khung thi thu nho
va(RC,
   '		const float fW = (float)m_nTgW, fH = (float)m_nTgH;' + R,
   '		const float fW = (float)g_nScreenWidth, fH = (float)g_nScreenHeight;	// [ZOOM 13/09] dich blit = KHUNG (da tra lai o tren): RT to hon khung thi thu nho; khong zoom thi bang m_nTgW/H nhu cu' + R,
   '[ZOOM 13/09] dich blit = KHUNG')
# B7 CoordinateTransform + CoordinateTransformX (2D): chia zoom o khung (sau blit)
va(RC,
   '\tif (g_renderModel == RenderModel2D)\t\t// [REP3 03/09] cong thuc Represent2' + R +
   '\t{' + R +
   '\t\tnX = nX - m_nLeft;' + R +
   '\t\tnY = nY / 2 - m_nTop - ((nZ * 887) >> 10);' + R +
   '\t\treturn;' + R +
   '\t}' + R,
   '\tif (g_renderModel == RenderModel2D)\t\t// [REP3 03/09] cong thuc Represent2' + R +
   '\t{' + R +
   '\t\tnX = nX - m_nLeft;' + R +
   '\t\tnY = nY / 2 - m_nTop - ((nZ * 887) >> 10);' + R +
   '#ifdef JX_MOBILE' + R +
   '\t\tif (m_nTgZoom > 1000 && m_nTgTrangThai == 0)' + R +
   '\t\t{\t// [ZOOM 13/09] ve len khung (sau blit): diem anh RT = (diem - goc khung) + lui goc RT, roi thu nho theo zoom' + R +
   '\t\t\tnX = (nX + m_nZoomDx) * 1000 / m_nTgZoom;' + R +
   '\t\t\tnY = (nY + m_nZoomDy) * 1000 / m_nTgZoom;' + R +
   '\t\t}' + R +
   '#endif' + R +
   '\t\treturn;' + R +
   '\t}' + R,
   '[ZOOM 13/09] ve len khung (sau blit)', so_lan=2)
# B8 ViewPortCoordToSpaceCoord (2D): nhan zoom
va(RC,
   '\t\t// Legacy 2D logic (if you\'re using map rendering or UI mode)' + R +
   '\t\tnX = nX + m_nLeft;' + R +
   '\t\tnY = (nY + m_nTop + ((nZ * 887) >> 10)) * 2;' + R,
   '\t\t// Legacy 2D logic (if you\'re using map rendering or UI mode)' + R +
   '#ifdef JX_MOBILE' + R +
   '\t\tif (m_nTgZoom > 1000)' + R +
   '\t\t{\t// [ZOOM 13/09] cham tren khung -> diem anh RT (nhan zoom, bo lui goc RT) -> the gioi theo goc KHUNG (m_nLeft dang lui neu dang ve RT)' + R +
   '\t\t\tconst int nGocX = (m_nTgTrangThai == 1) ? m_nTgLeftKhung : m_nLeft;' + R +
   '\t\t\tconst int nGocY = (m_nTgTrangThai == 1) ? m_nTgTopKhung : m_nTop;' + R +
   '\t\t\tnX = nX * m_nTgZoom / 1000 - m_nZoomDx + nGocX;' + R +
   '\t\t\tnY = (nY * m_nTgZoom / 1000 - m_nZoomDy + nGocY + ((nZ * 887) >> 10)) * 2;' + R +
   '\t\t\treturn;' + R +
   '\t\t}' + R +
   '#endif' + R +
   '\t\tnX = nX + m_nLeft;' + R +
   '\t\tnY = (nY + m_nTop + ((nZ * 887) >> 10)) * 2;' + R,
   '[ZOOM 13/09] cham tren khung')

# ================= C. KSdlApp.h / .cpp: chum hai ngon =================
va(os.path.join(S3, 'Platform', 'KSdlApp.h'),
   '\tint\t\t\t\tm_nChum;\t\t\t// 1 = dang chum' + R,
   '\tint\t\t\t\tm_nChum;\t\t\t// 1 = dang chum' + R +
   '\tint\t\t\t\tm_nLiaChum;\t\t\t// [ZOOM 13/09] 1 = dang chum hai ngon tren BAN DO (zoom nhin rong + lia theo tam hai ngon, JxLia_Chum*)' + R,
   'm_nLiaChum;')
KS = os.path.join(S3, 'Platform', 'KSdlApp.cpp')
va(KS,
   '\tm_nChum = 0;' + R + '#endif' + R + '\ts_pSdlApp = this;' + R,
   '\tm_nChum = 0;' + R + '\tm_nLiaChum = 0;\t// [ZOOM 13/09]' + R + '#endif' + R + '\ts_pSdlApp = this;' + R,
   'm_nLiaChum = 0;')
va(KS,
   '\t\t\tif (bNgonDau)' + R +
   '\t\t\t\treturn false;\t// ngon thu nhat: de chuot gia lap lo nhu cu' + R,
   '\t\t\tif (bNgonDau)' + R +
   '\t\t\t{' + R +
   '\t\t\t\tm_nNgon1 = nNgon; m_nNgon1X = (int)fx; m_nNgon1Y = (int)fy;\t// [ZOOM 13/09] nho ngon 1 de chum hai ngon ngoai che do sua giao dien' + R +
   '\t\t\t\treturn false;\t// ngon thu nhat: de chuot gia lap lo nhu cu' + R +
   '\t\t\t}' + R,
   '[ZOOM 13/09] nho ngon 1')
va(KS,
   '\t\t\t\tJxCan_BatDau((int)fx, (int)fy, (int)fx, (int)fy);' + R +
   '\t\t\t\treturn true;' + R +
   '\t\t\t}' + R +
   '\t\t\treturn false;' + R +
   '\t\t}' + R,
   '\t\t\t\tJxCan_BatDau((int)fx, (int)fy, (int)fx, (int)fy);' + R +
   '\t\t\t\treturn true;' + R +
   '\t\t\t}' + R +
   '\t\t\t// [ZOOM 13/09] Ngon 2 dat len BAN DO khi ngon 1 cung dang tren ban do (cho, dang lia, dang keo) = CHUM: zoom nhin rong' + R +
   '\t\t\t// (hai ngon gan nhau = thay rong hon, xa nhau = ve 1:1) + lia theo tam hai ngon. Ngon 1 bo duong chuot gia lap (nhu CHUM sua giao dien).' + R +
   '\t\t\tif (!m_nLiaChum && m_nNgon1 >= 0 && m_nNgonDangDat == 2' + R +
   '\t\t\t\t&& (m_nCham == CHAM_CHO || m_nCham == CHAM_LIA || m_nCham == CHAM_KEO)' + R +
   '\t\t\t\t&& JxLia_ChumDuoc(m_nNgon1X, m_nNgon1Y, (int)fx, (int)fy))' + R +
   '\t\t\t{' + R +
   '\t\t\t\tif (m_nCham == CHAM_KEO)' + R +
   '\t\t\t\t{' + R +
   '\t\t\t\t\tGhiChuot(0, MAKELPARAM(m_nChamX, m_nChamY));' + R +
   '\t\t\t\t\tMsgProc(hWnd, WM_LBUTTONUP, 0, MAKELPARAM(m_nChamX, m_nChamY));' + R +
   '\t\t\t\t}' + R +
   '\t\t\t\tm_nCham = CHAM_KHONG;' + R +
   '\t\t\t\tm_nNgon2 = nNgon; m_nNgon2X = (int)fx; m_nNgon2Y = (int)fy;' + R +
   '\t\t\t\tm_nLiaChum = 1;' + R +
   '\t\t\t\tJxLia_ChumBatDau(m_nNgon1X, m_nNgon1Y, m_nNgon2X, m_nNgon2Y);' + R +
   '\t\t\t\treturn true;' + R +
   '\t\t\t}' + R +
   '\t\t\treturn false;' + R +
   '\t\t}' + R,
   'JxLia_ChumBatDau(m_nNgon1X')
va(KS,
   '\t\t\tif (nNgon == m_nNgonKyNang)' + R +
   '\t\t\t{' + R +
   '\t\t\t\tJxKyNang_Keo((int)fx, (int)fy);' + R,
   '\t\t\tif (m_nLiaChum)\t// [ZOOM 13/09] dang chum hai ngon tren ban do' + R +
   '\t\t\t{' + R +
   '\t\t\t\tif (nNgon == m_nNgon1)\t\t{ m_nNgon1X = (int)fx; m_nNgon1Y = (int)fy; }' + R +
   '\t\t\t\telse if (nNgon == m_nNgon2)\t{ m_nNgon2X = (int)fx; m_nNgon2Y = (int)fy; }' + R +
   '\t\t\t\telse return false;' + R +
   '\t\t\t\tJxLia_ChumKeo(m_nNgon1X, m_nNgon1Y, m_nNgon2X, m_nNgon2Y);' + R +
   '\t\t\t\treturn true;' + R +
   '\t\t\t}' + R +
   '\t\t\tif (nNgon == m_nNgon1) { m_nNgon1X = (int)fx; m_nNgon1Y = (int)fy; }\t// [ZOOM 13/09] theo doi ngon 1 (chuot gia lap van lo)' + R +
   '\t\t\tif (nNgon == m_nNgonKyNang)' + R +
   '\t\t\t{' + R +
   '\t\t\t\tJxKyNang_Keo((int)fx, (int)fy);' + R,
   'JxLia_ChumKeo(m_nNgon1X')
va(KS,
   '\t\tif (nNgon == m_nNgonKyNang)' + R +
   '\t\t{' + R +
   '\t\t\tJxKyNang_Nha();' + R,
   '\t\tif (m_nLiaChum && (nNgon == m_nNgon1 || nNgon == m_nNgon2))\t// [ZOOM 13/09] mot trong hai ngon nha = het chum (ngon con lai khong lam gi toi khi nha)' + R +
   '\t\t{' + R +
   '\t\t\tJxLia_ChumNha();' + R +
   '\t\t\tm_nLiaChum = 0;' + R +
   '\t\t\tm_nNgon1 = m_nNgon2 = -1;' + R +
   '\t\t\tm_nCham = CHAM_KHONG;' + R +
   '\t\t\treturn true;' + R +
   '\t\t}' + R +
   '\t\tif (nNgon == m_nNgon1) m_nNgon1 = -1;\t// [ZOOM 13/09]' + R +
   '\t\tif (nNgon == m_nNgonKyNang)' + R +
   '\t\t{' + R +
   '\t\t\tJxKyNang_Nha();' + R,
   'JxLia_ChumNha();')

# ================= D. JxLiaCanh.h / .cpp: zoom + chum =================
va(os.path.join(S3, 'Platform', 'JxLiaCanh.h'),
   '// 1 = dang lech (dang keo, dang cho ve hoac dang ve).' + R + 'int  JxLia_DangLia();' + R,
   '// 1 = dang lech (dang keo, dang cho ve hoac dang ve).' + R + 'int  JxLia_DangLia();' + R + R +
   '// [ZOOM 13/09] Chum hai ngon tren ban do = ZOOM NHIN RONG (the gioi ve vao RT to hon khung roi thu nho - Represent3 Rep3_JxTheGioi' + R +
   '// lenh 4/5, giao dien giu nguyen co) + lia theo tam hai ngon. Hai ngon gan nhau = thay rong hon, xa nhau = ve 1:1. Zoom giu toi khi doi.' + R +
   'bool JxLia_ChumDuoc(int x1, int y1, int x2, int y2);\t// [Cham] ZoomCanh=1 va ca hai diem deu duoc lia' + R +
   'void JxLia_ChumBatDau(int x1, int y1, int x2, int y2);' + R +
   'void JxLia_ChumKeo(int x1, int y1, int x2, int y2);' + R +
   'void JxLia_ChumNha();' + R +
   'int  JxLia_ZoomLay();\t\t\t\t// % (100 = 1:1)' + R +
   'void JxLia_ZoomDat(int nPhanTram);\t// dat zoom % (100..ZoomToiDa, lam tron ZoomBuoc): Represent3 + vung truy van vat the (SetRepresentAreaSize)' + R,
   'JxLia_ChumDuoc')

LC = os.path.join(S3, 'Platform', 'JxLiaCanh.cpp')
va(LC,
   'static int\ts_nThuDx = 0, s_nThuDy = 0;\t// LiaThu=dx,dy: GO LOI - 3 s sau khi vao the gioi tu keo (dx,dy) roi tu ve' + R,
   'static int\ts_nThuDx = 0, s_nThuDy = 0;\t// LiaThu=dx,dy: GO LOI - 3 s sau khi vao the gioi tu keo (dx,dy) roi tu ve' + R +
   '// [ZOOM 13/09]' + R +
   'static int\ts_nZoomBat = 1;\t\t// ZoomCanh: 0 = tat chum hai ngon' + R +
   'static int\ts_nZoomToiDa = 150;\t// ZoomToiDa: % nhin rong toi da (150 = rong gap 1,5 lan moi chieu)' + R +
   'static int\ts_nZoomBuoc = 5;\t// ZoomBuoc: nac zoom, %' + R +
   'static int\ts_nZoomMacDinh = 100;\t// ZoomMacDinh: zoom luc vao game, %' + R +
   'static int\ts_nZoomThu = 0;\t\t// ZoomThu: GO LOI - % zoom tu dat 3 s sau khi vao the gioi (may ao khong co hai ngon)' + R,
   's_nZoomBat')
va(LC,
   'static unsigned int\ts_uKeoDem = 0;\t\t// thong ke: so lan keo trong dot nay' + R,
   'static unsigned int\ts_uKeoDem = 0;\t\t// thong ke: so lan keo trong dot nay' + R +
   '// [ZOOM 13/09]' + R +
   'static int\t\t\ts_nZoom = 100;\t\t// % dang ap (100 = 1:1)' + R +
   'static int\t\t\ts_nZoomDaAp = -1;\t// % da gui cho Represent3 / Core (-1 = chua)' + R +
   'static int\t\t\ts_nChum = 0;\t\t// 1 = dang chum' + R +
   'static float\t\ts_fChumD0 = 1.f;\t// khoang cach hai ngon luc bat dau chum' + R +
   'static int\t\t\ts_nChumZoom0 = 100;\t// zoom luc bat dau chum' + R +
   'static int\t\t\ts_nZoomThuXong = 0;' + R,
   's_nZoomDaAp')
va(LC,
   '\tGetPrivateProfileString("Cham", "LiaThu", szThu, szThu, sizeof(szThu), szCfg);' + R +
   '\tsscanf(szThu, "%d,%d", &s_nThuDx, &s_nThuDy);' + R,
   '\tGetPrivateProfileString("Cham", "LiaThu", szThu, szThu, sizeof(szThu), szCfg);' + R +
   '\tsscanf(szThu, "%d,%d", &s_nThuDx, &s_nThuDy);' + R +
   '\ts_nZoomBat     = GetPrivateProfileInt("Cham", "ZoomCanh", 1, szCfg);\t// [ZOOM 13/09]' + R +
   '\ts_nZoomToiDa   = GetPrivateProfileInt("Cham", "ZoomToiDa", 150, szCfg);' + R +
   '\ts_nZoomBuoc    = GetPrivateProfileInt("Cham", "ZoomBuoc", 5, szCfg);' + R +
   '\ts_nZoomMacDinh = GetPrivateProfileInt("Cham", "ZoomMacDinh", 100, szCfg);' + R +
   '\ts_nZoomThu     = GetPrivateProfileInt("Cham", "ZoomThu", 0, szCfg);' + R +
   '\tif (s_nZoomToiDa < 100) s_nZoomToiDa = 100;   if (s_nZoomToiDa > 300) s_nZoomToiDa = 300;' + R +
   '\tif (s_nZoomBuoc < 1)    s_nZoomBuoc = 1;      if (s_nZoomBuoc > 50)   s_nZoomBuoc = 50;' + R +
   '\tif (s_nZoomMacDinh < 100) s_nZoomMacDinh = 100; if (s_nZoomMacDinh > s_nZoomToiDa) s_nZoomMacDinh = s_nZoomToiDa;' + R +
   '\ts_nZoom = s_nZoomMacDinh;' + R,
   '"ZoomCanh"')
va(LC,
   '\tg_DebugLog("[LIA] lia canh: bat=%d xa=%d%%x%d%% nguong=%d cho ve=%d ms toc do=%d px/s em=%d ve khi di=%d thu=%d,%d",' + R +
   '\t\ts_nBat, s_nXaNgang, s_nXaDoc, s_nNguong, s_nChoVeMs, s_nVeTocDo, s_nVeEm, s_nVeKhiDi, s_nThuDx, s_nThuDy);' + R,
   '\tg_DebugLog("[LIA] lia canh: bat=%d xa=%d%%x%d%% nguong=%d cho ve=%d ms toc do=%d px/s em=%d ve khi di=%d thu=%d,%d",' + R +
   '\t\ts_nBat, s_nXaNgang, s_nXaDoc, s_nNguong, s_nChoVeMs, s_nVeTocDo, s_nVeEm, s_nVeKhiDi, s_nThuDx, s_nThuDy);' + R +
   '\tg_DebugLog("[ZOOM] chum hai ngon: bat=%d toi da=%d%% buoc=%d%% mac dinh=%d%% thu=%d%%", s_nZoomBat, s_nZoomToiDa, s_nZoomBuoc, s_nZoomMacDinh, s_nZoomThu);' + R,
   '[ZOOM] chum hai ngon: bat')
# Rep3 qua GetProcAddress + kep lech theo zoom
va(LC,
   '// Dang trong the gioi (co thanh cong cu) - o menu / dang nhap / chon nhan vat thi khong lia.' + R,
   '// [ZOOM 13/09] Represent3 (libRepresent3.so / Represent3.dll) qua GetProcAddress nhu Wnds.cpp JxUi_TheGioi: libmain.so khong link Represent3;' + R +
   '// iOS tra bang ky hieu tinh (Rep3_JxTheGioi da dang ky trong ios/JxIosMain.cpp). Khong co ham (ban cu) -> 0.' + R +
   'typedef int (*PFN_Rep3JxTheGioi)(int, int);' + R +
   'static int Rep3TheGioi(int nLenh, int nThamSo)' + R +
   '{' + R +
   '\tstatic PFN_Rep3JxTheGioi s_pfn = NULL; static int s_nThu = 0;' + R +
   '\tif (!s_pfn && s_nThu < 8) { s_nThu++; HMODULE h = GetModuleHandleA("Represent3.dll"); if (h) s_pfn = (PFN_Rep3JxTheGioi)GetProcAddress(h, "Rep3_JxTheGioi"); }' + R +
   '\treturn s_pfn ? s_pfn(nLenh, nThamSo) : 0;' + R +
   '}' + R + R +
   '// Dang trong the gioi (co thanh cong cu) - o menu / dang nhap / chon nhan vat thi khong lia.' + R,
   'PFN_Rep3JxTheGioi')
va(LC,
   '\tfloat fMaxX = (float)(SCREEN_WIDTH * s_nXaNgang / 100);' + R +
   '\tfloat fMaxY = (float)(SCREEN_HEIGHT * s_nXaDoc / 100 * 2);\t// Y the gioi = 2 x px' + R,
   '\tfloat fMaxX = (float)(SCREEN_WIDTH * s_nXaNgang / 100) * (float)s_nZoom / 100.f;\t// [ZOOM 13/09] nhin rong thi lia xa hon theo ti le' + R +
   '\tfloat fMaxY = (float)(SCREEN_HEIGHT * s_nXaDoc / 100 * 2) * (float)s_nZoom / 100.f;\t// Y the gioi = 2 x px' + R,
   '[ZOOM 13/09] nhin rong thi lia xa hon')
# ZoomThu + ap lai zoom sau khi vao the gioi (SetRepresentAreaSize) trong JxLia_Nhip
va(LC,
   '\t// LiaThu (go loi, thu tren may ao khong co ngon tay): 3 s sau khi vao the gioi tu keo mot lan roi tu ve' + R,
   '\t// [ZOOM 13/09] zoom mac dinh / ZoomThu (go loi): ap sau khi vao the gioi 3 s (Represent3 va Core da san sang)' + R +
   '\tif (!s_uVaoGameLuc)' + R +
   '\t\ts_uVaoGameLuc = uNay;' + R +
   '\telse if (uNay - s_uVaoGameLuc >= 3000 && !s_nZoomThuXong)' + R +
   '\t{' + R +
   '\t\ts_nZoomThuXong = 1;' + R +
   '\t\tif (s_nZoomThu > 100) { g_DebugLog("[ZOOM] ZoomThu: tu dat %d%%", s_nZoomThu); JxLia_ZoomDat(s_nZoomThu); }' + R +
   '\t\telse if (s_nZoom != 100 || s_nZoomDaAp != s_nZoom) JxLia_ZoomDat(s_nZoom);' + R +
   '\t}' + R +
   '\t// LiaThu (go loi, thu tren may ao khong co ngon tay): 3 s sau khi vao the gioi tu keo mot lan roi tu ve' + R,
   'ZoomThu: tu dat')
# JxLia_DatLai: ap lai vung truy van sau doi map
va(LC,
   'void JxLia_DatLai()' + R + '{' + R +
   '\tif (s_nTrangThai != LIA_KHONG || s_nDaBatCo)' + R +
   '\t\tKetThuc("dat lai");' + R +
   '\ts_nCoGoc = 0;' + R +
   '\ts_uVaoGameLuc = 0;' + R + '}' + R,
   'void JxLia_DatLai()' + R + '{' + R +
   '\tif (s_nTrangThai != LIA_KHONG || s_nDaBatCo)' + R +
   '\t\tKetThuc("dat lai");' + R +
   '\ts_nCoGoc = 0;' + R +
   '\ts_uVaoGameLuc = 0;' + R +
   '\ts_nChum = 0;' + R +
   '\ts_nZoomThuXong = 0; s_nZoomDaAp = -1;\t// [ZOOM 13/09] doi map: 3 s sau ap lai zoom (vung truy van vat the) - Represent3 van giu zoom' + R +
   '}' + R,
   's_nZoomThuXong = 0; s_nZoomDaAp = -1;')
# cac ham zoom / chum (them cuoi tep, truoc #endif)
va(LC,
   '#endif // JX_MOBILE' + R,
   '// ================= [ZOOM 13/09] zoom nhin rong + chum hai ngon =================' + R +
   'int JxLia_ZoomLay()' + R +
   '{' + R +
   '\treturn s_nZoom;' + R +
   '}' + R + R +
   'void JxLia_ZoomDat(int nPhanTram)' + R +
   '{' + R +
   '\tDocCaiDat();' + R +
   '\tif (nPhanTram < 100) nPhanTram = 100;' + R +
   '\tif (nPhanTram > s_nZoomToiDa) nPhanTram = s_nZoomToiDa;' + R +
   '\tnPhanTram = (nPhanTram + s_nZoomBuoc / 2) / s_nZoomBuoc * s_nZoomBuoc;' + R +
   '\tif (nPhanTram < 100) nPhanTram = 100;' + R +
   '\ts_nZoom = nPhanTram;' + R +
   '\tif (s_nZoomDaAp == s_nZoom || !g_pCoreShell)' + R +
   '\t\treturn;' + R +
   '\ts_nZoomDaAp = s_nZoom;' + R +
   '\t// Represent3: RT = khung x zoom, thu nho khi blit; Core: vung truy van vat the = khung x zoom (khong thi nguoi o ria khong duoc ve)' + R +
   '\tRep3TheGioi(4, s_nZoom * 10);' + R +
   '\tg_pCoreShell->SetRepresentAreaSize(SCREEN_WIDTH * s_nZoom / 100, SCREEN_HEIGHT * s_nZoom / 100);' + R +
   '\tif (s_nNhatKy || s_nZoomThu)' + R +
   '\t\tg_DebugLog("[ZOOM] zoom %d%% (Rep3 %d, vung truy van %dx%d)", s_nZoom, Rep3TheGioi(5, 0), SCREEN_WIDTH * s_nZoom / 100, SCREEN_HEIGHT * s_nZoom / 100);' + R +
   '}' + R + R +
   'bool JxLia_ChumDuoc(int x1, int y1, int x2, int y2)' + R +
   '{' + R +
   '\tDocCaiDat();' + R +
   '\tif (!s_nZoomBat)' + R +
   '\t\treturn false;' + R +
   '\treturn JxLia_DuocBatDau(x1, y1) && JxLia_DuocBatDau(x2, y2);' + R +
   '}' + R + R +
   'void JxLia_ChumBatDau(int x1, int y1, int x2, int y2)' + R +
   '{' + R +
   '\tfloat dx = (float)(x2 - x1), dy = (float)(y2 - y1);' + R + R +
   '\ts_fChumD0 = sqrtf(dx * dx + dy * dy);' + R +
   '\tif (s_fChumD0 < 10.f)' + R +
   '\t\ts_fChumD0 = 10.f;' + R +
   '\ts_nChumZoom0 = s_nZoom;' + R +
   '\ts_nChum = 1;' + R +
   '\tJxLia_BatDau((x1 + x2) / 2, (y1 + y2) / 2);\t// lia theo tam hai ngon (dang lech thi giu lech, neo lai tu tam)' + R +
   '\tif (s_nNhatKy)' + R +
   '\t\tg_DebugLog("[ZOOM] chum bat dau: d0=%.0f zoom=%d%%", s_fChumD0, s_nZoom);' + R +
   '}' + R + R +
   'void JxLia_ChumKeo(int x1, int y1, int x2, int y2)' + R +
   '{' + R +
   '\tfloat dx = (float)(x2 - x1), dy = (float)(y2 - y1);' + R +
   '\tfloat d = sqrtf(dx * dx + dy * dy);' + R + R +
   '\tif (!s_nChum)' + R +
   '\t\treturn;' + R +
   '\tif (d < 10.f)' + R +
   '\t\td = 10.f;' + R +
   '\tJxLia_ZoomDat((int)((float)s_nChumZoom0 * s_fChumD0 / d + 0.5f));\t// hai ngon gan nhau (d < d0) = zoom lon = thay rong hon' + R +
   '\tJxLia_Keo((x1 + x2) / 2, (y1 + y2) / 2);' + R +
   '}' + R + R +
   'void JxLia_ChumNha()' + R +
   '{' + R +
   '\tif (!s_nChum)' + R +
   '\t\treturn;' + R +
   '\ts_nChum = 0;' + R +
   '\tif (s_nNhatKy)' + R +
   '\t\tg_DebugLog("[ZOOM] chum nha: zoom=%d%%", s_nZoom);' + R +
   '\tJxLia_Nha();' + R +
   '}' + R + R +
   '#endif // JX_MOBILE' + R,
   'void JxLia_ZoomDat(int nPhanTram)' + R + '{')

# ================= E. config.ini: khoa Zoom* sau LiaThu =================
KHOI = [
    '; [ZOOM 13/09] chum hai ngon tren ban do = NHIN RONG RA (the gioi ve vao anh dem to hon roi thu nho, giao dien giu nguyen co);',
    ';   hai ngon gan nhau = thay rong hon, xa nhau = ve 1:1; zoom giu toi khi doi. 0 = tat.',
    'ZoomCanh=1',
    '; % nhin rong toi da (150 = thay rong gap 1,5 lan moi chieu = 2,25 lan dien tich - tot GPU/nen dat theo ti le)',
    'ZoomToiDa=150',
    '; nac zoom, %',
    'ZoomBuoc=5',
    '; zoom luc vao game, %',
    'ZoomMacDinh=100',
    '; GO LOI (may ao khong co hai ngon): % zoom tu dat 3 s sau khi vao the gioi; 0 = tat',
    'ZoomThu=0',
]
def va_config(p):
    s = doc(p)
    if 'ZoomCanh=' in s:
        KQ.append('bo qua (da co): %s : ZoomCanh' % p); return
    nl = '\r\n' if s.count('\r\n') > s.count('\n') // 2 else '\n'
    neo = 'LiaThu=0,0' + nl
    if s.count(neo) != 1:
        raise SystemExit('LOI: khong thay LiaThu=0,0 duy nhat trong %s' % p)
    ghi(p, s.replace(neo, neo + nl.join(KHOI) + nl))
    KQ.append('da va: %s : khoi [Cham] Zoom*' % p)
va_config(os.path.join(GOC, 'android', 'du_lieu_ghi_de', 'config.ini'))
if '--config' in sys.argv:
    for p in sys.argv[sys.argv.index('--config') + 1:]:
        if os.path.isfile(p): va_config(p)
        else: KQ.append('khong co tep: %s' % p)
print('\n'.join(KQ))
