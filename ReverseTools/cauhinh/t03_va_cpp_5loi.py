# -*- coding: utf-8 -*-
"""t03_va_cpp_5loi.py - THI CONG DOT 2: 5 loi C++ da TU DOC MA kiem chung.

  L1 (NANG NHAT - TREO GAMESERVER) KNpc.cpp:9258 KNpc::DropRateItem
      while (j < nCount) { nRand = g_Random(nMaxRandRate); for(...) if(trung){...
      j++; break; } }
      `j++` (:9756) CHI nam trong nhanh trung. Neu TONG RandRate = 0 thi khong
      muc nao trung duoc => vong while KHONG BAO GIO THOAT => treo ca may chu.
      Co tep .ini nhu the that: settings\\droprate\\npcdroprate_fenglindubei.ini
      khai Count=86 va npcdroprate_fenglindunan.ini khai Count=53 nhung KHONG
      co muc [n] nao => moi nRate = 0 (KNpcTemplate.cpp:41-44 mac dinh 0).
      Hien chua tep nao tro toi chung, nhung do la MIN: chi can mot dong Lua
      tro nham ten, hoac GM sua .ini sai, la treo.
      VA: cong tong rate truoc vong lap, tong <= 0 thi thoi; them tran so lan
      roll. Du lieu dung thi khong bao gio cham tran => KHONG doi ti le rot do.

  L2 KPlayer.cpp:8470   fopen("ScripNpcDialog.log","a") -> fprintf KHONG kiem NULL
  L3 KObj.cpp:1245      fopen("ScripObjectDialog.log","a") -> nhu tren
      Ca hai chay khi g_WriteScriptNpcLog bat (gamesetting.ini dang =1). fopen
      that bai (het handle / tep bi khoa / dia day) => fprintf(NULL) => SAP.

  L4 KItem.cpp:3130     nSafeIndex = pScript->SafeCallBegin();  <- viet nham cho
      SafeCallEnd. Gan lai nSafeIndex = dinh ngan xep HIEN TAI (da co ket qua
      GetDesc tren do), roi SafeCallEnd(nSafeIndex) khoi phuc ve chinh no =>
      KHONG don gi. Ro ri ngan xep Lua moi lan xem mo ta vat pham co GetDesc.
      (KLuaScript.cpp: SafeCallBegin ghi dinh, SafeCallEnd tra dinh ve.)
      VA: bo dong thua.

  L5 ScriptFuns.cpp:2547  for (int k = 0; k < 21; k++)  ma szMod[22] co DU 22
      chuoi (:2516), phan tu cuoi la "PET" o chi so 21 => IncludeLib("PET")
      KHONG BAO GIO khop => he Ban Dong Hanh khong nap duoc thu vien.
      VA: 21 -> 22.

KHONG dong toi (BAO CHU, khong tu vá - doi CAN BANG, Gate 4):
  KNpc.cpp:9301-9307 nhanh VIP doc Player[CLIENT_PLAYER_INDEX] (chi so client,
      tren may chu luon sai) va nhanh `else` ngay sau GHI DE
      nLuckySoDong = nTotalLucky = nLuck, xoa sach phan cong may man cua boss
      xanh (+5/+10) va boss vang (+10/+20) vua tinh o :9288-9299.
  KNpc.cpp:9309 printf("Debug: Lucky...") chay MOI LAN rot do -> de dot t06
      (he log co cong tac) xu ly chung.

CAN BUILD LAI: CA HAI cau hinh (Core.vcxproj dung chung client + server).
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_5loi lan dau).
"""
import io
import os
import shutil
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

SRC = r"D:\GAMEDEVNEW\Sources"
CORE = os.path.join(SRC, "Core", "Src")
HAU_TO = ".truoc_5loi"
NHAN = "[VA5LOI 29/08]"
T = "\t"

VA = []

# ---------------- L1: chong treo vong lap rot do ----------------
VA.append(dict(
    ten="L1 KNpc::DropRateItem - chong TREO may chu",
    tep=os.path.join(CORE, "KNpc.cpp"),
    cu=[
        T + "int j = 0;",
        T + "GetMpsPos(&nX, &nY);",
        "",
        T + "while (j < nCount)",
        T + "{",
        T*2 + "nRand = g_Random(m_pDropRate->nMaxRandRate);",
    ],
    moi=[
        T + "int j = 0;",
        T + "GetMpsPos(&nX, &nY);",
        "",
        T + "// " + NHAN + " CHONG TREO MAY CHU. `j++` chi nam trong nhanh trung",
        T + "// (cuoi ham), nen neu TONG RandRate = 0 thi khong muc nao trung duoc",
        T + "// va vong while duoi day KHONG BAO GIO THOAT. Co tep .ini nhu the",
        T + "// that: npcdroprate_fenglindubei.ini khai Count=86 ma khong co muc",
        T + "// [n] nao (KNpcTemplate.cpp:41-44 cho moi truong mac dinh 0).",
        T + "int nTongRate = 0;",
        T + "for (int nT = 0; nT < m_pDropRate->nCount; nT++)",
        T*2 + "nTongRate += m_pDropRate->pItemParam[nT].nRate;",
        T + "if (nTongRate <= 0 || m_pDropRate->nMaxRandRate <= 0)",
        T*2 + "return;",
        T + "// Tran so lan roll: tep .ini te nhat trong du an can ~1.400 lan roll",
        T + "// cho moi mon (RandRange 3.000.000 / tong rate 2.200), nhan 10 mon =",
        T + "// ~14.000. Tran 2.000.000 la du xa - du lieu dung KHONG BAO GIO cham",
        T + "// toi, nen ti le rot do giu nguyen y het truoc khi va.",
        T + "int nRollConLai = 2000000;",
        "",
        T + "while (j < nCount)",
        T + "{",
        T*2 + "if (--nRollConLai < 0)",
        T*3 + "break;",
        T*2 + "nRand = g_Random(m_pDropRate->nMaxRandRate);",
    ],
))

# ---------------- L2 + L3: fopen khong kiem NULL ----------------
for _ten, _tep, _log in (
    ("L2 KPlayer.cpp fopen NULL", os.path.join(CORE, "KPlayer.cpp"), "ScripNpcDialog"),
    ("L3 KObj.cpp fopen NULL", os.path.join(CORE, "KObj.cpp"), "ScripObjectDialog"),
):
    _bien = "Npc[nIdx].Name ,Npc[nIdx].ActionScript" if "KPlayer" in _tep \
        else "m_szName ,m_szScriptName"
    _tab = T*6 if "KPlayer" in _tep else T*2
    VA.append(dict(
        ten=_ten,
        tep=_tep,
        cu=[
            _tab + 'FILE* pFile = fopen("%s.log", "a");' % _log,
            _tab + 'fprintf(pFile, "%%s \\t [%%s] \\t %%s\\n", buff , %s);' % _bien,
            _tab + "fclose(pFile);",
        ],
        moi=[
            _tab + "// " + NHAN + " fopen co the tra NULL (het handle / tep bi khoa /",
            _tab + "// dia day) - truoc day fprintf(NULL) lam SAP may chu.",
            _tab + 'FILE* pFile = fopen("%s.log", "a");' % _log,
            _tab + "if (pFile)",
            _tab + "{",
            _tab + T + 'fprintf(pFile, "%%s \\t [%%s] \\t %%s\\n", buff , %s);' % _bien,
            _tab + T + "fclose(pFile);",
            _tab + "}",
        ],
    ))

# ---------------- L4: SafeCallBegin viet nham cho SafeCallEnd ----------------
VA.append(dict(
    ten="L4 KItem.cpp - ro ri ngan xep Lua",
    tep=os.path.join(CORE, "KItem.cpp"),
    cu=[
        T*2 + "nSafeIndex = pScript->SafeCallBegin();",
        T*2 + "pScript->SafeCallEnd(nSafeIndex);",
    ],
    moi=[
        T*2 + "// " + NHAN + " bo dong `nSafeIndex = SafeCallBegin();` o day: no ghi",
        T*2 + "// de nSafeIndex bang dinh ngan xep HIEN TAI (da co ket qua GetDesc",
        T*2 + "// tren do), khien SafeCallEnd khoi phuc ve chinh no = KHONG don gi.",
        T*2 + "// Dinh dung phai la cai lay o SafeCallBegin() truoc CallFunction.",
        T*2 + "pScript->SafeCallEnd(nSafeIndex);",
    ],
))

# ---------------- L5: off-by-one bo sot "PET" ----------------
VA.append(dict(
    ten="L5 ScriptFuns.cpp - IncludeLib(\"PET\") khong bao gio khop",
    tep=os.path.join(CORE, "ScriptFuns.cpp"),
    cu=[T*1 + "for (int k = 0; k < 21; k++)"],
    moi=[
        T + "// " + NHAN + " szMod[22] (:2516) co DU 22 chuoi, phan tu cuoi la",
        T + '// "PET" o chi so 21 - vong cu k<21 bo sot no, nen IncludeLib("PET")',
        T + "// khong bao gio khop va he Ban Dong Hanh khong nap duoc thu vien.",
        T + "for (int k = 0; k < 22; k++)",
    ],
))


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t03_va_cpp_5loi - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    theo_tep = {}
    for v in VA:
        theo_tep.setdefault(v["tep"], []).append(v)

    ketqua = []
    for tep, ds in theo_tep.items():
        if not os.path.isfile(tep):
            print("!!! LOI TO: thieu %s" % tep)
            return 1
        raw = doc(tep)
        ten_tep = os.path.basename(tep)
        if NHAN in raw:
            print("  %-18s DA VA - bo qua" % ten_tep)
            continue
        eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
        nd = raw
        hi0 = sum(1 for c in raw if ord(c) > 127)
        for v in ds:
            kcu = eol.join(v["cu"])
            n = nd.count(kcu)
            if n != 1:
                print("!!! LOI TO: %s - moc khop %d lan (can dung 1)" % (v["ten"], n))
                print("    moc:\n%s" % kcu)
                return 1
            nd = nd.replace(kcu, eol.join(v["moi"]))
            print("  %-18s %s" % (ten_tep, v["ten"]))
        if sum(1 for c in nd if ord(c) > 127) != hi0:
            print("!!! LOI TO: byte cao doi (%s)" % ten_tep)
            return 1
        # chot ngoac: so { va } trong PHAN THEM PHAI can bang
        them = "".join(eol.join(v["moi"]) for v in ds)
        bot = "".join(eol.join(v["cu"]) for v in ds)
        if (them.count("{") - them.count("}")) != (bot.count("{") - bot.count("}")):
            print("!!! LOI TO: ngoac nhon lech (%s)" % ten_tep)
            return 1
        ketqua.append((tep, nd, ten_tep))

    if not ketqua:
        print("Khong co gi de lam.")
        return 0
    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    for tep, nd, ten_tep in ketqua:
        sao = tep + HAU_TO
        if not os.path.isfile(sao):
            shutil.copy2(tep, sao)
        with io.open(tep, "wb") as f:
            f.write(nd.encode("latin-1"))
        if doc(tep) != nd:
            print("!!! LOI TO: doc lai KHONG khop: %s" % tep)
            return 1
        print("  DA GHI %s" % ten_tep)
    print("\nCAN BUILD LAI CA HAI cau hinh (Core dung chung client + server).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
