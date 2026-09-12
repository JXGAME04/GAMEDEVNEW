# -*- coding: utf-8 -*-
r"""[CHUATLAS 11/09] PORT buoc (d) cua loat [MANG 09/09] tu duong D3D11 (ban PC) sang duong SDL_GPU (ban mobile):
CHO TEXTURE MANAGED (chu / anh dung san cua nhan vat) VAO ATLAS.

Boi canh: chu 11/09 chi "ban sua nam o main chinh cua du an". Loat [MANG 09/09] a-f tren nhanh main da giai xong bai toan nay cho
duong D3D11 (ban PC 144 fps, dong van > 135), nhung mobile chay duong D3D9onGPU (SDL_GPU/Vulkan) nen chua duoc huong.
Buoc (d) = commit 4ee8e6ad: "chu (texture MANAGED) vao atlas": tren PC chi la NOI DIEU KIEN cua CAtlasMgr::Eligible, vi texture ao
da ho tro ban CPU + tai lai vung ban. So do cua PC truoc khi sua: vo lo 100-450 lan/khung chi vi chu bi loai khoi atlas do POOL.

Do tren mobile (log Fold 7 11/09): ly do quad khong gop = texture0 82-88 %, pipeline 12-17 %. Chu tren mobile la MANAGED
(REP3_POOL_MANAGED = D3DPOOL_MANAGED khi Rep3Ex=0, D3D_Device.h:83) nen CAtlasMgrGpu::Eligible loai -> moi nhan ten / dong chat /
so sat thuong deu cat lo. Day dung la nguyen nhan (d) cua PC.

Sua (chi JX_ANDROID, co cong tac):
  1. Eligible: cho D3DPOOL_MANAGED vao atlas khi [Client] Rep3AtlasManaged=1 (mac dinh 1; 0 = nhu cu).
  2. Dem de DO duoc: so texture ao duoc cap cho trong trang theo tung loai pool, va so lan phai XIN O MOI vi noi dung doi giua khung
     (day la rui ro cua buoc nay: texture MANAGED bi ghi lai thuong xuyen se xin o moi lien tuc). In trong [VE-GOP].
Doc/ghi latin-1, giu CRLF/LF, moc khop dung 1 cho, byte cao khong doi; chay lai nhieu lan khong sao.
Dung:  python android\va_nguon_android_chuatlas.py
"""
import io
import os
import re

GOC = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
R3 = os.path.join(GOC, "Sources", "Represent", "Represent3")
CFG = os.path.join(GOC, "android", "du_lieu_ghi_de", "config.ini")
DAU = "[CHUATLAS 11/09]"


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


def sau_dong(s, rx, them, ten):
    m = list(re.finditer(rx, s, re.M))
    if len(m) != 1:
        raise SystemExit("moc dong '%s' khop %d cho (can 1)" % (ten, len(m)))
    return s[:m[0].end()] + them + s[m[0].end():]


# ============================================================ BaseInclude.h
p = os.path.join(R3, "BaseInclude.h")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = sau_dong(s, r"^extern unsigned g_uJxAtlasCum;[^\n]*\n",
        "extern int g_nJxAtlasManaged;\t// " + DAU + " [Client] Rep3AtlasManaged: cho texture MANAGED (chu / anh dung san) vao atlas - port buoc (d) cua [MANG 09/09] ben duong D3D11\n"
        "extern unsigned g_uJxAtlasODat[2], g_uJxAtlasOMoi;\t// " + DAU + " o atlas da cap: [0] DEFAULT, [1] MANAGED; so lan phai xin O MOI vi noi dung doi giua khung\n",
        "extern atlas cum")
    ghi(p, s, nl, cao)

# ============================================================ KRepresentShell3.cpp
p = os.path.join(R3, "KRepresentShell3.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = sau_dong(s, r"^unsigned g_uJxAtlasCum = 0;[^\n]*\n",
        "int g_nJxAtlasManaged = 1;\t// " + DAU + "\n"
        "unsigned g_uJxAtlasODat[2] = { 0, 0 }, g_uJxAtlasOMoi = 0;\t// " + DAU + "\n",
        "dinh nghia atlas cum")
    s = thay1(s, "\tg_nJxBindRing       = Rep3Ini(\"Rep3BindRing\", 1) ? 1 : 0;",
        "\tg_nJxAtlasManaged   = Rep3Ini(\"Rep3AtlasManaged\", 1) ? 1 : 0;\t// " + DAU + " 1 = texture MANAGED (chu, anh dung san) cung vao atlas (port buoc (d) cua [MANG 09/09]); 0 = nhu cu\n"
        "\tg_nJxBindRing       = Rep3Ini(\"Rep3BindRing\", 1) ? 1 : 0;",
        "doc ini bind ring")
    s = thay1(s, "| ps bang %u muc (tran %u) | atlas mang=%d: %u cum\",\n",
        "| ps bang %u muc (tran %u) | atlas mang=%d: %u cum | o atlas: DEFAULT %u, MANAGED %u (managed=%d), xin o moi %u\",\n",
        "[VE-GOP] format")
    s = thay1(s, "g_uJxPsBangMax, g_uJxPsTran, g_nJxAtlasMang, g_uJxAtlasCum);",
        "g_uJxPsBangMax, g_uJxPsTran, g_nJxAtlasMang, g_uJxAtlasCum, g_uJxAtlasODat[0], g_uJxAtlasODat[1], g_nJxAtlasManaged, g_uJxAtlasOMoi);",
        "[VE-GOP] args")
    s = thay1(s, "\ts_dJxVeCpuTong = 0.0; s_dJxVeCpuMax = 0.0; s_uJxVeCpuKhung = 0; g_uJxPsBangMax = 0; g_uJxPsTran = 0;",
        "\ts_dJxVeCpuTong = 0.0; s_dJxVeCpuMax = 0.0; s_uJxVeCpuKhung = 0; g_uJxPsBangMax = 0; g_uJxPsTran = 0; g_uJxAtlasODat[0] = g_uJxAtlasODat[1] = 0; g_uJxAtlasOMoi = 0;\t// " + DAU,
        "dat lai ky")
    ghi(p, s, nl, cao)

# ============================================================ D3D9onGPURes.cpp
p = os.path.join(R3, "D3D9onGPURes.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    # 1. Eligible: cho MANAGED vao atlas
    s = thay1(s, "\tif (pool != D3DPOOL_DEFAULT) return false;\n",
        "#ifdef JX_ANDROID\n"
        "\t// " + DAU + " port buoc (d) cua [MANG 09/09] (commit 4ee8e6ad ben duong D3D11): chu / anh dung san la POOL_MANAGED, truoc day bi loai\n"
        "\t// khoi atlas CHI vi pool -> moi nhan ten / dong chat / so sat thuong cat lo quad. Texture ao da ho tro ban CPU + tai lai vung ban.\n"
        "\tif (pool != D3DPOOL_DEFAULT && !(g_nJxAtlasManaged && pool == D3DPOOL_MANAGED)) return false;\n"
        "#else\n"
        "\tif (pool != D3DPOOL_DEFAULT) return false;\n"
        "#endif\n",
        "Eligible pool")
    # 2. dem o cap theo pool
    s = thay1(s, "\t\t\t\tm_uGpuBytes = m_w * m_h * m_pPage->m_bpp; m_pDev->m_uTexBytes += m_uGpuBytes; g_uRep3GpuTexCount++; g_uRep3GpuTexBytes += m_uGpuBytes;\n",
        "\t\t\t\tm_uGpuBytes = m_w * m_h * m_pPage->m_bpp; m_pDev->m_uTexBytes += m_uGpuBytes; g_uRep3GpuTexCount++; g_uRep3GpuTexBytes += m_uGpuBytes;\n"
        "#ifdef JX_ANDROID\n"
        "\t\t\t\tg_uJxAtlasODat[(m_pool == D3DPOOL_MANAGED) ? 1 : 0]++;\t// " + DAU + " dem o atlas theo loai bo nho\n"
        "#endif\n",
        "dem o cap")
    # 3. dem xin o moi (rui ro cua buoc nay)
    s = thay1(s, "\t\t\t\t\tif (m_pDev->m_pAtlas && m_pDev->m_pAtlas->Alloc(m_w, m_h, m_fi.gpu, &pNew, &x, &y))\n",
        "#ifdef JX_ANDROID\n"
        "\t\t\t\t\tg_uJxAtlasOMoi++;\t// " + DAU + " noi dung doi giua khung -> phai xin o MOI (texture MANAGED bi ghi lai thuong xuyen se lam so nay tang vot)\n"
        "#endif\n"
        "\t\t\t\t\tif (m_pDev->m_pAtlas && m_pDev->m_pAtlas->Alloc(m_w, m_h, m_fi.gpu, &pNew, &x, &y))\n",
        "dem xin o moi")
    ghi(p, s, nl, cao)

# ============================================================ config.ini
p = CFG
s, nl, cao = doc(p)
if "Rep3AtlasManaged" in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = thay1(s, "Rep3BindRing=1\n",
        "Rep3BindRing=1\n"
        "; " + DAU + " Rep3AtlasManaged=1: cho texture MANAGED (chu, anh dung san cua nhan vat) vao atlas - port buoc (d) cua loat [MANG 09/09]\n"
        ";   ben duong D3D11 cua ban PC (commit 4ee8e6ad). Truoc day chu bi loai khoi atlas CHI vi loai bo nho -> moi nhan ten / dong chat /\n"
        ";   so sat thuong deu cat lo quad (mobile: texture0 chiem 82-88 %% ly do khong gop). 0 = nhu cu.\n"
        ";   Doc [VE-GOP] \"o atlas: DEFAULT n, MANAGED m ... xin o moi k\": k tang vot = noi dung doi lien tuc, nen tat.\n"
        "Rep3AtlasManaged=1\n",
        "config Rep3BindRing")
    ghi(p, s, nl, cao)

print("xong")
