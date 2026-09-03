# -*- coding: utf-8 -*-
# vhtd_data_patch12_luctay.py [VHTD 02/09v]
# HA LUC TAY NOI CONG HOA SON ve muc <= 20.000 theo yeu cau chu ("full ky nang full mach cung khong hon 20k").
#
# Chu chot CACH 1 + CACH 3. Bo qua cach 2 (chia hien thi cho 5) vi ap chung voi 1+3 se ra ~2-3k, thap xa muc tieu.
#
# CACH 1 - Phach Thach Pha Ngoc (1382), bang SKILLS.pishi_poyu.colddamage_v:
#   Doi cho: chu thich dong dang dung {1,160},{25,12000*1.2},{40,24000*1.5}
#            bat lai dong VLTK TU CHU THICH SAN {1,200},{40,8000},{41,8000}
#   -> bang goc: cap20 11.433 -> 4.000 | cap25 14.400 -> 5.000 | cap30 21.600 -> 6.000
#
# CACH 3 - bot buff cong phan tram vao 1382. Chi co BA nguon that (bang thu tu moyun_jianqi1 la DU LIEU MO COI,
#   khong hang skills.txt nao tro toi - da kiem ca server lan client):
#     qingfeng_songshuang (1372 Thanh Van Tong Sang)  +60% cap20  -> CHU THICH (bo)
#     longxuan_jianqi1    (1376 Long Huyen Kiem Khi)  +60% cap20  -> CHU THICH (bo)
#     shenguang_xuanrao   (1380 Ma Van Kiem Khi)      +60% cap20  -> GIU LAI (chieu cap cao nhat, gan co che kich no)
#   manatoskill_enhance cua 1379 Khi Quan Truong Hong (+100% cap20 khi day noi luc) - GIU NGUYEN.
#
# KET QUA DU KIEN: cap20 10.400 | cap25 ~14.100 | cap30 ~18.400  (deu <= 20.000)
#
# CANH BAO CAN BANG: du lieu ta TRUNG VLTK ca bon bang. Day la chinh sua CO CHU Y theo yeu cau chu, KHONG phai va loi.
# Thuan du lieu Lua: KHONG build, KHONG swap .moi. Chi restart GameServer + nguoi choi thoat vao lai.
#
# DUNG: python vhtd_data_patch12_luctay.py --kiem      (thu, khong ghi)
#       python vhtd_data_patch12_luctay.py             (ghi that)
#       python vhtd_data_patch12_luctay.py --hoan-tac  (tra ve nhu cu tu ban luu)
import io
import os
import sys
import shutil
import subprocess

KIEM = "--kiem" in sys.argv
HOANTAC = "--hoan-tac" in sys.argv
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

BIN = "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin"
LUA4 = "D:/GAMEDEVNEW/ReverseTools/lua4/lua4.exe"
BAK = ".truoc_luctay_0209"
MK = "[VHTD 02/09v]"
T = chr(9)
NL = chr(13) + chr(10)
BSLASH = chr(92)

CU1 = T * 3 + "--[1]={{1,200},{40,8000},{41,8000}},"
CU3 = T * 3 + "--[3]={{1,200},{40,8000},{41,8000}},"
MOI1 = T * 3 + "[1]={{1,160},{25,12000*1.2},{40,24000*1.5}},  "
MOI3 = "   " + T * 3 + "[3]={{1,160},{25,12000*1.2},{40,24000*1.5}},  "

BO_BUFF = ["qingfeng_songshuang", "longxuan_jianqi1"]
GIU_BUFF = "shenguang_xuanrao"

LUA_DO = '''dofile("@DIR@/head.lua")
dofile("@FILE@")
local B = SKILLS.pishi_poyu.colddamage_v[1]
local M = SKILLS.qiguan_changhong.manatoskill_enhance[1]
for _,lv in {20,25,30} do
  local p = 100 + Link(lv,M)
  local n = 0
  for tn,tv in SKILLS do
    if tv.addskilldamage1 and type(tv.addskilldamage1[1]) == "table" then
      if Link(1,tv.addskilldamage1[1]) == 1382 and tn ~= "moyun_jianqi1" then
        p = p + Link(lv,tv.addskilldamage1[3])
        n = n + 1
      end
    end
  end
  print(format("  cap %2d: bang goc %6d  x %3d%%  (%d buff)  =  LUC TAY %6d",
        lv, Link(lv,B), p, n, Link(lv,B)*p/100))
end
'''


def rd(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def khop_ngoac(s, i):
    """i tro toi mot dau '{'; tra ve chi so cua '}' dong tuong ung."""
    d = 0
    for j in range(i, len(s)):
        if s[j] == "{":
            d += 1
        elif s[j] == "}":
            d -= 1
            if d == 0:
                return j
    raise SystemExit("khong khop ngoac tu vi tri %d" % i)


def than_bang(s, ten):
    """tra ve (dau, cuoi) cua than bang SKILLS.<ten> = { ... }"""
    moc = "SKILLS." + ten
    i = s.find(moc)
    if i < 0:
        raise SystemExit("khong thay bang " + ten)
    j = s.find("{", i)
    return j, khop_ngoac(s, j)


def chu_thich_khoi(s, ten, truong):
    """Chu thich toan bo khoi <truong>={...}, nam trong bang <ten>."""
    d, c = than_bang(s, ten)
    k = s[d:c].find(truong + "={")
    if k < 0:
        raise SystemExit("bang %s khong co %s" % (ten, truong))
    a = d + k
    while a > 0 and s[a - 1] in (T, " "):
        a -= 1
    b = khop_ngoac(s, d + k + len(truong) + 1) + 1
    if b < len(s) and s[b] == ",":
        b += 1
    khoi = s[a:b]
    if "--" + truong in khoi:
        return s, 0
    ra = []
    for dong in khoi.split(NL):
        n = 0
        while n < len(dong) and dong[n] in (T, " "):
            n += 1
        ra.append(dong[:n] + "--" + dong[n:] if dong[n:] else dong)
    ghi_chu = (T * 2 + "-- " + MK + " bo buff cong % vao 1382 - ha luc tay noi cong theo yeu cau chu" + NL)
    return s[:a] + ghi_chu + NL.join(ra) + s[b:], 1


def do_lua(path):
    """Nap head.lua + tep vua sua, in bang luc tay moi. Bao loi va DUNG neu lua4 khong chay."""
    p = path.replace(BSLASH, "/")
    t = LUA_DO.replace("@DIR@", os.path.dirname(p)).replace("@FILE@", p)
    tp = os.path.join(os.environ.get("TEMP", "."), "_t_luctay.lua")
    io.open(tp, "w", encoding="latin-1", newline=chr(10)).write(t)
    r = subprocess.run([LUA4, "-s100", tp], capture_output=True)
    os.remove(tp)
    if r.returncode != 0 or b"cap 20" not in r.stdout:
        raise SystemExit("lua4 BAO LOI - KHONG ghi:" + chr(10) + r.stderr.decode("latin-1")[:600])
    return r.stdout.decode("latin-1").rstrip()


def main():
    for side in ("server", "client"):
        p = (BIN + "/" + side + "/script/skill/huashan.lua")
        if HOANTAC:
            if os.path.exists(p + BAK):
                shutil.copy2(p + BAK, p)
                print("  [<] %s huashan.lua: da hoan tac tu ban luu" % side)
            else:
                print("  [!] %s: khong co ban luu %s" % (side, BAK))
            continue
        s = rd(p)
        cao_truoc = sum(1 for ch in s if ord(ch) >= 0x80)
        if MK in s:
            print("  [=] %s huashan.lua da co %s" % (side, MK))
            continue
        for a, b in ((CU1, CU1.replace("--[1]", "[1]")),
                     (CU3, CU3.replace("--[3]", "[3]")),
                     (MOI1, MOI1.replace(T + "[1]", T + "--[1]")),
                     (MOI3, MOI3.replace(T + "[3]", T + "--[3]"))):
            if s.count(a) != 1:
                raise SystemExit("neo cach 1 xuat hien %d lan: %s" % (s.count(a), repr(a)))
            s = s.replace(a, b)
        print("  [+] %s | cach 1: pishi_poyu.colddamage_v -> dong VLTK cu {1,200},{40,8000}" % side)
        for ten in BO_BUFF:
            s, n = chu_thich_khoi(s, ten, "addskilldamage1")
            print("  [+] %s | cach 3: %s bo addskilldamage1 -> 1382 %s"
                  % (side, ten, "" if n else "(da bo tu truoc)"))
        print("  [.] %s | giu lai buff cua %s" % (side, GIU_BUFF))
        cao_sau = sum(1 for ch in s if ord(ch) >= 0x80)
        if cao_sau != cao_truoc:
            raise SystemExit("lech byte cao %d -> %d tai %s" % (cao_truoc, cao_sau, p))
        if chr(0xFFFD) in s:
            raise SystemExit("FFFD trong " + p)
        tmp = p + ".luctay_tmp"
        io.open(tmp, "w", encoding="latin-1", newline="").write(s)
        try:
            out = do_lua(tmp)
        finally:
            os.remove(tmp)
        print("  --- LUC TAY NOI CONG SAU KHI SUA ---")
        print(out)
        if not KIEM:
            if not os.path.exists(p + BAK):
                shutil.copy2(p, p + BAK)
            io.open(p, "w", encoding="latin-1", newline="").write(s)
        print("  => %s %s" % ("KIEM" if KIEM else "GHI", p))


if __name__ == "__main__":
    print("vhtd_data_patch12_luctay %s%s%s"
          % (MK, " (KIEM)" if KIEM else "", " (HOAN TAC)" if HOANTAC else ""))
    main()
    print("XONG. Thuan du lieu: KHONG build, KHONG swap. Restart GameServer + nguoi choi thoat vao lai.")
