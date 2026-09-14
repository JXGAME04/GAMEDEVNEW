# -*- coding: latin-1 -*-
# [TGCAP 14/09] RT the gioi cap MOT lan, moi buoc zoom / le chi doi phan dung + uv (phien do nhip do Fold 7 12:41 ban 109141129: chum ngon
#   zoom 1200 -> 800 = 5 buoc zoom + RT2 cap lai 3 lan -> khung 97 ms; moi lan [LAC] le 1000 -> 1120 cap lai RT -> khung 20-30 ms; [TAI-DO] tao
#   texture lon 5-54 ms + driver cam ket bo nho o lan nop dau).
#   Sources/Represent/Represent3/KRepresentShell3.cpp/.h (chi khoi JX_MOBILE [TG]):
#   - RT: texture that co CAP = khung x TheGioiRTCap (phan nghin, mac dinh 1500 = du zoom 150 % hoac le 112 %); m_nTgW/H = phan DUNG
#     (khung x zoom x le) doi moi buoc; chi cap lai khi khung doi (gap / mo, xoay) hoac phan dung vuot cap (cap moi theo nac 250 phan nghin).
#     TheGioiRTSan=1: cap san o khung dau tien vao the gioi (khi TheGioiRT=1) de lan chum / lia dau khong khung 50 ms.
#   - RT2 (phong to co loc net): cap = 2 x khung x TheGioiRT2Cap (mac dinh 1150 >= le lia 112 %); phan dung = 2 x m_nTgW/H.
#   - Blit: uv = phan dung / cap (RT -> RT2 va RT/RT2 -> khung). Toa do pixel khong doi vi CDevGpu::SetRenderTarget dat viewport = ca texture
#     (VS uniform 2/cap) -> ve o goc trai tren cua texture; mau LINEAR o mep phan dung khong cham texel ngoai (thu nho / 1:1: vi tri mau cuoi
#     <= dung - 0,5 texel).
# Idempotent, latin-1, neo CRLF. Dung: python android/va_nguon_tgcap_1409.py [goc]
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
RH = os.path.join(GOC, 'Sources', 'Represent', 'Represent3', 'KRepresentShell3.h')

# ---------------- header: cap RT / RT2 ----------------
va(RH,
   '    int   m_nTgW, m_nTgH;',
   '    int   m_nTgW, m_nTgH;' + R +
   '    int   m_nTgCapW, m_nTgCapH, m_nTgCapKhungW, m_nTgCapKhungH;   // [TGCAP 14/09] co CAP (texture that) cua RT + co khung luc cap; m_nTgW/H = phan DUNG (khung x zoom x le), uv blit = dung / cap',
   '[TGCAP 14/09] co CAP (texture that) cua RT')

va(RH,
   '    int   m_nTg2W, m_nTg2H;',
   '    int   m_nTg2W, m_nTg2H;' + R +
   '    int   m_nTg2CapW, m_nTg2CapH;   // [TGCAP 14/09] co cap RT2 (2 x khung x TheGioiRT2Cap), m_nTg2W/H = phan dung (2 x m_nTgW/H)',
   '[TGCAP 14/09] co cap RT2')

# ---------------- cpp: ctor init ----------------
va(RC,
   'm_pTgTex2 = NULL; m_pTgSurf2 = NULL; m_nTg2W = m_nTg2H = 0;' + T + '// [ZOOM3D 14/09] RT2 phong to',
   'm_pTgTex2 = NULL; m_pTgSurf2 = NULL; m_nTg2W = m_nTg2H = 0;' + T + '// [ZOOM3D 14/09] RT2 phong to' + R +
   T + 'm_nTgCapW = m_nTgCapH = m_nTgCapKhungW = m_nTgCapKhungH = 0; m_nTg2CapW = m_nTg2CapH = 0;' + T + '// [TGCAP 14/09]',
   'm_nTg2CapW = m_nTg2CapH = 0;' + T + '// [TGCAP 14/09]')

# ---------------- cpp: statics + doc ini ----------------
va(RC,
   'static unsigned s_uTgDemVe = 0, s_uTgDemBlit = 0, s_uTgDemThuong = 0, s_uTgDemDoiK = 0;' + T + '// thong ke ky [TG]',
   'static unsigned s_uTgDemVe = 0, s_uTgDemBlit = 0, s_uTgDemThuong = 0, s_uTgDemDoiK = 0;' + T + '// thong ke ky [TG]' + R +
   'static int s_nTgRtCap = 1500, s_nTgRt2Cap = 1150, s_nTgRtSan = 1;' + T + '// [TGCAP 14/09] cap RT = khung x RtCap, RT2 = 2 x khung x Rt2Cap (phan nghin); San = cap san RT o khung dau vao the gioi',
   '[TGCAP 14/09] cap RT = khung x RtCap')

va(RC,
   T + 'Rep3Log("[TG] the gioi RT khi qua tai: bat=%d ep=%d, the gioi toi thieu %d Hz (TheGioiRT / TheGioiRTEp / TheGioiToiThieuHz)", s_nTgBat, s_nTgEp, s_nTgToiThieuHz);',
   T + 'Rep3Log("[TG] the gioi RT khi qua tai: bat=%d ep=%d, the gioi toi thieu %d Hz (TheGioiRT / TheGioiRTEp / TheGioiToiThieuHz)", s_nTgBat, s_nTgEp, s_nTgToiThieuHz);' + R +
   T + 's_nTgRtCap = Rep3Ini("TheGioiRTCap", 1500); if (s_nTgRtCap < 1000) s_nTgRtCap = 1000; if (s_nTgRtCap > 3000) s_nTgRtCap = 3000;' + T + '// [TGCAP 14/09]' + R +
   T + 's_nTgRt2Cap = Rep3Ini("TheGioiRT2Cap", 1150); if (s_nTgRt2Cap < 1000) s_nTgRt2Cap = 1000; if (s_nTgRt2Cap > 1500) s_nTgRt2Cap = 1500;' + R +
   T + 's_nTgRtSan = Rep3Ini("TheGioiRTSan", 1) ? 1 : 0;' + R +
   T + 'Rep3Log("[TGCAP] RT cap %d, RT2 cap 2 x %d phan nghin khung, cap san %d (TheGioiRTCap / TheGioiRT2Cap / TheGioiRTSan)", s_nTgRtCap, s_nTgRt2Cap, s_nTgRtSan);',
   's_nTgRtCap = Rep3Ini("TheGioiRTCap"')

# ---------------- cpp: JxTheGioiHuy ----------------
va(RC,
   'SAFE_RELEASE(m_pTgSurf2); SAFE_RELEASE(m_pTgTex2); m_nTg2W = m_nTg2H = 0;' + T + '// [ZOOM3D 14/09]' + R +
   T + 'm_nTgW = m_nTgH = 0; s_uTgRTVe = 0xFFFFFFFFu;',
   'SAFE_RELEASE(m_pTgSurf2); SAFE_RELEASE(m_pTgTex2); m_nTg2W = m_nTg2H = m_nTg2CapW = m_nTg2CapH = 0;' + T + '// [ZOOM3D 14/09] [TGCAP 14/09] ca cap' + R +
   T + 'm_nTgW = m_nTgH = m_nTgCapW = m_nTgCapH = m_nTgCapKhungW = m_nTgCapKhungH = 0; s_uTgRTVe = 0xFFFFFFFFu;',
   '[ZOOM3D 14/09] [TGCAP 14/09] ca cap')

# ---------------- cpp: JxTheGioiNet2 (RT2 cap co dinh) ----------------
va(RC,
   T + 'const int nW = m_nTgW * 2, nH = m_nTgH * 2;' + R +
   T + 'if (m_pTgTex2 && (m_nTg2W != nW || m_nTg2H != nH)) { SAFE_RELEASE(m_pTgSurf2); SAFE_RELEASE(m_pTgTex2); }' + R +
   T + 'if (!m_pTgTex2)' + R +
   T + '{' + R +
   T + T + 'm_nTg2W = nW; m_nTg2H = nH;' + R +
   T + T + 'if (FAILED(PD3DDEVICE->CreateTexture(nW, nH, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_pTgTex2, NULL)) || !m_pTgTex2',
   T + 'const int nW = m_nTgW * 2, nH = m_nTgH * 2;' + T + '// phan DUNG cua RT2' + R +
   T + 'if (m_pTgTex2 && (nW > m_nTg2CapW || nH > m_nTg2CapH)) { SAFE_RELEASE(m_pTgSurf2); SAFE_RELEASE(m_pTgTex2); m_nTg2CapW = m_nTg2CapH = 0; }' + T + '// [TGCAP 14/09] chi cap lai khi KHONG DU cho, khong cap lai moi buoc zoom' + R +
   T + 'm_nTg2W = nW; m_nTg2H = nH;' + R +
   T + 'if (!m_pTgTex2)' + R +
   T + '{' + R +
   T + T + 'int nCapW = (m_nTgCapKhungW * 2 * s_nTgRt2Cap / 1000 + 1) & ~1, nCapH = (m_nTgCapKhungH * 2 * s_nTgRt2Cap / 1000 + 1) & ~1;' + T + '// [TGCAP 14/09] cap = 2 x khung x le toi da (TheGioiRT2Cap)' + R +
   T + T + 'if (nCapW < nW) nCapW = nW; if (nCapH < nH) nCapH = nH;' + R +
   T + T + 'm_nTg2CapW = nCapW; m_nTg2CapH = nCapH;' + R +
   T + T + 'if (FAILED(PD3DDEVICE->CreateTexture(nCapW, nCapH, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_pTgTex2, NULL)) || !m_pTgTex2',
   '[TGCAP 14/09] chi cap lai khi KHONG DU cho')

va(RC,
   T + T + T + 'Rep3Log("[ZOOM3D] khong tao duoc RT2 %dx%d -> phong to blit thang LINEAR", nW, nH);' + R +
   T + T + T + 'SAFE_RELEASE(m_pTgSurf2); SAFE_RELEASE(m_pTgTex2); m_nTg2W = m_nTg2H = 0; s_nNet = 0;',
   T + T + T + 'Rep3Log("[ZOOM3D] khong tao duoc RT2 %dx%d -> phong to blit thang LINEAR", nCapW, nCapH);' + R +
   T + T + T + 'SAFE_RELEASE(m_pTgSurf2); SAFE_RELEASE(m_pTgTex2); m_nTg2W = m_nTg2H = m_nTg2CapW = m_nTg2CapH = 0; s_nNet = 0;' + T + '// [TGCAP 14/09]',
   'm_nTg2W = m_nTg2H = m_nTg2CapW = m_nTg2CapH = 0; s_nNet = 0;')

va(RC,
   T + T + 'Rep3Log("[ZOOM3D] RT2 %dx%d cho phong to co loc net (zoom %d)", nW, nH, m_nTgZoom);',
   T + T + 'Rep3Log("[ZOOM3D] RT2 cap %dx%d (dung %dx%d) cho phong to co loc net (zoom %d)", nCapW, nCapH, nW, nH, m_nTgZoom);' + T + '// [TGCAP 14/09]',
   '[ZOOM3D] RT2 cap %dx%d (dung %dx%d)')

# ---------------- cpp: lenh 0 - cap RT mot lan + cap san ----------------
va(RC,
   T + T + 'if (!bZoom && !bPhongTo && (!s_nTgBat || (nK != 2 && s_nTgEp != 1))) { s_uTgVeThat = s_uTgKhung; s_uTgDemThuong++; return 0; }',
   T + T + 'const bool bDuongCu = (!bZoom && !bPhongTo && (!s_nTgBat || (nK != 2 && s_nTgEp != 1)));' + T + '// [TGCAP 14/09] khung nay ve thang (khong qua RT)' + R +
   T + T + 'if (bDuongCu && (m_pTgTex || !s_nTgRtSan || !s_nTgBat)) { s_uTgVeThat = s_uTgKhung; s_uTgDemThuong++; return 0; }' + T + '// [TGCAP 14/09] chua co RT + TheGioiRTSan: di tiep de CAP SAN roi moi ve thang',
   '[TGCAP 14/09] khung nay ve thang (khong qua RT)')

va(RC,
   T + T + 'if (!m_pTgTex || m_nTgW != nRtW || m_nTgH != nRtH)' + R +
   T + T + '{' + T + '// RT phai dung co khung logic (gap / mo, doi ho khung); [ZOOM 13/09] hoac khung x zoom' + R +
   T + T + T + 'JxTheGioiHuy();' + R +
   T + T + T + 'm_nTgW = nRtW; m_nTgH = nRtH;' + R +
   T + T + T + 'if (m_nTgW <= 0 || m_nTgH <= 0 || FAILED(PD3DDEVICE->CreateTexture(m_nTgW, m_nTgH, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_pTgTex, NULL))',
   T + T + 'if (!m_pTgTex || nRtW > m_nTgCapW || nRtH > m_nTgCapH || g_nScreenWidth != m_nTgCapKhungW || g_nScreenHeight != m_nTgCapKhungH)' + R +
   T + T + '{' + T + '// [TGCAP 14/09] chi cap lai khi khung doi (gap / mo, xoay) hoac phan dung vuot cap; cap = khung x TheGioiRTCap (nac 250 phan nghin khi vuot)' + R +
   T + T + T + '// de moi buoc zoom / le chi doi phan dung + uv blit, khong huy / cap lai texture (Fold 7 12:41: ~10 lan cap/giay khi chum ngon = khung 97 ms)' + R +
   T + T + T + 'JxTheGioiHuy();' + R +
   T + T + T + 'int nMuc = s_nTgRtCap;' + R +
   T + T + T + 'if (g_nScreenWidth > 0 && g_nScreenHeight > 0)' + R +
   T + T + T + '{' + R +
   T + T + T + T + 'const int nCanW = (nRtW * 1000 + g_nScreenWidth - 1) / g_nScreenWidth, nCanH = (nRtH * 1000 + g_nScreenHeight - 1) / g_nScreenHeight;' + R +
   T + T + T + T + 'const int nCan = (nCanW > nCanH) ? nCanW : nCanH;' + R +
   T + T + T + T + 'while (nMuc < nCan && nMuc < 6000) nMuc += 250;' + R +
   T + T + T + '}' + R +
   T + T + T + 'm_nTgCapKhungW = g_nScreenWidth; m_nTgCapKhungH = g_nScreenHeight;' + R +
   T + T + T + 'm_nTgCapW = (g_nScreenWidth * nMuc / 1000 + 1) & ~1; m_nTgCapH = (g_nScreenHeight * nMuc / 1000 + 1) & ~1;' + R +
   T + T + T + 'if (m_nTgCapW < nRtW) m_nTgCapW = nRtW; if (m_nTgCapH < nRtH) m_nTgCapH = nRtH;' + R +
   T + T + T + 'm_nTgW = nRtW; m_nTgH = nRtH;' + R +
   T + T + T + 'if (m_nTgCapW <= 0 || m_nTgCapH <= 0 || FAILED(PD3DDEVICE->CreateTexture(m_nTgCapW, m_nTgCapH, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_pTgTex, NULL))',
   '[TGCAP 14/09] chi cap lai khi khung doi')

va(RC,
   T + T + T + T + 'Rep3Log("[TG] khong tao duoc render target %dx%d -> tat the gioi RT", m_nTgW, m_nTgH);',
   T + T + T + T + 'Rep3Log("[TG] khong tao duoc render target %dx%d -> tat the gioi RT", m_nTgCapW, m_nTgCapH);' + T + '// [TGCAP 14/09]',
   'tat the gioi RT", m_nTgCapW, m_nTgCapH);')

va(RC,
   T + T + T + 'Rep3Log("[TG] render target the gioi %dx%d", m_nTgW, m_nTgH);' + R +
   T + T + '}' + R,
   T + T + T + 'Rep3Log("[TG] render target the gioi cap %dx%d (dung %dx%d, khung %dx%d, muc %d)", m_nTgCapW, m_nTgCapH, m_nTgW, m_nTgH, g_nScreenWidth, g_nScreenHeight, nMuc);' + T + '// [TGCAP 14/09]' + R +
   T + T + '}' + R +
   T + T + 'else { m_nTgW = nRtW; m_nTgH = nRtH; }' + T + '// [TGCAP 14/09] chi doi phan dung, giu texture' + R +
   T + T + 'if (bDuongCu) { s_uTgVeThat = s_uTgKhung; s_uTgDemThuong++; return 0; }' + T + '// [TGCAP 14/09] da cap san, khung nay van ve thang' + R,
   '[TGCAP 14/09] chi doi phan dung, giu texture')

# ---------------- cpp: lenh 2 - uv = dung / cap ----------------
va(RC,
   T + T + T + T + 'PD3DDEVICE->SetFVF(D3DFVF_VERTEX2D);' + R +
   T + T + T + T + 'for (int i = 0; i < 4; i++)',
   T + T + T + T + 'PD3DDEVICE->SetFVF(D3DFVF_VERTEX2D);' + R +
   T + T + T + T + 'const float fU1 = (float)m_nTgW / (float)m_nTgCapW, fV1 = (float)m_nTgH / (float)m_nTgCapH;' + T + '// [TGCAP 14/09] uv = phan dung / cap cua RT' + R +
   T + T + T + T + 'for (int i = 0; i < 4; i++)',
   '[TGCAP 14/09] uv = phan dung / cap cua RT')

va(RC,
   'q[i].color = 0xffffffff; q[i].tu = (i & 1) ? 1.0f : 0.0f; q[i].tv = (i & 2) ? 1.0f : 0.0f;',
   'q[i].color = 0xffffffff; q[i].tu = (i & 1) ? fU1 : 0.0f; q[i].tv = (i & 2) ? fV1 : 0.0f;' + T + '// [TGCAP 14/09]',
   'q[i].tu = (i & 1) ? fU1 : 0.0f;')

va(RC,
   T + T + T + 'const float fHx = (float)m_nTgW * 0.5f, fHy = (float)m_nTgH * 0.5f;' + R,
   T + T + T + 'const float fHx = (float)m_nTgW * 0.5f, fHy = (float)m_nTgH * 0.5f;' + R +
   T + T + T + 'const float fU = (pTexBlit == m_pTgTex2) ? (float)m_nTg2W / (float)m_nTg2CapW : (float)m_nTgW / (float)m_nTgCapW;' + T + '// [TGCAP 14/09] uv = phan dung / cap (RT2 hay RT)' + R +
   T + T + T + 'const float fV = (pTexBlit == m_pTgTex2) ? (float)m_nTg2H / (float)m_nTg2CapH : (float)m_nTgH / (float)m_nTgCapH;' + R,
   '[TGCAP 14/09] uv = phan dung / cap (RT2 hay RT)')

va(RC,
   'v[i].color = 0xffffffff; v[i].tu = (i & 1) ? 1.0f : 0.0f; v[i].tv = (i & 2) ? 1.0f : 0.0f;',
   'v[i].color = 0xffffffff; v[i].tu = (i & 1) ? fU : 0.0f; v[i].tv = (i & 2) ? fV : 0.0f;' + T + '// [TGCAP 14/09]',
   'v[i].tu = (i & 1) ? fU : 0.0f;')

# ================= [TGCAP 14/09 b] soi cheo phien do nhip: cham san RT (+ RT2) mot lan, kep nac 3500, lenh 11 ZoomToiThieu =================
# - Sau khi cap san (TheGioiRTSan, khung dau vao the gioi): doi dich sang RT + Clear + tra dich = lop SDL_GPU mo pass rong load-op CLEAR
#   (XOANEN 13/09) -> driver cam ket bo nho ngay trong man nap; log Fold 7 12:41: texture lon moi ton 2-3 lan nop 21-43 ms o lan dung dau.
# - RT2 cung cap san + cham khi S3Client bao ZoomToiThieu < 1000 (lenh 11 tu JxLiaCanh Camera_DocMap) va Rep3ZoomNet=1.
# - nMuc (nac cap khi vuot) kep <= 3500 thay vi 6000.
JL = os.path.join(GOC, 'Sources', 'S3Client', 'Platform', 'JxLiaCanh.cpp')

va(RH,
   '    int   m_nTg2CapW, m_nTg2CapH;   // [TGCAP 14/09] co cap RT2 (2 x khung x TheGioiRT2Cap), m_nTg2W/H = phan dung (2 x m_nTgW/H)',
   '    int   m_nTg2CapW, m_nTg2CapH;   // [TGCAP 14/09] co cap RT2 (2 x khung x TheGioiRT2Cap), m_nTg2W/H = phan dung (2 x m_nTgW/H)' + R +
   '    int   m_nTgZoomMin;   // [TGCAP 14/09 b] ZoomToiThieu (phan nghin) S3Client bao qua lenh 11: < 1000 = co phong to -> cap san RT2',
   '[TGCAP 14/09 b] ZoomToiThieu (phan nghin) S3Client bao qua lenh 11')

va(RC,
   'm_nTg2CapW = m_nTg2CapH = 0;' + T + '// [TGCAP 14/09]',
   'm_nTg2CapW = m_nTg2CapH = 0; m_nTgZoomMin = 1000;' + T + '// [TGCAP 14/09]',
   'm_nTgZoomMin = 1000;')

va(RC,
   'static int s_nTgRtCap = 1500, s_nTgRt2Cap = 1150, s_nTgRtSan = 1;',
   'static int s_nTgRtCap = 1500, s_nTgRt2Cap = 1150, s_nTgRtSan = 1;' + T + '// [TGCAP 14/09]' + R +
   'static int s_nTgDaChamRt = 0, s_nTgDaChamRt2 = 0;' + T + '// [TGCAP 14/09 b] da cham san RT / RT2 (pass rong CLEAR) sau khi cap; ve 0 khi huy',
   '[TGCAP 14/09 b] da cham san RT / RT2')

va(RC,
   'm_nTgW = m_nTgH = m_nTgCapW = m_nTgCapH = m_nTgCapKhungW = m_nTgCapKhungH = 0; s_uTgRTVe = 0xFFFFFFFFu;',
   'm_nTgW = m_nTgH = m_nTgCapW = m_nTgCapH = m_nTgCapKhungW = m_nTgCapKhungH = 0; s_uTgRTVe = 0xFFFFFFFFu; s_nTgDaChamRt = s_nTgDaChamRt2 = 0;',
   's_nTgDaChamRt = s_nTgDaChamRt2 = 0;')

va(RC,
   T + 'if (nLenh == 10) return m_nTgDoc;',
   T + 'if (nLenh == 11)' + R +
   T + '{' + T + '// [TGCAP 14/09 b] S3Client (JxLiaCanh Camera_DocMap) bao ZoomToiThieu phan nghin luc vao map / doi cong tac: < 1000 = co the phong to -> cap san RT2 (TheGioiRTSan + Rep3ZoomNet)' + R +
   T + T + 'int nT = nThamSo; if (nT < 500) nT = 500; if (nT > 1000) nT = 1000;' + R +
   T + T + 'm_nTgZoomMin = nT;' + R +
   T + T + 'return m_nTgZoomMin;' + R +
   T + '}' + R +
   T + 'if (nLenh == 10) return m_nTgDoc;',
   '[TGCAP 14/09 b] S3Client (JxLiaCanh Camera_DocMap) bao ZoomToiThieu')

va(RC,
   'while (nMuc < nCan && nMuc < 6000) nMuc += 250;',
   'while (nMuc < nCan && nMuc < 3500) nMuc += 250;' + T + '// [TGCAP 14/09 b] kep 3500 (phien do nhip: 12x diem khung = 30 MB store/khung)',
   'nMuc < 3500')

va(RC,
   T + T + 'if (bDuongCu) { s_uTgVeThat = s_uTgKhung; s_uTgDemThuong++; return 0; }' + T + '// [TGCAP 14/09] da cap san, khung nay van ve thang',
   T + T + 'if (bDuongCu && s_nTgRtSan && m_pTgSurf && (!s_nTgDaChamRt || (!s_nTgDaChamRt2 && m_nTgZoomMin < 1000)))' + R +
   T + T + '{' + T + '// [TGCAP 14/09 b] cham san RT (+ RT2 khi co phong to) MOT lan = doi dich + Clear + tra dich (lop SDL_GPU mo pass rong load-op CLEAR, XOANEN 13/09)' + R +
   T + T + T + '// de driver cam ket bo nho ngay khung dau vao the gioi (log Fold 7 12:41: texture lon moi ton 2-3 lan nop 21-43 ms o lan dung dau)' + R +
   T + T + T + 'LPDIRECT3DSURFACE9 pCu = NULL;' + R +
   T + T + T + 'if (SUCCEEDED(PD3DDEVICE->GetRenderTarget(0, &pCu)) && pCu)' + R +
   T + T + T + '{' + R +
   T + T + T + T + 'if (!s_nTgDaChamRt && SUCCEEDED(PD3DDEVICE->SetRenderTarget(0, m_pTgSurf))) PD3DDEVICE->Clear(0, NULL, D3DCLEAR_TARGET, m_dwTgMauXoa, 1.0f, 0L);' + R +
   T + T + T + T + 's_nTgDaChamRt = 1;' + R +
   T + T + T + T + 'if (!s_nTgDaChamRt2 && m_nTgZoomMin < 1000)' + R +
   T + T + T + T + '{' + R +
   T + T + T + T + T + 's_nTgDaChamRt2 = 1;' + R +
   T + T + T + T + T + 'if (JxTheGioiNet2() && SUCCEEDED(PD3DDEVICE->SetRenderTarget(0, m_pTgSurf2))) PD3DDEVICE->Clear(0, NULL, D3DCLEAR_TARGET, m_dwTgMauXoa, 1.0f, 0L);' + R +
   T + T + T + T + '}' + R +
   T + T + T + T + 'PD3DDEVICE->SetRenderTarget(0, pCu);' + R +
   T + T + T + T + 'Rep3Log("[TGCAP] cham san RT %dx%d%s (zoom toi thieu %d phan nghin)", m_nTgCapW, m_nTgCapH, m_pTgTex2 ? " + RT2" : "", m_nTgZoomMin);' + R +
   T + T + T + '}' + R +
   T + T + T + 'SAFE_RELEASE(pCu);' + R +
   T + T + '}' + R +
   T + T + 'if (bDuongCu) { s_uTgVeThat = s_uTgKhung; s_uTgDemThuong++; return 0; }' + T + '// [TGCAP 14/09] da cap san, khung nay van ve thang',
   '[TGCAP 14/09 b] cham san RT (+ RT2 khi co phong to) MOT lan')

# ---------------- S3Client JxLiaCanh.cpp: lenh 11 = ZoomToiThieu luc doc map ----------------
va(JL,
   T + 's_nZoomDich = nDich;' + R +
   T + 's_fZoomTroi = (float)s_nZoom;' + R +
   T + 'if (s_nZoomDaAp < 0)',
   T + 's_nZoomDich = nDich;' + R +
   T + 's_fZoomTroi = (float)s_nZoom;' + R +
   T + 'Rep3TheGioi(11, (s_nZoomBat && s_nNcZoom) ? s_nZoomToiThieu * 10 : 1000);' + T + '// [TGCAP 14/09 b] bao Represent3 co phong to hay khong (< 1000) -> cap san RT2 o khung dau' + R +
   T + 'if (s_nZoomDaAp < 0)',
   '[TGCAP 14/09 b] bao Represent3 co phong to hay khong')

# ================= [TGCAP 14/09 c] cham san ca khi dang di duong RT (TheGioiRTEp=1 / K=2 ngay tu dau) + log lenh 11 =================
va(RC,
   T + T + 'if (bDuongCu && s_nTgRtSan && m_pTgSurf && (!s_nTgDaChamRt || (!s_nTgDaChamRt2 && m_nTgZoomMin < 1000)))',
   T + T + 'if (s_nTgRtSan && m_pTgSurf && (!s_nTgDaChamRt || (!s_nTgDaChamRt2 && m_nTgZoomMin < 1000)))' + T + '// [TGCAP 14/09 c] ca khi khung nay di duong RT (RT cham thua mot pass rong, RT2 van can cap san)',
   '[TGCAP 14/09 c] ca khi khung nay di duong RT')

va(RC,
   T + T + 'm_nTgZoomMin = nT;' + R +
   T + T + 'return m_nTgZoomMin;',
   T + T + 'if (nT != m_nTgZoomMin) Rep3Log("[TGCAP] zoom toi thieu %d -> %d phan nghin (lenh 11)", m_nTgZoomMin, nT);' + T + '// [TGCAP 14/09 c]' + R +
   T + T + 'm_nTgZoomMin = nT;' + R +
   T + T + 'return m_nTgZoomMin;',
   '[TGCAP] zoom toi thieu %d -> %d phan nghin (lenh 11)')

# ================= [TGCAP 14/09 d] lenh 11 den SAU khung dau (~1 s, Camera_DocMap cho co map) -> khung ve thang phai di tiep de cap san RT2 =================
va(RC,
   T + T + 'if (bDuongCu && (m_pTgTex || !s_nTgRtSan || !s_nTgBat)) { s_uTgVeThat = s_uTgKhung; s_uTgDemThuong++; return 0; }' + T + '// [TGCAP 14/09] chua co RT + TheGioiRTSan: di tiep de CAP SAN roi moi ve thang',
   T + T + 'const bool bCapSan = (s_nTgRtSan && s_nTgBat && (!m_pTgTex || (!s_nTgDaChamRt2 && m_nTgZoomMin < 1000)));' + T + '// [TGCAP 14/09 d] con viec cap san: RT chua co, hoac RT2 chua cap ma S3Client vua bao co phong to (lenh 11 den sau khung dau ~1 s)' + R +
   T + T + 'if (bDuongCu && !bCapSan) { s_uTgVeThat = s_uTgKhung; s_uTgDemThuong++; return 0; }' + T + '// [TGCAP 14/09] chua viec cap san: ve thang ngay',
   '[TGCAP 14/09 d] con viec cap san')

for k in KQ:
    print(k)
