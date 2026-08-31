# -*- coding: utf-8 -*-
"""t47_kiem_moc_hoatdong.py - kiem bang MOC HOAT DONG TRONG NGAY con dung khong.

script\\lib\\lib_ham.lua:296-305 co bang TB_BU_HD: moi dong la mot hoat dong,
cot 2 la TASK DEM LUOT. He thong "bu hoat dong" doc task do de biet nguoi choi
da lam du chua.

NGHI VAN: cac task nay la bo dem cua cac he CU. Neu he MOI (ban Linux/JX2) dem
bang task KHAC thi nguoi choi lam hoat dong moi se KHONG duoc tinh moc - dung
cung mot loai loi nhu Long Huyet Hoan.

Cong cu nay: voi tung task trong bang, tim AI GHI no (SetTask) va AI DOC no,
roi bao tep nao thuoc he CU / he MOI.

CHI DOC.
"""
import io
import os
import re
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

S = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
LIB_HAM = os.path.join(S, "lib", "lib_ham.lua")
LIB_TASK = os.path.join(S, "lib", "lib_task.lua")
TASKID = os.path.join(S, "header", "taskid.lua")

# thu muc thuoc HE MOI (port Linux/JX2) - de phan loai noi ghi
HE_MOI = [
    "missions\\challengeoftime", "missions\\fengling_ferry",
    "missions\\citywar", "missions\\leaguematch", "leaguematch",
    "task\\newtask\\tasklink", "task\\tollgate", "global\\seasonnpc.lua",
    "missions\\tong", "missions\\tongwar", "missions\\bw",
    "missions\\tongcastle", "missions\\bairenleitai", "tinhnang\\3hoatdong",
    "missions\\yandibaozang",
]
HE_CU = [
    "tinhnang\\datau", "tinhnang\\vuot_ai", "tinhnang\\phonglangdo",
    "tinhnang\\boss_satthu", "tinhnang\\loidai", "tinhnang\\congthanhchien",
]


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def sach(s):
    s = re.sub(r"--\[\[.*?\]\]", " ", s, flags=re.S)
    s = re.sub(r"--[^\n]*", " ", s)
    return s


def bang_task():
    """Ten task -> so hieu, doc tu lib_task.lua + header\\taskid.lua."""
    ra = {}
    for p in (LIB_TASK, TASKID):
        if not os.path.isfile(p):
            continue
        for m in re.finditer(r"^\s*([A-Za-z_]\w*)\s*=\s*(\d+)", sach(doc(p)),
                             re.M):
            ra.setdefault(m.group(1), m.group(2))
    return ra


def phan_loai(rel):
    r = rel.replace("/", "\\").lower()
    for x in HE_MOI:
        if x.lower() in r:
            return "MOI"
    for x in HE_CU:
        if x.lower() in r:
            return "CU"
    return "chung"


def tim(ten_task, kieu):
    """kieu = 'SetTask' hoac 'GetTask'. Tra ve [(rel, dong, noi_dung, loai)]."""
    pat = re.compile(r"\b%s\s*\(\s*%s\b" % (kieu, re.escape(ten_task)))
    ra = []
    for goc, _, tep in os.walk(S):
        for t in tep:
            if not t.lower().endswith(".lua") or ".truoc_" in t \
                    or t.endswith(".goc"):
                continue
            p = os.path.join(goc, t)
            rel = os.path.relpath(p, S)
            try:
                dong = doc(p).replace("\r\n", "\n").split("\n")
            except OSError:
                continue
            for i, l in enumerate(dong, 1):
                if l.strip().startswith("--"):
                    continue
                if pat.search(l):
                    ra.append((rel, i, l.strip()[:96], phan_loai(rel)))
    return ra


def main():
    bt = bang_task()
    raw = doc(LIB_HAM).replace("\r\n", "\n")
    m = re.search(r"TB_BU_HD\s*=\s*\{(.*?)\n\}", raw, re.S)
    if not m:
        print("!!! khong tim thay bang TB_BU_HD trong lib_ham.lua")
        return 1
    muc = []
    for l in m.group(1).split("\n"):
        if l.strip().startswith("--"):
            continue
        mm = re.match(r'\s*\[(\d+)\]\s*=\s*\{\s*"([^"]*)"\s*,\s*([A-Za-z_]\w*)'
                      r'\s*,\s*([A-Za-z_]\w*)\s*,\s*(\d+)', l)
        if mm:
            muc.append(mm.groups())
    print("=== BANG MOC HOAT DONG TRONG NGAY (lib_ham.lua TB_BU_HD) ===")
    print("    %d muc dang bat" % len(muc))
    print()
    hong = []
    for so, ten, task_dem, task_miss, can in muc:
        sh = bt.get(task_dem, "?")
        ghi = tim(task_dem, "SetTask")
        # bo dong reset hang ngay trong chinh lib_ham
        ghi_that = [g for g in ghi
                    if not (g[0].lower().endswith("lib_ham.lua")
                            and re.search(r",\s*0\s*\)", g[2]))]
        loai = sorted(set(g[3] for g in ghi_that))
        print("[%s] %-10s can %2s luot  |  task dem = %-18s (so %s)"
              % (so, ten, can, task_dem, sh))
        if not ghi_that:
            print("      !!! KHONG NOI NAO GHI TASK NAY -> moc KHONG BAO GIO"
                  " len duoc")
            hong.append((so, ten, task_dem, sh, "khong ai ghi"))
        else:
            print("      %d cho ghi (%s):" % (len(ghi_that), "/".join(loai)))
            for g in ghi_that[:6]:
                print("        [%-5s] %s:%d | %s" % (g[3], g[0], g[1], g[2]))
            if len(ghi_that) > 6:
                print("        (+%d cho nua)" % (len(ghi_that) - 6))
            if "MOI" not in loai:
                hong.append((so, ten, task_dem, sh,
                             "chi he CU/chung ghi - he MOI khong ghi"))
        print()

    print("=" * 74)
    if hong:
        print("KET LUAN: %d/%d moc CO VAN DE" % (len(hong), len(muc)))
        for so, ten, task_dem, sh, ly in hong:
            print("  [%s] %-10s task %-18s (so %s): %s"
                  % (so, ten, task_dem, sh, ly))
    else:
        print("KET LUAN: ca %d moc deu co he dang chay ghi vao" % len(muc))
    return 0


if __name__ == "__main__":
    sys.exit(main())
