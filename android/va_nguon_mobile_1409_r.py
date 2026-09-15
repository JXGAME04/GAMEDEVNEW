# -*- coding: latin-1 -*-
r"""[VAOMAP c 14/09] Do ClosePlace - phan con lai cua pha "mo map" luc doi map.

Chuoi suy luan tren may ao (ban 109141701 va 109141716):
  [VAOMAP] ... tong 157-432 ms = dong 0 + MO MAP 144-405 + ini 0 + ...   (ini da ve 0 nho ban giu MapList)
  [VAOMAP-MO] (>= 1 ms) KHONG ra dong nao  ->  than ham OpenPlace RE.
Ma moc jxMo[0] cua [VAOMAP-MO] dat SAU ClosePlace(), tuc ClosePlace nam ngoai phep do. ClosePlace lai co
EnterCriticalSection(m_LoadCritical) va m_ProcessCritical - hai khoa ma LUONG NAP VUNG giu khi doc pak.
Nghi ngo: doi map = luong chinh DUNG CHO luong nap xong vung dang do (144-405 ms, dung kieu cho khoa chu
khong phai viec CPU). Ban nay cam moc de chung minh thay vi doan:

  [VAOMAP-DONG] map: tong X ms = ban do Y + cho khoa nap Z + cho khoa xu ly W + don vung V + con lai U

Chi them QueryPerformanceCounter + mot dong log khi g_nCorePaintLog > 0 va tong >= 1 ms. Khong doi hanh vi.
Rao #ifdef JX_MOBILE don (bo kiem tuong duong chi hieu #ifdef don).

Chay SAU _o.py, _p.py, _q.py:
    PYTHONIOENCODING=latin-1 python android\va_nguon_mobile_1409_r.py
"""
import io
import os

GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def va(duong, cap, tag):
    p = os.path.join(GOC, duong)
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    cao0 = sum(1 for c in s if ord(c) >= 128)
    lf0 = s.count("\n") - s.count("\r\n")
    if tag in s:
        print("  bo qua (da co %s): %s" % (tag, duong))
        return
    for cu, moi in cap:
        if cu not in s:
            raise SystemExit("HONG: khong thay neo trong %s:\n%s" % (duong, cu[:220]))
        if s.count(cu) != 1:
            raise SystemExit("HONG: neo xuat hien %d lan trong %s" % (s.count(cu), duong))
        s = s.replace(cu, moi, 1)
    cao1 = sum(1 for c in s if ord(c) >= 128)
    lf1 = s.count("\n") - s.count("\r\n")
    if cao1 != cao0:
        raise SystemExit("HONG: %s byte cao %d -> %d" % (duong, cao0, cao1))
    if lf1 != lf0:
        raise SystemExit("HONG: %s co %d dong LF don (truoc %d)" % (duong, lf1, lf0))
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("  da va: %s" % duong)


print("[VAOMAP c] nam moc trong KScenePlaceC::ClosePlace")

CU1 = ("void KScenePlaceC::ClosePlace()\r\n"
       "{\r\n"
       "\tint i = 0;\r\n"
       "\tif (m_szPlaceRootPath[0] == 0)\r\n"
       "\t\treturn;\r\n"
       "\r\n"
       "\tm_nSceneId = SPWP_NO_SCENE;\r\n"
       "\tm_Map.Free();\r\n")
MOI1 = ("void KScenePlaceC::ClosePlace()\r\n"
        "{\r\n"
        "\tint i = 0;\r\n"
        "\tif (m_szPlaceRootPath[0] == 0)\r\n"
        "\t\treturn;\r\n"
        "#ifdef JX_MOBILE\r\n"
        "\t// [VAOMAP 14/09 c] doi map: OpenPlace goi ClosePlace dau tien; hai EnterCriticalSection duoi day cho\r\n"
        "\t// LUONG NAP VUNG nha khoa. Do rieng tung doan de biet 144-405 ms la CHO KHOA hay la viec that.\r\n"
        "\tLARGE_INTEGER jxD[6], jxDF; QueryPerformanceFrequency(&jxDF);\r\n"
        "\tQueryPerformanceCounter(&jxD[0]); jxD[1] = jxD[2] = jxD[3] = jxD[4] = jxD[5] = jxD[0];\r\n"
        "#endif\r\n"
        "\r\n"
        "\tm_nSceneId = SPWP_NO_SCENE;\r\n"
        "\tm_Map.Free();\r\n"
        "#ifdef JX_MOBILE\r\n"
        "\tQueryPerformanceCounter(&jxD[1]);\t// [VAOMAP 14/09 c] xong ban do nho\r\n"
        "#endif\r\n")

CU2 = ("\tEnterCriticalSection(&m_RegionListAdjustCritical);\r\n"
       "\tEnterCriticalSection(&m_LoadCritical);\r\n")
MOI2 = ("\tEnterCriticalSection(&m_RegionListAdjustCritical);\r\n"
        "\tEnterCriticalSection(&m_LoadCritical);\r\n"
        "#ifdef JX_MOBILE\r\n"
        "\tQueryPerformanceCounter(&jxD[2]);\t// [VAOMAP 14/09 c] da lay duoc hai khoa nap\r\n"
        "#endif\r\n")

CU3 = ("\tEnterCriticalSection(&m_ProcessCritical);\r\n"
       "\tClearPreprocess(true);\r\n")
MOI3 = ("\tEnterCriticalSection(&m_ProcessCritical);\r\n"
        "#ifdef JX_MOBILE\r\n"
        "\tQueryPerformanceCounter(&jxD[3]);\t// [VAOMAP 14/09 c] da lay duoc khoa xu ly\r\n"
        "#endif\r\n"
        "\tClearPreprocess(true);\r\n")

CU4 = ("\tLeaveCriticalSection(&m_ProcessCritical);\r\n"
       "\r\n"
       "\tm_nHLSpecialObjectBioIndex = SPWP_NO_HL_SPECAIL_OBJECT;\r\n")
MOI4 = ("\tLeaveCriticalSection(&m_ProcessCritical);\r\n"
        "#ifdef JX_MOBILE\r\n"
        "\tQueryPerformanceCounter(&jxD[4]);\t// [VAOMAP 14/09 c] xong don vung\r\n"
        "#endif\r\n"
        "\r\n"
        "\tm_nHLSpecialObjectBioIndex = SPWP_NO_HL_SPECAIL_OBJECT;\r\n")

CU5 = ("\tif(m_pWeather)\r\n"
       "\t{\r\n"
       "\t\tdelete m_pWeather;\r\n"
       "\t\tm_pWeather = NULL;\r\n"
       "\t}\r\n"
       "}\r\n")
MOI5 = ("\tif(m_pWeather)\r\n"
        "\t{\r\n"
        "\t\tdelete m_pWeather;\r\n"
        "\t\tm_pWeather = NULL;\r\n"
        "\t}\r\n"
        "#ifdef JX_MOBILE\r\n"
        "\t{\t// [VAOMAP 14/09 c] mot dong khi ClosePlace >= 1 ms\r\n"
        "\t\textern int g_nCorePaintLog;\r\n"
        "\t\tQueryPerformanceCounter(&jxD[5]);\r\n"
        "\t\tconst double dK = jxDF.QuadPart ? 1000.0 / (double)jxDF.QuadPart : 0.0;\r\n"
        "\t\tconst double dTong = (double)(jxD[5].QuadPart - jxD[0].QuadPart) * dK;\r\n"
        "\t\tif (g_nCorePaintLog > 0 && dTong >= 1.0)\r\n"
        "\t\t{\r\n"
        "\t\t\tFILE* pDg = fopen(\"jx_paint.log\", \"a\");\r\n"
        "\t\t\tif (pDg)\r\n"
        "\t\t\t{\r\n"
        "\t\t\t\tfprintf(pDg, \"[VAOMAP-DONG] tong %.0f ms = ban do %.0f + CHO KHOA NAP %.0f + cho khoa xu ly %.0f + don vung %.0f + con lai %.0f\\n\",\r\n"
        "\t\t\t\t\tdTong, (double)(jxD[1].QuadPart - jxD[0].QuadPart) * dK, (double)(jxD[2].QuadPart - jxD[1].QuadPart) * dK,\r\n"
        "\t\t\t\t\t(double)(jxD[3].QuadPart - jxD[2].QuadPart) * dK, (double)(jxD[4].QuadPart - jxD[3].QuadPart) * dK,\r\n"
        "\t\t\t\t\t(double)(jxD[5].QuadPart - jxD[4].QuadPart) * dK);\r\n"
        "\t\t\t\tfclose(pDg);\r\n"
        "\t\t\t}\r\n"
        "\t\t}\r\n"
        "\t}\r\n"
        "#endif\r\n"
        "}\r\n")

va(r"Sources\Core\Src\Scene\KScenePlaceC.cpp", [(CU1, MOI1), (CU2, MOI2), (CU3, MOI3), (CU4, MOI4), (CU5, MOI5)], "[VAOMAP 14/09 c]")

print("xong.")
