# -*- coding: utf-8 -*-
"""deploy.py [VTCN 06/09] - kiem dau hieu tinh nang trong 3 nhi phan vua build roi dat .moi.
Server: .moi dang cho cua phien khac duoc DOI TEN (khong xoa) thanh .moi.<nhan>_<sha>_<hhmm>.
Chay: python deploy.py [--dry]
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
mk_vt = unicode_to_tcvn3_bytes("Long Môn Tiêu Cục - vận tiêu cá nhân")
mk_msg = unicode_to_tcvn3_bytes("[Chỉ nam] Đã hủy dẫn đường vận tiêu")
chk = {
  srv:  ["CreateBiaoChe", "BC_SetEnable", "AUC_MsgTong", "CL_Cong", "UpdateBattleInfo", "st3_goboss", "Lua54Dll.dll", "vt_goto_canhan", "vt_chinam.lua", "[RELAYHT]", "LUA_CALL"],
  cli:  ["vt_goto_canhan", "Lua54Dll.dll", mk_msg, "st3_goboss"],
  game: ["NewTask", "vt_quit_canhan", "vt_quit_bang", mk_vt, "tg_quit", "st3_quit"],
}
ok = True
for p, ms in chk.items():
    print("==", os.path.basename(p), "sha", sha(p), "size", os.path.getsize(p), "mtime", time.strftime("%H:%M", time.localtime(os.path.getmtime(p))))
    for m in ms:
        c = cnt(p, m)
        print("   %-40s %d" % (m if isinstance(m, str) else m.decode("latin-1"), c))
        if c == 0:
            ok = False
# Lua54Dll: ban trong Lib cua worktree phai TRUNG ban .moi dang cho (cung pairing voi phien LUA54)
for lib, moi in ((r"Lib\lua54\x64\Lua54Dll.dll", os.path.join(SRV, "Lua54Dll.dll.moi")), (r"Lib\lua54\Win32\Lua54Dll.dll", os.path.join(CLI, "Lua54Dll.dll.moi"))):
    a = os.path.join(WT, lib)
    if os.path.isfile(moi):
        print("Lua54Dll pairing:", lib, sha(a), "vs .moi", sha(moi), "KHOP" if sha(a) == sha(moi) else "KHAC")
    else:
        print("Lua54Dll pairing:", lib, sha(a), "(khong co .moi cho)")
if not ok:
    sys.exit("THIEU DAU HIEU - KHONG DEPLOY")
if DRY:
    sys.exit("(dry) khong dat")
old = os.path.join(SRV, "CoreServer.dll.moi")
if os.path.isfile(old):
    dst = os.path.join(SRV, "CoreServer.dll.moi.lua54b_%s_%s" % (sha(old), time.strftime("%H%M", time.localtime(os.path.getmtime(old)))))
    if not os.path.exists(dst):
        os.rename(old, dst)
        print("doi ten .moi cu ->", os.path.basename(dst))
shutil.copyfile(srv, old); print("dat", old, sha(old))
for src, name in ((game, "Game.exe.moi"), (cli, "CoreClient.dll.moi")):
    d = os.path.join(CLI, name)
    if os.path.exists(d):
        dst = d + ".phienkhac_%s" % sha(d)
        os.rename(d, dst); print("doi ten", name, "cua phien khac ->", os.path.basename(dst))
    shutil.copyfile(src, d); print("dat", d, sha(d))
