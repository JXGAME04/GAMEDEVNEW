# -*- coding: ascii -*-
"""b6_hoantat.py -- [LMBC 06/09] BUOC B6: hoan tat phan con thieu truoc khi dat vao cay chay that.

Ba viec bat buoc ma cac mang truoc de lai:
  1. Them Include("\\script\\global\\yunbiao_system.lua") vao HAI tep script cua XE.
     LY DO: ba ham engine goi nguoc (OnBiaoCheDisapper / OnBiaoCheFarAwayPlayerDisapper /
     OnBiaoCheChangeMapNotice) duoc ban vao LUA STATE CUA SCRIPT XE (pC->m_ActionScriptID),
     KHONG phai state toan cuc. Dat ba ham o yunbiao_system.lua thoi la CHUA DU -
     script xe bat buoc phai Include no, neu khong ba goi nguoc roi vao hu khong.
  2. Them Include("\\script\\lib\\lib_lmbiaoche.lua") vao moi tep goi WriteYunBiaoLog.
     Thieu -> "attempt to call a nil value" ngay lan ghi nhat ky dau tien.
  3. Chep settings\event\longmenbiaoju\route.txt sang cay dich (bang 26 tuyen).
     Bay: cot 2 va 3 co DAU CACH dau/cuoi la THAT (khop tung byte voi tasknpc.lua) -> giu nguyen.

Chay lai duoc. Chi chen ASCII. So byte cao moi tep KHONG doi.
dung: python b6_hoantat.py [<goc worktree>]
"""
import io
import os
import shutil
import sys

ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_vantieu"
LUA54 = os.path.join(ROOT, "serverscript_jx2", "vantieu", "lua54")
MOI = os.path.join(ROOT, "serverscript_jx2", "vantieu", "moi")
LINUX_ROUTE = r"D:\ServerLinux\server1\settings\event\longmenbiaoju\route.txt"
MARK = "[LMBC 06/09]"


def rd(p):
    with io.open(p, "r", encoding="latin-1", newline="") as f:
        return f.read()


def wr(p, s):
    with io.open(p, "w", encoding="latin-1", newline="") as f:
        f.write(s)


def hb(s):
    return sum(1 for c in s if ord(c) >= 0x80)


def them_include(path, duong_dan_lua, ly_do):
    """Chen mot dong Include ngay sau khoi Include dau tep (hoac dau tep neu chua co)."""
    if not os.path.isfile(path):
        print("  BO QUA (khong co tep):", os.path.basename(path))
        return False
    d = rd(path)
    if duong_dan_lua in d:
        print("  da co Include:", os.path.basename(path), "->", duong_dan_lua)
        return False
    eol = "\r\n" if "\r\n" in d else "\n"
    before = hb(d)
    lines = d.split(eol)
    # tim dong Include cuoi cung trong 60 dong dau
    last = -1
    for i, l in enumerate(lines[:60]):
        if l.lstrip().startswith("Include(") or l.lstrip().startswith("IncludeLib("):
            last = i
    dong = 'Include("%s")\t-- %s %s' % (duong_dan_lua.replace("\\", "\\\\"), MARK, ly_do)
    assert all(ord(c) < 0x80 for c in dong)
    lines.insert(last + 1 if last >= 0 else 0, dong)
    d = eol.join(lines)
    assert hb(d) == before, "so byte cao doi: " + path
    bak = path + ".truoc_b6"
    if not os.path.isfile(bak):
        wr(bak, rd(path))
    wr(path, d)
    print("  da them Include vao:", os.path.basename(path), "->", duong_dan_lua)
    return True


def main():
    print("goc:", ROOT)

    # --- 1. script XE phai Include yunbiao_system (ba ham engine goi nguoc) ---
    print("-- 1. Include yunbiao_system.lua vao script XE --")
    for rel in (
        os.path.join("script", "activitysys", "config", "129", "npc_lmbiaoche.lua"),
        os.path.join("script", "event", "longmenbiaoju", "biaoche.lua"),
    ):
        them_include(os.path.join(LUA54, rel),
                     "\\script\\global\\yunbiao_system.lua",
                     "ba ham engine goi nguoc ban vao state CUA SCRIPT XE")

    # --- 2. moi tep goi WriteYunBiaoLog phai Include lib_lmbiaoche ---
    print("-- 2. Include lib_lmbiaoche.lua o moi tep goi WriteYunBiaoLog --")
    for dp, _, fn in os.walk(LUA54):
        for f in fn:
            if not f.endswith(".lua"):
                continue
            p = os.path.join(dp, f)
            d = rd(p)
            # chi tinh loi goi THAT (bo dong chu thich)
            co_goi = False
            for l in d.split("\n"):
                code = l.split("--")[0]
                if "WriteYunBiaoLog" in code:
                    co_goi = True
                    break
            if not co_goi:
                continue
            if "lib_lmbiaoche" in d:
                continue
            them_include(p, "\\script\\lib\\lib_lmbiaoche.lua",
                         "WriteYunBiaoLog")

    # --- 3. route.txt ---
    print("-- 3. bang tuyen route.txt --")
    dich_dir = os.path.join(MOI, "settings", "event", "longmenbiaoju")
    os.makedirs(dich_dir, exist_ok=True)
    dich = os.path.join(dich_dir, "route.txt")
    if os.path.isfile(dich):
        print("  da co:", dich)
    else:
        assert os.path.isfile(LINUX_ROUTE), LINUX_ROUTE
        shutil.copyfile(LINUX_ROUTE, dich)
        a = open(LINUX_ROUTE, "rb").read()
        b = open(dich, "rb").read()
        assert a == b, "chep route.txt khong khop byte"
        print("  da chep route.txt (%d byte, %d dong) - giu nguyen dau cach dau/cuoi cot"
              % (len(b), b.count(b"\n")))

    print("XONG B6")


if __name__ == "__main__":
    main()
