// [D3D11 08/09 d/i] Atlas: gom texture sprite nho vao trang 1024x1024 BGRA8. Do that 08/09: game giu ~30.000-48.000 texture GPU,
// trung binh 11-14 KB; driver (ca D3D9 lan D3D11) ton ~6-9 KB RAM co dinh MOI texture -> 0,7 MB RAM moi MB texture. Gom vao trang:
// vai tram doi tuong GPU thay vi hang van. Texture ao (CTex11::m_bVirtual) = mot doan trong mot HANG cua trang; uv duoc nhan/dich khi
// chep dinh vao ring (DrawInternal).
// [i] Xep theo KE (shelf): trang thuoc mot lop CHIEU CAO (16/24/32/48/64/96/128/192/256/384/512), moi hang cao H, rong 1024; anh chiem
// mot doan rong w trong hang (first-fit), thu -> tra doan va gop voi doan ke. Do 12:19 voi o co dinh (bin ca hai chieu): 525 MB texture
// chiem 1.132 MB trang (2,16x); xep ke chi phi chieu cao (<= 1,5x) + doan thua cuoi hang.
#include "precompile.h"
#include "BaseInclude.h"
#include "D3D9on11.h"
#include "D3D9on11i.h"

unsigned g_uRep3AtlasPages = 0;
unsigned __int64 g_uRep3AtlasBytes = 0;

static UINT R11Bin(UINT v)
{
	static const UINT s_bins[] = { 16, 24, 32, 48, 64, 96, 128, 192, 256, 384, 512 };
	for (int i = 0; i < (int)(sizeof(s_bins) / sizeof(s_bins[0])); i++)
		if (v <= s_bins[i]) return s_bins[i];
	return 512;
}

CAtlasMgr::CAtlasMgr(CDev11* pDev) { m_pDev = pDev; m_pageSize = 1024; }
CAtlasMgr::~CAtlasMgr() { ReleaseAll(); }

void CAtlasMgr::ReleaseAll()
{
	for (size_t i = 0; i < m_pages.size(); i++)
	{
		CAtlasPage* p = m_pages[i];
		if (!p->m_pMang)	// [MANG 09/09] che do mang: texture/SRV thuoc mang, xoa o MangXoa
		{
			if (p->m_pSrv) p->m_pSrv->Release();
			if (p->m_pTex) p->m_pTex->Release();
		}
		delete p;
	}
	m_pages.clear();
	MangXoa();
	g_uRep3AtlasPages = 0; g_uRep3AtlasBytes = 0;
}

bool CAtlasMgr::Eligible(UINT w, UINT h, DWORD usage, D3DFORMAT fmt, D3DPOOL pool)
{
	if (pool != D3DPOOL_DEFAULT) return false;
	if (usage & (D3DUSAGE_RENDERTARGET | D3DUSAGE_DYNAMIC | D3DUSAGE_DEPTHSTENCIL)) return false;
	if (w == 0 || h == 0 || w > 512 || h > 512) return false;
	switch (fmt)
	{
	case D3DFMT_A8R8G8B8: case D3DFMT_X8R8G8B8: case D3DFMT_A4R4G4B4: case D3DFMT_R5G6B5: case D3DFMT_X1R5G5B5: case D3DFMT_A1R5G5B5: case D3DFMT_A8L8: return true;
	default: return false;
	}
}

// [MANG 09/09] lay mot lop trong cua mang theo dinh dang; het lop -> mang lon hon (+8 lop): chep cac lop cu bang GPU, doi SRV
CAtlasMang* CAtlasMgr::MangLay(DXGI_FORMAT fmt, UINT* pLop)
{
	CAtlasMang* pM = NULL;
	for (size_t i = 0; i < m_mang.size(); i++) if (m_mang[i]->m_fmt == fmt) { pM = m_mang[i]; break; }
	if (!pM)
	{
		pM = new CAtlasMang(); pM->m_pTex = NULL; pM->m_pSrv = NULL; pM->m_fmt = fmt; pM->m_bpp = (fmt == DXGI_FORMAT_R8G8_UNORM) ? 2 : 4; pM->m_nLop = 0; pM->m_nDung = 0;
		m_mang.push_back(pM);
	}
	if (!pM->m_lopTrong.empty()) { *pLop = pM->m_lopTrong.back(); pM->m_lopTrong.pop_back(); return pM; }
	if (pM->m_nDung >= pM->m_nLop)
	{
		const UINT nMoi = pM->m_nLop + 8;
		if (nMoi > 512) { R11Log("atlas mang %s: qua 512 lop", pM->m_bpp == 2 ? "R8G8" : "BGRA8"); return NULL; }
		m_pDev->FlushIfPending();	// lo dang cho con tham chieu SRV cu
		D3D11_TEXTURE2D_DESC td; memset(&td, 0, sizeof(td));
		td.Width = m_pageSize; td.Height = m_pageSize; td.MipLevels = 1; td.ArraySize = nMoi; td.Format = fmt; td.SampleDesc.Count = 1;
		td.Usage = D3D11_USAGE_DEFAULT; td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		const size_t uLop = (size_t)m_pageSize * m_pageSize * pM->m_bpp;
		BYTE* pZero = (BYTE*)calloc(1, uLop);	// moi lop khoi tao 0 (khong de rac)
		std::vector<D3D11_SUBRESOURCE_DATA> sr(nMoi);
		for (UINT k = 0; k < nMoi; k++) { memset(&sr[k], 0, sizeof(sr[k])); sr[k].pSysMem = pZero; sr[k].SysMemPitch = m_pageSize * pM->m_bpp; }
		ID3D11Texture2D* pTex = NULL;
		HRESULT hr = m_pDev->m_pDev->CreateTexture2D(&td, pZero ? &sr[0] : NULL, &pTex);
		if (pZero) free(pZero);
		if (FAILED(hr) || !pTex) { R11Log("atlas mang: CreateTexture2D %u lop that bai 0x%08X", nMoi, (unsigned)hr); return NULL; }
		D3D11_SHADER_RESOURCE_VIEW_DESC vd; memset(&vd, 0, sizeof(vd));
		vd.Format = fmt; vd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY; vd.Texture2DArray.MostDetailedMip = 0; vd.Texture2DArray.MipLevels = 1; vd.Texture2DArray.FirstArraySlice = 0; vd.Texture2DArray.ArraySize = nMoi;
		ID3D11ShaderResourceView* pSrv = NULL;
		hr = m_pDev->m_pDev->CreateShaderResourceView(pTex, &vd, &pSrv);
		if (FAILED(hr) || !pSrv) { pTex->Release(); R11Log("atlas mang: SRV that bai 0x%08X", (unsigned)hr); return NULL; }
		if (pM->m_pTex)
		{
			for (UINT k = 0; k < pM->m_nLop; k++) m_pDev->m_pCtx->CopySubresourceRegion(pTex, k, 0, 0, 0, pM->m_pTex, k, NULL);
			pM->m_pSrv->Release(); pM->m_pTex->Release();
		}
		pM->m_pTex = pTex; pM->m_pSrv = pSrv; pM->m_nLop = nMoi;
		for (size_t i = 0; i < m_pages.size(); i++) if (m_pages[i]->m_pMang == pM) { m_pages[i]->m_pTex = pTex; m_pages[i]->m_pSrv = pSrv; }
		m_pDev->m_bAppliedValid = false;	// SRV cu da huy, dia chi co the trung SRV moi -> ep gan lai
		g_uRep3AtlasBytes = 0; for (size_t i = 0; i < m_mang.size(); i++) g_uRep3AtlasBytes += (unsigned __int64)m_mang[i]->m_nLop * m_pageSize * m_pageSize * m_mang[i]->m_bpp;
		R11Log("atlas mang %s: %u lop (%u MB)", pM->m_bpp == 2 ? "R8G8" : "BGRA8", nMoi, (unsigned)(((unsigned __int64)nMoi * uLop) >> 20));
	}
	*pLop = pM->m_nDung++;
	return pM;
}

void CAtlasMgr::MangXoa()
{
	for (size_t i = 0; i < m_mang.size(); i++) { if (m_mang[i]->m_pSrv) m_mang[i]->m_pSrv->Release(); if (m_mang[i]->m_pTex) m_mang[i]->m_pTex->Release(); delete m_mang[i]; }
	m_mang.clear();
}

CAtlasPage* CAtlasMgr::NewPage(UINT binH, DXGI_FORMAT fmt)
{
	UINT bpp = (fmt == DXGI_FORMAT_R8G8_UNORM) ? 2 : 4;	// [r]
	ID3D11Texture2D* pTex = NULL; ID3D11ShaderResourceView* pSrv = NULL; CAtlasMang* pM = NULL; UINT lop = 0;
	if (g_nRep3AtlasMang)
	{	// [MANG 09/09] trang = mot lop cua mang theo dinh dang; xoa lop ve 0 (lop tra lai co the con anh cu)
		pM = MangLay(fmt, &lop);
		if (!pM) return NULL;
		pTex = pM->m_pTex; pSrv = pM->m_pSrv;
		BYTE* pZero = (BYTE*)calloc(1, (size_t)m_pageSize * m_pageSize * bpp);
		if (pZero) { m_pDev->m_pCtx->UpdateSubresource(pTex, lop, NULL, pZero, m_pageSize * bpp, 0); free(pZero); }
	}
	else
	{
		D3D11_TEXTURE2D_DESC td; memset(&td, 0, sizeof(td));
		td.Width = m_pageSize; td.Height = m_pageSize; td.MipLevels = 1; td.ArraySize = 1; td.Format = fmt; td.SampleDesc.Count = 1;
		td.Usage = D3D11_USAGE_DEFAULT; td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		BYTE* pZero = (BYTE*)calloc(1, (size_t)m_pageSize * m_pageSize * bpp);	// trang tao kem du lieu 0 (khong de rac)
		D3D11_SUBRESOURCE_DATA sr; memset(&sr, 0, sizeof(sr)); sr.pSysMem = pZero; sr.SysMemPitch = m_pageSize * bpp;
		HRESULT hr = m_pDev->m_pDev->CreateTexture2D(&td, pZero ? &sr : NULL, &pTex);
		if (pZero) free(pZero);
		if (FAILED(hr) || !pTex) { R11Log("atlas: CreateTexture2D trang %u that bai 0x%08X", m_pageSize, (unsigned)hr); return NULL; }
		hr = m_pDev->m_pDev->CreateShaderResourceView(pTex, NULL, &pSrv);
		if (FAILED(hr)) { pTex->Release(); R11Log("atlas: CreateShaderResourceView trang that bai 0x%08X", (unsigned)hr); return NULL; }
		g_uRep3AtlasBytes += (unsigned __int64)m_pageSize * m_pageSize * bpp;
	}
	CAtlasPage* p = new CAtlasPage();
	p->m_pTex = pTex; p->m_pSrv = pSrv; p->m_pMang = pM; p->m_lop = lop; p->m_fmt = fmt; p->m_bpp = bpp; p->m_binH = binH; p->m_rows = m_pageSize / binH; p->m_used = 0;
	p->m_free.resize(p->m_rows);
	for (UINT r = 0; r < p->m_rows; r++) p->m_free[r].push_back(std::make_pair(0u, m_pageSize));	// ca hang trong
	m_pages.push_back(p);
	g_uRep3AtlasPages++;
	return p;
}

bool CAtlasMgr::Alloc(UINT w, UINT h, DXGI_FORMAT fmt, CAtlasPage** ppPage, UINT* pX, UINT* pY)
{
	UINT binH = R11Bin(h);
	if (h > binH || w > m_pageSize) return false;
	for (int lan = 0; lan < 2; lan++)
	{
		for (size_t i = 0; i < m_pages.size(); i++)
		{
			CAtlasPage* p = m_pages[i];
			if (p->m_binH != binH || p->m_fmt != fmt) continue;
			for (UINT r = 0; r < p->m_rows; r++)
			{
				std::vector<std::pair<UINT, UINT> >& fr = p->m_free[r];
				for (size_t k = 0; k < fr.size(); k++)
				{
					if (fr[k].second - fr[k].first < w) continue;
					UINT x = fr[k].first;
					fr[k].first += w;
					if (fr[k].first >= fr[k].second) fr.erase(fr.begin() + k);
					p->m_used++;
					*ppPage = p; *pX = x; *pY = r * binH;
					return true;
				}
			}
		}
		if (!NewPage(binH, fmt)) return false;	// lan 2: thu lai voi trang moi
	}
	return false;
}

void CAtlasMgr::Free(CAtlasPage* pPage, UINT x, UINT y, UINT w)
{
	if (!pPage || pPage->m_binH == 0) return;
	UINT r = y / pPage->m_binH;
	if (r >= pPage->m_rows) return;
	std::vector<std::pair<UINT, UINT> >& fr = pPage->m_free[r];
	UINT x0 = x, x1 = x + w;
	size_t k = 0;
	while (k < fr.size() && fr[k].first < x0) k++;
	fr.insert(fr.begin() + k, std::make_pair(x0, x1));
	if (k + 1 < fr.size() && fr[k].second == fr[k + 1].first) { fr[k].second = fr[k + 1].second; fr.erase(fr.begin() + k + 1); }
	if (k > 0 && fr[k - 1].second == fr[k].first) { fr[k - 1].second = fr[k].second; fr.erase(fr.begin() + k); }
	if (pPage->m_used) pPage->m_used--;
	if (pPage->m_used == 0)
	{
		// giu toi da MOT trang rong moi lop chieu cao; trang rong thu hai thi tra lai VRAM
		int nEmptySameClass = 0;
		for (size_t i = 0; i < m_pages.size(); i++)
			if (m_pages[i] != pPage && m_pages[i]->m_binH == pPage->m_binH && m_pages[i]->m_fmt == pPage->m_fmt && m_pages[i]->m_used == 0) nEmptySameClass++;
		if (nEmptySameClass >= 1)
		{
			m_pDev->FlushIfPending();
			for (size_t i = 0; i < m_pages.size(); i++)
				if (m_pages[i] == pPage) { m_pages.erase(m_pages.begin() + i); break; }
			if (pPage->m_pMang)	// [MANG 09/09] tra lop cho mang (VRAM da cap giu nguyen, dung lai cho trang sau)
				pPage->m_pMang->m_lopTrong.push_back(pPage->m_lop);
			else
			{
				if (pPage->m_pSrv) pPage->m_pSrv->Release();
				if (pPage->m_pTex) pPage->m_pTex->Release();
				g_uRep3AtlasBytes -= (unsigned __int64)m_pageSize * m_pageSize * pPage->m_bpp;
			}
			if (g_uRep3AtlasPages) g_uRep3AtlasPages--;
			delete pPage;
		}
	}
}
