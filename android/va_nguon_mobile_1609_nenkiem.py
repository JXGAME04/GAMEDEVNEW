# -*- coding: latin-1 -*-
r"""[NENKIEM 16/09] Ban CHAN DOAN nen den Fold 7: doc nguoc GPU kiem tung o anh nen vung + ghi moi lan xa giua khung + giu dich sau xa.

BOI CANH (16/09, phien 11:08 Fold 7, anh chup 11:09 tai Tien Coc Dong 193/196):
  Log rep3 khung 67 (t = 5975..6021 ms) cho thay lan ghep dau cua anh #5 (vung 96,97) ghi lenh ve du 8 o san + 2 xuong (o 31, 39,
  47, 54, 55, 61, 62, 63, 64, 65 -> "VE"), nhung tren man hinh chi o 31 (3,7) co, tu o 39 tro di DEN het (bac thang 1-2-3 o
  + xuong bien mat). Tuc lenh ve DA GHI ma diem anh khong ra: mat o phia GPU / bo dung lenh, khong phai du lieu, khong phai
  GetImage NULL. Giai lap + PC cung ma khong bi -> can su that tu chinh dien thoai thay vi doan tiep.

VIEC (chi JX_MOBILE, PC bien dich y het - ios/kiem_android_tuongduong.py --pc phai DAT):
  1. KRepresentShell3: [NENKIEM] - ClearImageData xoa mat na; DrawPrimitivesOnImage ghi bit tung o 64x64 da ghi lenh ve (VE);
     sau Present doc nguoc render target (Rep3Gpu_DocLaiAnh, khong xa giua khung vi khong con lenh cho) roi kiem 8x8 o:
     '#' co diem anh, '.' rong (khong ve), 'X' = DA VE MA DEN (loi), 'o' = co ma khong ve (vat phu / xuong). Kiem lai dinh ky
     4 s cac anh dang hien (DrawPrimitives thay _*PlaceGround*_) de bat thoi diem mat. [Client] Rep3NenKiem=0 de tat.
  2. D3D9onGPUDev: [XAGIUA] ghi moi lan SubmitFrame(false) (ly do: ReadbackTexture / ReadbackRegion / ThuLaiCpu / LockRect RT /
     doi khung bay / Reset) kem dich dang ve. SUA: sau xa giua khung ma dang ve vao RT thi cham lai RT + ghi lai RGCMD_TARGET
     (truoc: FrameReset xoa lenh + m_touched -> phan con lai cua khung roi len swapchain, RT khong con trong m_touched = scissor 0x0).
  3. D3D9onGPURes: NewVersion(bTarget) ghi [NENKIEM] (anh nen vung chi duoc cap 1 lan; cap lai = mat noi dung).
  4. KScenePlaceRegionC: [PGND-R] ghi MOI lan ghep (bo nguong 3 ms) de doi chieu vung (x,y) <-> anh #N.

Chay:  PYTHONIOENCODING=latin-1 python android\va_nguon_mobile_1609_nenkiem.py
"""
import io
import os

GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TAG = "[NENKIEM 16/09]"


def va(duong, cap):
    p = os.path.join(GOC, duong)
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    cao0 = sum(1 for c in s if ord(c) >= 128)
    lf0 = s.count("\n") - s.count("\r\n")
    if TAG in s:
        print("  bo qua (da co %s): %s" % (TAG, duong))
        return
    for cu, moi in cap:
        if s.count(cu) != 1:
            raise SystemExit("HONG: neo xuat hien %d lan trong %s:\n%s" % (s.count(cu), duong, cu[:200]))
        s = s.replace(cu, moi, 1)
    cao1 = sum(1 for c in s if ord(c) >= 128)
    lf1 = s.count("\n") - s.count("\r\n")
    if cao1 != cao0 or lf1 != lf0:
        raise SystemExit("HONG: byte cao %d -> %d, LF le %d -> %d trong %s" % (cao0, cao1, lf0, lf1, duong))
    if any(ord(c) >= 128 for cu, moi in cap for c in moi):
        raise SystemExit("HONG: doan moi co byte >= 128")
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("  da va: %s" % duong)


# ---------------------------------------------------------------- 1. lop SDL_GPU: xa giua khung + doc nguoc
DEV = "Sources/Represent/Represent3/D3D9onGPUDev.cpp"
va(DEV, [
    # bien ly do + dem
    ("static int s_nJxCullCpuCur = 0;",
     "const char* g_szJxXaLyDo = NULL; unsigned g_uJxXaGiua = 0;\t// [NENKIEM 16/09] ly do + so lan xa giua khung (SubmitFrame(false)) -> [XAGIUA] jx_rep3.log\r\n"
     "static int s_nJxCullCpuCur = 0;"),
    # dau SubmitFrame: ghi moi lan xa giua khung
    ("\tif (!bPresent) m_bJxKhungCoFlush = true;\t// [BKG 11/09] flush giua khung (doc lai / doi khung bay): khung nay khong dung lam \"khung truoc\" de so\r\n",
     "\tif (!bPresent) m_bJxKhungCoFlush = true;\t// [BKG 11/09] flush giua khung (doc lai / doi khung bay): khung nay khong dung lam \"khung truoc\" de so\r\n"
     "\tif (!bPresent) { g_uJxXaGiua++; RgLog(\"[XAGIUA] xa giua khung #%u%s: %u lenh, %u tai, dich %s %ux%u\", g_uJxXaGiua, g_szJxXaLyDo ? g_szJxXaLyDo : \"\", (unsigned)m_cmds.size(), (unsigned)m_texUploads.size(), m_pRtTex ? \"RT\" : \"backbuffer\", m_pRtTex ? m_pRtTex->m_w : m_bbW, m_pRtTex ? m_pRtTex->m_h : m_bbH); g_szJxXaLyDo = NULL; }\t// [NENKIEM 16/09]\r\n"),
    # cuoi SubmitFrame: sau xa giua khung ma dang ve vao RT -> giu dich
    ("#endif\r\n\tFrameReset();\r\n\treturn true;\r\n}\r\n",
     "#endif\r\n\tFrameReset();\r\n"
     "#ifdef JX_MOBILE\r\n"
     "\tif (!bPresent && m_pRtTex && m_pRtTex->m_pGpu)\r\n"
     "\t{\t// [NENKIEM 16/09] xa giua khung khi dang ve vao RT (ghep anh nen vung): FrameReset xoa lenh + m_touched -> lenh ve tiep theo roi len swapchain\r\n"
     "\t\t// (pass builder bat dau tu pSwap) va RT khong con trong m_touched (scissor 0x0). Cham lai + ghi lai dich de phan con lai cua khung van vao RT.\r\n"
     "\t\tm_pRtTex->FrameEnd(); TouchTex(m_pRtTex); m_pRtTex->MarkUsed();\r\n"
     "\t\tRgCmd c; memset(&c, 0, sizeof(c)); c.type = RGCMD_TARGET; c.pTarget = m_pRtTex->m_pGpu; m_cmds.push_back(c);\r\n"
     "\t\tRgLog(\"[XAGIUA] giu lai dich RT %ux%u sau xa\", m_pRtTex->m_w, m_pRtTex->m_h);\r\n"
     "\t}\r\n"
     "#endif\r\n"
     "\treturn true;\r\n}\r\n"),
    # ly do: ReadbackTexture
    ("\tif (!pTex || !pDst) return false;\r\n\tif (!m_cmds.empty() || !m_texUploads.empty()) SubmitFrame(false);\r\n",
     "\tif (!pTex || !pDst) return false;\r\n"
     "#ifdef JX_MOBILE\r\n\tif (!g_szJxXaLyDo) g_szJxXaLyDo = \" (ReadbackTexture)\";\t// [NENKIEM 16/09]\r\n#endif\r\n"
     "\tif (!m_cmds.empty() || !m_texUploads.empty()) SubmitFrame(false);\r\n"),
    # ly do: ReadbackRegion
    ("\tif (!pTex || !pDst || !w || !h || !bpp) return false;\r\n\tif (!m_cmds.empty() || !m_texUploads.empty()) SubmitFrame(false);\r\n",
     "\tif (!pTex || !pDst || !w || !h || !bpp) return false;\r\n"
     "#ifdef JX_MOBILE\r\n\tif (!g_szJxXaLyDo) g_szJxXaLyDo = \" (ReadbackRegion)\";\t// [NENKIEM 16/09]\r\n#endif\r\n"
     "\tif (!m_cmds.empty() || !m_texUploads.empty()) SubmitFrame(false);\r\n"),
    # ly do: doi khung bay
    ("\t\tif (d->m_bFrameOpen || !d->m_cmds.empty() || !d->m_texUploads.empty()) d->SubmitFrame(false);\r\n",
     "\t\tg_szJxXaLyDo = \" (doi khung bay)\";\t// [NENKIEM 16/09]\r\n"
     "\t\tif (d->m_bFrameOpen || !d->m_cmds.empty() || !d->m_texUploads.empty()) d->SubmitFrame(false);\r\n"),
    # ly do: Reset
    ("\tif (m_bFrameOpen || !m_cmds.empty()) SubmitFrame(false);\r\n#ifdef JX_MOBILE\r\n\tm_bJxCoKhungTruoc = false; s_nJxEpTrinhChieu = 1;",
     "#ifdef JX_MOBILE\r\n\tg_szJxXaLyDo = \" (Reset)\";\t// [NENKIEM 16/09]\r\n#endif\r\n"
     "\tif (m_bFrameOpen || !m_cmds.empty()) SubmitFrame(false);\r\n#ifdef JX_MOBILE\r\n\tm_bJxCoKhungTruoc = false; s_nJxEpTrinhChieu = 1;"),
    # ham doc nguoc cho KRepresentShell3
    ("void Rep3Gpu_DichSeXoa(IDirect3DDevice9* pDev, int bBat)\r\n{\r\n\tif (pDev && g_nRep3ApiOn == 100) ((CDevGpu*)pDev)->m_bJxDichSeXoa = (bBat != 0);\r\n}\r\n",
     "void Rep3Gpu_DichSeXoa(IDirect3DDevice9* pDev, int bBat)\r\n{\r\n\tif (pDev && g_nRep3ApiOn == 100) ((CDevGpu*)pDev)->m_bJxDichSeXoa = (bBat != 0);\r\n}\r\n"
     "// [NENKIEM 16/09] KRepresentShell3 doc nguoc mot render target (anh nen vung 512x512 BGRA8) ve CPU de kiem tung o sau khi ghep;\r\n"
     "// goi SAU Present (khong con lenh cho nen ReadbackTexture khong xa giua khung). Tra 1 = duoc, 0 = khong.\r\n"
     "int Rep3Gpu_DocLaiAnh(IDirect3DDevice9* pDev, IDirect3DTexture9* pTex, void* pBuf, unsigned uPitch, unsigned* pW, unsigned* pH)\r\n"
     "{\r\n"
     "\tif (!pDev || !pTex || !pBuf || g_nRep3ApiOn != 100) return 0;\r\n"
     "\tCDevGpu* d = (CDevGpu*)pDev; CTexGpu* t = (CTexGpu*)pTex;\r\n"
     "\tif (!t->m_pGpu || !t->m_bGpuTarget) return 0;\r\n"
     "\tif (pW) *pW = t->m_w; if (pH) *pH = t->m_h;\r\n"
     "\tg_szJxXaLyDo = \" (DocLaiAnh - khong mong doi)\";\r\n"
     "\tconst int r = d->ReadbackTexture(t->m_pGpu, t->m_w, t->m_h, (BYTE*)pBuf, uPitch) ? 1 : 0;\r\n"
     "\tg_szJxXaLyDo = NULL;\r\n"
     "\treturn r;\r\n"
     "}\r\n"),
])

# ---------------------------------------------------------------- 2. texture GPU: phien ban moi cua RT + ly do doc lai
RES = "Sources/Represent/Represent3/D3D9onGPURes.cpp"
va(RES, [
    ("\tm_pGpu = p; m_gpuFmt = gf; m_bGpuTarget = bTarget; m_bGpuHasData = false; m_bUsedThisFrame = false; m_bGpuNewer = false;\r\n",
     "\tm_pGpu = p; m_gpuFmt = gf; m_bGpuTarget = bTarget; m_bGpuHasData = false; m_bUsedThisFrame = false; m_bGpuNewer = false;\r\n"
     "#ifdef JX_MOBILE\r\n"
     "\tif (bTarget) RgLog(\"[NENKIEM] render target %ux%u: phien ban GPU moi (noi dung cu neu co da mat)\", m_w, m_h);\t// [NENKIEM 16/09] anh nen vung 512x512 chi nen cap 1 lan\r\n"
     "#endif\r\n"),
    ("\tif (!m_pDev->ReadbackRegion(pSrc, ox, oy, m_w, m_h, gbpp, &tmp[0], m_w * gbpp, JxLop())) { m_bCpuBo = false; return false; }\t// [MANG 11/09] dung lop cua trang\r\n",
     "#ifdef JX_MOBILE\r\n"
     "\t{ extern const char* g_szJxXaLyDo; g_szJxXaLyDo = \" (ThuLaiCpu: doc lai ban CPU da bo)\"; }\t// [NENKIEM 16/09]\r\n"
     "#endif\r\n"
     "\tif (!m_pDev->ReadbackRegion(pSrc, ox, oy, m_w, m_h, gbpp, &tmp[0], m_w * gbpp, JxLop())) { m_bCpuBo = false; return false; }\t// [MANG 11/09] dung lop cua trang\r\n"),
    ("\t{\t// render target da ve tren GPU: doc lai ve CPU (dong bo, hiem)\r\n",
     "\t{\t// render target da ve tren GPU: doc lai ve CPU (dong bo, hiem)\r\n"
     "#ifdef JX_MOBILE\r\n"
     "\t\t{ extern const char* g_szJxXaLyDo; g_szJxXaLyDo = \" (LockRect render target)\"; }\t// [NENKIEM 16/09]\r\n"
     "#endif\r\n"),
])

# ---------------------------------------------------------------- 3. KRepresentShell3: kiem tung o sau ghep
SHELL = "Sources/Represent/Represent3/KRepresentShell3.cpp"
KIEM = (
    "#ifdef JX_MOBILE\r\n"
    "// [NENKIEM 16/09] Chan doan nen den Fold 7 (anh chup 11:09 16/09: lenh ve o san DA GHI ma diem anh khong ra). Sau moi lan ghep anh nen vung\r\n"
    "// (_*PlaceGround*_#~N~#_) doc nguoc render target ve CPU (SAU Present: khong con lenh cho -> khong xa giua khung) va kiem TUNG O 64x64:\r\n"
    "//   '#' co diem anh, '.' rong (khong ghi lenh ve: lo RegionTileDefault), 'X' = DA GHI LENH VE MA DEN (loi phia GPU / lenh), 'o' = co ma khong\r\n"
    "//   ghi lenh ve o (vat phu nen / xuong). Kiem lai dinh ky 4 s cac anh dang hien (DrawPrimitives thay ten) va chi ghi khi ban do doi -> bat\r\n"
    "//   duoc thoi diem mat. Ghi [NENKIEM] vao jx_rep3.log; doi chieu vung (x,y) <-> #N bang [PGND-R] trong jx_paint.log. [Client] Rep3NenKiem=0 tat.\r\n"
    "extern int Rep3Gpu_DocLaiAnh(IDirect3DDevice9* pDev, IDirect3DTexture9* pTex, void* pBuf, unsigned uPitch, unsigned* pW, unsigned* pH);\r\n"
    "struct JxNkMuc { unsigned long long ve; unsigned nVat; unsigned uGhepKhung; unsigned uVeKhung; unsigned nKiem; short nPos; unsigned char bCo; unsigned char cell[64]; };\r\n"
    "static JxNkMuc s_jxNk[64];\r\n"
    "static int s_jxNkCho[64]; static int s_nJxNkCho = 0;\r\n"
    "static unsigned s_uJxNkKhung = 0;\r\n"
    "static DWORD s_dwJxNkLuc = 0; static int s_nJxNkVong = -1;\r\n"
    "static unsigned char* s_pJxNkBuf = NULL;\r\n"
    "static int JxNkSlot(const char* pszImage)\r\n"
    "{\r\n"
    "\tif (!g_nJxNenKiem || !pszImage || strncmp(pszImage, \"_*PlaceGround*_#~\", 17) != 0) return -1;\r\n"
    "\tconst int n = atoi(pszImage + 17);\r\n"
    "\treturn (n >= 0 && n < 64) ? n : -1;\r\n"
    "}\r\n"
    "static void JxNkXoa(const char* pszImage)\r\n"
    "{\r\n"
    "\tconst int n = JxNkSlot(pszImage); if (n < 0) return;\r\n"
    "\tif (!s_jxNk[n].bCo) s_jxNk[n].nPos = IMAGE_IS_POSITION_INIT;\r\n"
    "\ts_jxNk[n].ve = 0; s_jxNk[n].nVat = 0; s_jxNk[n].uGhepKhung = s_uJxNkKhung; s_jxNk[n].nKiem = 0; s_jxNk[n].bCo = 1;\r\n"
    "}\r\n"
    "static void JxNkGhi(int n, int x, int y, bool bO)\r\n"
    "{\r\n"
    "\tif (bO && x >= 0 && y >= 0 && x < 512 && y < 512 && (x & 63) == 0 && (y & 63) == 0) s_jxNk[n].ve |= 1ull << ((y >> 6) * 8 + (x >> 6));\r\n"
    "\telse s_jxNk[n].nVat++;\r\n"
    "}\r\n"
    "static void JxNkXep(int n)\r\n"
    "{\r\n"
    "\tfor (int i = 0; i < s_nJxNkCho; i++) if (s_jxNkCho[i] == n) return;\r\n"
    "\tif (s_nJxNkCho < 64) s_jxNkCho[s_nJxNkCho++] = n;\r\n"
    "}\r\n"
    "static void JxNkDangVe(const char* q)\r\n"
    "{\r\n"
    "\tif (q && q[0] == '_' && q[1] == '*' && q[2] == 'P') { const int n = JxNkSlot(q); if (n >= 0) s_jxNk[n].uVeKhung = s_uJxNkKhung; }\r\n"
    "}\r\n"
    "// doc nguoc + kiem; bSauGhep = lan dau sau ghep (luon ghi), kiem lai thi chi ghi khi ban do doi\r\n"
    "static void JxNkKiem(TextureResMgr& mgr, IDirect3DDevice9* pDev, int n, bool bSauGhep)\r\n"
    "{\r\n"
    "\tJxNkMuc& m = s_jxNk[n];\r\n"
    "\tchar szTen[64]; sprintf(szTen, \"_*PlaceGround*_#~%d~#_\", n);\r\n"
    "\tunsigned int uImage = 0;\r\n"
    "\tTextureResBmp* pBmp = (TextureResBmp*)mgr.GetImage(szTen, uImage, m.nPos, 0, ISI_T_BITMAP16);\r\n"
    "\tif (!pBmp || !pBmp->m_FrameInfo.texInfo[0].pTexture) { Rep3Log(\"[NENKIEM] #%d: khong lay duoc anh (%s)\", n, pBmp ? \"khong co texture\" : \"GetImage NULL\"); return; }\r\n"
    "\tif (!s_pJxNkBuf) s_pJxNkBuf = (unsigned char*)malloc(512 * 512 * 4);\r\n"
    "\tif (!s_pJxNkBuf) return;\r\n"
    "\tunsigned w = 0, h = 0;\r\n"
    "\tLARGE_INTEGER a, b, f; QueryPerformanceFrequency(&f); QueryPerformanceCounter(&a);\r\n"
    "\tif (!Rep3Gpu_DocLaiAnh(pDev, pBmp->m_FrameInfo.texInfo[0].pTexture, s_pJxNkBuf, 512 * 4, &w, &h) || w != 512 || h != 512) { Rep3Log(\"[NENKIEM] #%d: doc nguoc GPU that bai (%ux%u)\", n, w, h); return; }\r\n"
    "\tQueryPerformanceCounter(&b);\r\n"
    "\tunsigned char cell[64]; unsigned nX = 0, nCo = 0, nO = 0, nVe = 0;\r\n"
    "\tfor (int cy = 0; cy < 8; cy++) for (int cx = 0; cx < 8; cx++)\r\n"
    "\t{\r\n"
    "\t\tunsigned nSang = 0;\r\n"
    "\t\tfor (int y = cy * 64 + 2; y < cy * 64 + 64; y += 4) { const unsigned char* p = s_pJxNkBuf + (size_t)y * 512 * 4 + (size_t)cx * 64 * 4 + 8; for (int x = 0; x < 16; x++, p += 16) if ((p[0] | p[1] | p[2]) > 24) nSang++; }\r\n"
    "\t\tconst bool bCo = nSang >= 32, bVe = ((m.ve >> (cy * 8 + cx)) & 1) != 0;\r\n"
    "\t\tcell[cy * 8 + cx] = bCo ? (bVe ? '#' : 'o') : (bVe ? 'X' : '.');\r\n"
    "\t\tif (bCo) nCo++; if (bVe) nVe++; if (bVe && !bCo) nX++; if (bCo && !bVe) nO++;\r\n"
    "\t}\r\n"
    "\tm.nKiem++;\r\n"
    "\tconst bool bDoi = (memcmp(cell, m.cell, 64) != 0);\r\n"
    "\tmemcpy(m.cell, cell, 64);\r\n"
    "\tif (!bSauGhep && !bDoi) return;\r\n"
    "\tchar szBanDo[80]; int k = 0;\r\n"
    "\tfor (int cy = 0; cy < 8; cy++) { memcpy(szBanDo + k, cell + cy * 8, 8); k += 8; szBanDo[k++] = (cy == 7) ? 0 : ' '; }\r\n"
    "\tRep3Log(\"[NENKIEM] #%d %s: ghep o khung %u, kiem lan %u (sau %u khung), doc %.1f ms | ve %u o + %u vat, co %u o, DEN-MA-VE %u, co-ma-khong-ve %u | %s\", n,\r\n"
    "\t\tbSauGhep ? \"SAU GHEP\" : \"DOI\", m.uGhepKhung, m.nKiem, s_uJxNkKhung - m.uGhepKhung, f.QuadPart ? (double)(b.QuadPart - a.QuadPart) * 1000.0 / (double)f.QuadPart : 0.0, nVe, m.nVat, nCo, nX, nO, szBanDo);\r\n"
    "}\r\n"
    "static void JxNkSauKhung(TextureResMgr& mgr, IDirect3DDevice9* pDev)\r\n"
    "{\r\n"
    "\ts_uJxNkKhung++;\r\n"
    "\tif (!g_nJxNenKiem) return;\r\n"
    "\tint nLam = 0;\r\n"
    "\twhile (s_nJxNkCho > 0 && nLam < 2) { const int n = s_jxNkCho[0]; for (int i = 1; i < s_nJxNkCho; i++) s_jxNkCho[i - 1] = s_jxNkCho[i]; s_nJxNkCho--; JxNkKiem(mgr, pDev, n, true); nLam++; }\r\n"
    "\tif (nLam) return;\r\n"
    "\tconst DWORD dwNow = timeGetTime();\r\n"
    "\tif (s_nJxNkVong < 0) { if (dwNow - s_dwJxNkLuc >= 4000) { s_dwJxNkLuc = dwNow; s_nJxNkVong = 0; } else return; }\r\n"
    "\twhile (s_nJxNkVong < 64)\r\n"
    "\t{\r\n"
    "\t\tconst int n = s_nJxNkVong++;\r\n"
    "\t\tif (s_jxNk[n].bCo && s_jxNk[n].nKiem > 0 && s_uJxNkKhung - s_jxNk[n].uVeKhung <= 2) { JxNkKiem(mgr, pDev, n, false); return; }\r\n"
    "\t}\r\n"
    "\ts_nJxNkVong = -1;\r\n"
    "}\r\n"
    "#endif\r\n"
)
va(SHELL, [
    # cong tac
    ("int g_nJxONenLog = 0;\t// [ONEN 15/09] [Client] Rep3ONenLog = so lan ghep nen con phai ghi chi tiet (tu giam ve 0)\r\n",
     "int g_nJxONenLog = 0;\t// [ONEN 15/09] [Client] Rep3ONenLog = so lan ghep nen con phai ghi chi tiet (tu giam ve 0)\r\n"
     "int g_nJxNenKiem = 1;\t// [NENKIEM 16/09] [Client] Rep3NenKiem: 1 = sau moi lan ghep anh nen vung doc nguoc GPU kiem tung o + kiem lai dinh ky (chan doan nen den Fold 7); 0 = tat\r\n"),
    ("\t{ extern int g_nJxONenLog; g_nJxONenLog = Rep3Ini(\"Rep3ONenLog\", 0); if (g_nJxONenLog < 0) g_nJxONenLog = 0; if (g_nJxONenLog > 64) g_nJxONenLog = 64; }\t// [ONEN 15/09]\r\n",
     "\t{ extern int g_nJxONenLog; g_nJxONenLog = Rep3Ini(\"Rep3ONenLog\", 0); if (g_nJxONenLog < 0) g_nJxONenLog = 0; if (g_nJxONenLog > 64) g_nJxONenLog = 64; }\t// [ONEN 15/09]\r\n"
     "\t{ extern int g_nJxNenKiem; g_nJxNenKiem = Rep3Ini(\"Rep3NenKiem\", 1) ? 1 : 0; Rep3Log(\"[NENKIEM] kiem tung o anh nen vung sau ghep: %d (Rep3NenKiem)\", g_nJxNenKiem); }\t// [NENKIEM 16/09]\r\n"),
    # ham kiem, dat truoc DrawPrimitives
    ("void KRepresentShell3::DrawPrimitives(int nPrimitiveCount, KRepresentUnit* pPrimitives, unsigned int uGenre, int bSinglePlaneCoord)\r\n{\r\n",
     KIEM +
     "void KRepresentShell3::DrawPrimitives(int nPrimitiveCount, KRepresentUnit* pPrimitives, unsigned int uGenre, int bSinglePlaneCoord)\r\n{\r\n"),
    # anh nen vung dang hien
    ("\tint bLopChu = 0; if (bSinglePlaneCoord == 2) { bLopChu = 1; bSinglePlaneCoord = 0; }\t// [CHUNET 14/09 d] KNpc::PaintInfo (mobile): icon canh ten = toa do the gioi + lop thong tin\r\n",
     "\tint bLopChu = 0; if (bSinglePlaneCoord == 2) { bLopChu = 1; bSinglePlaneCoord = 0; }\t// [CHUNET 14/09 d] KNpc::PaintInfo (mobile): icon canh ten = toa do the gioi + lop thong tin\r\n"
     "\tif (g_nJxNenKiem && uGenre == RU_T_IMAGE && nPrimitiveCount == 1) JxNkDangVe(((KRUImage*)pPrimitives)->szImage);\t// [NENKIEM 16/09] anh nen vung dang hien -> kiem lai dinh ky\r\n"),
    # ClearImageData: bat dau lan ghep moi
    ("\t\tif (strncmp(pszImage, \"_*PlaceGround*_\", 15) == 0 && pBitmap->m_FrameInfo.texInfo[0].pTexture)\r\n\t\t{\r\n",
     "\t\tif (strncmp(pszImage, \"_*PlaceGround*_\", 15) == 0 && pBitmap->m_FrameInfo.texInfo[0].pTexture)\r\n\t\t{\r\n"
     "\t\t\tJxNkXoa(pszImage);\t// [NENKIEM 16/09] bat dau mot lan ghep moi: xoa mat na o da ghi lenh ve\r\n"),
    # DrawPrimitivesOnImage: slot dich
    ("\tif (bJxONen) { g_nJxONenLog--; Rep3Log(\"[ONEN] === bat dau ghep %s: %d anh ===\", pszImage, nPrimitiveCount); }\r\n",
     "\tif (bJxONen) { g_nJxONenLog--; Rep3Log(\"[ONEN] === bat dau ghep %s: %d anh ===\", pszImage, nPrimitiveCount); }\r\n"
     "\tconst int nJxNk = (uGenre == RU_T_IMAGE) ? JxNkSlot(pszImage) : -1;\t// [NENKIEM 16/09]\r\n"),
    # o da ghi lenh ve
    ("\t\t\t\t\t\t\tconst unsigned uJxTN0 = g_uRep3FxTexNull;\r\n\t\t\t\t\t\t\tRIO_CopySprToBufferAlpha(pSprite, pTemp->nFrame, pDestBitmap, nX, nY);\r\n",
     "\t\t\t\t\t\t\tconst unsigned uJxTN0 = g_uRep3FxTexNull;\r\n\t\t\t\t\t\t\tRIO_CopySprToBufferAlpha(pSprite, pTemp->nFrame, pDestBitmap, nX, nY);\r\n"
     "\t\t\t\t\t\t\tif (nJxNk >= 0 && g_uRep3FxTexNull == uJxTN0) JxNkGhi(nJxNk, pTemp->oPosition.nX, pTemp->oPosition.nY, pTemp->bRenderStyle == IMAGE_RENDER_STYLE_OPACITY);\t// [NENKIEM 16/09] o da ghi lenh ve\r\n"),
    # cuoi DrawPrimitivesOnImage: xep kiem
    ("\t\t\t\ts_dJxNenGetMs, s_uJxNenGetLan, s_uJxNenNapLan, s_uJxNenBoLan, s_dJxNenRioMs, s_dJxNenRtMs);\r\n\t}\r\n#endif\r\n}\r\n",
     "\t\t\t\ts_dJxNenGetMs, s_uJxNenGetLan, s_uJxNenNapLan, s_uJxNenBoLan, s_dJxNenRioMs, s_dJxNenRtMs);\r\n\t}\r\n"
     "\tif (nJxNk >= 0) JxNkXep(nJxNk);\t// [NENKIEM 16/09] kiem sau Present\r\n"
     "#endif\r\n}\r\n"),
    # sau Present
    ("\t{ LARGE_INTEGER liJx0, liJx1; QueryPerformanceCounter(&liJx0); PD3DDEVICE->Present(NULL,NULL,NULL,NULL); QueryPerformanceCounter(&liJx1); JxVeGiatGhi(Rep3NapMs(liJx0, liJx1)); JxTheGioiCapNhat(Rep3NapMs(liJx0, liJx1)); }\t// [VE 11/09] [TG 13/09]\r\n",
     "\t{ LARGE_INTEGER liJx0, liJx1; QueryPerformanceCounter(&liJx0); PD3DDEVICE->Present(NULL,NULL,NULL,NULL); QueryPerformanceCounter(&liJx1); JxVeGiatGhi(Rep3NapMs(liJx0, liJx1)); JxTheGioiCapNhat(Rep3NapMs(liJx0, liJx1)); }\t// [VE 11/09] [TG 13/09]\r\n"
     "\tJxNkSauKhung(m_TextureResMgr, PD3DDEVICE);\t// [NENKIEM 16/09] doc nguoc + kiem anh nen vung vua ghep (toi da 2/khung) hoac kiem lai dinh ky\r\n"),
])

# ---------------------------------------------------------------- 4. Core: [PGND-R] moi lan ghep
va("Sources/Core/Src/Scene/KScenePlaceRegionC.cpp", [
    ("\t\tif (g_nCorePaintLog > 0 && dTong >= 3.0)\r\n",
     "\t\tif (g_nCorePaintLog > 0)\t// [NENKIEM 16/09] ghi MOI lan ghep (doi chieu vung (x,y) <-> anh #N voi [NENKIEM] trong jx_rep3.log); truoc: chi khi >= 3 ms\r\n"),
])
print("xong %s" % TAG)
