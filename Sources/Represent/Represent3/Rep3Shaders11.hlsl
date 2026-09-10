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
Texture2DArray g_a8  : register(t3);  // [MANG 09/09 b] atlas R8G8 (bang mau), gan co dinh
Texture2DArray g_a32 : register(t4);  // [MANG 09/09 b] atlas BGRA8, gan co dinh
// nguon texture stage 0 theo dinh: 0 = t0 (texture rieng, view mang 1 lop), 1 = t3, 2 = t4
float4 T0Sample(uint src, float lop, float2 uv)
{
    if (src == 1u) return g_a8.Sample(g_s0, float3(uv, lop));
    if (src == 2u) return g_a32.Sample(g_s0, float3(uv, lop));
    return g_t0.Sample(g_s0, float3(uv, 0.0));
}
float4 T0Load(uint src, float lop, int2 p)
{
    if (src == 1u) return g_a8.Load(int4(p, (int)lop, 0));
    if (src == 2u) return g_a32.Load(int4(p, (int)lop, 0));
    return g_t0.Load(int4(p, 0, 0));
}
float2 T0Dim(uint src)
{
    float3 d;
    if (src == 1u) g_a8.GetDimensions(d.x, d.y, d.z);
    else if (src == 2u) g_a32.GetDimensions(d.x, d.y, d.z);
    else g_t0.GetDimensions(d.x, d.y, d.z);
    return d.xy;
}
float4 T1Sample(float2 uv)
{   // stage 1: nguon = g_at.x, lop = g_at.y (cb, hiem doi)
    uint src = (uint)g_at.x; float lop = g_at.y;
    if (src == 1u) return g_a8.Sample(g_s1, float3(uv, lop));
    if (src == 2u) return g_a32.Sample(g_s1, float3(uv, lop));
    return g_t1.Sample(g_s1, float3(uv, 0.0));
}
#endif
#ifdef MANG
#define T0_SAMPLE(uv)  T0Sample(src, lop, uv)
#define T0_LOAD(p)     T0Load(src, lop, p)
#define T0_DIM(d)      d = T0Dim(src)
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

float4 PS(VSOut i) : SV_Target
{
    uint row = i.palrow.x & 0xFFFFu;   // [MANG 09/09] hang bang mau
#ifdef MANG
    float lop = (float)((i.palrow.x >> 16) & 0x1FFu); uint src = (i.palrow.x >> 25) & 3u;   // [MANG 09/09 b] lop + nguon theo dinh
#endif
    float4 dif = i.col;
    float4 cur = dif;
    if (g_st0.x != 1)   // stage 0 khong DISABLE
    {
        float4 tex0 = (g_st0b.z != 0) ? T0_SAMPLE(i.uv) : float4(1, 1, 1, 1);
        if (row != 0xFFFFu && g_st0b.z != 0)
        {   // [r] texture chi so (R8G8): R = chi so bang mau, G = alpha
            if (g_st0b.w != 0)
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
        cur = Stage(g_st0, g_st0b, dif, dif, tex0);
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
