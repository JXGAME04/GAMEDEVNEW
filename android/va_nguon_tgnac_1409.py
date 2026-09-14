# -*- coding: latin-1 -*-
# [TGNAC 14/09] De xuat (d) muc 12 (chu chon, phien do nhip do log Fold 7 14:47: GPU 65-85 % khi nhin rong luc dong vi RT 2,25x diem anh + blit):
#   Nhin rong (m_nTgZoom >= [Client] TheGioiRTNac, mac dinh 1100) thi ve the gioi THU NHO thang vao vung co khung x le trong RT (so diem anh ~1x)
#   thay vi ve 1:1 vao RT to roi thu nho luc blit. Cach: lop GPU (D3D9onGPU) them "viewport lo-gic" cho VS (Rep3Gpu_VpLogic: VS chia theo
#   m_nTgW x m_nTgH nhu Core dang ve, viewport that = vung nho) + ep loc palette tuyen tinh trong ps (Rep3Gpu_PalLin, co st0b[3]) de sprite thu
#   nho khong rung hat; blit 1:1 (uv = vung nho / cap). Cua toa do, CHUNET, cull CPU van theo m_nTgW/H (lo-gic) nen khong doi.
#   Phong to (zoom < 1000) va zoom < nguong: duong cu. TheGioiRTNac=0 tat.
# Idempotent, latin-1, neo CRLF. Dung: python android/va_nguon_tgnac_1409.py [goc]
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
RH = os.path.join(GOC, 'Sources', 'Represent', 'Represent3', 'KRepresentShell3.h')
GD = os.path.join(GOC, 'Sources', 'Represent', 'Represent3', 'D3D9onGPUDev.cpp')
GH = os.path.join(GOC, 'Sources', 'Represent', 'Represent3', 'D3D9onGPUi.h')

# ============================================================ A. lop GPU: viewport lo-gic cho VS + ep loc palette ============================================================
va(GH,
   'm_bPalLinForce;',
   'm_bPalLinForce;' + R +
   T + 'int             m_nJxVpLogicW, m_nJxVpLogicH;' + T + '// [TGNAC 14/09] viewport LO-GIC cho VS (0 = theo m_vp): the gioi ve thu nho vao RT co khung khi nhin rong (Rep3Gpu_VpLogic)',
   'm_nJxVpLogicW, m_nJxVpLogicH;')

va(GD,
   'm_pPalTex = NULL; { const char* e = getenv("REP3_PALLIN"); m_bPalLinForce = (e && atoi(e) != 0); }',
   'm_pPalTex = NULL; { const char* e = getenv("REP3_PALLIN"); m_bPalLinForce = (e && atoi(e) != 0); }' + R +
   T + 'm_nJxVpLogicW = m_nJxVpLogicH = 0;' + T + '// [TGNAC 14/09]',
   'm_nJxVpLogicW = m_nJxVpLogicH = 0;')

va(GD,
   'm_vsCb.vp[0] = (float)(m_vp.Width ? m_vp.Width : 1); m_vsCb.vp[1] = (float)(m_vp.Height ? m_vp.Height : 1);',
   'm_vsCb.vp[0] = (float)(m_nJxVpLogicW > 0 ? m_nJxVpLogicW : (m_vp.Width ? m_vp.Width : 1)); m_vsCb.vp[1] = (float)(m_nJxVpLogicH > 0 ? m_nJxVpLogicH : (m_vp.Height ? m_vp.Height : 1));' + T + '/* [TGNAC 14/09] VS chia theo viewport lo-gic (to hon viewport that = ve thu nho) */',
   '[TGNAC 14/09] VS chia theo viewport lo-gic')

va(GD,
   'void Rep3Gpu_DichSeXoa(IDirect3DDevice9* pDev, int bBat)' + R +
   '{' + R +
   T + 'if (pDev && g_nRep3ApiOn == 100) ((CDevGpu*)pDev)->m_bJxDichSeXoa = (bBat != 0);' + R +
   '}' + R,
   'void Rep3Gpu_DichSeXoa(IDirect3DDevice9* pDev, int bBat)' + R +
   '{' + R +
   T + 'if (pDev && g_nRep3ApiOn == 100) ((CDevGpu*)pDev)->m_bJxDichSeXoa = (bBat != 0);' + R +
   '}' + R +
   '// [TGNAC 14/09] KRepresentShell3 JxTheGioi: nhin rong ve thu nho - VS chia theo viewport LO-GIC nW x nH (0 = tat) trong khi viewport that nho hon' + R +
   'void Rep3Gpu_VpLogic(IDirect3DDevice9* pDev, int nW, int nH)' + R +
   '{' + R +
   T + 'if (!pDev || g_nRep3ApiOn != 100) return;' + R +
   T + 'CDevGpu* p = (CDevGpu*)pDev;' + R +
   T + 'if (p->m_nJxVpLogicW != nW || p->m_nJxVpLogicH != nH) { p->m_nJxVpLogicW = nW; p->m_nJxVpLogicH = nH; p->m_bVsDirty = true; }' + R +
   '}' + R +
   '// [TGNAC 14/09] ep ps loc palette tuyen tinh (st0b[3]) cho moi lenh ve trong luc the gioi ve thu nho; sampler that van NEAREST (chi so bang mau)' + R +
   'void Rep3Gpu_PalLin(IDirect3DDevice9* pDev, int bBat)' + R +
   '{' + R +
   T + 'if (pDev && g_nRep3ApiOn == 100) ((CDevGpu*)pDev)->m_bPalLinForce = (bBat != 0);' + R +
   '}' + R,
   'void Rep3Gpu_VpLogic(IDirect3DDevice9* pDev, int nW, int nH)')

# ============================================================ B. Represent3 [TG] ============================================================
va(RH,
   '    int   m_nTgCapW, m_nTgCapH, m_nTgCapKhungW, m_nTgCapKhungH;',
   '    int   m_nTgCapW, m_nTgCapH, m_nTgCapKhungW, m_nTgCapKhungH;' + R +
   '    int   m_nTgPxW, m_nTgPxH, m_bTgNac;   // [TGNAC 14/09] vung DIEM ANH that dang ve trong RT (= m_nTgW/H, hay = khung x le khi nhin rong ve thu nho); m_bTgNac = khung nay ve thu nho',
   '[TGNAC 14/09] vung DIEM ANH that dang ve trong RT')

va(RC,
   'm_nTgCapW = m_nTgCapH = m_nTgCapKhungW = m_nTgCapKhungH = 0; m_nTg2CapW = m_nTg2CapH = 0; m_nTgZoomMin = 1000;' + T + '// [TGCAP 14/09]',
   'm_nTgCapW = m_nTgCapH = m_nTgCapKhungW = m_nTgCapKhungH = 0; m_nTg2CapW = m_nTg2CapH = 0; m_nTgZoomMin = 1000;' + T + '// [TGCAP 14/09]' + R +
   T + 'm_nTgPxW = m_nTgPxH = 0; m_bTgNac = 0;' + T + '// [TGNAC 14/09]',
   'm_nTgPxW = m_nTgPxH = 0; m_bTgNac = 0;')

va(RC,
   'static int s_nTgRtCap = 1500, s_nTgRt2Cap = 1150, s_nTgRtSan = 1;' + T + '// [TGCAP 14/09]',
   'static int s_nTgRtCap = 1500, s_nTgRt2Cap = 1150, s_nTgRtSan = 1;' + T + '// [TGCAP 14/09]' + R +
   'static int s_nTgNac = 1100;' + T + '// [TGNAC 14/09] zoom (phan nghin) tu muc nay ve the gioi THU NHO vao vung khung x le (diem anh ~1x) thay vi RT to; 0 = tat' + R +
   'void Rep3Gpu_VpLogic(IDirect3DDevice9* pDev, int nW, int nH);' + T + '// D3D9onGPUDev.cpp' + R +
   'void Rep3Gpu_PalLin(IDirect3DDevice9* pDev, int bBat);',
   '[TGNAC 14/09] zoom (phan nghin) tu muc nay')

va(RC,
   T + 's_nTgChuNet = Rep3Ini("TheGioiRTChu", 1) ? 1 : 0; Rep3Log("[CHUNET] lop chu the gioi ve sau blit khi zoom / lac: %d (TheGioiRTChu)", s_nTgChuNet);' + T + '// [CHUNET 14/09]',
   T + 's_nTgChuNet = Rep3Ini("TheGioiRTChu", 1) ? 1 : 0; Rep3Log("[CHUNET] lop chu the gioi ve sau blit khi zoom / lac: %d (TheGioiRTChu)", s_nTgChuNet);' + T + '// [CHUNET 14/09]' + R +
   T + 's_nTgNac = Rep3Ini("TheGioiRTNac", 1100); if (s_nTgNac < 0) s_nTgNac = 0; if (s_nTgNac > 0 && s_nTgNac < 1001) s_nTgNac = 1001;' + T + '// [TGNAC 14/09]' + R +
   T + 'Rep3Log("[TGNAC] nhin rong tu %d phan nghin: ve the gioi thu nho vao vung khung x le (TheGioiRTNac; 0 = tat)", s_nTgNac);',
   's_nTgNac = Rep3Ini("TheGioiRTNac"')

# lenh 0: vung diem anh that; cap theo vung do
va(RC,
   T + T + 'const int nRtH = bZoom ? ((g_nScreenHeight * m_nTgZoomRt / 1000 + 1) & ~1) : g_nScreenHeight;' + R,
   T + T + 'const int nRtH = bZoom ? ((g_nScreenHeight * m_nTgZoomRt / 1000 + 1) & ~1) : g_nScreenHeight;' + R +
   T + T + 'const bool bNac = (s_nTgNac > 0 && bZoom && m_nTgZoom >= s_nTgNac && g_nJxTheGioiEp >= 0);' + T + '// [TGNAC 14/09] nhin rong: ve thu nho (he so 1000/zoom) vao vung khung x le' + R +
   T + T + 'const int nPxW = bNac ? ((nRtW * 1000 / m_nTgZoom + 1) & ~1) : nRtW, nPxH = bNac ? ((nRtH * 1000 / m_nTgZoom + 1) & ~1) : nRtH;' + R +
   T + T + 'if ((int)bNac != m_bTgNac) Rep3Log("[TGNAC] zoom %d: %s (vung %dx%d, lo-gic %dx%d)", m_nTgZoom, bNac ? "ve thu nho vao vung khung x le" : "ve 1:1 vao RT to", nPxW, nPxH, nRtW, nRtH);' + R +
   T + T + 'm_bTgNac = bNac ? 1 : 0; m_nTgPxW = nPxW; m_nTgPxH = nPxH;' + R,
   '[TGNAC 14/09] nhin rong: ve thu nho (he so 1000/zoom)')

va(RC,
   T + T + 'if (!m_pTgTex || nRtW > m_nTgCapW || nRtH > m_nTgCapH || g_nScreenWidth != m_nTgCapKhungW || g_nScreenHeight != m_nTgCapKhungH)',
   T + T + 'if (!m_pTgTex || nPxW > m_nTgCapW || nPxH > m_nTgCapH || g_nScreenWidth != m_nTgCapKhungW || g_nScreenHeight != m_nTgCapKhungH)' + T + '// [TGNAC 14/09] cap theo vung diem anh that',
   '[TGNAC 14/09] cap theo vung diem anh that')

va(RC,
   T + T + T + T + 'const int nCanW = (nRtW * 1000 + g_nScreenWidth - 1) / g_nScreenWidth, nCanH = (nRtH * 1000 + g_nScreenHeight - 1) / g_nScreenHeight;',
   T + T + T + T + 'const int nCanW = (nPxW * 1000 + g_nScreenWidth - 1) / g_nScreenWidth, nCanH = (nPxH * 1000 + g_nScreenHeight - 1) / g_nScreenHeight;' + T + '// [TGNAC 14/09] theo vung diem anh',
   '[TGNAC 14/09] theo vung diem anh')

va(RC,
   T + T + T + 'if (m_nTgCapW < nRtW) m_nTgCapW = nRtW; if (m_nTgCapH < nRtH) m_nTgCapH = nRtH;',
   T + T + T + 'if (m_nTgCapW < nPxW) m_nTgCapW = nPxW; if (m_nTgCapH < nPxH) m_nTgCapH = nPxH;' + T + '// [TGNAC 14/09]',
   'if (m_nTgCapW < nPxW) m_nTgCapW = nPxW;')

# lenh 1: viewport that = vung nho, viewport lo-gic = m_nTgW x m_nTgH, ep loc palette
va(RC,
   T + T + 'PD3DDEVICE->Clear(0, NULL, D3DCLEAR_TARGET, m_dwTgMauXoa, 1.0f, 0L);' + R +
   T + T + 'm_nTgLeftKhung = m_nLeft; m_nTgTopKhung = m_nTop; m_nTgKhungW = g_nScreenWidth; m_nTgKhungH = g_nScreenHeight;',
   T + T + 'PD3DDEVICE->Clear(0, NULL, D3DCLEAR_TARGET, m_dwTgMauXoa, 1.0f, 0L);' + R +
   T + T + 'if (m_bTgNac)' + R +
   T + T + '{' + T + '// [TGNAC 14/09] ve THU NHO: viewport that = vung m_nTgPxW x m_nTgPxH (khung x le), VS chia theo lo-gic m_nTgW x m_nTgH (Core van ve toa do 1:1), ps loc palette tuyen tinh' + R +
   T + T + T + 'D3DVIEWPORT9 vp; memset(&vp, 0, sizeof(vp)); vp.Width = (DWORD)m_nTgPxW; vp.Height = (DWORD)m_nTgPxH; vp.MaxZ = 1.0f;' + R +
   T + T + T + 'PD3DDEVICE->SetViewport(&vp);' + R +
   T + T + T + 'Rep3Gpu_VpLogic(PD3DDEVICE, m_nTgW, m_nTgH); Rep3Gpu_PalLin(PD3DDEVICE, 1);' + R +
   T + T + '}' + R +
   T + T + 'm_nTgLeftKhung = m_nLeft; m_nTgTopKhung = m_nTop; m_nTgKhungW = g_nScreenWidth; m_nTgKhungH = g_nScreenHeight;',
   '[TGNAC 14/09] ve THU NHO: viewport that')

# lenh 2: tra lai
va(RC,
   T + T + T + 'PD3DDEVICE->SetRenderTarget(0, m_pTgSurfCu); SAFE_RELEASE(m_pTgSurfCu); m_nTgTrangThai = 0;' + R,
   T + T + T + 'PD3DDEVICE->SetRenderTarget(0, m_pTgSurfCu); SAFE_RELEASE(m_pTgSurfCu); m_nTgTrangThai = 0;' + R +
   T + T + T + 'if (m_bTgNac) { Rep3Gpu_VpLogic(PD3DDEVICE, 0, 0); Rep3Gpu_PalLin(PD3DDEVICE, 0); }' + T + '// [TGNAC 14/09] tra viewport lo-gic + loc palette (SetRenderTarget da tra viewport that)' + R,
   '[TGNAC 14/09] tra viewport lo-gic')

# blit: uv theo vung diem anh that
va(RC,
   'const float fU1 = (float)m_nTgW / (float)m_nTgCapW, fV1 = (float)m_nTgH / (float)m_nTgCapH;',
   'const float fU1 = (float)m_nTgPxW / (float)m_nTgCapW, fV1 = (float)m_nTgPxH / (float)m_nTgCapH;' + T + '/* [TGNAC 14/09] vung diem anh */',
   'fU1 = (float)m_nTgPxW / (float)m_nTgCapW')

va(RC,
   'const float fU = (pTexBlit == m_pTgTex2) ? (float)m_nTg2W / (float)m_nTg2CapW : (float)m_nTgW / (float)m_nTgCapW;',
   'const float fU = (pTexBlit == m_pTgTex2) ? (float)m_nTg2W / (float)m_nTg2CapW : (float)m_nTgPxW / (float)m_nTgCapW;' + T + '/* [TGNAC 14/09] vung diem anh */',
   ': (float)m_nTgPxW / (float)m_nTgCapW;')

va(RC,
   'const float fV = (pTexBlit == m_pTgTex2) ? (float)m_nTg2H / (float)m_nTg2CapH : (float)m_nTgH / (float)m_nTgCapH;',
   'const float fV = (pTexBlit == m_pTgTex2) ? (float)m_nTg2H / (float)m_nTg2CapH : (float)m_nTgPxH / (float)m_nTgCapH;',
   ': (float)m_nTgPxH / (float)m_nTgCapH;')

# huy
va(RC,
   's_uTgRTVe = 0xFFFFFFFFu; s_nTgDaChamRt = s_nTgDaChamRt2 = 0; s_TgChu.clear(); s_TgChuVb.clear();',
   's_uTgRTVe = 0xFFFFFFFFu; s_nTgDaChamRt = s_nTgDaChamRt2 = 0; s_TgChu.clear(); s_TgChuVb.clear(); m_nTgPxW = m_nTgPxH = 0; m_bTgNac = 0;',
   'm_nTgPxW = m_nTgPxH = 0; m_bTgNac = 0;' + T + '// [CHUNET' if False else 's_TgChuVb.clear(); m_nTgPxW = m_nTgPxH = 0; m_bTgNac = 0;')

# ============================================================ B2. [TGNAC 14/09 b] viewport ep GAN VOI texture RT (ghep nen dat doi target giua pha roi quay lai -> SetRenderTarget dat lai viewport = ca texture, goc tren trai den) ============================================================
va(GH,
   T + 'int             m_nJxVpLogicW, m_nJxVpLogicH;',
   T + 'int             m_nJxVpLogicW, m_nJxVpLogicH;' + R +
   T + 'CTexGpu*        m_pJxVpEpTex; int m_nJxVpEpW, m_nJxVpEpH;' + T + '// [TGNAC 14/09 b] viewport EP gan voi texture nay: SetRenderTarget ve no thi viewport = vung ep (khong phai ca texture); viewport lo-gic cung chi ap khi dich = no',
   'm_pJxVpEpTex; int m_nJxVpEpW, m_nJxVpEpH;')

va(GD,
   T + 'm_nJxVpLogicW = m_nJxVpLogicH = 0;' + T + '// [TGNAC 14/09]',
   T + 'm_nJxVpLogicW = m_nJxVpLogicH = 0; m_pJxVpEpTex = NULL; m_nJxVpEpW = m_nJxVpEpH = 0;' + T + '// [TGNAC 14/09]',
   'm_pJxVpEpTex = NULL; m_nJxVpEpW = m_nJxVpEpH = 0;')

va(GD,
   T + 'memset(&m_vp, 0, sizeof(m_vp)); m_vp.Width = pNewTex ? pNewTex->m_w : m_bbW; m_vp.Height = pNewTex ? pNewTex->m_h : m_bbH; m_vp.MaxZ = 1.0f; m_bVsDirty = true;' + R,
   T + 'memset(&m_vp, 0, sizeof(m_vp)); m_vp.Width = pNewTex ? pNewTex->m_w : m_bbW; m_vp.Height = pNewTex ? pNewTex->m_h : m_bbH; m_vp.MaxZ = 1.0f; m_bVsDirty = true;' + R +
   '#ifdef JX_MOBILE' + R +
   T + 'if (pNewTex && pNewTex == m_pJxVpEpTex && m_nJxVpEpW > 0) { m_vp.Width = (DWORD)m_nJxVpEpW; m_vp.Height = (DWORD)m_nJxVpEpH; }' + T + '// [TGNAC 14/09 b] quay lai RT the gioi giua pha (sau ghep nen dat) -> giu vung ep' + R +
   '#endif' + R,
   '[TGNAC 14/09 b] quay lai RT the gioi giua pha')

va(GD,
   'm_vsCb.vp[0] = (float)(m_nJxVpLogicW > 0 ? m_nJxVpLogicW : (m_vp.Width ? m_vp.Width : 1)); m_vsCb.vp[1] = (float)(m_nJxVpLogicH > 0 ? m_nJxVpLogicH : (m_vp.Height ? m_vp.Height : 1));',
   '{ const bool bLg = (m_nJxVpLogicW > 0 && m_pRtTex && m_pRtTex == m_pJxVpEpTex); m_vsCb.vp[0] = (float)(bLg ? m_nJxVpLogicW : (m_vp.Width ? m_vp.Width : 1)); m_vsCb.vp[1] = (float)(bLg ? m_nJxVpLogicH : (m_vp.Height ? m_vp.Height : 1)); }' + T + '/* [TGNAC 14/09 b] lo-gic chi khi dich = RT the gioi (ghep nen dat vao texture vung thi theo co that) */',
   '[TGNAC 14/09 b] lo-gic chi khi dich = RT the gioi')

va(GD,
   '// [TGNAC 14/09] ep ps loc palette tuyen tinh (st0b[3]) cho moi lenh ve trong luc the gioi ve thu nho; sampler that van NEAREST (chi so bang mau)' + R,
   '// [TGNAC 14/09 b] gan viewport EP (nW x nH) voi texture cua be mat pSurf: SetRenderTarget ve no giu vung ep, VS chia theo lo-gic chi khi dich = no; pSurf NULL = bo' + R +
   'void Rep3Gpu_VpEp(IDirect3DDevice9* pDev, IDirect3DSurface9* pSurf, int nW, int nH)' + R +
   '{' + R +
   T + 'if (!pDev || g_nRep3ApiOn != 100) return;' + R +
   T + 'CDevGpu* p = (CDevGpu*)pDev; CSurfGpu* s = (CSurfGpu*)pSurf;' + R +
   T + 'p->m_pJxVpEpTex = (s && s->m_kind == RGSURF_TEX) ? s->m_pTex : NULL; p->m_nJxVpEpW = nW; p->m_nJxVpEpH = nH;' + R +
   T + 'if (p->m_pJxVpEpTex && p->m_pRtTex == p->m_pJxVpEpTex && nW > 0) { p->m_vp.X = p->m_vp.Y = 0; p->m_vp.Width = (DWORD)nW; p->m_vp.Height = (DWORD)nH; }' + R +
   T + 'p->m_bVsDirty = true;' + R +
   '}' + R +
   '// [TGNAC 14/09] ep ps loc palette tuyen tinh (st0b[3]) cho moi lenh ve trong luc the gioi ve thu nho; sampler that van NEAREST (chi so bang mau)' + R,
   'void Rep3Gpu_VpEp(IDirect3DDevice9* pDev, IDirect3DSurface9* pSurf, int nW, int nH)')

va(RC,
   'void Rep3Gpu_PalLin(IDirect3DDevice9* pDev, int bBat);',
   'void Rep3Gpu_PalLin(IDirect3DDevice9* pDev, int bBat);' + R +
   'void Rep3Gpu_VpEp(IDirect3DDevice9* pDev, IDirect3DSurface9* pSurf, int nW, int nH);' + T + '// [TGNAC 14/09 b]',
   'void Rep3Gpu_VpEp(IDirect3DDevice9* pDev, IDirect3DSurface9* pSurf, int nW, int nH);' + T + '// [TGNAC 14/09 b]')

va(RC,
   T + T + T + 'PD3DDEVICE->SetViewport(&vp);' + R +
   T + T + T + 'Rep3Gpu_VpLogic(PD3DDEVICE, m_nTgW, m_nTgH); Rep3Gpu_PalLin(PD3DDEVICE, 1);' + R,
   T + T + T + 'PD3DDEVICE->SetViewport(&vp);' + R +
   T + T + T + 'Rep3Gpu_VpLogic(PD3DDEVICE, m_nTgW, m_nTgH); Rep3Gpu_VpEp(PD3DDEVICE, m_pTgSurf, m_nTgPxW, m_nTgPxH); Rep3Gpu_PalLin(PD3DDEVICE, 1);' + T + '// [TGNAC 14/09 b] vung ep gan voi RT (ghep nen dat doi target giua pha)' + R,
   '[TGNAC 14/09 b] vung ep gan voi RT')

va(RC,
   T + T + T + 'if (m_bTgNac) { Rep3Gpu_VpLogic(PD3DDEVICE, 0, 0); Rep3Gpu_PalLin(PD3DDEVICE, 0); }',
   T + T + T + 'if (m_bTgNac) { Rep3Gpu_VpLogic(PD3DDEVICE, 0, 0); Rep3Gpu_VpEp(PD3DDEVICE, NULL, 0, 0); Rep3Gpu_PalLin(PD3DDEVICE, 0); }',
   'Rep3Gpu_VpEp(PD3DDEVICE, NULL, 0, 0); Rep3Gpu_PalLin(PD3DDEVICE, 0); }')

# ============================================================ B3. [TGNAC 14/09 c] soi cheo phien do nhip: ep loc palette CHI khi dich = RT the gioi; go con tro RT khoi lop GPU truoc khi huy ============================================================
va(GD,
   'cb.st0b[3] = (m_bPalLinForce ||',
   'cb.st0b[3] = ((m_bPalLinForce && m_pRtTex && m_pRtTex == m_pJxVpEpTex) || /* [TGNAC 14/09 c] ep loc chi khi dich = RT the gioi (anh vung nen ghep song mai) */',
   '[TGNAC 14/09 c] ep loc chi khi dich = RT the gioi')

va(RC,
   T + 'if (m_nTgTrangThai == 1 && PD3DDEVICE && m_pTgSurfCu) PD3DDEVICE->SetRenderTarget(0, m_pTgSurfCu);',
   T + 'if (PD3DDEVICE) { Rep3Gpu_VpLogic(PD3DDEVICE, 0, 0); Rep3Gpu_VpEp(PD3DDEVICE, NULL, 0, 0); Rep3Gpu_PalLin(PD3DDEVICE, 0); }' + T + '// [TGNAC 14/09 c] go con tro RT khoi lop GPU TRUOC khi release (Reset / doi cap giua pha: khong de m_pJxVpEpTex treo)' + R +
   T + 'if (m_nTgTrangThai == 1 && PD3DDEVICE && m_pTgSurfCu) PD3DDEVICE->SetRenderTarget(0, m_pTgSurfCu);',
   '[TGNAC 14/09 c] go con tro RT khoi lop GPU')

# ============================================================ C. config (lop ghi de + may ao; dt_v4 do phien do nhip dang sua - ma mac dinh 1100) ============================================================
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
    them_sau(os.path.join(g, 'config.ini'), 'TheGioiRTChu=1',
             '; [TGNAC 14/09] nhin rong tu N phan nghin (1100 = 110 %) thi ve the gioi THU NHO thang vao vung khung x le (so diem anh ~1x) thay vi ve 1:1\n'
             ';   vao RT to roi thu nho luc blit (150 % = 2,25x diem anh, GPU nghen luc dong - log Fold 7 14:47). Sprite loc palette tuyen tinh trong ps. 0 = tat (cach cu).\n'
             'TheGioiRTNac=1100\n', 'TheGioiRTNac=')

for k in KQ:
    print(k)
