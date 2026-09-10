# -*- coding: utf-8 -*-
"""goi_va_mang_d_0909.py - [MANG 09/09 d] (1) texture MANAGED (chu KFont3 512x512 A4R4G4B4 pool 1) duoc vao atlas;
(2) tham so stage 0 (colorOp/args, alphaOp/args, tex0 bound) theo DINH -> khong con la ly do vo lo.

So do 18:2x (ban MANG c): lo quad con vo 100-450 lan/khung vi chu (texture rieng <-> atlas) va 40-460 lan/khung vi doi
tham so stage 0. Ring 16 MB: 0 vong giua khung. Ket qua tung diem anh y het (cung phep tron, cung texel).
Dinh: y = alpha ref (8) | colorOp (4) <<8 | cArg1 (4) <<12 | cArg2 (4) <<16 | alphaOp (4) <<20 | aArg1 (4) <<24 | aArg2 (4) <<28;
arg goi = sel (2 bit, >3 -> 3 = trang) | complement <<2 | alphareplicate <<3; op > 15 -> 15 (shader chi cai <= 13, con lai = cur).
x bit 31 = tex0 bound. Ban cu (Rep3AtlasMang=0): cb nhu truoc.
"""
import io
import os
import subprocess
import sys

D = "D:/GAMEDEVNEW_wt_delta/Sources/Represent/Represent3/"
TAG = "[MANG 09/09 d]"
FXC = r"C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64\fxc.exe"
T = "\t"


def doc(p):
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    return s, sum(1 for c in s if ord(c) >= 0x80), s.count("\n") - s.count("\r\n"), s.count("\r\n")


def ghi(p, s, h0, lf0, crlf0, ten):
    ok = (s.count("\n") - s.count("\r\n") == lf0) if crlf0 else (s.count("\r\n") == 0)
    if not ok or sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s:
        print("FAIL ma hoa " + ten); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("OK " + ten)


def rep(s, old, new, ten, n_mong=1):
    n = s.count(old)
    if n != n_mong:
        print("FAIL neo %s: %d (mong %d)" % (ten, n, n_mong)); sys.exit(1)
    return s.replace(old, new)


# ====================================================================== 1. HLSL
P = D + "Rep3Shaders11.hlsl"
s, h0, lf0, crlf0 = doc(P)
NL = "\r\n" if crlf0 else "\n"
if TAG not in s:
    # ham mo goi arg (MANG) - dat truoc PS: chen ngay truoc 'float4 PS(VSOut i) : SV_Target'
    s = rep(s, "float4 PS(VSOut i) : SV_Target" + NL + "{" + NL,
            NL.join([
                "#ifdef MANG",
                "// " + TAG + " arg stage 0 goi 4 bit theo dinh: sel (2) | complement (1) | alphareplicate (1) -> ma D3DTA nhu cu",
                "int ArgMo(uint a) { return (int)((a & 3u) | (((a & 4u) != 0u) ? 0x10u : 0u) | (((a & 8u) != 0u) ? 0x20u : 0u)); }",
                "#endif",
                "float4 PS(VSOut i) : SV_Target",
                "{", ""]), "hlsl ArgMo")
    s = rep(s, "    float lop = (float)((i.palrow.x >> 16) & 0x1FFu); uint src = (i.palrow.x >> 25) & 3u;   // [MANG 09/09 b] lop + nguon theo dinh" + NL,
            NL.join([
                "    float lop = (float)((i.palrow.x >> 16) & 0x1FFu); uint src = (i.palrow.x >> 25) & 3u;   // [MANG 09/09 b] lop + nguon theo dinh",
                "    // " + TAG + " tham so stage 0 theo dinh (y) + tex0 bound (x bit 31); loc tuyen tinh van tu cb (g_st0b.w)",
                "    uint y = i.palrow.y;",
                "    int4 st0 = int4((int)((y >> 8) & 15u), ArgMo((y >> 12) & 15u), ArgMo((y >> 16) & 15u), (int)((y >> 20) & 15u));",
                "    int4 st0b = int4(ArgMo((y >> 24) & 15u), ArgMo((y >> 28) & 15u), (int)((i.palrow.x >> 31) & 1u), g_st0b.w);",
                "#else",
                "    int4 st0 = g_st0; int4 st0b = g_st0b;",
                ""]), "hlsl PS dau")
    s = rep(s, "    if (g_st0.x != 1)   // stage 0 khong DISABLE" + NL, "    if (st0.x != 1)   // stage 0 khong DISABLE" + NL, "hlsl st0.x")
    s = rep(s, "        float4 tex0 = (g_st0b.z != 0) ? T0_SAMPLE(i.uv) : float4(1, 1, 1, 1);" + NL, "        float4 tex0 = (st0b.z != 0) ? T0_SAMPLE(i.uv) : float4(1, 1, 1, 1);" + NL, "hlsl tex0")
    s = rep(s, "        if (row != 0xFFFFu && g_st0b.z != 0)" + NL, "        if (row != 0xFFFFu && st0b.z != 0)" + NL, "hlsl row bound")
    s = rep(s, "            if (g_st0b.w != 0)" + NL, "            if (st0b.w != 0)" + NL, "hlsl linear")
    s = rep(s, "        cur = Stage(g_st0, g_st0b, dif, dif, tex0);" + NL, "        cur = Stage(st0, st0b, dif, dif, tex0);" + NL, "hlsl Stage")
    ghi(P, s, h0, lf0, crlf0, "Rep3Shaders11.hlsl")
else:
    print("Rep3Shaders11.hlsl da co")

# kiem: sau khi thay, PS khong con doc g_st0 truc tiep (tru dong int4 st0 = g_st0 va g_st0b.w)
s = io.open(P, "r", encoding="latin-1", newline="").read()
i = s.find("float4 PS(VSOut i) : SV_Target")
body = s[i:]
for tok in ["g_st0.x", "g_st0.y", "g_st0b.z", "Stage(g_st0"]:
    if tok in body:
        print("FAIL PS van dung " + tok); sys.exit(1)

if not os.path.exists(FXC):
    print("FAIL khong thay fxc"); sys.exit(1)
for args, ten in [(["/T", "ps_4_0", "/E", "PS", "/Fh", "Rep3Shaders11_ps.h", "/Vn", "g_Rep3PS11", "Rep3Shaders11.hlsl"], "PS cu"),
                  (["/T", "ps_4_0", "/E", "PS", "/D", "MANG=1", "/Fh", "Rep3Shaders11_psmang.h", "/Vn", "g_Rep3PS11Mang", "Rep3Shaders11.hlsl"], "PS MANG")]:
    r = subprocess.run([FXC, "/nologo"] + args, cwd=D, capture_output=True, text=True)
    if r.returncode != 0:
        print("FAIL fxc %s:\n%s\n%s" % (ten, r.stdout, r.stderr)); sys.exit(1)
    print("OK fxc " + ten)

# ====================================================================== 2. D3D9on11Atlas.cpp: MANAGED vao atlas
P = D + "D3D9on11Atlas.cpp"
s, h0, lf0, crlf0 = doc(P)
NL = "\r\n" if crlf0 else "\n"
if TAG not in s:
    s = rep(s, T + "if (pool != D3DPOOL_DEFAULT) return false;" + NL,
            T + "if (pool != D3DPOOL_DEFAULT && pool != D3DPOOL_MANAGED) return false;" + T + "// " + TAG + " MANAGED (chu KFont3 512x512 A4R4G4B4) cung vao atlas: ban CPU + UploadRect vung ban da ho tro texture ao" + NL, "A eligible")
    ghi(P, s, h0, lf0, crlf0, "D3D9on11Atlas.cpp")
else:
    print("D3D9on11Atlas.cpp da co")

# ====================================================================== 3. D3D9on11Dev.cpp
P = D + "D3D9on11Dev.cpp"
s, h0, lf0, crlf0 = doc(P)
NL = "\r\n" if crlf0 else "\n"
if TAG not in s:
    s = rep(s, "static inline void R11DinhThem(CTex11* pTex, DWORD dwAtBat, DWORD dwAtHam, DWORD dwAtRef, UINT* pX, UINT* pY)" + NL + "{" + NL,
            NL.join([
                "static inline UINT R11ArgGoi(DWORD a) { UINT sel = a & 15u; if (sel > 3u) sel = 3u; return sel | ((a & 0x10u) ? 4u : 0u) | ((a & 0x20u) ? 8u : 0u); }" + T + "// " + TAG,
                "static inline UINT R11OpGoi(DWORD v) { return (v > 15u) ? 15u : (UINT)v; }",
                "static inline void R11DinhThem(CTex11* pTex, DWORD dwAtBat, DWORD dwAtHam, DWORD dwAtRef, const DWORD* pTss0, int nBound, UINT* pX, UINT* pY)",
                "{", ""]), "V DinhThem ky")
    s = rep(s, T + "*pX = x; *pY = (UINT)(dwAtRef & 255u);" + NL + "}" + NL,
            NL.join([
                T + "if (nBound) x |= (1u << 31);" + T + "// " + TAG + " tex0 bound",
                T + "UINT y = (UINT)(dwAtRef & 255u);",
                T + "y |= (R11OpGoi(pTss0[D3DTSS_COLOROP]) << 8) | (R11ArgGoi(pTss0[D3DTSS_COLORARG1]) << 12) | (R11ArgGoi(pTss0[D3DTSS_COLORARG2]) << 16)",
                T + "   | (R11OpGoi(pTss0[D3DTSS_ALPHAOP]) << 20) | (R11ArgGoi(pTss0[D3DTSS_ALPHAARG1]) << 24) | (R11ArgGoi(pTss0[D3DTSS_ALPHAARG2]) << 28);",
                T + "*pX = x; *pY = y;",
                "}", ""]), "V DinhThem than")
    s = rep(s, "R11DinhThem(m_tex[0], m_rs[D3DRS_ALPHATESTENABLE], m_rs[D3DRS_ALPHAFUNC], m_rs[D3DRS_ALPHAREF], &uX, &uY);",
            "R11DinhThem(m_tex[0], m_rs[D3DRS_ALPHATESTENABLE], m_rs[D3DRS_ALPHAFUNC], m_rs[D3DRS_ALPHAREF], m_tss[0], (m_tex[0] && !(m_pRt && m_pRt->m_pTex && m_tex[0] == m_pRt->m_pTex)) ? 1 : 0, &uX, &uY);", "V goi", 2)
    old = (T + "cb.st0[0] = (int)m_tss[0][D3DTSS_COLOROP]; cb.st0[1] = (int)m_tss[0][D3DTSS_COLORARG1]; cb.st0[2] = (int)m_tss[0][D3DTSS_COLORARG2]; cb.st0[3] = (int)m_tss[0][D3DTSS_ALPHAOP];" + NL +
           T + "cb.st0b[0] = (int)m_tss[0][D3DTSS_ALPHAARG1]; cb.st0b[1] = (int)m_tss[0][D3DTSS_ALPHAARG2]; cb.st0b[2] = nBound[0];" + NL)
    new = NL.join([
        T + "if (g_nRep3AtlasMang) { cb.st0[0] = cb.st0[1] = cb.st0[2] = cb.st0[3] = 0; cb.st0b[0] = cb.st0b[1] = cb.st0b[2] = 0; }" + T + "// " + TAG + " stage 0 theo dinh -> cb co dinh, khong vo lo",
        T + "else",
        T + "{",
        T*2 + "cb.st0[0] = (int)m_tss[0][D3DTSS_COLOROP]; cb.st0[1] = (int)m_tss[0][D3DTSS_COLORARG1]; cb.st0[2] = (int)m_tss[0][D3DTSS_COLORARG2]; cb.st0[3] = (int)m_tss[0][D3DTSS_ALPHAOP];",
        T*2 + "cb.st0b[0] = (int)m_tss[0][D3DTSS_ALPHAARG1]; cb.st0b[1] = (int)m_tss[0][D3DTSS_ALPHAARG2]; cb.st0b[2] = nBound[0];",
        T + "}", ""])
    s = rep(s, old, new, "V cb st0")
    ghi(P, s, h0, lf0, crlf0, "D3D9on11Dev.cpp")
else:
    print("D3D9on11Dev.cpp da co")
print("XONG " + TAG)
