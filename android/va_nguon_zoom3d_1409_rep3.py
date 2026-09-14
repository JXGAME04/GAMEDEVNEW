# -*- coding: latin-1 -*-
# [ZOOM3D 14/09] Phan Represent3 cua "lam 1-3": PHONG TO (zoom < 100 %) co loc net (soi cheo voi phien do nhip truoc khi day).
#   Sources/Represent/Represent3/KRepresentShell3.cpp/.h (chi trong khoi JX_MOBILE [TG]):
#   - JxTheGioi lenh 4: kep duoi 1000 -> 500 (phong to toi da 2x; JxLiaCanh kep them ZoomToiThieu).
#   - lenh 0: m_nTgZoomRt < 1000 (phong to) van di duong RT (RT = co khung, K = 1) thay vi return 0.
#   - ba cua toa do (CoordinateTransform, ViewPortCoordToSpaceCoord, sampler blit): dieu kien m_nTgZoomRt > 1000 -> != 1000 (cong thuc 1000/m_nTgZoom
#     da tong quat cho ca phong to).
#   - lenh 2: khi m_nTgZoom < 1000 va [Client] Rep3ZoomNet=1: blit hai buoc RT -> RT2 (co 2x, sampler POINT) roi RT2 -> khung (LINEAR) = "sharp
#     bilinear" cho anh diem (vien diem net, chi mep diem hoa tron); Rep3ZoomNet=0 = blit thang LINEAR. RT2 tao lan dau (JxTheGioiNet2), huy cung
#     JxTheGioiHuy. Toa do quad theo VIEWPORT cua render target (CDevGpu::SetRenderTarget dat m_vp = co target) nen khong doi g_nScreenWidth.
# Idempotent, latin-1, neo CRLF. Dung: python android/va_nguon_zoom3d_1409_rep3.py [goc]
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

def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)

def va(p, cu, moi, dau_hieu, so_lan=1):
    s = doc(p)
    if dau_hieu in s:
        KQ.append('bo qua (da co): %s : %s' % (os.path.basename(p), dau_hieu[:50])); return
    n = s.count(cu)
    if n != so_lan:
        raise SystemExit('LOI: neo gap %d lan (can %d) trong %s: %r' % (n, so_lan, p, cu[:90]))
    s2 = s.replace(cu, moi)
    if cao(s2) != cao(s):
        raise SystemExit('LOI: so byte cao doi o %s' % p)
    ghi(p, s2)
    KQ.append('da va: %s : %s' % (os.path.basename(p), dau_hieu[:50]))

RC = os.path.join(GOC, 'Sources', 'Represent', 'Represent3', 'KRepresentShell3.cpp')
RH = os.path.join(GOC, 'Sources', 'Represent', 'Represent3', 'KRepresentShell3.h')

# ---------------- header ----------------
va(RH,
   '    void JxTheGioiHuy();',
   '    void JxTheGioiHuy();' + R +
   '    bool JxTheGioiNet2();\t// [ZOOM3D 14/09] RT2 co 2x cho phong to co loc net (RT -> RT2 POINT -> khung LINEAR)',
   '[ZOOM3D 14/09] RT2 co 2x cho phong to co loc net')

va(RH,
   '    int   m_nTgDoc;      // [LAC 14/09 b]',
   '    LPDIRECT3DTEXTURE9    m_pTgTex2;     // [ZOOM3D 14/09] RT trung gian co 2x RT khi PHONG TO (m_nTgZoom < 1000): RT -> RT2 (POINT) -> khung (LINEAR)' + R +
   '    LPDIRECT3DSURFACE9    m_pTgSurf2;' + R +
   '    int   m_nTg2W, m_nTg2H;' + R +
   '    int   m_nTgDoc;      // [LAC 14/09 b]',
   '[ZOOM3D 14/09] RT trung gian co 2x RT')

# ---------------- cpp: ctor init ----------------
va(RC,
   '\tm_nTgXoay = 0; m_nTgLe = 1000; m_nTgZoomRt = 1000;',
   '\tm_pTgTex2 = NULL; m_pTgSurf2 = NULL; m_nTg2W = m_nTg2H = 0;\t// [ZOOM3D 14/09] RT2 phong to' + R +
   '\tm_nTgXoay = 0; m_nTgLe = 1000; m_nTgZoomRt = 1000;',
   '[ZOOM3D 14/09] RT2 phong to')

# ---------------- cpp: huy + tao RT2 ----------------
va(RC,
   '\tSAFE_RELEASE(m_pTgSurfCu); SAFE_RELEASE(m_pTgSB); SAFE_RELEASE(m_pTgSurf); SAFE_RELEASE(m_pTgTex);' + R +
   '\tm_nTgW = m_nTgH = 0; s_uTgRTVe = 0xFFFFFFFFu;' + R +
   '}' + R,
   '\tSAFE_RELEASE(m_pTgSurfCu); SAFE_RELEASE(m_pTgSB); SAFE_RELEASE(m_pTgSurf); SAFE_RELEASE(m_pTgTex);' + R +
   '\tSAFE_RELEASE(m_pTgSurf2); SAFE_RELEASE(m_pTgTex2); m_nTg2W = m_nTg2H = 0;\t// [ZOOM3D 14/09]' + R +
   '\tm_nTgW = m_nTgH = 0; s_uTgRTVe = 0xFFFFFFFFu;' + R +
   '}' + R + R +
   '// [ZOOM3D 14/09] RT2 = 2 x RT (BGRA8, render target) cho PHONG TO co loc net; [Client] Rep3ZoomNet=0 tat (blit thang LINEAR). Tao lan dau /' + R +
   '// khi RT doi co; huy cung JxTheGioiHuy. Khong tao duoc thi tat luon (khong thu lai moi khung).' + R +
   'bool KRepresentShell3::JxTheGioiNet2()' + R +
   '{' + R +
   '\tstatic int s_nNet = -1;' + R +
   '\tif (s_nNet < 0) s_nNet = Rep3Ini("Rep3ZoomNet", 1) ? 1 : 0;' + R +
   '\tif (!s_nNet || !m_pTgTex || m_nTgW <= 0 || m_nTgH <= 0) return false;' + R +
   '\tconst int nW = m_nTgW * 2, nH = m_nTgH * 2;' + R +
   '\tif (m_pTgTex2 && (m_nTg2W != nW || m_nTg2H != nH)) { SAFE_RELEASE(m_pTgSurf2); SAFE_RELEASE(m_pTgTex2); }' + R +
   '\tif (!m_pTgTex2)' + R +
   '\t{' + R +
   '\t\tm_nTg2W = nW; m_nTg2H = nH;' + R +
   '\t\tif (FAILED(PD3DDEVICE->CreateTexture(nW, nH, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_pTgTex2, NULL)) || !m_pTgTex2' + R +
   '\t\t\t|| FAILED(m_pTgTex2->GetSurfaceLevel(0, &m_pTgSurf2)) || !m_pTgSurf2)' + R +
   '\t\t{' + R +
   '\t\t\tRep3Log("[ZOOM3D] khong tao duoc RT2 %dx%d -> phong to blit thang LINEAR", nW, nH);' + R +
   '\t\t\tSAFE_RELEASE(m_pTgSurf2); SAFE_RELEASE(m_pTgTex2); m_nTg2W = m_nTg2H = 0; s_nNet = 0;' + R +
   '\t\t\treturn false;' + R +
   '\t\t}' + R +
   '\t\tRep3Log("[ZOOM3D] RT2 %dx%d cho phong to co loc net (zoom %d)", nW, nH, m_nTgZoom);' + R +
   '\t}' + R +
   '\treturn true;' + R +
   '}' + R,
   '[ZOOM3D 14/09] RT2 = 2 x RT (BGRA8, render target)')

# ---------------- lenh 4: kep duoi 500 ----------------
va(RC,
   'int nZ = nThamSo; if (nZ < 1000) nZ = 1000; if (nZ > 3000) nZ = 3000;',
   'int nZ = nThamSo; if (nZ < 500) nZ = 500; if (nZ > 3000) nZ = 3000;\t// [ZOOM3D 14/09] < 1000 = phong to (toi da 2x)',
   '[ZOOM3D 14/09] < 1000 = phong to (toi da 2x)')

# ---------------- lenh 0: phong to van di duong RT ----------------
va(RC,
   '\t\tif (bZoom) nK = 1;' + R +
   '\t\tif (!bZoom && (!s_nTgBat || (nK != 2 && s_nTgEp != 1))) { s_uTgVeThat = s_uTgKhung; s_uTgDemThuong++; return 0; }' + R,
   '\t\tconst bool bPhongTo = (m_nTgZoomRt < 1000);\t// [ZOOM3D 14/09] phong to: RT = co khung (K = 1), lenh 2 blit phong to (co loc net neu Rep3ZoomNet)' + R +
   '\t\tif (bZoom || bPhongTo) nK = 1;' + R +
   '\t\tif (!bZoom && !bPhongTo && (!s_nTgBat || (nK != 2 && s_nTgEp != 1))) { s_uTgVeThat = s_uTgKhung; s_uTgDemThuong++; return 0; }' + R,
   '[ZOOM3D 14/09] phong to: RT = co khung (K = 1)')

# ---------------- lenh 2: sampler + blit hai buoc ----------------
va(RC,
   '{ const DWORD dwLoc = (m_nTgZoomRt > 1000 || m_nTgXoay || m_nTgDoc != 1000) ? D3DTEXF_LINEAR : D3DTEXF_POINT;',
   '{ const DWORD dwLoc = (m_nTgZoomRt != 1000 /*[ZOOM3D 14/09] ca phong to*/ || m_nTgXoay || m_nTgDoc != 1000) ? D3DTEXF_LINEAR : D3DTEXF_POINT;',
   '/*[ZOOM3D 14/09] ca phong to*/')

va(RC,
   '\t\tPD3DDEVICE->SetTexture(0, m_pTgTex); PD3DDEVICE->SetTexture(1, NULL);' + R +
   '\t\tPD3DDEVICE->SetFVF(D3DFVF_VERTEX2D);' + R,
   '\t\tLPDIRECT3DTEXTURE9 pTexBlit = m_pTgTex;' + R +
   '\t\tif (m_nTgZoom < 1000 && JxTheGioiNet2())' + R +
   '\t\t{\t// [ZOOM3D 14/09] phong to co loc net: buoc 1 RT -> RT2 (co 2x, POINT = nhan doi diem sac), buoc 2 RT2 -> khung (LINEAR) = "sharp bilinear"' + R +
   '\t\t\tLPDIRECT3DSURFACE9 pDichCu = NULL;' + R +
   '\t\t\tif (SUCCEEDED(PD3DDEVICE->GetRenderTarget(0, &pDichCu)) && pDichCu && SUCCEEDED(PD3DDEVICE->SetRenderTarget(0, m_pTgSurf2)))' + R +
   '\t\t\t{' + R +
   '\t\t\t\tVERTEX2D q[4];' + R +
   '\t\t\t\tPD3DDEVICE->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT); PD3DDEVICE->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);' + R +
   '\t\t\t\tPD3DDEVICE->SetTexture(0, m_pTgTex); PD3DDEVICE->SetTexture(1, NULL);' + R +
   '\t\t\t\tPD3DDEVICE->SetFVF(D3DFVF_VERTEX2D);' + R +
   '\t\t\t\tfor (int i = 0; i < 4; i++)' + R +
   '\t\t\t\t{\t// quad phu kin RT2 (toa do pixel theo viewport cua target = RT2)' + R +
   '\t\t\t\t\tq[i].position = D3DXVECTOR4((i & 1) ? (float)m_nTg2W : 0.f, (i & 2) ? (float)m_nTg2H : 0.f, 100, 1);' + R +
   '\t\t\t\t\tq[i].color = 0xffffffff; q[i].tu = (i & 1) ? 1.0f : 0.0f; q[i].tv = (i & 2) ? 1.0f : 0.0f;' + R +
   '\t\t\t\t}' + R +
   '\t\t\t\tPD3DDEVICE->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, q, sizeof(VERTEX2D));' + R +
   '\t\t\t\tPD3DDEVICE->SetRenderTarget(0, pDichCu);' + R +
   '\t\t\t\tPD3DDEVICE->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR); PD3DDEVICE->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);' + R +
   '\t\t\t\tpTexBlit = m_pTgTex2;' + R +
   '\t\t\t}' + R +
   '\t\t\tSAFE_RELEASE(pDichCu);' + R +
   '\t\t}' + R +
   '\t\tPD3DDEVICE->SetTexture(0, pTexBlit); PD3DDEVICE->SetTexture(1, NULL);' + R +
   '\t\tPD3DDEVICE->SetFVF(D3DFVF_VERTEX2D);' + R,
   '[ZOOM3D 14/09] phong to co loc net: buoc 1 RT -> RT2')

# ---------------- ba cua toa do: > 1000 -> != 1000 ----------------
va(RC,
   'if ((m_nTgZoomRt > 1000 || m_nTgXoay || m_nTgDoc != 1000) && m_nTgTrangThai == 0)',
   'if ((m_nTgZoomRt != 1000 || m_nTgXoay || m_nTgDoc != 1000) && m_nTgTrangThai == 0)',
   'if ((m_nTgZoomRt != 1000 || m_nTgXoay || m_nTgDoc != 1000) && m_nTgTrangThai == 0)', so_lan=2)

va(RC,
   '\t\tif (m_nTgZoomRt > 1000 || m_nTgXoay || m_nTgDoc != 1000)' + R + '\t\t{\t// [ZOOM 13/09] cham tren khung',
   '\t\tif (m_nTgZoomRt != 1000 || m_nTgXoay || m_nTgDoc != 1000)\t// [ZOOM3D 14/09] != 1000 (ca phong to)' + R + '\t\t{\t// [ZOOM 13/09] cham tren khung',
   '[ZOOM3D 14/09] != 1000 (ca phong to)' + R + '\t\t{\t// [ZOOM 13/09] cham tren khung')

print('\n'.join(KQ))
