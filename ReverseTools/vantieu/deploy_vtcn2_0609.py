# -*- coding: utf-8 -*-
"""deploy2.py [VTCN 06/09 v2] - kiem dau hieu v2 (khong nhay map + chan map su kien) trong 3 nhi phan roi dat .moi.
Khe .moi dang trong (chu da swap 18:04/18:05); neu co .moi cua phien khac thi DOI TEN (khong xoa).
Chay: python deploy2.py [--dry]
"""
import hashlib, os, shutil, sys, time
sys.stdout.reconfigure(encoding="utf-8")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes
DRY = "--dry" in sys.argv
WT = r"D:\GAMEDEVNEW_wt_vtcn"
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"

def sha(p): return hashlib.sha256(open(p, "rb").read()).hexdigest()[:8]
def cnt(p, s): return open(p, "rb").read().count(s if isinstance(s, bytes) else s.encode("latin-1"))

srv = os.path.join(WT, r"Sources\Core\x64\ServerRelease\CoreServer.dll")
cli = os.path.join(WT, r"Sources\Core\ClientRelease\CoreClient.dll")
game = os.path.join(WT, r"Sources\S3Client\Release\Game.exe")
V = unicode_to_tcvn3_bytes
must = {
  srv:  ["CreateBiaoChe", "BC_SetEnable", "AUC_MsgTong", "CL_Cong", "UpdateBattleInfo", "st3_goboss", "Lua54Dll.dll", "vt_quit_canhan", "vt_quit_bang", "vt_chinam.lua", "[RELAYHT]", "LUA_CALL", "_duongdan_cu"],
  cli:  ["Lua54Dll.dll", "st3_goboss", V("[Chỉ nam] Đang ở map sự kiện"), V("thành thị đã đi qua"), V("Trấn Long Môn"), V("Đang chạy tới Xa Phu để đi")],
  game: ["NewTask", "vt_quit_canhan", "vt_quit_bang", V("Long Môn Tiêu Cục - vận tiêu cá nhân"), V("Không dẫn đường khi đang ở map sự kiện"), "tg_quit", "st3_quit"],
}
mustnot = {srv: ["vt_goto_canhan"], cli: ["vt_goto_canhan"], game: ["vt_goto_canhan"]}
ok = True
for p in (srv, cli, game):
    print("==", os.path.basename(p), "sha", sha(p), "size", os.path.getsize(p), "mtime", time.strftime("%H:%M", time.localtime(os.path.getmtime(p))))
    for m in must[p]:
        c = cnt(p, m)
        print("   %-44s %d" % (m if isinstance(m, str) else m.decode("latin-1"), c))
        if c == 0:
            ok = False
    for m in mustnot[p]:
        c = cnt(p, m)
        print("   KHONG DUOC CO %-30s %d" % (m, c))
        if c != 0:
            ok = False
for lib, live in ((r"Lib\lua54\x64\Lua54Dll.dll", os.path.join(SRV, "Lua54Dll.dll")), (r"Lib\lua54\Win32\Lua54Dll.dll", os.path.join(CLI, "Lua54Dll.dll"))):
    a = os.path.join(WT, lib)
    print("Lua54Dll pairing:", lib, sha(a), "vs live", sha(live), "KHOP" if sha(a) == sha(live) else "KHAC")
    if sha(a) != sha(live):
        ok = False
if not ok:
    sys.exit("THIEU/THUA DAU HIEU - KHONG DEPLOY")
if DRY:
    sys.exit("(dry) khong dat")
for src, d in ((srv, os.path.join(SRV, "CoreServer.dll.moi")), (game, os.path.join(CLI, "Game.exe.moi")), (cli, os.path.join(CLI, "CoreClient.dll.moi"))):
    if os.path.exists(d):
        dst = d + ".phienkhac_%s_%s" % (sha(d), time.strftime("%H%M", time.localtime(os.path.getmtime(d))))
        os.rename(d, dst)
        print("doi ten", os.path.basename(d), "cua phien khac ->", os.path.basename(dst))
    shutil.copyfile(src, d)
    print("dat", d, sha(d))
