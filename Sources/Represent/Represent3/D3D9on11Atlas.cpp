// [D3D11 08/09 d] Atlas: gom texture sprite nho vao trang 1024x1024 BGRA8. Do that 08/09: game giu ~30.000 texture GPU, trung binh 14 KB;
// driver (ca D3D9 lan D3D11) ton ~6-9 KB RAM co dinh MOI texture -> 0,7 MB RAM moi MB texture. Gom vao trang: vai tram doi tuong GPU thay vi
// hang van. Texture ao (CTex11::m_bVirtual) = mot o (slot) trong trang; toa do uv duoc nhan/dich khi chep dinh vao ring (DrawInternal).
#include "precompile.h"
#include "BaseInclude.h"
#include "D3D9on11.h"
#include "D3D9on11i.h"

unsigned g_uRep3AtlasPages = 0;
unsigned __int64 g_uRep3AtlasBytes = 0;

static UINT R11Pow2(UINT v, UINT lo, UINT hi)
{
	UINT p = lo;
	while (p < v && p < hi) p <<= 1;
	return p;
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
	case D3DFMT_A8R8G8B8: case D3DFMT_X8R8G8B8: case D3DFMT_A4R4G4B4: case D3DFMT_R5G6B5: case D3DFMT_X1R5G5B5: case D3DFMT_A1R5G5B5: return true;
	default: return false;
	}
}

CAtlasPage* CAtlasMgr::NewPage(UINT binW, UINT binH)
{
	D3D11_TEXTURE2D_DESC td; memset(&td, 0, sizeof(td));
	td.Width = m_pageSize; td.Height = m_pageSize; td.MipLevels = 1; td.ArraySize = 1; td.Format = DXGI_FORMAT_B8G8R8A8_UNORM; td.SampleDesc.Count = 1;
	td.Usage = D3D11_USAGE_DEFAULT; td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	// trang tao kem du lieu 0 (khong de rac): 4 MB tam
	BYTE* pZero = (BYTE*)calloc(1, (size_t)m_pageSize * m_pageSize * 4);
	D3D11_SUBRESOURCE_DATA sr; memset(&sr, 0, sizeof(sr)); sr.pSysMem = pZero; sr.SysMemPitch = m_pageSize * 4;
	ID3D11Texture2D* pTex = NULL;
	HRESULT hr = m_pDev->m_pDev->CreateTexture2D(&td, pZero ? &sr : NULL, &pTex);
	if (pZero) free(pZero);
	if (FAILED(hr) || !pTex) { R11Log("atlas: CreateTexture2D trang %u that bai 0x%08X", m_pageSize, (unsigned)hr); return NULL; }
	ID3D11ShaderResourceView* pSrv = NULL;
	hr = m_pDev->m_pDev->CreateShaderResourceView(pTex, NULL, &pSrv);
	if (FAILED(hr)) { pTex->Release(); R11Log("atlas: CreateShaderResourceView trang that bai 0x%08X", (unsigned)hr); return NULL; }
	CAtlasPage* p = new CAtlasPage();
	p->m_pTex = pTex; p->m_pSrv = pSrv; p->m_binW = binW; p->m_binH = binH;
	p->m_cols = m_pageSize / binW; p->m_rows = m_pageSize / binH; p->m_used = 0;
	UINT n = p->m_cols * p->m_rows;
	p->m_free.reserve(n);
	for (UINT i = n; i > 0; i--) p->m_free.push_back(i - 1);
	m_pages.push_back(p);
	g_uRep3AtlasPages++; g_uRep3AtlasBytes += (unsigned __int64)m_pageSize * m_pageSize * 4;
	return p;
}

bool CAtlasMgr::Alloc(UINT w, UINT h, CAtlasPage** ppPage, UINT* pSlot, UINT* pX, UINT* pY)
{
	// o rong hon anh 1 diem moi chieu (chong tran mau khi loc LINEAR); bin luy thua 2 trong [16, 512]
	UINT binW = R11Pow2(w + 1, 16, 512), binH = R11Pow2(h + 1, 16, 512);
	if (w > binW || h > binH) return false;
	CAtlasPage* pPage = NULL;
	for (size_t i = 0; i < m_pages.size(); i++)
		if (m_pages[i]->m_binW == binW && m_pages[i]->m_binH == binH && !m_pages[i]->m_free.empty()) { pPage = m_pages[i]; break; }
	if (!pPage) pPage = NewPage(binW, binH);
	if (!pPage) return false;
	UINT slot = pPage->m_free.back(); pPage->m_free.pop_back(); pPage->m_used++;
	*ppPage = pPage; *pSlot = slot; *pX = (slot % pPage->m_cols) * binW; *pY = (slot / pPage->m_cols) * binH;
	return true;
}

void CAtlasMgr::Free(CAtlasPage* pPage, UINT slot)
{
	if (!pPage) return;
	pPage->m_free.push_back(slot);
	if (pPage->m_used) pPage->m_used--;
	if (pPage->m_used == 0)
	{
		// giu toi da MOT trang rong moi lop (bin) de khong tao/xoa lien tuc; trang rong thu hai thi tra
		int nEmptySameClass = 0;
		for (size_t i = 0; i < m_pages.size(); i++)
			if (m_pages[i] != pPage && m_pages[i]->m_binW == pPage->m_binW && m_pages[i]->m_binH == pPage->m_binH && m_pages[i]->m_used == 0) nEmptySameClass++;
		if (nEmptySameClass >= 1)
		{
			for (size_t i = 0; i < m_pages.size(); i++)
				if (m_pages[i] == pPage) { m_pages.erase(m_pages.begin() + i); break; }
			if (pPage->m_pSrv) pPage->m_pSrv->Release();
			if (pPage->m_pTex) pPage->m_pTex->Release();
			delete pPage;
			if (g_uRep3AtlasPages) g_uRep3AtlasPages--;
			g_uRep3AtlasBytes -= (unsigned __int64)m_pageSize * m_pageSize * 4;
		}
	}
}
