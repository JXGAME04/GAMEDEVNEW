# -*- coding: utf-8 -*-
#
# [ANDROID 09/09 HAINGON] Vua di chuyen VUA danh - hai ngon cung luc.
#
# Chu: "phai lam them phan vua dung duoc nut di chuyen vua dung duoc cac nut ky nang mot
# lan, khi do moi di chuyen danh duoc".
#
# Vi sao truoc day khong duoc: SDL chi GIA LAP CHUOT cho ngon THU NHAT (da ghi o dong 580).
# Ca bo nhan cham cua ban nay di theo duong chuot gia lap do, va chi co MOT trang thai
# m_nCham - nen dat ngon thu hai xuong la khong co su kien nao ca. Giu can dieu khien
# thi khong bam duoc nut ky nang, va nguoc lai.
#
# Sua: mo them mot duong RIENG di thang tu su kien NGON TAY that (SDL_EVENT_FINGER_*),
# danh cho ngon THU HAI tro di:
#   - dat ngon thu hai trung mot nut ky nang -> ngon do dieu khien nut ky nang;
#   - dat ngon thu hai vao vung can (khi can chua ai cam) -> ngon do cam can.
# Ngon thu nhat van di duong cu, khong doi gi - nen moi thao tac mot ngon giu nguyen.
#
# Nho vay: mot ngon giu can de chay, ngon kia de nut ky nang de danh - dung nhu chu can.

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
NL = "\r\n"
DAU = "[ANDROID 09/09 HAINGON]"

# ================================================================ 1. KSdlApp.h
P = "Sources/S3Client/Platform/KSdlApp.h"
s = io.open(P, encoding="latin-1", newline="").read()
if "m_nNgonKyNang" in s:
    print("da va roi, bo qua:", P)
else:
    CU = ("\tint\t\t\t\tm_nNgonToiDa;\t\t"
          "// [ANDROID 09/09 HAINGON] nhieu nhat may ngon trong lan cham nay")
    assert s.count(CU) == 1, "khong tim thay m_nNgonToiDa (%d)" % s.count(CU)
    MOI = NL.join([
        CU,
        "\t// [ANDROID 09/09 HAINGON] Ngon THU HAI tro di di duong rieng, khong qua chuot gia",
        "\t// lap (SDL chi gia lap chuot cho ngon thu nhat). -1 = chua ngon nao giu.",
        "\tlong long\tm_nNgonKyNang;\t// ngon dang giu mot nut ky nang",
        "\tlong long\tm_nNgonCan;\t\t// ngon dang cam can dieu khien",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)

# ================================================================ 2. KSdlApp.cpp
P = "Sources/S3Client/Platform/KSdlApp.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU + " duong" in s:
    print("da va roi, bo qua:", P)
else:
    # khoi tao
    CU = NL.join([
        "\tm_nNgonDangDat = 0;",
        "\tm_nNgonToiDa = 0;",
    ])
    assert s.count(CU) == 1, "khong tim thay cho khoi tao dem ngon (%d)" % s.count(CU)
    MOI = NL.join([
        CU,
        "\tm_nNgonKyNang = -1;\t// [ANDROID 09/09 HAINGON]",
        "\tm_nNgonCan = -1;",
    ])
    s = s.replace(CU, MOI)

    # duong rieng cho ngon thu hai
    CU = NL.join([
        "\tif (ev.type == SDL_EVENT_FINGER_DOWN)",
        "\t{",
        "\t\tm_nNgonDangDat++;",
        "\t\tif (m_nNgonDangDat > m_nNgonToiDa)",
        "\t\t\tm_nNgonToiDa = m_nNgonDangDat;",
        "\t\treturn false;\t// van de su kien chuot gia lap di duong cua no",
        "\t}",
        "\tif (ev.type == SDL_EVENT_FINGER_UP || ev.type == SDL_EVENT_FINGER_CANCELED)",
        "\t{",
        "\t\tif (m_nNgonDangDat > 0)",
        "\t\t\tm_nNgonDangDat--;",
        "\t\treturn false;",
        "\t}",
    ])
    assert s.count(CU) == 1, "khong tim thay khoi su kien ngon tay (%d)" % s.count(CU)
    MOI = NL.join([
        "\t// [ANDROID 09/09 HAINGON] duong RIENG cho ngon THU HAI tro di: SDL chi gia lap",
        "\t// chuot cho ngon thu nhat, nen muon vua giu can vua bam nut ky nang thi phai doc",
        "\t// thang su kien ngon tay. Ngon thu nhat van di duong cu, khong doi gi.",
        "\tif (ev.type == SDL_EVENT_FINGER_DOWN || ev.type == SDL_EVENT_FINGER_MOTION",
        "\t\t|| ev.type == SDL_EVENT_FINGER_UP || ev.type == SDL_EVENT_FINGER_CANCELED)",
        "\t{",
        "\t\tlong long nNgon = (long long)ev.tfinger.fingerID;",
        "\t\tint nW = 0, nH = 0;",
        "\t\tfloat fx, fy;",
        "",
        "\t\tSDL_GetWindowSize(m_pWindow, &nW, &nH);",
        "\t\tfx = ev.tfinger.x * (float)nW;",
        "\t\tfy = ev.tfinger.y * (float)nH;",
        "\t\tSdlToLogical(m_pWindow, fx, fy);",
        "",
        "\t\tif (ev.type == SDL_EVENT_FINGER_DOWN)",
        "\t\t{",
        "\t\t\tbool bNgonDau = (m_nNgonDangDat == 0);",
        "",
        "\t\t\tm_nNgonDangDat++;",
        "\t\t\tif (m_nNgonDangDat > m_nNgonToiDa)",
        "\t\t\t\tm_nNgonToiDa = m_nNgonDangDat;",
        "\t\t\tif (bNgonDau)",
        "\t\t\t\treturn false;\t// ngon thu nhat: de chuot gia lap lo nhu cu",
        "",
        "\t\t\t// Ngon thu hai tro di: uu tien nut ky nang, roi den can dieu khien.",
        "\t\t\tif (m_nNgonKyNang < 0)",
        "\t\t\t{",
        "\t\t\t\tint nNut = JxKyNang_TrungNut((int)fx, (int)fy);",
        "\t\t\t\tif (nNut > 0)",
        "\t\t\t\t{",
        "\t\t\t\t\tm_nNgonKyNang = nNgon;",
        "\t\t\t\t\tJxKyNang_BatDau(nNut, (int)fx, (int)fy);",
        "\t\t\t\t\treturn true;",
        "\t\t\t\t}",
        "\t\t\t}",
        "\t\t\tif (m_nNgonCan < 0 && !JxCan_DangCam()",
        "\t\t\t\t&& JxCan_TrongVung((int)fx, (int)fy)",
        "\t\t\t\t&& !JxUi_CoGiaoDienTaiDiem((int)fx, (int)fy))",
        "\t\t\t{",
        "\t\t\t\tm_nNgonCan = nNgon;",
        "\t\t\t\tJxCan_BatDau((int)fx, (int)fy, (int)fx, (int)fy);",
        "\t\t\t\treturn true;",
        "\t\t\t}",
        "\t\t\treturn false;",
        "\t\t}",
        "",
        "\t\tif (ev.type == SDL_EVENT_FINGER_MOTION)",
        "\t\t{",
        "\t\t\tif (nNgon == m_nNgonKyNang)",
        "\t\t\t{",
        "\t\t\t\tJxKyNang_Keo((int)fx, (int)fy);",
        "\t\t\t\treturn true;",
        "\t\t\t}",
        "\t\t\tif (nNgon == m_nNgonCan)",
        "\t\t\t{",
        "\t\t\t\tJxCan_Keo((int)fx, (int)fy);",
        "\t\t\t\treturn true;",
        "\t\t\t}",
        "\t\t\treturn false;",
        "\t\t}",
        "",
        "\t\t// FINGER_UP / FINGER_CANCELED",
        "\t\tif (m_nNgonDangDat > 0)",
        "\t\t\tm_nNgonDangDat--;",
        "\t\tif (nNgon == m_nNgonKyNang)",
        "\t\t{",
        "\t\t\tJxKyNang_Nha();",
        "\t\t\tm_nNgonKyNang = -1;",
        "\t\t\treturn true;",
        "\t\t}",
        "\t\tif (nNgon == m_nNgonCan)",
        "\t\t{",
        "\t\t\tJxCan_Nha();",
        "\t\t\tm_nNgonCan = -1;",
        "\t\t\treturn true;",
        "\t\t}",
        "\t\treturn false;",
        "\t}",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)
