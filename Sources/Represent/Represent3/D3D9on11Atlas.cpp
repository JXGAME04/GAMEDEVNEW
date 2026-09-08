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
		if (p->m_pSrv) p->m_pSrv->Release();
		if (p->m_pTex) p->m_pTex->Release();
		delete p;
	}
	m_pages.clear();
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

CAtlasPage* CAtlasMgr::NewPage(UINT binH, DXGI_FORMAT fmt)
{
	UINT bpp = (fmt == DXGI_FORMAT_R8G8_UNORM) ? 2 : 4;	// [r]
	D3D11_TEXTURE2D_DESC td; memset(&td, 0, sizeof(td));
	td.Width = m_pageSize; td.Height = m_pageSize; td.MipLevels = 1; td.ArraySize = 1; td.Format = fmt; td.SampleDesc.Count = 1;
	td.Usage = D3D11_USAGE_DEFAULT; td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	BYTE* pZero = (BYTE*)calloc(1, (size_t)m_pageSize * m_pageSize * bpp);	// trang tao kem du lieu 0 (khong de rac)
	D3D11_SUBRESOURCE_DATA sr; memset(&sr, 0, sizeof(sr)); sr.pSysMem = pZero; sr.SysMemPitch = m_pageSize * bpp;
	ID3D11Texture2D* pTex = NULL;
	HRESULT hr = m_pDev->m_pDev->CreateTexture2D(&td, pZero ? &sr : NULL, &pTex);
	if (pZero) free(pZero);
	if (FAILED(hr) || !pTex) { R11Log("atlas: CreateTexture2D trang %u that bai 0x%08X", m_pageSize, (unsigned)hr); return NULL; }
	ID3D11ShaderResourceView* pSrv = NULL;
	hr = m_pDev->m_pDev->CreateShaderResourceView(pTex, NULL, &pSrv);
	if (FAILED(hr)) { pTex->Release(); R11Log("atlas: CreateShaderResourceView trang that bai 0x%08X", (unsigned)hr); return NULL; }
	CAtlasPage* p = new CAtlasPage();
	p->m_pTex = pTex; p->m_pSrv = pSrv; p->m_fmt = fmt; p->m_bpp = bpp; p->m_binH = binH; p->m_rows = m_pageSize / binH; p->m_used = 0;
	p->m_free.resize(p->m_rows);
	for (UINT r = 0; r < p->m_rows; r++) p->m_free[r].push_back(std::make_pair(0u, m_pageSize));	// ca hang trong
	m_pages.push_back(p);
	g_uRep3AtlasPages++; g_uRep3AtlasBytes += (unsigned __int64)m_pageSize * m_pageSize * bpp;
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
			if (pPage->m_pSrv) pPage->m_pSrv->Release();
			if (pPage->m_pTex) pPage->m_pTex->Release();
			delete pPage;
			if (g_uRep3AtlasPages) g_uRep3AtlasPages--;
			g_uRep3AtlasBytes -= (unsigned __int64)m_pageSize * m_pageSize * pPage->m_bpp;
		}
	}
}
