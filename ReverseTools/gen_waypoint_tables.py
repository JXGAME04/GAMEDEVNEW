# -*- coding: utf-8 -*-
r"""gen_waypoint_tables.py - SINH BANG "NHUNG NOI DA DI QUA" cua Xa Phu.

Vi sao can: Xa Phu co HAI danh sach dia diem (script\global\npcchucnang\xaphu.lua):
    "Nhung noi da di qua"      -> WayPointFun -> settings\WayPoint.txt   (dong/bai luyen cong)
    "Nhung thanh thi da di qua" -> StationFun  -> settings\Station.txt    (16 thanh thi)
Truoc day WAuto chi biet Station (bang g_aDTSapTown[10] go tay), nen ac chinh dung o
map 75 "Khoa Lang dong" thi auto bao "map nay khong co tuyen Xa Phu" - SAI, chu game
bao dung: map do CO trong Xa Phu, chi la nam o danh sach kia.

Sinh ra KWayPointTables.h: mapId -> ten muc trong menu "Nhung noi da di qua".
Mot map co the co NHIEU diem (vi du map 75 co 4 diem cung ten "Khoa Lang dong") -
lay ten cua diem DAU TIEN, vi menu chi can dung ten de bam.

Chay: python gen_waypoint_tables.py [--ghi]
WayPoint.txt la TCVN3 -> doc latin-1, ghi thang byte do vao .h (nguon JX1 cung TCVN3).
"""
import io
import os
import re
import sys

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
HDR = r"D:\GAMEDEVNEW\Sources\Core\Src\KWayPointTables.h"
GHI = "--ghi" in sys.argv

sys.path.insert(0, os.path.join(os.environ.get("USERPROFILE", ""),
                                ".claude", "skills", "swordonline-dev", "scripts"))
from vn_to_octal import _UNICODE_TO_TCVN3

_T = {ord(v): k for k, v in _UNICODE_TO_TCVN3.items()}


def doc(s):
    """byte TCVN3 (da doc bang latin-1) -> tieng Viet, chi de IN ra man hinh"""
    return "".join(_T.get(ord(c), c) for c in s)


def main():
    p = os.path.join(SRV, r"settings\WayPoint.txt")
    s = io.open(p, encoding="latin-1", newline="").read().replace("\r", "")
    ra = []          # (mapId, ten) theo thu tu gap dau tien
    thay = {}
    dem = {}
    for l in s.split("\n"):
        c = l.split("\t")
        if len(c) < 3 or not c[0].strip().isdigit():
            continue
        ten = c[1].strip()
        m = re.match(r"^\s*(\d+)\s*,", c[2])
        if not m:
            continue
        mid = int(m.group(1))
        dem[mid] = dem.get(mid, 0) + 1
        if mid in thay:
            continue
        thay[mid] = ten
        ra.append((mid, ten))
    ra.sort()

    print("WayPoint.txt: %d map khac nhau (%d diem)" % (len(ra), sum(dem.values())))
    for mid, ten in ra[:8]:
        print("   map %-4d %-28s (%d diem)" % (mid, doc(ten), dem[mid]))
    print("   ...")
    if 75 in thay:
        print("   -> map 75 = %r  <-- map chu game bao" % doc(thay[75]))

    d = ['// ===== "NHUNG NOI DA DI QUA" cua Xa Phu - SINH TU DONG boi',
         '// ReverseTools/gen_waypoint_tables.py tu bin\\server\\settings\\WayPoint.txt. DUNG SUA TAY.',
         '// Xa Phu co HAI danh sach (script/global/npcchucnang/xaphu.lua):',
         '//   "Nhung noi da di qua"       -> WayPoint.txt (bang nay: dong / bai luyen cong)',
         '//   "Nhung thanh thi da di qua" -> Station.txt  (bang g_aDTSapTown trong CoreShell.cpp)',
         '// Mot map co the co nhieu diem trung ten - chi giu ten cua diem dau tien vi menu',
         '// chon theo TEN. Ten giu nguyen byte TCVN3 nhu trong tep goc.',
         '#ifndef KWAYPOINTTABLES_H',
         '#define KWAYPOINTTABLES_H',
         '',
         'struct WayPointRow { int nMapId; const char* szMenu; };',
         '#define WAYPOINT_COUNT\t%d' % len(ra),
         'static const WayPointRow g_aWayPoint[WAYPOINT_COUNT] =',
         '{']
    for mid, ten in ra:
        d.append('\t{ %-4d, "%s" },' % (mid, ten))
    d.append('};')
    d.append('')
    d.append('// map nay co trong menu "Nhung noi da di qua"? tra ten muc, NULL = khong co')
    d.append('static const char* WayPointMenu(int nMapId)')
    d.append('{')
    d.append('\tfor (int i = 0; i < WAYPOINT_COUNT; ++i)')
    d.append('\t\tif (g_aWayPoint[i].nMapId == nMapId)')
    d.append('\t\t\treturn g_aWayPoint[i].szMenu;')
    d.append('\treturn NULL;')
    d.append('}')
    d.append('')
    d.append('#endif // KWAYPOINTTABLES_H')
    khoi = "\n".join(d)
    print("\n(%d dong header)" % len(d))

    if GHI:
        nl = "\r\n"
        io.open(HDR, "w", encoding="latin-1", newline="").write(khoi.replace("\n", nl) + nl)
        print("-> da ghi %s" % HDR)


if __name__ == "__main__":
    main()
