# -*- coding: utf-8 -*-
"""goi_va_d3d11r2_palette_linear_0809.py - [D3D11 08/09 r2] texture bang mau + loc LINEAR:
Represent3 bat D3DTEXF_LINEAR o vai cho (DrawImage3D che do phoi canh, ve bitmap co gian). Loc tuyen tinh tren texture CHI SO
noi suy chi so -> mau sai. Sua: shim bao co loc tuyen tinh o stage 0 (g_st0b.w), shader tu lay 4 diem, tra bang tung diem roi
noi suy (dung nhu phan cung lam voi BGRA8). Bien moi truong REP3_PALLIN=1 ep co de thu (harness: ket qua phai y het duong diem)."""
import io, sys
ROOT = r"D:\GAMEDEVNEW_wt_delta\Sources\Represent\Represent3"
def load(name):
    p = ROOT + "\\" + name
    s = io.open(p, "r", encoding="latin-1", newline="").read().replace("\r\n", "\n")
    return p, s
def save(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)
def rep(s, old, new, n=1):
    c = s.count(old)
    if c != n: print("FAIL neo %r: thay %d, can %d" % (old[:70], c, n)); sys.exit(1)
    return s.replace(old, new)

p, s = load("D3D9on11i.h")
if "m_bPalLinForce" not in s:
    s = rep(s, "\tbool    PalInit(); void PalRelease(); void PalFrameEnd();", "\tbool    m_bPalLinForce;\t// [r2] REP3_PALLIN=1: ep duong loc tuyen tinh bang mau (thu)\n\tbool    PalInit(); void PalRelease(); void PalFrameEnd();")
save(p, s); print("OK D3D9on11i.h")

p, s = load("D3D9on11Dev.cpp")
if "m_bPalLinForce" not in s:
    s = rep(s, "m_bRingDiscard = true; m_pAtlas = NULL; m_pPalTex = NULL; m_pPalSrv = NULL; m_pDummy = NULL;",
        "m_bRingDiscard = true; m_pAtlas = NULL; m_pPalTex = NULL; m_pPalSrv = NULL; m_pDummy = NULL;\n\t{ const char* e = getenv(\"REP3_PALLIN\"); m_bPalLinForce = (e && atoi(e) != 0); }\t// [r2]")
    s = rep(s, "cb.st0b[2] = a.srv[0] ? 1 : 0; cb.st0b[3] = 0;",
        "cb.st0b[2] = a.srv[0] ? 1 : 0;\n\tcb.st0b[3] = (m_bPalLinForce || (m_ss[0][D3DSAMP_MAGFILTER] & 7) >= D3DTEXF_LINEAR || (m_ss[0][D3DSAMP_MINFILTER] & 7) >= D3DTEXF_LINEAR) ? 1 : 0;\t// [r2] loc tuyen tinh stage 0 -> shader tu noi suy texture bang mau")
save(p, s); print("OK D3D9on11Dev.cpp")

p, s = load("Rep3Shaders11.hlsl")
if "PalTex(" not in s:
    s = rep(s, "    int4   g_st0b;  // stage 0: alphaArg1, alphaArg2, tex0 bound, 0\n", "    int4   g_st0b;  // stage 0: alphaArg1, alphaArg2, tex0 bound, [r2] loc tuyen tinh (1) -> tu noi suy texture bang mau\n")
    s = rep(s, "float4 Arg(int a, float4 dif, float4 cur, float4 tex)\n",
        "// [r] texture chi so R8G8 -> mau: R = chi so bang mau, G = alpha\nfloat4 PalTex(float4 ia, uint row)\n{\n    uint idx = (uint)(ia.r * 255.0 + 0.5);\n    float4 c = g_pal.Load(int3(idx, row, 0));\n    return float4(c.rgb, ia.g);\n}\n\nfloat4 Arg(int a, float4 dif, float4 cur, float4 tex)\n")
    s = rep(s, "        if (i.palrow != 0xFFFFu && g_st0b.z != 0)\n        {   // [r] texture chi so (R8G8): R = chi so bang mau, G = alpha\n"
        "            uint idx = (uint)(tex0.r * 255.0 + 0.5);\n"
        "            float4 c = g_pal.Load(int3(idx, i.palrow, 0));\n"
        "            tex0 = float4(c.rgb, tex0.g);\n        }\n",
        "        if (i.palrow != 0xFFFFu && g_st0b.z != 0)\n        {   // [r] texture chi so (R8G8): R = chi so bang mau, G = alpha\n"
        "            if (g_st0b.w != 0)\n            {   // [r2] loc tuyen tinh: lay 4 diem, tra bang tung diem roi noi suy (nhu phan cung voi BGRA8); khong noi suy CHI SO\n"
        "                float2 dim; g_t0.GetDimensions(dim.x, dim.y);\n"
        "                float2 p = i.uv * dim - 0.5; float2 f = frac(p); int2 p0 = (int2)floor(p); int2 mx = (int2)dim - 1;\n"
        "                float4 c00 = PalTex(g_t0.Load(int3(clamp(p0, int2(0, 0), mx), 0)), i.palrow);\n"
        "                float4 c10 = PalTex(g_t0.Load(int3(clamp(p0 + int2(1, 0), int2(0, 0), mx), 0)), i.palrow);\n"
        "                float4 c01 = PalTex(g_t0.Load(int3(clamp(p0 + int2(0, 1), int2(0, 0), mx), 0)), i.palrow);\n"
        "                float4 c11 = PalTex(g_t0.Load(int3(clamp(p0 + int2(1, 1), int2(0, 0), mx), 0)), i.palrow);\n"
        "                tex0 = lerp(lerp(c00, c10, f.x), lerp(c01, c11, f.x), f.y);\n            }\n"
        "            else\n                tex0 = PalTex(tex0, i.palrow);\n        }\n")
save(p, s); print("OK Rep3Shaders11.hlsl")
print("XONG")
