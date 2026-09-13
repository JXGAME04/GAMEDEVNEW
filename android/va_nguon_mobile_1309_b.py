# -*- coding: utf-8 -*-
r"""[TG 13/09] Mot lan cho CA HAI nen (rao JX_MOBILE): THE GIOI VE VAO RENDER TARGET KHI QUA TAI (P1 cua PHANTICH_SAU_IOS_ANDROID_1309.md,
da chot theo rang buoc "khong anh huong trai nghiem": chi lam viec khi khung dang roi tren man >= ~105 Hz; luc do the gioi ve cach khung
= 60 Hz deu (bang nac 60 cua thanh FPS), giao dien + can dieu khien van theo man; het qua tai tu ve duong cu, pixel y het).

Cho sua:
  1. KRepresentShell3.h/.cpp: thanh vien + ham JxTheGioi (hoi / bat dau / ket thuc+blit / ep), JxTheGioiCapNhat (quyet K moi 2 s),
     JxTheGioiKyIn ([TG] moi 30 s), JxTheGioiDocIni, JxTheGioiHuy; xuat "C" Rep3_JxTheGioi cho S3Client goi qua GetProcAddress
     (libmain.so KHONG link Represent3 - xem android-ky-hieu-trung-ten-dll-vs-so). Moc: RepresentBegin (mau xoa + dem khung),
     RepresentEnd (cap nhat sau Present), Create (doc ini), Invalidate/DeleteDeviceObjects (huy RT).
  2. D3D9onGPUDev.cpp: Rep3_JxEpTrinhChieu (be mat doi) -> the gioi cung phai ve that o khung toi.
  3. Wnds.cpp (S3Client): quanh pGameSpaceWnd->Paint(): hoi Represent3 -> 0 ve nhu cu / 1 ve vao RT roi blit / 2 chi blit.
  4. ios/JxIosMain.cpp: dang ky Rep3_JxTheGioi + Rep3_JxEpTrinhChieu vao bang tra tinh (iOS khong dlopen).
  5. android/du_lieu_ghi_de/config.ini: TheGioiRT / TheGioiRTEp / TheGioiToiThieuHz + Rep3AtlasKhoi=1 (khop dt_v4 dang chay, KHOI3 da do).
  6. ios/CMakeLists.txt: chay ios/va_sdl3_d1_metal.py (D1 cho Metal) luc configure.

Doc/ghi latin-1, giu CRLF/LF cua tung tep, moi cho vao bat khop DUNG MOT lan, chay lai vo hai.
Dung:  python android/va_nguon_mobile_1309_b.py [goc worktree]
"""
import io
import os
import sys

GOC = os.path.abspath(sys.argv[1]) if len(sys.argv) > 1 else os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
R3 = os.path.join(GOC, "Sources", "Represent", "Represent3")
DAU = "[TG 13/09]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s, cao_cu):
    cao = sum(1 for c in s if ord(c) >= 0x80)
    if cao != cao_cu:
        raise SystemExit("LOI: so byte cao doi %d -> %d, KHONG ghi: %s" % (cao_cu, cao, p))
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def nl_cua(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def thay1(s, cu, moi, ten):
    n = s.count(cu)
    if n != 1:
        raise SystemExit("LOI %s: tim thay %d cho (can 1)" % (ten, n))
    return s.replace(cu, moi)


def chen_sau_dong(s, NL, dau_dong, them, ten):
    """chen khoi `them` (list dong) NGAY SAU dong duy nhat bat dau bang dau_dong (giu nguyen dong do ke ca chu thich sau)"""
    dong = s.split(NL)
    vt = [i for i, d in enumerate(dong) if d.startswith(dau_dong)]
    if len(vt) != 1:
        raise SystemExit("LOI %s: dong bat dau '%s' co %d cho (can 1)" % (ten, dau_dong, len(vt)))
    i = vt[0]
    return NL.join(dong[:i + 1] + them + dong[i + 1:])


# ---------------------------------------------------------------- 1a. KRepresentShell3.h
p = os.path.join(R3, "KRepresentShell3.h")
s = doc(p); cao = sum(1 for c in s if ord(c) >= 0x80); NL = nl_cua(s)
if "JxTheGioi(" not in s:
    s = chen_sau_dong(s, NL, "    int NapTruoc(const char* pszImage, int nNguon);", [
        "#ifdef JX_MOBILE",
        "    // %s the gioi ve vao render target khi qua tai (Wnds.cpp goi Rep3_JxTheGioi qua GetProcAddress): 0 hoi, 1 bat dau, 2 ket thuc + blit, 3 ep ve that" % DAU,
        "    int  JxTheGioi(int nLenh, int nThamSo);   // nThamSo: lenh 0 = PaintFps muc tieu cua S3Client",
        "    void JxTheGioiHuy();",
        "    void JxTheGioiCapNhat(double dTrinhChieuMs);",
        "    void JxTheGioiKyIn();",
        "    void JxTheGioiDocIni();",
        "#endif",
    ], "khai ham JxTheGioi")
    s = chen_sau_dong(s, NL, "    int m_nTop;", [
        "#ifdef JX_MOBILE",
        "    LPDIRECT3DTEXTURE9    m_pTgTex;      // %s render target the gioi (BGRA8, cung co khung logic)" % DAU,
        "    LPDIRECT3DSURFACE9    m_pTgSurf;     // be mat cua m_pTgTex",
        "    LPDIRECT3DSURFACE9    m_pTgSurfCu;   // backbuffer giu trong luc ve vao RT",
        "    LPDIRECT3DSTATEBLOCK9 m_pTgSB;       // trang thai luu quanh lenh blit",
        "    int   m_nTgW, m_nTgH;",
        "    int   m_nTgTrangThai;                // 1 = dang ve vao RT",
        "    int   m_nTgLeft, m_nTgTop;           // goc toa do (m_nLeft/m_nTop) luc ve RT lan cuoi - khung chi blit dat lai de lop phu trung anh",
        "    DWORD m_dwTgMauXoa;                  // mau xoa cua RepresentBegin khung nay",
        "#endif",
    ], "thanh vien TG")
    ghi(p, s, cao)
    print("  KRepresentShell3.h: khai ham + thanh vien TG")
else:
    print("  KRepresentShell3.h: da co, bo qua")

# ---------------------------------------------------------------- 1b. KRepresentShell3.cpp
p = os.path.join(R3, "KRepresentShell3.cpp")
s = doc(p); cao = sum(1 for c in s if ord(c) >= 0x80); NL = nl_cua(s)
if "int KRepresentShell3::JxTheGioi(" not in s:
    # ctor
    s = chen_sau_dong(s, NL, "\tm_nTop = 0;", [
        "#ifdef JX_MOBILE",
        "\tm_pTgTex = NULL; m_pTgSurf = NULL; m_pTgSurfCu = NULL; m_pTgSB = NULL; m_nTgW = m_nTgH = 0; m_nTgTrangThai = 0; m_nTgLeft = m_nTgTop = 0; m_dwTgMauXoa = 0xff000000;\t// %s" % DAU,
        "#endif",
    ], "ctor TG")
    # khoi TG: truoc Rep3VeDem (sau khoi thong ke JX_MOBILE, sau dinh nghia s_dJxVeCpuCuoi / g_jxVeKhung / Rep3NapMs)
    khoi = [
        "#ifdef JX_MOBILE",
        "// ============================ %s THE GIOI VE VAO RENDER TARGET KHI QUA TAI ============================" % DAU,
        "// Chu 13/09: \"fix mot lan cho iOS va Android, khong anh huong trai nghiem\". Chi lam viec khi (a) PaintFps muc tieu >= 2 x ToiThieuHz (117) VA man",
        "// that (SDL_GetCurrentDisplayMode) >= 2 x ToiThieuHz: chu ky = max(1000/PaintFps, 1000/Hz man) - KHONG uoc tu khoang cach present (lan thu 13/09 16:00:",
        "// may ao 60 Hz co hai khung cach nhau 5 ms -> tuong man 120 Hz -> K=2 = the gioi 30 Hz, chu thay nhay man hinh luc vao map / di chuyen);",
        "// (b) viec/khung (ve CPU + chep + ghi + nop, khong ke cho) cua cac khung CO ve the gioi > 70 % chu ky trong cua so 2 s, VA (c) khoang cach",
        "// present thuc > 1,12 chu ky (khung dang roi). Luc do: the gioi ve cach khung (K = 2) vao render target, khung xen giua chi ve lai anh RT",
        "// + giao dien / can dieu khien -> the gioi 60 Hz DEU (bang nac 60 cua thanh FPS), giao dien theo man. Het qua tai (viec < 45 % chu ky",
        "// trong 2 cua so lien) -> K = 1 = duong cu, khong RT, khong them mot lenh nao. The gioi khong bao gio duoi TheGioiToiThieuHz (60).",
        "// Blit 1:1 bang quad point-sampling, cung phep chieu +0,5 cua DrawBitmap16 -> pixel y het; K = 2 chi lam NPC / dan cap nhat 60 Hz.",
        "// Khung chi blit: goc toa do m_nLeft/m_nTop dat lai = goc luc ve RT de lop phu (vong chon, nut ky nang) trung voi anh RT.",
        "// S3Client (Wnds.cpp) goi Rep3_JxTheGioi qua GetProcAddress (libmain.so khong link Represent3); iOS dang ky trong ios/JxIosMain.cpp.",
        "// [Client] TheGioiRT=1 (0 = tat han), TheGioiRTEp=0 (1 = luon RT K=1 de thu pixel, 2 = luon K=2 de thu), TheGioiToiThieuHz=60. Log [TG] moi ky.",
        "int g_nJxTheGioiEp = 0;\t// Rep3_JxEpTrinhChieu (D3D9onGPUDev.cpp): be mat / cua so doi -> khung toi phai ve the gioi that",
        "extern \"C\" double Rep3_JxManHinhMs();\t// D3D9onGPUDev.cpp: 1000 / tan so man SDL bao (0 = khong biet)",
        "static int s_nTgBat = 1, s_nTgEp = 0, s_nTgToiThieuHz = 60;",
        "static int s_nTgPaintFps = 0;\t\t\t\t\t// PaintFps muc tieu S3Client dua sang o lenh 0 (JxDoNhip_LayNhip)",
        "static int s_nTgK = 1;\t\t\t\t\t\t\t// 1 = ve the gioi moi khung, 2 = cach khung",
        "static unsigned s_uTgKhung = 0;\t\t\t\t\t// dem RepresentBegin",
        "static unsigned s_uTgVeThat = 0xFFFFFFFFu;\t\t// khung gan nhat that su ve the gioi (duong cu hoac vao RT) - de tinh viec/khung",
        "static unsigned s_uTgRTVe = 0xFFFFFFFFu;\t\t// khung gan nhat ve the gioi VAO RT (chi blit duoc o khung ngay sau no)",
        "static double s_dTgMinCach = 1e9, s_dTgCachTong = 0.0, s_dTgViecTong = 0.0; static unsigned s_uTgCachSo = 0, s_uTgViecSo = 0;",
        "static LARGE_INTEGER s_liTgCuoi = { 0 }; static DWORD s_dwTgCuaSo = 0; static int s_nTgCuaSoNhe = 0;",
        "static double s_dTgChuKy = 0.0, s_dTgViecTB = 0.0, s_dTgCachTB = 0.0;\t// ket qua cua so gan nhat (in [TG])",
        "static unsigned s_uTgDemVe = 0, s_uTgDemBlit = 0, s_uTgDemThuong = 0, s_uTgDemDoiK = 0;\t// thong ke ky [TG]",
        "",
        "void KRepresentShell3::JxTheGioiDocIni()",
        "{",
        "\ts_nTgBat = Rep3Ini(\"TheGioiRT\", 1) ? 1 : 0;",
        "\ts_nTgEp = Rep3Ini(\"TheGioiRTEp\", 0); if (s_nTgEp < 0 || s_nTgEp > 2) s_nTgEp = 0;",
        "\ts_nTgToiThieuHz = Rep3Ini(\"TheGioiToiThieuHz\", 60); if (s_nTgToiThieuHz < 30) s_nTgToiThieuHz = 30; if (s_nTgToiThieuHz > 120) s_nTgToiThieuHz = 120;",
        "\tRep3Log(\"[TG] the gioi RT khi qua tai: bat=%d ep=%d, the gioi toi thieu %d Hz (TheGioiRT / TheGioiRTEp / TheGioiToiThieuHz)\", s_nTgBat, s_nTgEp, s_nTgToiThieuHz);",
        "}",
        "",
        "void KRepresentShell3::JxTheGioiHuy()",
        "{",
        "\tif (m_nTgTrangThai == 1 && PD3DDEVICE && m_pTgSurfCu) PD3DDEVICE->SetRenderTarget(0, m_pTgSurfCu);",
        "\tm_nTgTrangThai = 0;",
        "\tSAFE_RELEASE(m_pTgSurfCu); SAFE_RELEASE(m_pTgSB); SAFE_RELEASE(m_pTgSurf); SAFE_RELEASE(m_pTgTex);",
        "\tm_nTgW = m_nTgH = 0; s_uTgRTVe = 0xFFFFFFFFu;",
        "}",
        "",
        "int KRepresentShell3::JxTheGioi(int nLenh, int nThamSo)",
        "{",
        "\tif (nLenh == 3) { g_nJxTheGioiEp = 1; return 0; }",
        "\tif (!PD3DDEVICE || m_bDeviceLost) return 0;",
        "\tif (nLenh == 0)",
        "\t{\t// hoi: 0 = duong cu, 1 = ve the gioi vao RT roi blit, 2 = chi blit anh RT cua khung truoc",
        "\t\tif (nThamSo > 0 && nThamSo <= 240) s_nTgPaintFps = nThamSo;",
        "\t\tint nK = s_nTgK;",
        "\t\tif (s_nTgEp == 1) nK = 1; else if (s_nTgEp == 2) nK = 2;",
        "\t\tif (!s_nTgBat || (nK != 2 && s_nTgEp != 1)) { s_uTgVeThat = s_uTgKhung; s_uTgDemThuong++; return 0; }",
        "\t\tif (!m_pTgTex || m_nTgW != g_nScreenWidth || m_nTgH != g_nScreenHeight)",
        "\t\t{\t// RT phai dung co khung logic (gap / mo, doi ho khung)",
        "\t\t\tJxTheGioiHuy();",
        "\t\t\tm_nTgW = g_nScreenWidth; m_nTgH = g_nScreenHeight;",
        "\t\t\tif (m_nTgW <= 0 || m_nTgH <= 0 || FAILED(PD3DDEVICE->CreateTexture(m_nTgW, m_nTgH, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_pTgTex, NULL))",
        "\t\t\t\t|| !m_pTgTex || FAILED(m_pTgTex->GetSurfaceLevel(0, &m_pTgSurf)) || !m_pTgSurf)",
        "\t\t\t{",
        "\t\t\t\tRep3Log(\"[TG] khong tao duoc render target %dx%d -> tat the gioi RT\", m_nTgW, m_nTgH);",
        "\t\t\t\tJxTheGioiHuy(); s_nTgBat = 0; s_uTgDemThuong++; return 0;",
        "\t\t\t}",
        "\t\t\tif (FAILED(PD3DDEVICE->CreateStateBlock(D3DSBT_ALL, &m_pTgSB))) m_pTgSB = NULL;",
        "\t\t\tRep3Log(\"[TG] render target the gioi %dx%d\", m_nTgW, m_nTgH);",
        "\t\t}",
        "\t\t// chi blit khi khung NGAY TRUOC da ve the gioi vao RT (anh moi dung 1 khung), khong bi ep (be mat doi), K = 2 -> hai khung ve mot lan",
        "\t\tif (nK == 2 && !g_nJxTheGioiEp && m_nTgTrangThai == 0 && s_uTgRTVe == s_uTgKhung - 1)",
        "\t\t\treturn 2;",
        "\t\ts_uTgVeThat = s_uTgKhung;",
        "\t\treturn 1;",
        "\t}",
        "\tif (nLenh == 1)",
        "\t{\t// bat dau ve the gioi vao RT",
        "\t\tif (!m_pTgSurf || m_nTgTrangThai == 1) return 0;",
        "\t\tif (FAILED(PD3DDEVICE->GetRenderTarget(0, &m_pTgSurfCu))) { m_pTgSurfCu = NULL; return 0; }",
        "\t\tif (FAILED(PD3DDEVICE->SetRenderTarget(0, m_pTgSurf))) { SAFE_RELEASE(m_pTgSurfCu); return 0; }",
        "\t\tPD3DDEVICE->Clear(0, NULL, D3DCLEAR_TARGET, m_dwTgMauXoa, 1.0f, 0L);",
        "\t\tm_nTgTrangThai = 1; m_nTgLeft = m_nLeft; m_nTgTop = m_nTop;",
        "\t\tg_nJxTheGioiEp = 0; s_uTgRTVe = s_uTgKhung; s_uTgDemVe++;",
        "\t\treturn 1;",
        "\t}",
        "\tif (nLenh == 2)",
        "\t{\t// ket thuc (neu dang ve) + blit anh RT len backbuffer",
        "\t\tif (m_nTgTrangThai == 1)",
        "\t\t{",
        "\t\t\tPD3DDEVICE->SetRenderTarget(0, m_pTgSurfCu); SAFE_RELEASE(m_pTgSurfCu); m_nTgTrangThai = 0;",
        "\t\t\tPD3DDEVICE->Clear(0, NULL, D3DCLEAR_TARGET, m_dwTgMauXoa, 1.0f, 0L);\t// lenh xoa dau khung cua RepresentBegin bi doi dich ve nuot -> xoa lai backbuffer",
        "\t\t}",
        "\t\telse",
        "\t\t{\t// khung chi blit: lop phu ve sau (vong chon, nut ky nang, chu the gioi) dung goc toa do luc ve RT de trung anh",
        "\t\t\tm_nLeft = m_nTgLeft; m_nTop = m_nTgTop; s_uTgDemBlit++;",
        "\t\t}",
        "\t\tif (!m_pTgTex) return 0;",
        "\t\tif (m_pTgSB) m_pTgSB->Capture();",
        "\t\tPD3DDEVICE->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);",
        "\t\tPD3DDEVICE->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);",
        "\t\tPD3DDEVICE->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);",
        "\t\tPD3DDEVICE->SetRenderState(D3DRS_COLORWRITEENABLE, 0xF);",
        "\t\tPD3DDEVICE->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);",
        "\t\tPD3DDEVICE->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1); PD3DDEVICE->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);",
        "\t\tPD3DDEVICE->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1); PD3DDEVICE->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);",
        "\t\tPD3DDEVICE->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE); PD3DDEVICE->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);",
        "\t\tPD3DDEVICE->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT); PD3DDEVICE->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);",
        "\t\tPD3DDEVICE->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP); PD3DDEVICE->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);",
        "\t\tPD3DDEVICE->SetTexture(0, m_pTgTex); PD3DDEVICE->SetTexture(1, NULL);",
        "\t\tPD3DDEVICE->SetFVF(D3DFVF_VERTEX2D);",
        "\t\tVERTEX2D v[4];",
        "\t\tconst float fW = (float)m_nTgW, fH = (float)m_nTgH;",
        "\t\tv[0].position = D3DXVECTOR4(0.0f, 0.0f, 100, 1); v[0].color = 0xffffffff; v[0].tu = 0.0f; v[0].tv = 0.0f;",
        "\t\tv[1].position = D3DXVECTOR4(fW, 0.0f, 100, 1);   v[1].color = 0xffffffff; v[1].tu = 1.0f; v[1].tv = 0.0f;",
        "\t\tv[2].position = D3DXVECTOR4(0.0f, fH, 100, 1);   v[2].color = 0xffffffff; v[2].tu = 0.0f; v[2].tv = 1.0f;",
        "\t\tv[3].position = D3DXVECTOR4(fW, fH, 100, 1);     v[3].color = 0xffffffff; v[3].tu = 1.0f; v[3].tv = 1.0f;",
        "\t\tPD3DDEVICE->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(VERTEX2D));",
        "\t\tPD3DDEVICE->SetTexture(0, NULL);",
        "\t\tif (m_pTgSB) m_pTgSB->Apply();",
        "\t\treturn 1;",
        "\t}",
        "\treturn 0;",
        "}",
        "",
        "// sau Present moi khung: chu ky = max(1000/PaintFps muc tieu, 1000/Hz man that); viec/khung cua cac khung co ve the gioi; quyet K moi 2 s",
        "void KRepresentShell3::JxTheGioiCapNhat(double dTrinhChieuMs)",
        "{",
        "\t(void)dTrinhChieuMs;",
        "\tLARGE_INTEGER li; QueryPerformanceCounter(&li);",
        "\tif (s_liTgCuoi.QuadPart)",
        "\t{",
        "\t\tconst double dCach = Rep3NapMs(s_liTgCuoi, li);",
        "\t\tif (dCach > 0.5 && dCach < 200.0) { if (dCach < s_dTgMinCach) s_dTgMinCach = dCach; s_dTgCachTong += dCach; s_uTgCachSo++; }",
        "\t}",
        "\ts_liTgCuoi = li;",
        "\tif (s_uTgVeThat == s_uTgKhung)",
        "\t{\t// khung nay CO ve the gioi (duong cu hoac RT): viec = ve CPU + (chep + ghi + nop), khong ke cho swapchain",
        "\t\tconst double dViec = s_dJxVeCpuCuoi + (g_jxVeKhung.dTong - g_jxVeKhung.dCho);",
        "\t\tif (dViec > 0.0 && dViec < 500.0) { s_dTgViecTong += dViec; s_uTgViecSo++; }",
        "\t}",
        "\tconst DWORD dwNow = timeGetTime();",
        "\tif (s_dwTgCuaSo == 0) { s_dwTgCuaSo = dwNow; return; }",
        "\tif (dwNow - s_dwTgCuaSo < 2000) return;",
        "\ts_dwTgCuaSo = dwNow;",
        "\t// chu ky = MAX(1000 / PaintFps muc tieu, 1000 / Hz man that): nac 60 tren man 120 -> 16,7 (khong K=2); PaintFps 120 nhung man da tut 60 Hz -> 16,7 (khong K=2)",
        "\tconst double dMucTieu = (s_nTgPaintFps > 0) ? 1000.0 / (double)s_nTgPaintFps : 1000.0 / 60.0;",
        "\tconst double dManHinh = Rep3_JxManHinhMs();",
        "\tdouble dChuKy = (dManHinh > dMucTieu) ? dManHinh : dMucTieu; if (dChuKy < 4.0) dChuKy = 4.0; if (dChuKy > 40.0) dChuKy = 40.0;",
        "\tconst double dCachTB = s_uTgCachSo ? s_dTgCachTong / s_uTgCachSo : 0.0;",
        "\tconst double dViecTB = s_uTgViecSo ? s_dTgViecTong / s_uTgViecSo : 0.0;",
        "\ts_dTgChuKy = dChuKy; s_dTgViecTB = dViecTB; s_dTgCachTB = dCachTB;",
        "\t// K = 2 chi khi the gioi van >= TheGioiToiThieuHz: 2 chu ky <= 1000 / ToiThieuHz (dung sai 3 %) - tuc PaintFps va man deu >= ~117 Hz",
        "\tconst bool bManNhanh = s_nTgPaintFps > 0 && dManHinh > 0.0 && (2.0 * dChuKy) <= (1000.0 / (double)s_nTgToiThieuHz) * 1.03;",
        "\tif (s_nTgK == 1)",
        "\t{",
        "\t\tif (s_nTgBat && bManNhanh && s_uTgViecSo >= 30 && dViecTB > 0.70 * dChuKy && dCachTB > 1.12 * dChuKy)",
        "\t\t{",
        "\t\t\ts_nTgK = 2; s_nTgCuaSoNhe = 0; s_uTgDemDoiK++;",
        "\t\t\tRep3Log(\"[TG] qua tai: viec/khung %.2f ms > 70%% chu ky %.2f (PaintFps %d, man %.2f ms), cach present TB %.2f -> the gioi cach khung (K=2, %d Hz)\", dViecTB, dChuKy, s_nTgPaintFps, dManHinh, dCachTB, (int)(500.0 / dChuKy + 0.5));",
        "\t\t}",
        "\t}",
        "\telse",
        "\t{",
        "\t\tif (!s_nTgBat || !bManNhanh) { s_nTgK = 1; s_nTgCuaSoNhe = 0; s_uTgDemDoiK++; Rep3Log(\"[TG] ve K=1: chu ky %.2f ms (PaintFps %d, man %.2f) khong du nhanh (hoac tat)\", dChuKy, s_nTgPaintFps, dManHinh); }",
        "\t\telse if (s_uTgViecSo >= 30 && dViecTB < 0.45 * dChuKy)",
        "\t\t{",
        "\t\t\tif (++s_nTgCuaSoNhe >= 2) { s_nTgK = 1; s_nTgCuaSoNhe = 0; s_uTgDemDoiK++; Rep3Log(\"[TG] het qua tai: viec/khung %.2f ms < 45%% chu ky %.2f trong 4 s -> K=1\", dViecTB, dChuKy); }",
        "\t\t}",
        "\t\telse s_nTgCuaSoNhe = 0;",
        "\t}",
        "\ts_dTgMinCach = 1e9; s_dTgCachTong = s_dTgViecTong = 0.0; s_uTgCachSo = s_uTgViecSo = 0;",
        "}",
        "",
        "void KRepresentShell3::JxTheGioiKyIn()",
        "{",
        "\tRep3Log(\"[TG] the gioi RT: bat=%d ep=%d K=%d | khung: duong cu %u, ve vao RT %u, chi blit %u | doi K %u lan | cua so gan nhat: chu ky %.2f ms (PaintFps %d, man %.2f ms), viec/khung the gioi TB %.2f, cach present TB %.2f, min %.2f\",",
        "\t\ts_nTgBat, s_nTgEp, s_nTgK, s_uTgDemThuong, s_uTgDemVe, s_uTgDemBlit, s_uTgDemDoiK, s_dTgChuKy, s_nTgPaintFps, Rep3_JxManHinhMs(), s_dTgViecTB, s_dTgCachTB, (s_dTgMinCach < 1e8) ? s_dTgMinCach : 0.0);",
        "\ts_uTgDemThuong = s_uTgDemVe = s_uTgDemBlit = s_uTgDemDoiK = 0;",
        "}",
        "#endif",
    ]
    s = thay1(s, NL + "void Rep3VeDem(const char* p)" + NL, NL + NL.join(khoi) + NL + "void Rep3VeDem(const char* p)" + NL, "khoi TG truoc Rep3VeDem")
    # xuat "C" sau Rep3_NapTruoc2
    cu = NL.join([
        "int Rep3_NapTruoc2(const char* pszImage, int nNguon)\t// [NAPNPC 09/09]",
        "{",
        "\tif (!g_pRep3ShellDuyNhat || !pszImage)",
        "\t\treturn 0;",
        "\treturn g_pRep3ShellDuyNhat->NapTruoc(pszImage, nNguon);",
        "}",
    ])
    moi = cu + NL + NL.join([
        "#ifdef JX_MOBILE",
        "// %s Wnds.cpp goi qua GetProcAddress (nhu Rep3_NapTruoc2, khong doi vtable); iOS dang ky trong ios/JxIosMain.cpp" % DAU,
        "extern \"C\" __declspec(dllexport)",
        "int Rep3_JxTheGioi(int nLenh, int nThamSo)",
        "{",
        "\tif (!g_pRep3ShellDuyNhat) return 0;",
        "\treturn g_pRep3ShellDuyNhat->JxTheGioi(nLenh, nThamSo);",
        "}",
        "#endif",
    ])
    s = thay1(s, cu, moi, "xuat Rep3_JxTheGioi")
    # Create: doc ini
    s = chen_sau_dong(s, NL, "\tg_nRep3StatSec   = Rep3Ini(\"Rep3StatSec\", 30);", [
        "#ifdef JX_MOBILE",
        "\tJxTheGioiDocIni();\t// %s" % DAU,
        "#endif",
    ], "Create doc ini TG")
    # RepresentBegin: mau xoa + dem khung
    s = chen_sau_dong(s, NL, "\tPD3DDEVICE->Clear( 0, NULL, D3DCLEAR_TARGET, bClear ? (0xff000000 | (Color & 0x00ffffff)) : D3DCOLOR_XRGB(0,0,0), 1.0f, 0L );", [
        "#ifdef JX_MOBILE",
        "\tm_dwTgMauXoa = bClear ? (0xff000000 | (Color & 0x00ffffff)) : D3DCOLOR_XRGB(0,0,0); s_uTgKhung++;\t// %s" % DAU,
        "#endif",
    ], "RepresentBegin TG")
    # RepresentEnd: sau Present
    cu = "{ LARGE_INTEGER liJx0, liJx1; QueryPerformanceCounter(&liJx0); PD3DDEVICE->Present(NULL,NULL,NULL,NULL); QueryPerformanceCounter(&liJx1); JxVeGiatGhi(Rep3NapMs(liJx0, liJx1)); }\t// [VE 11/09]"
    moi = "{ LARGE_INTEGER liJx0, liJx1; QueryPerformanceCounter(&liJx0); PD3DDEVICE->Present(NULL,NULL,NULL,NULL); QueryPerformanceCounter(&liJx1); JxVeGiatGhi(Rep3NapMs(liJx0, liJx1)); JxTheGioiCapNhat(Rep3NapMs(liJx0, liJx1)); }\t// [VE 11/09] %s" % DAU
    s = thay1(s, cu, moi, "RepresentEnd TG")
    # ky thong ke
    s = chen_sau_dong(s, NL, "\t\t\tJxVeKyIn();\t// [VE 11/09] [VE] + [VE-GOP] + [VE-NAP]", [
        "\t\t\tJxTheGioiKyIn();\t// %s" % DAU,
    ], "ky in TG")
    # huy RT khi mat / xoa doi tuong thiet bi
    s = thay1(s, "bool KRepresentShell3::InvalidateDeviceObjects()" + NL + "{" + NL,
              "bool KRepresentShell3::InvalidateDeviceObjects()" + NL + "{" + NL + "#ifdef JX_MOBILE" + NL + "\tJxTheGioiHuy();\t// %s" % DAU + NL + "#endif" + NL, "Invalidate TG")
    s = thay1(s, "void KRepresentShell3::DeleteDeviceObjects()" + NL + "{" + NL,
              "void KRepresentShell3::DeleteDeviceObjects()" + NL + "{" + NL + "#ifdef JX_MOBILE" + NL + "\tJxTheGioiHuy();\t// %s" % DAU + NL + "#endif" + NL, "Delete TG")
    ghi(p, s, cao)
    print("  KRepresentShell3.cpp: khoi TG + xuat + 5 moc")
else:
    print("  KRepresentShell3.cpp: da co TG, bo qua")

# ---------------------------------------------------------------- 2. D3D9onGPUDev.cpp: ep ve that khi be mat doi
p = os.path.join(R3, "D3D9onGPUDev.cpp")
s = doc(p); cao = sum(1 for c in s if ord(c) >= 0x80); NL = nl_cua(s)
if "g_nJxTheGioiEp" not in s:
    cu = "extern \"C\" void Rep3_JxEpTrinhChieu() { s_nJxEpTrinhChieu = 1; }"
    moi = NL.join([
        "extern int g_nJxTheGioiEp;\t// %s KRepresentShell3.cpp" % DAU,
        "extern \"C\" void Rep3_JxEpTrinhChieu() { s_nJxEpTrinhChieu = 1; g_nJxTheGioiEp = 1; }\t// %s be mat doi -> the gioi cung phai ve that o khung toi" % DAU,
        "// %s 1000 / tan so man SDL bao cho cua so hien tai (ms); 0 = khong biet. KRepresentShell3.cpp dung de KHONG bao gio cach khung tren man 60 Hz." % DAU,
        "extern \"C\" double Rep3_JxManHinhMs()",
        "{",
        "\tCDevGpu* d = g_pRep3DevGpu;",
        "\tif (!d || !d->m_pWin) return 0.0;",
        "\tconst SDL_DisplayID id = SDL_GetDisplayForWindow(d->m_pWin);",
        "\tconst SDL_DisplayMode* m = id ? SDL_GetCurrentDisplayMode(id) : NULL;",
        "\tif (!m || m->refresh_rate <= 1.0f) return 0.0;",
        "\treturn 1000.0 / (double)m->refresh_rate;",
        "}",
    ])
    s = thay1(s, cu, moi, "EpTrinhChieu TG")
    ghi(p, s, cao)
    print("  D3D9onGPUDev.cpp: Rep3_JxEpTrinhChieu ep ve the gioi")
else:
    print("  D3D9onGPUDev.cpp: da co, bo qua")

# ---------------------------------------------------------------- 3. Wnds.cpp (S3Client)
p = os.path.join(GOC, "Sources", "S3Client", "Ui", "Elem", "Wnds.cpp")
s = doc(p); cao = sum(1 for c in s if ord(c) >= 0x80); NL = nl_cua(s)
if "JxUi_TheGioi" not in s:
    cu = NL.join([
        "void Wnd_RenderWindows()",
        "{",
    ])
    moi = NL.join([
        "#ifdef JX_MOBILE",
        "// %s the gioi ve vao render target khi qua tai (Represent3 quyet; xem KRepresentShell3.cpp). Goi Rep3_JxTheGioi qua GetProcAddress" % DAU,
        "// nhu JxSdl_EpTrinhChieu (libmain.so khong link Represent3; iOS tra bang ky hieu tinh). Khong co ham (ban cu) -> 0 = ve nhu cu.",
        "typedef int (*PFN_Rep3JxTheGioi)(int, int);",
        "void JxDoNhip_LayNhip(int* pFps, int* pVsync, int* pSmooth);\t// S3Client.cpp (JX_MOBILE): PaintFps muc tieu hien tai (nac nguoi choi chon)",
        "static int JxUi_TheGioi(int nLenh, int nThamSo)",
        "{",
        "\tstatic PFN_Rep3JxTheGioi s_pfn = NULL; static int s_nThu = 0;",
        "\tif (!s_pfn && s_nThu < 8) { s_nThu++; HMODULE h = GetModuleHandleA(\"Represent3.dll\"); if (h) s_pfn = (PFN_Rep3JxTheGioi)GetProcAddress(h, \"Rep3_JxTheGioi\"); }",
        "\treturn s_pfn ? s_pfn(nLenh, nThamSo) : 0;",
        "}",
        "#endif",
        "void Wnd_RenderWindows()",
        "{",
    ])
    s = thay1(s, cu, moi, "Wnds.cpp ham JxUi_TheGioi")
    cu = NL.join([
        "\tif (s_WndStation.pGameSpaceWnd && s_WndStation.bPaintGameSpace)",
        "\t\ts_WndStation.pGameSpaceWnd->Paint();",
    ])
    moi = NL.join([
        "\tif (s_WndStation.pGameSpaceWnd && s_WndStation.bPaintGameSpace)",
        "#ifdef JX_MOBILE",
        "\t{\t// %s 0 = ve nhu cu; 1 = ve the gioi vao RT roi blit; 2 = khung qua tai xen giua: chi blit anh RT khung truoc" % DAU,
        "\t\tint nFpsMucTieu = 0; JxDoNhip_LayNhip(&nFpsMucTieu, NULL, NULL);",
        "\t\tconst int nTg = JxUi_TheGioi(0, nFpsMucTieu);",
        "\t\tif (nTg == 1 && JxUi_TheGioi(1, 0))",
        "\t\t{\t// ve the gioi vao RT roi blit; bat dau that bai (hiem) -> roi xuong duong cu, khong bao gio mat the gioi mot khung",
        "\t\t\ts_WndStation.pGameSpaceWnd->Paint();",
        "\t\t\tJxUi_TheGioi(2, 0);",
        "\t\t}",
        "\t\telse if (nTg == 2)",
        "\t\t\tJxUi_TheGioi(2, 0);",
        "\t\telse",
        "\t\t\ts_WndStation.pGameSpaceWnd->Paint();",
        "\t}",
        "#else",
        "\t\ts_WndStation.pGameSpaceWnd->Paint();",
        "#endif",
    ])
    s = thay1(s, cu, moi, "Wnds.cpp moc Paint")
    ghi(p, s, cao)
    print("  Wnds.cpp: moc the gioi RT")
else:
    print("  Wnds.cpp: da co, bo qua")

# ---------------------------------------------------------------- 4. ios/JxIosMain.cpp: dang ky ky hieu
p = os.path.join(GOC, "ios", "JxIosMain.cpp")
s = doc(p); cao = sum(1 for c in s if ord(c) >= 0x80); NL = nl_cua(s)
if "Rep3_JxTheGioi" not in s:
    s = chen_sau_dong(s, NL, "extern \"C\" int             Rep3_NapTruoc2(const char*, int);", [
        "extern \"C\" int             Rep3_JxTheGioi(int, int);          // %s Represent3/KRepresentShell3.cpp (Wnds.cpp goi qua GetProcAddress)" % DAU,
        "extern \"C\" void            Rep3_JxEpTrinhChieu();             // [BKG] D3D9onGPUDev.cpp (KSdlApp goi qua GetProcAddress) - tu 13/09 co tren iOS (JX_MOBILE)",
    ], "khai ky hieu iOS")
    cu = NL.join([
        "// Ghi chu: Rep3_JxEpTrinhChieu nam trong \"#ifdef JX_ANDROID\" (D3D9onGPUDev.cpp:1731) nen ban iOS",
        "// KHONG co ham do; noi goi no trong KSdlApp cung rao JX_ANDROID nen khong can dang ky.",
    ])
    moi = "// [MOBILE 13/09] Rep3_JxEpTrinhChieu / Rep3_JxTheGioi nay rao JX_MOBILE (co tren iOS) -> dang ky o duoi de KSdlApp / Wnds.cpp tra duoc."
    s = thay1(s, cu, moi, "chu thich cu EpTrinhChieu")
    s = chen_sau_dong(s, NL, "\tJxPosix_DangKyKyHieu(\"Represent3.dll\", \"Rep3_NapTruoc2\",       (void*)&Rep3_NapTruoc2);", [
        "\tJxPosix_DangKyKyHieu(\"Represent3.dll\", \"Rep3_JxTheGioi\",       (void*)&Rep3_JxTheGioi);       // %s" % DAU,
        "\tJxPosix_DangKyKyHieu(\"Represent3.dll\", \"Rep3_JxEpTrinhChieu\",  (void*)&Rep3_JxEpTrinhChieu);  // [BKG] be mat doi -> khung toi phai trinh chieu / ve the gioi that",
    ], "dang ky ky hieu iOS")
    ghi(p, s, cao)
    print("  ios/JxIosMain.cpp: dang ky Rep3_JxTheGioi + Rep3_JxEpTrinhChieu")
else:
    print("  ios/JxIosMain.cpp: da co, bo qua")

# ---------------------------------------------------------------- 5. config.ini lop ghi de
p = os.path.join(GOC, "android", "du_lieu_ghi_de", "config.ini")
s = doc(p); cao = sum(1 for c in s if ord(c) >= 0x80); NL = nl_cua(s)
if "TheGioiRT=" not in s:
    s = thay1(s, "; [VE 11/09] nap KHUNG sprite o luong nen (Represent3, chi Android):", "; [VE 11/09] nap KHUNG sprite o luong nen (Represent3, mobile: Android + iOS tu 13/09):", "chu thich NapKhungNen")
    s = thay1(s, "Rep3AtlasKhoi=0" + NL, "Rep3AtlasKhoi=1" + NL, "Rep3AtlasKhoi")
    s = chen_sau_dong(s, NL, "Rep3AtlasKhoiLop=8", [
        "; [MOBILE 13/09] Rep3AtlasKhoi=1 khop config dt_v4 dang chay (KHOI3 do 11 phut nhiet 0, khoang ve 4,16 -> 1,21 ms); iOS: tu tat toi khi Mac sinh bien the MSL.",
        "; %s The gioi ve vao render target KHI QUA TAI (man >= ~105 Hz, viec/khung > 70 %% chu ky trong 2 s va khung dang roi): the gioi ve cach khung" % DAU,
        ";   (60 Hz deu = nac 60 cua thanh FPS), giao dien + can dieu khien van theo man; het qua tai tu ve duong cu. Pixel y het khi khong qua tai.",
        ";   TheGioiRT=1 bat / 0 tat han; TheGioiRTEp: 0 tu dong, 1 = luon ve qua RT moi khung (thu pixel), 2 = luon cach khung (thu); TheGioiToiThieuHz = the gioi khong bao gio duoi (60).",
        ";   Doc [TG] trong jx_rep3.log moi 30 s (duong cu / ve vao RT / chi blit / doi K).",
        "TheGioiRT=1",
        "TheGioiRTEp=0",
        "TheGioiToiThieuHz=60",
    ], "config TG")
    ghi(p, s, cao)
    print("  config.ini: TheGioiRT / TheGioiRTEp / TheGioiToiThieuHz, Rep3AtlasKhoi=1")
else:
    print("  config.ini: da co, bo qua")

# ---------------------------------------------------------------- 6. ios/CMakeLists.txt: va SDL Metal (D1)
p = os.path.join(GOC, "ios", "CMakeLists.txt")
s = doc(p); cao = sum(1 for c in s if ord(c) >= 0x80); NL = nl_cua(s)
if "va_sdl3_d1_metal.py" not in s:
    cu = "  add_subdirectory(${JX_SDL3_SRC} SDL3 EXCLUDE_FROM_ALL)"
    moi = NL.join([
        "  # [MOBILE 13/09] D1 cho Metal: drawable = khung logic theo hint JX_SWAPCHAIN_W/H (ios/va_sdl3_d1_metal.py, idempotent) - nhu android/va_sdl3_d1.py ben Vulkan",
        "  find_package(Python3 COMPONENTS Interpreter)",
        "  if(Python3_Interpreter_FOUND)",
        "    execute_process(COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/va_sdl3_d1_metal.py ${JX_SDL3_SRC}",
        "                    RESULT_VARIABLE JX_VA_D1M_KQ OUTPUT_VARIABLE JX_VA_D1M_RA ERROR_VARIABLE JX_VA_D1M_LOI OUTPUT_STRIP_TRAILING_WHITESPACE)",
        "    if(NOT JX_VA_D1M_KQ EQUAL 0)",
        "      message(FATAL_ERROR \"va_sdl3_d1_metal.py loi (${JX_VA_D1M_KQ}): ${JX_VA_D1M_RA} ${JX_VA_D1M_LOI}\")",
        "    endif()",
        "    message(STATUS \"va_sdl3_d1_metal.py: ${JX_VA_D1M_RA}\")",
        "  else()",
        "    message(WARNING \"Khong co python3 -> bo qua va_sdl3_d1_metal.py (D1 Metal tat: GPU to full res nhu truoc)\")",
        "  endif()",
        "  add_subdirectory(${JX_SDL3_SRC} SDL3 EXCLUDE_FROM_ALL)",
    ])
    s = thay1(s, cu, moi, "ios CMake va SDL")
    ghi(p, s, cao)
    print("  ios/CMakeLists.txt: goi va_sdl3_d1_metal.py luc configure")
else:
    print("  ios/CMakeLists.txt: da co, bo qua")

print("XONG %s b" % DAU)
