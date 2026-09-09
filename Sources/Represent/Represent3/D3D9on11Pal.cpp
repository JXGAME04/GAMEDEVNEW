// [D3D11 08/09 r] Texture bang mau (palette) KHONG MAT MAU: sprite SPR = (chi so 8 bit, alpha 8 bit) theo run RLE; truoc day bung ra
// BGRA8 4 byte/diem. Nay giu 2 byte/diem (D3DFMT_A8L8 = DXGI R8G8: R = chi so, G = alpha) trong trang atlas R8G8, bang mau cua sprite
// (256 mau x RGB) nam trong "atlas bang mau" 256 x 8192 BGRA8 (8 MB); shader tra bang: mau = pal[hang][chi so], alpha = G.
// Ket qua y het duong 8888 (cung bang mau 24 bit), VRAM texture giam mot nua, giai ma nhanh hon (khong bung mau).
#include "precompile.h"
#include "BaseInclude.h"
#include "D3D9on11.h"
#include "D3D9on11i.h"

#define R11_PAL_ROWS 8192

bool CDev11::PalInit()
{
	if (m_pPalTex) return true;
	D3D11_TEXTURE2D_DESC td; memset(&td, 0, sizeof(td));
	td.Width = 256; td.Height = R11_PAL_ROWS; td.MipLevels = 1; td.ArraySize = 1; td.Format = DXGI_FORMAT_B8G8R8A8_UNORM; td.SampleDesc.Count = 1;
	td.Usage = D3D11_USAGE_DEFAULT; td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	HRESULT hr = m_pDev->CreateTexture2D(&td, NULL, &m_pPalTex);
	if (FAILED(hr) || !m_pPalTex) { R11Log("bang mau: CreateTexture2D 256x%d that bai 0x%08X", R11_PAL_ROWS, (unsigned)hr); m_pPalTex = NULL; return false; }
	hr = m_pDev->CreateShaderResourceView(m_pPalTex, NULL, &m_pPalSrv);
	if (FAILED(hr)) { m_pPalTex->Release(); m_pPalTex = NULL; R11Log("bang mau: SRV that bai 0x%08X", (unsigned)hr); return false; }
	m_palFree.reserve(R11_PAL_ROWS);
	for (int i = R11_PAL_ROWS - 1; i >= 0; i--) m_palFree.push_back(i);
	m_pCtx->PSSetShaderResources(2, 1, &m_pPalSrv);
	R11Log("bang mau: atlas 256x%d BGRA8 (%d MB)", R11_PAL_ROWS, R11_PAL_ROWS / 1024);
	return true;
}

void CDev11::PalRelease()
{
	if (m_pPalSrv) { m_pPalSrv->Release(); m_pPalSrv = NULL; }
	if (m_pPalTex) { m_pPalTex->Release(); m_pPalTex = NULL; }
	m_palFree.clear(); m_palDeferred.clear();
}

// hang duoc thu trong khung -> chi dung lai tu khung sau (lo lenh dang cho co the con tra hang cu)
void CDev11::PalFrameEnd()
{
	for (size_t i = 0; i < m_palDeferred.size(); i++) m_palFree.push_back(m_palDeferred[i]);
	m_palDeferred.clear();
}

int CDev11::PalAlloc(const unsigned char* pPal24, int nColors)
{
	if (!pPal24 || nColors <= 0) return -1;
	Lock();
	if (!PalInit() || m_palFree.empty()) { Unlock(); return -1; }
	int row = m_palFree.back(); m_palFree.pop_back();
	DWORD rgb[256];
	if (nColors > 256) nColors = 256;
	for (int i = 0; i < 256; i++)
	{
		if (i < nColors) rgb[i] = 0xFF000000u | ((DWORD)pPal24[i * 3 + 0] << 16) | ((DWORD)pPal24[i * 3 + 1] << 8) | (DWORD)pPal24[i * 3 + 2];
		else rgb[i] = 0xFF000000u;
	}
	D3D11_BOX box; box.left = 0; box.top = row; box.right = 256; box.bottom = row + 1; box.front = 0; box.back = 1;
	m_pCtx->UpdateSubresource(m_pPalTex, 0, &box, rgb, 256 * 4, 0);
	g_uRep3PalRows++;
	Unlock();
	return row;
}

void CDev11::PalFree(int row)
{
	if (row < 0 || row >= R11_PAL_ROWS) return;
	Lock();
	m_palDeferred.push_back(row);
	if (g_uRep3PalRows) g_uRep3PalRows--;
	Unlock();
}

unsigned g_uRep3PalRows = 0;

#ifdef JX_PLATFORM_SDL	// [GPU 08/09] thiet bi SDL_GPU (Rep3Api=100) dung cung API bang mau
#include "D3D9onGPU.h"
int Rep3_D3D11PaletteOK() { if (Rep3_GpuPaletteOK()) return 1; return (g_pRep3Dev11 && g_pRep3Dev11->m_pDev) ? 1 : 0; }
int Rep3_D3D11AllocPalette(const unsigned char* pPal24, int nColors) { if (Rep3_GpuPaletteOK()) return Rep3_GpuAllocPalette(pPal24, nColors); return g_pRep3Dev11 ? g_pRep3Dev11->PalAlloc(pPal24, nColors) : -1; }
void Rep3_D3D11FreePalette(int nRow) { if (Rep3_GpuPaletteOK()) { Rep3_GpuFreePalette(nRow); return; } if (g_pRep3Dev11) g_pRep3Dev11->PalFree(nRow); }
void Rep3_D3D11TagPalette(IDirect3DTexture9* pTex, int nRow) { if (Rep3_GpuPaletteOK()) { Rep3_GpuTagPalette(pTex, nRow); return; } if (pTex) ((CTex11*)pTex)->m_nPalRow = nRow; }
#else
int Rep3_D3D11PaletteOK() { return (g_pRep3Dev11 && g_pRep3Dev11->m_pDev) ? 1 : 0; }
int Rep3_D3D11AllocPalette(const unsigned char* pPal24, int nColors) { return g_pRep3Dev11 ? g_pRep3Dev11->PalAlloc(pPal24, nColors) : -1; }
void Rep3_D3D11FreePalette(int nRow) { if (g_pRep3Dev11) g_pRep3Dev11->PalFree(nRow); }
void Rep3_D3D11TagPalette(IDirect3DTexture9* pTex, int nRow) { if (pTex) ((CTex11*)pTex)->m_nPalRow = nRow; }
#endif	// JX_PLATFORM_SDL [GPU 08/09]
