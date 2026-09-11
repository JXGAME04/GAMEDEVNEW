#version 450
// [GPU 08/09] Shader diem anh cho lop D3D9 tren SDL_GPU - chuyen tu Rep3Shaders11.hlsl (PS): mo phong 2 texture stage cua duong co dinh
// D3D9 (D3DTOP_* / D3DTA_*), bang mau (texture chi so R8G8: R = chi so, G = alpha) va alpha test.
// Dich: glslc -fshader-stage=frag -O Rep3ShadersGPU.frag -o Rep3ShadersGPU.frag.spv
// SDL_GPU: sampler cua fragment o set 2 (binding 0 = t0, 1 = t1, 2 = bang mau), uniform cua fragment o set 3.

layout(location = 0) in vec4 vCol;
layout(location = 1) in vec2 vUv;
layout(location = 2) flat in uint vPal;

layout(set = 2, binding = 0) uniform sampler2D g_t0;
layout(set = 2, binding = 1) uniform sampler2D g_t1;
#ifdef JX_PAL_BUFFER
// [PALBUF 11/09] (chi Android, -DJX_PAL_BUFFER) bang mau = storage buffer 8192 hang x 256 mau BGRA8 (uint), hang = vPal; set 2 binding 2 = ngay sau 2 sampler
layout(std430, set = 2, binding = 2) readonly buffer PalBuf { uint g_palBuf[]; };
#else
layout(set = 2, binding = 2) uniform sampler2D g_pal;   // atlas bang mau 256 x N (BGRA8), hang = vPal
#endif

layout(set = 3, binding = 0) uniform PSCB
{
    ivec4 g_st0;   // stage 0: colorOp, colorArg1, colorArg2, alphaOp   (D3DTOP_* / D3DTA_*)
    ivec4 g_st0b;  // stage 0: alphaArg1, alphaArg2, tex0 bound, loc tuyen tinh (1) -> tu noi suy texture bang mau
    ivec4 g_st1;   // stage 1: colorOp, colorArg1, colorArg2, alphaOp
    ivec4 g_st1b;  // stage 1: alphaArg1, alphaArg2, tex1 bound, 0
    vec4  g_at;    // x = alpha test bat, y = D3DCMP_*, z = alpha ref (0..255), w = 0
};

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
    vec4 dif = vCol;
    vec4 cur = dif;
    if (g_st0.x != 1)   // stage 0 khong DISABLE
    {
        vec4 tex0 = (g_st0b.z != 0) ? texture(g_t0, vUv) : vec4(1.0);
        if (vPal != 0xFFFFu && g_st0b.z != 0)
        {   // texture chi so (R8G8): R = chi so bang mau, G = alpha
            if (g_st0b.w != 0)
            {   // loc tuyen tinh: lay 4 diem, tra bang tung diem roi noi suy; khong noi suy CHI SO
                ivec2 dim = textureSize(g_t0, 0);
                vec2 p = vUv * vec2(dim) - 0.5; vec2 f = fract(p); ivec2 p0 = ivec2(floor(p)); ivec2 mx = dim - 1;
                vec4 c00 = PalTex(texelFetch(g_t0, clamp(p0, ivec2(0), mx), 0), vPal);
                vec4 c10 = PalTex(texelFetch(g_t0, clamp(p0 + ivec2(1, 0), ivec2(0), mx), 0), vPal);
                vec4 c01 = PalTex(texelFetch(g_t0, clamp(p0 + ivec2(0, 1), ivec2(0), mx), 0), vPal);
                vec4 c11 = PalTex(texelFetch(g_t0, clamp(p0 + ivec2(1, 1), ivec2(0), mx), 0), vPal);
                tex0 = mix(mix(c00, c10, f.x), mix(c01, c11, f.x), f.y);
            }
            else
                tex0 = PalTex(tex0, vPal);
        }
        cur = Stage(g_st0, g_st0b, dif, dif, tex0);
        if (g_st1.x != 1)
        {
            vec4 tex1 = (g_st1b.z != 0) ? texture(g_t1, vUv) : vec4(1.0);
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
