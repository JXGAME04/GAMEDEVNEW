# -*- coding: utf-8 -*-
"""VIEM DE - buoc 2: khai bao voi may chu 15 ban do PHONG CHO NHAN BAN (be pho ban).

Vi sao can: ban goc Linux nhan ban phong cho cho TUNG TO DOI
(`readymap.lua:77` self:new_dungeon(852), `ready.lua:74` tran 15 doi).
JX1 khong co co che nhan ban ban do luc chay, NHUNG:
  - KSubWorld::LoadMap doc duong dan tu MapList.ini theo ID, nen NHIEU ID
    tro CUNG MOT thu muc ban do la hop le (JX1 da lam san: 379 va 380 dung
    chung y het thu muc - xem chu thich KSubWorld.cpp:1771).
  - Vay ta khai truoc mot BE 15 ban do tinh, roi ApplyDungeonMap() phat ra
    mot id ranh. Hanh vi voi nguoi choi GIONG HET ban goc: moi to doi mot phong rieng.

Sua 2 tep:
  settings\\MapList.ini            them <id>= / <id>_name / <id>_NewWorldScript
  maps\\WorldSet_GameServer.ini    them World<nnn>=<id> va nang [Init] Count

Chay lai duoc nhieu lan. Co sao luu .truoc_viemde_2608.
"""
import io
import os
import re
import shutil
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MAPLIST = SRV + r"\settings\MapList.ini"
WORLDSET = SRV + r"\maps\WorldSet_GameServer.ini"
BAK = ".truoc_viemde_2608"
NHAN = "; [VIEM DE 26/08] be phong cho nhan ban"

POOL = list(range(1003, 1018))          # 15 ban do
DUONG_DAN = "\xcc\xd8\xca\xe2\xd3\xc3\xb5\xd8\\\xd1\xd7\xb5\xdb\xb1\xa6\xb2\xd8\xd7\xbc\xb1\xb8\xb3\xa1"
HOOK = "\\script\\missions\\yandibaozang\\readymap\\ydbz_maphook.lua"


def V(s):
    """chu Viet -> byte TCVN3, doc lai bang latin-1 de ghi thang vao tep"""
    return unicode_to_tcvn3_bytes(s).decode("latin-1")


def doc(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def ghi(p, d):
    if not os.path.isfile(p + BAK):
        shutil.copyfile(p, p + BAK)
    io.open(p, "w", encoding="latin-1", newline="").write(d)


def va_maplist():
    d = doc(MAPLIST)
    if NHAN in d:
        print("MapList.ini  : da vá rồi, bỏ qua")
        return 0
    # kiem: id trong be chua bi ai dung
    for i in POOL:
        assert not re.search(r"(?m)^%d\s*=" % i, d), "MapList da co ban do %d - doi be!" % i
    khoi = ["", NHAN]
    for k, i in enumerate(POOL, 1):
        khoi.append("%d=%s" % (i, DUONG_DAN))
        khoi.append("%d_name=%s" % (i, V("Bảo tàng Viêm Đế - phòng chờ %d" % k)))
        khoi.append("%d_NewWorldScript=%s" % (i, HOOK))
        khoi.append("")
    # chen NGAY TRUOC dong Default_NewWorldScript (cuoi tep)
    neo = "Default_NewWorldScript="
    assert d.count(neo) == 1, "khong tim thay neo Default_NewWorldScript"
    vt = d.rindex(neo)
    # lui ve dau dong chua neo
    dau = d.rfind("\n", 0, vt) + 1
    d = d[:dau] + "\r\n".join(khoi) + "\r\n" + d[dau:]
    ghi(MAPLIST, d)
    print("MapList.ini  : đã thêm %d bản đồ (%d–%d)" % (len(POOL), POOL[0], POOL[-1]))
    return len(POOL)


def va_worldset():
    d = doc(WORLDSET)
    if NHAN in d:
        print("WorldSet     : da vá rồi, bỏ qua")
        return 0
    dong = d.split("\n")
    # tim so muc World hien co + so lon nhat
    maxn = -1
    for l in dong:
        m = re.match(r"World(\d+)\s*=", l.strip())
        if m:
            maxn = max(maxn, int(m.group(1)))
    assert maxn >= 0, "khong doc duoc muc World nao"
    # kiem trung id
    hien = set()
    for l in dong:
        m = re.match(r"World\d+\s*=\s*(\d+)", l.strip())
        if m:
            hien.add(int(m.group(1)))
    for i in POOL:
        assert i not in hien, "WorldSet da nap ban do %d - doi be!" % i
    # CHU THICH PHAI THUAN ASCII: tep nay dung TCVN3 BOC UTF-8 (check_encoding.py bao
    # "UTF-8-WRAPPED TCVN3"), khac han MapList.ini (TCVN3 tho). Viet ASCII la an toan
    # ca hai kieu, va dung y het cac muc moi nhat cua phien truoc (World909, World910).
    them = [NHAN]
    for k, i in enumerate(POOL, 1):
        maxn += 1
        them.append("World%03d=%d --- Bao tang Viem De - phong cho %d [VIEMDE 26/08]" % (maxn, i, k))
    d = d.rstrip("\r\n") + "\r\n" + "\r\n".join(them) + "\r\n"
    # nang Count
    m = re.search(r"(?m)^(Count\s*=\s*)(\d+)", d)
    assert m, "khong tim thay [Init] Count"
    cu = int(m.group(2))
    moi = maxn + 1
    d = d[:m.start()] + m.group(1) + str(moi) + d[m.end():]
    ghi(WORLDSET, d)
    print("WorldSet     : Count %d -> %d, thêm World%03d..World%03d" % (cu, moi, maxn - len(POOL) + 1, maxn))
    return len(POOL)


def kiem_lai():
    d = doc(MAPLIST)
    ws = doc(WORLDSET)
    ok = 0
    for i in POOL:
        a = re.search(r"(?m)^%d\s*=" % i, d) is not None
        b = re.search(r"(?m)^World\d+\s*=\s*%d\s" % i, ws) is not None
        c = re.search(r"(?m)^%d_NewWorldScript=" % i, d) is not None
        if a and b and c:
            ok += 1
        else:
            print("   !! ban do %d: MapList=%s WorldSet=%s Hook=%s" % (i, a, b, c))
    n = len(re.findall(r"(?m)^World\d+\s*=", ws))
    cnt = int(re.search(r"(?m)^Count\s*=\s*(\d+)", ws).group(1))
    print("KIEM LAI     : %d/%d bản đồ khai đủ 3 khoá | WorldSet có %d mục, Count=%d %s"
          % (ok, len(POOL), n, cnt, "(khớp)" if n == cnt else "<< LỆCH!"))
    assert n == cnt, "Count khong khop so muc World - GameServer se nap thieu/thua!"


va_maplist()
va_worldset()
kiem_lai()
