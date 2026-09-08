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
extern int      g_nRep3Flip;         // [Client] Rep3Flip: 1 = swapchain flip (mac dinh), 0 = kieu bitblt cu (DISCARD)
extern double   g_dRep3PresentMs;    // tong ms trong Present ke tu lan in thong ke truoc
extern unsigned g_uRep3Presents;     // so lan Present
extern double   g_dRep3DrawMs;       // tong ms trong DrawPrimitive/UP (ca ApplyState + Map)
extern unsigned g_uRep3Draws;        // so lenh ve

#endif
