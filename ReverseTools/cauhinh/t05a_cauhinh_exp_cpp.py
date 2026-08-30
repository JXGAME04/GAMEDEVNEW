# -*- coding: utf-8 -*-
"""t05a_cauhinh_exp_cpp.py - THI CONG DOT 3/a: dua HANG SO EXP tu C++ ra tep cau hinh.

VI SAO: chu game muon "co file cau hinh exp". Nhung do do NHIP GAME that su
KHONG nam trong ExpRate cua gamesetting.ini - no nam CUNG trong C++:

  KPlayer.cpp:2525-2532   he so theo cap: <50 -> 80 | <80 -> 70 | <140 -> 280 | >= -> 100
  KPlayer.cpp:2516-2521   VIP cong them 20
  KPlayer.cpp:2534-2540   chuyen sinh >3 lan o map 341: <140 -> 160 | >= -> 50
  KPlayer.cpp:2543-2559   phat chenh cap: chenh <=9 giu nguyen, >9 chia 10,
                          quai cao hon = 1 exp, ca hai >= 90 thi bo qua phat
  KPlayer.cpp:2455-2470   chia to doi: 2->80 3->70 4->60 5->55 6->55 7->50 8->50 khac->60
  KPlayer.cpp:2579        nGetExp = nGetExp * g_ExpRate * nxExpTanThu

Voi ExpRate=1 hien tai, nguoi choi 8x-13x dang an x280 do MOT SO CUNG trong ma
nguon. Sua ExpRate trong ini chi NHAN THEM len tren 280 do.

MIENG VA: doc tat ca cac so tren tu gamesetting.ini nhom [Exp].
  ⚠️ GIA TRI MAC DINH TRONG C++ = DUNG BANG SO CUNG CU.
  => Neu tep ini khong co nhom [Exp], hanh vi KHONG DOI MOT LI NAO.
  (Gate 4: khong duoc doi can bang khi chua duoc yeu cau.)

KEM MOT SUA LOI THAT (KHONG phai doi can bang):
  KPlayer.cpp:2579 tinh trong `int`. nGetExp * 1 * 280 TRAN int32 khi
  nGetExp > 7.669.584. Tran xong ra SO AM, `m_nExp += so am` => nguoi choi
  MAT exp khi giet quai/boss co m_Experience lon. m_nExp la `double`
  (KPlayer.h:646) nen von thua suc chua. VA: tinh bang double.
  ⚠️ BAO CHU: sau khi va, boss exp rat lon se cho exp DUNG thay vi am. Neu
  thay ai do len cap vot sau khi cap nhat, do la vi truoc day ho dang bi
  tru exp - hay bao toi, dung tu doan.

Tep dich: KCore.cpp, KCore.h, KPlayer.cpp (Core dung chung client+server nen
phai BUILD CA HAI cau hinh) + ghi them nhom [Exp] vao gamesetting.ini van hanh.
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_cfgexp lan dau).
"""
import io
import os
import shutil
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

CORE = r"D:\GAMEDEVNEW\Sources\Core\Src"
INI = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
       r"\settings\gamesetting.ini")
HAU_TO = ".truoc_cfgexp"
NHAN = "[CFGEXP 29/08]"
T = "\t"

# (khoa ini, ten bien, gia tri mac dinh = SO CUNG CU, giai thich)
KHOA = [
    ("MocCap1", "g_nExpMocCap1", 50, "duoi cap nay dung HeSo1"),
    ("HeSo1", "g_nExpHeSo1", 80, "he so exp cho cap < MocCap1"),
    ("MocCap2", "g_nExpMocCap2", 80, "duoi cap nay dung HeSo2"),
    ("HeSo2", "g_nExpHeSo2", 70, "he so exp cho cap < MocCap2"),
    ("MocCap3", "g_nExpMocCap3", 140, "duoi cap nay dung HeSo3"),
    ("HeSo3", "g_nExpHeSo3", 280, "he so exp cho cap < MocCap3 (dang chi phoi 8x-13x)"),
    ("HeSo4", "g_nExpHeSo4", 100, "he so exp cho cap >= MocCap3"),
    ("VipCong", "g_nExpVipCong", 20, "VIP cong them vao he so"),
    ("CsMap", "g_nExpCsMap", 341, "map ap dung luat chuyen sinh"),
    ("CsLanToiThieu", "g_nExpCsLanToiThieu", 3, "so lan chuyen sinh toi thieu"),
    ("CsDuoi", "g_nExpCsDuoi", 160, "he so chuyen sinh khi cap < MocCap3"),
    ("CsTren", "g_nExpCsTren", 50, "he so chuyen sinh khi cap >= MocCap3"),
    ("ChenhCapMax", "g_nExpChenhCapMax", 9, "chenh cap toi da con an du exp"),
    ("ChiaKhiChenh", "g_nExpChiaKhiChenh", 10, "chenh qua muc thi chia cho so nay"),
    ("MienTruCap", "g_nExpMienTruCap", 90, "tu cap nay tro len bo moi phat chenh cap"),
    ("ToDoi2", "g_nExpToDoi2", 80, "% exp moi nguoi khi to doi 2 nguoi"),
    ("ToDoi3", "g_nExpToDoi3", 70, "% exp moi nguoi khi to doi 3 nguoi"),
    ("ToDoi4", "g_nExpToDoi4", 60, "% exp moi nguoi khi to doi 4 nguoi"),
    ("ToDoi5", "g_nExpToDoi5", 55, "% exp moi nguoi khi to doi 5 nguoi"),
    ("ToDoi6", "g_nExpToDoi6", 55, "% exp moi nguoi khi to doi 6 nguoi"),
    ("ToDoi7", "g_nExpToDoi7", 50, "% exp moi nguoi khi to doi 7 nguoi"),
    ("ToDoi8", "g_nExpToDoi8", 50, "% exp moi nguoi khi to doi 8 nguoi"),
    ("ToDoiKhac", "g_nExpToDoiKhac", 60, "% exp khi so nguoi ngoai 2..8"),
]

VA = []

# ---------------- KCore.cpp: khai bao bien + doc ini ----------------
VA.append(dict(
    ten="KCore.cpp - khai bao bien",
    tep=os.path.join(CORE, "KCore.cpp"),
    cu=[
        "int\t\t\t\tg_ExpRate = 1;",
        "int\t\t\t\tg_MoneyRate = 1;",
    ],
    moi=(
        [
            "int\t\t\t\tg_ExpRate = 1;",
            "int\t\t\t\tg_MoneyRate = 1;",
            "",
            "// " + NHAN + " He so exp truoc day nam CUNG trong KPlayer.cpp.",
            "// Gia tri mac dinh duoi day DUNG BANG so cung cu, nen neu",
            "// gamesetting.ini khong co nhom [Exp] thi hanh vi KHONG DOI.",
        ]
        + ["int\t\t\t\t%s = %d;\t// %s" % (b, m, g) for _, b, m, g in KHOA]
    ),
))

VA.append(dict(
    ten="KCore.cpp - doc gamesetting.ini nhom [Exp]",
    tep=os.path.join(CORE, "KCore.cpp"),
    # MOC PHAI LA ASCII THUAN - dong NotAddNpcNormal co tieng Viet TCVN3, tu
    # tay dung lai byte cua no la BAY da mac nhieu lan. Neo vao dong Skill120Rate.
    cu=[
        T*2 + 'g_GameSetting.GetInteger("ServerConfig", "Skill120Rate", 0,'
              ' &g_Skill120ExpRate);\t\t\t//#trong file config khong duoc duoi 100',
    ],
    moi=(
        [
            T*2 + 'g_GameSetting.GetInteger("ServerConfig", "Skill120Rate", 0,'
                  ' &g_Skill120ExpRate);\t\t\t//#trong file config khong duoc duoi 100',
            "",
            T*2 + "// " + NHAN + " nhom [Exp] - moi khoa mac dinh = so cung cu",
        ]
        + [T*2 + 'g_GameSetting.GetInteger("Exp", "%s", %d, &%s);' % (k, m, b)
           for k, b, m, _ in KHOA]
    ),
))

# ---------------- KCore.h: extern ----------------
VA.append(dict(
    ten="KCore.h - extern",
    tep=os.path.join(CORE, "KCore.h"),
    cu=[
        "extern int\t\t \t\tg_ExpRate;",
        "extern int\t\t \t\tg_MoneyRate;",
    ],
    moi=(
        [
            "extern int\t\t \t\tg_ExpRate;",
            "extern int\t\t \t\tg_MoneyRate;",
            "// " + NHAN + " he so exp doc tu gamesetting.ini nhom [Exp]",
        ]
        + ["extern int\t\t \t\t%s;" % b for _, b, _, _ in KHOA]
    ),
))

# ---------------- KPlayer.cpp: he so theo cap ----------------
VA.append(dict(
    ten="KPlayer.cpp - he so theo cap + VIP",
    tep=os.path.join(CORE, "KPlayer.cpp"),
    cu=[
        "    int nxExpVip = 0;",
        "    if (Npc[m_nIndex].m_CurrentExpSkillsVip == 2)",
        T*2 + "nxExpVip = 20;",
        T + "else",
        T*2 + "nxExpVip = 0;",
        T + "",
        T + "int Map = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_SubWorldID;",
        T + "int nxExpTanThu;",
        T + "int level = Npc[m_nIndex].m_Level;\t",
        "    if (level < 50)",
        T + "    nxExpTanThu = 80 + nxExpVip;",
        T + "else if (level < 80)",
        T + "    nxExpTanThu = 70 + nxExpVip;",
        T + "else if (level < 140)",
        T + "    nxExpTanThu = 280 + nxExpVip;",
        T + "else",
        T + "    nxExpTanThu = 100 + nxExpVip;",
        T + "    ",
        T + "if (m_cReBorn.GetReBornNum() > 3 && Map == 341)",
        T + "{",
        T + "    if (level < 140)",
        T + "        nxExpTanThu = 160 + nxExpVip;",
        T + "    else",
        T + "        nxExpTanThu = 50 + nxExpVip;",
        T + "}",
    ],
    moi=[
        T + "// " + NHAN + " moi so duoi day doc tu gamesetting.ini nhom [Exp];",
        T + "// mac dinh trong KCore.cpp DUNG BANG so cung cu (80/70/280/100...).",
        "    int nxExpVip = 0;",
        "    if (Npc[m_nIndex].m_CurrentExpSkillsVip == 2)",
        T*2 + "nxExpVip = g_nExpVipCong;",
        T + "else",
        T*2 + "nxExpVip = 0;",
        T + "",
        T + "int Map = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_SubWorldID;",
        T + "int nxExpTanThu;",
        T + "int level = Npc[m_nIndex].m_Level;\t",
        "    if (level < g_nExpMocCap1)",
        T + "    nxExpTanThu = g_nExpHeSo1 + nxExpVip;",
        T + "else if (level < g_nExpMocCap2)",
        T + "    nxExpTanThu = g_nExpHeSo2 + nxExpVip;",
        T + "else if (level < g_nExpMocCap3)",
        T + "    nxExpTanThu = g_nExpHeSo3 + nxExpVip;",
        T + "else",
        T + "    nxExpTanThu = g_nExpHeSo4 + nxExpVip;",
        T + "    ",
        T + "if (m_cReBorn.GetReBornNum() > g_nExpCsLanToiThieu && Map == g_nExpCsMap)",
        T + "{",
        T + "    if (level < g_nExpMocCap3)",
        T + "        nxExpTanThu = g_nExpCsDuoi + nxExpVip;",
        T + "    else",
        T + "        nxExpTanThu = g_nExpCsTren + nxExpVip;",
        T + "}",
    ],
))

# ---------------- KPlayer.cpp: phat chenh cap ----------------
VA.append(dict(
    ten="KPlayer.cpp - phat chenh cap",
    tep=os.path.join(CORE, "KPlayer.cpp"),
    cu=[
        T*2 + "if (nSubLevel <= 9)",
        T*3 + "nGetExp = nExp;",
        T*2 + "else",
        T*3 + "nGetExp = nExp /10;",
    ],
    moi=[
        T*2 + "if (nSubLevel <= g_nExpChenhCapMax)\t// " + NHAN,
        T*3 + "nGetExp = nExp;",
        T*2 + "else",
        T*3 + "nGetExp = (g_nExpChiaKhiChenh > 0)"
              " ? (nExp / g_nExpChiaKhiChenh) : nExp;",
    ],
))

VA.append(dict(
    ten="KPlayer.cpp - mien tru cap cao",
    tep=os.path.join(CORE, "KPlayer.cpp"),
    cu=[T + "if (nTarLevel >= 90 && Npc[m_nIndex].m_Level >= 90)"],
    moi=[
        T + "// " + NHAN + " nguong mien tru phat chenh cap (cu la 90)",
        T + "if (nTarLevel >= g_nExpMienTruCap"
            " && Npc[m_nIndex].m_Level >= g_nExpMienTruCap)",
    ],
))

# ---------------- KPlayer.cpp: bang chia to doi ----------------
VA.append(dict(
    ten="KPlayer.cpp - bang chia exp to doi",
    tep=os.path.join(CORE, "KPlayer.cpp"),
    cu=[
        T + "if (nTotalPlayer == 2)",
        T*2 + "ShareExp = 80;",
        T + "else if (nTotalPlayer == 3)",
        T*2 + "ShareExp = 70;",
        T + "else if (nTotalPlayer == 4)",
        T*2 + "ShareExp = 60;",
        T + "else if (nTotalPlayer == 5)",
        T*2 + "ShareExp = 55;",
        T + "else if (nTotalPlayer == 6)",
        T*2 + "ShareExp = 55;",
        T + "else if (nTotalPlayer == 7)",
        T*2 + "ShareExp = 50;",
        T + "else if (nTotalPlayer == 8)",
        T*2 + "ShareExp = 50;",
        T + "else",
        T*2 + "ShareExp = 60;",
    ],
    moi=[
        T + "// " + NHAN + " % exp moi nguoi theo so thanh vien - doc tu",
        T + "// gamesetting.ini nhom [Exp] khoa ToDoi2..ToDoi8 / ToDoiKhac.",
        T + "if (nTotalPlayer == 2)",
        T*2 + "ShareExp = g_nExpToDoi2;",
        T + "else if (nTotalPlayer == 3)",
        T*2 + "ShareExp = g_nExpToDoi3;",
        T + "else if (nTotalPlayer == 4)",
        T*2 + "ShareExp = g_nExpToDoi4;",
        T + "else if (nTotalPlayer == 5)",
        T*2 + "ShareExp = g_nExpToDoi5;",
        T + "else if (nTotalPlayer == 6)",
        T*2 + "ShareExp = g_nExpToDoi6;",
        T + "else if (nTotalPlayer == 7)",
        T*2 + "ShareExp = g_nExpToDoi7;",
        T + "else if (nTotalPlayer == 8)",
        T*2 + "ShareExp = g_nExpToDoi8;",
        T + "else",
        T*2 + "ShareExp = g_nExpToDoiKhac;",
    ],
))

# ---------------- KPlayer.cpp: TRAN SO ----------------
VA.append(dict(
    ten="KPlayer.cpp - VA TRAN SO exp (loi that)",
    tep=os.path.join(CORE, "KPlayer.cpp"),
    cu=[
        T + "if(g_ExpRate) //#x2 exp trong config",
        T + "{",
        T*2 + "nGetExp = nGetExp * g_ExpRate * nxExpTanThu;\t",
        T*2 + "m_nExp += nGetExp;",
        T + "}",
        T + "else",
        T + "{",
        T*2 + "m_nExp += nGetExp;",
        T + "}",
    ],
    moi=[
        T + "if(g_ExpRate) //#x2 exp trong config",
        T + "{",
        T*2 + "// " + NHAN + " VA TRAN SO: phep nhan nay truoc day tinh trong",
        T*2 + "// `int`, tran int32 khi nGetExp > 7.669.584 (voi he so 280) va",
        T*2 + "// cho ra SO AM => nguoi choi BI TRU exp khi giet quai/boss co",
        T*2 + "// m_Experience lon. m_nExp la double (KPlayer.h:646) nen thua",
        T*2 + "// suc chua. Tinh bang double roi moi cong.",
        T*2 + "double dGetExp = (double)nGetExp * (double)g_ExpRate"
              " * (double)nxExpTanThu;",
        T*2 + "m_nExp += dGetExp;",
        T*2 + "if (dGetExp > 2147483647.0)\t// chi de ghi log ben duoi",
        T*3 + "nGetExp = 2147483647;",
        T*2 + "else",
        T*3 + "nGetExp = (int)dGetExp;",
        T + "}",
        T + "else",
        T + "{",
        T*2 + "m_nExp += nGetExp;",
        T + "}",
    ],
))


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def ap_ini(ghi):
    """Ghi them nhom [Exp] vao gamesetting.ini van hanh (de chu NHIN THAY va sua)."""
    if not os.path.isfile(INI):
        print("  (bo qua ini: khong thay %s)" % INI)
        return True
    raw = doc(INI)
    if NHAN in raw or "[Exp]" in raw:
        print("  gamesetting.ini: DA CO nhom [Exp] - bo qua")
        return True
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    them = [
        "",
        "; " + NHAN + " He so kinh nghiem - truoc day nam CUNG trong KPlayer.cpp.",
        "; Moi so duoi day DUNG BANG gia tri dang chay, nen file nay khong lam",
        "; doi can bang. Sua xong phai KHOI DONG LAI GameServer.",
        "[Exp]",
    ]
    for k, _b, m, g in KHOA:
        them.append("%-14s= %-6d; %s" % (k, m, g))
    nd = raw.rstrip() + eol + eol.join(them) + eol
    if not ghi:
        print("  gamesetting.ini: se them nhom [Exp] (%d khoa)" % len(KHOA))
        return True
    sao = INI + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(INI, sao)
    with io.open(INI, "wb") as f:
        f.write(nd.encode("latin-1"))
    print("  DA GHI gamesetting.ini (+%d khoa nhom [Exp])" % len(KHOA))
    return True


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t05a_cauhinh_exp_cpp - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

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
            print("  %-14s DA VA - bo qua" % ten_tep)
            continue
        eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
        nd = raw
        hi0 = sum(1 for c in raw if ord(c) > 127)
        for v in ds:
            kcu = eol.join(v["cu"])
            n = nd.count(kcu)
            if n != 1:
                print("!!! LOI TO: %s - moc khop %d lan (can dung 1)" % (v["ten"], n))
                print("    moc:\n%s" % kcu.replace("\t", "->"))
                return 1
            nd = nd.replace(kcu, eol.join(v["moi"]))
            print("  %-14s %s" % (ten_tep, v["ten"]))
        if sum(1 for c in nd if ord(c) > 127) != hi0:
            print("!!! LOI TO: byte cao doi (%s)" % ten_tep)
            return 1
        them = "".join(eol.join(v["moi"]) for v in ds)
        bot = "".join(eol.join(v["cu"]) for v in ds)
        for cap in ("{}", "()"):
            if (them.count(cap[0]) - them.count(cap[1])) != \
               (bot.count(cap[0]) - bot.count(cap[1])):
                print("!!! LOI TO: ngoac %s lech (%s)" % (cap, ten_tep))
                return 1
        ketqua.append((tep, nd, ten_tep))

    ap_ini(False if not ghi else False)  # bao truoc, ghi that o duoi

    if not ketqua:
        print("Khong co gi de lam (ma nguon).")
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
    ap_ini(True)
    print("\nCAN BUILD LAI CA HAI cau hinh, roi khoi dong lai GameServer.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
