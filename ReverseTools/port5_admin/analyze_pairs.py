# -*- coding: utf-8 -*-
r"""Phan tich 8 cap task trung: dem so CHO DUNG cua tung ben de quyet dinh doi ben nao."""
import io, os, re

E = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"

# (task, ten hang ben A, ten hang ben B)  - dem theo TEN HANG, va theo so tran
PAIRS = [
    (300, ["T_SAVE_TK_KILLPLAYER"], ["MS_LEAVEWORLD"]),
    (301, ["T_SAVE_TK_KILLNPC"],    ["MS_LEAVEPOSX"]),
    (302, ["T_SAVE_TK_DEATH"],      ["MS_LEAVEPOSY"]),
    (57,  ["TSK_NV_THIENVUONG"],    []),
    (53,  ["TASK_DUNGCHUNG3"],      ["TSK_HAI_QUA_HH_NGAY"]),
    (12,  ["TASK_DUNGCHUNG2"],      ["T_TIMDOCHI"]),
    (11,  ["TASK_DUNGCHUNG"],       ["T_TAYTUY_TYPE"]),
    (44,  ["TASK_NVTHONGTHUONG"],   ["T_REPCAUHOI"]),
    (17,  [],                       ["T_USEHATHKIM"]),
    (21,  ["TMP_BAYTK"],            []),
]

def scan(names, num):
    """tra ve (so_file, so_lan, vai file dai dien) cho tung ten hang + so tran"""
    res = {}
    pats = {n: re.compile(r"\b" + re.escape(n) + r"\b") for n in names}
    pats["<so tran %d>" % num] = re.compile(r"(?:SetTask|GetTask|SetTaskTemp|GetTaskTemp|nt_setTask|nt_getTask|SetByte|GetByte|SetNumber|GetNumber)\s*\(\s*%d\b" % num)
    for k in pats: res[k] = [0, 0, []]
    for dp, _, fs in os.walk(E):
        for f in fs:
            if not f.lower().endswith(".lua") or f.lower() in ("codenew.lua",): continue
            p = os.path.join(dp, f)
            try: s = io.open(p, "r", encoding="latin-1", newline="").read()
            except Exception: continue
            body = "\n".join(l for l in s.split("\n") if not l.lstrip().startswith("--"))
            for k, rx in pats.items():
                c = len(rx.findall(body))
                if c:
                    res[k][0] += 1; res[k][1] += c
                    if len(res[k][2]) < 2:
                        res[k][2].append(p.replace(E + "\\", ""))
    return res

for num, A, B in PAIRS:
    print("\n=== TASK %d ===" % num)
    r = scan(A + B, num)
    for k, (nf, nc, ex) in sorted(r.items(), key=lambda x: -x[1][1]):
        if nc == 0: continue
        print("   %-24s %2d tep / %3d lan   %s" % (k, nf, nc, " | ".join(ex)))
