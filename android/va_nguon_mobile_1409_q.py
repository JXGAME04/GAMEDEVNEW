# -*- coding: latin-1 -*-
r"""[MAPLIST b][VAOMAP b 14/09] Pha "mo map" = thu pham that cua lan doi map.

Do duoc ngay tren may ao voi ban 109141701 (auto doi map may lan):
    [VAOMAP] map 78 ...: tong 352 ms = dong 0 + MO MAP 314 + ini 0 + ... + trang tri 15 + luoi duong 14
    [VAOMAP] map 93 ...: tong 160 ms = dong 0 + MO MAP 149 + ini 0 + ...
    [VAOMAP] map 78 ...: tong 432 ms = dong 0 + MO MAP 405 + ini 0 + ...
Pha "ini" da ve 0 (ban giu MapList.ini chay dung). Gio tat ca nam trong g_ScenePlace.OpenPlace().

Hai viec o ban nay:
 1. [MAPLIST b] OpenPlace CUNG doc \settings\MapList.ini (191 KB) mot lan nua moi lan doi map
    (KScenePlaceC.cpp:344) -> dung chung ban da phan tich cua KSubWorld (ham JxMapListGiu).
    KHONG goi Clear() tren ban giu (Ini rieng van Clear nhu cu).
 2. [VAOMAP b] cam ba moc trong OpenPlace -> dong [VAOMAP-MO] (>= 1 ms): danh sach map + .wor |
    m_Map.Load (du lieu map) | con lai (anh nen vung, den moi truong). Vong sau biet cat cho nao.

Rao: #ifdef JX_MOBILE ... #else <nguyen van PC> #endif; bo kiem tuong duong chi hieu #ifdef don.
Neo cho khoi cuoi ham chi lay den "\tSetLoadingStatus(true);" roi "\t//" vi dong ke tiep la chu thich GBK.
Chay SAU _o.py va _p.py:
    PYTHONIOENCODING=latin-1 python android\va_nguon_mobile_1409_q.py
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


# ---------------------------------------------------------------- 1. ham dung chung trong KSubWorld.cpp
print("[MAPLIST b] mo ban giu MapList.ini cho ca KScenePlaceC")

CU1 = ("\t\tif (s_nJxMapList == 0)\r\n"
       "\t\t{\r\n"
       "\t\t\tg_SetFilePath(\"\\\\settings\");\r\n"
       "\t\t\ts_pJxMapList = new KIniFile;\r\n"
       "\t\t\ts_nJxMapList = (s_pJxMapList && s_pJxMapList->Load(\"MapList.ini\")) ? 1 : -1;\r\n"
       "\t\t\tif (s_nJxMapList != 1 && s_pJxMapList) { delete s_pJxMapList; s_pJxMapList = NULL; }\r\n"
       "\t\t}\r\n"
       "\t\tsprintf(szKeyName, \"%d\", nId);\r\n"
       "\t\tif (s_nJxMapList == 1)\r\n")
MOI1 = ("\t\tJxMapListGiu();\t// [MAPLIST 14/09 b] nap ban giu neu chua co\r\n"
        "\t\tsprintf(szKeyName, \"%d\", nId);\r\n"
        "\t\tif (s_nJxMapList == 1)\r\n")

CU2 = ("static int s_nJxMapList = 0;\t// 0 = chua thu, 1 = dang dung ban giu, -1 = nap hong -> di duong cu\r\n")
MOI2 = (CU2 +
        "// [MAPLIST 14/09 b] ban giu dung chung trong libCoreClient.so (KScenePlaceC::OpenPlace cung doc MapList.ini\r\n"
        "// mot lan nua moi lan doi map). Tra NULL = khong dung duoc -> noi goi tu doc tep nhu duong cu.\r\n"
        "KIniFile* JxMapListGiu()\r\n"
        "{\r\n"
        "\tif (s_nJxMapList == 0)\r\n"
        "\t{\r\n"
        "\t\tg_SetFilePath(\"\\\\settings\");\r\n"
        "\t\ts_pJxMapList = new KIniFile;\r\n"
        "\t\ts_nJxMapList = (s_pJxMapList && s_pJxMapList->Load(\"MapList.ini\")) ? 1 : -1;\r\n"
        "\t\tif (s_nJxMapList != 1 && s_pJxMapList) { delete s_pJxMapList; s_pJxMapList = NULL; }\r\n"
        "\t}\r\n"
        "\treturn (s_nJxMapList == 1) ? s_pJxMapList : NULL;\r\n"
        "}\r\n")

va(r"Sources\Core\Src\KSubWorld.cpp", [(CU2, MOI2), (CU1, MOI1)], "[MAPLIST 14/09 b]")

# ---------------------------------------------------------------- 2. OpenPlace: dung ban giu + ba moc
print("[VAOMAP b] ba moc trong KScenePlaceC::OpenPlace + dung ban giu")

CU3 = ("bool KScenePlaceC::OpenPlace(int nPlaceIndex)\r\n"
       "{\r\n"
       "\tif (m_bInited == false)\r\n"
       "\t\treturn false;\r\n"
       "\r\n"
       "\tClosePlace();\r\n"
       "\r\n"
       "\tKIniFile\tIni;\r\n"
       "\tchar\t\tIndex[16];\r\n"
       "\tchar\t\tBuff[128];\r\n")
MOI3 = ("#ifdef JX_MOBILE\r\n"
        "KIniFile* JxMapListGiu();\t// [MAPLIST 14/09 b] KSubWorld.cpp (cung libCoreClient.so)\r\n"
        "#endif\r\n"
        + CU3 +
        "#ifdef JX_MOBILE\r\n"
        "\t// [VAOMAP 14/09 b] may ao do duoc pha 'mo map' 149-405 ms trong tong 160-432 ms cua mot lan doi map\r\n"
        "\t// -> chia nho: danh sach map + .wor | m_Map.Load (du lieu map) | con lai (anh nen vung, den moi truong)\r\n"
        "\tLARGE_INTEGER jxMo[4], jxMoF; QueryPerformanceFrequency(&jxMoF); QueryPerformanceCounter(&jxMo[0]);\r\n"
        "\tjxMo[1] = jxMo[2] = jxMo[3] = jxMo[0];\r\n"
        "\tKIniFile* pJxGiu = JxMapListGiu();\r\n"
        "#endif\r\n")

CU4 = ("\tif (Ini.Load(\"\\\\settings\\\\MapList.ini\") == FALSE)\r\n"
       "\t\treturn false;\r\n"
       "\r\n"
       "\titoa(nPlaceIndex, Index, 10);\r\n"
       "\tif (Ini.GetString(\"List\", Index, \"\", Buff, sizeof(Buff)) == FALSE)\r\n"
       "\t\treturn false;\r\n")
MOI4 = ("#ifdef JX_MOBILE\r\n"
        "\tif (pJxGiu == NULL && Ini.Load(\"\\\\settings\\\\MapList.ini\") == FALSE)\t// [MAPLIST 14/09 b] co ban giu thi khoi doc lai 191 KB\r\n"
        "\t\treturn false;\r\n"
        "\r\n"
        "\titoa(nPlaceIndex, Index, 10);\r\n"
        "\tif ((pJxGiu ? pJxGiu : &Ini)->GetString(\"List\", Index, \"\", Buff, sizeof(Buff)) == FALSE)\r\n"
        "\t\treturn false;\r\n"
        "#else\r\n"
        + CU4 +
        "#endif\r\n")

CU5 = ("\tstrcat(Index, \"_name\");\r\n"
       "\tif (!Ini.GetString(\"List\", Index, \"\", m_szSceneName, sizeof(m_szSceneName)))\r\n")
MOI5 = ("\tstrcat(Index, \"_name\");\r\n"
        "#ifdef JX_MOBILE\r\n"
        "\tif (!(pJxGiu ? pJxGiu : &Ini)->GetString(\"List\", Index, \"\", m_szSceneName, sizeof(m_szSceneName)))\t// [MAPLIST 14/09 b]\r\n"
        "#else\r\n"
        "\tif (!Ini.GetString(\"List\", Index, \"\", m_szSceneName, sizeof(m_szSceneName)))\r\n"
        "#endif\r\n")

CU6 = ("\tm_nSceneId = nPlaceIndex;\r\n"
       "\tNapLopCanh(nPlaceIndex);\t// [ANHNEN 10/09]\r\n")
MOI6 = ("\tm_nSceneId = nPlaceIndex;\r\n"
        "#ifdef JX_MOBILE\r\n"
        "\tQueryPerformanceCounter(&jxMo[1]);\t// [VAOMAP 14/09 b] xong danh sach map + .wor\r\n"
        "#endif\r\n"
        "\tNapLopCanh(nPlaceIndex);\t// [ANHNEN 10/09]\r\n")

CU7 = ("\tint nIsInDoor;\r\n"
       "\tIni.GetInteger(\"MAIN\", \"IsInDoor\", 0, &nIsInDoor);\r\n")
MOI7 = ("#ifdef JX_MOBILE\r\n"
        "\tQueryPerformanceCounter(&jxMo[2]);\t// [VAOMAP 14/09 b] xong m_Map.Load (du lieu map)\r\n"
        "#endif\r\n"
        + CU7)

CU8 = ("\tSetLoadingStatus(true);\r\n"
       "\t//")
MOI8 = ("\tSetLoadingStatus(true);\r\n"
        "#ifdef JX_MOBILE\r\n"
        "\t{\t// [VAOMAP 14/09 b] mot dong khi OpenPlace >= 1 ms\r\n"
        "\t\textern int g_nCorePaintLog;\r\n"
        "\t\tQueryPerformanceCounter(&jxMo[3]);\r\n"
        "\t\tconst double dK = jxMoF.QuadPart ? 1000.0 / (double)jxMoF.QuadPart : 0.0;\r\n"
        "\t\tconst double dTong = (double)(jxMo[3].QuadPart - jxMo[0].QuadPart) * dK;\r\n"
        "\t\tif (g_nCorePaintLog > 0 && dTong >= 1.0)\r\n"
        "\t\t{\r\n"
        "\t\t\tFILE* pMo = fopen(\"jx_paint.log\", \"a\");\r\n"
        "\t\t\tif (pMo)\r\n"
        "\t\t\t{\r\n"
        "\t\t\t\tfprintf(pMo, \"[VAOMAP-MO] map %d: tong %.0f ms = danh sach + wor %.0f + du lieu map %.0f + con lai %.0f (ban giu MapList %d)\\n\",\r\n"
        "\t\t\t\t\tnPlaceIndex, dTong, (double)(jxMo[1].QuadPart - jxMo[0].QuadPart) * dK,\r\n"
        "\t\t\t\t\t(double)(jxMo[2].QuadPart - jxMo[1].QuadPart) * dK, (double)(jxMo[3].QuadPart - jxMo[2].QuadPart) * dK,\r\n"
        "\t\t\t\t\t(int)(pJxGiu != NULL));\r\n"
        "\t\t\t\tfclose(pMo);\r\n"
        "\t\t\t}\r\n"
        "\t\t}\r\n"
        "\t}\r\n"
        "#endif\r\n"
        "\t//")

va(r"Sources\Core\Src\Scene\KScenePlaceC.cpp", [(CU3, MOI3), (CU4, MOI4), (CU5, MOI5), (CU6, MOI6), (CU7, MOI7), (CU8, MOI8)], "[VAOMAP 14/09 b]")

print("xong.")
