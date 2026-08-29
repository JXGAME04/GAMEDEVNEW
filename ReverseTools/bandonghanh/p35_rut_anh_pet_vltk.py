# -*- coding: utf-8 -*-
r"""[PETSYS 29/08] Hinh pet khong hien vi client JX1 THIEU bo anh cac template
pet (chi co boss186/188 loose). VLTK pak co du theo dung cau truc
\spr\npcres\<nhom>\<res>\<file>. Rut:
1. feature.txt (server) -> tap template pet
2. npcs.txt (client) row tpl+1 (0-based) cot 12 -> res
3. bang settings\NpcRes\npcres_v4\<pho thong npc>.txt dong <res> -> danh sach file spr
4. bam id tung path, quet pak VLTK theo thu tu uu tien, rut ra loose client JX1
5. neu chi co "_st.spr" thi chep them ban "_st01.spr" (UiPet C dang ghep _st01)
"""
import glob
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
import pakdump as P  # noqa: E402
import ucl  # noqa: E402

SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
VL = r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky\data"

THU_TU = ["1024.pak", "serverlist.pak", "slistcache.pak"] + \
    ["updatejx%02d.pak" % i for i in range(17, 0, -1)] + \
    ["updatejx07_jxf06.pak", "update02.pak", "update.pak", "update04.pak",
     "update03.pak", "update01.pak", "skills.pak", "spr.pak", "resource.pak",
     "maps.pak", "settings.pak", "ui.pak", "script.pak", "slistfree.pak"]


def blob_of(f, e):
    u, off, size, cf = e
    cs = cf & 0xFFFFFF
    fl = cf >> 24
    if fl == 0x20:
        fl = 1
    fl &= 0x0F
    f.seek(off)
    raw = f.read(cs if cs else size)
    try:
        if fl == 0 or cs in (0, size):
            return raw[:size] if size > 0 else raw
        if fl == 1:
            return ucl.nrv2b_decompress_8(raw, size)
        import bz2
        import zlib
        return bz2.decompress(raw) if fl == 2 else zlib.decompress(raw)
    except Exception:
        return b""


# 1. tap template tu feature.txt
tpls = set()
for d in io.open(os.path.join(SV, r"settings\petsys\feature.txt"), "r",
                 encoding="latin-1").read().replace(chr(13), "").split("\n")[1:]:
    c = d.split("\t")
    if len(c) >= 3 and c[2].strip().isdigit():
        tpls.add(int(c[2]))
print("templates:", sorted(tpls))

# 2. res tu npcs.txt client
npcs = io.open(os.path.join(CL, r"settings\npcs.txt"), "r",
               encoding="latin-1").read().replace(chr(13), "").split("\n")
res_all = {}
for t in sorted(tpls):
    row = npcs[t + 1].split("\t") if t + 1 < len(npcs) else []
    if len(row) > 11 and row[11].strip():
        res_all[t] = row[11].strip()
print("res:", res_all)

# 3. danh sach file spr theo bang npc res (dong dau tien khop res)
bang = ""
for f in glob.glob(os.path.join(CL, r"settings\NpcRes\npcres_v4", "*.txt")):
    bang += io.open(f, "r", encoding="latin-1").read().replace(chr(13), "")


def files_of(res):
    # bang JX1 chi map res -> thu muc; ten file theo bo suffix chuan cua
    # NormalNpc (dong VLTK: st/wlk/bat/die/at1/at2). Sinh du bo pho quat.
    ra = set()
    for suf in ("_st", "_st01", "_wlk", "_bat", "_die", "_at1", "_at2",
                "_at3", "_run", "_sit", "_mgc"):
        ra.add(res + suf + ".spr")
    return ra


def nhom_of(res):
    i = 0
    while i < len(res) and not res[i].isdigit():
        i += 1
    return res[:i] if i else res


# 4. bam + rut
muon = {}
for t, res in res_all.items():
    fs = files_of(res)
    if not fs:
        fs = {res + "_st.spr", res + "_st01.spr"}
        print("!! khong thay trong bang npcres:", res, "- thu 2 ten mac dinh")
    for fn in fs:
        duong = "\\spr\\npcres\\%s\\%s\\%s" % (nhom_of(res), res, fn)
        muon[P.name2id(duong)] = duong

print("can rut", len(muon), "file")
da = 0
for pak in THU_TU:
    p = os.path.join(VL, pak)
    if not os.path.exists(p) or not muon:
        continue
    f, es = P.entries(p)
    for e in es:
        if e[0] in muon:
            d = blob_of(f, e)
            duong = muon.pop(e[0])
            ra = CL + duong
            os.makedirs(os.path.dirname(ra), exist_ok=True)
            if not os.path.exists(ra):
                open(ra, "wb").write(d)
            da += 1
    f.close()
print("rut duoc", da, "| thieu:", len(muon))
for v in list(muon.values())[:10]:
    print("  thieu:", v)

# 5. ban sao _st01 cho UiPet
for t, res in res_all.items():
    thu = os.path.join(CL, "spr", "npcres", nhom_of(res), res)
    st = os.path.join(thu, res + "_st.spr")
    st01 = os.path.join(thu, res + "_st01.spr")
    if os.path.exists(st) and not os.path.exists(st01):
        open(st01, "wb").write(open(st, "rb").read())
        print("chep them:", res + "_st01.spr")
print("XONG p35")
