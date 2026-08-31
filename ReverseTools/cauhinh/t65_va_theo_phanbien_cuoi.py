# -*- coding: utf-8 -*-
"""t65_va_theo_phanbien_cuoi.py - sua 4 diem cuoi bo phan bien bat duoc.

1. [VUA] Cong tac BDH_THOIHAN_DANHHIEU_NGAY VO TAC DUNG.
   partner_reward.lua:27  TITLETIME = BDH_CFG("BDH_THOIHAN_DANHHIEU_NGAY",30)*...
   partner_reward2.lua:15 TITLETIME = 30 * ...            <- GHI DE VO DIEU KIEN
   Thu tu nap luon la reward -> reward2 -> reward3 (reward_partner.lua:7-9 va
   changge_people.lua:9-11), va Title_AddTitle doc bien TOAN CUC luc chay, nen
   gia tri thay duoc luon la so cung cua reward2. Chinh chu thich ch_thuong.lua:54
   da ghi "!! khai lai y het o partner_reward2.lua" - biet ma chua sua.
   VA: cho reward2 doc cung mot cau hinh.

2. [NHE] Chu thich toi chen vao KPlayer.cpp chi SAI SO DONG - lech dung 14 dong
   bang so dong ma chinh ban va chen vao.
       ghi 7115  -> that la 7129 (nhanh if(bGlobal) o 7128)
       ghi 7607/7612 -> that la 7621/7626
   Da do lai bang cach liet ke moi cho gan m_ActionScriptID kem ten ham chua no.

3. [NHE] onkillnpc.lua:2 ghi sai ten ham C++ goi no: "KNpc::Die". That ra moc
   nam trong KNpc::OnDeath (khai bao KNpc.cpp:1648, loi goi :1702). KNpc::Die la
   ham KHAC, ket thuc o :1646.

4. [NHE] onkillnpc.lua co "function OnRevive(nNpcIndex) end" la MA CHET -
   KNpc.cpp:8929 phat OnRevive qua bien ActionScript cua NPC chu khong qua tep
   nay. Bo di cho khoi gay hieu nham.

Muc 2 chi sua CHU THICH nen khong doi hanh vi, nhung VAN PHAI dung lai DLL vi
tep nguon doi -> md5 doi.

Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import re
import shutil
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import lua_ham as lh  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

S = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
CPP = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayer.cpp"
P_R2 = os.path.join(S, "task", "partner", "reward", "partner_reward2.lua")
P_R1 = os.path.join(S, "task", "partner", "reward", "partner_reward.lua")
P_OK = os.path.join(S, "global", "onkillnpc.lua")


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def hi(s):
    return sum(1 for c in s if ord(c) > 127)


def ghi(p, nd, that, duoi):
    if not that:
        return True
    sao = p + duoi
    if not os.path.isfile(sao):
        shutil.copy2(p, sao)
    with io.open(p, "wb") as f:
        f.write(nd.encode("latin-1"))
    return doc(p) == nd


def main():
    that = "--ghi" in sys.argv[1:]
    print("=== t65 sua 4 diem cuoi - %s ==="
          % ("GHI THAT" if that else "DIEN TAP"))
    print()

    # ---- 1. TITLETIME ----
    print("## 1. partner_reward2.lua - cong tac BDH_THOIHAN_DANHHIEU_NGAY")
    raw = doc(P_R2)
    if "BDH_THOIHAN_DANHHIEU_NGAY" in raw:
        print("   da sua - bo qua")
    else:
        # lay NGUYEN VAN dong cua reward.lua de dung y het cach goi
        m = re.search(r"^(\s*TITLETIME\s*=\s*BDH_CFG\([^\n]*)$", doc(P_R1),
                      re.M)
        if not m:
            print("!!! LOI TO: khong doc duoc dong TITLETIME o partner_reward.lua")
            return 1
        mau = m.group(1).rstrip("\r")
        dong = raw.split("\n")
        ci = [i for i, l in enumerate(dong)
              if re.match(r"\s*TITLETIME\s*=", l)]
        if len(ci) != 1:
            print("!!! LOI TO: khop %d dong TITLETIME" % len(ci))
            return 1
        i = ci[0]
        cr = "\r" if dong[i].endswith("\r") else ""
        hi0, cb0 = hi(raw), lh.can_bang(raw)
        print("   cu : %s" % dong[i].strip()[:96])
        dong[i:i + 1] = [
            "-- [PB 30/08] dong nay TRUOC DAY ghi cung 30 ngay, ghi de len gia tri" + cr,
            "-- doc tu cau hinh o partner_reward.lua:27 (thu tu nap luon la reward ->" + cr,
            "-- reward2), lam cong tac BDH_THOIHAN_DANHHIEU_NGAY vo tac dung." + cr,
            mau + cr,
        ]
        nd = "\n".join(dong)
        if lh.can_bang(nd) != cb0 or hi(nd) != hi0:
            print("!!! LOI TO: can bang / byte tieng Viet doi")
            return 1
        print("   moi: %s" % mau.strip()[:96])
        if not ghi(P_R2, nd, that, ".truoc_pb"):
            print("!!! LOI TO: doc lai KHONG khop")
            return 1
        print("   %s" % ("DA GHI" if that else "se ghi"))
    print()

    # ---- 2. so dong trong chu thich KPlayer.cpp ----
    print("## 2. KPlayer.cpp - sua so dong trong chu thich")
    raw = doc(CPP)
    n1 = raw.count("(KPlayer.cpp:7115)")
    n2 = raw.count("(KPlayer.cpp:7607/7612)")
    print("   tim thay %d chu thich ghi 7115, %d chu thich ghi 7607/7612"
          % (n1, n2))
    if n1 == 0 and n2 == 0:
        print("   da sua - bo qua")
    else:
        nd = raw.replace("(KPlayer.cpp:7115)", "(KPlayer.cpp:7129)")
        nd = nd.replace("(KPlayer.cpp:7607/7612)", "(KPlayer.cpp:7621/7626)")
        if (raw.count("{") - raw.count("}")) != (nd.count("{") - nd.count("}")):
            print("!!! LOI TO: can bang ngoac doi")
            return 1
        if not ghi(CPP, nd, that, ".truoc_sodong"):
            print("!!! LOI TO: doc lai KHONG khop")
            return 1
        print("   %s (chi sua chu thich - khong doi hanh vi, nhung PHAI dung lai"
              " DLL)" % ("DA GHI" if that else "se ghi"))
    print()

    # ---- 3+4. onkillnpc.lua ----
    print("## 3+4. onkillnpc.lua - ten ham C++ va ma chet OnRevive")
    raw = doc(P_OK)
    if "KNpc::OnDeath" in raw and "OnRevive" not in raw:
        print("   da sua - bo qua")
    else:
        nd = raw.replace(
            "-- MOC \"nguoi choi giet mot NPC\". Duoc goi tu C++ (KNpc.cpp, KNpc::Die)",
            "-- MOC \"nguoi choi giet mot NPC\". Duoc goi tu C++ trong KNpc::OnDeath")
        nd = nd.replace(
            "-- mot lan cho MOI xac NPC do nguoi choi ha.",
            "-- (khai bao KNpc.cpp:1648, loi goi :1702) - mot lan cho MOI xac NPC\r\n"
            "-- do nguoi choi ha. LUU Y: KNpc::Die la ham KHAC (ket thuc :1646).")
        # bo ma chet OnRevive
        dong = nd.split("\n")
        ci = [i for i, l in enumerate(dong)
              if l.startswith("function OnRevive(")]
        if ci:
            i = ci[0]
            r = lh.tim_ham([x.rstrip("\r") for x in dong], "OnRevive")
            if r is None:
                print("!!! LOI TO: khong do duoc ham OnRevive")
                return 1
            a, b = r
            cr = "\r" if dong[a].endswith("\r") else ""
            dong[a:b + 1] = [
                "-- [PB 30/08] da bo \"function OnRevive() end\" - MA CHET." + cr,
                "-- KNpc.cpp:8929 phat OnRevive qua bien ActionScript cua tung NPC," + cr,
                "-- khong qua tep nay, nen ham do khong bao gio duoc goi." + cr,
            ]
            nd = "\n".join(dong)
        if lh.can_bang(nd) != lh.can_bang(raw):
            print("!!! LOI TO: can bang tu khoa doi")
            return 1
        if not ghi(P_OK, nd, that, ".truoc_pb"):
            print("!!! LOI TO: doc lai KHONG khop")
            return 1
        print("   %s" % ("DA GHI" if that else "se ghi"))
    print()
    if not that:
        print("DIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
