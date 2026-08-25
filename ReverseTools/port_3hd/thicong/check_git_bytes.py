# -*- coding: ascii -*-
"""So BLOB da stage voi TEP TREN DIA cho cac tep quan trong.
Muc tieu: xac nhan git CHI doi xuong dong (CRLF->LF) chu KHONG doi byte cao
(TCVN3 tieng Viet / GBK chu Han). Neu so byte cao lech => CORRUPT => dung commit.
"""
import subprocess, os, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
R = r"D:\GAMEDEVNEW"

paths = [
    "Sources/Core/Src/KJx2WarInfra.cpp",
    "Sources/Core/Src/ScriptFuns.cpp",
    "Sources/Core/Src/KSortScript.cpp",
    "serverscript_jx2/3hoatdong/script/item/hd3_admin.lua",
    "serverscript_jx2/3hoatdong/script/missions/fengling_ferry/fld_head.lua",
    "serverscript_jx2/3hoatdong/script/task/tollgate/killer/nieshichen.lua",
    "serverscript_jx2/3hoatdong/script/global/autoexec_npc_hd3.lua",
    "serverscript_jx2/3hoatdong/script/header/cauhinh_hoatdong.lua",
    "serverscript_jx2/3hoatdong/settings/TimerTask.txt",
]
# them vai bang GBK ten thu muc chu Han
for dp, dn, fs in os.walk(os.path.join(R, "serverscript_jx2", "3hoatdong", "settings", "maps")):
    for f in fs:
        rel = os.path.relpath(os.path.join(dp, f), R).replace(os.sep, "/")
        paths.append(rel)
        if len(paths) > 14:
            break
    if len(paths) > 14:
        break

bad = 0
print("%-62s %8s %8s %6s %6s %s" % ("tep", "disk", "blob", "hiDisk", "hiBlob", "ket luan"))
for p in paths:
    fp = os.path.join(R, p.replace("/", os.sep))
    if not os.path.isfile(fp):
        continue
    disk = open(fp, "rb").read()
    r = subprocess.run(["git", "-C", R, "show", ":" + p], capture_output=True)
    if r.returncode != 0:
        print("%-62s  (chua stage)" % p[-62:])
        continue
    blob = r.stdout
    hd = sum(1 for b in disk if b > 127)
    hb = sum(1 for b in blob if b > 127)
    same_hi = (hd == hb)
    only_eol = (disk.replace(b"\r\n", b"\n") == blob.replace(b"\r\n", b"\n"))
    verdict = "OK (chi xuong dong)" if (same_hi and only_eol) else "!!! LECH BYTE CAO"
    if not (same_hi and only_eol):
        bad += 1
    print("%-62s %8d %8d %6d %6d %s" % (p[-62:], len(disk), len(blob), hd, hb, verdict))

print()
print("So tep LECH BYTE CAO:", bad, "(0 = an toan commit)")
