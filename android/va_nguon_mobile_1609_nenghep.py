# -*- coding: latin-1 -*-
r"""[NENGHEP 16/09] Nen den Fold 7 - SUA TAN GOC o lop SDL_GPU: khung co ghep anh nen vung thi chep len GPU bang command buffer rieng, cho xong roi moi ve.

BANG CHUNG (ban chan doan [NENKIEM 16/09], phien Fold 7 12:50, khung 348 vao map Tien Coc Dong):
  Doc nguoc render target ngay sau ghep: anh #5 = vung (97,97) ghi lenh ve 50 o, chi 19 o co diem anh (31 o 'X'), anh #6 = vung
  (98,97) 53/53 o 'X'; #4 ngay truoc va moi anh sau deu du. Cung mot sprite/khung co o nay mat o kia (红土_红卵:9 co o (5,6), mat o
  (3,3)) -> KHONG phai texture. Cac o mat la cac DAI LIEN TIEP theo thu tu ghi lenh (quad 18-31 va 43-114 cua khung) trong MOT lenh
  ve gop (94 lenh / 31 pass khop 11 luot ghep) -> GPU doc dinh trong ring khi du lieu chua toi (lenh chep ring nam CUOI copy pass, sau
  15,7 MB texture tai cung khung; pass ghep la pass DAU tien sau copy pass). Chi xay ra o khung tai nhieu (vao map); giai lap / PC
  khong bi vi khong co engine chep bat dong bo. Rao chan cua SDL (TRANSFER -> VERTEX_INPUT, SDL_gpu_vulkan.c) dung chuan -> hanh vi
  driver Adreno; ta khong sua duoc driver nen tach: khung co ghep -> copy pass vao command buffer RIENG, nop + cho fence xong
  (Rep3GhepTach=2, mac dinh) roi moi ghi pass ve vao command buffer thu hai. Khung khong ghep (99 % so khung) GIU NGUYEN mot command
  buffer nhu cu -> khong mat toi uu. Rep3GhepTach=1: chi nop truoc khong cho (thu neu 2 gay khung); 0 = tat.

Chay:  PYTHONIOENCODING=latin-1 python android\va_nguon_mobile_1609_nenghep.py   (sau va_nguon_mobile_1609_nenkiem.py)
"""
import io
import os

GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TAG = "[NENGHEP 16/09]"


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


DEV = "Sources/Represent/Represent3/D3D9onGPUDev.cpp"
va(DEV, [
    # bien
    ("const char* g_szJxXaLyDo = NULL; unsigned g_uJxXaGiua = 0;",
     "int g_nJxGhepTach = 2; static bool s_bJxKhungCoGhep = false; unsigned g_uJxGhepTachSo = 0; double g_dJxGhepTachMs = 0.0;\t// [NENGHEP 16/09] [Client] Rep3GhepTach: 2 = khung co ghep anh nen vung -> copy pass vao command buffer rieng, nop + CHO fence; 1 = chi nop truoc; 0 = tat\r\n"
     "const char* g_szJxXaLyDo = NULL; unsigned g_uJxXaGiua = 0;"),
    # dau SubmitFrame: command buffer chep rieng
    ("\tSDL_GPUTexture* pSwap = NULL; Uint32 swW = 0, swH = 0;\r\n",
     "\tSDL_GPUTexture* pSwap = NULL; Uint32 swW = 0, swH = 0;\r\n"
     "#ifdef JX_MOBILE\r\n"
     "\tSDL_GPUCommandBuffer* cbJxChep = cb; bool bJxTach = false;\r\n"
     "\tif (bPresent && s_bJxKhungCoGhep && g_nJxGhepTach > 0)\r\n"
     "\t{\t// [NENGHEP 16/09] khung nay ghep anh nen vung (render target giu vinh vien) trong khi cung khung tai texture/ring len GPU: tren Fold 7 (Adreno)\r\n"
     "\t\t// lenh ve doc dinh trong ring truoc khi lenh chep toi (doc nguoc [NENKIEM] 16/09: 31/50 o vung (97,97) va 53/53 o vung (98,97) ghi lenh VE ma trong,\r\n"
     "\t\t// mat theo dai lien tiep trong MOT lenh ve gop). Rao chan SDL dung chuan -> tach: copy pass vao command buffer RIENG, nop truoc (=2: cho fence\r\n"
     "\t\t// xong) roi moi ghi pass ve. Khung khong ghep giu nguyen mot command buffer.\r\n"
     "\t\tcbJxChep = SDL_AcquireGPUCommandBuffer(m_pGpu);\r\n"
     "\t\tif (cbJxChep) bJxTach = true; else { cbJxChep = cb; RgLog(\"[NENGHEP] AcquireGPUCommandBuffer (chep) that bai: %s -> dung chung\", SDL_GetError()); }\r\n"
     "\t}\r\n"
     "#endif\r\n"),
    # copy pass vao command buffer chep
    ("\t// ---- copy pass: bang mau, texture, ring dinh\r\n\t{\r\n\t\tSDL_GPUCopyPass* cp = SDL_BeginGPUCopyPass(cb);\r\n",
     "\t// ---- copy pass: bang mau, texture, ring dinh\r\n\t{\r\n"
     "#ifdef JX_MOBILE\r\n"
     "\t\tSDL_GPUCopyPass* cp = SDL_BeginGPUCopyPass(cbJxChep);\t// [NENGHEP 16/09] khung co ghep: command buffer chep rieng\r\n"
     "#else\r\n"
     "\t\tSDL_GPUCopyPass* cp = SDL_BeginGPUCopyPass(cb);\r\n"
     "#endif\r\n"),
    # sau copy pass: nop (+ cho) command buffer chep
    ("\t\tSDL_EndGPUCopyPass(cp);\r\n\t}\r\n#ifdef JX_MOBILE\r\n\t{ const Uint64 u = SDL_GetPerformanceCounter(); jxK.dChep = JxVeMs(uJxK1, u); uJxK1 = u; }\t// [VE 11/09] chep len GPU\r\n#endif\r\n",
     "\t\tSDL_EndGPUCopyPass(cp);\r\n\t}\r\n"
     "#ifdef JX_MOBILE\r\n"
     "\tif (bJxTach)\r\n"
     "\t{\t// [NENGHEP 16/09] nop command buffer chep TRUOC; =2: cho GPU chep xong (fence) roi moi ghi pass ve -> lenh ve khong bao gio thay ring / texture chua toi\r\n"
     "\t\tconst Uint64 uJxTach0 = SDL_GetPerformanceCounter();\r\n"
     "\t\tif (g_nJxGhepTach >= 2)\r\n"
     "\t\t{\r\n"
     "\t\t\tSDL_GPUFence* f = SDL_SubmitGPUCommandBufferAndAcquireFence(cbJxChep);\r\n"
     "\t\t\tif (f) { SDL_WaitForGPUFences(m_pGpu, true, &f, 1); SDL_ReleaseGPUFence(m_pGpu, f); }\r\n"
     "\t\t\telse RgLog(\"[NENGHEP] nop command buffer chep (fence) that bai: %s\", SDL_GetError());\r\n"
     "\t\t}\r\n"
     "\t\telse if (!SDL_SubmitGPUCommandBuffer(cbJxChep)) RgLog(\"[NENGHEP] nop command buffer chep that bai: %s\", SDL_GetError());\r\n"
     "\t\tconst double dJxTach = JxVeMs(uJxTach0, SDL_GetPerformanceCounter());\r\n"
     "\t\tg_uJxGhepTachSo++; g_dJxGhepTachMs += dJxTach;\r\n"
     "\t\tif (g_uJxGhepTachSo <= 40 || (g_uJxGhepTachSo % 50) == 0) RgLog(\"[NENGHEP] khung co ghep #%u: chep len GPU bang command buffer rieng (%u tex %u KB, ring %u KB), %s %.1f ms\", g_uJxGhepTachSo, jxK.uTai, jxK.uTaiKB, jxK.uRingKB, g_nJxGhepTach >= 2 ? \"cho xong\" : \"nop truoc\", dJxTach);\r\n"
     "\t}\r\n"
     "\t{ const Uint64 u = SDL_GetPerformanceCounter(); jxK.dChep = JxVeMs(uJxK1, u); uJxK1 = u; }\t// [VE 11/09] chep len GPU\r\n"
     "#endif\r\n"),
    # FrameReset: het khung
    ("\tm_jxZeroCopy.clear();\t// [TAI 14/09]\r\n",
     "\tm_jxZeroCopy.clear();\t// [TAI 14/09]\r\n"
     "\ts_bJxKhungCoGhep = false;\t// [NENGHEP 16/09]\r\n"),
    # danh dau tu KRepresentShell3
    ("\tg_szJxXaLyDo = NULL;\r\n\treturn r;\r\n}\r\n",
     "\tg_szJxXaLyDo = NULL;\r\n\treturn r;\r\n}\r\n"
     "// [NENGHEP 16/09] KRepresentShell3::DrawPrimitivesOnImage bao: khung nay ghep anh nen vung -> SubmitFrame tach copy pass ra command buffer rieng\r\n"
     "void Rep3Gpu_DanhDauGhep(IDirect3DDevice9* pDev)\r\n"
     "{\r\n"
     "\tif (pDev && g_nRep3ApiOn == 100) s_bJxKhungCoGhep = true;\r\n"
     "}\r\n"),
])

SHELL = "Sources/Represent/Represent3/KRepresentShell3.cpp"
va(SHELL, [
    # mac dinh kiem o = TAT (chan doan; dt_v4 bat bang Rep3NenKiem=1 khi can)
    ("int g_nJxNenKiem = 1;\t// [NENKIEM 16/09]",
     "int g_nJxNenKiem = 0;\t// [NENGHEP 16/09] mac dinh TAT (doc nguoc 2-14 ms moi lan ghep) - bat bang Rep3NenKiem=1 khi can chan doan. [NENKIEM 16/09]"),
    ("\t{ extern int g_nJxNenKiem; g_nJxNenKiem = Rep3Ini(\"Rep3NenKiem\", 1) ? 1 : 0; Rep3Log(\"[NENKIEM] kiem tung o anh nen vung sau ghep: %d (Rep3NenKiem)\", g_nJxNenKiem); }\t// [NENKIEM 16/09]\r\n",
     "\t{ extern int g_nJxNenKiem; g_nJxNenKiem = Rep3Ini(\"Rep3NenKiem\", 0) ? 1 : 0; Rep3Log(\"[NENKIEM] kiem tung o anh nen vung sau ghep: %d (Rep3NenKiem)\", g_nJxNenKiem); }\t// [NENKIEM 16/09] [NENGHEP 16/09] mac dinh 0\r\n"
     "\t{ extern int g_nJxGhepTach; g_nJxGhepTach = Rep3Ini(\"Rep3GhepTach\", 2); if (g_nJxGhepTach < 0) g_nJxGhepTach = 0; if (g_nJxGhepTach > 2) g_nJxGhepTach = 2; Rep3Log(\"[NENGHEP] khung co ghep anh nen vung: copy pass tach command buffer rieng = %d (Rep3GhepTach: 2 cho fence, 1 nop truoc, 0 tat)\", g_nJxGhepTach); }\t// [NENGHEP 16/09]\r\n"),
    # DrawPrimitivesOnImage: bao khung co ghep
    ("\tconst int nJxNk = (uGenre == RU_T_IMAGE) ? JxNkSlot(pszImage) : -1;\t// [NENKIEM 16/09]\r\n",
     "\tconst int nJxNk = (uGenre == RU_T_IMAGE) ? JxNkSlot(pszImage) : -1;\t// [NENKIEM 16/09]\r\n"
     "\tif (uGenre == RU_T_IMAGE && pszImage && strncmp(pszImage, \"_*PlaceGround*_\", 15) == 0) { extern void Rep3Gpu_DanhDauGhep(IDirect3DDevice9*); Rep3Gpu_DanhDauGhep(PD3DDEVICE); }\t// [NENGHEP 16/09] khung nay ghep anh nen vung\r\n"),
])
print("xong %s" % TAG)
