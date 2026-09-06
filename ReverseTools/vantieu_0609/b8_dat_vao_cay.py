# -*- coding: ascii -*-
"""b8_dat_vao_cay.py -- [LMBC 06/09] BUOC B8: dat script van tieu vao cay chay that + moc khoi dong.

Lam gi:
  1. Chep cay script da chuyen Lua 5.4 (lua54\\script\\**) sang bin\\server\\script\\**
  2. Chep cac tep TU VIET (moi\\script\\**) sang bin\\server\\script\\**
  3. Chep bang tuyen (moi\\settings\\**) sang bin\\server\\settings\\**
  4. Moc lmbj_addnpc() vao script\\startgame.lua (Include + loi goi), theo dung loi nha JX1

KHONG lam gi voi van tieu CU (script\\event\\event_vantieu) - viec go he cu de BUOC RIENG,
lam cung luc voi khi bat he moi, de may chu khong mat tinh nang trong luc dang thi cong.

AN TOAN:
  - moi tep dich deu duoc sao luu .truoc_lmbc truoc khi de (chi tao lan dau)
  - KHONG ghi de tep dich NEU noi dung giong het (tranh doi mtime vo ich)
  - startgame.lua: kiem so byte cao khong doi, chi chen ASCII
  - in ra danh sach day du de doi chieu

dung: python b8_dat_vao_cay.py            # xem truoc, khong ghi
      python b8_dat_vao_cay.py --ghi      # ghi that
"""
import io
import os
import shutil
import sys

ROOT = r"D:\GAMEDEVNEW_wt_vantieu"
VT = os.path.join(ROOT, "serverscript_jx2", "vantieu")
LUA54 = os.path.join(VT, "lua54")
MOI = os.path.join(VT, "moi")
SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
GHI = "--ghi" in sys.argv
MARK = "[LMBC 06/09]"


def rd(p):
    with io.open(p, "r", encoding="latin-1", newline="") as f:
        return f.read()


def wr(p, s):
    with io.open(p, "w", encoding="latin-1", newline="") as f:
        f.write(s)


def hb(s):
    return sum(1 for c in s if ord(c) >= 0x80)


def chep_cay(goc, nhan, duoi):
    """Chep moi tep duoi 'goc' sang <SV>/<duoi>/..., giu nguyen cau truc thu muc.

    'duoi' la thu muc con cua cay chay that ("script" hoac "settings"). Bat buoc
    truyen tuong minh: lan dau viet ham nay da quen no va 49 tep roi thang vao
    bin\server\event, bin\server\activitysys... (engine khong nap, nhung
    startgame.lua thi da moc roi -> ScriptError luc boot).
    """
    n_moi = n_de = n_giong = 0
    for dp, _, fn in os.walk(goc):
        for f in fn:
            # bo qua tep phu tro cua bo cong cu.
            # LUU Y: engine chi nap tep co 4 ky tu cuoi la .LUA/.TXT nen ban sao luu
            # ".lua.truoc_*" khong bi chay, nhung van khong dua vao cay cho sach.
            if ".truoc" in f or f.endswith((".py", ".bak", ".json")):
                continue
            src = os.path.join(dp, f)
            rel = os.path.relpath(src, goc)
            dst = os.path.join(SV, duoi, rel)
            if os.path.isfile(dst):
                if open(src, "rb").read() == open(dst, "rb").read():
                    n_giong += 1
                    continue
                n_de += 1
                trang = "DE  "
            else:
                n_moi += 1
                trang = "MOI "
            if GHI:
                os.makedirs(os.path.dirname(dst), exist_ok=True)
                bak = dst + ".truoc_lmbc"
                if os.path.isfile(dst) and not os.path.isfile(bak):
                    shutil.copyfile(dst, bak)
                shutil.copyfile(src, dst)
            print("    %s %s" % (trang, rel))
    print("  [%s] moi=%d  de=%d  giong-san=%d" % (nhan, n_moi, n_de, n_giong))
    return n_moi + n_de


def moc_startgame():
    p = os.path.join(SV, "script", "startgame.lua")
    d = rd(p)
    if "lmbj_addnpc" in d:
        print("  startgame.lua: da moc tu truoc")
        return
    eol = "\r\n" if "\r\n" in d else "\n"
    before = hb(d)
    bs = chr(92)
    dd = bs * 2
    inc = 'Include("' + dd + 'script' + dd + 'startgame' + dd + 'lmbj_addnpc.lua")\t-- ' + MARK + ' NPC van tieu Long Mon Tieu Cuc'

    lines = d.split(eol)
    # 1) chen Include sau dong Include cuoi cung cua khoi dau
    last_inc = -1
    for i, l in enumerate(lines):
        if l.lstrip().startswith("Include("):
            last_inc = i
    assert last_inc >= 0, "startgame.lua: khong thay dong Include nao"
    lines.insert(last_inc + 1, inc)

    # 2) chen loi goi ngay sau mot loi goi addnpc san co (cung khoi khoi tao)
    goi = "\tlmbj_addnpc()\t-- " + MARK + " sinh 45 NPC van tieu (bang + ca nhan)"
    vt_goi = -1
    for i, l in enumerate(lines):
        s = l.strip()
        if s.startswith("tinsu_addnpc()") or s.startswith("bw_addnpc()"):
            vt_goi = i
    assert vt_goi >= 0, "startgame.lua: khong thay loi goi addnpc nao de bam vao"
    lines.insert(vt_goi + 1, goi)

    d2 = eol.join(lines)
    assert hb(d2) == before, "startgame.lua: so byte cao doi"
    assert all(ord(c) < 0x80 for c in inc + goi)
    print("  startgame.lua: se chen")
    print("     dong %d: %s" % (last_inc + 2, inc.strip()))
    print("     dong %d: %s" % (vt_goi + 2, goi.strip()))
    if GHI:
        bak = p + ".truoc_lmbc"
        if not os.path.isfile(bak):
            wr(bak, d)
        wr(p, d2)
        print("  startgame.lua: DA GHI")


def main():
    assert os.path.isdir(SV), SV
    print("che do:", "GHI THAT" if GHI else "XEM TRUOC (khong ghi gi)")
    print("-- 1. cay script da chuyen Lua 5.4 --")
    chep_cay(os.path.join(LUA54, "script"), "lua54", "script")
    print("-- 2. cac tep tu viet --")
    chep_cay(os.path.join(MOI, "script"), "moi/script", "script")
    print("-- 3. bang du lieu --")
    if os.path.isdir(os.path.join(MOI, "settings")):
        chep_cay(os.path.join(MOI, "settings"), "moi/settings", "settings")
    print("-- 4. moc vao startgame.lua --")
    moc_startgame()
    print("XONG" + ("" if GHI else "  (chua ghi gi - them --ghi de ap dung)"))


if __name__ == "__main__":
    main()
