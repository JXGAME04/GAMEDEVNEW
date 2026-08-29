# -*- coding: utf-8 -*-
"""BAN DONG HANH - G1 buoc 2: rut anh UI tu image2.pak + chep 5 ini giao dien.

- 24 anh \\spr\\UI3\\同伴界面\\... + icon 同伴任务系统 nam trong E:\\jx1m_cdn\\data\\image2.pak
  (dinh dang PACK chuan PC, DUNG bo doc 16 byte/entry cua pakdump).
- 3 anh 同伴背包/同伴的物品栏 da co san trong update03.pak cua client JX1 -> khong rut.
- 5 tep ini + tep loose ghi vao cay client TESTLOFFF (tien le 投色子.ini dot Viem De:
  tep roi de len pak).
Idempotent: tep da co + dung kich thuoc thi bo qua.
"""
import os
import sys
import importlib.util

sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
import pakdump as P  # noqa: E402
import ucl  # noqa: E402
import zlib  # noqa: E402


def doc_entry(f, e):
    """pakdump.read_entry KHONG giai nen UCL (tra blob con nen) -> tu xu:
    XPACK_METHOD byte cao: 0 none, 1 UCL nrv2b, 2 bzip2."""
    uid, off, size, cf = e
    cs = cf & 0xFFFFFF
    fl = cf >> 24
    if fl == 0x20:                  # UCL 'Load Pak VNG' (XPackFile.cpp:258)
        fl = 1
    fl &= 0x0F
    f.seek(off)
    blob = f.read(cs if cs else size)
    if fl == 0 or cs in (0, size):
        return blob[:size] if size > 0 else blob
    if fl == 1:
        return ucl.nrv2b_decompress_8(blob, size)
    if fl == 2:
        import bz2
        return bz2.decompress(blob)
    return zlib.decompress(blob)

spec = importlib.util.spec_from_file_location(
    "pak_id", r"D:\GAMEDEVNEW\ReverseTools\viemde\pak_id.py")
pak_id = importlib.util.module_from_spec(spec)
spec.loader.exec_module(pak_id)
fid = pak_id.file_name_to_id

SEP = "\\"
CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
IMAGE2 = r"E:\jx1m_cdn\data\image2.pak"
PATCH_UI = r"D:\ServerLinux\Patch\ui\ui3_1024"


def g(s):
    return s.encode("gbk").decode("latin-1")


# ---- 25 anh phai rut tu image2.pak (duong dan trong pak = duong dan loose) ----
GIAODIEN = g("同伴界面")
PHIMTAT = g("同伴快捷栏")
SPRS = [
    SEP + "spr" + SEP + "UI3" + SEP + GIAODIEN + SEP + g(x) + ".spr"
    for x in ("标签1", "标签2", "标签3", "标签4", "标签5", "关闭", "解散", "切换",
              "同伴-技能", "同伴-人物", "同伴技能title", "同伴属性title", "选定", "遗忘")
] + [
    SEP + "spr" + SEP + "UI3" + SEP + GIAODIEN + SEP + PHIMTAT + SEP + g(x) + ".spr"
    for x in ("仅跟随", "同伴的基本属性", "同伴的武功技能", "同伴提示", "同伴选择",
              "与同伴对话", "召唤同伴", "主动攻击", "主界面按钮条改副本")
] + [
    SEP + "spr" + SEP + "Ui3" + SEP + g("同伴任务系统") + SEP + g("任务图标") + SEP + g("同伴教育") + ".spr",
]

# ---- 5 ini tu Patch (ban VNG 1024, da Viet hoa mot phan) ----
INIS = [g(x) + ".ini" for x in ("同伴属性", "同伴技能", "同伴技能树", "同伴背包", "同伴快捷栏")]


def save(dst, data):
    if os.path.isfile(dst) and os.path.getsize(dst) == len(data):
        return 0
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    with open(dst, "wb") as f:
        f.write(data)
    return 1


def main():
    f, es = P.entries(IMAGE2)
    idx = {e[0]: e for e in es}
    n_ok = n_moi = 0
    for rel in SPRS:
        u = fid(rel)
        if u not in idx:
            print("!!! THIEU trong image2.pak:", rel.encode("latin-1").decode("gbk", "replace"))
            continue
        data = doc_entry(f, idx[u])
        if not (len(data) > 8 and data[:4] == b"SPR\x00"):
            # spr chuan bat dau 'SPR\0'; van ghi nhung bao de soat
            print("  ? dinh dang la:", rel.encode("latin-1").decode("gbk", "replace"),
                  "4 byte dau:", data[:4])
        n_moi += save(CLI + rel, data)
        n_ok += 1
    f.close()
    print("anh: %d/%d rut duoc, ghi moi %d" % (n_ok, len(SPRS), n_moi))

    n_ini = 0
    for name in INIS:
        src = os.path.join(PATCH_UI, name)
        data = open(src, "rb").read()
        n_ini += save(CLI + SEP + "Ui" + SEP + "Ui3" + SEP + name, data)
        print("ini:", name.encode("latin-1").decode("gbk", "replace"), len(data), "byte")
    print("ini ghi moi:", n_ini)


if __name__ == "__main__":
    main()
