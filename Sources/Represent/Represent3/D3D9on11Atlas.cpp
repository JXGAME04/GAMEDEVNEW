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
	if (pool != D3DPOOL_DEFAULT && pool != D3DPOOL_MANAGED) return false;	// [MANG 09/09 d] MANAGED (chu KFont3 512x512 A4R4G4B4) cung vao atlas: ban CPU + UploadRect vung ban da ho tro texture ao
	if (usage & (D3DUSAGE_RENDERTARGET | D3DUSAGE_DYNAMIC | D3DUSAGE_DEPTHSTENCIL)) return false;
	if (w == 0 || h == 0 || w > 512 || h > 512) return false;
	switch (fmt)
	{
	case D3DFMT_A8R8G8B8: case D3DFMT_X8R8G8B8: case D3DFMT_A4R4G4B4: case D3DFMT_R5G6B5: case D3DFMT_X1R5G5B5: case D3DFMT_A1R5G5B5: case D3DFMT_A8L8: return true;
	default: return false;
	}
}

// [MANG 09/09 f] atlas theo KHOI co dinh: R8G8 32 lop (64 MB), BGRA8 4 lop (16 MB), toi da 16 khoi moi dinh dang (khe t3+k / t19+k).
// Khong chep mang khi lon, khong cap du; khoi khong con lop dung -> tra VRAM (giu lai 1 khoi rong moi dinh dang de khoi cap/tra lien tuc).
#define R11_KHOI_MAX 16
static UINT R11KhoiLop(DXGI_FORMAT fmt) { return (fmt == DXGI_FORMAT_R8G8_UNORM) ? 32u : 4u; }
CAtlasMang* CAtlasMgr::MangLay(DXGI_FORMAT fmt, UINT* pLop)
{
	for (size_t i = 0; i < m_mang.size(); i++)
	{
		CAtlasMang* pM = m_mang[i];
		if (pM->m_fmt != fmt) continue;
		if (!pM->m_lopTrong.empty()) { *pLop = pM->m_lopTrong.back(); pM->m_lopTrong.pop_back(); return pM; }
		if (pM->m_nDung < pM->m_nLop) { *pLop = pM->m_nDung++; return pM; }
	}
	// het cho: tao khoi moi voi chi so nho nhat chua dung
	bool aDung[R11_KHOI_MAX]; memset(aDung, 0, sizeof(aDung));
	for (size_t i = 0; i < m_mang.size(); i++) if (m_mang[i]->m_fmt == fmt && m_mang[i]->m_nKhoi < R11_KHOI_MAX) aDung[m_mang[i]->m_nKhoi] = true;
	UINT nKhoi = R11_KHOI_MAX; for (UINT k = 0; k < R11_KHOI_MAX; k++) if (!aDung[k]) { nKhoi = k; break; }
	if (nKhoi >= R11_KHOI_MAX) { R11Log("atlas khoi %s: da du 16 khoi, texture moi dung texture rieng", (fmt == DXGI_FORMAT_R8G8_UNORM) ? "R8G8" : "BGRA8"); return NULL; }
	const UINT bpp = (fmt == DXGI_FORMAT_R8G8_UNORM) ? 2 : 4, nLop = R11KhoiLop(fmt);
	D3D11_TEXTURE2D_DESC td; memset(&td, 0, sizeof(td));
	td.Width = m_pageSize; td.Height = m_pageSize; td.MipLevels = 1; td.ArraySize = nLop; td.Format = fmt; td.SampleDesc.Count = 1;
	td.Usage = D3D11_USAGE_DEFAULT; td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	ID3D11Texture2D* pTex = NULL;
	HRESULT hr = m_pDev->m_pDev->CreateTexture2D(&td, NULL, &pTex);	// khong kem du lieu: lop duoc xoa 0 luc NewPage
	if (FAILED(hr) || !pTex) { R11Log("atlas khoi: CreateTexture2D %u lop that bai 0x%08X", nLop, (unsigned)hr); return NULL; }
	D3D11_SHADER_RESOURCE_VIEW_DESC vd; memset(&vd, 0, sizeof(vd));
	vd.Format = fmt; vd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY; vd.Texture2DArray.MostDetailedMip = 0; vd.Texture2DArray.MipLevels = 1; vd.Texture2DArray.FirstArraySlice = 0; vd.Texture2DArray.ArraySize = nLop;
	ID3D11ShaderResourceView* pSrv = NULL;
	hr = m_pDev->m_pDev->CreateShaderResourceView(pTex, &vd, &pSrv);
	if (FAILED(hr) || !pSrv) { pTex->Release(); R11Log("atlas khoi: SRV that bai 0x%08X", (unsigned)hr); return NULL; }
	CAtlasMang* pM = new CAtlasMang(); pM->m_pTex = pTex; pM->m_pSrv = pSrv; pM->m_fmt = fmt; pM->m_bpp = bpp; pM->m_nLop = nLop; pM->m_nDung = 0; pM->m_nKhoi = nKhoi; pM->m_nSuDung = 0;
	m_mang.push_back(pM);
	m_pDev->FlushIfPending(); m_pDev->m_bAppliedValid = false; m_pDev->m_bPipeBound = false;	// gan lai bo khe t3..t34
	g_uRep3AtlasBytes = 0; for (size_t i = 0; i < m_mang.size(); i++) g_uRep3AtlasBytes += (unsigned __int64)m_mang[i]->m_nLop * m_pageSize * m_pageSize * m_mang[i]->m_bpp;
	R11Log("atlas khoi %s #%u: %u lop (%u MB), tong %u MB", bpp == 2 ? "R8G8" : "BGRA8", nKhoi, nLop, (unsigned)(((unsigned __int64)nLop * m_pageSize * m_pageSize * bpp) >> 20), (unsigned)(g_uRep3AtlasBytes >> 20));
	*pLop = pM->m_nDung++;
	return pM;
}

void CAtlasMgr::KhoiXoaNeuTrong(CAtlasMang* pM)
{
	if (!pM || pM->m_nSuDung != 0) return;
	int nRongKhac = 0;
	for (size_t i = 0; i < m_mang.size(); i++) if (m_mang[i] != pM && m_mang[i]->m_fmt == pM->m_fmt && m_mang[i]->m_nSuDung == 0) nRongKhac++;
	if (nRongKhac == 0) return;	// giu lai mot khoi rong moi dinh dang
	m_pDev->FlushIfPending();
	for (size_t i = 0; i < m_pages.size(); i++) if (m_pages[i]->m_pMang == pM) return;	// con trang tro vao (khong nen xay ra khi m_nSuDung == 0)
	for (size_t i = 0; i < m_mang.size(); i++) if (m_mang[i] == pM) { m_mang.erase(m_mang.begin() + i); break; }
	if (pM->m_pSrv) pM->m_pSrv->Release();
	if (pM->m_pTex) pM->m_pTex->Release();
	R11Log("atlas khoi %s #%u: tra VRAM (%u MB)", pM->m_bpp == 2 ? "R8G8" : "BGRA8", pM->m_nKhoi, (unsigned)(((unsigned __int64)pM->m_nLop * m_pageSize * m_pageSize * pM->m_bpp) >> 20));
	delete pM;
	m_pDev->m_bAppliedValid = false; m_pDev->m_bPipeBound = false;
	g_uRep3AtlasBytes = 0; for (size_t i = 0; i < m_mang.size(); i++) g_uRep3AtlasBytes += (unsigned __int64)m_mang[i]->m_nLop * m_pageSize * m_pageSize * m_mang[i]->m_bpp;
}

// [MANG 09/09 b] gan cac khoi atlas co dinh: t3..t18 = R8G8 #0..15, t19..t34 = BGRA8 #0..15 ([MANG 09/09 f])
void CAtlasMgr::GanMang()
{
	ID3D11ShaderResourceView* v[2 * R11_KHOI_MAX]; memset(v, 0, sizeof(v));
	for (size_t i = 0; i < m_mang.size(); i++)
	{
		const UINT k = m_mang[i]->m_nKhoi; if (k >= R11_KHOI_MAX) continue;
		v[(m_mang[i]->m_bpp == 2 ? 0 : R11_KHOI_MAX) + k] = m_mang[i]->m_pSrv;
	}
	m_pDev->m_pCtx->PSSetShaderResources(3, 2 * R11_KHOI_MAX, v);
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
		pM->m_nSuDung++;	// [MANG 09/09 f]
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
			if (pPage->m_pMang)	// [MANG 09/09] tra lop cho khoi; khoi trong -> tra VRAM ([MANG 09/09 f])
			{
				pPage->m_pMang->m_lopTrong.push_back(pPage->m_lop);
				if (pPage->m_pMang->m_nSuDung) pPage->m_pMang->m_nSuDung--;
				CAtlasMang* pM = pPage->m_pMang; pPage->m_pMang = NULL;
				for (size_t i = 0; i < m_pages.size(); i++) if (m_pages[i] == pPage) { m_pages.erase(m_pages.begin() + i); break; }
				KhoiXoaNeuTrong(pM);
			}
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
