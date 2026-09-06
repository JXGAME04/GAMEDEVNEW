# -*- coding: utf-8 -*-
"""p06_chuyen_sharedata.py  [RELAYHT 06/09]

Chuyen kho ShareData CUC BO cua GameServer sang kho CHUNG cua S3Relay.

Truoc day GameServer luu ShareData ra tung tep:
    bin\\server\\settings\\jx2sharedata\\<KHOA>_<p1>_<p2>.bin
(xem Core\\Src\\KJx2SharedStore.cpp ham sShareDataPath). Noi dung tep la NGUYEN KHOI
byte cua ObjBuffer -- dung y het thu ma bang MySQL relay_sharedata luu o cot sdata.
Nen chuyen chi la doc tep roi chen thang, khong phai dich gi.

Chay:
    python p06_chuyen_sharedata.py            # xem truoc, KHONG ghi
    python p06_chuyen_sharedata.py --ghi      # ghi that vao MySQL
    python p06_chuyen_sharedata.py --ghi --loc MISSION_tongcastle

Doc cau hinh MySQL tu bin\\multiserver\\DataBase.ini muc [relaydb] -- dung cho ma
S3Relay dang dung, khong go tay mat khau.
"""
import io
import os
import re
import sys

try:
    import pymysql
except ImportError:
    print("Thieu pymysql: pip install pymysql")
    sys.exit(1)

THU_MUC_BIN = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\jx2sharedata"
INI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\multiserver\DataBase.ini"
BANG = "relay_sharedata"


def doc_ini():
    cfg, trong_muc = {}, False
    for ln in io.open(INI, encoding="latin-1"):
        s = ln.strip()
        if s.startswith("["):
            trong_muc = s.lower() == "[relaydb]"
            continue
        if not trong_muc or "=" not in s or s.startswith(";"):
            continue
        k, _, v = s.partition("=")
        cfg[k.strip().lower()] = v.strip()
    return cfg


def tach_ten(ten_tep):
    """<KHOA>_<p1>_<p2>.bin -> (khoa, p1, p2). Tra None neu khong dung khuon."""
    m = re.match(r"^(.*)_(\d+)_(\d+)\.bin$", ten_tep)
    if not m:
        return None
    return m.group(1), int(m.group(2)), int(m.group(3))


def main():
    ghi = "--ghi" in sys.argv
    loc = None
    if "--loc" in sys.argv:
        loc = sys.argv[sys.argv.index("--loc") + 1]

    if not os.path.isdir(THU_MUC_BIN):
        print("Khong thay thu muc %s -- khong co gi de chuyen." % THU_MUC_BIN)
        return

    ds = []
    for f in sorted(os.listdir(THU_MUC_BIN)):
        if not f.lower().endswith(".bin"):
            continue
        t = tach_ten(f)
        if not t:
            print("  BO QUA (ten khong dung khuon): %s" % f)
            continue
        khoa, p1, p2 = t
        if loc and khoa != loc:
            continue
        data = io.open(os.path.join(THU_MUC_BIN, f), "rb").read()
        ds.append((khoa, p1, p2, data))
        print("  %-40s khoa=%-24s p1=%-4d p2=%-6d %d byte" % (f, khoa, p1, p2, len(data)))

    if not ds:
        print("Khong co ban ghi nao khop.")
        return

    if not ghi:
        print("\n%d ban ghi se duoc chen. Chay lai voi --ghi de ghi that." % len(ds))
        return

    cfg = doc_ini()
    conn = pymysql.connect(
        host=cfg.get("server", "127.0.0.1"),
        port=int(cfg.get("port", 3306)),
        user=cfg.get("user", "root"),
        password=cfg.get("password", ""),
        database=cfg.get("database", "jx1_role"),
    )
    cur = conn.cursor()
    n_moi, n_de = 0, 0
    for khoa, p1, p2, data in ds:
        cur.execute("SELECT LENGTH(sdata) FROM " + BANG + " WHERE skey=%s AND p1=%s AND p2=%s",
                    (khoa.encode("latin-1"), p1, p2))
        co = cur.fetchone()
        cur.execute("REPLACE INTO " + BANG + " (skey,p1,p2,sdata) VALUES (%s,%s,%s,%s)",
                    (khoa.encode("latin-1"), p1, p2, data))
        if co:
            n_de += 1
        else:
            n_moi += 1
    conn.commit()
    conn.close()
    print("\nXONG: chen moi %d, ghi de %d." % (n_moi, n_de))
    print("Kiem: SELECT skey,p1,p2,LENGTH(sdata) FROM %s;" % BANG)


if __name__ == "__main__":
    main()
