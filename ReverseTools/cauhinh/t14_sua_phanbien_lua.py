# -*- coding: utf-8 -*-
"""t14_sua_phanbien_lua.py - sua 3 diem phia Lua/C++ ma bo phan bien bat duoc.

SUA 1 - TIEN DE CUA t04 SAI: "RunTime chay moi phut mot lan".
  Da TU DOC CoreServerShell.cpp:1162-1177:
      if (!(g_SubWorldSet.GetGameTime() % GAME_FPS))
          if (aSysTime.wSecond == 0)
              pTimeScript->CallFunction("RunTime", 0, "");
  GetGameTime() la bo dem KHUNG (tang moi vong lap chinh), khong phai dong ho.
  Khi may chu khung lai roi chay don, nhieu boi so cua 18 cung roi vao GIAY 0
  => RunTime chay 2-3 lan trong CUNG mot phut. Phep `mod(phut, nhip) == 0` cua
  t04 se cho dofile chay ca 2-3 lan do (moi lan ~0,97 MB).
  VA: dung DAU MOC lan chay cuoi. Bien phai CHI DUOC GAN trong ham, KHONG khoi
  tao o cap tep - vi dofile nap lai ca tep se xoa gia tri. Day dung la khuon ma
  timerserver.lua da dung san cho g_nTongMaintainDay.
  ⚠️ Voi nhip = 1 (mac dinh) hanh vi KHONG DOI: van dofile moi lan RunTime chay,
     y het truoc ban va.

SUA 2 - v30 bien "loi no to" thanh "hong trang thai im lang".
  Truoc ban va, random(1,0) nem loi lam DUT ca loi goi, nen cac dong ghi so o
  npc_death.lua KHONG chay. Sau ban va, chot `return` lam ham thoat em, phia goi
  van ghi so nhu da sinh quai xong => to doi ket, khong ai biet vi sao.
  VA: cho YDBZ_add_npc TRA VE 0/1. Phia goi chua doc gia tri nay (viec do de dot
  sau, phai sua npc_death.lua co nghiem thu rieng) nhung it nhat ham da noi that,
  va ghi ro dieu do trong chu thich.
  Kem: chot cu dat TRE MOT CHI SO - `YDBZ_map_npcBossEx[nteams][nway]` no ngay
  tren chinh dong do neu nteams ngoai 1..3. Kiem TUNG TANG.
  Kem: rao them cho `file` nil / `file[2]` khong hop le - ba dong ngay TREN hai
  chot cu, la cho no THAT SU cua loi toa do.

SUA 3 - chu thich tran roll trong KNpc.cpp dung SO BIA ("~14.000").
  Da do that ca 49 tep .ini: te nhat la npcdroprate110.ini = 400.000/4.300 =
  93 lan roll cho moi mon. Sua chu thich theo so do that.

Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import re
import shutil
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

S = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
CORE = r"D:\GAMEDEVNEW\Sources\Core\Src"
NHAN = "[PBLUA 29/08]"
T = "\t"


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def can_bang(s):
    t = re.sub(r"--[^\n]*", "", s)
    t = re.sub(r'"[^"]*"', '""', t)
    t = re.sub(r"'[^']*'", "''", t)
    d = lambda w: len(re.findall(r"\b%s\b" % w, t))
    return (d("function") + d("then") + d("do") - d("elseif")) - d("end")


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t14_sua_phanbien_lua - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))
    viec = []

    # ============ SUA 1: timerserver.lua - dung dau moc ============
    p1 = os.path.join(S, "timerserver.lua")
    raw1 = doc(p1)
    if NHAN in raw1:
        print("  timerserver.lua DA VA - bo qua")
    else:
        eol = "\r\n" if raw1.count("\r\n") >= (raw1.count("\n")
                                              - raw1.count("\r\n")) else "\n"
        cu = eol.join([
            T + "if (nNhipNap ~= nil and nNhipNap > 0) then",
            T*2 + "local nYrN, nMoN, nDyN, nHrN, nMiN = GetTimeNow()",
            T*2 + "-- RunTime chay moi phut nen chi can chia du theo phut trong ngay;",
            T*2 + "-- KHONG dung bien dem vi bien se mat sau moi lan dofile.",
            T*2 + "if (nNhipNap <= 1 or mod(nHrN * 60 + nMiN, nNhipNap) == 0) then",
            T*3 + "dofile(\"script/timerserver.lua\")",
            T*2 + "end",
            T + "end",
        ])
        if raw1.count(cu) != 1:
            print("!!! LOI TO: timerserver.lua khong khop khoi nhip (khop %d)"
                  % raw1.count(cu))
            return 1
        moi = eol.join([
            T + "if (nNhipNap ~= nil and nNhipNap > 0) then",
            T*2 + "-- " + NHAN + " RunTime KHONG chay dung mot lan moi phut.",
            T*2 + "-- CoreServerShell.cpp:1165-1171 goi khi (bo dem KHUNG % 18 == 0)",
            T*2 + "-- VA giay == 0; khi may chu khung lai roi chay don thi nhieu boi",
            T*2 + "-- so cua 18 cung roi vao giay 0 => RunTime chay 2-3 lan trong",
            T*2 + "-- cung mot phut. Nen dung DAU MOC thay cho phep chia du.",
            T*2 + "-- g_nMocNapLai CHI duoc gan trong ham (khong khai o cap tep) de",
            T*2 + "-- song qua dofile - dung khuon ma tep nay da dung cho",
            T*2 + "-- g_nTongMaintainDay.",
            T*2 + "local nYrN, nMoN, nDyN, nHrN, nMiN = GetTimeNow()",
            T*2 + "local nPhutNay = nDyN * 1440 + nHrN * 60 + nMiN",
            T*2 + "if (g_nMocNapLai == nil) then",
            T*3 + "g_nMocNapLai = -99999",
            T*2 + "end",
            T*2 + "if (nPhutNay - g_nMocNapLai >= nNhipNap) then",
            T*3 + "g_nMocNapLai = nPhutNay",
            T*3 + "dofile(\"script/timerserver.lua\")",
            T*2 + "end",
            T + "end",
        ])
        nd1 = raw1.replace(cu, moi)
        if can_bang(nd1) != can_bang(raw1):
            print("!!! LOI TO: can bang tu khoa Lua doi (timerserver)")
            return 1
        print("  timerserver.lua: doi phep chia du -> dau moc lan chay cuoi")
        viec.append((p1, nd1, raw1))

    # ============ SUA 2: npc.lua - tra trang thai + chot tung tang ============
    p2 = os.path.join(S, "missions", "yandibaozang", "npc.lua")
    raw2 = doc(p2)
    if NHAN in raw2:
        print("  npc.lua DA VA - bo qua")
    else:
        eol = "\r\n" if raw2.count("\r\n") >= (raw2.count("\n")
                                              - raw2.count("\r\n")) else "\n"
        nd2 = raw2

        # 2a) rao dau ham cho `file`
        # CO HAI ham cung khuon: YDBZ_add_npc va YDBZ_add_npcboss. Ca hai deu
        # co dung loi nay nen rao ca hai.
        cu_a = T + "local file_name = file[1];\t-- "
        n_a = nd2.count(cu_a)
        if n_a != 2:
            print("!!! LOI TO: npc.lua co %d dong lay file[1] (mong 2)" % n_a)
            return 1
        moi_a = eol.join([
            T + "-- " + NHAN + " rao NGAY DAU HAM. Ba dong duoi day moi la cho no",
            T + "-- THAT SU cua loi toa do: file nil, hoac file[2] <= 0 lam",
            T + "-- random(1, pos_count) nem loi truoc khi toi duoc cac chot ben duoi.",
            T + "if (file == nil or file[1] == nil or file[2] == nil"
                " or file[2] < 1) then",
            T*2 + 'print("[VIEMDE] YDBZ_add_npc: tham so file khong hop le")',
            T*2 + "return 0",
            T + "end",
            cu_a,
        ])
        nd2 = nd2.replace(cu_a, moi_a)

        # 2b) chot boss theo TUNG TANG
        cu_b = eol.join([
            T*2 + "if nstate == 21 then",
            T*3 + "item = YDBZ_map_npcBossEx[nteams][nway]",
        ])
        if nd2.count(cu_b) != 1:
            print("!!! LOI TO: npc.lua khong khop khoi boss ai")
            return 1
        moi_b = eol.join([
            T*2 + "if nstate == 21 then",
            T*3 + "-- " + NHAN + " kiem TUNG TANG: chot cu dat sau phep tra hai",
            T*3 + "-- tang nen neu nteams ngoai 1..3 thi no ngay tai day, chot",
            T*3 + "-- khong bao gio chay toi.",
            T*3 + "local tbBossEx = YDBZ_map_npcBossEx[nteams]",
            T*3 + "if (tbBossEx == nil) then",
            T*4 + 'print("[VIEMDE] khong co bang boss cho to "..nteams)',
            T*4 + "return 0",
            T*3 + "end",
            T*3 + "item = tbBossEx[nway]",
        ])
        nd2 = nd2.replace(cu_b, moi_b)

        # 2c) doi cac `return` cua hai chot cu thanh `return 0`
        for cu_c in (
            eol.join([
                T*3 + 'print("[VIEMDE] khong co boss ai cho to "..nteams..'
                      '" ai "..nway)',
                T*4 + "return",
            ]),
            eol.join([
                T*4 + 'print("[VIEMDE] khong co boss ai cho to "..nteams..'
                      '" ai "..nway)',
                T*4 + "return",
            ]),
        ):
            if cu_c in nd2:
                nd2 = nd2.replace(cu_c, cu_c[:-len("return")] + "return 0")
                break
        cu_d = eol.join([
            T*3 + 'print("[VIEMDE] thieu hoac khong doc duoc tep toa do: "'
                  '..file_name)',
            T*3 + "return",
        ])
        if nd2.count(cu_d) == 1:
            nd2 = nd2.replace(cu_d, cu_d[:-len("return")] + "return 0")

        # 2d) ghi chu ve viec tra ve
        moc_ham = "function YDBZ_add_npc(file,nteams,nway,nstate)"
        if nd2.count(moc_ham) != 1:
            print("!!! LOI TO: npc.lua khong thay dung 1 khai bao YDBZ_add_npc")
            return 1
        nd2 = nd2.replace(moc_ham, eol.join([
            "-- " + NHAN + " Ham nay nay TRA VE 0 khi khong sinh duoc quai/boss.",
            "-- Phia goi (npc_death.lua) HIEN CHUA doc gia tri nay - no van ghi so",
            "-- nhu da sinh xong, nen mot ai thieu tep toa do se lam to doi ket ma",
            "-- khong bao gi. Sua phia goi la viec cua dot sau (can nghiem thu rieng).",
            moc_ham,
        ]))

        if can_bang(nd2) != can_bang(raw2):
            print("!!! LOI TO: can bang tu khoa Lua doi (npc.lua): %d -> %d"
                  % (can_bang(raw2), can_bang(nd2)))
            return 1
        if sum(1 for c in nd2 if ord(c) > 127) != sum(1 for c in raw2
                                                     if ord(c) > 127):
            print("!!! LOI TO: byte cao doi (npc.lua)")
            return 1
        print("  npc.lua: rao dau ham + chot tung tang + tra ve 0/1")
        viec.append((p2, nd2, raw2))

    # ============ SUA 3: KNpc.cpp - chu thich dung so do that ============
    p3 = os.path.join(CORE, "KNpc.cpp")
    raw3 = doc(p3)
    if NHAN in raw3:
        print("  KNpc.cpp DA VA - bo qua")
    else:
        eol = "\r\n" if raw3.count("\r\n") >= (raw3.count("\n")
                                              - raw3.count("\r\n")) else "\n"
        cu3 = eol.join([
            T + "// Tran so lan roll: tep .ini te nhat trong du an can ~1.400 lan roll",
            T + "// cho moi mon (RandRange 3.000.000 / tong rate 2.200), nhan 10 mon =",
            T + "// ~14.000. Tran 2.000.000 la du xa - du lieu dung KHONG BAO GIO cham",
            T + "// toi, nen ti le rot do giu nguyen y het truoc khi va.",
        ])
        if raw3.count(cu3) != 1:
            print("!!! LOI TO: KNpc.cpp khong khop chu thich tran roll")
            return 1
        moi3 = eol.join([
            T + "// " + NHAN + " Tran so lan roll, dat theo SO DO THAT (da quet ca 49",
            T + "// tep settings\\droprate\\**\\*.ini): tep te nhat la",
            T + "// npcdroprate110.ini voi RandRange 400.000 / tong rate 4.300 = 93",
            T + "// lan roll ky vong cho MOI mon. Voi so mon nhieu nhat ma script yeu",
            T + "// cau, ky vong xau nhat van duoi 2.000 lan. Tran 2.000.000 gap hon",
            T + "// mot nghin lan con so do, nen du lieu dung KHONG BAO GIO cham toi",
            T + "// va ti le rot do giu nguyen y het truoc khi va.",
        ])
        nd3 = raw3.replace(cu3, moi3)
        print("  KNpc.cpp: sua chu thich tran roll theo so do that")
        viec.append((p3, nd3, raw3))

    if not viec:
        print("Khong co gi de lam.")
        return 0
    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    for p, nd, raw in viec:
        sao = p + ".truoc_pblua"
        if not os.path.isfile(sao):
            shutil.copy2(p, sao)
        with io.open(p, "wb") as f:
            f.write(nd.encode("latin-1"))
        if doc(p) != nd:
            print("!!! LOI TO: doc lai KHONG khop: %s" % p)
            return 1
        print("  DA GHI %s" % os.path.basename(p))
    return 0


if __name__ == "__main__":
    sys.exit(main())
