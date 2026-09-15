# -*- coding: latin-1 -*-
r"""[PALDO 15/09] Gan do BANG MAU de MOT lan chay goi ten thu pham cua cac o nen den.

TINH TRANG (da chung minh, xem BANGIAO_MOBILE_TOIUU_1309.md):
  - Man hinh co cac HINH CHU NHAT DEN trong long ban do; do bang diem anh: canh nam DUNG tren luoi
    o nen (o nen = anh 64x64), mau doc theo hang la DEN DEN SANG DEN DEN SANG - tuc XEN KE THEO O.
  - Du lieu ban do cho cac o do la NEN THAT (mep hang trong du lieu la duong cheo gon). Quet ca
    14 336 o cua map: mau xen ke do KHONG xuat hien -> KHONG phai du lieu.
  - Bo dem "bo o" khop dung so o mac dinh cua vung -> cac o con lai DA DUOC VE. Tuc mot anh o nen
    DA VE RA nhung RA MAU DEN.
  - Da loai tru bang do dac / bang ma: atlas KHOI, duong ve the gioi qua RT, muc zoom, nhanh to so 0
    o atlas (Rep3GpuBoBanCpu=0 van den), moi duong nop lenh giua khung, duong tai qua anh dem.
  - Xen ke THEO O = hong THEO ANH, khong theo vung. Hang bang mau thuoc ve TEP .spr (m_nPalRow la
    thanh vien cua TextureResSpr) nen mot hang hong = anh do den o MOI cho no duoc dung.
  - Shader: hang toan 0 -> unpackUnorm4x8(0) = (0,0,0,0) -> tra vec4(0,0,0, alpha) = DEN DAC, dung hinh.

BAN NAY THEM (tat ca rao #ifdef JX_MOBILE, ban PC bien dich y het):
  1. So sach theo hang: hang nao cap roi ma CHUA BAO GIO tai len GPU, hang nao TOAN DEN.
  2. Ghi ngay khi cap mot hang toan den (bat loi du lieu .spr trong pak).
  3. Ghi khi ca lo staging cua mot khung bi bo (map transfer that bai) - luc do TAT CA hang bang mau
     cua khung do mat vinh vien vi m_palPending da clear truoc do va khong ai cap lai.
  4. Tong ket moi 1800 khung: hang song / cap ma chua tai / toan den / so khung mat lo.
  5. [Client] Rep3PalDo=1: TO MOI HANG BANG MAU SAC RO theo chi so hang (thi nghiem phan dinh).
       - o nen den HOA SAC SO  -> diem anh CO di qua bang mau -> loi o NOI DUNG hang -> doc [PALDO]
       - o nen VAN DEN         -> diem anh khong toi bang mau -> bang mau VO CAN, tim huong khac
     Mot lan chay tra loi ca hai nhanh.

Chay:  PYTHONIOENCODING=latin-1 python android\va_nguon_mobile_1509_paldo.py
"""
import io
import os

GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TAG = "[PALDO 15/09]"


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
            raise SystemExit("HONG: neo xuat hien %d lan trong %s:\n%s" % (s.count(cu), duong, cu[:160]))
        s = s.replace(cu, moi, 1)
    cao1 = sum(1 for c in s if ord(c) >= 128)
    lf1 = s.count("\n") - s.count("\r\n")
    if cao1 != cao0:
        raise SystemExit("HONG: %s byte cao %d -> %d" % (duong, cao0, cao1))
    if lf1 != lf0:
        raise SystemExit("HONG: %s co %d dong LF don (truoc %d)" % (duong, lf1, lf0))
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("  da va: %s" % duong)


print("[PALDO 15/09] gan do bang mau")

va(r"Sources\Represent\Represent3\D3D9onGPUDev.cpp", [
    # --- 1. so sach + cong tac
    ("#define RG_PAL_ROWS 8192\r\n",
     "#define RG_PAL_ROWS 8192\r\n"
     "#ifdef JX_MOBILE\r\n"
     "// [PALDO 15/09] so sach hang bang mau: bat hai ca lam mot anh DA VE ma RA DEN.\r\n"
     "//   (a) hang cap roi ma lo staging cua khung do bi bo -> hang o lai TOAN 0 vinh vien (m_palPending\r\n"
     "//       da clear truoc khi biet map co thanh cong khong, va m_nPalRow >= 0 nen khong ai cap lai).\r\n"
     "//   (b) hang duoc cap tu mot bang mau .spr toan den (du lieu pak hong).\r\n"
     "static unsigned char s_ucJxPalDaTai[RG_PAL_ROWS];\t// 1 = da tai len GPU it nhat mot lan\r\n"
     "static unsigned char s_ucJxPalDen[RG_PAL_ROWS];\t\t// 1 = hang toan mau den\r\n"
     "static unsigned s_uJxPalMatKhung = 0, s_uJxPalMatHang = 0;\r\n"
     "int g_nJxPalDo = 0;\t// [Client] Rep3PalDo=1: to moi hang bang mau bang mau sac ro theo chi so hang (thi nghiem phan dinh)\r\n"
     "#endif\r\n"),
    # --- 2. PalAlloc: ghi so sach, bao hang toan den, va cong tac to mau
    ("\tm_palPending.push_back(std::make_pair(row, rgb));\r\n",
     "#ifdef JX_MOBILE\r\n"
     "\t{\t// [PALDO 15/09]\r\n"
     "\t\tunsigned uJxKhac = 0;\r\n"
     "\t\tfor (int i = 0; i < nColors; i++) if (rgb[i] & 0x00FFFFFFu) uJxKhac++;\r\n"
     "\t\tif (row >= 0 && row < RG_PAL_ROWS) { s_ucJxPalDaTai[row] = 0; s_ucJxPalDen[row] = (uJxKhac == 0) ? 1 : 0; }\r\n"
     "\t\tif (uJxKhac == 0) RgLog(\"[PALDO] hang %d: bang mau TOAN DEN (%d mau) - moi anh dung hang nay se ve ra DEN\", row, nColors);\r\n"
     "\t\tif (g_nJxPalDo)\t// thi nghiem phan dinh: hang nao cung thanh mot mau sac ro rieng\r\n"
     "\t\t\tfor (int i = 0; i < 256; i++) rgb[i] = 0xFF000000u | ((DWORD)((row * 53) & 0xFF) << 16) | ((DWORD)((row * 97) & 0xFF) << 8) | (DWORD)((row * 29) & 0xFF);\r\n"
     "\t}\r\n"
     "#endif\r\n"
     "\tm_palPending.push_back(std::make_pair(row, rgb));\r\n"),
    # --- 3. danh dau hang da len GPU
    ("\t\t\t\t\t\tSDL_UploadToGPUBuffer(cp, &src, &dst, false);\r\n"
     "\t\t\t\t\t}\r\n"
     "\t\t\t\t\tm_uUploads += (unsigned)m_jxPalUploads.size();\r\n",
     "\t\t\t\t\t\tSDL_UploadToGPUBuffer(cp, &src, &dst, false);\r\n"
     "\t\t\t\t\t\t{ const int jxR = (int)m_jxPalUploads[i].first; if (jxR >= 0 && jxR < RG_PAL_ROWS) s_ucJxPalDaTai[jxR] = 1; }\t// [PALDO 15/09]\r\n"
     "\t\t\t\t\t}\r\n"
     "\t\t\t\t\tm_uUploads += (unsigned)m_jxPalUploads.size();\r\n"),
    # --- 4. mat ca lo staging cua mot khung
    ("\t\t\telse RgLog(\"map transfer texture (%u B) that bai: %s\", (unsigned)m_texStage.size(), SDL_GetError());\r\n",
     "\t\t\telse RgLog(\"map transfer texture (%u B) that bai: %s\", (unsigned)m_texStage.size(), SDL_GetError());\r\n"
     "#ifdef JX_MOBILE\r\n"
     "\t\t\tif (!p && !m_jxPalUploads.empty())\r\n"
     "\t\t\t{\t// [PALDO 15/09] ca lo cua khung bi bo: cac hang bang mau nay KHONG bao gio duoc cap lai\r\n"
     "\t\t\t\ts_uJxPalMatKhung++; s_uJxPalMatHang += (unsigned)m_jxPalUploads.size();\r\n"
     "\t\t\t\tRgLog(\"[PALDO] MAT CA LO KHUNG %u: %u hang bang mau + %u lenh tai texture bi bo -> cac anh do se ve ra DEN vinh vien\",\r\n"
     "\t\t\t\t\tm_uFrames, (unsigned)m_jxPalUploads.size(), (unsigned)m_texUploads.size());\r\n"
     "\t\t\t}\r\n"
     "#endif\r\n"),
    # --- 5. tong ket moi 1800 khung
    ("\ts_uRep3GpuLenhVe = m_uDrawCmds; s_uRep3GpuQuad = m_uQuads;\t// [ANDROID 11/09 HUD]\r\n",
     "\tif (m_uFrames == 1 || (m_uFrames % 1800) == 0)\r\n"
     "\t{\t// [PALDO 15/09] hang cap roi ma chua tai len GPU = anh se ve ra DEN\r\n"
     "\t\tunsigned uJxCho = 0, uJxDen = 0;\r\n"
     "\t\tfor (int i = 0; i < RG_PAL_ROWS; i++) { if (s_ucJxPalDen[i]) uJxDen++; }\r\n"
     "\t\tfor (unsigned i = 0; i < g_uRep3PalRows && i < (unsigned)RG_PAL_ROWS; i++) if (!s_ucJxPalDaTai[i]) uJxCho++;\r\n"
     "\t\tRgLog(\"[PALDO] khung %u: hang bang mau da cap %u | CAP MA CHUA TAI LEN GPU %u | hang TOAN DEN %u | khung mat ca lo %u (mat %u hang) | to mau thu %d\",\r\n"
     "\t\t\tm_uFrames, g_uRep3PalRows, uJxCho, uJxDen, s_uJxPalMatKhung, s_uJxPalMatHang, g_nJxPalDo);\r\n"
     "\t}\r\n"
     "\ts_uRep3GpuLenhVe = m_uDrawCmds; s_uRep3GpuQuad = m_uQuads;\t// [ANDROID 11/09 HUD]\r\n"),
])

va(r"Sources\Represent\Represent3\KRepresentShell3.cpp", [
    ("\tg_nJxPsBuffer       = Rep3Ini(\"Rep3PsBuffer\", 1) ? 1 : 0;",
     "#ifdef JX_MOBILE\r\n"
     "\t{ extern int g_nJxPalDo; g_nJxPalDo = Rep3Ini(\"Rep3PalDo\", 0) ? 1 : 0; }\t// [PALDO 15/09] 1 = to hang bang mau sac ro theo chi so (thi nghiem phan dinh den)\r\n"
     "#endif\r\n"
     "\tg_nJxPsBuffer       = Rep3Ini(\"Rep3PsBuffer\", 1) ? 1 : 0;"),
])

print("xong.")
