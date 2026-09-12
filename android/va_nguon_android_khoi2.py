# -*- coding: utf-8 -*-
r"""[KHOI2 11/09] NOI TRAN KHOI ATLAS: 8 khoi -> 12 khoi, bang cach chia lai o PALROW.

Do phien SM-F966U1_20260911_185508 (ban [KHOI], APK 109111847), sau ~35 phut Tong Kim:
  atlas khoi=1: 8 khoi (8 lop/khoi, 512 MB), het khoi 11
  texture/sampler 41-65 lan/khung (dinh 128-172)   <- luc moi vao chi 19-24
Tam khoi da day (tran 4 + 7*8 = 60 trang), 11 trang atlas phai lui ve texture rieng, va moi trang lui ve la mot
nguon doi texture moi. Chua chua thi con so nay chi tang dan.

Cho lay bit: log noi suot ca phien "ps bang 5 muc (tran 0)" - truong chi so to hop trang thai tang texture dang
duoc cap 12 bit (4096 muc) ma thuc te chi dung 5. Cat xuong 11 bit (2048 muc, van gap 400 lan nhu cau) la du mot
bit cho truong khoi.

O PALROW (32 bit moi dinh), truoc va sau:
  truoc: 0..12 hang bang mau | 13..24 chi so ps (12 bit) | 25..27 lop | 28..30 khoi (3 bit = 8) | 31 co
  sau  : 0..12 hang bang mau | 13..23 chi so ps (11 bit) | 24..26 lop | 27..30 khoi (4 bit = 16) | 31 co
So khoi thuc te dat 12 chu khong phai 16: SDL_GPU chot 16 khe sampler moi tang, dang dung 2 cho hai tang texture,
de 12 khoi o khe 2..13 thi hai storage buffer ve binding 14, 15 - con du 2 khe, khong dam sat tran.
Tran trang atlas: 4 + 11*8 = 92 trang (truoc 60). Phien nang nhat tu truoc den nay dung 56 trang.

Sua kem: bien the C1 (JX_TEX_ARRAY, dang tat) cung doi lop sang bit 24..29 cho khop bo cuc moi, de sau nay bat lai
khong bi lech ngam.
Doc/ghi latin-1, giu CRLF/LF, moc khop dung 1 cho, byte cao khong doi; chay lai nhieu lan khong sao.
Dung:  python android\va_nguon_android_khoi2.py   (roi chay lai ReverseTools\mobile_x64\dich_shader_gpu.py)
"""
import io
import os
import re

GOC = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
R3 = os.path.join(GOC, "Sources", "Represent", "Represent3")
DAU = "[KHOI2 11/09]"
KHOI = 12          # so khoi (khe sampler 2..13)
BUF0 = 2 + KHOI    # binding cua storage buffer bang mau
BUF1 = BUF0 + 1    # binding cua storage buffer bang ps


def doc(p):
    s = io.open(p, encoding="latin-1", newline="").read()
    crlf = s.count("\r\n")
    if crlf and crlf != s.count("\n"):
        raise SystemExit("tep co xuong dong lan lon, khong va: " + p)
    return s.replace("\r\n", "\n"), ("\r\n" if crlf else "\n"), sum(1 for c in s if ord(c) >= 0x80)


def ghi(p, s, nl, cao):
    if sum(1 for c in s if ord(c) >= 0x80) != cao:
        raise SystemExit("so byte cao doi - khong ghi: " + p)
    io.open(p, "w", encoding="latin-1", newline="").write(s.replace("\n", nl))
    print("da va:", os.path.relpath(p, GOC))


def thay1(s, cu, moi, ten):
    n = s.count(cu)
    if n != 1:
        raise SystemExit("moc '%s' khop %d cho (can 1)" % (ten, n))
    return s.replace(cu, moi)


# ============================================================ Rep3ShadersGPU.frag
p = os.path.join(R3, "Rep3ShadersGPU.frag")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    i0 = s.index("#ifdef JX_ATLAS_KHOI\n")
    i1 = s.index("#elif defined(JX_TEX_ARRAY)\n")
    khai = "\n".join("layout(set = 2, binding = %d) uniform sampler2DArray g_k%d;" % (2 + i, i) for i in range(KHOI))
    sw_tex = "\n".join("    %sif (k == %du) return texture(g_k%d, vec3(uv, l));" % ("" if i == 0 else "else ", i, i) for i in range(KHOI))
    sw_fet = "\n".join("    %sif (k == %du) return texelFetch(g_k%d, ivec3(p, l), 0);" % ("" if i == 0 else "else ", i, i) for i in range(KHOI))
    sw_dim = "\n".join("    %sif (k == %du) return textureSize(g_k%d, 0).xy;" % ("" if i == 0 else "else ", i, i) for i in range(KHOI))
    moi = """#ifdef JX_ATLAS_KHOI
// [KHOI 11/09] (chi Android) port buoc (f) cua loat [MANG 09/09] ben duong D3D11 (commit 5311778b): atlas theo KHOI CO DINH.
// Moi khoi = mot texture mang 2D nhieu lop, GAN CHET vao mot khe sampler va KHONG BAO GIO doi trong ca khung ->
// hai quad o hai trang atlas khac nhau van gop chung mot lenh ve.
// Khe 0 (g_t0) van la sampler2D cho texture RIENG (khong vao atlas) - khong ep texture thuong thanh mang.
// %s NOI TRAN: 8 -> %d khoi (khe 2..%d), lay mot bit tu truong chi so ps (12 -> 11 bit; do thuc te chi dung 5 muc).
// O PALROW: bit 0..12 hang bang mau | 13..23 chi so ps | 24..26 lop | 27..30 khoi | 31 = lay tu khoi atlas.
// SDL_GPU chot 16 khe sampler MOI TANG nen dung %d khoi la con du 2 khe; hai storage buffer ve binding %d, %d.
layout(set = 2, binding = 0) uniform sampler2D g_t0;
layout(set = 2, binding = 1) uniform sampler2D g_t1;
%s

#define JX_KHOI_CO   ((vPal & 0x80000000u) != 0u)
#define JX_KHOI_I    ((vPal >> 27) & 0xFu)
#define JX_KHOI_L    float((vPal >> 24) & 7u)
#define JX_KHOI_LI   int((vPal >> 24) & 7u)

vec4 JxKhoiTex(vec2 uv)
{
    uint k = JX_KHOI_I; float l = JX_KHOI_L;
%s
    return vec4(1.0);
}
vec4 JxKhoiFetch(ivec2 p)
{
    uint k = JX_KHOI_I; int l = JX_KHOI_LI;
%s
    return vec4(1.0);
}
ivec2 JxKhoiDim()
{
    uint k = JX_KHOI_I;
%s
    return ivec2(1, 1);
}
#define JX_TEX0(uv)  (JX_KHOI_CO ? JxKhoiTex(uv)  : texture(g_t0, uv))
#define JX_TEX1(uv)  texture(g_t1, uv)
#define JX_FETCH0(p) (JX_KHOI_CO ? JxKhoiFetch(p) : texelFetch(g_t0, p, 0))
#define JX_DIM0      (JX_KHOI_CO ? JxKhoiDim()    : textureSize(g_t0, 0))
#define JX_BUF0      %d
#define JX_BUF1      %d
""" % (DAU, KHOI, 2 + KHOI - 1, KHOI, BUF0, BUF1, khai, sw_tex, sw_fet, sw_dim, BUF0, BUF1)
    s = s[:i0] + moi + s[i1:]
    # bien the C1: lop doi sang bit 24..29 cho khop bo cuc moi
    s = thay1(s, "#define JX_LOP0      float((vPal >> 25) & 0x3Fu)",
              "#define JX_LOP0      float((vPal >> 24) & 0x3Fu)\t// " + DAU + " bo cuc moi: chi so ps chi con 11 bit (13..23)", "JX_LOP0")
    s = thay1(s, "#define JX_FETCH0(p) texelFetch(g_t0, ivec3(p, int((vPal >> 25) & 0x3Fu)), 0)",
              "#define JX_FETCH0(p) texelFetch(g_t0, ivec3(p, int((vPal >> 24) & 0x3Fu)), 0)\t// " + DAU, "JX_FETCH0 mang")
    # chi so ps: 12 -> 11 bit
    s = thay1(s, "    JxPsRec jxR = g_psBuf[(vPal >> 13) & 0xFFFu];",
              "    JxPsRec jxR = g_psBuf[(vPal >> 13) & 0x7FFu];   // " + DAU + " 11 bit (2048 muc; do thuc te 5 muc) - nhuong 1 bit cho truong khoi", "chi so ps")
    s = thay1(s, "#define JX_PALROW   (vPal & 0x1FFFu)   // [GOP 11/09] o PALROW: bit 0..12 = hang bang mau, 13..24 = chi so to hop trang thai tang texture",
              "#define JX_PALROW   (vPal & 0x1FFFu)   // [GOP 11/09] o PALROW: bit 0..12 = hang bang mau, 13..23 = chi so to hop trang thai tang texture (" + DAU + ")", "chu thich PALROW")
    ghi(p, s, nl, cao)

# ============================================================ D3D9onGPUDev.cpp
p = os.path.join(R3, "D3D9onGPUDev.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = thay1(s, "#define JX_PS_MAX 4096\t// [GOP 11/09] so to hop trang thai tang texture toi da trong mot khung (chi so 12 bit trong o PALROW)",
        "#define JX_PS_MAX 2048\t// [GOP 11/09] so to hop trang thai tang texture toi da trong mot khung (chi so 11 bit trong o PALROW sau " + DAU + ";\n"
        "\t\t\t\t\t\t// do that te suot ca phien Tong Kim: 5 muc, tran 0 - nen 2048 van la thua 400 lan)",
        "JX_PS_MAX")
    s = thay1(s, "\t\tuPal = uRow | ((JxPsIdx(st.ps) & 0xFFFu) << 13);\n",
        "\t\tuPal = uRow | ((JxPsIdx(st.ps) & 0x7FFu) << 13);\t// " + DAU + " 11 bit\n", "chi so ps")
    s = thay1(s, "\t\tif (g_nJxAtlasMang && m_tex[0]) uPal |= ((m_tex[0]->JxLop() & 0x3Fu) << 25);\t// [MANG 11/09] bit 25..30 = lop trong texture mang\n",
        "\t\tif (g_nJxAtlasMang && m_tex[0]) uPal |= ((m_tex[0]->JxLop() & 0x3Fu) << 24);\t// [MANG 11/09] lop trong texture mang (" + DAU + " doi sang bit 24..29)\n",
        "lop cua C1")
    s = thay1(s, "\t\t\tuPal |= 0x80000000u | ((m_tex[0]->JxKhoi() & 7u) << 28) | ((m_tex[0]->JxLop() & 7u) << 25);\t// [KHOI 11/09] bit 31 = o khoi, 28..30 khoi, 25..27 lop\n",
        "\t\t\tuPal |= 0x80000000u | ((m_tex[0]->JxKhoi() & 0xFu) << 27) | ((m_tex[0]->JxLop() & 7u) << 24);\t// [KHOI 11/09] bit 31 = o khoi, 27..30 khoi (" + DAU + " 4 bit), 24..26 lop\n",
        "bit khoi")
    s = thay1(s, "\t\t\t\tsi.code = g_Rep3GpuFSPalPsKhoi; si.code_size = sizeof(g_Rep3GpuFSPalPsKhoi); si.num_samplers = 2 + 8;\n",
        "\t\t\t\tsi.code = g_Rep3GpuFSPalPsKhoi; si.code_size = sizeof(g_Rep3GpuFSPalPsKhoi); si.num_samplers = 2 + 12;\t// " + DAU + " 12 khoi\n",
        "so sampler")
    s = thay1(s, "\t\t\t\tSDL_GPUTextureSamplerBinding tk[8];\n"
        "\t\t\t\tfor (int q = 0; q < 8; q++)\n",
        "\t\t\t\tSDL_GPUTextureSamplerBinding tk[12];\t// " + DAU + "\n"
        "\t\t\t\tfor (int q = 0; q < 12; q++)\n", "vong gan khoi")
    s = thay1(s, "\t\t\t\tSDL_BindGPUFragmentSamplers(pass, 2, tk, 8);\n",
        "\t\t\t\tSDL_BindGPUFragmentSamplers(pass, 2, tk, 12);\t// " + DAU + "\n", "gan khoi")
    s = thay1(s, "\t\t\t{\t// [KHOI 11/09] 8 khoi atlas o khe 2..9 (sampler2DArray, gan chet ca khung); hai storage buffer doi ve 10, 11\n",
        "\t\t\t{\t// [KHOI 11/09] " + str(KHOI) + " khoi atlas o khe 2.." + str(2 + KHOI - 1) + " (sampler2DArray, gan chet ca khung); hai storage buffer doi ve " + str(BUF0) + ", " + str(BUF1) + " (" + DAU + ")\n",
        "chu thich chon shader")
    s = thay1(s, "\t\t\t{\t// [KHOI 11/09] 8 khoi atlas o khe 2..9. Gan bang BO LOC cua tang 0 (lenh gop duoc da phai cung sampler[0] nen khong sai)\n",
        "\t\t\t{\t// [KHOI 11/09] " + str(KHOI) + " khoi atlas o khe 2.." + str(2 + KHOI - 1) + ". Gan bang BO LOC cua tang 0 (lenh gop duoc da phai cung sampler[0] nen khong sai)\n",
        "chu thich gan khoi")
    ghi(p, s, nl, cao)

# ============================================================ D3D9onGPURes.cpp
p = os.path.join(R3, "D3D9onGPURes.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = thay1(s, "#define JX_KHOI_MAX 8\t// so khe sampler danh cho khoi (khe 2..9); SDL_GPU chot 16 khe moi tang\n",
        "#define JX_KHOI_MAX " + str(KHOI) + "\t// " + DAU + " so khe sampler danh cho khoi (khe 2.." + str(2 + KHOI - 1) + "); SDL_GPU chot 16 khe moi tang, con du 2 khe.\n"
        "\t\t\t\t\t\t// Tran trang atlas = 4 (khoi 4 byte) + " + str(KHOI - 1) + "*8 = " + str(4 + (KHOI - 1) * 8) + " trang. Do phien 185508: 8 khoi het cho sau ~35 phut Tong Kim (het khoi 11).\n",
        "JX_KHOI_MAX")
    ghi(p, s, nl, cao)

print("xong - nho chay lai: python ReverseTools\\mobile_x64\\dich_shader_gpu.py <goc>")
