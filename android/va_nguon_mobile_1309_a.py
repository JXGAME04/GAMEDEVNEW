# -*- coding: utf-8 -*-
r"""[MOBILE 13/09 a] Mot lan cho CA HAI nen: chuyen toan bo toi uu lop ve dang rao "JX_ANDROID" sang "JX_MOBILE"
(iOS cung dinh nghia JX_MOBILE) + sua ba cho rieng iOS + state block chu re (P2).

Chu chot 13/09: "fix lai cho iOS va Android toi uu mot lan, khong ton time viet rieng tung cai" va
"khong anh huong den trai nghiem game cua nguoi choi". Moi thu trong kich ban nay la PIXEL Y HET
(chi doi cach ghi lenh / dong goi texture / dem trang thai), khong bot mot sprite nao.

Doc/ghi latin-1 (giu tung byte TCVN3/GBK, giu CRLF), moi cho vao deu bat phai khop DUNG MOT lan,
chay lai vo hai (da va thi bo qua). Kiem sau khi chay:
    python ios/kiem_android_tuongduong.py          -> Android bien dich y het (tru P2, co y)
    python ios/kiem_android_tuongduong.py --pc     -> Windows bien dich y het
    python "<skill>/scripts/check_encoding.py" <tep>  -> so byte cao khong doi

Viec lam (theo PHANTICH_SAU_IOS_ANDROID_1309.md muc 1.3, 3.2, 3.10):
  1. 9 tep Represent3: moi dong "#ifdef JX_ANDROID" / "#ifndef JX_ANDROID" -> JX_MOBILE.
     Android dinh nghia CA HAI macro nen bien dich ra y het (kiem bang may). iOS tu day co: BKG, cull CPU,
     bind ring, atlas ke 2048 + chu MANAGED vao atlas + KHOI, nap KHUNG o luong nen, vung 0 co dinh, D1 (khi co va SDL),
     tat chep swapchain moi khung, [VE]/[VE-GOP]/[VE-GIAT]/[VE-BKG], doc DUOC cac khoa Rep3*/NapKhung* tu config.ini.
  2. KRepresentShell3.cpp: bo khoi JX_APPLE khai rieng (trung voi khoi JX_MOBILE); dong ep tat trong khoi doc ini
     chi con g_nJxAtlasMang = 0 (Metal chua co bien the MSL texture mang; mac dinh von tat).
  3. D3D9onGPUDev.cpp CreateShaders (nhanh JX_APPLE): chon bien the MSL KHOI khi Rep3ShadersGPU_msl.h co JX_MSL_CO_KHOI
     (ios/sinh_shader_msl.py sinh), chua co thi tu tat KHOI va ghi log - khong bao gio dung shader sai.
  4. P2: CSBGpu (state block) chup/ap bang memcpy 3 mang thay vi 616 muc x 2 lan moi nhan chu (KFont3::OutputText).
     Ket qua trang thai y het (cung tap gia tri), chi re hon. Rao JX_MOBILE.
  5. Rep3ShadersGPU_spv.h + ReverseTools/mobile_x64/dich_shader_gpu.py: bien the shader rao JX_MOBILE (ma sinh lai van dung).
  6. ios/sinh_shader_msl.py: sinh them bien the KHOI (14 sampler, 2 dem luu tru) + #define JX_MSL_CO_KHOI.

Dung:  python android/va_nguon_mobile_1309_a.py [goc worktree]
"""
import io
import os
import re
import sys

GOC = os.path.abspath(sys.argv[1]) if len(sys.argv) > 1 else os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
R3 = os.path.join(GOC, "Sources", "Represent", "Represent3")
DAU = "[MOBILE 13/09]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s, cao_cu):
    cao = sum(1 for c in s if ord(c) >= 0x80)
    if cao != cao_cu:
        raise SystemExit("LOI: so byte cao doi %d -> %d, KHONG ghi: %s" % (cao_cu, cao, p))
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def nl_cua(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def thay1(s, cu, moi, ten, so=1):
    """thay dung `so` lan (mac dinh 1); cu/moi la chuoi da ghep NL"""
    n = s.count(cu)
    if n != so:
        raise SystemExit("LOI %s: tim thay %d cho (can %d)" % (ten, n, so))
    return s.replace(cu, moi)


def doi_rao(s, ten):
    """#ifdef/#ifndef JX_ANDROID -> JX_MOBILE tren moi dong chi thi; tra (chuoi, so dong doi)"""
    dong = s.split("\n")
    n = 0
    for i, d in enumerate(dong):
        t = d.lstrip()
        m = re.match(r"(#\s*if(?:n?def)\s+)JX_ANDROID\b", t)
        if m:
            dong[i] = d.replace("JX_ANDROID", "JX_MOBILE", 1)
            n += 1
    return "\n".join(dong), n


# so dong chi thi #ifdef/#ifndef JX_ANDROID mong doi trong tung tep (dem tren cay 72818dc9)
TEP_RAO = {
    "D3D9onGPUDev.cpp": 57,     # 58 chu JX_ANDROID, 1 nam trong chu thich dong '#ifdef JX_ANDROID // ... atlas khoi CHUA port' -> dong do van la chi thi -> 57 + 1? (dem lai khi chay)
    "D3D9onGPURes.cpp": 17,
    "D3D9onGPUi.h": 3,
    "KRepresentShell3.cpp": 11,  # 12 chu: 1 nam trong chu thich "vi khoi JX_ANDROID o tren"
    "TextureRes.cpp": 5,
    "TextureRes.h": 2,
    "TextureResMgr.cpp": 11,
    "TextureResMgr.h": 2,
    "Rep3ShadersGPU_spv.h": 4,
}

tong = 0
for ten, mong in TEP_RAO.items():
    p = os.path.join(R3, ten)
    s = doc(p)
    cao = sum(1 for c in s if ord(c) >= 0x80)
    s2, n = doi_rao(s, ten)
    if n == 0:
        print("  %-24s khong con dong #ifdef JX_ANDROID (da doi roi), bo qua" % ten)
        continue
    ghi(p, s2, cao)
    print("  %-24s doi %d dong rao" % (ten, n))
    tong += n
print("tong %d dong rao da doi" % tong)

# ---------------------------------------------------------------- 2. KRepresentShell3.cpp: khoi JX_APPLE rieng + dong ep tat
p = os.path.join(R3, "KRepresentShell3.cpp")
s = doc(p); cao = sum(1 for c in s if ord(c) >= 0x80); NL = nl_cua(s)
if "int g_nJxPsBuffer = 0, g_nJxBindRing = 0;	// [GOP] chua port sang Metal" in s:
    cu = NL.join([
        "#ifdef JX_APPLE\t// [IOS-GOP 12/09 b] Metal chi dung MOT phan cua bo cong tac tren: khai rieng dung may cai can,",
        "\t\t\t\t\t// vi khoi JX_ANDROID o tren con chua nhieu thu chi Android moi co (bo nap khung o luong nen...).",
        "int g_nJxPalBuffer = 1;\t\t\t\t\t// [PALBUF] bang mau trong storage buffer - DA PORT",
        "int g_nJxPsBuffer = 0, g_nJxBindRing = 0;\t// [GOP] chua port sang Metal",
        "int g_nJxAtlasMang = 0, g_nJxAtlasLop = 8, g_nJxAtlasCumMB = 64;\t// [MANG] chua port",
        "int g_nJxAtlasKhoi = 0, g_nJxAtlasKhoiLop = 8;\t// [KHOI] chua port",
        "JxVeDo g_jxVeKhung, g_jxVeTong, g_jxVeMax;\t// bo do ve (D3D9onGPUDev.cpp ghi)",
        "unsigned g_uJxPsBangMax = 0, g_uJxPsTran = 0;\t// [GOP] do bang trang thai tang texture",
        "#endif",
    ])
    s = thay1(s, cu + NL, "", "khoi JX_APPLE globals")   # bo han (iOS dung chung khoi JX_MOBILE o tren)
    cu = NL.join([
        "#ifdef JX_APPLE\t// [IOS-GOP 12/09 b] Metal moi port bang mau; cac toi uu con lai EP TAT cho toi khi do xong tung cai",
        "\tg_nJxBindRing = 0; g_nJxAtlasMang = 0; g_nJxAtlasKhoi = 0;\t// [IOS-GOP 12/09 d] ps buffer DA port; bind ring / texture mang / atlas khoi thi chua",
    ])
    moi = NL.join([
        "#ifdef JX_APPLE\t// %s Metal: bind ring dung duoc (drawPrimitives:vertexStart:), KHOI tu tat trong CreateShaders khi thieu JX_MSL_CO_KHOI;" % DAU,
        "\tg_nJxAtlasMang = 0;\t// texture mang chua co bien the MSL (Android cung mac dinh tat sau khi do Fold 7 16:22)",
    ])
    s = thay1(s, cu, moi, "dong ep tat Apple trong khoi doc ini")
    ghi(p, s, cao)
    print("  KRepresentShell3.cpp: bo khoi JX_APPLE rieng, sua dong ep tat")
else:
    print("  KRepresentShell3.cpp: khoi JX_APPLE da sua roi, bo qua")

# ---------------------------------------------------------------- 3. D3D9onGPUDev.cpp: CreateShaders nhanh Apple chon KHOI
p = os.path.join(R3, "D3D9onGPUDev.cpp")
s = doc(p); cao = sum(1 for c in s if ord(c) >= 0x80); NL = nl_cua(s)
if DAU + " KHOI Metal" not in s:
    cu = NL.join([
        "\t\t\tsi.code = (const Uint8*)g_Rep3GpuFSPalPsMsl; si.code_size = sizeof(g_Rep3GpuFSPalPsMsl) - 1;",
        "\t\t\tsi.num_storage_buffers = 2; si.num_uniform_buffers = 0;",
        "\t\t}",
        "\t}",
        "#endif",
    ])
    moi = NL.join([
        "\t\t\tsi.code = (const Uint8*)g_Rep3GpuFSPalPsMsl; si.code_size = sizeof(g_Rep3GpuFSPalPsMsl) - 1;",
        "\t\t\tsi.num_storage_buffers = 2; si.num_uniform_buffers = 0;",
        "\t\t\tif (g_nJxAtlasKhoi)",
        "\t\t\t{\t// %s KHOI Metal: 12 texture mang gan chet khe 2..13 + 2 dem luu tru, y het bien the SPIR-V; can bien the MSL do ios/sinh_shader_msl.py sinh (g_nJxMslCoKhoi)" % DAU,
        "\t\t\t\tif (g_nJxMslCoKhoi) { si.code = (const Uint8*)g_Rep3GpuFSPalPsKhoiMsl; si.code_size = sizeof(g_Rep3GpuFSPalPsKhoiMsl) - 1; si.num_samplers = 2 + 12; }",
        "\t\t\t\telse { RgLog(\"[KHOI] Rep3AtlasKhoi=1 nhung Rep3ShadersGPU_msl.h chua co bien the khoi (chay ios/sinh_shader_msl.py tren Mac) -> TU TAT\"); g_nJxAtlasKhoi = 0; }",
        "\t\t\t}",
        "\t\t}",
        "\t}",
        "#endif",
    ])
    s = thay1(s, cu, moi, "CreateShaders Apple KHOI")
    ghi(p, s, cao)
    print("  D3D9onGPUDev.cpp: CreateShaders nhanh Apple chon bien the KHOI khi g_nJxMslCoKhoi (kiem luc chay)")
else:
    print("  D3D9onGPUDev.cpp: CreateShaders da sua roi, bo qua")

# ---------------------------------------------------------------- 4. P2: state block memcpy (JX_MOBILE)
p = os.path.join(R3, "D3D9onGPUi.h")
s = doc(p); cao = sum(1 for c in s if ord(c) >= 0x80); NL = nl_cua(s)
if DAU + " P2" not in s:
    cu = NL.join([
        "\tLONG    m_ref;",
        "\tCDevGpu* m_pDev;",
        "\tstd::vector<std::pair<DWORD, DWORD> > m_entries;",
        "};",
    ])
    moi = NL.join([
        "\tLONG    m_ref;",
        "\tCDevGpu* m_pDev;",
        "\tstd::vector<std::pair<DWORD, DWORD> > m_entries;",
        "#ifdef JX_MOBILE",
        "\t// %s P2: khoi TRON (CreateStateBlock) chup/ap bang memcpy ba mang trang thai thay vi 616 muc x 2 lan moi nhan chu" % DAU,
        "\t// (KFont3::OutputText goi Capture + Apply + Apply cho MOI chuoi; Tong Kim 150-450 chuoi/khung). Cung tap gia tri -> hinh y het.",
        "\tbool  m_bJxDay;",
        "\tDWORD m_jxRs[256];",
        "\tDWORD m_jxTss[8][33];",
        "\tDWORD m_jxSs[8][14];",
        "#endif",
        "};",
    ])
    s = thay1(s, cu, moi, "CSBGpu members")
    ghi(p, s, cao)
    print("  D3D9onGPUi.h: CSBGpu them mang chup tron (P2)")
else:
    print("  D3D9onGPUi.h: P2 da co, bo qua")

p = os.path.join(R3, "D3D9onGPURes.cpp")
s = doc(p); cao = sum(1 for c in s if ord(c) >= 0x80); NL = nl_cua(s)
if DAU + " P2" not in s:
    cu = "CSBGpu::CSBGpu(CDevGpu* pDev) { m_ref = 1; m_pDev = pDev; pDev->AddRef(); }"
    moi = NL.join([
        "#ifdef JX_MOBILE",
        "CSBGpu::CSBGpu(CDevGpu* pDev) { m_ref = 1; m_pDev = pDev; pDev->AddRef(); m_bJxDay = false; }\t// %s P2" % DAU,
        "#else",
        "CSBGpu::CSBGpu(CDevGpu* pDev) { m_ref = 1; m_pDev = pDev; pDev->AddRef(); }",
        "#endif",
    ])
    s = thay1(s, cu, moi, "CSBGpu ctor")
    cu = NL.join([
        "HRESULT CSBGpu::Capture()",
        "{",
        "\tfor (size_t i = 0; i < m_entries.size(); i++) m_entries[i].second = m_pDev->GetStateInternal(m_entries[i].first);",
        "\treturn D3D_OK;",
        "}",
        "HRESULT CSBGpu::Apply()",
        "{",
        "\tfor (size_t i = 0; i < m_entries.size(); i++) m_pDev->SetStateInternal(m_entries[i].first, m_entries[i].second);",
        "\treturn D3D_OK;",
        "}",
    ])
    moi = NL.join([
        "HRESULT CSBGpu::Capture()",
        "{",
        "#ifdef JX_MOBILE",
        "\tif (m_bJxDay)",
        "\t{\t// %s P2: khoi tron = chup ca ba mang (256 RS + 8x33 TSS + 8x14 SS) - cung noi dung nhu 616 muc Record, re hon nhieu" % DAU,
        "\t\tmemcpy(m_jxRs, m_pDev->m_rs, sizeof(m_jxRs)); memcpy(m_jxTss, m_pDev->m_tss, sizeof(m_jxTss)); memcpy(m_jxSs, m_pDev->m_ss, sizeof(m_jxSs));",
        "\t\treturn D3D_OK;",
        "\t}",
        "#endif",
        "\tfor (size_t i = 0; i < m_entries.size(); i++) m_entries[i].second = m_pDev->GetStateInternal(m_entries[i].first);",
        "\treturn D3D_OK;",
        "}",
        "HRESULT CSBGpu::Apply()",
        "{",
        "#ifdef JX_MOBILE",
        "\tif (m_bJxDay)",
        "\t{\t// %s P2 (o [s][0] cua TSS/SS khong ai ghi, hai ben deu 0 -> chep ca cung vo hai)" % DAU,
        "\t\tmemcpy(m_pDev->m_rs, m_jxRs, sizeof(m_jxRs)); memcpy(m_pDev->m_tss, m_jxTss, sizeof(m_jxTss)); memcpy(m_pDev->m_ss, m_jxSs, sizeof(m_jxSs));",
        "\t\treturn D3D_OK;",
        "\t}",
        "#endif",
        "\tfor (size_t i = 0; i < m_entries.size(); i++) m_pDev->SetStateInternal(m_entries[i].first, m_entries[i].second);",
        "\treturn D3D_OK;",
        "}",
    ])
    s = thay1(s, cu, moi, "CSBGpu Capture/Apply")
    ghi(p, s, cao)
    print("  D3D9onGPURes.cpp: CSBGpu Capture/Apply memcpy (P2)")
else:
    print("  D3D9onGPURes.cpp: P2 da co, bo qua")

p = os.path.join(R3, "D3D9onGPUDev.cpp")
s = doc(p); cao = sum(1 for c in s if ord(c) >= 0x80); NL = nl_cua(s)
if DAU + " P2" not in s:
    cu = NL.join([
        "\tCSBGpu* p = new CSBGpu(this);",
        "\tfor (DWORD i = 0; i < 256; i++) p->Record(RGSB_RS | i, m_rs[i]);",
        "\tfor (DWORD s = 0; s < 8; s++) { for (DWORD t = 1; t < 33; t++) p->Record(RGSB_TSS | (s << 16) | t, m_tss[s][t]); for (DWORD t = 1; t < 14; t++) p->Record(RGSB_SS | (s << 16) | t, m_ss[s][t]); }",
        "\t*ppSB = p;",
    ])
    moi = NL.join([
        "\tCSBGpu* p = new CSBGpu(this);",
        "#ifdef JX_MOBILE",
        "\t// %s P2: khoi tron -> chup ba mang mot lan (Capture/Apply memcpy), khong Record 616 muc; ket qua nhu cu" % DAU,
        "\tp->m_bJxDay = true; p->Capture();",
        "#else",
        "\tfor (DWORD i = 0; i < 256; i++) p->Record(RGSB_RS | i, m_rs[i]);",
        "\tfor (DWORD s = 0; s < 8; s++) { for (DWORD t = 1; t < 33; t++) p->Record(RGSB_TSS | (s << 16) | t, m_tss[s][t]); for (DWORD t = 1; t < 14; t++) p->Record(RGSB_SS | (s << 16) | t, m_ss[s][t]); }",
        "#endif",
        "\t*ppSB = p;",
    ])
    s = thay1(s, cu, moi, "CreateStateBlock")
    ghi(p, s, cao)
    print("  D3D9onGPUDev.cpp: CreateStateBlock khoi tron (P2)")
else:
    print("  D3D9onGPUDev.cpp: P2 da co, bo qua")

# ---------------------------------------------------------------- 5. bo sinh SPIR-V: rao JX_MOBILE khi sinh lai
p = os.path.join(GOC, "ReverseTools", "mobile_x64", "dich_shader_gpu.py")
s = doc(p); cao = sum(1 for c in s if ord(c) >= 0x80)
cu = 'if opts: out.append("#ifdef JX_ANDROID\\t// [PALBUF 11/09] bien the bang mau = storage buffer (%s), chi Android" % " ".join(opts))'
moi = 'if opts: out.append("#ifdef JX_MOBILE\\t// [PALBUF 11/09] bien the bang mau = storage buffer (%s), mobile (Android + iOS) [MOBILE 13/09]" % " ".join(opts))'
if cu in s:
    s = thay1(s, cu, moi, "dich_shader_gpu.py rao")
    ghi(p, s, cao)
    print("  dich_shader_gpu.py: sinh lai se rao JX_MOBILE")
else:
    print("  dich_shader_gpu.py: da sua roi hoac khong khop, bo qua (%s)" % ("da co" if "JX_MOBILE" in s else "KHONG KHOP"))

# ---------------------------------------------------------------- 6. bo sinh MSL: them bien the KHOI + #define JX_MSL_CO_KHOI
p = os.path.join(GOC, "ios", "sinh_shader_msl.py")
s = doc(p); cao = sum(1 for c in s if ord(c) >= 0x80); NL = nl_cua(s)
if "g_nJxMslCoKhoi" not in s:
    cu = '    "g_Rep3GpuFSPalPs":   (0, 2),' + NL + '}'
    moi = NL.join([
        '    "g_Rep3GpuFSPalPs":   (0, 2),',
        '    "g_Rep3GpuFSPalPsKhoi": (0, 2),   # [MOBILE 13/09] 14 sampler (t0, t1 + 12 khoi texture mang) + 2 dem luu tru (bang mau, bang ps)',
        '}',
    ])
    s = thay1(s, cu, moi, "SO_DEM khoi")
    cu = '                  ("g_Rep3GpuFSPalBuf", "fragment"), ("g_Rep3GpuFSPalPs", "fragment")):'
    moi = '                  ("g_Rep3GpuFSPalBuf", "fragment"), ("g_Rep3GpuFSPalPs", "fragment"),' + NL + \
          '                  ("g_Rep3GpuFSPalPsKhoi", "fragment")):   # [MOBILE 13/09] atlas khoi (D3D9onGPUDev.cpp CreateShaders nhanh JX_APPLE)'
    s = thay1(s, cu, moi, "vong lap bien the")
    cu = 'io.open(RA, "w", encoding="latin-1", newline="\\r\\n").write("\\n".join(ra) + "\\n")'
    moi = NL.join([
        '# [MOBILE 13/09] co bien the khoi -> g_nJxMslCoKhoi = 1; header CHUA sinh lai thi giu stub g_nJxMslCoKhoi = 0 -> KHOI tu tat tren Metal',
        'ra.append("static const int g_nJxMslCoKhoi = 1;   // co bien the KHOI (D3D9onGPUDev.cpp CreateShaders nhanh JX_APPLE kiem luc chay)")',
        'ra.append("")',
        'io.open(RA, "w", encoding="latin-1", newline="\\r\\n").write("\\n".join(ra) + "\\n")',
    ])
    s = thay1(s, cu, moi, "define JX_MSL_CO_KHOI")
    ghi(p, s, cao)
    print("  ios/sinh_shader_msl.py: them bien the KHOI + JX_MSL_CO_KHOI (chay tren Mac de sinh Rep3ShadersGPU_msl.h)")
else:
    print("  ios/sinh_shader_msl.py: da co KHOI, bo qua")

# ---------------------------------------------------------------- 7. stub trong Rep3ShadersGPU_msl.h (chi Apple include) cho toi khi Mac sinh lai
p = os.path.join(R3, "Rep3ShadersGPU_msl.h")
s = doc(p); cao = sum(1 for c in s if ord(c) >= 0x80); NL = nl_cua(s)
if "g_nJxMslCoKhoi" not in s:
    s = s + NL.join([
        "// [MOBILE 13/09] CHUA sinh bien the atlas KHOI (can spirv-cross tren Mac: python3 ios/sinh_shader_msl.py). Khi sinh lai, hai dong duoi",
        "// duoc thay bang mang MSL that + g_nJxMslCoKhoi = 1. Con stub nay thi D3D9onGPUDev.cpp tu tat Rep3AtlasKhoi tren Metal (co ghi log).",
        "static const char g_Rep3GpuFSPalPsKhoiMsl[] = \"\";",
        "static const int g_nJxMslCoKhoi = 0;", ""])
    ghi(p, s, cao)
    print("  Rep3ShadersGPU_msl.h: them stub g_nJxMslCoKhoi = 0")
else:
    print("  Rep3ShadersGPU_msl.h: da co g_nJxMslCoKhoi, bo qua")

print("XONG %s a" % DAU)
