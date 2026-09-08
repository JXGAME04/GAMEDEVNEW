# -*- coding: ascii -*-
"""goi_va_d3d11j_batch_0809.py - [D3D11 08/09 j] GOP LENH VE: cac quad (TRIANGLESTRIP 4 dinh = sprite, bitmap, o mau) lien tiep co
CUNG trang thai (texture/trang atlas, blend, sampler, raster, hang so VS/PS, viewport, layout, stride) duoc gop thanh MOT Draw
(danh sach tam giac). Do 12:38: 0,7 us x 800-3.600 lenh/khung. Trang thai duoc CHUP lai luc dua lenh (R11Applied) nen xa (flush)
sau khi trang thai doi van ap dung dung trang thai cu. Xa khi: lenh ve khong phai quad, Present, Clear, SetRenderTarget, doc lai
khung, Reset, cap nhat texture (UpdateSubresource), thu texture/trang dang duoc lenh cho tham chieu. [Client] Rep3Batch=0 de tat.
Kem: do tre khung toi da 1 -> 2 (do 12:38: latency 1 lam bo ~30 khung/s = hien thi ~32 fps)."""
import io, re, sys
ROOT = r"D:\GAMEDEVNEW_wt_delta\Sources\Represent\Represent3"
def load(name):
    p = ROOT + "\\" + name
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    if name.startswith("D3D9on11"): s = s.replace("\r\n", "\n")
    return p, s
def save(p, s, h0=None):
    if h0 is not None and sum(1 for ch in s if ord(ch) >= 0x80) != h0: print("FAIL byte cao", p); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
def rep(s, old, new, n=1):
    c = s.count(old)
    if c != n: print("FAIL neo %r: thay %d, can %d" % (old[:70], c, n)); sys.exit(1)
    return s.replace(old, new)

# ---------------- D3D9on11i.h
p, s = load("D3D9on11i.h")
if "struct R11Applied" not in s:
    s = rep(s, "struct R11VsCb { float vp[4]; float wvp[16]; float flags[4]; };\nstruct R11PsCb { int st0[4]; int st0b[4]; int st1[4]; int st1b[4]; float at[4]; };\n",
        "struct R11VsCb { float vp[4]; float wvp[16]; float flags[4]; };\n"
        "struct R11PsCb { int st0[4]; int st0b[4]; int st1[4]; int st1b[4]; float at[4]; };\n"
        "// [j] trang thai D3D11 da tinh cho mot lenh ve (so sanh memcmp => khoa gop lenh)\n"
        "struct R11Applied\n{\n"
        "\tID3D11ShaderResourceView* srv[2];\n\tID3D11BlendState* pBlend;\n\tID3D11SamplerState* pSamp[2];\n\tID3D11RasterizerState* pRaster;\n"
        "\tID3D11InputLayout* pIL;\n\tUINT stride;\n\tUINT bScissor;\n\tRECT rcScissor;\n\tD3DVIEWPORT9 vp;\n\tR11VsCb vs;\n\tR11PsCb ps;\n};\n")
    s = rep(s, "\tbool            m_bPipeBound;			// shader + CB + depth da gan sau Present/Reset\n\tvoid            ResetAppliedState();\n",
        "\tbool            m_bPipeBound;			// shader + CB + depth da gan sau Present/Reset\n\tvoid            ResetAppliedState();\n"
        "\t// [j] gop lenh ve\n"
        "\tR11Applied      m_lastApplied; bool m_bAppliedValid;\n"
        "\tstd::vector<BYTE> m_batch; UINT m_batchVerts; R11Applied m_batchState;\n"
        "\tvoid            ComputeApplied(R11Applied& a, ID3D11InputLayout* pIL, UINT stride);\n"
        "\tvoid            ApplyComputed(const R11Applied& a);\n"
        "\tvoid            UploadRing(const BYTE* pData, UINT bytes, UINT stride, UINT* pPos);\n"
        "\tvoid            FlushBatch();\n"
        "\tvoid            FlushIfPending() { if (m_batchVerts) FlushBatch(); }\n")
save(p, s); print("OK D3D9on11i.h")

# ---------------- D3D9on11.h
p, s = load("D3D9on11.h")
if "g_nRep3Batch" not in s:
    s = rep(s, "extern int      g_nRep3Flip;", "extern int      g_nRep3Batch;        // [Client] Rep3Batch: 1 = gop quad cung trang thai thanh mot Draw (mac dinh), 0 = tat\nextern unsigned g_uRep3BatchQuads;   // so quad da gop trong ky\nextern unsigned g_uRep3BatchDraws;   // so Draw thuc te tu cac lo gop\nextern int      g_nRep3Flip;")
save(p, s); print("OK D3D9on11.h")

# ---------------- D3D9on11Dev.cpp
p, s = load("D3D9on11Dev.cpp")
if "FlushBatch" not in s:
    s = rep(s, "unsigned g_uRep3PresentSkip = 0;\n", "unsigned g_uRep3PresentSkip = 0;\nunsigned g_uRep3BatchQuads = 0;\nunsigned g_uRep3BatchDraws = 0;\n")
    # ctor
    s = rep(s, "\tResetAppliedState();\n}\n", "\tm_bAppliedValid = false; m_batchVerts = 0; memset(&m_batchState, 0, sizeof(m_batchState)); memset(&m_lastApplied, 0, sizeof(m_lastApplied));\n\tResetAppliedState();\n}\n")
    # do tre 1 -> 2
    s = rep(s, "{ pDev1->SetMaximumFrameLatency(1); pDev1->Release(); }", "{ pDev1->SetMaximumFrameLatency(2); pDev1->Release(); }")
    # xa truoc Present / Clear / SetRenderTarget / doc lai / Reset / UpdateTexture
    s = rep(s, "\tLock();\n\tLARGE_INTEGER t0, t1; QueryPerformanceCounter(&t0);\n\tUpdateLastFrame();\n", "\tLock();\n\tFlushIfPending();\n\tLARGE_INTEGER t0, t1; QueryPerformanceCounter(&t0);\n\tUpdateLastFrame();\n")
    s = rep(s, "\tif (!(Flags & D3DCLEAR_TARGET)) return D3D_OK;\n\tLock();\n\tBindRenderTarget();\n", "\tif (!(Flags & D3DCLEAR_TARGET)) return D3D_OK;\n\tLock();\n\tFlushIfPending();\n\tBindRenderTarget();\n")
    s = rep(s, "\tif (!pS) return D3DERR_INVALIDCALL;\n\tLock();\n\tR11_SAFE_RELEASE(m_pRt);\n", "\tif (!pS) return D3DERR_INVALIDCALL;\n\tLock();\n\tFlushIfPending();\n\tR11_SAFE_RELEASE(m_pRt);\n")
    s = rep(s, "\tif (!pDst || pDst->m_kind != R11SURF_OFFSCREEN || !pDst->m_pCpu || !m_pLastFrame) return D3DERR_INVALIDCALL;\n\tLock();\n", "\tif (!pDst || pDst->m_kind != R11SURF_OFFSCREEN || !pDst->m_pCpu || !m_pLastFrame) return D3DERR_INVALIDCALL;\n\tLock();\n\tFlushIfPending();\n")
    s = rep(s, "\tif (outFmt != D3DFMT_A8R8G8B8 && outFmt != D3DFMT_X8R8G8B8) { R11Stub(\"GetRenderTargetData dinh dang dich khong 32 bit\"); return D3DERR_INVALIDCALL; }\n\tLock();\n", "\tif (outFmt != D3DFMT_A8R8G8B8 && outFmt != D3DFMT_X8R8G8B8) { R11Stub(\"GetRenderTargetData dinh dang dich khong 32 bit\"); return D3DERR_INVALIDCALL; }\n\tLock();\n\tFlushIfPending();\n")
    s = rep(s, "\tif (!pp) return D3DERR_INVALIDCALL;\n\tLock();\n\tm_pp = *pp;\n", "\tif (!pp) return D3DERR_INVALIDCALL;\n\tLock();\n\tFlushIfPending();\n\tm_pp = *pp;\n")
    s = rep(s, "\tif (pS->m_w != pD->m_w || pS->m_h != pD->m_h || pS->m_fmt != pD->m_fmt) { R11Log(\"UpdateTexture: kich thuoc/dinh dang lech\"); return D3DERR_INVALIDCALL; }\n\tLock();\n", "\tif (pS->m_w != pD->m_w || pS->m_h != pD->m_h || pS->m_fmt != pD->m_fmt) { R11Log(\"UpdateTexture: kich thuoc/dinh dang lech\"); return D3DERR_INVALIDCALL; }\n\tLock();\n\tFlushIfPending();\n")
    # thay vung ResetAppliedState..DrawInternal
    a = s.index("void CDev11::ResetAppliedState()")
    b = s.index("HRESULT CDev11::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)")
    NEW = r'''void CDev11::ResetAppliedState()
{
	m_lastBlendKey = 0xFFFFFFFF; m_lastRasterKey = 0xFFFFFFFF; m_lastSampKey[0] = m_lastSampKey[1] = 0xFFFFFFFF;
	m_lastSrv[0] = m_lastSrv[1] = (ID3D11ShaderResourceView*)1; m_lastIL = NULL; m_lastTopo = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
	memset(&m_lastVp, 0xFF, sizeof(m_lastVp)); m_bPipeBound = false;
	m_bAppliedValid = false;
}

// [j] tinh trang thai D3D11 cho lenh ve hien tai (khong goi context)
void CDev11::ComputeApplied(R11Applied& a, ID3D11InputLayout* pIL, UINT stride)
{
	memset(&a, 0, sizeof(a));
	a.pIL = pIL; a.stride = stride;
	a.vp = m_vp;
	a.bScissor = m_rs[D3DRS_SCISSORTESTENABLE] ? 1 : 0; a.rcScissor = m_scissor;
	a.pRaster = GetRasterState();
	a.pBlend = GetBlendState();
	a.pSamp[0] = GetSamplerState(0); a.pSamp[1] = GetSamplerState(1);
	for (int s = 0; s < 2; s++)
	{
		if (m_tex[s]) { m_tex[s]->PrepareForBind(); a.srv[s] = m_tex[s]->m_pSrv; }
		if (m_pRt && m_pRt->m_pTex && m_tex[s] == m_pRt->m_pTex) a.srv[s] = NULL;
	}
	bool bRhw = ((m_fvf & D3DFVF_POSITION_MASK) == D3DFVF_XYZRHW);
	if (m_bVsDirty || (m_vsCb.flags[0] > 0.5f) != bRhw)
	{
		m_vsCb.vp[0] = (float)(m_vp.Width ? m_vp.Width : 1); m_vsCb.vp[1] = (float)(m_vp.Height ? m_vp.Height : 1); m_vsCb.vp[2] = (float)m_vp.X; m_vsCb.vp[3] = (float)m_vp.Y;
		D3DXMATRIX wv, wvp;
		D3DXMatrixMultiply(&wv, (const D3DXMATRIX*)&m_matWorld, (const D3DXMATRIX*)&m_matView);
		D3DXMatrixMultiply(&wvp, &wv, (const D3DXMATRIX*)&m_matProj);
		memcpy(m_vsCb.wvp, &wvp, sizeof(float) * 16);
		m_vsCb.flags[0] = bRhw ? 1.0f : 0.0f;
		m_bVsDirty = false;
	}
	a.vs = m_vsCb;
	R11PsCb& cb = a.ps;
	cb.st0[0] = (int)m_tss[0][D3DTSS_COLOROP]; cb.st0[1] = (int)m_tss[0][D3DTSS_COLORARG1]; cb.st0[2] = (int)m_tss[0][D3DTSS_COLORARG2]; cb.st0[3] = (int)m_tss[0][D3DTSS_ALPHAOP];
	cb.st0b[0] = (int)m_tss[0][D3DTSS_ALPHAARG1]; cb.st0b[1] = (int)m_tss[0][D3DTSS_ALPHAARG2]; cb.st0b[2] = a.srv[0] ? 1 : 0; cb.st0b[3] = 0;
	cb.st1[0] = (int)m_tss[1][D3DTSS_COLOROP]; cb.st1[1] = (int)m_tss[1][D3DTSS_COLORARG1]; cb.st1[2] = (int)m_tss[1][D3DTSS_COLORARG2]; cb.st1[3] = (int)m_tss[1][D3DTSS_ALPHAOP];
	cb.st1b[0] = (int)m_tss[1][D3DTSS_ALPHAARG1]; cb.st1b[1] = (int)m_tss[1][D3DTSS_ALPHAARG2]; cb.st1b[2] = a.srv[1] ? 1 : 0; cb.st1b[3] = 0;
	cb.at[0] = m_rs[D3DRS_ALPHATESTENABLE] ? 1.0f : 0.0f; cb.at[1] = (float)(m_rs[D3DRS_ALPHAFUNC] & 15); cb.at[2] = (float)(m_rs[D3DRS_ALPHAREF] & 255); cb.at[3] = 0.0f;
}

// [j] gan len context nhung gi khac voi lan gan truoc
void CDev11::ApplyComputed(const R11Applied& a)
{
	if (!m_bRtBound) { BindRenderTarget(); m_bAppliedValid = false; m_bPipeBound = false; }
	if (!m_bPipeBound)
	{
		m_pCtx->VSSetShader(m_pVS, NULL, 0); m_pCtx->PSSetShader(m_pPS, NULL, 0);
		m_pCtx->VSSetConstantBuffers(0, 1, &m_pVsCb); m_pCtx->PSSetConstantBuffers(0, 1, &m_pPsCb);
		m_pCtx->OMSetDepthStencilState(m_pDss, 0);
		m_bPipeBound = true;
	}
	const bool v = m_bAppliedValid;
	const R11Applied& L = m_lastApplied;
	if (!v || memcmp(&a.vp, &L.vp, sizeof(a.vp)) != 0)
	{
		D3D11_VIEWPORT vp; vp.TopLeftX = (float)a.vp.X; vp.TopLeftY = (float)a.vp.Y; vp.Width = (float)a.vp.Width; vp.Height = (float)a.vp.Height; vp.MinDepth = 0.0f; vp.MaxDepth = 1.0f;
		m_pCtx->RSSetViewports(1, &vp);
	}
	if (!v || a.pRaster != L.pRaster || a.bScissor != L.bScissor || memcmp(&a.rcScissor, &L.rcScissor, sizeof(RECT)) != 0)
	{
		if (a.bScissor) { D3D11_RECT r = { a.rcScissor.left, a.rcScissor.top, a.rcScissor.right, a.rcScissor.bottom }; m_pCtx->RSSetScissorRects(1, &r); }
		m_pCtx->RSSetState(a.pRaster);
	}
	if (!v || a.pBlend != L.pBlend) { float bf[4] = { 0, 0, 0, 0 }; m_pCtx->OMSetBlendState(a.pBlend, bf, 0xFFFFFFFF); }
	if (!v || a.pSamp[0] != L.pSamp[0] || a.pSamp[1] != L.pSamp[1]) m_pCtx->PSSetSamplers(0, 2, a.pSamp);
	if (!v || a.srv[0] != L.srv[0] || a.srv[1] != L.srv[1]) m_pCtx->PSSetShaderResources(0, 2, a.srv);
	if (!v || memcmp(&a.vs, &L.vs, sizeof(a.vs)) != 0)
	{
		D3D11_MAPPED_SUBRESOURCE ms;
		if (SUCCEEDED(m_pCtx->Map(m_pVsCb, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms))) { memcpy(ms.pData, &a.vs, sizeof(a.vs)); m_pCtx->Unmap(m_pVsCb, 0); }
	}
	if (!v || memcmp(&a.ps, &L.ps, sizeof(a.ps)) != 0)
	{
		D3D11_MAPPED_SUBRESOURCE ms;
		if (SUCCEEDED(m_pCtx->Map(m_pPsCb, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms))) { memcpy(ms.pData, &a.ps, sizeof(a.ps)); m_pCtx->Unmap(m_pPsCb, 0); }
	}
	if (!v || a.pIL != L.pIL) m_pCtx->IASetInputLayout(a.pIL);
	m_lastApplied = a; m_bAppliedValid = true;
}

void CDev11::ApplyState()
{
	R11Applied a;
	ComputeApplied(a, m_lastIL, m_streamStride);
	ApplyComputed(a);
}

// chep dinh vao ring; tra vi tri byte
void CDev11::UploadRing(const BYTE* pData, UINT bytes, UINT stride, UINT* pPos)
{
	UINT pos = m_ringPos;
	if (stride && (pos % stride)) pos += stride - (pos % stride);
	D3D11_MAP mapType = D3D11_MAP_WRITE_NO_OVERWRITE;
	// [D3D11 08/09 c] dau moi khung (sau Present) hoac het ring: DISCARD de driver cap vung moi.
	if (m_bRingDiscard || pos + bytes > m_ringSize) { pos = 0; mapType = D3D11_MAP_WRITE_DISCARD; m_bRingDiscard = false; }
	D3D11_MAPPED_SUBRESOURCE ms;
	HRESULT hr = m_pCtx->Map(m_pRing, 0, mapType, 0, &ms);
	if (FAILED(hr)) { R11Log("Map ring that bai 0x%08X", (unsigned)hr); *pPos = 0xFFFFFFFF; return; }
	memcpy((BYTE*)ms.pData + pos, pData, bytes);
	m_pCtx->Unmap(m_pRing, 0);
	m_ringPos = pos + bytes;
	*pPos = pos;
}

// [j] ve lo quad dang cho bang MOT Draw voi trang thai da chup luc dua lenh
void CDev11::FlushBatch()
{
	if (!m_batchVerts) return;
	Lock();
	UINT nVerts = m_batchVerts, stride = m_batchState.stride, bytes = nVerts * stride;
	m_batchVerts = 0;
	if (bytes > m_ringSize) { m_batch.clear(); Unlock(); return; }
	UINT pos = 0;
	UploadRing(&m_batch[0], bytes, stride, &pos);
	m_batch.clear();
	if (pos == 0xFFFFFFFF) { Unlock(); return; }
	ApplyComputed(m_batchState);
	ID3D11Buffer* bufs[2] = { m_pRing, m_pDummy }; UINT strides[2] = { stride, 16 }; UINT offs[2] = { pos, 0 };
	m_pCtx->IASetVertexBuffers(0, 2, bufs, strides, offs);
	if (m_lastTopo != D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) { m_pCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); m_lastTopo = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; }
	m_pCtx->Draw(nVerts, 0);
	g_uRep3BatchDraws++;
	Unlock();
}

static UINT R11VertexCount(D3DPRIMITIVETYPE t, UINT n)
{
	switch (t)
	{
	case D3DPT_POINTLIST: return n;
	case D3DPT_LINELIST: return n * 2;
	case D3DPT_LINESTRIP: return n + 1;
	case D3DPT_TRIANGLELIST: return n * 3;
	case D3DPT_TRIANGLESTRIP: return n + 2;
	case D3DPT_TRIANGLEFAN: return n + 2;
	}
	return 0;
}

struct R11DrawTimer
{
	LARGE_INTEGER t0;
	R11DrawTimer() { QueryPerformanceCounter(&t0); }
	~R11DrawTimer() { LARGE_INTEGER t1; QueryPerformanceCounter(&t1); g_dRep3DrawMs += R11Ms(t0, t1); g_uRep3Draws++; }
};

// uv cua texture ao (stage 0) -> uv trong trang atlas, sua tai cho tren dinh da chep
static void R11AtlasUv(BYTE* pV, UINT nVerts, UINT stride, DWORD fvf, CTex11* pTex, float fPage)
{
	if (!pTex || !pTex->m_bVirtual || !pTex->m_pPage || ((fvf & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT) < 1) return;
	UINT uvOff = ((fvf & D3DFVF_POSITION_MASK) == D3DFVF_XYZRHW) ? 16 : 12;
	DWORD pt = fvf & D3DFVF_POSITION_MASK;
	if (pt == D3DFVF_XYZB1) uvOff += 4; else if (pt == D3DFVF_XYZB2) uvOff += 8; else if (pt == D3DFVF_XYZB3) uvOff += 12; else if (pt == D3DFVF_XYZB4) uvOff += 16; else if (pt == D3DFVF_XYZB5) uvOff += 20;
	if (fvf & D3DFVF_NORMAL) uvOff += 12; if (fvf & D3DFVF_PSIZE) uvOff += 4; if (fvf & D3DFVF_DIFFUSE) uvOff += 4; if (fvf & D3DFVF_SPECULAR) uvOff += 4;
	const float sx = (float)pTex->m_w / fPage, sy = (float)pTex->m_h / fPage, ox = (float)pTex->m_ax / fPage, oy = (float)pTex->m_ay / fPage;
	for (UINT i = 0; i < nVerts; i++) { float* uv = (float*)(pV + i * stride + uvOff); uv[0] = uv[0] * sx + ox; uv[1] = uv[1] * sy + oy; }
}

HRESULT CDev11::DrawInternal(D3DPRIMITIVETYPE type, const BYTE* pVerts, UINT nVerts, UINT stride)
{
	R11DrawTimer timer;
	if (!pVerts || nVerts == 0 || stride == 0) return D3DERR_INVALIDCALL;
	ID3D11InputLayout* pIL = GetInputLayout(m_fvf, stride);
	if (!pIL) return D3DERR_INVALIDCALL;
	const float fPage = m_pAtlas ? (float)m_pAtlas->m_pageSize : 1024.0f;
	// ---- [j] quad (strip 4 dinh): gop
	if (g_nRep3Batch && type == D3DPT_TRIANGLESTRIP && nVerts == 4)
	{
		R11Applied a;
		ComputeApplied(a, pIL, stride);
		if (m_batchVerts && (memcmp(&a, &m_batchState, sizeof(a)) != 0 || m_batch.size() + 6 * stride > 2 * 1024 * 1024))
			FlushBatch();
		if (!m_batchVerts) m_batchState = a;
		size_t base = m_batch.size();
		m_batch.resize(base + 6 * stride);
		BYTE* d = &m_batch[base];
		static const int s_idx[6] = { 0, 1, 2, 2, 1, 3 };
		for (int i = 0; i < 6; i++) memcpy(d + i * stride, pVerts + s_idx[i] * stride, stride);
		R11AtlasUv(d, 6, stride, m_fvf, m_tex[0], fPage);
		m_batchVerts += 6;
		g_uRep3BatchQuads++;
		return D3D_OK;
	}
	FlushIfPending();
	// ---- lenh khac: ve ngay
	std::vector<BYTE> tmp;
	if (type == D3DPT_TRIANGLEFAN)
	{
		UINT nTri = nVerts - 2;
		tmp.resize((size_t)nTri * 3 * stride);
		for (UINT i = 0; i < nTri; i++)
		{
			memcpy(&tmp[(i * 3 + 0) * stride], pVerts, stride);
			memcpy(&tmp[(i * 3 + 1) * stride], pVerts + (i + 1) * stride, stride);
			memcpy(&tmp[(i * 3 + 2) * stride], pVerts + (i + 2) * stride, stride);
		}
		pVerts = &tmp[0]; nVerts = nTri * 3; type = D3DPT_TRIANGLELIST;
	}
	else
	{
		tmp.assign(pVerts, pVerts + (size_t)nVerts * stride);
		pVerts = &tmp[0];
	}
	R11AtlasUv(&tmp[0], nVerts, stride, m_fvf, m_tex[0], fPage);
	UINT bytes = nVerts * stride;
	if (bytes > m_ringSize) { R11Log("DrawInternal: %u byte vuot ring", bytes); return D3DERR_INVALIDCALL; }
	UINT pos = 0;
	UploadRing(pVerts, bytes, stride, &pos);
	if (pos == 0xFFFFFFFF) return D3DERR_INVALIDCALL;
	R11Applied a;
	ComputeApplied(a, pIL, stride);
	ApplyComputed(a);
	ID3D11Buffer* bufs[2] = { m_pRing, m_pDummy }; UINT strides[2] = { stride, 16 }; UINT offs[2] = { pos, 0 };
	m_pCtx->IASetVertexBuffers(0, 2, bufs, strides, offs);
	D3D11_PRIMITIVE_TOPOLOGY topo = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	switch (type)
	{
	case D3DPT_POINTLIST: topo = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST; break;
	case D3DPT_LINELIST: topo = D3D11_PRIMITIVE_TOPOLOGY_LINELIST; break;
	case D3DPT_LINESTRIP: topo = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP; break;
	case D3DPT_TRIANGLESTRIP: topo = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP; break;
	default: break;
	}
	if (topo != m_lastTopo) { m_pCtx->IASetPrimitiveTopology(topo); m_lastTopo = topo; }
	m_pCtx->Draw(nVerts, 0);
	return D3D_OK;
}

'''
    s = s[:a] + NEW + s[b:]
save(p, s); print("OK D3D9on11Dev.cpp")

# ---------------- D3D9on11.cpp: xa lo truoc khi cap nhat / thu texture dang duoc tham chieu
p, s = load("D3D9on11.cpp")
if "FlushIfPending" not in s:
    s = rep(s, "HRESULT CTex11::UploadRect(const RECT* prc)\n{\n\tif (!m_pCpu) return D3DERR_INVALIDCALL;\n", "HRESULT CTex11::UploadRect(const RECT* prc)\n{\n\tif (!m_pCpu) return D3DERR_INVALIDCALL;\n\tm_pDev->FlushIfPending();\t// [j] lenh dang cho co the dang doc texture nay\n")
    s = rep(s, "void CTex11::ReleaseGpu()\n{\n", "void CTex11::ReleaseGpu()\n{\n\tif (m_pDev && (m_pSrv || m_pPage)) m_pDev->FlushIfPending();\t// [j] SRV cua texture/trang co the nam trong lo dang cho\n")
save(p, s); print("OK D3D9on11.cpp")

# ---------------- D3D9on11Atlas.cpp: xa truoc khi tra trang
p, s = load("D3D9on11Atlas.cpp")
if "FlushIfPending" not in s:
    s = rep(s, "\t\tif (nEmptySameClass >= 1)\n\t\t{\n\t\t\tfor (size_t i = 0; i < m_pages.size(); i++)\n", "\t\tif (nEmptySameClass >= 1)\n\t\t{\n\t\t\tm_pDev->FlushIfPending();\n\t\t\tfor (size_t i = 0; i < m_pages.size(); i++)\n")
save(p, s); print("OK D3D9on11Atlas.cpp")

# ---------------- KRepresentShell3.cpp: ini + thong ke
p, s = load("KRepresentShell3.cpp"); h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if "g_nRep3Batch" not in s:
    s = rep(s, "int  g_nRep3Tearing   = 0;", "int  g_nRep3Batch     = 1;\t// [D3D11 08/09 j] gop quad cung trang thai thanh mot Draw\r\nint  g_nRep3Tearing   = 0;")
    s = rep(s, '\tg_nRep3Tearing   = Rep3Ini("Rep3Tearing", 0);\t// [D3D11 08/09 f]\r\n', '\tg_nRep3Tearing   = Rep3Ini("Rep3Tearing", 0);\t// [D3D11 08/09 f]\r\n\tg_nRep3Batch     = Rep3Ini("Rep3Batch", 1);\t// [D3D11 08/09 j]\r\n')
    s = rep(s, "| d3d11: present TB %.2f ms bo %u, ve %u lenh %.1f us/lenh\",", "| d3d11: present TB %.2f ms bo %u, ve %u lenh %.1f us/lenh, gop %u quad -> %u Draw\",")
    s = rep(s, "g_uRep3PresentSkip, g_uRep3Draws, g_uRep3Draws ? g_dRep3DrawMs * 1000.0 / g_uRep3Draws : 0.0);", "g_uRep3PresentSkip, g_uRep3Draws, g_uRep3Draws ? g_dRep3DrawMs * 1000.0 / g_uRep3Draws : 0.0, g_uRep3BatchQuads, g_uRep3BatchDraws);")
    s = rep(s, "g_uRep3PresentSkip = 0; g_dRep3DrawMs = 0.0; g_uRep3Draws = 0;", "g_uRep3PresentSkip = 0; g_dRep3DrawMs = 0.0; g_uRep3Draws = 0; g_uRep3BatchQuads = 0; g_uRep3BatchDraws = 0;")
save(p, s, h0); print("OK KRepresentShell3.cpp")

p, s = load("BaseInclude.h"); h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if "g_nRep3Batch" not in s:
    s = rep(s, "extern int  g_nRep3Tearing;     // [D3D11 08/09 f]", "extern int  g_nRep3Batch; extern unsigned g_uRep3BatchQuads; extern unsigned g_uRep3BatchDraws;\t// [D3D11 08/09 j]\r\nextern int  g_nRep3Tearing;     // [D3D11 08/09 f]")
save(p, s, h0); print("OK BaseInclude.h")
print("XONG")
