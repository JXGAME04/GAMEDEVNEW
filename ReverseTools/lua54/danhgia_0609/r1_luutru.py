import os, shutil, sys, time
BIN = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin"
SV = os.path.join(BIN, "server"); CL = os.path.join(BIN, "client")
LUU = os.path.join(BIN, "_luutru", "0609")
os.makedirs(LUU, exist_ok=True)
log = []
def mv(src, dst_rel):
    if not os.path.exists(src):
        log.append("BO QUA (khong co): " + src); return
    dst = os.path.join(LUU, dst_rel)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    if os.path.exists(dst):
        dst = dst + ".%d" % int(time.time())
    shutil.move(src, dst)
    log.append("DA CHUYEN: %s -> %s" % (src, dst))
# backup dirs trong bin\server
for d in ["_backup_bossbanghoi_2108", "_backup_loidai_2108", "_backup_tinsu_2108", "_backup_vantieu_20260905",
          "_backup_wlls_20260820", "_moi_backup", "_x86_khong_nap_duoc_2008", "_dara", "_gobo_lien_dau_20260820",
          "script_go_bo_3hd"]:
    mv(os.path.join(SV, d), os.path.join("server", d))
# duong lui Lua 4 (chu bo)
for f in ["script.lua4", "scriptjx2.lua4", "LuiLua4.bat", "lualibdll.dll", "lualibdll.exp", "lualibdll.lib",
          "lualibdll.map", "lualibdll.pdb", "ChayGameServer.bat.truoc", "ChayGameServer.bat.v2_1214"]:
    mv(os.path.join(SV, f), os.path.join("server", "lua4", f))
for f in ["script.lua4", "lualibdll.dll", "LuiLua4.bat"]:
    mv(os.path.join(CL, f), os.path.join("client", "lua4", f))
# thu muc rong
for d in ["ClientDebug", "ServerDebug"]:
    p = os.path.join(SV, d)
    if os.path.isdir(p) and not os.listdir(p):
        os.rmdir(p); log.append("XOA THU MUC RONG: " + p)
# ban luu trong settings / Maps (chi tep .truoc* .goc .cu_* .bak, khong dung tep dang dung)
n = 0
for top in ("settings", "Maps"):
    for dp, dn, fn in os.walk(os.path.join(SV, top)):
        for f in fn:
            fl = f.lower()
            if ".truoc" in fl or fl.endswith(".goc") or ".cu_" in fl or fl.endswith(".bak") or fl.endswith(".cu"):
                src = os.path.join(dp, f)
                rel = os.path.relpath(src, SV)
                mv(src, os.path.join("server", rel)); n += 1
log.append("tep ban luu settings/Maps da chuyen: %d" % n)
open(os.path.join(LUU, "NHATKY_LUUTRU_0609.txt"), "a", encoding="utf-8").write("\n".join(log) + "\n")
print("\n".join(log[-12:]))
print("TONG dong nhat ky:", len(log))
