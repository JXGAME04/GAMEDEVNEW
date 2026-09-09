// [GPU 08/09] D3D9 tren SDL_GPU (Vulkan / Metal / D3D12 qua SDL3) - lop cho mobile, Rep3Api=100. Chi bien dich khi JX_PLATFORM_SDL.
// Cung y tuong voi D3D9on11 (phien D3D11 08/09): Represent3 (KRepresentShell3/TextureRes) giu nguyen, goi IDirect3DDevice9; lop nay
// cai vtable D3D9 tren SDL_GPU. Khac D3D11: SDL_GPU ghi lenh theo command buffer / render pass, tai du lieu (copy pass) phai DI TRUOC
// render pass -> lop nay ghi danh sach lenh ve trong khung (RgCmd) roi phat lai luc Present; texture sua giua khung tao PHIEN BAN moi.
#pragma once
#ifdef JX_PLATFORM_SDL

struct IDirect3D9;
struct IDirect3DTexture9;

IDirect3D9* Rep3_CreateD3D9onGPU();									// NULL neu khong tao duoc thiet bi SDL_GPU
void Rep3_GpuVramInfo(unsigned* puUsedMB, unsigned* puBudgetMB);	// uoc luong (SDL_GPU khong bao VRAM): tong byte texture da tao

// bang mau (nhu Rep3_D3D11*Palette): hang trong atlas 256 x N BGRA8; texture chi so D3DFMT_A8L8 (R = chi so, G = alpha)
int  Rep3_GpuPaletteOK();
int  Rep3_GpuAllocPalette(const unsigned char* pPal24, int nColors);
void Rep3_GpuFreePalette(int nRow);
void Rep3_GpuTagPalette(IDirect3DTexture9* pTex, int nRow);

#endif // JX_PLATFORM_SDL
