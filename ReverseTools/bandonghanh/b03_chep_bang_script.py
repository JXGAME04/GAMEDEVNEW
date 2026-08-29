# -*- coding: utf-8 -*-
"""BAN DONG HANH - G1 buoc 3: chep bang cau hinh + script tu cay Linux sang server/client.

Chep BYTE-EXACT (khong doi ma hoa - bang so + GBK/TCVN giu nguyen; engine ta viet
se doc dung dinh dang goc):
  server:
    settings\\partner\\**            (11 bang engine doc + talk\\ + skill_requirement)
    settings\\task\\partner_task_def.txt
    settings\\task\\partner\\**      (problem + reward)
    script\\skill\\partner\\**       (74 tep - skills.txt JX1 DA tro san duong nay)
    script\\partner\\*.lua           (4 tep hook engine: talk/levelup_sv/levelup_cl/action)
  client (theo dung bang chung pak VLTK client doc):
    settings\\partner\\partner_bag.ini, aptitude_mode.txt, feature.txt
    settings\\task\\partner\\problem\\partner_allproblem.txt
    settings\\task\\partner\\reward\\index_taskid.txt, reward_allprize.txt
Idempotent. Bao cao tung tep.
"""
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

SEP = "\\"
LIN = r"D:\ServerLinux\server1"
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"

DIRS_SRV = [
    r"settings\partner",
    r"settings\task\partner",
    r"script\partner",
]
# script\skill\partner: cay D:\ServerLinux chi co thu muc RONG; 75 tep that lay tu
# J:\jx-thiendieu-x64 - DA KIEM 75/75 trung byte voi pak client VLTK (xcheck_skill.py)
THIENDIEU_SKILL = r"J:\jx-thiendieu-x64\bin\server\script\skill\partner"
FILES_SRV = [
    r"settings\task\partner_task_def.txt",
]
FILES_CLI = [
    r"settings\partner\partner_bag.ini",
    r"settings\partner\aptitude_mode.txt",
    r"settings\partner\feature.txt",
    r"settings\task\partner\problem\partner_allproblem.txt",
    r"settings\task\partner\reward\index_taskid.txt",
    r"settings\task\partner\reward\reward_allprize.txt",
]


def copy_one(src, dst):
    data = open(src, "rb").read()
    if os.path.isfile(dst) and open(dst, "rb").read() == data:
        return 0
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    with open(dst, "wb") as f:
        f.write(data)
    return 1


def disp(rel):
    try:
        return rel.encode("latin-1").decode("gbk")
    except Exception:
        return rel


def main():
    n_all = n_new = 0
    for d in DIRS_SRV:
        base = os.path.join(LIN, d)
        if not os.path.isdir(base):
            print("!!! THIEU nguon:", d)
            continue
        cnt = 0
        for dp, dns, fns in os.walk(base):
            for fn in fns:
                src = os.path.join(dp, fn)
                rel = os.path.relpath(src, LIN)
                dst = os.path.join(SRV, rel)
                n_new += copy_one(src, dst)
                cnt += 1
        n_all += cnt
        print("dir %-28s: %3d tep" % (disp(d), cnt))
    # skill\partner tu cay thiendieu (nguon da kiem chung)
    cnt = 0
    for dp, dns, fns in os.walk(THIENDIEU_SKILL):
        for fn in fns:
            src = os.path.join(dp, fn)
            rel = os.path.join(r"script\skill\partner",
                               os.path.relpath(src, THIENDIEU_SKILL))
            n_new += copy_one(src, os.path.join(SRV, rel))
            cnt += 1
    n_all += cnt
    print("dir script\\skill\\partner (thiendieu): %d tep" % cnt)
    for rel in FILES_SRV:
        n_new += copy_one(os.path.join(LIN, rel), os.path.join(SRV, rel))
        n_all += 1
        print("srv %s" % disp(rel))
    for rel in FILES_CLI:
        n_new += copy_one(os.path.join(LIN, rel), os.path.join(CLI, rel))
        n_all += 1
        print("cli %s" % disp(rel))
    print("TONG: %d tep, ghi moi %d" % (n_all, n_new))


if __name__ == "__main__":
    main()
