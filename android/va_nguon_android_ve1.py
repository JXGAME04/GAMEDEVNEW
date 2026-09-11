# -*- coding: utf-8 -*-
r"""[VE 11/09] Android: (A) do tung buoc trinh chieu SubmitFrame + (B) nap KHUNG sprite o luong nen theo ngan sach. CHI Android (#ifdef JX_ANDROID).
Boi canh (BANGIAO_DONHIP_MOBILE_1209.md, phien y2 Fold 7 02:07): dam dong toi giat la PHIA VE - moi khung anh NPC lan dau duoc ve thi
rut khung tu pak + giai ma RLE + tao texture ngay tren luong ve (khung >16 ms: 5-7 lan/30 s, max 86 ms/khung); con chi phi trinh chieu
(SubmitFrame: cho swapchain, chep len GPU, ghi lenh Vulkan, nop) chua he do.

  A. D3D9onGPUDev.cpp: do cho/chep/ghi lenh/nop moi khung + dem doi pipeline/texture/uniform/cat + ly do quad khong gop
     -> KRepresentShell3.cpp in [VE] + [VE-GOP] moi ky Rep3StatSec (jx_rep3.log) va [VE-GIAT] cho khung ve CPU + trinh chieu > VeGiatMs.
  B. TextureRes/TextureResMgr: PrepareFrameData khi DANG VE va tong nap dong bo cua khung da qua NapKhungMs -> giao khung cho luong nen
     (rut khung + giai ma vao bo dem), bo ve khung nay; luong ve tao texture o dau khung sau theo ngan sach NapKhungApMs; nap truoc
     NapKhungTruoc khung ke tiep cung huong. Sprite bi xoa -> huy viec (JxNapKhungHuy). Thong ke [VE-NAP].
  C. config.ini lop ghi de: NapKhungNen/NapKhungMs/NapKhungTruoc/NapKhungApMs/VeGiatMs.
Doc/ghi latin-1 (TCVN3/GBK, CRLF), moi moc khop dung 1 cho (regex tren ban LF), so byte cao khong doi; chay lai nhieu lan khong sao.
Dung:  python android\va_nguon_android_ve1.py
"""
import io
import os
import re

GOC = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[VE 11/09]"
R3 = os.path.join(GOC, "Sources", "Represent", "Represent3")


def doc(p):
    s = io.open(p, encoding="latin-1", newline="").read()
    nl = "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"
    if nl == "\r\n" and s.count("\r\n") != s.count("\n"):
        raise SystemExit("tep co xuong dong lan lon, khong va: " + p)
    return s.replace("\r\n", "\n"), nl, sum(1 for c in s if ord(c) >= 0x80)


def ghi(p, s, nl, cao):
    if sum(1 for c in s if ord(c) >= 0x80) != cao:
        raise SystemExit("so byte cao doi - khong ghi: " + p)
    for c in DAU:
        assert ord(c) < 0x80
    io.open(p, "w", encoding="latin-1", newline="").write(s.replace("\n", nl))
    print("da va:", os.path.relpath(p, GOC))


def mot(s, rx, ten):
    m = list(re.finditer(rx, s, re.M))
    if len(m) != 1:
        raise SystemExit("khong tim thay dung 1 cho (%d): %s" % (len(m), ten))
    return m[0]


def chen_sau(s, rx, them, ten):
    m = mot(s, rx, ten)
    return s[:m.end()] + "\n" + them + s[m.end():]


def chen_truoc(s, rx, them, ten):
    m = mot(s, rx, ten)
    return s[:m.start()] + them + "\n" + s[m.start():]


def thay(s, rx, moi, ten):
    m = mot(s, rx, ten)
    return s[:m.start()] + moi + s[m.end():]


def thay_nhom(s, rx, moi, ten):
    """nhu thay() nhung moi duoc mo rong \1.. (nhom bat trong rx)"""
    m = mot(s, rx, ten)
    return s[:m.start()] + m.expand(moi) + s[m.end():]


def A(*dong):
    """noi cac dong bang LF, moi dong la chuoi ASCII"""
    for d in dong:
        for c in d:
            if ord(c) >= 0x80:
                raise SystemExit("dong moi co byte cao: " + d)
    return "\n".join(dong)


# ============================================================ 1. BaseInclude.h: khai bao chung (Android)
p = os.path.join(R3, "BaseInclude.h")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    s = chen_sau(s, r'^double Rep3NapMs\(const LARGE_INTEGER& a, const LARGE_INTEGER& b\);$', A(
        "#ifdef JX_ANDROID",
        "// " + DAU + " Android: (A) do tung buoc trinh chieu CDevGpu::SubmitFrame; (B) nap KHUNG sprite o luong nen theo ngan sach.",
        "//   In jx_rep3.log: [VE] + [VE-GOP] + [VE-NAP] moi ky Rep3StatSec (KRepresentShell3.cpp JxVeKyIn), [VE-GIAT] khung cham (JxVeGiatGhi).",
        "struct JxVeDo { double dCho, dChep, dGhi, dNop, dTong; unsigned uTai, uTaiKB, uRingKB, uLenh, uQuad, uDinh, uPass, uDoiPipe, uDoiTex, uDoiVs, uDoiPs, uDoiCat; };",
        "extern JxVeDo g_jxVeKhung, g_jxVeTong, g_jxVeMax;\t\t// khung vua submit / cong don ky / max trong ky (D3D9onGPUDev.cpp ghi; KRepresentShell3.cpp doc + dat lai)",
        "extern unsigned g_uJxVeKhungSo, g_uJxVe8, g_uJxVe16, g_uJxGopVo[8];\t// so khung trong ky, khung SubmitFrame > 8 / > 16 ms, ly do quad khong gop (xem JxGopVo)",
        "class TextureResSpr; class TextureResMgr;",
        "extern TextureResMgr* g_pJxTexMgr;\t// bo quan ly texture duy nhat (TextureResMgr ctor gan)",
        "struct JxKhungViec { TextureResSpr* pSpr; int nFrame; int nBpp; int eFmt; int bPal; int nNguon; unsigned uLuc; };\t// viec cho luong nen (nNguon 0 = dang ve can, 1 = nap truoc)",
        "struct JxKhungXong { TextureResSpr* pSpr; int nFrame; int nW, nH, nOffX, nOffY; BYTE* pDiem; int nBpp; int eFmt; int bPal; int bHong; unsigned uLuc; };\t// ket qua: khung da giai ma",
        "extern int g_nJxNapKhungNen, g_nJxNapKhungMs, g_nJxNapKhungTruoc, g_nJxNapKhungApMs, g_nJxVeGiatMs;\t// [Client] NapKhungNen / NapKhungMs / NapKhungTruoc / NapKhungApMs / VeGiatMs",
        "extern int g_nJxAnhBoVeNen;\t// 1 = GetImage vua tra NULL vi khung dang nap o luong nen (Rep3AnhNullGhi bo qua, khong tinh la anh thieu)",
        "extern unsigned g_uJxNapKhungBoVe, g_uJxNapKhungBoVeKhung, g_uJxNapKhungDongBo, g_uJxNapKhungGiao, g_uJxNapKhungTruocSo, g_uJxNapKhungXong, g_uJxNapKhungHong, g_uJxNapKhungBo, g_uJxNapKhungChoMax;",
        "extern double g_dJxNapKhungTre, g_dJxNapKhungTreMax, g_dJxNapNenBan, g_dJxNapKhungAp, g_dJxNapKhungApMax; extern unsigned g_uJxNapKhungApKhung;",
        "extern Rep3NapDo g_jxNapNgoaiVe;\t// nap dong bo NGOAI luc ve (hoi kich thuoc / alpha tu logic) trong ky",
        "#endif"), "BaseInclude.h khai bao")
    ghi(p, s, nl, cao)

# ============================================================ 2. D3D9onGPUDev.cpp: do SubmitFrame + ly do khong gop quad
p = os.path.join(R3, "D3D9onGPUDev.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    s = chen_truoc(s, r'^#define RG_PAL_ROWS 8192$', A(
        "#ifdef JX_ANDROID",
        "// " + DAU + " do tung buoc SubmitFrame tren luong ve (cho lenh + swapchain, chep len GPU, ghi lenh render pass, nop) va dem doi trang thai;",
        "// KRepresentShell3.cpp in [VE]/[VE-GOP] moi ky va [VE-GIAT] cho khung cham. Chi cong khung co Present (bPresent).",
        "JxVeDo g_jxVeKhung, g_jxVeTong, g_jxVeMax;",
        "unsigned g_uJxVeKhungSo = 0, g_uJxVe8 = 0, g_uJxVe16 = 0, g_uJxGopVo[8];",
        "static double JxVeMs(Uint64 a, Uint64 b) { return (double)(b - a) * 1000.0 / (double)SDL_GetPerformanceFrequency(); }",
        "static void JxVeCong(const JxVeDo& k)",
        "{",
        "\tg_jxVeKhung = k; g_uJxVeKhungSo++;",
        "\tg_jxVeTong.dCho += k.dCho; g_jxVeTong.dChep += k.dChep; g_jxVeTong.dGhi += k.dGhi; g_jxVeTong.dNop += k.dNop; g_jxVeTong.dTong += k.dTong;",
        "\tg_jxVeTong.uTai += k.uTai; g_jxVeTong.uTaiKB += k.uTaiKB; g_jxVeTong.uRingKB += k.uRingKB; g_jxVeTong.uLenh += k.uLenh; g_jxVeTong.uQuad += k.uQuad; g_jxVeTong.uDinh += k.uDinh; g_jxVeTong.uPass += k.uPass;",
        "\tg_jxVeTong.uDoiPipe += k.uDoiPipe; g_jxVeTong.uDoiTex += k.uDoiTex; g_jxVeTong.uDoiVs += k.uDoiVs; g_jxVeTong.uDoiPs += k.uDoiPs; g_jxVeTong.uDoiCat += k.uDoiCat;",
        "\tif (k.dCho > g_jxVeMax.dCho) g_jxVeMax.dCho = k.dCho; if (k.dChep > g_jxVeMax.dChep) g_jxVeMax.dChep = k.dChep; if (k.dGhi > g_jxVeMax.dGhi) g_jxVeMax.dGhi = k.dGhi;",
        "\tif (k.dNop > g_jxVeMax.dNop) g_jxVeMax.dNop = k.dNop; if (k.dTong > g_jxVeMax.dTong) g_jxVeMax.dTong = k.dTong;",
        "\tif (k.uTaiKB > g_jxVeMax.uTaiKB) g_jxVeMax.uTaiKB = k.uTaiKB; if (k.uRingKB > g_jxVeMax.uRingKB) g_jxVeMax.uRingKB = k.uRingKB;",
        "\tif (k.uLenh > g_jxVeMax.uLenh) g_jxVeMax.uLenh = k.uLenh; if (k.uQuad > g_jxVeMax.uQuad) g_jxVeMax.uQuad = k.uQuad; if (k.uDoiTex > g_jxVeMax.uDoiTex) g_jxVeMax.uDoiTex = k.uDoiTex;",
        "\tif (k.dTong > 16.0) g_uJxVe16++; else if (k.dTong > 8.0) g_uJxVe8++;",
        "}",
        "// ly do quad KHONG gop duoc vao lenh truoc, kiem theo thu tu: 0 stride khac, 1 khong lien tiep trong ring (hoac lenh truoc la clear/target),",
        "// 2 pipeline, 3 texture 0, 4 texture 1 / sampler, 5 uniform vs, 6 uniform ps, 7 cat / viewport",
        "static void JxGopVo(const RgCmd& L, const RgDrawState& st, UINT stride, UINT ringOff, UINT s2)",
        "{",
        "\tint k;",
        "\tif (L.type != RGCMD_DRAW) k = 1;",
        "\telse if (L.stride != stride) k = 0;",
        "\telse if (L.ringOff + L.nVerts * s2 != ringOff) k = 1;",
        "\telse if (L.st.pPipe != st.pPipe) k = 2;",
        "\telse if (L.st.pTex[0] != st.pTex[0]) k = 3;",
        "\telse if (L.st.pTex[1] != st.pTex[1] || L.st.pSamp[0] != st.pSamp[0] || L.st.pSamp[1] != st.pSamp[1]) k = 4;",
        "\telse if (memcmp(&L.st.vs, &st.vs, sizeof(st.vs)) != 0) k = 5;",
        "\telse if (memcmp(&L.st.ps, &st.ps, sizeof(st.ps)) != 0) k = 6;",
        "\telse k = 7;",
        "\tg_uJxGopVo[k]++;",
        "}",
        "#endif",
        ""), "dinh nghia JxVe")
    s = chen_sau(s, r'^\t\t\t\{ L\.nVerts \+= 6; return D3D_OK; \}$', A(
        "#ifdef JX_ANDROID",
        "\t\t\tJxGopVo(L, st, stride, ringOff, s2);\t// " + DAU + " vi sao khong gop",
        "#endif"), "DrawInternal khong gop")
    s = thay(s, r'^bool CDevGpu::SubmitFrame\(bool bPresent\)\n\{\n\tSDL_GPUCommandBuffer\* cb = SDL_AcquireGPUCommandBuffer\(m_pGpu\);$', A(
        "bool CDevGpu::SubmitFrame(bool bPresent)",
        "{",
        "#ifdef JX_ANDROID",
        "\tJxVeDo jxK; memset(&jxK, 0, sizeof(jxK)); const Uint64 uJxK0 = SDL_GetPerformanceCounter(); Uint64 uJxK1 = uJxK0;\t// " + DAU,
        "#endif",
        "\tSDL_GPUCommandBuffer* cb = SDL_AcquireGPUCommandBuffer(m_pGpu);"), "SubmitFrame dau")
    s = chen_truoc(s, r'^\t// ---- copy pass: bang mau, texture, ring dinh$', A(
        "#ifdef JX_ANDROID",
        "\tuJxK1 = SDL_GetPerformanceCounter(); jxK.dCho = JxVeMs(uJxK0, uJxK1);\t// " + DAU + " cho lenh + swapchain",
        "\tjxK.uTai = (unsigned)m_texUploads.size(); jxK.uTaiKB = (unsigned)(m_texStage.size() >> 10); jxK.uRingKB = (unsigned)(m_ring.size() >> 10);",
        "#endif"), "SubmitFrame cho")
    s = chen_truoc(s, r'^\t// ---- render pass$', A(
        "#ifdef JX_ANDROID",
        "\t{ const Uint64 u = SDL_GetPerformanceCounter(); jxK.dChep = JxVeMs(uJxK1, u); uJxK1 = u; }\t// " + DAU + " chep len GPU",
        "#endif"), "SubmitFrame chep")
    s = chen_sau(s, r'^\t\t\tpass = SDL_BeginGPURenderPass\(cb, &ci, 1, NULL\);\n\t\t\tbPendingClear = false; bLast = false;$', A(
        "#ifdef JX_ANDROID",
        "\t\t\tjxK.uPass++;\t// " + DAU,
        "#endif"), "SubmitFrame pass")
    s = thay(s, r'^\t\tif \(!bLast \|\| st\.pPipe != last\.pPipe\) SDL_BindGPUGraphicsPipeline\(pass, st\.pPipe\);$', A(
        "\t\tif (!bLast || st.pPipe != last.pPipe)",
        "\t\t{",
        "\t\t\tSDL_BindGPUGraphicsPipeline(pass, st.pPipe);",
        "#ifdef JX_ANDROID",
        "\t\t\tjxK.uDoiPipe++;\t// " + DAU,
        "#endif",
        "\t\t}"), "SubmitFrame pipeline")
    s = chen_sau(s, r'^\t\tif \(!bLast \|\| memcmp\(&st\.vp, &last\.vp, sizeof\(st\.vp\)\) != 0\)\n\t\t\{$', A(
        "#ifdef JX_ANDROID",
        "\t\t\tjxK.uDoiCat++;\t// " + DAU + " (viewport)",
        "#endif"), "SubmitFrame viewport")
    s = chen_sau(s, r'^\t\tif \(!bLast \|\| st\.bScissor != last\.bScissor \|\| memcmp\(&st\.rcScissor, &last\.rcScissor, sizeof\(RECT\)\) != 0\)\n\t\t\{$', A(
        "#ifdef JX_ANDROID",
        "\t\t\tjxK.uDoiCat++;\t// " + DAU + " (scissor)",
        "#endif"), "SubmitFrame scissor")
    s = chen_sau(s, r'^\t\tif \(!bLast \|\| st\.pTex\[0\] != last\.pTex\[0\] \|\| st\.pTex\[1\] != last\.pTex\[1\] \|\| st\.pSamp\[0\] != last\.pSamp\[0\] \|\| st\.pSamp\[1\] != last\.pSamp\[1\]\)\n\t\t\{$', A(
        "#ifdef JX_ANDROID",
        "\t\t\tjxK.uDoiTex++;\t// " + DAU,
        "#endif"), "SubmitFrame sampler")
    s = thay(s, r'^\t\tif \(!bLast \|\| memcmp\(&st\.vs, &last\.vs, sizeof\(st\.vs\)\) != 0\) SDL_PushGPUVertexUniformData\(cb, 0, &st\.vs, sizeof\(st\.vs\)\);$', A(
        "\t\tif (!bLast || memcmp(&st.vs, &last.vs, sizeof(st.vs)) != 0)",
        "\t\t{",
        "\t\t\tSDL_PushGPUVertexUniformData(cb, 0, &st.vs, sizeof(st.vs));",
        "#ifdef JX_ANDROID",
        "\t\t\tjxK.uDoiVs++;\t// " + DAU,
        "#endif",
        "\t\t}"), "SubmitFrame vs")
    s = thay(s, r'^\t\tif \(!bLast \|\| memcmp\(&st\.ps, &last\.ps, sizeof\(st\.ps\)\) != 0\) SDL_PushGPUFragmentUniformData\(cb, 0, &st\.ps, sizeof\(st\.ps\)\);$', A(
        "\t\tif (!bLast || memcmp(&st.ps, &last.ps, sizeof(st.ps)) != 0)",
        "\t\t{",
        "\t\t\tSDL_PushGPUFragmentUniformData(cb, 0, &st.ps, sizeof(st.ps));",
        "#ifdef JX_ANDROID",
        "\t\t\tjxK.uDoiPs++;\t// " + DAU,
        "#endif",
        "\t\t}"), "SubmitFrame ps")
    s = chen_sau(s, r'^\t\tSDL_DrawGPUPrimitives\(pass, c\.nVerts, 1, 0, 0\);$', A(
        "#ifdef JX_ANDROID",
        "\t\tjxK.uLenh++; jxK.uDinh += c.nVerts;\t// " + DAU,
        "#endif"), "SubmitFrame draw")
    s = chen_truoc(s, r'^\t// ---- ban sao khung de chup man hinh$', A(
        "#ifdef JX_ANDROID",
        "\t{ const Uint64 u = SDL_GetPerformanceCounter(); jxK.dGhi = JxVeMs(uJxK1, u); uJxK1 = u; }\t// " + DAU + " ghi lenh render pass",
        "#endif"), "SubmitFrame ghi")
    s = chen_sau(s, r'^\tif \(!SDL_SubmitGPUCommandBuffer\(cb\)\) RgLog\("SubmitGPUCommandBuffer that bai: %s", SDL_GetError\(\)\);$', A(
        "#ifdef JX_ANDROID",
        "\t{ const Uint64 u = SDL_GetPerformanceCounter(); jxK.dNop = JxVeMs(uJxK1, u); jxK.dTong = JxVeMs(uJxK0, u); jxK.uQuad = m_uQuads; if (bPresent) JxVeCong(jxK); }\t// " + DAU + " nop",
        "#endif"), "SubmitFrame nop")
    ghi(p, s, nl, cao)

# ============================================================ 3. KRepresentShell3.cpp: cong tac, thong ke, [VE]/[VE-GOP]/[VE-NAP]/[VE-GIAT]
p = os.path.join(R3, "KRepresentShell3.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    s = thay(s, r'^void Rep3NapCong\(Rep3NapDo& d, double ms\) \{ d\.n\+\+; d\.ms \+= ms; if \(ms > d\.max\) d\.max = ms; g_dRep3NapKhung \+= ms; \}$', A(
        "#ifndef JX_ANDROID\t// " + DAU + " Android: ban co thong ke theo khung ben duoi",
        "void Rep3NapCong(Rep3NapDo& d, double ms) { d.n++; d.ms += ms; if (ms > d.max) d.max = ms; g_dRep3NapKhung += ms; }",
        "#endif"), "Rep3NapCong cu")
    s = chen_truoc(s, r'^void Rep3VeDem\(const char\* p\)$', A(
        "#ifdef JX_ANDROID",
        "// " + DAU + " Android: cong tac + thong ke nap KHUNG o luong nen (TextureRes.cpp / TextureResMgr.cpp) va do trinh chieu (D3D9onGPUDev.cpp)",
        "int g_nJxNapKhungNen = 1, g_nJxNapKhungMs = 3, g_nJxNapKhungTruoc = 2, g_nJxNapKhungApMs = 3, g_nJxVeGiatMs = 20;",
        "int g_nJxAnhBoVeNen = 0;",
        "unsigned g_uJxNapKhungBoVe = 0, g_uJxNapKhungBoVeKhung = 0, g_uJxNapKhungDongBo = 0, g_uJxNapKhungGiao = 0, g_uJxNapKhungTruocSo = 0, g_uJxNapKhungXong = 0, g_uJxNapKhungHong = 0, g_uJxNapKhungBo = 0, g_uJxNapKhungChoMax = 0;",
        "double g_dJxNapKhungTre = 0.0, g_dJxNapKhungTreMax = 0.0, g_dJxNapNenBan = 0.0, g_dJxNapKhungAp = 0.0, g_dJxNapKhungApMax = 0.0; unsigned g_uJxNapKhungApKhung = 0;",
        "Rep3NapDo g_jxNapNgoaiVe = {0, 0, 0};",
        "static Rep3NapDo s_jxNapKhung[5];\t\t// nap trong KHUNG hien tai theo loai: 0 tep spr, 1 jpeg, 2 rut khung, 3 giai ma, 4 tao GPU -> [VE-GIAT]",
        "static Rep3NapDo s_jxNapKhungCuoi[5]; static double s_dJxNapNgoaiKhung = 0.0, s_dJxNapNgoaiKhungCuoi = 0.0, s_dJxVeCpuCuoi = 0.0;",
        "static double s_dJxVeCpuTong = 0.0, s_dJxVeCpuMax = 0.0; static unsigned s_uJxVeCpuKhung = 0, s_uJxBoVeKhungCuoi = 0;",
        "void Rep3NapCong(Rep3NapDo& d, double ms)",
        "{",
        "\td.n++; d.ms += ms; if (ms > d.max) d.max = ms; g_dRep3NapKhung += ms;",
        "\tconst int k = (&d == &g_napSpr) ? 0 : (&d == &g_napJpeg) ? 1 : (&d == &g_napKhung) ? 2 : (&d == &g_napGiaiMa) ? 3 : 4;",
        "\ts_jxNapKhung[k].n++; s_jxNapKhung[k].ms += ms; if (ms > s_jxNapKhung[k].max) s_jxNapKhung[k].max = ms;",
        "\tif (g_pJxTexMgr && !g_pJxTexMgr->m_bVeDangDien) { g_jxNapNgoaiVe.n++; g_jxNapNgoaiVe.ms += ms; if (ms > g_jxNapNgoaiVe.max) g_jxNapNgoaiVe.max = ms; s_dJxNapNgoaiKhung += ms; }",
        "}",
        "// RepresentEnd (truoc khi dat lai g_dRep3NapKhung): chot so nap + thoi gian ve CPU cua khung nay",
        "static void JxVeKhungChot()",
        "{",
        "\tLARGE_INTEGER li; QueryPerformanceCounter(&li);",
        "\ts_dJxVeCpuCuoi = g_liRep3VeBegin.QuadPart ? Rep3NapMs(g_liRep3VeBegin, li) : 0.0;",
        "\tmemcpy(s_jxNapKhungCuoi, s_jxNapKhung, sizeof(s_jxNapKhung)); memset(s_jxNapKhung, 0, sizeof(s_jxNapKhung));",
        "\ts_dJxNapNgoaiKhungCuoi = s_dJxNapNgoaiKhung; s_dJxNapNgoaiKhung = 0.0;",
        "\ts_uJxBoVeKhungCuoi = g_uJxNapKhungBoVeKhung; g_uJxNapKhungBoVeKhung = 0;",
        "}",
        "// Ngay sau Present: ve CPU + trinh chieu (hoac nap ngoai luc ve) vuot VeGiatMs -> ghi [VE-GIAT] (toi da 12 dong / 10 s)",
        "static void JxVeGiatGhi(double dTrinhChieu)",
        "{",
        "\ts_dJxVeCpuTong += s_dJxVeCpuCuoi; if (s_dJxVeCpuCuoi > s_dJxVeCpuMax) s_dJxVeCpuMax = s_dJxVeCpuCuoi; s_uJxVeCpuKhung++;",
        "\tif (g_nJxVeGiatMs <= 0) return;",
        "\tif (s_dJxVeCpuCuoi + dTrinhChieu < (double)g_nJxVeGiatMs && s_dJxNapNgoaiKhungCuoi < (double)g_nJxVeGiatMs) return;",
        "\tstatic DWORD s_dwMoc = 0; static int s_nDem = 0;",
        "\tconst DWORD dwNow = timeGetTime();",
        "\tif (s_dwMoc == 0 || dwNow - s_dwMoc >= 10000) { s_dwMoc = dwNow; s_nDem = 0; }",
        "\tif (++s_nDem > 12) return;",
        "\tconst JxVeDo& k = g_jxVeKhung;",
        "\tdouble dNap = 0.0; for (int i = 0; i < 5; i++) dNap += s_jxNapKhungCuoi[i].ms;",
        "\tRep3Log(\"[VE-GIAT] khung %u: %.1f ms = ve CPU %.1f (nap %.1f ms: tep spr %u/%.1f, rut khung %u/%.1f, giai ma %u/%.1f, tao GPU %u/%.1f; ngoai luc ve %.1f) + trinh chieu %.1f (cho %.1f, chep %.1f [tai %u tex %u KB, ring %u KB], ghi %.1f [%u lenh, %u quad, %u pass, doi tex %u], nop %.1f) | nen: bo ve %u, cho ap %u, ap %u khung %.1f ms\",",
        "\t\tg_uJxVeKhungSo, s_dJxVeCpuCuoi + dTrinhChieu, s_dJxVeCpuCuoi, dNap, s_jxNapKhungCuoi[0].n, s_jxNapKhungCuoi[0].ms, s_jxNapKhungCuoi[2].n, s_jxNapKhungCuoi[2].ms, s_jxNapKhungCuoi[3].n, s_jxNapKhungCuoi[3].ms, s_jxNapKhungCuoi[4].n, s_jxNapKhungCuoi[4].ms, s_dJxNapNgoaiKhungCuoi,",
        "\t\tdTrinhChieu, k.dCho, k.dChep, k.uTai, k.uTaiKB, k.uRingKB, k.dGhi, k.uLenh, k.uQuad, k.uPass, k.uDoiTex, k.dNop,",
        "\t\ts_uJxBoVeKhungCuoi, g_pJxTexMgr ? g_pJxTexMgr->JxNapKhungDangCho() : 0u, g_pJxTexMgr ? g_pJxTexMgr->m_uJxApKhungCuoi : 0u, g_pJxTexMgr ? g_pJxTexMgr->m_dJxApCuoi : 0.0);",
        "}",
        "// Moi ky Rep3StatSec (khoi thong ke cua RepresentEnd): [VE] trinh chieu, [VE-GOP] doi trang thai / ly do khong gop, [VE-NAP] nap khung nen",
        "static void JxVeKyIn()",
        "{",
        "\tconst unsigned n = g_uJxVeKhungSo ? g_uJxVeKhungSo : 1;",
        "\tconst JxVeDo& t = g_jxVeTong; const JxVeDo& m = g_jxVeMax;",
        "\tRep3Log(\"[VE] %ds trinh chieu %u khung: cho lenh+swapchain TB %.2f ms (max %.1f) | chep len GPU TB %.2f (max %.1f): tai %u texture %u KB (max %u KB/khung), ring TB %u KB (max %u) | ghi lenh TB %.2f (max %.1f): TB %u lenh, %u quad, %u dinh, %u pass/khung | nop TB %.2f (max %.1f) | tong TB %.2f (max %.1f), khung >8 ms %u, >16 ms %u | ve CPU (Begin->End) TB %.2f (max %.1f)\",",
        "\t\tg_nRep3StatSec, g_uJxVeKhungSo, t.dCho / n, m.dCho, t.dChep / n, m.dChep, t.uTai, t.uTaiKB, m.uTaiKB, t.uRingKB / n, m.uRingKB, t.dGhi / n, m.dGhi, t.uLenh / n, t.uQuad / n, t.uDinh / n, t.uPass / n,",
        "\t\tt.dNop / n, m.dNop, t.dTong / n, m.dTong, g_uJxVe8, g_uJxVe16, s_uJxVeCpuKhung ? s_dJxVeCpuTong / s_uJxVeCpuKhung : 0.0, s_dJxVeCpuMax);",
        "\tRep3Log(\"[VE-GOP] doi trang thai/khung TB: pipeline %u, texture/sampler %u (max %u), uniform vs %u, ps %u, cat/viewport %u | quad khong gop (ca ky): stride %u, khong lien tiep %u, pipeline %u, texture0 %u, texture1/sampler %u, vs %u, ps %u, cat/vp %u\",",
        "\t\tt.uDoiPipe / n, t.uDoiTex / n, m.uDoiTex, t.uDoiVs / n, t.uDoiPs / n, t.uDoiCat / n, g_uJxGopVo[0], g_uJxGopVo[1], g_uJxGopVo[2], g_uJxGopVo[3], g_uJxGopVo[4], g_uJxGopVo[5], g_uJxGopVo[6], g_uJxGopVo[7]);",
        "\tmemset(&g_jxVeTong, 0, sizeof(g_jxVeTong)); memset(&g_jxVeMax, 0, sizeof(g_jxVeMax)); g_uJxVeKhungSo = 0; g_uJxVe8 = 0; g_uJxVe16 = 0; memset(g_uJxGopVo, 0, sizeof(g_uJxGopVo));",
        "\ts_dJxVeCpuTong = 0.0; s_dJxVeCpuMax = 0.0; s_uJxVeCpuKhung = 0;",
        "\tRep3Log(\"[VE-NAP] nap khung nen (bat=%d, ngan sach %d ms/khung, nap truoc %d, ap %d ms): giao %u (nap truoc %u) xong %u hong %u bo %u | bo ve %u luot, dong bo trong ngan sach %u | hang cho max %u | tre giao->ap TB %.1f ms (max %.1f) | luong nen ban %.0f ms | ap tren luong ve %u khung %.1f ms (max %.2f/khung) | nap dong bo NGOAI luc ve: %u lan %.1f ms (max %.2f)\",",
        "\t\tg_nJxNapKhungNen, g_nJxNapKhungMs, g_nJxNapKhungTruoc, g_nJxNapKhungApMs, g_uJxNapKhungGiao, g_uJxNapKhungTruocSo, g_uJxNapKhungXong, g_uJxNapKhungHong, g_uJxNapKhungBo, g_uJxNapKhungBoVe, g_uJxNapKhungDongBo, g_uJxNapKhungChoMax,",
        "\t\tg_uJxNapKhungXong ? g_dJxNapKhungTre / g_uJxNapKhungXong : 0.0, g_dJxNapKhungTreMax, g_dJxNapNenBan, g_uJxNapKhungApKhung, g_dJxNapKhungAp, g_dJxNapKhungApMax, g_jxNapNgoaiVe.n, g_jxNapNgoaiVe.ms, g_jxNapNgoaiVe.max);",
        "\tg_uJxNapKhungGiao = g_uJxNapKhungTruocSo = g_uJxNapKhungXong = g_uJxNapKhungHong = g_uJxNapKhungBo = g_uJxNapKhungBoVe = g_uJxNapKhungDongBo = g_uJxNapKhungChoMax = 0;",
        "\tg_dJxNapKhungTre = g_dJxNapKhungTreMax = g_dJxNapNenBan = g_dJxNapKhungAp = g_dJxNapKhungApMax = 0.0; g_uJxNapKhungApKhung = 0; memset(&g_jxNapNgoaiVe, 0, sizeof(g_jxNapNgoaiVe));",
        "}",
        "#endif"), "khoi JxVe")
    s = chen_sau(s, r'^static void Rep3AnhNullGhi\(const char\* szTen, int nKhung\)\n\{$', A(
        "#ifdef JX_ANDROID",
        "\tif (g_nJxAnhBoVeNen) { g_nJxAnhBoVeNen = 0; return; }\t// " + DAU + " khung dang nap o luong nen: khong phai anh thieu",
        "#endif"), "Rep3AnhNullGhi")
    s = chen_sau(s, r'^\tg_nRep3NapNen    = Rep3Ini\("Rep3NapNen", 1\);\t// \[NAP 08/09 b\]$', A(
        "#ifdef JX_ANDROID",
        "\tg_nJxNapKhungNen   = Rep3Ini(\"NapKhungNen\", 1);\t\t// " + DAU + " 1 = nap khung sprite o luong nen khi het ngan sach dong bo (0 = nhu cu)",
        "\tg_nJxNapKhungMs    = Rep3Ini(\"NapKhungMs\", 3);\t\t// ngan sach nap dong bo tren luong ve moi khung (ms); qua thi giao luong nen, bo ve khung nay",
        "\tg_nJxNapKhungTruoc = Rep3Ini(\"NapKhungTruoc\", 2);\t// so khung KE TIEP cung huong nap truoc o luong nen (0 = tat)",
        "\tg_nJxNapKhungApMs  = Rep3Ini(\"NapKhungApMs\", 3);\t// ngan sach tao texture tu ket qua luong nen moi khung (ms)",
        "\tg_nJxVeGiatMs      = Rep3Ini(\"VeGiatMs\", 20);\t\t// ghi [VE-GIAT] khi ve CPU + trinh chieu (hoac nap ngoai luc ve) cua mot khung vuot nguong (ms); 0 = tat",
        "\tRep3Log(\"[VE] nap khung nen=%d, ngan sach %d ms/khung, nap truoc %d khung, ap %d ms/khung; nguong [VE-GIAT] %d ms\", g_nJxNapKhungNen, g_nJxNapKhungMs, g_nJxNapKhungTruoc, g_nJxNapKhungApMs, g_nJxVeGiatMs);",
        "#endif"), "ini")
    s = chen_sau(s, r'^\tm_TextureResMgr\.NapNenNhan\(\);\t// \[NAP 08/09 b\] nhan ket qua luong nen truoc khi ve$', A(
        "#ifdef JX_ANDROID",
        "\tm_TextureResMgr.JxNapKhungNhan();\t// " + DAU + " tao texture tu khung da giai ma o luong nen (theo ngan sach NapKhungApMs)",
        "#endif"), "RepresentBegin")
    s = chen_truoc(s, r'^\t\tg_dRep3NapKhung = 0\.0;$', A(
        "#ifdef JX_ANDROID",
        "\t\tJxVeKhungChot();\t// " + DAU + " chot so nap + ve CPU cua khung (in [VE-GIAT] sau Present)",
        "#endif"), "RepresentEnd chot")
    s = thay(s, r'^\tPD3DDEVICE->Present\(NULL,NULL,NULL,NULL\);$', A(
        "#ifdef JX_ANDROID",
        "\t{ LARGE_INTEGER liJx0, liJx1; QueryPerformanceCounter(&liJx0); PD3DDEVICE->Present(NULL,NULL,NULL,NULL); QueryPerformanceCounter(&liJx1); JxVeGiatGhi(Rep3NapMs(liJx0, liJx1)); }\t// " + DAU,
        "#else",
        "\tPD3DDEVICE->Present(NULL,NULL,NULL,NULL);",
        "#endif"), "Present")
    s = chen_truoc(s, r'^\t\t\tRep3AnhNullIn\(\);\t// \[REP3 08/09 h\]$', A(
        "#ifdef JX_ANDROID",
        "\t\t\tJxVeKyIn();\t// " + DAU + " [VE] + [VE-GOP] + [VE-NAP]",
        "#endif"), "thong ke ky")
    ghi(p, s, nl, cao)

# ============================================================ 4. TextureResMgr.h: hang doi khung nen
p = os.path.join(R3, "TextureResMgr.h")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    s = chen_sau(s, r'^class TextureRes;$', A(
        "#ifdef JX_ANDROID",
        "class TextureResSpr;\t// " + DAU,
        "#endif"), "khai bao truoc")
    s = chen_sau(s, r'^\tstruct NapKetQua \{ char szTen\[MAX_PATH\]; uint32 uId; uint32 nType; TextureRes\* pRes; \};$', A(
        "#ifdef JX_ANDROID",
        "\t// " + DAU + " nap KHUNG sprite o luong nen: luong ve giao (TextureResSpr::PrepareFrameData khi dang ve ma het ngan sach NapKhungMs),",
        "\t// luong nen rut khung + giai ma vao bo dem (JxGiaiMaNen), luong ve tao texture o dau khung sau (JxNapKhungNhan, ngan sach NapKhungApMs).",
        "\t// Vong doi: TextureResSpr::Release goi JxNapKhungHuy -> bo viec chua chay, CHO viec dang chay xong, bo ket qua cua sprite do.",
        "\tbool JxKhungXep(const JxKhungViec& v);\t\t// luong ve: xep viec (tu bat luong nen); false = khong co luong nen -> nap dong bo",
        "\tvoid JxNapKhungNhan();\t\t\t\t\t\t// luong ve, dau khung: tao texture tu ket qua theo ngan sach",
        "\tvoid JxNapKhungHuy(TextureResSpr* p);\t\t// luong ve: sprite sap bi xoa",
        "\tunsigned JxNapKhungDangCho() const { return (unsigned)m_jxKhungCho.size(); }",
        "\tunsigned m_uJxApKhungCuoi; double m_dJxApCuoi;\t// so khung / ms da ap trong RepresentBegin vua roi (cho [VE-GIAT])",
        "private:",
        "\tbool JxNapLuongBat();\t\t\t\t\t\t// tao luong nen neu chua co (cung luong voi NapNenGiao)",
        "\tvector<JxKhungViec> m_jxKhungViec; vector<JxKhungXong> m_jxKhungXong; vector<JxKhungXong> m_jxKhungCho; TextureResSpr* volatile m_pJxKhungDangChay;",
        "public:",
        "#endif"), "TextureResMgr.h thanh vien")
    ghi(p, s, nl, cao)

# ============================================================ 5. TextureResMgr.cpp: luong nen xu ly viec khung, nhan ket qua, huy
p = os.path.join(R3, "TextureResMgr.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    s = chen_sau(s, r'^#define REP3_RELOAD_COOLDOWN_LAU\t600000.*$', A(
        "#ifdef JX_ANDROID",
        "TextureResMgr* g_pJxTexMgr = NULL;\t// " + DAU + " bo quan ly duy nhat (KRepresentShell3::m_TextureResMgr) cho TextureRes.cpp / KRepresentShell3.cpp",
        "#endif"), "g_pJxTexMgr")
    s = chen_sau(s, r'^\tm_hNapLuong = NULL; m_hNapCo = NULL; m_lNapDung = 0; m_bNapNenLoi = false;$', A(
        "#ifdef JX_ANDROID",
        "\tm_pJxKhungDangChay = NULL; m_uJxApKhungCuoi = 0; m_dJxApCuoi = 0.0; g_pJxTexMgr = this;\t// " + DAU,
        "#endif"), "ctor")
    s = thay(s, r'^TextureResMgr::~TextureResMgr\(\)\n\{\n\tFree\(\);$', A(
        "TextureResMgr::~TextureResMgr()",
        "{",
        "\tFree();",
        "#ifdef JX_ANDROID",
        "\tif (g_pJxTexMgr == this) g_pJxTexMgr = NULL;\t// " + DAU,
        "#endif"), "dtor")
    s = chen_sau(s, r'^TextureRes\* TextureResMgr::GetImage\( const char\* pszImage, unsigned int& uImage, short& nImagePosition, \n\t+int nFrame, int nType, bool bPrepareTex\)\n\{$', A(
        "#ifdef JX_ANDROID",
        "\tg_nJxAnhBoVeNen = 0;\t// " + DAU,
        "#endif"), "GetImage dau")
    s = thay_nhom(s, r'^\t\t\tNapViec v;\n\t\t\t\{\n\t\t\t\tKAutoCriticalSection k\(m_napKhoa\);\n\t\t\t\tvector<NapViec>& q = m_napViec\.empty\(\) \? m_napViecSau : m_napViec;(\t// \[NAPNPC 09/09\][^\n]*)\n\t\t\t\tif \(q\.empty\(\)\)\n\t\t\t\t\tbreak;\n\t\t\t\tv = q\.front\(\);\n\t\t\t\tq\.erase\(q\.begin\(\)\);\n\t\t\t\}\n\t\t\tNapKetQua kq;$', A(
        "\t\t\tNapViec v;",
        "#ifdef JX_ANDROID",
        "\t\t\tJxKhungViec kv; bool bKhung = false;\t// " + DAU,
        "#endif",
        "\t\t\t{",
        "\t\t\t\tKAutoCriticalSection k(m_napKhoa);",
        "#ifdef JX_ANDROID",
        "\t\t\t\tif (m_napViec.empty() && !m_jxKhungViec.empty())\t// " + DAU + " hang TRUOC (anh dang ve can) > KHUNG (dang ve can / nap truoc) > hang SAU",
        "\t\t\t\t{",
        "\t\t\t\t\tkv = m_jxKhungViec.front(); m_jxKhungViec.erase(m_jxKhungViec.begin()); m_pJxKhungDangChay = kv.pSpr; bKhung = true;",
        "\t\t\t\t}",
        "\t\t\t\telse",
        "\t\t\t\t{",
        "#endif",
        "\t\t\t\tvector<NapViec>& q = m_napViec.empty() ? m_napViecSau : m_napViec;\\1",
        "\t\t\t\tif (q.empty())",
        "\t\t\t\t\tbreak;",
        "\t\t\t\tv = q.front();",
        "\t\t\t\tq.erase(q.begin());",
        "#ifdef JX_ANDROID",
        "\t\t\t\t}",
        "#endif",
        "\t\t\t}",
        "#ifdef JX_ANDROID",
        "\t\t\tif (bKhung)",
        "\t\t\t{",
        "\t\t\t\tJxKhungXong kq; memset(&kq, 0, sizeof(kq)); kq.pSpr = kv.pSpr; kq.nFrame = kv.nFrame; kq.nBpp = kv.nBpp; kq.eFmt = kv.eFmt; kq.bPal = kv.bPal; kq.uLuc = kv.uLuc;",
        "\t\t\t\tLARGE_INTEGER liA, liB; QueryPerformanceCounter(&liA);",
        "\t\t\t\tkq.bHong = kv.pSpr->JxGiaiMaNen(kv.nFrame, kv.nBpp, (D3DFORMAT)kv.eFmt, kv.bPal != 0, kq) ? 0 : 1;",
        "\t\t\t\tQueryPerformanceCounter(&liB);",
        "\t\t\t\t{",
        "\t\t\t\t\tKAutoCriticalSection k(m_napKhoa);",
        "\t\t\t\t\tm_jxKhungXong.push_back(kq); m_pJxKhungDangChay = NULL; g_dJxNapNenBan += Rep3NapMs(liA, liB);",
        "\t\t\t\t}",
        "\t\t\t\tif (m_lNapDung)",
        "\t\t\t\t\tbreak;",
        "\t\t\t\tcontinue;",
        "\t\t\t}",
        "#endif",
        "\t\t\tNapKetQua kq;"), "NapNenChay")
    s = chen_sau(s, r'^\tm_napXong\.clear\(\); m_napViec\.clear\(\); m_napViecSau\.clear\(\);\t// \[NAPNPC 09/09\]$', A(
        "#ifdef JX_ANDROID",
        "\tfor (size_t i = 0; i < m_jxKhungXong.size(); i++) if (m_jxKhungXong[i].pDiem) free(m_jxKhungXong[i].pDiem);\t// " + DAU,
        "\tfor (size_t i = 0; i < m_jxKhungCho.size(); i++) if (m_jxKhungCho[i].pDiem) free(m_jxKhungCho[i].pDiem);",
        "\tm_jxKhungXong.clear(); m_jxKhungViec.clear(); m_jxKhungCho.clear(); m_pJxKhungDangChay = NULL;",
        "#endif"), "NapNenDung")
    if not s.endswith("\n"):
        s += "\n"
    s += A(
        "",
        "#ifdef JX_ANDROID",
        "// ============================ " + DAU + " nap KHUNG sprite o luong nen ============================",
        "bool TextureResMgr::JxNapLuongBat()\t// giong doan dau NapNenGiao (giu nguyen ham do cho Windows)",
        "{",
        "\tif (m_bNapNenLoi)",
        "\t\treturn false;",
        "\tif (m_hNapLuong)",
        "\t\treturn true;",
        "\tm_hNapCo = CreateEventA(NULL, FALSE, FALSE, NULL);",
        "\tm_lNapDung = 0;",
        "\tunsigned uTid = 0;",
        "\tm_hNapLuong = m_hNapCo ? (HANDLE)_beginthreadex(NULL, 0, NapNenLuong, this, 0, &uTid) : NULL;",
        "\tif (!m_hNapLuong)",
        "\t{",
        "\t\tm_bNapNenLoi = true;",
        "\t\tif (m_hNapCo) { CloseHandle(m_hNapCo); m_hNapCo = NULL; }",
        "\t\tRep3Log(\"[REP3] nap nen: khong tao duoc luong -> nap ngay tren luong ve\");",
        "\t\treturn false;",
        "\t}",
        "\tSetThreadPriority(m_hNapLuong, THREAD_PRIORITY_BELOW_NORMAL);",
        "\tRep3Log(\"[REP3] nap nen: luong nen da chay (Rep3NapNen=1)\");",
        "\treturn true;",
        "}",
        "",
        "bool TextureResMgr::JxKhungXep(const JxKhungViec& v)",
        "{",
        "\tif (!JxNapLuongBat())",
        "\t\treturn false;",
        "\t{",
        "\t\tKAutoCriticalSection k(m_napKhoa);",
        "\t\tm_jxKhungViec.push_back(v);",
        "\t\tif ((unsigned)m_jxKhungViec.size() > g_uJxNapKhungChoMax) g_uJxNapKhungChoMax = (unsigned)m_jxKhungViec.size();",
        "\t}",
        "\tSetEvent(m_hNapCo);",
        "\treturn true;",
        "}",
        "",
        "// Luong ve, dau khung (RepresentBegin, sau NapNenNhan): tao texture tu khung da giai ma, toi da NapKhungApMs ms; phan con lai de khung sau.",
        "void TextureResMgr::JxNapKhungNhan()",
        "{",
        "\t{",
        "\t\tKAutoCriticalSection k(m_napKhoa);",
        "\t\tif (!m_jxKhungXong.empty()) { m_jxKhungCho.insert(m_jxKhungCho.end(), m_jxKhungXong.begin(), m_jxKhungXong.end()); m_jxKhungXong.clear(); }",
        "\t}",
        "\tm_uJxApKhungCuoi = 0; m_dJxApCuoi = 0.0;",
        "\tif (m_jxKhungCho.empty())",
        "\t\treturn;",
        "\tKAutoCriticalSection AutoLock(m_ImageProcessLock);",
        "\tLARGE_INTEGER li0, li1; QueryPerformanceCounter(&li0);",
        "\tconst unsigned uNow = (unsigned)timeGetTime();",
        "\tsize_t i = 0;",
        "\tfor (; i < m_jxKhungCho.size(); i++)",
        "\t{",
        "\t\tJxKhungXong& kq = m_jxKhungCho[i];",
        "\t\tkq.pSpr->JxNhanKhungNen(kq);\t// giai phong kq.pDiem",
        "\t\tm_uJxApKhungCuoi++;",
        "\t\t{ const double dTre = (double)(uNow - kq.uLuc); g_dJxNapKhungTre += dTre; if (dTre > g_dJxNapKhungTreMax) g_dJxNapKhungTreMax = dTre; }",
        "\t\tQueryPerformanceCounter(&li1);",
        "\t\tif (Rep3NapMs(li0, li1) >= (double)g_nJxNapKhungApMs) { i++; break; }",
        "\t}",
        "\tm_jxKhungCho.erase(m_jxKhungCho.begin(), m_jxKhungCho.begin() + i);",
        "\tQueryPerformanceCounter(&li1);",
        "\tm_dJxApCuoi = Rep3NapMs(li0, li1); g_dJxNapKhungAp += m_dJxApCuoi; if (m_dJxApCuoi > g_dJxNapKhungApMax) g_dJxNapKhungApMax = m_dJxApCuoi; g_uJxNapKhungApKhung += m_uJxApKhungCuoi;",
        "}",
        "",
        "// Luong ve (TextureResSpr::Release): sprite sap bi xoa -> bo viec chua chay, cho viec dang chay xong (vai ms), bo ket qua cua no.",
        "// Luong nen KHONG bao gio giu m_ImageProcessLock nen cho o day khong ket.",
        "void TextureResMgr::JxNapKhungHuy(TextureResSpr* p)",
        "{",
        "\tif (!p)",
        "\t\treturn;",
        "\t{",
        "\t\tKAutoCriticalSection k(m_napKhoa);",
        "\t\tfor (size_t i = m_jxKhungViec.size(); i > 0; i--)",
        "\t\t\tif (m_jxKhungViec[i - 1].pSpr == p) m_jxKhungViec.erase(m_jxKhungViec.begin() + (i - 1));",
        "\t}",
        "\twhile (m_pJxKhungDangChay == p)",
        "\t\tSleep(1);",
        "\t{",
        "\t\tKAutoCriticalSection k(m_napKhoa);",
        "\t\tfor (size_t i = m_jxKhungXong.size(); i > 0; i--)",
        "\t\t\tif (m_jxKhungXong[i - 1].pSpr == p) { if (m_jxKhungXong[i - 1].pDiem) free(m_jxKhungXong[i - 1].pDiem); m_jxKhungXong.erase(m_jxKhungXong.begin() + (i - 1)); }",
        "\t}",
        "\tfor (size_t i = m_jxKhungCho.size(); i > 0; i--)",
        "\t\tif (m_jxKhungCho[i - 1].pSpr == p) { if (m_jxKhungCho[i - 1].pDiem) free(m_jxKhungCho[i - 1].pDiem); m_jxKhungCho.erase(m_jxKhungCho.begin() + (i - 1)); }",
        "}",
        "#endif",
        "")
    ghi(p, s, nl, cao)

# ============================================================ 6. TextureRes.h: co khung dang nap + ham
p = os.path.join(R3, "TextureRes.h")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    s = chen_sau(s, r'^\tvoid\t\*pFrame;[^\n]*$', A(
        "#ifdef JX_ANDROID",
        "\tint32\tnJxNen;\t\t\t\t\t\t// " + DAU + " 1 = khung dang nap o luong nen (da giao, chua tao texture)",
        "#endif"), "FrameToTexture")
    s = chen_sau(s, r'^\tint32 GetPixelAlpha\(int32 nFrame, int32 x, int32 y\);$', A(
        "#ifdef JX_ANDROID",
        "\t// " + DAU + " nap khung o luong nen: giao + nap truoc (luong ve), giai ma (luong nen: chi doc m_pHeader/m_pOffset/m_pPal24/pRawData), nhan (luong ve: tao texture)",
        "\tbool JxNapKhungGiao(int32 nFrame, int nNguon);",
        "\tvoid JxNapKhungTruoc(int32 nFrame);",
        "\tbool JxGiaiMaNen(int32 nFrame, int nBpp, D3DFORMAT eFmt, bool bPal, JxKhungXong& kq);",
        "\tvoid JxNhanKhungNen(JxKhungXong& kq);",
        "\tbool\t\tm_bJxCoNen;\t\t\t\t\t// da tung giao viec luong nen (Release moi can huy)",
        "#endif"), "TextureResSpr ham")
    ghi(p, s, nl, cao)

# ============================================================ 7. TextureRes.cpp: PrepareFrameData giao luong nen + cac ham
p = os.path.join(R3, "TextureRes.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    s = chen_sau(s, r'^#include "TextureRes\.h"$', A(
        "#ifdef JX_ANDROID",
        "#include \"TextureResMgr.h\"\t// " + DAU + " g_pJxTexMgr",
        "#endif"), "include")
    s = chen_sau(s, r'^\tm_bLastFrameUsed = false;\n\tm_bNew = false;$', A(
        "#ifdef JX_ANDROID",
        "\tm_bJxCoNen = false;\t// " + DAU,
        "#endif"), "ResetVar")
    s = thay(s, r'^void TextureResSpr::Release\(\)\n\{$', A(
        "void TextureResSpr::Release()",
        "{",
        "#ifdef JX_ANDROID",
        "\tif (m_bJxCoNen && g_pJxTexMgr) g_pJxTexMgr->JxNapKhungHuy(this);\t// " + DAU + " bo viec luong nen cua sprite nay truoc khi tra khung / texture",
        "#endif"), "Release")
    s = chen_sau(s, r'^\tif\(m_pFrameInfo\[nFrame\]\.texInfo\[0\]\.pTexture\)\n\t\treturn true;$', A(
        "#ifdef JX_ANDROID",
        "\t// " + DAU + " dang ve ma tong nap dong bo cua khung da qua NapKhungMs: khung dang cho luong nen (hoac giao duoc ngay) -> bo ve khung nay",
        "\t// mot khung; con ngan sach -> nap dong bo nhu cu (ke ca khi luong nen dang lam khung nay: ket qua ve sau bi bo, dem vao 'bo').",
        "\t// Ca hai truong hop nap truoc NapKhungTruoc khung ke tiep cung huong o luong nen.",
        "\tif (bPrepareTex && g_nJxNapKhungNen > 0 && g_pJxTexMgr && g_pJxTexMgr->m_bVeDangDien)",
        "\t{",
        "\t\tif (g_dRep3NapKhung >= (double)g_nJxNapKhungMs && (m_pFrameInfo[nFrame].nJxNen || JxNapKhungGiao(nFrame, 0)))",
        "\t\t{",
        "\t\t\tJxNapKhungTruoc(nFrame);",
        "\t\t\tg_uJxNapKhungBoVe++; g_uJxNapKhungBoVeKhung++; g_nJxAnhBoVeNen = 1; return false;",
        "\t\t}",
        "\t\tJxNapKhungTruoc(nFrame);",
        "\t\tg_uJxNapKhungDongBo++;",
        "\t}",
        "#endif"), "PrepareFrameData")
    if not s.endswith("\n"):
        s += "\n"
    s += A(
        "",
        "#ifdef JX_ANDROID",
        "// ============================ " + DAU + " nap KHUNG o luong nen ============================",
        "// Luong ve: giao khung nFrame cho luong nen (nNguon 0 = dang ve can, 1 = nap truoc). Dinh dang texture chon NHU CreateTexture16Bit",
        "// (bang mau A8L8 2 B neu co, khong thi 8888 / 4444); hang bang mau cap ngay o day (can device) de luong nen chi giai ma.",
        "bool TextureResSpr::JxNapKhungGiao(int32 nFrame, int nNguon)",
        "{",
        "\tif (nFrame < 0 || nFrame >= m_nFrameNum || !m_pFrameInfo || !g_pJxTexMgr)",
        "\t\treturn false;",
        "\tFrameToTexture& f = m_pFrameInfo[nFrame];",
        "\tif (f.nJxNen || f.texInfo[0].pTexture)",
        "\t\treturn false;",
        "\tif (!m_pHeader && !f.pRawData)",
        "\t\treturn false;\t// khong co nguon du lieu",
        "\tJxKhungViec v; memset(&v, 0, sizeof(v));",
        "\tv.pSpr = this; v.nFrame = nFrame; v.nNguon = nNguon; v.uLuc = (unsigned)timeGetTime();",
        "\tv.nBpp = g_nRep3Tex32 ? 4 : 2; v.eFmt = (int)(g_nRep3Tex32 ? D3DFMT_A8R8G8B8 : D3DFMT_A4R4G4B4); v.bPal = 0;",
        "#ifdef JX_PLATFORM_SDL",
        "\tif (g_nRep3Pal && (g_nRep3ApiOn == 11 || g_nRep3ApiOn == 100) && g_nRep3Pool && m_pPal24 && Rep3_D3D11PaletteOK())",
        "#else",
        "\tif (g_nRep3Pal && g_nRep3ApiOn == 11 && g_nRep3Pool && m_pPal24 && Rep3_D3D11PaletteOK())",
        "#endif",
        "\t{",
        "\t\tif (m_nPalRow < 0)",
        "\t\t\tm_nPalRow = Rep3_D3D11AllocPalette((const unsigned char*)m_pPal24, (int)m_nColors);",
        "\t\tif (m_nPalRow >= 0) { v.bPal = 1; v.nBpp = 2; v.eFmt = (int)D3DFMT_A8L8; }",
        "\t}",
        "\tif (!g_pJxTexMgr->JxKhungXep(v))",
        "\t\treturn false;",
        "\tf.nJxNen = 1; m_bJxCoNen = true;",
        "\tg_uJxNapKhungGiao++; if (nNguon) g_uJxNapKhungTruocSo++;",
        "\treturn true;",
        "}",
        "",
        "// Luong ve: nap truoc NapKhungTruoc khung ke tiep CUNG HUONG (khung = huong * so khung moi huong + chi so; xoay vong trong huong)",
        "void TextureResSpr::JxNapKhungTruoc(int32 nFrame)",
        "{",
        "\tif (g_nJxNapKhungTruoc <= 0 || m_nFrameNum <= 1 || nFrame < 0)",
        "\t\treturn;",
        "\tconst int nHuong = (m_nDirections > 0) ? (int)m_nDirections : 1;",
        "\tconst int nMoiHuong = m_nFrameNum / nHuong;",
        "\tif (nMoiHuong <= 1)",
        "\t\treturn;",
        "\tconst int nH = nFrame / nMoiHuong, nI = nFrame % nMoiHuong;",
        "\tfor (int k = 1; k <= g_nJxNapKhungTruoc && k < nMoiHuong; k++)",
        "\t\tJxNapKhungGiao(nH * nMoiHuong + (nI + k) % nMoiHuong, 1);",
        "}",
        "",
        "// LUONG NEN: rut khung tu pak (SprGetFrame co khoa rieng) + giai ma RLE vao kq.pDiem (malloc, nW*nH*nBpp). Chi doc du lieu bat bien",
        "// cua sprite (m_pHeader, m_pOffset, bang mau; pRawData chi khi spr khong nen theo khung - bat bien tu luc nap). false = hong.",
        "bool TextureResSpr::JxGiaiMaNen(int32 nFrame, int nBpp, D3DFORMAT eFmt, bool bPal, JxKhungXong& kq)",
        "{",
        "\tif (nFrame < 0 || nFrame >= m_nFrameNum || !m_pFrameInfo)",
        "\t\treturn false;",
        "\tconst FrameToTexture& f = m_pFrameInfo[nFrame];",
        "\tBYTE* pRaw = NULL; int nRawLen = 0; SPRFRAME* pFrame = NULL;",
        "\tif (m_pHeader)",
        "\t{",
        "\t\tpFrame = (SPRFRAME*)SprGetFrame((SPRHEAD*)m_pHeader, nFrame);",
        "\t\tif (!pFrame)",
        "\t\t\treturn false;",
        "\t\tint nL = (int)m_pOffset[nFrame].Length; if (nL < 0) nL = -nL;\t// nhu PrepareFrameData ([REP3 03/09 SAP]: dau am = khung luu tho)",
        "\t\tpRaw = pFrame->Sprite; nRawLen = nL - 8;",
        "\t\tkq.nW = pFrame->Width; kq.nH = pFrame->Height; kq.nOffX = pFrame->OffsetX; kq.nOffY = pFrame->OffsetY;",
        "\t}",
        "\telse",
        "\t{",
        "\t\tpRaw = f.pRawData; nRawLen = f.nRawDataLen;",
        "\t\tkq.nW = f.nWidth; kq.nH = f.nHeight; kq.nOffX = f.nOffX; kq.nOffY = f.nOffY;",
        "\t}",
        "\tbool bOk = false;",
        "\tif (pRaw && nRawLen > 0 && kq.nW > 0 && kq.nH > 0 && (nBpp == 2 || nBpp == 4))",
        "\t{",
        "\t\tkq.pDiem = (BYTE*)malloc((size_t)kq.nW * kq.nH * nBpp);",
        "\t\tif (kq.pDiem)",
        "\t\t{",
        "\t\t\tif (bPal) RenderToIndexAlpha((WORD*)kq.pDiem, pRaw, nRawLen, kq.nW * kq.nH, (int)m_nColors);",
        "\t\t\telse if (nBpp == 4) RenderToA8R8G8B8((DWORD*)kq.pDiem, pRaw, nRawLen, kq.nW * kq.nH, m_pPal24, (int)m_nColors);",
        "\t\t\telse RenderToA4R4G4B4Safe((WORD*)kq.pDiem, pRaw, nRawLen, kq.nW * kq.nH, m_pPal16, (int)m_nColors);",
        "\t\t\tbOk = true;",
        "\t\t}",
        "\t}",
        "\tif (pFrame)",
        "\t\tSprReleaseFrame(pFrame);",
        "\t(void)eFmt;",
        "\treturn bOk;",
        "}",
        "",
        "// Luong ve: tao texture tu khung da giai ma (phan sau cua CreateTexture16Bit). Lop SDL_GPU khoa duoc texture POOL_DEFAULT nen ghi thang,",
        "// khong qua texture tam SYSTEMMEM + UpdateTexture (bot mot cap phat + mot lan chep). Luon giai phong kq.pDiem.",
        "void TextureResSpr::JxNhanKhungNen(JxKhungXong& kq)",
        "{",
        "\tBYTE* pDiem = kq.pDiem; kq.pDiem = NULL;",
        "\tconst int nFrame = kq.nFrame;",
        "\tif (nFrame < 0 || nFrame >= m_nFrameNum || !m_pFrameInfo) { if (pDiem) free(pDiem); g_uJxNapKhungBo++; return; }",
        "\tFrameToTexture& f = m_pFrameInfo[nFrame];",
        "\tf.nJxNen = 0;",
        "\tif (kq.bHong || !pDiem) { if (pDiem) free(pDiem); g_uJxNapKhungHong++; return; }",
        "\tif (f.texInfo[0].pTexture) { free(pDiem); g_uJxNapKhungBo++; return; }\t// da nap dong bo trong luc cho (hoi kich thuoc/alpha)",
        "\tf.nWidth = kq.nW; f.nHeight = kq.nH; f.nOffX = kq.nOffX; f.nOffY = kq.nOffY; f.nTexNum = 0;",
        "\tif (f.nWidth <= 0 || f.nHeight <= 0) { free(pDiem); g_uJxNapKhungHong++; return; }",
        "\tSplitTexture(nFrame);",
        "\tconst int nW = f.nWidth, nBpp = kq.nBpp; const D3DFORMAT eFmt = (D3DFORMAT)kq.eFmt; const bool bPal = kq.bPal != 0;",
        "\tif (bPal && m_nPalRow < 0)",
        "\t\tm_nPalRow = Rep3_D3D11AllocPalette((const unsigned char*)m_pPal24, (int)m_nColors);\t// da cap luc giao; phong khi bi tra giua chung",
        "\tint i;",
        "\tfor (i = 0; i < f.nTexNum; i++)",
        "\t{",
        "\t\tTextureInfo& ti = f.texInfo[i];",
        "\t\tSAFE_RELEASE(ti.pTexture);",
        "\t\tLPDIRECT3DTEXTURE9 pTex = NULL;",
        "\t\tif (FAILED(PD3DDEVICE->CreateTexture(ti.nWidth, ti.nHeight, 1, 0, eFmt, g_nRep3Pool ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &pTex, NULL)))",
        "\t\t\tbreak;",
        "\t\tD3DLOCKED_RECT lr;",
        "\t\tif (FAILED(pTex->LockRect(0, &lr, NULL, 0))) { pTex->Release(); break; }",
        "\t\tBYTE* pDst = (BYTE*)lr.pBits; const BYTE* pSrc = pDiem + ((size_t)ti.nFrameY * nW + ti.nFrameX) * nBpp;",
        "\t\tfor (int j = 0; j < ti.nFrameHeight; j++) { memcpy(pDst, pSrc, (size_t)ti.nFrameWidth * nBpp); pDst += lr.Pitch; pSrc += (size_t)nW * nBpp; }",
        "\t\tpTex->UnlockRect(0);",
        "\t\tti.pTexture = pTex;",
        "\t\tif (bPal) Rep3_D3D11TagPalette(pTex, m_nPalRow);",
        "\t\tm_nTexMemUsed += ti.nWidth * ti.nHeight * nBpp;",
        "\t}",
        "\tfree(pDiem);",
        "\tif (i < f.nTexNum)",
        "\t{\t// hong giua chung: tra cac texture da tao, khung se duoc nap lai dong bo lan ve sau",
        "\t\tfor (int k = 0; k < i; k++) { m_nTexMemUsed -= f.texInfo[k].nWidth * f.texInfo[k].nHeight * nBpp; SAFE_RELEASE(f.texInfo[k].pTexture); }",
        "\t\tg_uRep3FxTaoHong++; g_uJxNapKhungHong++;",
        "\t\treturn;",
        "\t}",
        "\tif (m_pHeader && f.pFrame) { SprReleaseFrame((SPRFRAME*)f.pFrame); f.pFrame = NULL; f.pRawData = NULL; }\t// raw cua lan hoi kich thuoc truoc: khong can nua (nhu CreateTexture16Bit)",
        "\tg_uJxNapKhungXong++;",
        "}",
        "#endif",
        "")
    ghi(p, s, nl, cao)

# ============================================================ 8. config.ini lop ghi de
p = os.path.join(GOC, "android", "du_lieu_ghi_de", "config.ini")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    s = chen_sau(s, r'^PaintSmooth=2$', A(
        "; " + DAU + " nap KHUNG sprite o luong nen (Represent3, chi Android): NapKhungNen 1 bat / 0 nhu cu; NapKhungMs = ngan sach giai ma DONG BO tren luong ve",
        ";   moi khung (ms), qua thi giao luong nen va bo ve khung do mot khung; NapKhungTruoc = so khung ke tiep cung huong nap truoc; NapKhungApMs = ngan sach",
        ";   tao texture tu ket qua luong nen moi khung (ms); VeGiatMs = nguong ghi [VE-GIAT] (ve CPU + trinh chieu) vao jx_rep3.log, 0 = tat. Thong ke [VE]/[VE-GOP]/[VE-NAP] moi 30 s.",
        "NapKhungNen=1",
        "NapKhungMs=3",
        "NapKhungTruoc=2",
        "NapKhungApMs=3",
        "VeGiatMs=20"), "config.ini")
    ghi(p, s, nl, cao)

print("xong")
