# -*- coding: latin-1 -*-
# [LAC 14/09] CAMERA LAC NHE KHI LIA (chu 13/09 23:1x "lam 2 di" sau khi xem mo phong quay 90/45/15/5 do): keo ngang toi LiaXaNgang
# thi CA CANH (anh dem the gioi) xoay LiaLacDo do (mac dinh 5) theo phep "quay mat dat" 2:1 M = [[c,-2s],[s/2,c]] quanh tam khung,
# tha tay troi ve 0 cung do lech. Represent3: lenh 6 dat goc (0,01 do), 7 hoi, 8 le RT (phan nghin) de goc khung khong ho khi xoay;
# RT that = zoom x le (m_nTgZoomRt); blit = quad xoay; hai cua toa do (CoordinateTransform/X, ViewPortCoordToSpaceCoord) them M / M^-1.
# JxLiaCanh: ApXoay() moi lan ap lech (goc ~ lechX/max), KetThuc -> goc 0 + bo le; LiaLacThu = go loi giu goc co dinh.
# Idempotent, latin-1, neo CRLF (nguon) / LF-CRLF tu nhan (ini). Dung: python android/va_nguon_lac_1409.py [goc] [--config <ini> ...]
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

RH = os.path.join(GOC, 'Sources', 'Represent', 'Represent3', 'KRepresentShell3.h')
RC = os.path.join(GOC, 'Sources', 'Represent', 'Represent3', 'KRepresentShell3.cpp')
LC = os.path.join(GOC, 'Sources', 'S3Client', 'Platform', 'JxLiaCanh.cpp')

# ===================== KRepresentShell3.h =====================
va(RH,
   '    int   m_nTgLeftKhung, m_nTgTopKhung;' + R,
   '    int   m_nTgLeftKhung, m_nTgTopKhung;' + R +
   '    // [LAC 14/09] camera lac nhe khi lia: xoay mat dat goc m_nTgXoay (0,01 do; phep 2:1 x\' = c.x - 2s.y, y\' = s.x/2 + c.y quanh tam khung)' + R +
   '    // luc blit + hai cua toa do; RT them le m_nTgLe (phan nghin, lenh 8) de goc khung khong ho; m_nTgZoomRt = zoom x le = ti le RT that.' + R +
   '    int   m_nTgXoay, m_nTgLe, m_nTgZoomRt;' + R,
   'int   m_nTgXoay, m_nTgLe, m_nTgZoomRt;')

# ===================== KRepresentShell3.cpp =====================
va(RC,
   'int g_nJxTheGioiEp = 0;\t// Rep3_JxEpTrinhChieu (D3D9onGPUDev.cpp): be mat / cua so doi -> khung toi phai ve the gioi that' + R,
   '#include <math.h>\t// [LAC 14/09] cosf / sinf / floorf (xoay anh dem the gioi)' + R +
   'int g_nJxTheGioiEp = 0;\t// Rep3_JxEpTrinhChieu (D3D9onGPUDev.cpp): be mat / cua so doi -> khung toi phai ve the gioi that' + R,
   '#include <math.h>\t// [LAC 14/09]')
va(RC,
   '\tm_nTgZoom = 1000; m_nZoomDx = m_nZoomDy = 0; m_nTgKhungW = m_nTgKhungH = 0; m_nTgLeftKhung = m_nTgTopKhung = 0;\t// [ZOOM 13/09]' + R,
   '\tm_nTgZoom = 1000; m_nZoomDx = m_nZoomDy = 0; m_nTgKhungW = m_nTgKhungH = 0; m_nTgLeftKhung = m_nTgTopKhung = 0;\t// [ZOOM 13/09]' + R +
   '\tm_nTgXoay = 0; m_nTgLe = 1000; m_nTgZoomRt = 1000;\t// [LAC 14/09]' + R,
   'm_nTgXoay = 0; m_nTgLe = 1000; m_nTgZoomRt = 1000;')
# lenh 6/7/8
va(RC,
   '\tif (nLenh == 5) return m_nTgZoom;\t// [ZOOM 13/09] hoi zoom hien tai' + R,
   '\tif (nLenh == 5) return m_nTgZoom;\t// [ZOOM 13/09] hoi zoom hien tai' + R +
   '\tif (nLenh == 6)' + R +
   '\t{\t// [LAC 14/09] dat goc xoay (0,01 do, kep +-15 do); 0 = khong xoay. Chi doi blit + cua toa do, khong doi RT' + R +
   '\t\tint nG = nThamSo; if (nG > 1500) nG = 1500; if (nG < -1500) nG = -1500;' + R +
   '\t\tif (nG != m_nTgXoay) { m_nTgXoay = nG; g_nJxTheGioiEp = 1; }' + R +
   '\t\treturn m_nTgXoay;' + R +
   '\t}' + R +
   '\tif (nLenh == 7) return m_nTgXoay;' + R +
   '\tif (nLenh == 8)' + R +
   '\t{\t// [LAC 14/09] le RT khi dang lia (phan nghin, 1000 = khong; kep 1000..1500): RT = khung x zoom x le -> lenh 0 cap lai RT' + R +
   '\t\tint nL = nThamSo; if (nL < 1000) nL = 1000; if (nL > 1500) nL = 1500;' + R +
   '\t\tif (nL != m_nTgLe) { Rep3Log("[LAC] le RT %d -> %d (phan nghin), goc %d", m_nTgLe, nL, m_nTgXoay); m_nTgLe = nL; g_nJxTheGioiEp = 1; }' + R +
   '\t\treturn m_nTgLe;' + R +
   '\t}' + R,
   '\tif (nLenh == 6)')
# lenh 0: ti le RT that = zoom x le
va(RC,
   '\t\tconst bool bZoom = (m_nTgZoom > 1000);\t// [ZOOM 13/09] dang nhin rong: luon ve the gioi vao RT to hon khung roi thu nho (K = 1)' + R +
   '\t\tif (bZoom) nK = 1;' + R,
   '\t\tm_nTgZoomRt = (m_nTgLe > 1000) ? m_nTgZoom * m_nTgLe / 1000 : m_nTgZoom;\t// [LAC 14/09] RT them le khi dang lia (goc khung khong ho luc xoay)' + R +
   '\t\tconst bool bZoom = (m_nTgZoomRt > 1000);\t// [ZOOM 13/09] dang nhin rong (hoac RT co le): luon ve the gioi vao RT to hon khung roi thu nho (K = 1)' + R +
   '\t\tif (bZoom) nK = 1;' + R,
   'm_nTgZoomRt = (m_nTgLe > 1000)')
va(RC,
   '\t\tconst int nRtW = bZoom ? ((g_nScreenWidth * m_nTgZoom / 1000 + 1) & ~1) : g_nScreenWidth;' + R +
   '\t\tconst int nRtH = bZoom ? ((g_nScreenHeight * m_nTgZoom / 1000 + 1) & ~1) : g_nScreenHeight;' + R,
   '\t\tconst int nRtW = bZoom ? ((g_nScreenWidth * m_nTgZoomRt / 1000 + 1) & ~1) : g_nScreenWidth;\t// [LAC 14/09] theo ti le RT that' + R +
   '\t\tconst int nRtH = bZoom ? ((g_nScreenHeight * m_nTgZoomRt / 1000 + 1) & ~1) : g_nScreenHeight;' + R,
   'g_nScreenWidth * m_nTgZoomRt / 1000')
# lenh 1: lui goc theo RT that
va(RC,
   '\t\tif (m_nTgZoom > 1000)' + R +
   '\t\t{\t// [ZOOM 13/09] RT to hon khung: goc RT lui de tieu diem van o giua; cull / cat trong shell theo co RT trong luc ve' + R,
   '\t\tif (m_nTgZoomRt > 1000)' + R +
   '\t\t{\t// [ZOOM 13/09] RT to hon khung: goc RT lui de tieu diem van o giua; cull / cat trong shell theo co RT trong luc ve ([LAC 14/09] ke ca le)' + R,
   '\t\tif (m_nTgZoomRt > 1000)' + R + '\t\t{\t// [ZOOM 13/09] RT to hon khung')
# lenh 2: tra khung, loc, quad xoay
va(RC,
   '\t\t\tif (m_nTgZoom > 1000) { g_nScreenWidth = m_nTgKhungW; g_nScreenHeight = m_nTgKhungH; m_nLeft = m_nTgLeftKhung; m_nTop = m_nTgTopKhung; }\t// [ZOOM 13/09] tra co khung + goc khung' + R,
   '\t\t\tif (m_nTgZoomRt > 1000) { g_nScreenWidth = m_nTgKhungW; g_nScreenHeight = m_nTgKhungH; m_nLeft = m_nTgLeftKhung; m_nTop = m_nTgTopKhung; }\t// [ZOOM 13/09] tra co khung + goc khung ([LAC 14/09] ke ca le)' + R,
   'if (m_nTgZoomRt > 1000) { g_nScreenWidth = m_nTgKhungW;')
va(RC,
   '\t\t{ const DWORD dwLoc = (m_nTgZoom > 1000) ? D3DTEXF_LINEAR : D3DTEXF_POINT;',
   '\t\t{ const DWORD dwLoc = (m_nTgZoomRt > 1000 || m_nTgXoay) ? D3DTEXF_LINEAR : D3DTEXF_POINT;',
   '(m_nTgZoomRt > 1000 || m_nTgXoay) ? D3DTEXF_LINEAR')
va(RC,
   '\t\tVERTEX2D v[4];' + R +
   '\t\tconst float fW = (float)g_nScreenWidth, fH = (float)g_nScreenHeight;\t// [ZOOM 13/09] dich blit = KHUNG (da tra lai o tren): RT to hon khung thi thu nho; khong zoom thi bang m_nTgW/H nhu cu' + R +
   '\t\tv[0].position = D3DXVECTOR4(0.0f, 0.0f, 100, 1); v[0].color = 0xffffffff; v[0].tu = 0.0f; v[0].tv = 0.0f;' + R +
   '\t\tv[1].position = D3DXVECTOR4(fW, 0.0f, 100, 1);   v[1].color = 0xffffffff; v[1].tu = 1.0f; v[1].tv = 0.0f;' + R +
   '\t\tv[2].position = D3DXVECTOR4(0.0f, fH, 100, 1);   v[2].color = 0xffffffff; v[2].tu = 0.0f; v[2].tv = 1.0f;' + R +
   '\t\tv[3].position = D3DXVECTOR4(fW, fH, 100, 1);     v[3].color = 0xffffffff; v[3].tu = 1.0f; v[3].tv = 1.0f;' + R,
   '\t\tVERTEX2D v[4];' + R +
   '\t\t{\t// [ZOOM 13/09] dich blit = KHUNG (da tra lai o tren); [LAC 14/09] = RT thu theo zoom nguoi choi (khung x le khi dang lia) roi XOAY goc m_nTgXoay' + R +
   '\t\t\t// quanh tam khung theo phep quay mat dat 2:1 (x\' = c.x - 2s.y, y\' = s.x/2 + c.y). Goc 0, le 1000: dung bang khung nhu cu.' + R +
   '\t\t\tconst float fKW = (float)g_nScreenWidth, fKH = (float)g_nScreenHeight;' + R +
   '\t\t\tconst float fTl = 1000.0f / (float)m_nTgZoom;' + R +
   '\t\t\tconst float fGoc = (float)m_nTgXoay * 3.14159265f / 18000.0f, fC = cosf(fGoc), fS = sinf(fGoc);' + R +
   '\t\t\tconst float fHx = (float)m_nTgW * 0.5f, fHy = (float)m_nTgH * 0.5f;' + R +
   '\t\t\tfor (int i = 0; i < 4; i++)' + R +
   '\t\t\t{' + R +
   '\t\t\t\tconst float qx = ((i & 1) ? fHx : -fHx) * fTl, qy = ((i & 2) ? fHy : -fHy) * fTl;' + R +
   '\t\t\t\tv[i].position = D3DXVECTOR4(fKW * 0.5f + fC * qx - 2.0f * fS * qy, fKH * 0.5f + 0.5f * fS * qx + fC * qy, 100, 1);' + R +
   '\t\t\t\tv[i].color = 0xffffffff; v[i].tu = (i & 1) ? 1.0f : 0.0f; v[i].tv = (i & 2) ? 1.0f : 0.0f;' + R +
   '\t\t\t}' + R +
   '\t\t}' + R,
   '[LAC 14/09] = RT thu theo zoom nguoi choi')
# cua toa do: khung -> the gioi (cham / chon vat the)
va(RC,
   '\t\tif (m_nTgZoom > 1000)' + R +
   '\t\t{\t// [ZOOM 13/09] cham tren khung -> diem anh RT (nhan zoom, bo lui goc RT) -> the gioi theo goc KHUNG (m_nLeft dang lui neu dang ve RT)' + R +
   '\t\t\tconst int nGocX = (m_nTgTrangThai == 1) ? m_nTgLeftKhung : m_nLeft;' + R +
   '\t\t\tconst int nGocY = (m_nTgTrangThai == 1) ? m_nTgTopKhung : m_nTop;' + R +
   '\t\t\tnX = nX * m_nTgZoom / 1000 - m_nZoomDx + nGocX;' + R +
   '\t\t\tnY = (nY * m_nTgZoom / 1000 - m_nZoomDy + nGocY + ((nZ * 887) >> 10)) * 2;' + R +
   '\t\t\treturn;' + R +
   '\t\t}' + R,
   '\t\tif (m_nTgZoomRt > 1000 || m_nTgXoay)' + R +
   '\t\t{\t// [ZOOM 13/09] cham tren khung -> diem anh RT (nhan zoom, bo lui goc RT) -> the gioi theo goc KHUNG (m_nLeft dang lui neu dang ve RT)' + R +
   '\t\t\t// [LAC 14/09] them M^-1 (x = c.q.x + 2s.q.y, y = -s.q.x/2 + c.q.y) quanh tam khung; goc 0 va le 1000 = cong thuc cu' + R +
   '\t\t\tconst int nGocX = (m_nTgTrangThai == 1) ? m_nTgLeftKhung : m_nLeft;' + R +
   '\t\t\tconst int nGocY = (m_nTgTrangThai == 1) ? m_nTgTopKhung : m_nTop;' + R +
   '\t\t\tconst int nKW = (m_nTgTrangThai == 1) ? m_nTgKhungW : g_nScreenWidth, nKH = (m_nTgTrangThai == 1) ? m_nTgKhungH : g_nScreenHeight;' + R +
   '\t\t\tconst float fGoc = (float)m_nTgXoay * 3.14159265f / 18000.0f, fC = cosf(fGoc), fS = sinf(fGoc);' + R +
   '\t\t\tconst float qx = (float)(nX - nKW / 2), qy = (float)(nY - nKH / 2), fZ = (float)m_nTgZoom / 1000.0f;' + R +
   '\t\t\tconst float px = (float)m_nTgW * 0.5f + (fC * qx + 2.0f * fS * qy) * fZ;' + R +
   '\t\t\tconst float py = (float)m_nTgH * 0.5f + (-0.5f * fS * qx + fC * qy) * fZ;' + R +
   '\t\t\tnX = (int)floorf(px + 0.5f) - m_nZoomDx + nGocX;' + R +
   '\t\t\tnY = ((int)floorf(py + 0.5f) - m_nZoomDy + nGocY + ((nZ * 887) >> 10)) * 2;' + R +
   '\t\t\treturn;' + R +
   '\t\t}' + R,
   '[LAC 14/09] them M^-1')
# cua toa do: the gioi -> khung (CoordinateTransform va CoordinateTransformX, cung mot khoi)
va(RC,
   '\t\tif (m_nTgZoom > 1000 && m_nTgTrangThai == 0)' + R +
   '\t\t{\t// [ZOOM 13/09] ve len khung (sau blit): diem anh RT = (diem - goc khung) + lui goc RT, roi thu nho theo zoom' + R +
   '\t\t\tnX = (nX + m_nZoomDx) * 1000 / m_nTgZoom;' + R +
   '\t\t\tnY = (nY + m_nZoomDy) * 1000 / m_nTgZoom;' + R +
   '\t\t}' + R,
   '\t\tif ((m_nTgZoomRt > 1000 || m_nTgXoay) && m_nTgTrangThai == 0)' + R +
   '\t\t{\t// [ZOOM 13/09] ve len khung (sau blit): diem anh RT = (diem - goc khung) + lui goc RT, roi thu nho theo zoom; [LAC 14/09] + xoay M quanh tam khung' + R +
   '\t\t\tconst float fGoc = (float)m_nTgXoay * 3.14159265f / 18000.0f, fC = cosf(fGoc), fS = sinf(fGoc);' + R +
   '\t\t\tconst float qx = ((float)(nX + m_nZoomDx) - (float)m_nTgW * 0.5f) * 1000.0f / (float)m_nTgZoom;' + R +
   '\t\t\tconst float qy = ((float)(nY + m_nZoomDy) - (float)m_nTgH * 0.5f) * 1000.0f / (float)m_nTgZoom;' + R +
   '\t\t\tnX = (int)floorf((float)g_nScreenWidth * 0.5f + fC * qx - 2.0f * fS * qy + 0.5f);' + R +
   '\t\t\tnY = (int)floorf((float)g_nScreenHeight * 0.5f + 0.5f * fS * qx + fC * qy + 0.5f);' + R +
   '\t\t}' + R,
   '[LAC 14/09] + xoay M quanh tam khung', so_lan=2)

# ===================== JxLiaCanh.cpp =====================
va(LC,
   'static int VeEm()    { return s_nNcVeNhanh ? s_nVeEm * 2 : s_nVeEm; }' + R,
   'static int VeEm()    { return s_nNcVeNhanh ? s_nVeEm * 2 : s_nVeEm; }' + R +
   '// [LAC 14/09] camera lac nhe khi lia: goc = LiaLacDo x (lechX / lech ngang toi da), Represent3 lenh 6 (goc 0,01 do) / 8 (le RT phan nghin)' + R +
   'static int\t\t\ts_nLacDo = 5;\t\t// LiaLacDo: do xoay toi da (0 = tat)' + R +
   'static int\t\t\ts_nLacLe = 112;\t\t// LiaLacLe: % anh dem rong them khi dang lia (goc khung khong ho khi xoay)' + R +
   'static int\t\t\ts_nLacThu = 0;\t\t// LiaLacThu: GO LOI - giu goc co dinh (do) 3 s sau khi vao the gioi' + R +
   'static int\t\t\ts_nLacThuXong = 0;' + R +
   'static int\t\t\ts_nGocDaGui = 0, s_nLeDaGui = 1000;\t// da gui cho Represent3' + R,
   's_nLacDo = 5;')
va(LC,
   '\ts_nZoomChu   = GetPrivateProfileInt("Cham", "ZoomChu", 1, szCfg);' + R,
   '\ts_nZoomChu   = GetPrivateProfileInt("Cham", "ZoomChu", 1, szCfg);' + R +
   '\ts_nLacDo  = GetPrivateProfileInt("Cham", "LiaLacDo", 5, szCfg);\t// [LAC 14/09]' + R +
   '\ts_nLacLe  = GetPrivateProfileInt("Cham", "LiaLacLe", 112, szCfg);' + R +
   '\ts_nLacThu = GetPrivateProfileInt("Cham", "LiaLacThu", 0, szCfg);' + R +
   '\tif (s_nLacDo < -15) s_nLacDo = -15; if (s_nLacDo > 15) s_nLacDo = 15;' + R +
   '\tif (s_nLacLe < 100) s_nLacLe = 100; if (s_nLacLe > 150) s_nLacLe = 150;' + R +
   '\tif (s_nLacThu < -15) s_nLacThu = -15; if (s_nLacThu > 15) s_nLacThu = 15;' + R,
   '"LiaLacDo"')
va(LC,
   '\tg_DebugLog("[CAMERA] theo map: nho=%d (zoom nguoi choi da nho %d%%) toc do troi=%d%%/s chu=%d", s_nZoomNho, s_nZoomNguoiChoi, s_nZoomTocDo, s_nZoomChu);' + R,
   '\tg_DebugLog("[CAMERA] theo map: nho=%d (zoom nguoi choi da nho %d%%) toc do troi=%d%%/s chu=%d", s_nZoomNho, s_nZoomNguoiChoi, s_nZoomTocDo, s_nZoomChu);' + R +
   '\tg_DebugLog("[LAC] camera lac nhe khi lia: %d do, le RT %d%%, thu %d do", s_nLacDo, s_nLacLe, s_nLacThu);' + R,
   '[LAC] camera lac nhe khi lia:')
va(LC,
   'static void ApLech()' + R + '{' + R + '\tif (!g_pCoreShell)' + R + '\t\treturn;' + R,
   '// [LAC 14/09] goc xoay theo do lech ngang (dang lia) -> Represent3 lenh 6; le RT (lenh 8) + vung truy van vat the rong theo khi dang lia' + R +
   'static void ApXoay()' + R +
   '{' + R +
   '\tint nGoc = 0, nLe = 1000;' + R + R +
   '\tif (s_nLacThu && s_nLacThuXong)' + R +
   '\t\treturn;\t// go loi: giu goc co dinh' + R +
   '\tif (s_nLacDo != 0 && s_nTrangThai != LIA_KHONG)' + R +
   '\t{' + R +
   '\t\tconst float fMaxX = (float)(SCREEN_WIDTH * s_nXaNgang / 100) * (float)s_nZoom / 100.f;' + R +
   '\t\tfloat fTi = (fMaxX > 1.f) ? s_fLechX / fMaxX : 0.f;' + R +
   '\t\tif (fTi > 1.f) fTi = 1.f; if (fTi < -1.f) fTi = -1.f;' + R +
   '\t\tnGoc = (int)(fTi * (float)s_nLacDo * 100.f);' + R +
   '\t\tnLe = s_nLacLe * 10;' + R +
   '\t}' + R +
   '\tif (nLe != s_nLeDaGui)' + R +
   '\t{' + R +
   '\t\tRep3TheGioi(8, nLe); s_nLeDaGui = nLe;' + R +
   '\t\tif (g_pCoreShell) g_pCoreShell->SetRepresentAreaSize(SCREEN_WIDTH * s_nZoom / 100 * nLe / 1000, SCREEN_HEIGHT * s_nZoom / 100 * nLe / 1000);' + R +
   '\t}' + R +
   '\tif (nGoc != s_nGocDaGui) { Rep3TheGioi(6, nGoc); s_nGocDaGui = nGoc; }' + R +
   '}' + R + R +
   'static void ApLech()' + R + '{' + R + '\tif (!g_pCoreShell)' + R + '\t\treturn;' + R,
   'static void ApXoay()')
va(LC,
   '\tg_pCoreShell->SceneMapOperation(GSMOI_SCENE_MAP_FOCUS_OFFSET, (KUPARAM)(unsigned int)s_nLechX, s_nLechY);' + R + '}' + R,
   '\tg_pCoreShell->SceneMapOperation(GSMOI_SCENE_MAP_FOCUS_OFFSET, (KUPARAM)(unsigned int)s_nLechX, s_nLechY);' + R +
   '\tApXoay();\t// [LAC 14/09]' + R + '}' + R,
   '\tApXoay();\t// [LAC 14/09]' + R + '}')
va(LC,
   '\ts_nDaBatCo = 0;' + R + '\ts_nTrangThai = LIA_KHONG;' + R + '}' + R,
   '\ts_nDaBatCo = 0;' + R + '\ts_nTrangThai = LIA_KHONG;' + R + '\tApXoay();\t// [LAC 14/09] goc ve 0, bo le RT' + R + '}' + R,
   '\tApXoay();\t// [LAC 14/09] goc ve 0')
va(LC,
   '\tg_pCoreShell->SetRepresentAreaSize(SCREEN_WIDTH * s_nZoom / 100, SCREEN_HEIGHT * s_nZoom / 100);' + R,
   '\tg_pCoreShell->SetRepresentAreaSize(SCREEN_WIDTH * s_nZoom / 100 * s_nLeDaGui / 1000, SCREEN_HEIGHT * s_nZoom / 100 * s_nLeDaGui / 1000);\t// [LAC 14/09] + le RT khi dang lia' + R,
   'SCREEN_WIDTH * s_nZoom / 100 * s_nLeDaGui / 1000')
va(LC,
   '\telse if (uNay - s_uVaoGameLuc >= 1000 && s_nCanDocMap)' + R + '\t\tCamera_DocMap();' + R,
   '\telse if (uNay - s_uVaoGameLuc >= 1000 && s_nCanDocMap)' + R + '\t\tCamera_DocMap();' + R +
   '\tif (s_nLacThu && !s_nLacThuXong && uNay - s_uVaoGameLuc >= 3000)' + R +
   '\t{\t// [LAC 14/09] GO LOI: giu goc co dinh de chup anh / cham thu (may ao khong keo tay duoc)' + R +
   '\t\ts_nLacThuXong = 1;' + R +
   '\t\tRep3TheGioi(8, s_nLacLe * 10); s_nLeDaGui = s_nLacLe * 10;' + R +
   '\t\tRep3TheGioi(6, s_nLacThu * 100); s_nGocDaGui = s_nLacThu * 100;' + R +
   '\t\tif (g_pCoreShell) g_pCoreShell->SetRepresentAreaSize(SCREEN_WIDTH * s_nZoom / 100 * s_nLeDaGui / 1000, SCREEN_HEIGHT * s_nZoom / 100 * s_nLeDaGui / 1000);' + R +
   '\t\tg_DebugLog("[LAC] LiaLacThu: giu goc %d do, le RT %d%%", s_nLacThu, s_nLacLe);' + R +
   '\t}' + R,
   '[LAC 14/09] GO LOI: giu goc co dinh')

# LiaThu (keo gia lap trong Nhip): JxLia_Nha() lay GetTickCount MOI > uNay cua nhip -> uNay - s_uNhaLuc am (unsigned) -> "het cho" ngay,
# ve xong sau 0,3 s, khong thu duoc nhip cho ve / lac. Dat lai s_uNhaLuc = uNay (chi duong go loi).
va(LC,
   '\t\t\tJxLia_Nha();' + R + '\t\t}' + R,
   '\t\t\tJxLia_Nha();' + R + '\t\t\ts_uNhaLuc = uNay;\t// [LAC 14/09] keo gia lap: luc nha = luc nhip (khong thi unsigned am -> ve ngay)' + R + '\t\t}' + R,
   's_uNhaLuc = uNay;\t// [LAC 14/09]')

# ===================== config.ini =====================
KHOI = [
    '; [LAC 14/09] camera lac nhe khi lia (chu: "lam 2 di"): keo ngang toi LiaXaNgang thi ca canh xoay LiaLacDo do (phep quay mat dat 2:1,',
    ';   am = xoay chieu nguoc), tha tay troi ve 0 cung do lech. 0 = tat. LiaLacLe = % anh dem the gioi rong them khi dang lia de goc khung',
    ';   khong ho luc xoay (112). LiaLacThu = GO LOI: giu goc co dinh (do) 3 s sau khi vao the gioi de chup anh / cham thu; 0 = tat',
    'LiaLacDo=5',
    'LiaLacLe=112',
    'LiaLacThu=0',
]
def va_config(p):
    s = doc(p)
    if 'LiaLacDo=' in s:
        KQ.append('bo qua (da co): %s : LiaLacDo' % p); return
    nl = '\r\n' if s.count('\r\n') > s.count('\n') // 2 else '\n'
    neo = 'ZoomChu=1' + nl
    if s.count(neo) != 1:
        raise SystemExit('LOI: khong thay ZoomChu=1 duy nhat trong %s' % p)
    ghi(p, s.replace(neo, neo + nl.join(KHOI) + nl))
    KQ.append('da va: %s : khoi [Cham] LiaLac*' % p)
va_config(os.path.join(GOC, 'android', 'du_lieu_ghi_de', 'config.ini'))
if '--config' in sys.argv:
    for p in sys.argv[sys.argv.index('--config') + 1:]:
        if os.path.isfile(p): va_config(p)
        else: KQ.append('khong co tep: %s' % p)
print('\n'.join(KQ))
