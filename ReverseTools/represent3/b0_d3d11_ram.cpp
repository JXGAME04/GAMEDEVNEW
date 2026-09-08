// b0_d3d11_ram.cpp - [D3D11 08/09] Thi nghiem quyet dinh B0: driver co giu ban sao he thong (RAM) cua texture
// video khi dung D3D11 khong? So voi D3D9 (duong dang chay cua Represent3: SYSTEMMEM + UpdateTexture -> DEFAULT).
// Chay: b0_d3d11_ram.exe <che_do> [so_texture] [canh]
//   che_do: 9      = D3D9 DEFAULT qua SYSTEMMEM+UpdateTexture (nhu game, Rep3Pool=1)
//           9m     = D3D9 MANAGED + LockRect (nhu game Rep3Pool=0)
//           11     = D3D11 USAGE_DEFAULT + du lieu khoi tao
//           11i    = D3D11 USAGE_IMMUTABLE + du lieu khoi tao
//           11u    = D3D11 USAGE_DEFAULT rong + UpdateSubresource (duong shim se dung)
//           11s    = D3D11 USAGE_DEFAULT + CopyResource tu STAGING (staging thu sau khi chep)
// In: PrivateUsage / WorkingSet cua tien trinh + VRAM dung (DXGI) truoc, sau khi tao+ve 30 khung, sau khi thu.
// Build 32 bit (nhu Game.exe): xem build_b0.cmd
#include <windows.h>
#include <psapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <d3d9.h>
#include <d3d11.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

static int g_nTex = 2800, g_nSide = 256;

static void DoRam(const char* pszMoc)
{
    PROCESS_MEMORY_COUNTERS_EX pmc; pmc.cb = sizeof(pmc);
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    UINT64 uVram = 0;
    IDXGIFactory4* pF = NULL;
    if (SUCCEEDED(CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&pF)))
    {
        IDXGIAdapter1* pA = NULL;
        if (SUCCEEDED(pF->EnumAdapters1(0, &pA)))
        {
            IDXGIAdapter3* pA3 = NULL;
            if (SUCCEEDED(pA->QueryInterface(__uuidof(IDXGIAdapter3), (void**)&pA3)))
            {
                DXGI_QUERY_VIDEO_MEMORY_INFO mi;
                if (SUCCEEDED(pA3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &mi))) uVram = mi.CurrentUsage;
                pA3->Release();
            }
            pA->Release();
        }
        pF->Release();
    }
    printf("[RAM] %-34s private %6.0f MB | working set %6.0f MB | VRAM tien trinh %6.0f MB\n",
        pszMoc, pmc.PrivateUsage / 1048576.0, pmc.WorkingSetSize / 1048576.0, uVram / 1048576.0);
    fflush(stdout);
}

static LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l) { return DefWindowProcA(h, m, w, l); }
static HWND TaoCuaSo()
{
    WNDCLASSA wc; memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = WndProc; wc.hInstance = GetModuleHandleA(NULL); wc.lpszClassName = "b0ram";
    RegisterClassA(&wc);
    return CreateWindowA("b0ram", "b0", WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, NULL, wc.hInstance, NULL);
}

static void DienAnh(BYTE* p, int nPitch, int i)
{
    for (int y = 0; y < g_nSide; y++)
    {
        DWORD* row = (DWORD*)(p + y * nPitch);
        for (int x = 0; x < g_nSide; x++) row[x] = 0xFF000000 | ((x * 7 + i) & 0xFF) | (((y * 3 + i) & 0xFF) << 8) | (((x ^ y) & 0xFF) << 16);
    }
}

// ---------------------------------------------------------------- D3D9
struct V9 { float x, y, z, rhw; DWORD c; float u, v; };
static int Chay9(HWND hWnd, bool bManaged)
{
    IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (!pD3D) { printf("Direct3DCreate9 that bai\n"); return 1; }
    D3DPRESENT_PARAMETERS pp; memset(&pp, 0, sizeof(pp));
    pp.Windowed = TRUE; pp.SwapEffect = D3DSWAPEFFECT_DISCARD; pp.BackBufferFormat = D3DFMT_X8R8G8B8;
    pp.BackBufferWidth = 800; pp.BackBufferHeight = 600; pp.hDeviceWindow = hWnd; pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    IDirect3DDevice9* pDev = NULL;
    if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &pDev)))
    { printf("CreateDevice9 that bai\n"); return 1; }
    DoRam("9: sau tao device");
    IDirect3DTexture9** pp9 = new IDirect3DTexture9*[g_nTex];
    for (int i = 0; i < g_nTex; i++)
    {
        pp9[i] = NULL;
        if (bManaged)
        {
            if (FAILED(pDev->CreateTexture(g_nSide, g_nSide, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pp9[i], NULL))) { printf("CreateTexture MANAGED hong o %d\n", i); break; }
            D3DLOCKED_RECT lr; if (SUCCEEDED(pp9[i]->LockRect(0, &lr, NULL, 0))) { DienAnh((BYTE*)lr.pBits, lr.Pitch, i); pp9[i]->UnlockRect(0); }
        }
        else
        {
            IDirect3DTexture9* pSys = NULL;
            if (FAILED(pDev->CreateTexture(g_nSide, g_nSide, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pSys, NULL))) { printf("CreateTexture SYSTEMMEM hong o %d\n", i); break; }
            D3DLOCKED_RECT lr; if (SUCCEEDED(pSys->LockRect(0, &lr, NULL, 0))) { DienAnh((BYTE*)lr.pBits, lr.Pitch, i); pSys->UnlockRect(0); }
            if (FAILED(pDev->CreateTexture(g_nSide, g_nSide, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pp9[i], NULL))) { printf("CreateTexture DEFAULT hong o %d\n", i); pSys->Release(); break; }
            pDev->UpdateTexture(pSys, pp9[i]);
            pSys->Release();
        }
    }
    DoRam(bManaged ? "9m: sau tao texture MANAGED" : "9: sau tao texture DEFAULT (UpdateTexture)");
    pDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    pDev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
    for (int f = 0; f < 30; f++)
    {
        pDev->Clear(0, NULL, D3DCLEAR_TARGET, 0xFF202020, 1.0f, 0);
        pDev->BeginScene();
        for (int i = 0; i < g_nTex; i++)
        {
            if (!pp9[i]) continue;
            float x = (float)((i * 13) % 780), y = (float)((i * 29) % 580);
            V9 v[4] = { {x, y, 0, 1, 0xFFFFFFFF, 0, 0}, {x + 16, y, 0, 1, 0xFFFFFFFF, 1, 0}, {x, y + 16, 0, 1, 0xFFFFFFFF, 0, 1}, {x + 16, y + 16, 0, 1, 0xFFFFFFFF, 1, 1} };
            pDev->SetTexture(0, pp9[i]);
            pDev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(V9));
        }
        pDev->EndScene();
        pDev->Present(NULL, NULL, NULL, NULL);
    }
    Sleep(500);
    DoRam("9: sau ve 30 khung");
    for (int i = 0; i < g_nTex; i++) if (pp9[i]) pp9[i]->Release();
    delete[] pp9;
    Sleep(500);
    DoRam("9: sau thu texture");
    pDev->Release(); pD3D->Release();
    return 0;
}

// ---------------------------------------------------------------- D3D11
static const char* g_szHlsl =
"struct VSI { float2 p : POSITION; float2 t : TEXCOORD0; };\n"
"struct VSO { float4 p : SV_Position; float2 t : TEXCOORD0; };\n"
"VSO VS(VSI i) { VSO o; o.p = float4(i.p.x / 400.0 - 1.0, 1.0 - i.p.y / 300.0, 0, 1); o.t = i.t; return o; }\n"
"Texture2D tex : register(t0); SamplerState smp : register(s0);\n"
"float4 PS(VSO i) : SV_Target { return tex.Sample(smp, i.t); }\n";
struct V11 { float x, y, u, v; };

static int Chay11(HWND hWnd, const char* pszMode)
{
    DXGI_SWAP_CHAIN_DESC sd; memset(&sd, 0, sizeof(sd));
    sd.BufferCount = 2; sd.BufferDesc.Width = 800; sd.BufferDesc.Height = 600; sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; sd.OutputWindow = hWnd; sd.SampleDesc.Count = 1; sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    ID3D11Device* pDev = NULL; ID3D11DeviceContext* pCtx = NULL; IDXGISwapChain* pSC = NULL; D3D_FEATURE_LEVEL fl;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0, D3D11_SDK_VERSION, &sd, &pSC, &pDev, &fl, &pCtx);
    if (FAILED(hr)) { printf("D3D11CreateDeviceAndSwapChain that bai %08X\n", hr); return 1; }
    printf("D3D11 feature level %X\n", fl);
    DoRam("11: sau tao device");
    ID3D11Texture2D* pBB = NULL; pSC->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBB);
    ID3D11RenderTargetView* pRTV = NULL; pDev->CreateRenderTargetView(pBB, NULL, &pRTV); pBB->Release();
    ID3DBlob* pVS = NULL; ID3DBlob* pPS = NULL; ID3DBlob* pErr = NULL;
    if (FAILED(D3DCompile(g_szHlsl, strlen(g_szHlsl), NULL, NULL, NULL, "VS", "vs_4_0", 0, 0, &pVS, &pErr))) { printf("VS: %s\n", pErr ? (char*)pErr->GetBufferPointer() : "?"); return 1; }
    if (FAILED(D3DCompile(g_szHlsl, strlen(g_szHlsl), NULL, NULL, NULL, "PS", "ps_4_0", 0, 0, &pPS, &pErr))) { printf("PS: %s\n", pErr ? (char*)pErr->GetBufferPointer() : "?"); return 1; }
    ID3D11VertexShader* pVSO = NULL; ID3D11PixelShader* pPSO = NULL; ID3D11InputLayout* pIL = NULL;
    pDev->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), NULL, &pVSO);
    pDev->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), NULL, &pPSO);
    D3D11_INPUT_ELEMENT_DESC ie[2] = { {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}, {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0} };
    pDev->CreateInputLayout(ie, 2, pVS->GetBufferPointer(), pVS->GetBufferSize(), &pIL);
    D3D11_BUFFER_DESC bd; memset(&bd, 0, sizeof(bd)); bd.ByteWidth = sizeof(V11) * 4; bd.Usage = D3D11_USAGE_DYNAMIC; bd.BindFlags = D3D11_BIND_VERTEX_BUFFER; bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    ID3D11Buffer* pVB = NULL; pDev->CreateBuffer(&bd, NULL, &pVB);
    D3D11_SAMPLER_DESC smd; memset(&smd, 0, sizeof(smd)); smd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; smd.AddressU = smd.AddressV = smd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    ID3D11SamplerState* pSmp = NULL; pDev->CreateSamplerState(&smd, &pSmp);

    BYTE* pAnh = (BYTE*)malloc(g_nSide * g_nSide * 4);
    ID3D11Texture2D** ppT = new ID3D11Texture2D*[g_nTex]; ID3D11ShaderResourceView** ppS = new ID3D11ShaderResourceView*[g_nTex];
    D3D11_TEXTURE2D_DESC td; memset(&td, 0, sizeof(td));
    td.Width = td.Height = g_nSide; td.MipLevels = 1; td.ArraySize = 1; td.Format = DXGI_FORMAT_B8G8R8A8_UNORM; td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_DEFAULT; td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    if (!strcmp(pszMode, "11i")) td.Usage = D3D11_USAGE_IMMUTABLE;
    if (!strcmp(pszMode, "11m")) { td.Usage = D3D11_USAGE_DYNAMIC; td.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; }
    // 11r: MOT texture STAGING dung lai (vong dem) -> Map WRITE -> CopyResource sang DEFAULT
    ID3D11Texture2D* pStRing = NULL;
    if (!strcmp(pszMode, "11r"))
    {
        D3D11_TEXTURE2D_DESC ts = td; ts.Usage = D3D11_USAGE_STAGING; ts.BindFlags = 0; ts.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        pDev->CreateTexture2D(&ts, NULL, &pStRing);
    }
    for (int i = 0; i < g_nTex; i++)
    {
        ppT[i] = NULL; ppS[i] = NULL;
        DienAnh(pAnh, g_nSide * 4, i);
        D3D11_SUBRESOURCE_DATA sr; sr.pSysMem = pAnh; sr.SysMemPitch = g_nSide * 4; sr.SysMemSlicePitch = 0;
        if (!strcmp(pszMode, "11u") || !strcmp(pszMode, "11uf"))
        {
            hr = pDev->CreateTexture2D(&td, NULL, &ppT[i]);
            if (SUCCEEDED(hr)) pCtx->UpdateSubresource(ppT[i], 0, NULL, pAnh, g_nSide * 4, 0);
            if (!strcmp(pszMode, "11uf") && (i % 16) == 15) pCtx->Flush();
        }
        else if (!strcmp(pszMode, "11r"))
        {
            hr = pDev->CreateTexture2D(&td, NULL, &ppT[i]);
            D3D11_MAPPED_SUBRESOURCE ms;
            if (SUCCEEDED(hr) && pStRing && SUCCEEDED(pCtx->Map(pStRing, 0, D3D11_MAP_WRITE, 0, &ms)))
            {
                for (int y = 0; y < g_nSide; y++) memcpy((BYTE*)ms.pData + y * ms.RowPitch, pAnh + y * g_nSide * 4, g_nSide * 4);
                pCtx->Unmap(pStRing, 0);
                pCtx->CopyResource(ppT[i], pStRing);
            }
        }
        else if (!strcmp(pszMode, "11m"))
        {
            hr = pDev->CreateTexture2D(&td, NULL, &ppT[i]);
            D3D11_MAPPED_SUBRESOURCE ms;
            if (SUCCEEDED(hr) && SUCCEEDED(pCtx->Map(ppT[i], 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)))
            {
                for (int y = 0; y < g_nSide; y++) memcpy((BYTE*)ms.pData + y * ms.RowPitch, pAnh + y * g_nSide * 4, g_nSide * 4);
                pCtx->Unmap(ppT[i], 0);
            }
        }
        else if (!strcmp(pszMode, "11s"))
        {
            D3D11_TEXTURE2D_DESC ts = td; ts.Usage = D3D11_USAGE_STAGING; ts.BindFlags = 0; ts.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            ID3D11Texture2D* pSt = NULL;
            hr = pDev->CreateTexture2D(&ts, &sr, &pSt);
            if (SUCCEEDED(hr)) hr = pDev->CreateTexture2D(&td, NULL, &ppT[i]);
            if (SUCCEEDED(hr)) { pCtx->CopyResource(ppT[i], pSt); }
            if (pSt) pSt->Release();
        }
        else
            hr = pDev->CreateTexture2D(&td, &sr, &ppT[i]);
        if (FAILED(hr)) { printf("CreateTexture2D hong o %d: %08X\n", i, hr); break; }
        pDev->CreateShaderResourceView(ppT[i], NULL, &ppS[i]);
    }
    free(pAnh);
    if (pStRing) pStRing->Release();
    char sz[96]; sprintf(sz, "%s: sau tao %d texture", pszMode, g_nTex); DoRam(sz);
    for (int f = 0; f < 30; f++)
    {
        float cl[4] = { 0.1f, 0.1f, 0.1f, 1 };
        pCtx->OMSetRenderTargets(1, &pRTV, NULL); pCtx->ClearRenderTargetView(pRTV, cl);
        D3D11_VIEWPORT vp = { 0, 0, 800, 600, 0, 1 }; pCtx->RSSetViewports(1, &vp);
        pCtx->IASetInputLayout(pIL); pCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        pCtx->VSSetShader(pVSO, NULL, 0); pCtx->PSSetShader(pPSO, NULL, 0); pCtx->PSSetSamplers(0, 1, &pSmp);
        UINT st = sizeof(V11), of = 0; pCtx->IASetVertexBuffers(0, 1, &pVB, &st, &of);
        for (int i = 0; i < g_nTex; i++)
        {
            if (!ppS[i]) continue;
            float x = (float)((i * 13) % 780), y = (float)((i * 29) % 580);
            V11 v[4] = { {x, y, 0, 0}, {x + 16, y, 1, 0}, {x, y + 16, 0, 1}, {x + 16, y + 16, 1, 1} };
            D3D11_MAPPED_SUBRESOURCE ms; if (SUCCEEDED(pCtx->Map(pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms))) { memcpy(ms.pData, v, sizeof(v)); pCtx->Unmap(pVB, 0); }
            pCtx->PSSetShaderResources(0, 1, &ppS[i]);
            pCtx->Draw(4, 0);
        }
        pSC->Present(0, 0);
    }
    Sleep(500);
    sprintf(sz, "%s: sau ve 30 khung", pszMode); DoRam(sz);
    for (int i = 0; i < g_nTex; i++) { if (ppS[i]) ppS[i]->Release(); if (ppT[i]) ppT[i]->Release(); }
    delete[] ppT; delete[] ppS;
    pCtx->ClearState(); pCtx->Flush(); Sleep(500);
    sprintf(sz, "%s: sau thu texture", pszMode); DoRam(sz);
    pSmp->Release(); pVB->Release(); pIL->Release(); pVSO->Release(); pPSO->Release(); pVS->Release(); pPS->Release(); pRTV->Release(); pSC->Release(); pCtx->Release(); pDev->Release();
    return 0;
}

int main(int argc, char** argv)
{
    const char* pszMode = argc > 1 ? argv[1] : "11";
    if (argc > 2) g_nTex = atoi(argv[2]);
    if (argc > 3) g_nSide = atoi(argv[3]);
    printf("che do %s, %d texture %dx%d BGRA = %.0f MB\n", pszMode, g_nTex, g_nSide, g_nSide, g_nTex * (double)g_nSide * g_nSide * 4 / 1048576.0);
    HWND hWnd = TaoCuaSo();
    DoRam("khoi dong");
    int r;
    if (!strcmp(pszMode, "9")) r = Chay9(hWnd, false);
    else if (!strcmp(pszMode, "9m")) r = Chay9(hWnd, true);
    else r = Chay11(hWnd, pszMode);
    DestroyWindow(hWnd);
    return r;
}
