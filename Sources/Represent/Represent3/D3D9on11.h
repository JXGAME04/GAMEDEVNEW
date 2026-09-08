// [D3D11 08/09] Lop "D3D9 tren D3D11": cai mot tap con giao dien IDirect3D9 / IDirect3DDevice9 / IDirect3DTexture9 /
// IDirect3DSurface9 / IDirect3DVertexBuffer9 / IDirect3DStateBlock9 bang Direct3D 11, de toan bo Represent3 (va KFont3)
// chay NGUYEN VAN tren D3D11. Ly do: driver D3D9 giu ban sao he thong ~0,8 MB moi MB texture (do 08/09), D3D11 khong
// (25 MB cho 700 MB texture). Bat bang [Client] Rep3Api=11 (mac dinh 9 = D3D9 that, khong doi gi).
#ifndef __D3D9ON11_H__
#define __D3D9ON11_H__

// Tao doi tuong IDirect3D9 chay tren D3D11. Tra NULL neu may khong co D3D11 (Windows 7 khong co DXGI 1.2, card < FL10).
IDirect3D9* Rep3_CreateD3D9on11();

extern int      g_nRep3Api;          // [Client] Rep3Api: 9 (mac dinh) hoac 11
extern int      g_nRep3ApiOn;        // API thuc te dang chay sau D3D_Shell::Create (9 hoac 11)
extern int      g_nRep3D3D11FL;      // feature level D3D11 (0xB000 = 11.0, 0xA000 = 10.0)
extern unsigned g_uRep3GpuTexCount;  // so texture GPU dang song (D3D11)
extern unsigned __int64 g_uRep3GpuTexBytes;   // tong byte texture GPU (D3D11)
extern int      g_nRep3Atlas;        // [Client] Rep3Atlas: 1 = gom texture nho vao trang (mac dinh), 0 = tat
extern unsigned g_uRep3AtlasPages;   // so trang atlas
extern unsigned __int64 g_uRep3AtlasBytes;
extern int      g_nRep3Tearing;      // [Client] Rep3Tearing: 1 = cho phep xe hinh (ALLOW_TEARING) khi flip + vsync 0; mac dinh 0
void Rep3_D3D11VramInfo(unsigned* puUsedMB, unsigned* puBudgetMB);   // VRAM tien trinh (0/0 neu khong phai D3D11)
extern int      g_nRep3Batch;        // [Client] Rep3Batch: 1 = gop quad cung trang thai thanh mot Draw (mac dinh), 0 = tat
extern unsigned g_uRep3BatchQuads;   // so quad da gop trong ky
extern unsigned g_uRep3BatchDraws;   // so Draw thuc te tu cac lo gop
extern int      g_nRep3Waitable;     // [Client] Rep3Waitable: 1 = dung doi tuong cho khung cua DXGI (mac dinh), 0 = tat
extern int      g_nRep3Pal;          // [Client] Rep3Pal: 1 = texture sprite bang mau 2 B/px (mac dinh khi D3D11), 0 = BGRA8
int  Rep3_D3D11PaletteOK();
int  Rep3_D3D11AllocPalette(const unsigned char* pPal24, int nColors);	// hang bang mau (-1 = het)
void Rep3_D3D11FreePalette(int nRow);
void Rep3_D3D11TagPalette(IDirect3DTexture9* pTex, int nRow);
extern int      g_nRep3Buffers;      // [Client] Rep3Buffers: so backbuffer flip (2..4), mac dinh 3
extern int      g_nRep3NoWait;       // [Client] Rep3NoWait: 1 = Present khong cho (hang day thi bo khung); mac dinh 0 = cho nhu D3D9
extern int      g_nRep3Latency;      // [Client] Rep3Latency: so khung toi da cho trinh chieu (1..3), mac dinh 3
extern int      g_nRep3Flip;         // [Client] Rep3Flip: 1 = swapchain flip (mac dinh), 0 = kieu bitblt cu (DISCARD)
extern double   g_dRep3PresentMs;    // tong ms trong Present ke tu lan in thong ke truoc
extern unsigned g_uRep3Presents;     // so lan Present
extern unsigned g_uRep3PresentSkip;  // so khung bo vi hang trinh chieu day (DO_NOT_WAIT)
extern double   g_dRep3DrawMs;       // tong ms trong DrawPrimitive/UP (ca ApplyState + Map)
extern unsigned g_uRep3Draws;        // so lenh ve

#endif
