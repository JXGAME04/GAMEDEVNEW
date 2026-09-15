# -*- coding: latin-1 -*-
r"""[MAPLIST 14/09] Doi map: KHONG doc + phan tich lai settings\MapList.ini moi lan nua.

Do duoc bang [VAOMAP] vua cam (may ao, doi map 324): tong 95 ms = mo map 43 + INI 38 + trang tri 12 + ...
Pha "ini" = KIniFile::Load("MapList.ini") + Load("<map>.wor"). MapList.ini o ban dien thoai la 191 KB /
6 518 dong, phan tich lai tu dau MOI LAN doi map, trong khi noi dung KHONG BAO GIO doi luc dang chay.

Sua: giu MOT ban da phan tich (cap phat lan dau, khong dung bien static co ham dung de khoi phu thuoc thu tu
khoi tao), lan sau dung lai. Gia tri doc ra y het (cung tep, cung khoa). Tep .wor van doc moi lan vi moi map
mot tep. Dong [VAOMAP] in them "MapList dung lai %d" (1 = dang dung ban giu, -1 = nap hong nen di duong cu)
de doc chuoi trong .so ma biet chac ban dang chay co ban va nay.

Rao: #ifdef JX_MOBILE ... #else <nguyen van ban PC> #endif - phan PC giu TUNG DONG de bo kiem tuong duong
van bao DAT (bai hoc cua phien camera 16:4x: gom thanh bien dung chung la doi dong cua PC -> HONG).

Chay SAU va_nguon_mobile_1409_o.py:
    PYTHONIOENCODING=latin-1 python android\va_nguon_mobile_1409_p.py
"""
import io
import os

GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TAG = "[MAPLIST 14/09]"


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
            raise SystemExit("HONG: khong thay neo trong %s:\n%s" % (duong, cu[:200]))
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


print("[MAPLIST] dung lai ban MapList.ini da phan tich (LoadMap doi map)")

# --- 1. bien giu ban da phan tich, dat canh bo dem [VAOMAP] (pham vi tep de dong log dung duoc)
CU1 = ("static LARGE_INTEGER s_liJxVaoMap[9];\r\n"
       "static int s_nJxVaoMapSo = 0;\r\n")
MOI1 = (CU1 +
        "// [MAPLIST 14/09] ban MapList.ini da phan tich, giu lai giua cac lan doi map (xem chu thich trong LoadMap)\r\n"
        "static KIniFile* s_pJxMapList = NULL;\r\n"
        "static int s_nJxMapList = 0;\t// 0 = chua thu, 1 = dang dung ban giu, -1 = nap hong -> di duong cu\r\n")

# --- 2. thay khoi doc MapList.ini trong LoadMap(nId, nRegion)
CU2 = ("\t\tg_SetFilePath(\"\\\\settings\");\r\n"
       "\t\tIniFile.Load(\"MapList.ini\");\r\n"
       "\t\tsprintf(szKeyName, \"%d\", nId);\r\n"
       "\t\tIniFile.GetString(\"List\", szKeyName, \"\", m_szPathName, sizeof(m_szPathName));\r\n")

MOI2 = ("#ifdef JX_MOBILE\r\n"
        "\t\t// [MAPLIST 14/09] MapList.ini (191 KB, 6 518 dong o ban dien thoai) truoc day duoc doc + phan tich LAI\r\n"
        "\t\t// moi lan doi map: [VAOMAP] do duoc pha 'ini' 38 ms tren 95 ms cua ca lan doi map (may ao). Noi dung tep\r\n"
        "\t\t// khong doi luc dang chay -> giu mot ban da phan tich, lan sau dung lai. Gia tri doc ra y het ban cu.\r\n"
        "\t\tif (s_nJxMapList == 0)\r\n"
        "\t\t{\r\n"
        "\t\t\tg_SetFilePath(\"\\\\settings\");\r\n"
        "\t\t\ts_pJxMapList = new KIniFile;\r\n"
        "\t\t\ts_nJxMapList = (s_pJxMapList && s_pJxMapList->Load(\"MapList.ini\")) ? 1 : -1;\r\n"
        "\t\t\tif (s_nJxMapList != 1 && s_pJxMapList) { delete s_pJxMapList; s_pJxMapList = NULL; }\r\n"
        "\t\t}\r\n"
        "\t\tsprintf(szKeyName, \"%d\", nId);\r\n"
        "\t\tif (s_nJxMapList == 1)\r\n"
        "\t\t\ts_pJxMapList->GetString(\"List\", szKeyName, \"\", m_szPathName, sizeof(m_szPathName));\r\n"
        "\t\telse\r\n"
        "\t\t{\r\n"
        "\t\t\tg_SetFilePath(\"\\\\settings\");\r\n"
        "\t\t\tIniFile.Load(\"MapList.ini\");\r\n"
        "\t\t\tIniFile.GetString(\"List\", szKeyName, \"\", m_szPathName, sizeof(m_szPathName));\r\n"
        "\t\t}\r\n"
        "#else\r\n"
        + CU2 +
        "#endif\r\n")

# --- 3. dong [VAOMAP] in them trang thai (de doc chuoi trong .so ma biet ban co ban va nay)
CU3 = ("\t\t\t\tfprintf(pVm, \"[VAOMAP] map %d vung (%d,%d) map_moi=%d: tong %.0f ms = dong %.0f + mo map %.0f + ini %.0f + vung giua %.0f + 8 vung ke %.0f + trang tri %.0f + noi vung %.0f + luoi duong %.0f\\n\",\r\n"
       "\t\t\t\t\tnId, LOWORD(nRegion), HIWORD(nRegion), (int)(bLoadNew ? 1 : 0), dTong,\r\n")
MOI3 = ("\t\t\t\tfprintf(pVm, \"[VAOMAP] map %d vung (%d,%d) map_moi=%d: tong %.0f ms = dong %.0f + mo map %.0f + ini %.0f + vung giua %.0f + 8 vung ke %.0f + trang tri %.0f + noi vung %.0f + luoi duong %.0f | MapList dung lai %d\\n\",\r\n"
        "\t\t\t\t\tnId, LOWORD(nRegion), HIWORD(nRegion), (int)(bLoadNew ? 1 : 0), dTong,\r\n")
CU4 = ("\t\t\t\t\tJxVaoMapMs(4, 5), JxVaoMapMs(5, 6), JxVaoMapMs(6, 7), JxVaoMapMs(7, 8));\r\n")
MOI4 = ("\t\t\t\t\tJxVaoMapMs(4, 5), JxVaoMapMs(5, 6), JxVaoMapMs(6, 7), JxVaoMapMs(7, 8), s_nJxMapList);\t// [MAPLIST 14/09]\r\n")

va(r"Sources\Core\Src\KSubWorld.cpp", [(CU1, MOI1), (CU2, MOI2), (CU3, MOI3), (CU4, MOI4)], TAG)

print("xong.")
