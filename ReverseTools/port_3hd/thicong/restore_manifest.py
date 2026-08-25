# -*- coding: utf-8 -*-
"""Chép LẠI NGUYÊN BYTE từ bản Linux đè lên các tệp trong b1_manifest (cả JX1 lẫn
MIRROR) — để hoàn tác B2 sai rồi chạy lại B2 đúng.

Nguồn: gốc A cho script\\, gốc B cho settings\\maps\\<GBK>, gốc A cho settings còn lại.
lib/ và tong/ đã chép vào scriptjx2\\ (đường remap) nên khôi phục theo đúng đích ghi trong manifest.
"""
import io, os, sys, shutil
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

HERE = os.path.dirname(os.path.abspath(__file__))
LNX_A = r"D:\ServerLinux\server1"
LNX_B = r"D:\ServerLinux\Patch"
JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIRROR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"


def src_for(rel):
    r = rel.replace("/", os.sep)
    low = r.lower()
    # settings\maps\<GBK 中原北区> -> gốc B (byte)
    if low.startswith("settings" + os.sep + "maps" + os.sep):
        raw = r.encode("mbcs", "replace")
        if b"\xd6\xd0\xd4\xad\xb1\xb1\xc7\xf8" in raw:
            return os.path.join(LNX_B, r)
    # scriptjx2\lib\X -> Linux script\lib\X ; scriptjx2\tong_vn\X -> Linux script\tong\X
    if low.startswith("scriptjx2" + os.sep + "lib" + os.sep):
        return os.path.join(LNX_A, "script", "lib", r.split(os.sep, 2)[2])
    if low.startswith("scriptjx2" + os.sep + "tong_vn" + os.sep):
        return os.path.join(LNX_A, "script", "tong", r.split(os.sep, 2)[2])
    # script\... và settings\... còn lại -> gốc A
    return os.path.join(LNX_A, r)


def main():
    manifest = io.open(os.path.join(HERE, "b1_manifest.txt"), encoding="utf-8").read().split("\n")
    n = miss = 0
    for rel in manifest:
        rel = rel.strip()
        if not rel:
            continue
        src = src_for(rel)
        if not os.path.isfile(src):
            print("  !! MAT NGUON:", src)
            miss += 1
            continue
        for base in (JX1, MIRROR):
            dst = os.path.join(base, rel.replace("/", os.sep))
            os.makedirs(os.path.dirname(dst), exist_ok=True)
            shutil.copyfile(src, dst)
        n += 1
    print("khoi phuc", n, "tep (moi tep vao JX1 + MIRROR); mat nguon", miss)


if __name__ == "__main__":
    main()
