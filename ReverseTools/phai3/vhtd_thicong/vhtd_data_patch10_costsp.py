# -*- coding: utf-8 -*-
"""vhtd_data_patch10_costsp.py [VHTD 02/09s] - HA yeu cau Am Luat cua Tap Dap Luu Tinh (2118).

CHUA CHAY - cho chu duyet. Ly do: da kiem chung du lieu cua ta GIONG HET VLTK va o cap 20 ky nang chi doi 6 tang
(khong phai 10). Doi day la QUYET DINH CAN BANG, khong phai sua loi.

Cach lam: trong bang SKILLS.sataliuxing (xiaoyao.lua, CA server LAN client - hai tep dang giong het nhau, md5 bda7f656),
DOI CHO hai dong: bat dong VLTK da tu chu thich san `{{1,8},{20,3},{21,3}}` va chu thich dong dang dung `{{1,10},{30,4},{31,4}}`.
Ket qua: cap 1: 10 -> 8 | cap 10: 8 -> 5 | cap 20 (cap toi da cua 2118): 6 -> 3 | cap 30: 4 -> 3.

CANH BAO CAN BANG: ca ba chieu Tieu Dao an chung kho 10 tang Am Luat (+1 moi lan tich). Ha 2118 xuong 3 = NGANG
Thap Bo Nhat Sat (2129, co dinh 3), ma 2118 la chieu LUOT 280 don vi -> gan nhu luot lien tuc. Lac Nhan Binh Sa (2138)
co dinh 8 tang khong doi.

Sau khi chay: KHONG phai build, KHONG phai swap .moi (thuan du lieu Lua). Nhung PHAI restart GameServer va nguoi choi
thoat vao lai (doi tuong KSkill duoc cache theo tung cap).

DUNG: python vhtd_data_patch10_costsp.py --kiem   (thu, khong ghi)
      python vhtd_data_patch10_costsp.py          (ghi that)
      python vhtd_data_patch10_costsp.py --hoan-tac   (tra ve nhu cu)
"""
import io, os, sys, shutil, subprocess

KIEM = "--kiem" in sys.argv
HOANTAC = "--hoan-tac" in sys.argv
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
BIN = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin"
LUA4 = r"D:\GAMEDEVNEW\ReverseTools\lua4\lua4.exe"
BAK = ".truoc_costsp_0209"

DUNG = "\t\t\t{{1,10},{30,4},{31,4}}, -- \xb3\xd4\xbc\xb8\xb8\xf6 \r\n"          # dong dang dung
CHUTHICH = "\t\t\t--{{1,8},{20,3},{21,3}}, -- \xb3\xd4\xbc\xb8\xb8\xf6 \r\n"      # dong VLTK da chu thich

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()

def kiem_lua(path):
    """nap head.lua + tep vua sua bang lua4 va in cost_sp theo cap"""
    d = os.path.dirname(path)
    t = ('dofile("%s/head.lua")\ndofile("%s")\n' % (d.replace("\\", "/"), path.replace("\\", "/")) +
         "".join('print("cap %d -> " .. tostring(GetSkillLevelData("cost_sp","sataliuxing",%d)))\n' % (lv, lv)
                 for lv in (1, 10, 20, 21, 30)))
    tp = os.path.join(os.environ.get("TEMP", "."), "_t_costsp.lua")
    io.open(tp, "w", encoding="latin-1", newline="\n").write(t)
    r = subprocess.run([LUA4, "-s8192", tp], capture_output=True)
    os.remove(tp)
    if r.returncode != 0 or b"cap 1" not in r.stdout:
        raise SystemExit("lua4 BAO LOI sau khi sua - KHONG ghi:\n" + r.stderr.decode("latin-1")[:400])
    return r.stdout.decode("latin-1").strip()

def main():
    tu, sang = (CHUTHICH, DUNG) if HOANTAC else (DUNG, CHUTHICH)
    tu_moi = sang.replace("--{{", "{{") if HOANTAC else DUNG.replace("{{", "--{{")
    for side in ("server", "client"):
        p = os.path.join(BIN, side, "script", "skill", "xiaoyao.lua")
        s = rd(p)
        if HOANTAC:
            moi = s.replace("\t\t\t--{{1,8},{20,3},{21,3}},", "\t\t\tXX8XX").replace("\t\t\t{{1,10},{30,4},{31,4}},", "\t\t\t--{{1,10},{30,4},{31,4}},").replace("\t\t\tXX8XX", "\t\t\t{{1,8},{20,3},{21,3}},")
        else:
            moi = s.replace("\t\t\t--{{1,8},{20,3},{21,3}},", "\t\t\tXX8XX").replace("\t\t\t{{1,10},{30,4},{31,4}},", "\t\t\t--{{1,10},{30,4},{31,4}},").replace("\t\t\tXX8XX", "\t\t\t{{1,8},{20,3},{21,3}},")
        if moi == s:
            print("  [=] %s xiaoyao.lua: khong tim thay cap dong can doi (da doi roi?)" % side); continue
        if sum(1 for c in moi if ord(c) >= 0x80) != sum(1 for c in s if ord(c) >= 0x80):
            raise SystemExit("lech byte cao " + p)
        tmp = p + ".costsp_tmp"
        io.open(tmp, "w", encoding="latin-1", newline="").write(moi)
        try:
            out = kiem_lua(tmp)
        finally:
            os.remove(tmp)
        print("  [+] %s xiaoyao.lua -> cost_sp moi:\n      %s" % (side, out.replace("\n", "\n      ")))
        if not KIEM:
            if not os.path.exists(p + BAK): shutil.copy2(p, p + BAK)
            io.open(p, "w", encoding="latin-1", newline="").write(moi)
        print("  => %s %s" % ("KIEM" if KIEM else "ghi", p))

if __name__ == "__main__":
    print("vhtd_data_patch10_costsp [VHTD 02/09s]%s%s" % (" (KIEM)" if KIEM else "", " (HOAN TAC)" if HOANTAC else ""))
    main()
    print("XONG. Nho: restart GameServer + nguoi choi thoat vao lai. Khong can build/swap .moi.")
