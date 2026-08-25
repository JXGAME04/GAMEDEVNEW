# -*- coding: utf-8 -*-
"""B1 — CHÉP nguyên byte các tệp Lua + bảng dữ liệu từ bản Linux sang máy chủ JX1.

Nguồn : D:\\ServerLinux\\server1  (gốc A)  +  D:\\ServerLinux\\Patch (gốc B, bảng toạ độ PLD)
Đích  : E:\\SourceTuanLe\\SourceVs22\\TESTLOFFF_ONLINE\\bin\\server  (cây sống)
Gương : D:\\GAMEDEVNEW\\serverscript_jx2\\3hoatdong  (repo, nguồn sự thật để git)

LUẬT:
  * KHÔNG BAO GIỜ đè tệp đã có ở đích (kể cả DIVERGED/NEAR/IDENTICAL) — chỉ chép MISSING.
  * lib/ và tong/ KHÔNG chép vào script\\ (engine tự remap sang scriptjx2\\lib +
    scriptjx2\\tong_vn — sJX2RemapScriptPath, ScriptFuns.cpp:1938); nếu scriptjx2
    cũng thiếu thì chép vào scriptjx2.
  * Chép NGUYÊN BYTE (GBK+TCVN3 giữ nguyên) — mọi chỉnh sửa để B2 làm.
Chạy: python b1_copy.py [--apply]   (mặc định: chỉ liệt kê)
"""
import io, os, sys, json, shutil

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
APPLY = "--apply" in sys.argv

HERE = os.path.dirname(os.path.abspath(__file__))
P3 = os.path.dirname(HERE)
LNX_A = r"D:\ServerLinux\server1"
LNX_B = r"D:\ServerLinux\Patch"
JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIRROR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"

stats = {"copy": 0, "skip_exist": 0, "skip_lib": 0, "miss_src": 0}
manifest = []


def copy_one(src, dst_rel):
    """src tuyệt đối -> JX1\\dst_rel + MIRROR\\dst_rel. Trả True nếu chép."""
    dst = os.path.join(JX1, dst_rel)
    if os.path.exists(dst):
        stats["skip_exist"] += 1
        return False
    if not os.path.isfile(src):
        stats["miss_src"] += 1
        print("  !! MAT NGUON:", src)
        return False
    if APPLY:
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        shutil.copyfile(src, dst)
        mdst = os.path.join(MIRROR, dst_rel)
        os.makedirs(os.path.dirname(mdst), exist_ok=True)
        shutil.copyfile(src, mdst)
    stats["copy"] += 1
    manifest.append(dst_rel)
    return True


def main():
    cj = json.load(io.open(os.path.join(P3, "closure3.json"), encoding="utf-8"))
    done = set()

    # ---- 1) bao đóng Include (3 tính năng) ----
    for feat, rows in cj.items():
        for r in rows:
            rel = r["rel"].replace("/", "\\")
            if rel.lower() in done:
                continue
            done.add(rel.lower())
            if not r["linux"]:
                continue        # global\路人_礼官.lua — không có cả ở Linux
            if r["in_jx1"]:
                continue        # đã có (kể cả DIVERGED — giữ bản JX1)
            src = os.path.join(LNX_A, "script", rel)
            low = rel.lower()
            if low.startswith("lib\\") or low.startswith("tong\\"):
                # đường remap: script\lib -> scriptjx2\lib ; script\tong -> scriptjx2\tong_vn
                sub = "scriptjx2\\lib\\" + rel[4:] if low.startswith("lib\\") \
                    else "scriptjx2\\tong_vn\\" + rel[5:]
                if os.path.exists(os.path.join(JX1, sub)):
                    stats["skip_lib"] += 1
                    continue
                copy_one(src, sub)
            else:
                copy_one(src, "script\\" + rel)

    # ---- 2) tầng LÕI phát hiện ở vòng 2 (gọi lúc chạy) ----
    extra = [
        "script\\global\\thanh\\npc\\add_npc.lua",
        "script\\global\\thanh\\npc\\bosssatthudeath.lua",
        "script\\global\\thanh\\npc\\bosssatthuhead.lua",
        "script\\global\\thanh\\npc\\npc_chuyensinh.lua",
        "script\\global\\thanh\\npc\\npc_hotro.lua",
        "script\\global\\thanh\\npc\\npcdeath.lua",
        "script\\huoyuedu\\huoyuedu.lua",
        # callbossdeathmini.lua: boss.lua:30/37 SetNpcDeathScript tro toi nhung
        # tep KHONG TON TAI o ca ban Linux (closure3_runtime linux=False) ->
        # boss 511/513 tren thuyen von khong co script chet rieng. Giu nguyen.
        "script\\activitysys\\config\\41\\extend.lua",
        "script\\vng_feature\\challengeoftime\\npcnhiepthitran.lua",
        "script\\vng_feature\\double_mission_award.lua",
        "settings\\trigger_challengeoftime.lua",
        "settings\\trigger_include.lua",
    ]
    for rel in extra:
        copy_one(os.path.join(LNX_A, rel), rel)

    # ---- 3) bảng dữ liệu settings (gốc A) ----
    tables_a = [
        "settings\\task\\tollgate\\killer\\killer.txt",
        "settings\\huoyuedu\\huoyuedu.txt",
    ]
    for lv in (20, 30, 40, 50, 60, 70, 80, 90):
        tables_a.append("settings\\droprate\\boss\\bosstask_lev%d.ini" % lv)
    for n in (8, 16, 20, 24, 32, 40, 56):
        tables_a.append("settings\\maps\\challengeoftime\\lineup%d.txt" % n)
    for rel in tables_a:
        copy_one(os.path.join(LNX_A, rel), rel)

    # ---- 4) bảng toạ độ PLD (gốc B, tên thư mục GBK — chép theo BYTE tên) ----
    # ánh xạ mojibake (đọc qua mbcs của Windows) -> đường dẫn thật
    gbk_files = []
    b_maps = os.path.join(LNX_B, "settings", "maps")
    for dp, dn, fs in os.walk(b_maps):
        for f in fs:
            full = os.path.join(dp, f)
            rel = os.path.relpath(full, LNX_B)
            try:
                raw = rel.encode("mbcs")
            except Exception:
                raw = rel.encode("latin-1", "replace")
            # 中原北区 = d6d0 d4ad b1b1 c7f8 ; chỉ lấy nhánh đó
            if b"\xd6\xd0\xd4\xad\xb1\xb1\xc7\xf8" in raw:
                gbk_files.append(rel)
    for rel in sorted(gbk_files):
        copy_one(os.path.join(LNX_B, rel), rel)

    print("== B1 %s ==" % ("APPLY" if APPLY else "(xem truoc)"))
    for k, v in stats.items():
        print("  %-12s %d" % (k, v))
    if APPLY:
        with io.open(os.path.join(HERE, "b1_manifest.txt"), "w", encoding="utf-8") as f:
            f.write("\n".join(manifest))
        print("  manifest -> b1_manifest.txt (%d tep)" % len(manifest))


if __name__ == "__main__":
    main()
