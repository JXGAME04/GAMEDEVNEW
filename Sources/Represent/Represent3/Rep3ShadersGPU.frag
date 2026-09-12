#version 450
// [GPU 08/09] Shader diem anh cho lop D3D9 tren SDL_GPU - chuyen tu Rep3Shaders11.hlsl (PS): mo phong 2 texture stage cua duong co dinh
// D3D9 (D3DTOP_* / D3DTA_*), bang mau (texture chi so R8G8: R = chi so, G = alpha) va alpha test.
// Dich: glslc -fshader-stage=frag -O Rep3ShadersGPU.frag -o Rep3ShadersGPU.frag.spv
// SDL_GPU: sampler cua fragment o set 2 (binding 0 = t0, 1 = t1, 2 = bang mau), uniform cua fragment o set 3.

layout(location = 0) in vec4 vCol;
layout(location = 1) in vec2 vUv;
layout(location = 2) flat in uint vPal;

#ifdef JX_ATLAS_KHOI
// [KHOI 11/09] (chi Android) port buoc (f) cua loat [MANG 09/09] ben duong D3D11 (commit 5311778b): atlas theo KHOI CO DINH.
// Moi khoi = mot texture mang 2D nhieu lop, GAN CHET vao mot khe sampler va KHONG BAO GIO doi trong ca khung ->
// hai quad o hai trang atlas khac nhau van gop chung mot lenh ve.
// Khe 0 (g_t0) van la sampler2D cho texture RIENG (khong vao atlas) - khong ep texture thuong thanh mang.
// [KHOI2 11/09] NOI TRAN: 8 -> 12 khoi (khe 2..13), lay mot bit tu truong chi so ps (12 -> 11 bit; do thuc te chi dung 5 muc).
// O PALROW: bit 0..12 hang bang mau | 13..23 chi so ps | 24..26 lop | 27..30 khoi | 31 = lay tu khoi atlas.
// SDL_GPU chot 16 khe sampler MOI TANG nen dung 12 khoi la con du 2 khe; hai storage buffer ve binding 14, 15.
layout(set = 2, binding = 0) uniform sampler2D g_t0;
layout(set = 2, binding = 1) uniform sampler2D g_t1;
layout(set = 2, binding = 2) uniform sampler2DArray g_k0;
layout(set = 2, binding = 3) uniform sampler2DArray g_k1;
layout(set = 2, binding = 4) uniform sampler2DArray g_k2;
layout(set = 2, binding = 5) uniform sampler2DArray g_k3;
layout(set = 2, binding = 6) uniform sampler2DArray g_k4;
layout(set = 2, binding = 7) uniform sampler2DArray g_k5;
layout(set = 2, binding = 8) uniform sampler2DArray g_k6;
layout(set = 2, binding = 9) uniform sampler2DArray g_k7;
layout(set = 2, binding = 10) uniform sampler2DArray g_k8;
layout(set = 2, binding = 11) uniform sampler2DArray g_k9;
layout(set = 2, binding = 12) uniform sampler2DArray g_k10;
layout(set = 2, binding = 13) uniform sampler2DArray g_k11;

#define JX_KHOI_CO   ((vPal & 0x80000000u) != 0u)
#define JX_KHOI_I    ((vPal >> 27) & 0xFu)
#define JX_KHOI_L    float((vPal >> 24) & 7u)
#define JX_KHOI_LI   int((vPal >> 24) & 7u)

vec4 JxKhoiTex(vec2 uv)
{
    uint k = JX_KHOI_I; float l = JX_KHOI_L;
    if (k == 0u) return texture(g_k0, vec3(uv, l));
    else if (k == 1u) return texture(g_k1, vec3(uv, l));
    else if (k == 2u) return texture(g_k2, vec3(uv, l));
    else if (k == 3u) return texture(g_k3, vec3(uv, l));
    else if (k == 4u) return texture(g_k4, vec3(uv, l));
    else if (k == 5u) return texture(g_k5, vec3(uv, l));
    else if (k == 6u) return texture(g_k6, vec3(uv, l));
    else if (k == 7u) return texture(g_k7, vec3(uv, l));
    else if (k == 8u) return texture(g_k8, vec3(uv, l));
    else if (k == 9u) return texture(g_k9, vec3(uv, l));
    else if (k == 10u) return texture(g_k10, vec3(uv, l));
    else if (k == 11u) return texture(g_k11, vec3(uv, l));
    return vec4(1.0);
}
vec4 JxKhoiFetch(ivec2 p)
{
    uint k = JX_KHOI_I; int l = JX_KHOI_LI;
    if (k == 0u) return texelFetch(g_k0, ivec3(p, l), 0);
    else if (k == 1u) return texelFetch(g_k1, ivec3(p, l), 0);
    else if (k == 2u) return texelFetch(g_k2, ivec3(p, l), 0);
    else if (k == 3u) return texelFetch(g_k3, ivec3(p, l), 0);
    else if (k == 4u) return texelFetch(g_k4, ivec3(p, l), 0);
    else if (k == 5u) return texelFetch(g_k5, ivec3(p, l), 0);
    else if (k == 6u) return texelFetch(g_k6, ivec3(p, l), 0);
    else if (k == 7u) return texelFetch(g_k7, ivec3(p, l), 0);
    else if (k == 8u) return texelFetch(g_k8, ivec3(p, l), 0);
    else if (k == 9u) return texelFetch(g_k9, ivec3(p, l), 0);
    else if (k == 10u) return texelFetch(g_k10, ivec3(p, l), 0);
    else if (k == 11u) return texelFetch(g_k11, ivec3(p, l), 0);
    return vec4(1.0);
}
ivec2 JxKhoiDim()
{
    uint k = JX_KHOI_I;
    if (k == 0u) return textureSize(g_k0, 0).xy;
    else if (k == 1u) return textureSize(g_k1, 0).xy;
    else if (k == 2u) return textureSize(g_k2, 0).xy;
    else if (k == 3u) return textureSize(g_k3, 0).xy;
    else if (k == 4u) return textureSize(g_k4, 0).xy;
    else if (k == 5u) return textureSize(g_k5, 0).xy;
    else if (k == 6u) return textureSize(g_k6, 0).xy;
    else if (k == 7u) return textureSize(g_k7, 0).xy;
    else if (k == 8u) return textureSize(g_k8, 0).xy;
    else if (k == 9u) return textureSize(g_k9, 0).xy;
    else if (k == 10u) return textureSize(g_k10, 0).xy;
    else if (k == 11u) return textureSize(g_k11, 0).xy;
    return ivec2(1, 1);
}
#define JX_TEX0(uv)  (JX_KHOI_CO ? JxKhoiTex(uv)  : texture(g_t0, uv))
#define JX_TEX1(uv)  texture(g_t1, uv)
#define JX_FETCH0(p) (JX_KHOI_CO ? JxKhoiFetch(p) : texelFetch(g_t0, p, 0))
#define JX_DIM0      (JX_KHOI_CO ? JxKhoiDim()    : textureSize(g_t0, 0))
#define JX_BUF0      14
#define JX_BUF1      15
#elif defined(JX_TEX_ARRAY)
// [MANG 11/09] (chi Android) nhieu trang atlas trong MOT texture mang 2D; lop lay tu o PALROW bit 25..30 -> hai quad o hai trang cung cum GOP duoc
layout(set = 2, binding = 0) uniform sampler2DArray g_t0;
layout(set = 2, binding = 1) uniform sampler2DArray g_t1;
#define JX_LOP0      float((vPal >> 24) & 0x3Fu)	// [KHOI2 11/09] bo cuc moi: chi so ps chi con 11 bit (13..23)
#define JX_TEX0(uv)  texture(g_t0, vec3(uv, JX_LOP0))
#define JX_TEX1(uv)  texture(g_t1, vec3(uv, 0.0))
#define JX_FETCH0(p) texelFetch(g_t0, ivec3(p, int((vPal >> 24) & 0x3Fu)), 0)	// [KHOI2 11/09]
#define JX_DIM0      textureSize(g_t0, 0).xy
#define JX_BUF0      2
#define JX_BUF1      3
#else
layout(set = 2, binding = 0) uniform sampler2D g_t0;
layout(set = 2, binding = 1) uniform sampler2D g_t1;
#define JX_TEX0(uv)  texture(g_t0, uv)
#define JX_TEX1(uv)  texture(g_t1, uv)
#define JX_FETCH0(p) texelFetch(g_t0, p, 0)
#define JX_DIM0      textureSize(g_t0, 0)
#define JX_BUF0      2
#define JX_BUF1      3
#endif
#ifdef JX_PAL_BUFFER
// [PALBUF 11/09] (chi Android, -DJX_PAL_BUFFER) bang mau = storage buffer 8192 hang x 256 mau BGRA8 (uint), hang = vPal; set 2 binding 2 = ngay sau 2 sampler
layout(std430, set = 2, binding = JX_BUF0) readonly buffer PalBuf { uint g_palBuf[]; };
#else
layout(set = 2, binding = 2) uniform sampler2D g_pal;   // atlas bang mau 256 x N (BGRA8), hang = vPal
#endif

#ifdef JX_PS_BUFFER
// [GOP 11/09] (chi Android, -DJX_PS_BUFFER) trang thai tang texture cua CA KHUNG nam trong storage buffer; moi dinh mang chi so
// trong o PALROW (bit 13..24) -> hai quad chi khac trang thai van gop chung mot lenh ve. set 2 binding 3 = sau bang mau.
struct JxPsRec { ivec4 st0; ivec4 st0b; ivec4 st1; ivec4 st1b; vec4 at; };
layout(std430, set = 2, binding = JX_BUF1) readonly buffer PsBuf { JxPsRec g_psBuf[]; };
#else
layout(set = 3, binding = 0) uniform PSCB
{
    ivec4 g_st0;   // stage 0: colorOp, colorArg1, colorArg2, alphaOp   (D3DTOP_* / D3DTA_*)
    ivec4 g_st0b;  // stage 0: alphaArg1, alphaArg2, tex0 bound, loc tuyen tinh (1) -> tu noi suy texture bang mau
    ivec4 g_st1;   // stage 1: colorOp, colorArg1, colorArg2, alphaOp
    ivec4 g_st1b;  // stage 1: alphaArg1, alphaArg2, tex1 bound, 0
    vec4  g_at;    // x = alpha test bat, y = D3DCMP_*, z = alpha ref (0..255), w = 0
};
#endif

#ifdef JX_PS_BUFFER
#define JX_PALROW   (vPal & 0x1FFFu)   // [GOP 11/09] o PALROW: bit 0..12 = hang bang mau, 13..23 = chi so to hop trang thai tang texture ([KHOI2 11/09])
#define JX_PALKHONG 0x1FFFu
#else
#define JX_PALROW   vPal
#define JX_PALKHONG 0xFFFFu
#endif

layout(location = 0) out vec4 outColor;

vec4 PalTex(vec4 ia, uint row)
{
    int idx = int(ia.r * 255.0 + 0.5);
#ifdef JX_PAL_BUFFER
    vec4 c = unpackUnorm4x8(g_palBuf[row * 256u + uint(idx)]);   // byte 0..3 = B, G, R, A (DWORD 0xAARRGGBB little-endian)
    return vec4(c.z, c.y, c.x, ia.g);
#else
    vec4 c = texelFetch(g_pal, ivec2(idx, int(row)), 0);
    return vec4(c.rgb, ia.g);
#endif
}

vec4 Arg(int a, vec4 dif, vec4 cur, vec4 tex)
{
    int sel = a & 15;                       // D3DTA_DIFFUSE 0, CURRENT 1, TEXTURE 2, TFACTOR 3 (coi = trang)
    vec4 v = (sel == 0) ? dif : ((sel == 1) ? cur : ((sel == 2) ? tex : vec4(1.0)));
    if ((a & 0x10) != 0) v = 1.0 - v;       // D3DTA_COMPLEMENT
    if ((a & 0x20) != 0) v = v.aaaa;        // D3DTA_ALPHAREPLICATE
    return v;
}

vec3 ColorOp(int op, vec3 a1, vec3 a2, vec3 cur)
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
    if (op == 13) return a1 * cur.rgb + a2 * (1.0 - cur.rgb); // BLENDDIFFUSEALPHA (gan dung, game khong dung)
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

vec4 Stage(ivec4 st, ivec4 stb, vec4 dif, vec4 cur, vec4 tex)
{
    vec4 c1 = Arg(st.y, dif, cur, tex);
    vec4 c2 = Arg(st.z, dif, cur, tex);
    vec4 a1 = Arg(stb.x, dif, cur, tex);
    vec4 a2 = Arg(stb.y, dif, cur, tex);
    vec3 rgb = ColorOp(st.x, c1.rgb, c2.rgb, cur.rgb);
    float a  = AlphaOp(st.w, a1.a, a2.a, cur.a);
    return clamp(vec4(rgb, a), 0.0, 1.0);
}

void main()
{
#ifdef JX_PS_BUFFER
    // [GOP 11/09] trang thai tang texture lay tu bang cua khung theo chi so mang tren dinh (khong con uniform moi lenh ve)
    JxPsRec jxR = g_psBuf[(vPal >> 13) & 0x7FFu];   // [KHOI2 11/09] 11 bit (2048 muc; do thuc te 5 muc) - nhuong 1 bit cho truong khoi
    ivec4 g_st0 = jxR.st0; ivec4 g_st0b = jxR.st0b; ivec4 g_st1 = jxR.st1; ivec4 g_st1b = jxR.st1b; vec4 g_at = jxR.at;
#endif
    vec4 dif = vCol;
    vec4 cur = dif;
    if (g_st0.x != 1)   // stage 0 khong DISABLE
    {
        vec4 tex0 = (g_st0b.z != 0) ? JX_TEX0(vUv) : vec4(1.0);
        if (JX_PALROW != JX_PALKHONG && g_st0b.z != 0)
        {   // texture chi so (R8G8): R = chi so bang mau, G = alpha
            if (g_st0b.w != 0)
            {   // loc tuyen tinh: lay 4 diem, tra bang tung diem roi noi suy; khong noi suy CHI SO
                ivec2 dim = JX_DIM0;
                vec2 p = vUv * vec2(dim) - 0.5; vec2 f = fract(p); ivec2 p0 = ivec2(floor(p)); ivec2 mx = dim - 1;
                vec4 c00 = PalTex(JX_FETCH0(clamp(p0, ivec2(0), mx)), JX_PALROW);
                vec4 c10 = PalTex(JX_FETCH0(clamp(p0 + ivec2(1, 0), ivec2(0), mx)), JX_PALROW);
                vec4 c01 = PalTex(JX_FETCH0(clamp(p0 + ivec2(0, 1), ivec2(0), mx)), JX_PALROW);
                vec4 c11 = PalTex(JX_FETCH0(clamp(p0 + ivec2(1, 1), ivec2(0), mx)), JX_PALROW);
                tex0 = mix(mix(c00, c10, f.x), mix(c01, c11, f.x), f.y);
            }
            else
                tex0 = PalTex(tex0, JX_PALROW);
        }
        cur = Stage(g_st0, g_st0b, dif, dif, tex0);
        if (g_st1.x != 1)
        {
            vec4 tex1 = (g_st1b.z != 0) ? JX_TEX1(vUv) : vec4(1.0);
            cur = Stage(g_st1, g_st1b, dif, cur, tex1);
        }
    }
    if (g_at.x > 0.5)
    {
        // so sanh tren alpha 8 bit nhu phan cung D3D9
        int a8 = int(floor(cur.a * 255.0 + 0.5));
        int r8 = int(g_at.z);
        int f = int(g_at.y);
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
    outColor = cur;
}
