# -*- coding: utf-8 -*-
r"""goi_va_wauto_ac_waypoint_0409.py - AC CHINH sang duoc CA MAP DONG / BAI LUYEN CONG.

Chu game 04/09 (kem anh): auto bao "Ac chinh dang o map 75 - map nay khong co tuyen Xa Phu
nen khong tu sang duoc", trong khi map do CO trong Xa Phu.

Chu game DUNG. Xa Phu co HAI danh sach (bin\server\script\global\npcchucnang\xaphu.lua):
    "Nhung noi da di qua"       -> WayPointFun -> settings\WayPoint.txt  (81 map, co 75)
    "Nhung thanh thi da di qua" -> StationFun  -> settings\Station.txt   (16 thanh thi)
Ban va truoc chi biet danh sach THANH THI (bang g_aDTSapTown[10] go tay trong CoreShell.cpp),
nen map 75 "Khoa Lang dong" bi coi la khong co tuyen.

Dot nay:
  - KWayPointTables.h (sinh boi gen_waypoint_tables.py tu WayPoint.txt): 81 map -> ten muc
  - AC_DiWayPoint: toi Xa Phu -> "Nhung noi da di qua" -> ten map ac chinh
  - AC_Process: khong phai thanh thi thi thu danh sach nay truoc khi bo cuoc

Chay: python goi_va_wauto_ac_waypoint_0409.py [--thu]
"""
import io
import os
import re
import sys

sys.path.insert(0, os.path.join(os.environ.get("USERPROFILE", ""),
                                ".claude", "skills", "swordonline-dev", "scripts"))
from vn_to_octal import unicode_to_tcvn3_bytes

CS = r"D:\GAMEDEVNEW\Sources\Core\Src\CoreShell.cpp"
THU = "--thu" in sys.argv
T = "\t"


def vn(t):
    return re.sub(r"@@(.*?)@@",
                  lambda m: unicode_to_tcvn3_bytes(m.group(1)).decode("latin-1"), t)


HAM = [
    "",
    "// (04/09) DI TOI MAP QUA MUC \"NHUNG NOI DA DI QUA\" CUA XA PHU.",
    "// Xa Phu co HAI danh sach (script/global/npcchucnang/xaphu.lua):",
    "//   \"Nhung thanh thi da di qua\" -> Station.txt  (16 thanh thi)   -> LD_DiThanh lo",
    "//   \"Nhung noi da di qua\"       -> WayPoint.txt (81 map dong / bai luyen cong)",
    "// Chu game 04/09: ac chinh dung o map 75 \"Khoa Lang dong\" ma auto bao \"khong co tuyen",
    "// Xa Phu\" - sai, vi truoc day WAuto chi biet danh sach thanh thi.",
    "// Tra: 1 = dang di, 0 = chua toi duoc Xa Phu, -1 = chiu (het gio / khong co trong bang).",
    "static int AC_DiWayPoint(int nPlayerIdx, const autoData* pAp, int nDestMap, UINT uCurTime)",
    "{",
    T + "ExtAuto& ea = Player[nPlayerIdx].m_sExtAuto;",
    T + "KDaTauCapture& cap = g_sDTCap;",
    T + "const char* szTen = WayPointMenu(nDestMap);",
    T + "if (!szTen)",
    T * 2 + "return -1;",
    T + "if (!ea.uLDHopT)",
    T + "{",
    T * 2 + "ea.uLDHopT = uCurTime + 150000;",
    T * 2 + "ea.uLDThpT = 0;",
    T + "}",
    T + "if (uCurTime > ea.uLDHopT)",
    T * 2 + "return -1;",
    T + "const int nMap = SubWorld[0].m_SubWorldID;",
    T + "std::map<int, StationVector>::iterator itXa = g_MoveStation.find(nMap);",
    T + "if (itXa == g_MoveStation.end() || itXa->second.empty())",
    T + "{" + T + "// dang o map khong co Xa Phu -> ve thanh truoc, nhip sau di tiep",
    T * 2 + "if (!DT_UsePortal(nPlayerIdx))",
    T * 3 + "return -1;",
    T * 2 + "ea.uLDNext = uCurTime + 4000;",
    T * 2 + "return 1;",
    T + "}",
    T + "if (cap.uDlgSeq != ea.uLDDlgSeen)",
    T + "{",
    T * 2 + "ea.uLDDlgSeen = cap.uDlgSeq;",
    T * 2 + "char szBuf[2048];",
    T * 2 + "char* apAns[16];",
    T * 2 + "g_StrCpyLen(szBuf, cap.szDlg, sizeof(szBuf));",
    T * 2 + "int nAns = DT_Split(szBuf, apAns, 16);",
    T * 2 + "int nOpt;",
    T * 2 + "if ((nOpt = DT_FindAns(apAns, nAns, szTen)) >= 0)",
    T * 2 + "{" + T + "// danh sach dang mo va co dung ten noi can den",
    T * 3 + "DT_Answer(nPlayerIdx, nOpt);",
    T * 3 + "ea.uLDNext = uCurTime + 1500;",
    T * 3 + "return 1;",
    T * 2 + "}",
    T * 2 + "if ((nOpt = DT_FindAns(apAns, nAns, DTM_SAP_NOIDADIQUA)) >= 0)",
    T * 2 + "{" + T + "// dang o menu chinh -> mo danh sach \"Nhung noi da di qua\"",
    T * 3 + "DT_Answer(nPlayerIdx, nOpt);",
    T * 3 + "ea.uLDNext = uCurTime + 900;",
    T * 3 + "return 1;",
    T * 2 + "}",
    T * 2 + "ea.uLDNext = uCurTime + 1200;",
    T * 2 + "return 1;",
    T + "}",
    T + "{" + T + "// chua mo duoc thoai -> di toi Xa Phu cua map dang dung",
    T * 2 + "const StationVector& v = itXa->second;",
    T * 2 + "int nR = TK_ToiNpc(nPlayerIdx, \"xa phu\", v[0].nX / 32, v[0].nY / 32, uCurTime);",
    T * 2 + "if (nR == 1)",
    T * 3 + "ea.uLDNext = uCurTime + 900;",
    T * 2 + "else if (nR < 0)",
    T * 3 + "return 0;",
    T + "}",
    T + "return 1;",
    "}",
]

NHANH = [
    T * 2 + "if (!DT_SapTownMenu(pAp->nACMap) && WayPointMenu(pAp->nACMap))",
    T * 2 + "{" + T + "// (04/09) khong phai thanh thi nhung CO trong \"Nhung noi da di qua\"",
    T * 2 + T + "// (81 map dong / bai luyen cong, vi du map 75 Khoa Lang dong)",
    T * 3 + "int nW = AC_DiWayPoint(nPlayerIdx, pAp, pAp->nACMap, uCurTime);",
    T * 3 + "if (ea.uLDNext > uCurTime)",
    T * 4 + "ea.uACNext = ea.uLDNext;",
    T * 3 + "if (nW >= 0)",
    T * 3 + "{",
    T * 4 + "if (ea.nACHold != 1)",
    T * 4 + "{",
    T * 5 + "s_uACBaoLoi = 0;",
    T * 5 + "DT_Msg(nPlayerIdx, \"<color=Cyan>@@Ac chính ở map khác - đang qua Xa Phu (nơi đã đi qua) để sang.@@\");",
    T * 4 + "}",
    T * 4 + "ea.nACHold = 1;",
    T * 4 + "return 1;",
    T * 3 + "}",
    T * 2 + "}",
]


def main():
    print("== goi_va_wauto_ac_waypoint_0409 %s ==" % ("(THU)" if THU else ""))
    s = io.open(CS, encoding="latin-1", newline="").read()
    goc = s
    L = s.split("\r\n")

    # 1. include bang moi
    if 'KWayPointTables.h' not in s:
        i = [k for k, l in enumerate(L) if l.strip() == '#include "KTongKimTables.h"']
        assert len(i) == 1, "khong thay include KTongKimTables.h"
        L.insert(i[0] + 1, '#include "KWayPointTables.h"')
        print("  them #include KWayPointTables.h")

    # 2. hang so ten muc menu
    if "DTM_SAP_NOIDADIQUA" not in "\r\n".join(L):
        i = [k for k, l in enumerate(L) if l.startswith("#define DTM_SAP_THANHTHI")]
        assert len(i) == 1, "khong thay DTM_SAP_THANHTHI"
        L[i[0]:i[0]] = [
            '// muc menu "Nhung noi da di qua" cua Xa Phu (xaphu.lua) - danh sach WayPoint.txt',
            '#define DTM_SAP_NOIDADIQUA "@@nơi đã đi qua@@"',
        ]
        print("  them #define DTM_SAP_NOIDADIQUA")

    # 3. ham AC_DiWayPoint - dat ngay truoc AC_KhacMap
    if "AC_DiWayPoint" not in "\r\n".join(L):
        i = [k for k, l in enumerate(L)
             if l.startswith("// (04/09) ac chinh con song va tin con moi, NHUNG dang o MAP KHAC?")]
        assert len(i) == 1, "khong thay neo dat ham"
        L[i[0]:i[0]] = HAM
        print("  them ham AC_DiWayPoint")

    # 4. nhanh thu WayPoint truoc khi bo cuoc
    if "AC_DiWayPoint(nPlayerIdx, pAp, pAp->nACMap, uCurTime)" not in "\r\n".join(L[200:]):
        i = [k for k, l in enumerate(L) if l.strip() == "if (!DT_SapTownMenu(pAp->nACMap))"]
        assert len(i) == 1, "khong thay nhanh bao loi (%d lan)" % len(i)
        k = i[0]
        L[k] = T * 2 + "if (!DT_SapTownMenu(pAp->nACMap) && !WayPointMenu(pAp->nACMap))"
        L[k + 1] = (T * 2 + "{" + T
                    + "// map ac chinh khong co trong CA HAI danh sach cua Xa Phu - bao MOT LAN")
        L[k:k] = NHANH
        print("  them nhanh WayPoint truoc nhanh bao loi")

    s2 = vn("\r\n".join(L))
    cao_g = sum(1 for c in goc if ord(c) > 127)
    cao_m = sum(1 for c in s2 if ord(c) > 127)
    print("byte cao: %d -> %d" % (cao_g, cao_m))
    if cao_m < cao_g:
        print("!! MAT byte cao"); sys.exit(1)
    if THU:
        print("(--thu: khong ghi)"); return
    io.open(CS, "w", encoding="latin-1", newline="").write(s2)
    print("da ghi CoreShell.cpp")


if __name__ == "__main__":
    main()
