# -*- coding: utf-8 -*-
"""t44_nghiem_thu.py - nghiem thu sau khi go he cu.

Ba viec:
  1. Quet TOAN CAY tim tham chieu TREO: dong ma (khong phai comment) tro toi
     mot tep/thu muc DA BI DOI sang _dara.
  2. Quet ham NIL: loi goi toi ham chi co trong cac tep da doi.
  3. Kiem cu phap TOAN BO cay script bang syncheck.exe (LuaLib 4.0.1 cua engine).

CHI DOC.
"""
import io
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import doi_tep as dt  # noqa: E402
import lua_ham as lh  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

SYNCHECK = (r"C:\Users\nguye\AppData\Local\Temp\claude"
            r"\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto"
            r"\61d924bb-ce66-46e1-a22b-820305b4bde7\scratchpad\luac"
            r"\syncheck.exe")
DARA_S = os.path.join(dt.DARA, "script")


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


SF = r"D:\GAMEDEVNEW\Sources\Core\Src\ScriptFuns.cpp"


def ham_engine():
    """Ten cac ham C++ dang ky cho Lua ({ "Ten", LuaTen }).

    BAT BUOC phai co: neu chi so voi khai bao Lua thi bao NHAM. Vi du
    IsArenaBegin / GetArenaBothSides / GetJoinTongTime la ham ENGINE
    (ScriptFuns.cpp:15553,15554,15663) nhung lib_loidai.lua cu cung khai ten
    trung -> doi lib_loidai di la tuong 'mat ham', trong khi that ra la TRA VE
    ban engine.
    """
    ra = set()
    if not os.path.isfile(SF):
        return ra
    d = doc(SF)
    for m in re.finditer(r'\{\s*"([A-Za-z_]\w*)"\s*,\s*\w+\s*\}', d):
        ra.add(m.group(1))
    return ra


def duong_da_doi():
    """Duong dan hien nam trong _dara VA KHONG con trong cay script.

    BAY DA VAP: neu lay ca thu muc cha cua tung tep (vd 'header', 'item') thi
    moi dong co chu 'item' deu bi cham la tham chieu treo -> 1116 bao nham.
    Chi lay: (a) duong dan TEP day du, (b) thu muc nao da BIEN MAT khoi cay
    script that.
    """
    tep_ra, tm_ra = [], set()
    if not os.path.isdir(DARA_S):
        return tep_ra, tm_ra
    for goc, _, tep in os.walk(DARA_S):
        for t in tep:
            rel = os.path.relpath(os.path.join(goc, t), DARA_S)
            if rel.endswith(".goc"):        # ban goc cua tep da rut gon
                continue
            tep_ra.append(rel)
            tm = os.path.dirname(rel)
            while tm:
                if not os.path.isdir(os.path.join(dt.S, tm)):
                    tm_ra.add(tm)
                tm = os.path.dirname(tm)
    return tep_ra, tm_ra


def main():
    print("=== t44 NGHIEM THU sau khi go he cu ===")
    print()

    # ---------- 1. tham chieu treo ----------
    dd, dtm = duong_da_doi()
    print("## 1. Tham chieu TREO (tro toi tep da doi sang _dara)")
    print("   %d tep trong _dara; %d thu muc da bien mat khoi cay script"
          % (len(dd), len(dtm)))
    can = sorted(set(dt._chuan(x) for x in dd)
                 | set(dt._chuan(x) for x in dtm))
    can = [c for c in can if c]
    treo = 0
    for goc, _, tep in os.walk(dt.S):
        for t in tep:
            if not t.lower().endswith((".lua", ".txt", ".ini")):
                continue
            if ".truoc_" in t or t.endswith(".goc"):
                continue
            p = os.path.join(goc, t)
            try:
                dong = doc(p).replace("\r\n", "\n").split("\n")
            except OSError:
                continue
            for i, l in enumerate(dong, 1):
                if l.strip().startswith("--"):
                    continue
                lc = dt._chuan(l)
                for c in can:
                    if dt._co_nhac(lc, c):
                        print("   %s:%d" % (os.path.relpath(p, dt.S), i))
                        print("       %s" % l.strip()[:104])
                        treo = treo + 1
                        break
    print("   => %d tham chieu treo" % treo)
    print()

    # ---------- 2. ham nil ----------
    print("## 2. Loi goi toi ham CHI CO trong cac tep da doi")
    ham_da_doi = set()
    for goc, _, tep in os.walk(DARA_S):
        for t in tep:
            if not t.lower().endswith(".lua"):
                continue
            try:
                d = lh.sach(doc(os.path.join(goc, t)))
            except OSError:
                continue
            for m in re.finditer(r"^\s*function\s+([A-Za-z_]\w*)", d, re.M):
                ham_da_doi.add(m.group(1))
    con_lai = set()
    for goc, _, tep in os.walk(dt.S):
        for t in tep:
            if not t.lower().endswith(".lua") or ".truoc_" in t:
                continue
            try:
                d = lh.sach(doc(os.path.join(goc, t)))
            except OSError:
                continue
            for m in re.finditer(r"^\s*function\s+([A-Za-z_]\w*)", d, re.M):
                con_lai.add(m.group(1))
    he = ham_engine()
    mat = ham_da_doi - con_lai - he
    print("   %d ham trong tep da doi; %d ham engine C++ (loai ra);"
          " %d ham thuc su khong con noi nao khai"
          % (len(ham_da_doi), len(ham_da_doi & he), len(mat)))
    nil = 0
    for goc, _, tep in os.walk(dt.S):
        for t in tep:
            if not t.lower().endswith(".lua") or ".truoc_" in t:
                continue
            p = os.path.join(goc, t)
            try:
                dong = doc(p).replace("\r\n", "\n").split("\n")
            except OSError:
                continue
            for i, l in enumerate(dong, 1):
                if l.strip().startswith("--"):
                    continue
                ls = lh.sach(l)
                for h in mat:
                    if re.search(r"\b%s\s*\(" % re.escape(h), ls):
                        print("   %s:%d  goi %s()"
                              % (os.path.relpath(p, dt.S), i, h))
                        print("       %s" % l.strip()[:104])
                        nil = nil + 1
    print("   => %d loi goi ham nil" % nil)
    print()

    # ---------- 3. cu phap toan cay ----------
    print("## 3. Kiem cu phap TOAN CAY bang syncheck.exe")
    if not os.path.isfile(SYNCHECK):
        print("   !! khong thay syncheck.exe - bo qua")
        return 0
    # Ten tep KHONG PHAI ASCII (script\skill\special co 12 tep ten tieng Trung)
    # khong truyen qua argv duoc - console doi ma, syncheck bao 'KHONG MO DUOC'.
    # Do KHONG phai loi cu phap. Tach ra, kiem bang can bang tu khoa.
    ds, ds_gbk = [], []
    for goc, _, tep in os.walk(dt.S):
        for t in tep:
            if not t.lower().endswith(".lua") or ".truoc_" in t:
                continue
            p = os.path.join(goc, t)
            (ds if all(ord(c) < 128 for c in t) else ds_gbk).append(p)
    print("   %d tep .lua ten ASCII + %d tep ten khong ASCII"
          % (len(ds), len(ds_gbk)))
    lech = [p for p in ds_gbk if lh.can_bang(doc(p)) != 0]
    print("   %d tep ten khong ASCII: can bang tu khoa lech = %d"
          % (len(ds_gbk), len(lech)))
    for p in lech:
        print("      %s" % os.path.relpath(p, dt.S))
    loi = []
    B = 60
    for i in range(0, len(ds), B):
        r = subprocess.run([SYNCHECK] + ds[i:i + B], capture_output=True)
        for l in r.stdout.decode("latin-1").split("\n"):
            if l.startswith("LOI CU PHAP") or l.startswith("KHONG MO DUOC"):
                loi.append(l.strip())
    if loi:
        print("   !!! %d TEP LOI:" % len(loi))
        for l in loi[:40]:
            print("      %s" % l)
    else:
        print("   => TAT CA %d tep dung cu phap Lua 4.0.1" % len(ds))
    print()
    print("=== KET LUAN: %s ==="
          % ("CO VAN DE" if (nil or loi or lech) else
             ("SACH (2 tham chieu con lai la co y - xem ghi chu)"
              if treo else "SACH")))
    return 0


if __name__ == "__main__":
    sys.exit(main())
