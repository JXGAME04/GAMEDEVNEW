// [D3D11 08/09] Shader mo phong duong co dinh (fixed-function) cua D3D9 cho Represent3 tren D3D11.
// Dich: fxc /T vs_4_0 /E VS /Fh Rep3Shaders11_vs.h /Vn g_Rep3VS11 Rep3Shaders11.hlsl
//       fxc /T ps_4_0 /E PS /Fh Rep3Shaders11_ps.h /Vn g_Rep3PS11 Rep3Shaders11.hlsl
// Dinh (vertex): XYZRHW (toa do man hinh, dung cho VERTEX2D + chu) hoac XYZ (nhan ma tran W*V*P, duong 3D cu).

cbuffer VSCB : register(b0)
{
    float4          g_vp;     // x = rong viewport, y = cao viewport, z = x viewport, w = y viewport
    row_major float4x4 g_wvp; // world * view * proj (D3D9 hang-chinh, v * M)
    float4          g_flags;  // x = 1 -> dinh XYZRHW, 0 -> XYZ
};

struct VSIn  { float4 pos : POSITION; float4 col : COLOR0; float2 uv : TEXCOORD0; uint2 palrow : PALROW; };
struct VSOut { float4 pos : SV_Position; float4 col : COLOR0; float2 uv : TEXCOORD0; nointerpolation uint2 palrow : PALROW; };   // [MANG 09/09 b] x = hang | lop | nguon | alpha test, y = alpha ref

VSOut VS(VSIn i)
{
    VSOut o;
    if (g_flags.x > 0.5)
    {
        // D3D9 dat tam diem anh o toa do nguyen, D3D11 o +0.5 -> cong 0.5 de anh xa texel y het D3D9
        float2 p = i.pos.xy + 0.5;
        o.pos = float4((p.x - g_vp.z) / g_vp.x * 2.0 - 1.0, 1.0 - (p.y - g_vp.w) / g_vp.y * 2.0, 0.5, 1.0);
    }
    else
    {
        o.pos = mul(float4(i.pos.xyz, 1.0), g_wvp);
    }
    o.col = i.col;
    o.uv = i.uv;
    o.palrow = i.palrow;
    return o;
}

// ---------------------------------------------------------------- pixel
cbuffer PSCB : register(b0)
{
    int4   g_st0;   // stage 0: colorOp, colorArg1, colorArg2, alphaOp   (D3DTOP_* / D3DTA_*)
    int4   g_st0b;  // stage 0: alphaArg1, alphaArg2, tex0 bound, [r2] loc tuyen tinh (1) -> tu noi suy texture bang mau
    int4   g_st1;   // stage 1: colorOp, colorArg1, colorArg2, alphaOp
    int4   g_st1b;  // stage 1: alphaArg1, alphaArg2, tex1 bound, 0
    float4 g_at;    // x = alpha test bat, y = D3DCMP_*, z = alpha ref (0..255), w = 0
};

#ifdef MANG
Texture2DArray g_t0 : register(t0);   // [MANG 09/09] atlas = mang trang; lop = 16 bit cao cua PALROW
#else
Texture2D    g_t0 : register(t0);
#endif
SamplerState g_s0 : register(s0);
#ifdef MANG
Texture2DArray g_t1 : register(t1);   // [MANG 09/09] lop cua stage 1 = g_st1b.w
#else
Texture2D    g_t1 : register(t1);
#endif
SamplerState g_s1 : register(s1);
Texture2D    g_pal : register(t2);   // [r] atlas bang mau 256 x N (BGRA8), hang = palrow
#ifdef MANG
// [MANG 09/09 f] atlas theo KHOI: 16 mang/dinh dang, chon khoi theo dinh (4 bit cao cua truong lop), lop trong khoi = 5 bit thap
Texture2DArray g_a8_0 : register(t3);
Texture2DArray g_a8_1 : register(t4);
Texture2DArray g_a8_2 : register(t5);
Texture2DArray g_a8_3 : register(t6);
Texture2DArray g_a8_4 : register(t7);
Texture2DArray g_a8_5 : register(t8);
Texture2DArray g_a8_6 : register(t9);
Texture2DArray g_a8_7 : register(t10);
Texture2DArray g_a8_8 : register(t11);
Texture2DArray g_a8_9 : register(t12);
Texture2DArray g_a8_10 : register(t13);
Texture2DArray g_a8_11 : register(t14);
Texture2DArray g_a8_12 : register(t15);
Texture2DArray g_a8_13 : register(t16);
Texture2DArray g_a8_14 : register(t17);
Texture2DArray g_a8_15 : register(t18);
Texture2DArray g_a32_0 : register(t19);
Texture2DArray g_a32_1 : register(t20);
Texture2DArray g_a32_2 : register(t21);
Texture2DArray g_a32_3 : register(t22);
Texture2DArray g_a32_4 : register(t23);
Texture2DArray g_a32_5 : register(t24);
Texture2DArray g_a32_6 : register(t25);
Texture2DArray g_a32_7 : register(t26);
Texture2DArray g_a32_8 : register(t27);
Texture2DArray g_a32_9 : register(t28);
Texture2DArray g_a32_10 : register(t29);
Texture2DArray g_a32_11 : register(t30);
Texture2DArray g_a32_12 : register(t31);
Texture2DArray g_a32_13 : register(t32);
Texture2DArray g_a32_14 : register(t33);
Texture2DArray g_a32_15 : register(t34);
float4 A8Sample(uint k, float3 p)
{
    switch (k)
    {
    case 0: return g_a8_0.SampleLevel(g_s0, p, 0);
    case 1: return g_a8_1.SampleLevel(g_s0, p, 0);
    case 2: return g_a8_2.SampleLevel(g_s0, p, 0);
    case 3: return g_a8_3.SampleLevel(g_s0, p, 0);
    case 4: return g_a8_4.SampleLevel(g_s0, p, 0);
    case 5: return g_a8_5.SampleLevel(g_s0, p, 0);
    case 6: return g_a8_6.SampleLevel(g_s0, p, 0);
    case 7: return g_a8_7.SampleLevel(g_s0, p, 0);
    case 8: return g_a8_8.SampleLevel(g_s0, p, 0);
    case 9: return g_a8_9.SampleLevel(g_s0, p, 0);
    case 10: return g_a8_10.SampleLevel(g_s0, p, 0);
    case 11: return g_a8_11.SampleLevel(g_s0, p, 0);
    case 12: return g_a8_12.SampleLevel(g_s0, p, 0);
    case 13: return g_a8_13.SampleLevel(g_s0, p, 0);
    case 14: return g_a8_14.SampleLevel(g_s0, p, 0);
    case 15: return g_a8_15.SampleLevel(g_s0, p, 0);
    default: return float4(0, 0, 0, 0);
    }
}
float4 A8Sample1(uint k, float3 p)
{
    switch (k)
    {
    case 0: return g_a8_0.SampleLevel(g_s1, p, 0);
    case 1: return g_a8_1.SampleLevel(g_s1, p, 0);
    case 2: return g_a8_2.SampleLevel(g_s1, p, 0);
    case 3: return g_a8_3.SampleLevel(g_s1, p, 0);
    case 4: return g_a8_4.SampleLevel(g_s1, p, 0);
    case 5: return g_a8_5.SampleLevel(g_s1, p, 0);
    case 6: return g_a8_6.SampleLevel(g_s1, p, 0);
    case 7: return g_a8_7.SampleLevel(g_s1, p, 0);
    case 8: return g_a8_8.SampleLevel(g_s1, p, 0);
    case 9: return g_a8_9.SampleLevel(g_s1, p, 0);
    case 10: return g_a8_10.SampleLevel(g_s1, p, 0);
    case 11: return g_a8_11.SampleLevel(g_s1, p, 0);
    case 12: return g_a8_12.SampleLevel(g_s1, p, 0);
    case 13: return g_a8_13.SampleLevel(g_s1, p, 0);
    case 14: return g_a8_14.SampleLevel(g_s1, p, 0);
    case 15: return g_a8_15.SampleLevel(g_s1, p, 0);
    default: return float4(0, 0, 0, 0);
    }
}
float4 A8Load(uint k, int4 p)
{
    switch (k)
    {
    case 0: return g_a8_0.Load(p);
    case 1: return g_a8_1.Load(p);
    case 2: return g_a8_2.Load(p);
    case 3: return g_a8_3.Load(p);
    case 4: return g_a8_4.Load(p);
    case 5: return g_a8_5.Load(p);
    case 6: return g_a8_6.Load(p);
    case 7: return g_a8_7.Load(p);
    case 8: return g_a8_8.Load(p);
    case 9: return g_a8_9.Load(p);
    case 10: return g_a8_10.Load(p);
    case 11: return g_a8_11.Load(p);
    case 12: return g_a8_12.Load(p);
    case 13: return g_a8_13.Load(p);
    case 14: return g_a8_14.Load(p);
    case 15: return g_a8_15.Load(p);
    default: return float4(0, 0, 0, 0);
    }
}
float2 A8Dim(uint k)
{
    float3 d = float3(1, 1, 1);
    switch (k)
    {
    case 0: g_a8_0.GetDimensions(d.x, d.y, d.z); break;
    case 1: g_a8_1.GetDimensions(d.x, d.y, d.z); break;
    case 2: g_a8_2.GetDimensions(d.x, d.y, d.z); break;
    case 3: g_a8_3.GetDimensions(d.x, d.y, d.z); break;
    case 4: g_a8_4.GetDimensions(d.x, d.y, d.z); break;
    case 5: g_a8_5.GetDimensions(d.x, d.y, d.z); break;
    case 6: g_a8_6.GetDimensions(d.x, d.y, d.z); break;
    case 7: g_a8_7.GetDimensions(d.x, d.y, d.z); break;
    case 8: g_a8_8.GetDimensions(d.x, d.y, d.z); break;
    case 9: g_a8_9.GetDimensions(d.x, d.y, d.z); break;
    case 10: g_a8_10.GetDimensions(d.x, d.y, d.z); break;
    case 11: g_a8_11.GetDimensions(d.x, d.y, d.z); break;
    case 12: g_a8_12.GetDimensions(d.x, d.y, d.z); break;
    case 13: g_a8_13.GetDimensions(d.x, d.y, d.z); break;
    case 14: g_a8_14.GetDimensions(d.x, d.y, d.z); break;
    case 15: g_a8_15.GetDimensions(d.x, d.y, d.z); break;
    default: break;
    }
    return d.xy;
}
float4 A32Sample(uint k, float3 p)
{
    switch (k)
    {
    case 0: return g_a32_0.SampleLevel(g_s0, p, 0);
    case 1: return g_a32_1.SampleLevel(g_s0, p, 0);
    case 2: return g_a32_2.SampleLevel(g_s0, p, 0);
    case 3: return g_a32_3.SampleLevel(g_s0, p, 0);
    case 4: return g_a32_4.SampleLevel(g_s0, p, 0);
    case 5: return g_a32_5.SampleLevel(g_s0, p, 0);
    case 6: return g_a32_6.SampleLevel(g_s0, p, 0);
    case 7: return g_a32_7.SampleLevel(g_s0, p, 0);
    case 8: return g_a32_8.SampleLevel(g_s0, p, 0);
    case 9: return g_a32_9.SampleLevel(g_s0, p, 0);
    case 10: return g_a32_10.SampleLevel(g_s0, p, 0);
    case 11: return g_a32_11.SampleLevel(g_s0, p, 0);
    case 12: return g_a32_12.SampleLevel(g_s0, p, 0);
    case 13: return g_a32_13.SampleLevel(g_s0, p, 0);
    case 14: return g_a32_14.SampleLevel(g_s0, p, 0);
    case 15: return g_a32_15.SampleLevel(g_s0, p, 0);
    default: return float4(0, 0, 0, 0);
    }
}
float4 A32Sample1(uint k, float3 p)
{
    switch (k)
    {
    case 0: return g_a32_0.SampleLevel(g_s1, p, 0);
    case 1: return g_a32_1.SampleLevel(g_s1, p, 0);
    case 2: return g_a32_2.SampleLevel(g_s1, p, 0);
    case 3: return g_a32_3.SampleLevel(g_s1, p, 0);
    case 4: return g_a32_4.SampleLevel(g_s1, p, 0);
    case 5: return g_a32_5.SampleLevel(g_s1, p, 0);
    case 6: return g_a32_6.SampleLevel(g_s1, p, 0);
    case 7: return g_a32_7.SampleLevel(g_s1, p, 0);
    case 8: return g_a32_8.SampleLevel(g_s1, p, 0);
    case 9: return g_a32_9.SampleLevel(g_s1, p, 0);
    case 10: return g_a32_10.SampleLevel(g_s1, p, 0);
    case 11: return g_a32_11.SampleLevel(g_s1, p, 0);
    case 12: return g_a32_12.SampleLevel(g_s1, p, 0);
    case 13: return g_a32_13.SampleLevel(g_s1, p, 0);
    case 14: return g_a32_14.SampleLevel(g_s1, p, 0);
    case 15: return g_a32_15.SampleLevel(g_s1, p, 0);
    default: return float4(0, 0, 0, 0);
    }
}
float4 A32Load(uint k, int4 p)
{
    switch (k)
    {
    case 0: return g_a32_0.Load(p);
    case 1: return g_a32_1.Load(p);
    case 2: return g_a32_2.Load(p);
    case 3: return g_a32_3.Load(p);
    case 4: return g_a32_4.Load(p);
    case 5: return g_a32_5.Load(p);
    case 6: return g_a32_6.Load(p);
    case 7: return g_a32_7.Load(p);
    case 8: return g_a32_8.Load(p);
    case 9: return g_a32_9.Load(p);
    case 10: return g_a32_10.Load(p);
    case 11: return g_a32_11.Load(p);
    case 12: return g_a32_12.Load(p);
    case 13: return g_a32_13.Load(p);
    case 14: return g_a32_14.Load(p);
    case 15: return g_a32_15.Load(p);
    default: return float4(0, 0, 0, 0);
    }
}
float2 A32Dim(uint k)
{
    float3 d = float3(1, 1, 1);
    switch (k)
    {
    case 0: g_a32_0.GetDimensions(d.x, d.y, d.z); break;
    case 1: g_a32_1.GetDimensions(d.x, d.y, d.z); break;
    case 2: g_a32_2.GetDimensions(d.x, d.y, d.z); break;
    case 3: g_a32_3.GetDimensions(d.x, d.y, d.z); break;
    case 4: g_a32_4.GetDimensions(d.x, d.y, d.z); break;
    case 5: g_a32_5.GetDimensions(d.x, d.y, d.z); break;
    case 6: g_a32_6.GetDimensions(d.x, d.y, d.z); break;
    case 7: g_a32_7.GetDimensions(d.x, d.y, d.z); break;
    case 8: g_a32_8.GetDimensions(d.x, d.y, d.z); break;
    case 9: g_a32_9.GetDimensions(d.x, d.y, d.z); break;
    case 10: g_a32_10.GetDimensions(d.x, d.y, d.z); break;
    case 11: g_a32_11.GetDimensions(d.x, d.y, d.z); break;
    case 12: g_a32_12.GetDimensions(d.x, d.y, d.z); break;
    case 13: g_a32_13.GetDimensions(d.x, d.y, d.z); break;
    case 14: g_a32_14.GetDimensions(d.x, d.y, d.z); break;
    case 15: g_a32_15.GetDimensions(d.x, d.y, d.z); break;
    default: break;
    }
    return d.xy;
}
// nguon texture stage 0 theo dinh: 0 = t0 (texture rieng, view mang 1 lop), 1 = khoi R8G8, 2 = khoi BGRA8; lop = khoi<<5 | lop
float4 T0Sample(uint src, uint lop, float2 uv)
{
    float3 p = float3(uv, (float)(lop & 31u));
    if (src == 1u) return A8Sample((lop >> 5) & 15u, p);
    if (src == 2u) return A32Sample((lop >> 5) & 15u, p);
    return g_t0.SampleLevel(g_s0, float3(uv, 0.0), 0);
}
float4 T0Load(uint src, uint lop, int2 p)
{
    int4 q = int4(p, (int)(lop & 31u), 0);
    if (src == 1u) return A8Load((lop >> 5) & 15u, q);
    if (src == 2u) return A32Load((lop >> 5) & 15u, q);
    return g_t0.Load(int4(p, 0, 0));
}
float2 T0Dim(uint src, uint lop)
{
    if (src == 1u) return A8Dim((lop >> 5) & 15u);
    if (src == 2u) return A32Dim((lop >> 5) & 15u);
    float3 d; g_t0.GetDimensions(d.x, d.y, d.z); return d.xy;
}
float4 T1Sample(float2 uv)
{   // stage 1: nguon = g_at.x, lop (khoi<<5|lop) = g_at.y (cb, hiem doi)
    uint src = (uint)g_at.x; uint lop = (uint)g_at.y;
    float3 p = float3(uv, (float)(lop & 31u));
    if (src == 1u) return A8Sample1((lop >> 5) & 15u, p);
    if (src == 2u) return A32Sample1((lop >> 5) & 15u, p);
    return g_t1.SampleLevel(g_s1, float3(uv, 0.0), 0);
}
#endif
#ifdef MANG
#define T0_SAMPLE(uv)  T0Sample(src, lop, uv)
#define T0_LOAD(p)     T0Load(src, lop, p)
#define T0_DIM(d)      d = T0Dim(src, lop)
#define T1_SAMPLE(uv)  T1Sample(uv)
#else
#define T0_SAMPLE(uv)  g_t0.Sample(g_s0, uv)
#define T0_LOAD(p)     g_t0.Load(int3(p, 0))
#define T0_DIM(d)      g_t0.GetDimensions(d.x, d.y)
#define T1_SAMPLE(uv)  g_t1.Sample(g_s1, uv)
#endif

// [r] texture chi so R8G8 -> mau: R = chi so bang mau, G = alpha
float4 PalTex(float4 ia, uint row)
{
    uint idx = (uint)(ia.r * 255.0 + 0.5);
    float4 c = g_pal.Load(int3(idx, row, 0));
    return float4(c.rgb, ia.g);
}

float4 Arg(int a, float4 dif, float4 cur, float4 tex)
{
    int sel = a & 15;                       // D3DTA_DIFFUSE 0, CURRENT 1, TEXTURE 2, TFACTOR 3 (coi = trang)
    float4 v = (sel == 0) ? dif : ((sel == 1) ? cur : ((sel == 2) ? tex : float4(1, 1, 1, 1)));
    if (a & 0x10) v = 1.0 - v;              // D3DTA_COMPLEMENT
    if (a & 0x20) v = v.aaaa;               // D3DTA_ALPHAREPLICATE
    return v;
}

float3 ColorOp(int op, float3 a1, float3 a2, float3 cur)
{
    if (op == 2) return a1;                         // SELECTARG1
    if (op == 3) return a2;                         // SELECTARG2
    if (op == 4) return a1 * a2;                    // MODULATE
    if (op == 5) return 2.0 * a1 * a2;              // MODULATE2X
    if (op == 6) return 4.0 * a1 * a2;              // MODULATE4X
    if (op == 7) return a1 + a2;                    // ADD
    if (op == 8) return a1 + a2 - 0.5;              // ADDSIGNED
    if (op == 9) return 2.0 * (a1 + a2 - 0.5);      // ADDSIGNED2X
    if (op == 10) return a1 - a2;                   // SUBTRACT
    if (op == 11) return a1 + a2 - a1 * a2;         // ADDSMOOTH
    if (op == 13) return a1 * cur.rgb + a2 * (1.0 - cur.rgb); // BLENDDIFFUSEALPHA... (gan dung, khong dung trong game)
    return cur;
}

float AlphaOp(int op, float a1, float a2, float cur)
{
    if (op == 2) return a1;
    if (op == 3) return a2;
    if (op == 4) return a1 * a2;
    if (op == 5) return 2.0 * a1 * a2;
    if (op == 6) return 4.0 * a1 * a2;
    if (op == 7) return a1 + a2;
    if (op == 8) return a1 + a2 - 0.5;
    if (op == 9) return 2.0 * (a1 + a2 - 0.5);
    if (op == 10) return a1 - a2;
    if (op == 11) return a1 + a2 - a1 * a2;
    return cur;
}

float4 Stage(int4 st, int4 stb, float4 dif, float4 cur, float4 tex)
{
    float4 c1 = Arg(st.y, dif, cur, tex);
    float4 c2 = Arg(st.z, dif, cur, tex);
    float4 a1 = Arg(stb.x, dif, cur, tex);
    float4 a2 = Arg(stb.y, dif, cur, tex);
    float3 rgb = ColorOp(st.x, c1.rgb, c2.rgb, cur.rgb);
    float  a   = AlphaOp(st.w, a1.a, a2.a, cur.a);
    return saturate(float4(rgb, a));
}

#ifdef MANG
// [MANG 09/09 d] arg stage 0 goi 4 bit theo dinh: sel (2) | complement (1) | alphareplicate (1) -> ma D3DTA nhu cu
int ArgMo(uint a) { return (int)((a & 3u) | (((a & 4u) != 0u) ? 0x10u : 0u) | (((a & 8u) != 0u) ? 0x20u : 0u)); }
#endif
float4 PS(VSOut i) : SV_Target
{
    uint row = i.palrow.x & 0xFFFFu;   // [MANG 09/09] hang bang mau
#ifdef MANG
    uint lop = (i.palrow.x >> 16) & 0x1FFu; uint src = (i.palrow.x >> 25) & 3u;   // [MANG 09/09 b] lop (khoi<<5|lop, [MANG 09/09 f]) + nguon theo dinh
    // [MANG 09/09 d] tham so stage 0 theo dinh (y) + tex0 bound (x bit 31); loc tuyen tinh van tu cb (g_st0b.w)
    uint y = i.palrow.y;
    int4 st0 = int4((int)((y >> 8) & 15u), ArgMo((y >> 12) & 15u), ArgMo((y >> 16) & 15u), (int)((y >> 20) & 15u));
    int4 st0b = int4(ArgMo((y >> 24) & 15u), ArgMo((y >> 28) & 15u), (int)((i.palrow.x >> 31) & 1u), g_st0b.w);
#else
    int4 st0 = g_st0; int4 st0b = g_st0b;
#endif
    float4 dif = i.col;
    float4 cur = dif;
    if (st0.x != 1)   // stage 0 khong DISABLE
    {
        float4 tex0 = (st0b.z != 0) ? T0_SAMPLE(i.uv) : float4(1, 1, 1, 1);
        if (row != 0xFFFFu && st0b.z != 0)
        {   // [r] texture chi so (R8G8): R = chi so bang mau, G = alpha
            if (st0b.w != 0)
            {   // [r2] loc tuyen tinh: lay 4 diem, tra bang tung diem roi noi suy (nhu phan cung voi BGRA8); khong noi suy CHI SO
                float2 dim; T0_DIM(dim);
                float2 p = i.uv * dim - 0.5; float2 f = frac(p); int2 p0 = (int2)floor(p); int2 mx = (int2)dim - 1;
                float4 c00 = PalTex(T0_LOAD(clamp(p0, int2(0, 0), mx)), row);
                float4 c10 = PalTex(T0_LOAD(clamp(p0 + int2(1, 0), int2(0, 0), mx)), row);
                float4 c01 = PalTex(T0_LOAD(clamp(p0 + int2(0, 1), int2(0, 0), mx)), row);
                float4 c11 = PalTex(T0_LOAD(clamp(p0 + int2(1, 1), int2(0, 0), mx)), row);
                tex0 = lerp(lerp(c00, c10, f.x), lerp(c01, c11, f.x), f.y);
            }
            else
                tex0 = PalTex(tex0, row);
        }
        cur = Stage(st0, st0b, dif, dif, tex0);
        if (g_st1.x != 1)
        {
            float4 tex1 = (g_st1b.z != 0) ? T1_SAMPLE(i.uv) : float4(1, 1, 1, 1);
            cur = Stage(g_st1, g_st1b, dif, cur, tex1);
        }
    }
#ifdef MANG
    if (((i.palrow.x >> 27) & 1u) != 0u)   // [MANG 09/09 b] alpha test theo dinh: bat | ham | ref
    {
        int a8 = (int)floor(cur.a * 255.0 + 0.5);
        int r8 = (int)(i.palrow.y & 0xFFu);
        int f = (int)((i.palrow.x >> 28) & 7u) + 1;
#else
    if (g_at.x > 0.5)
    {
        // so sanh tren alpha 8 bit nhu phan cung D3D9
        int a8 = (int)floor(cur.a * 255.0 + 0.5);
        int r8 = (int)g_at.z;
        int f = (int)g_at.y;
#endif
        bool ok = true;
        if (f == 1) ok = false;
        else if (f == 2) ok = a8 <  r8;
        else if (f == 3) ok = a8 == r8;
        else if (f == 4) ok = a8 <= r8;
        else if (f == 5) ok = a8 >  r8;
        else if (f == 6) ok = a8 != r8;
        else if (f == 7) ok = a8 >= r8;
        if (!ok) discard;
    }
    return cur;
}
