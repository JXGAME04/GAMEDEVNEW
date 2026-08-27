# -*- coding: utf-8 -*-
"""VIEM DE - buoc 1: chep du lieu ban do Viem De vao dung cho client + server.

Nguon (da co san trong cay client):
    bin\\client\\maps2\\<TAC THU DUNG DIA>\\{炎帝宝藏, 炎帝宝藏准备场}
Dich:
    bin\\client\\maps\\...    giu nguyen ten *_Region_C.dat   (client doc ban _C)
    bin\\server\\maps\\...    DOI TEN  *_Region_C.dat -> *_Region_S.dat
                              (KRegion::LoadObject KRegion.cpp:157 CHI mo Region_S.dat,
                               khong co nhanh lui ve _C; doan OBSTACLE cua hai ban la
                               MOT dinh dang, dai dung 2048 byte = sizeof int[16][32])

Chay lai duoc nhieu lan (idempotent): tep da co va trung kich thuoc thi bo qua.
"""
import os
import shutil
import struct
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

SEP = "\\"
CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"

# ten thu muc GBK, viet duoi dang byte doc bang latin-1 (KHONG go tay ky tu Trung)
DACTHU = "\xcc\xd8\xca\xe2\xd3\xc3\xb5\xd8"                      # 特殊用地
VIEMDE = "\xd1\xd7\xb5\xdb\xb1\xa6\xb2\xd8"                      # 炎帝宝藏
PHONGCHO = VIEMDE + "\xd7\xbc\xb1\xb8\xb3\xa1"                   # 炎帝宝藏准备场

SRC_ROOT = CLI + SEP + "maps2" + SEP + DACTHU
CLI_DST = CLI + SEP + "maps" + SEP + DACTHU
SRV_DST = SRV + SEP + "maps" + SEP + DACTHU

OBSTACLE_LEN = 16 * 32 * 4      # 2048


def check_region(path):
    """tra (co_vat_can, do_dai_doan_obstacle) - de bao cao, va bat tep hong."""
    d = open(path, "rb").read()
    if len(d) < 4:
        return None
    n = struct.unpack_from("<I", d, 0)[0]
    if n == 0 or n > 64:
        return None
    off, ln = struct.unpack_from("<II", d, 4)
    return ln


def copy_one(src, dst):
    if os.path.isfile(dst) and os.path.getsize(dst) == os.path.getsize(src):
        return 0
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.copyfile(src, dst)
    return 1


def do_map(name, tag):
    src = SRC_ROOT + SEP + name
    print("=== %s  (%s) ===" % (tag, name.encode("latin-1").decode("gbk")))
    if not os.path.isdir(src):
        print("    !! KHONG THAY NGUON:", src)
        return
    nCli = nSrv = 0
    nObs = nStub = nBad = 0
    for dv in sorted(os.listdir(src)):
        pv = os.path.join(src, dv)
        if not os.path.isdir(pv):
            continue
        for f in sorted(os.listdir(pv)):
            if not f.endswith("_Region_C.dat"):
                continue
            sp = os.path.join(pv, f)
            ln = check_region(sp)
            if ln is None:
                nBad += 1
                print("    !! tep hong, BO QUA:", dv + SEP + f)
                continue
            if ln == OBSTACLE_LEN:
                nObs += 1
            elif ln == 0:
                nStub += 1
            else:
                nBad += 1
                print("    !! doan OBSTACLE dai %d (can 0 hoac %d), BO QUA: %s"
                      % (ln, OBSTACLE_LEN, dv + SEP + f))
                continue
            nCli += copy_one(sp, CLI_DST + SEP + name + SEP + dv + SEP + f)
            fS = f.replace("_Region_C.dat", "_Region_S.dat")
            nSrv += copy_one(sp, SRV_DST + SEP + name + SEP + dv + SEP + fS)
    # .wor va anh nho
    nExtra = 0
    for suffix in (".wor", "24.jpg"):
        sp = SRC_ROOT + SEP + name + suffix
        if os.path.isfile(sp):
            nExtra += copy_one(sp, CLI_DST + SEP + name + suffix)
            nExtra += copy_one(sp, SRV_DST + SEP + name + suffix)
        else:
            print("    !! thieu tep phu:", name + suffix)
    print("    region: %d co vat can, %d dat trong, %d hong/bo qua" % (nObs, nStub, nBad))
    print("    da chep MOI: client %d tep, server %d tep, tep phu %d" % (nCli, nSrv, nExtra))


def main():
    print("NGUON :", SRC_ROOT)
    print("DICH  :", CLI_DST)
    print("       ", SRV_DST)
    print()
    do_map(PHONGCHO, "PHONG CHO (851/852 + pool)")
    print()
    do_map(VIEMDE, "BAO TANG (853-862)")
    print()
    # doi chieu lai: dem tep o dich
    for lbl, root, pat in (("client", CLI_DST, "_Region_C.dat"), ("server", SRV_DST, "_Region_S.dat")):
        tot = 0
        for dp, dn, fn in os.walk(root):
            tot += sum(1 for f in fn if f.endswith(pat))
        print("KIEM LAI: %-6s co %d tep %s" % (lbl, tot, pat))


main()
