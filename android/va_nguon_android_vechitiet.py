# -*- coding: utf-8 -*-
r"""[VECHITIET 11/09] DO CHO MAT CUA KHUNG GIAT: tach "ve CPU" thanh phan NAM TRONG lop ve va phan NGOAI no.

Boi canh: sau [KHOI3], khoang ve trung binh chi con 1,21 ms nhung van con khung giat.
Phien SM-F966U1_20260911_201617 (19 phut): 173 khung giat, trong do 106 khung do "ve CPU" 19,9 ms trung binh
(dinh 231,9) va 67 khung do "trinh chieu" (dinh 818,5 - nghi la gap/mo may hoac chuyen ung dung).
"ve CPU" = tu Represent Begin den End, tuc TOAN BO viec dung canh cua game cong voi viec ghi lenh cua lop ve.
Hien chi do duoc phan NAP (bang 0 o cac khung giat do) nen 20 ms kia chua biet nam o dau.

Bo do nay tach lam hai:
  - "trong lop ve" = tong thoi gian nam trong CDevGpu::DrawInternal cua ca khung (phan cua toi).
  - "ngoai"        = ve CPU tru phan tren (phan dung canh cua game: duyet NPC, sap xep, tinh toa do, giai ma...).
Biet ty le nay moi chon dung viec tiep theo: neu "ngoai" chiem gan het thi toi uu tiep o lop ve la vo ich.

Cong tac [Client] Rep3DoVeChiTiet MAC DINH 0. Bat len moi do, vi moi lenh ve ton them hai lan doc dong ho
(canh dong ~2 500 lenh/khung -> ~5 000 lan doc, uoc 0,1-0,2 ms/khung). Tat thi khong ton gi.
Doc/ghi latin-1, giu CRLF/LF, moc khop dung 1 cho, byte cao khong doi; chay lai nhieu lan khong sao.
Dung:  python android\va_nguon_android_vechitiet.py
"""
import io
import os
import re

GOC = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
R3 = os.path.join(GOC, "Sources", "Represent", "Represent3")
CFG = os.path.join(GOC, "android", "du_lieu_ghi_de", "config.ini")
DAU = "[VECHITIET 11/09]"


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
    s = sau_dong(s, r"^extern int g_nJxAtlasKhoi, g_nJxAtlasKhoiLop;[^\n]*\n",
        "extern int g_nJxDoVeChiTiet;\t// " + DAU + " [Client] Rep3DoVeChiTiet: do tong thoi gian nam TRONG lop ve moi khung (tach khoi viec dung canh cua game)\n"
        "extern double g_dJxTrongVeKhung, g_dJxTrongVeCuoi;\t// " + DAU + " cong don trong khung dang ve / gia tri cua khung vua xong (ms)\n"
        "extern unsigned g_uJxTrongVeLan;\t// " + DAU + " so lan goi DrawInternal trong khung vua xong\n",
        "extern atlas khoi")
    ghi(p, s, nl, cao)

# ============================================================ D3D9onGPUDev.cpp
p = os.path.join(R3, "D3D9onGPUDev.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = thay1(s, "HRESULT CDevGpu::DrawInternal(D3DPRIMITIVETYPE type, const BYTE* pVerts, UINT nVerts, UINT stride)\n{\n",
        "// " + DAU + " cong don thoi gian nam trong lop ve. Chi chay khi Rep3DoVeChiTiet=1 vi moi lenh ve ton hai lan doc dong ho.\n"
        "HRESULT CDevGpu::DrawInternal(D3DPRIMITIVETYPE type, const BYTE* pVerts, UINT nVerts, UINT stride)\n"
        "{\n"
        "#ifdef JX_ANDROID\n"
        "\tstruct JxDoVe\n"
        "\t{\n"
        "\t\tUint64 u0; bool bBat;\n"
        "\t\tJxDoVe() : u0(0), bBat(g_nJxDoVeChiTiet != 0) { if (bBat) u0 = SDL_GetPerformanceCounter(); }\n"
        "\t\t~JxDoVe()\n"
        "\t\t{\n"
        "\t\t\tif (!bBat) return;\n"
        "\t\t\tg_dJxTrongVeKhung += (double)(SDL_GetPerformanceCounter() - u0) * 1000.0 / (double)SDL_GetPerformanceFrequency();\n"
        "\t\t\tg_uJxTrongVeLan++;\n"
        "\t\t}\n"
        "\t} jxDoVe;\n"
        "#endif\n",
        "dau DrawInternal")
    ghi(p, s, nl, cao)

# ============================================================ KRepresentShell3.cpp
p = os.path.join(R3, "KRepresentShell3.cpp")
s, nl, cao = doc(p)
if DAU in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = sau_dong(s, r"^int g_nJxAtlasKhoi = 0, g_nJxAtlasKhoiLop = 8;[^\n]*\n",
        "int g_nJxDoVeChiTiet = 0;\t// " + DAU + "\n"
        "double g_dJxTrongVeKhung = 0.0, g_dJxTrongVeCuoi = 0.0;\tunsigned g_uJxTrongVeLan = 0;\t// " + DAU + "\n"
        "static double s_dJxTrongVeTong = 0.0, s_dJxTrongVeMax = 0.0;\t// " + DAU + " cong ca ky de in o [VE]\n",
        "dinh nghia atlas khoi")
    s = thay1(s, "\tg_nJxAtlasKhoi      = Rep3Ini(\"Rep3AtlasKhoi\", 0) ? 1 : 0;",
        "\tg_nJxDoVeChiTiet    = Rep3Ini(\"Rep3DoVeChiTiet\", 0) ? 1 : 0;\t// " + DAU + " 1 = tach 've CPU' thanh phan trong lop ve va phan dung canh cua game\n"
        "\tg_nJxAtlasKhoi      = Rep3Ini(\"Rep3AtlasKhoi\", 0) ? 1 : 0;",
        "doc ini atlas khoi")
    # chot gia tri cua khung ngay khi do ve CPU xong
    s = thay1(s, "\ts_dJxVeCpuCuoi = g_liRep3VeBegin.QuadPart ? Rep3NapMs(g_liRep3VeBegin, li) : 0.0;\n",
        "\ts_dJxVeCpuCuoi = g_liRep3VeBegin.QuadPart ? Rep3NapMs(g_liRep3VeBegin, li) : 0.0;\n"
        "\tg_dJxTrongVeCuoi = g_dJxTrongVeKhung; g_dJxTrongVeKhung = 0.0;\t// " + DAU + " chot phan nam trong lop ve cua khung vua xong\n"
        "\ts_dJxTrongVeTong += g_dJxTrongVeCuoi; if (g_dJxTrongVeCuoi > s_dJxTrongVeMax) s_dJxTrongVeMax = g_dJxTrongVeCuoi;\n",
        "chot ve CPU")
    # in o [VE-GIAT]
    s = thay1(s, "\tRep3Log(\"[VE-GIAT] khung %u: %.1f ms (viec %.1f, khong ke cho) = ve CPU %.1f (nap %.1f ms:",
        "\tRep3Log(\"[VE-GIAT] khung %u: %.1f ms (viec %.1f, khong ke cho) = ve CPU %.1f [trong lop ve %.1f / %u lenh, ngoai %.1f] (nap %.1f ms:",
        "[VE-GIAT] format")
    s = thay1(s, "\t\tg_uJxVeKhungSo, s_dJxVeCpuCuoi + dTrinhChieu, dViec, s_dJxVeCpuCuoi, dNap,",
        "\t\tg_uJxVeKhungSo, s_dJxVeCpuCuoi + dTrinhChieu, dViec, s_dJxVeCpuCuoi, g_dJxTrongVeCuoi, g_uJxTrongVeLan, s_dJxVeCpuCuoi - g_dJxTrongVeCuoi, dNap,",
        "[VE-GIAT] args")
    # in o [VE]
    s = thay1(s, "| ve CPU (Begin->End) TB %.2f (max %.1f) |",
        "| ve CPU (Begin->End) TB %.2f (max %.1f) [do chi tiet=%d: trong lop ve TB %.2f (max %.1f)] |",
        "[VE] format")
    s = thay1(s, "s_uJxVeCpuKhung ? s_dJxVeCpuTong / s_uJxVeCpuKhung : 0.0, s_dJxVeCpuMax,",
        "s_uJxVeCpuKhung ? s_dJxVeCpuTong / s_uJxVeCpuKhung : 0.0, s_dJxVeCpuMax,\n"
        "\t\tg_nJxDoVeChiTiet, s_uJxVeCpuKhung ? s_dJxTrongVeTong / s_uJxVeCpuKhung : 0.0, s_dJxTrongVeMax,\t// " + DAU + "\n",
        "[VE] args")
    s = thay1(s, "\ts_dJxVeCpuTong = 0.0; s_dJxVeCpuMax = 0.0; s_uJxVeCpuKhung = 0;",
        "\ts_dJxTrongVeTong = 0.0; s_dJxTrongVeMax = 0.0;\t// " + DAU + "\n"
        "\ts_dJxVeCpuTong = 0.0; s_dJxVeCpuMax = 0.0; s_uJxVeCpuKhung = 0;",
        "dat lai ky")
    ghi(p, s, nl, cao)

# ============================================================ config.ini
p = CFG
s, nl, cao = doc(p)
if "Rep3DoVeChiTiet" in s:
    print("da va roi:", os.path.relpath(p, GOC))
else:
    s = thay1(s, "Rep3AtlasKhoi=0\n",
        "; " + DAU + " Rep3DoVeChiTiet=1: tach 've CPU' thanh phan NAM TRONG lop ve va phan dung canh cua game.\n"
        ";   Chi bat khi dang do: moi lenh ve ton them hai lan doc dong ho (~0,1-0,2 ms/khung o canh dong).\n"
        ";   Doc [VE-GIAT] 've CPU X [trong lop ve Y / N lenh, ngoai Z]' va [VE] 'trong lop ve TB ...'.\n"
        "Rep3DoVeChiTiet=0\n"
        "Rep3AtlasKhoi=0\n",
        "config Rep3AtlasKhoi")
    ghi(p, s, nl, cao)

print("xong")
