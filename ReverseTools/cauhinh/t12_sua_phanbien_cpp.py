# -*- coding: utf-8 -*-
"""t12_sua_phanbien_cpp.py - SUA cac loi phia C++/ini ma bo phan bien bat duoc.

LOI CHI MANG 1 - CA HAI NHOM [Exp] VA [Log] KHONG BAO GIO DOC DUOC.
  Toi ghi kieu `MocCap1       = 50` (co dau cach TRUOC dau =). Da TU DOC
  Engine\\Src\\KIniFile.cpp:573-584: SplitKeyValue cat tai dau '=' dau tien va
  KHONG cat khoang trang, nen ten khoa luu vao la "MocCap1       " (co 7 dau
  cach duoi) - bam ra ID khac han "MocCap1". Ket qua: chu game sua bao nhieu
  cung khong an gi, va se tuong minh sua sai.
  Khuon DUNG cua chinh tep do: `ExpRate=1`, `Skill90Rate=10<TAB>;chu thich`.
  GetInteger dung atoi (KIniFile.cpp) nen chu thich sau so la an toan; nhung
  GetString thi lay NGUYEN chuoi, nen TepLog TUYET DOI khong duoc kem chu thich.

LOI 2 - Ten tep log trong ini khong khop ma: ini con `TepLog = logs\\debug.log`
  (dau vet cua ban t06 da hoan tac) trong khi ma va moi tai lieu deu noi
  logs\\hethong.log.

LOI 3 - Khoi doc [Log] nam TRONG `#ifdef _SERVER` trong khi ba bien va ham lai
  nam NGOAI => ban client ket cung o gia tri mac dinh, khong doc duoc cong tac.
  Dung dung cai bay ma t05a2/t06d vua sua xong cho phan khai bao.

LOI 4 - Chu thich noi "mac dinh 0 = tat" nhung ma truyen mac dinh 1. Chon:
  giu BAT (he log chi ghi khi script goi GhiLog - hien chua ai goi nen khong
  ton gi, va muc dich cua ca dot nay la "co log de biet loi o dau"), nhung
  SUA CHU THICH cho dung su that. Khong de ma va chu thich noi hai dang.

LOI 5 - TRAN SO khi tinh nguong xoay vong: `(long)TranMB * 1024 * 1024`.
  `long` tren MSVC la 32 bit o CA Win32 LAN x64, nen TranMB >= 2048 lam tran ra
  so AM => xoay vong sau MOI DONG, mat sach log. Doi sang so hoc 64 bit.

LOI 6 - 23 khoa [Exp] doc vao KHONG kiem pham vi. Truoc day chung la hang so
  bien dich nen khong the sai; nay chu game go nham dau tru la exp dung hoac
  tut. Kep tung khoa ve khoang hop ly va ghi log khi phai kep.

LOI 7 - VA TRAN EXP CHUA HET: con hai phep nhan int nguyen ven o nhanh bua exp
  (KPlayer.cpp, khoi `m_CurrentExpEnhance`). Va nua voi khong va la tinh trang
  xau nhat. Doi not sang double cho ca chuoi.

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

CORE = r"D:\GAMEDEVNEW\Sources\Core\Src"
INI = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
       r"\settings\gamesetting.ini")
NHAN = "[PBCPP 29/08]"
T = "\t"

# khoang hop le de kep: (khoa, min, max)
KHOANG = [
    ("g_nExpMocCap1", 1, 200), ("g_nExpMocCap2", 1, 200),
    ("g_nExpMocCap3", 1, 200),
    ("g_nExpHeSo1", 1, 100000), ("g_nExpHeSo2", 1, 100000),
    ("g_nExpHeSo3", 1, 100000), ("g_nExpHeSo4", 1, 100000),
    ("g_nExpVipCong", 0, 100000),
    ("g_nExpCsMap", 0, 100000), ("g_nExpCsLanToiThieu", 0, 100),
    ("g_nExpCsDuoi", 1, 100000), ("g_nExpCsTren", 1, 100000),
    ("g_nExpChenhCapMax", 0, 200), ("g_nExpChiaKhiChenh", 1, 100000),
    ("g_nExpMienTruCap", 1, 200),
    ("g_nExpToDoi2", 1, 100), ("g_nExpToDoi3", 1, 100),
    ("g_nExpToDoi4", 1, 100), ("g_nExpToDoi5", 1, 100),
    ("g_nExpToDoi6", 1, 100), ("g_nExpToDoi7", 1, 100),
    ("g_nExpToDoi8", 1, 100), ("g_nExpToDoiKhac", 1, 100),
]


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t12_sua_phanbien_cpp - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))
    viec = []

    # ================= 1) gamesetting.ini: ghi lai dung khuon =================
    raw_i = doc(INI)
    if NHAN in raw_i:
        print("  gamesetting.ini DA VA - bo qua")
    else:
        eol = "\r\n" if raw_i.count("\r\n") >= (raw_i.count("\n")
                                               - raw_i.count("\r\n")) else "\n"
        # cat bo hai nhom cu (tu dong '; [CFGEXP...' hoac '[Exp]' den het tep)
        vt = raw_i.find("; [CFGEXP 29/08]")
        if vt < 0:
            vt = raw_i.find("[Exp]")
        if vt < 0:
            print("!!! LOI TO: khong thay nhom [Exp] trong ini")
            return 1
        dau = raw_i[:vt].rstrip()

        # kiem: phan giu lai KHONG con [Exp]/[Log]
        if "[Exp]" in dau or "[Log]" in dau:
            print("!!! LOI TO: cat khong sach - phan giu lai van con nhom cu")
            return 1

        kh_exp = [
            ("MocCap1", 50, "duoi cap nay dung HeSo1"),
            ("HeSo1", 80, "he so exp cho cap < MocCap1"),
            ("MocCap2", 80, "duoi cap nay dung HeSo2"),
            ("HeSo2", 70, "he so exp cho cap < MocCap2"),
            ("MocCap3", 140, "duoi cap nay dung HeSo3"),
            ("HeSo3", 280, "he so exp cho cap < MocCap3 (dang chi phoi 8x-13x)"),
            ("HeSo4", 100, "he so exp cho cap >= MocCap3"),
            ("VipCong", 20, "VIP cong them vao he so"),
            ("CsMap", 341, "map ap dung luat chuyen sinh"),
            ("CsLanToiThieu", 3, "so lan chuyen sinh toi thieu"),
            ("CsDuoi", 160, "he so chuyen sinh khi cap < MocCap3"),
            ("CsTren", 50, "he so chuyen sinh khi cap >= MocCap3"),
            ("ChenhCapMax", 9, "chenh cap toi da con an du exp"),
            ("ChiaKhiChenh", 10, "chenh qua muc thi chia cho so nay"),
            ("MienTruCap", 90, "tu cap nay tro len bo moi phat chenh cap"),
            ("ToDoi2", 80, "% exp moi nguoi khi to doi 2 nguoi"),
            ("ToDoi3", 70, "% exp moi nguoi khi to doi 3 nguoi"),
            ("ToDoi4", 60, "% exp moi nguoi khi to doi 4 nguoi"),
            ("ToDoi5", 55, "% exp moi nguoi khi to doi 5 nguoi"),
            ("ToDoi6", 55, "% exp moi nguoi khi to doi 6 nguoi"),
            ("ToDoi7", 50, "% exp moi nguoi khi to doi 7 nguoi"),
            ("ToDoi8", 50, "% exp moi nguoi khi to doi 8 nguoi"),
            ("ToDoiKhac", 60, "% exp khi so nguoi ngoai 2..8"),
        ]
        d = [
            "",
            "; " + NHAN + " QUAN TRONG VE CACH VIET: ten khoa KHONG duoc co dau",
            "; cach truoc dau '=' - KIniFile::SplitKeyValue cat tai dau '=' va",
            "; GIU NGUYEN dau cach trong ten khoa, khien khoa khong khop gi ca.",
            "; Viet dung: Ten=GiaTri<TAB>;chu thich",
            "",
            "; [CFGEXP 29/08] He so kinh nghiem - truoc day nam CUNG trong",
            "; KPlayer.cpp. Moi so duoi day DUNG BANG gia tri dang chay nen file",
            "; nay khong lam doi can bang. Sua xong phai KHOI DONG LAI GameServer.",
            "[Exp]",
        ]
        for k, v, g in kh_exp:
            d.append("%s=%d\t;%s" % (k, v, g))
        d += [
            "",
            "; [HELOG2 29/08] Ghi log he thong ra tep. Hien chi co ham Lua GhiLog",
            "; ghi vao day; bat san cung khong ton gi khi chua co script nao goi.",
            "; TepLog KHONG duoc kem chu thich - GetString lay NGUYEN ca dong.",
            "[Log]",
            "GhiTep=1\t;1 = ghi ra tep, 0 = tat",
            "TranMB=64",
            "TepLog=logs\\hethong.log",
        ]
        nd_i = dau + eol + eol.join(d) + eol

        # chot: moi dong khoa phai co dang Ten=... khong dau cach truoc '='
        for l in nd_i.split(eol):
            if "=" not in l or l.strip().startswith(";") or l.strip().startswith("["):
                continue
            ten = l.split("=")[0]
            if ten != ten.rstrip():
                print("!!! LOI TO: con dong co dau cach truoc '=': %r" % l)
                return 1
        print("  gamesetting.ini: ghi lai [Exp] (23 khoa) + [Log] dung khuon")
        viec.append((INI, nd_i, raw_i))

    # ================= 2) KCore.cpp =================
    p = os.path.join(CORE, "KCore.cpp")
    raw = doc(p)
    if NHAN in raw:
        print("  KCore.cpp DA VA - bo qua")
    else:
        eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n")
                                             - raw.count("\r\n")) else "\n"
        nd = raw

        # 2a) tran so nguong xoay vong
        cu = (T + "if (g_nLogHeThongTranMB > 0 &&" + eol
              + T*2 + "nCo > (long)g_nLogHeThongTranMB * 1024 * 1024)")
        if nd.count(cu) != 1:
            print("!!! LOI TO: khong khop dong tinh nguong xoay vong")
            return 1
        moi = (T + "// " + NHAN + " `long` tren MSVC la 32 bit o CA Win32 lan x64,"
               + eol
               + T + "// nen TranMB >= 2048 lam phep nhan tran ra SO AM => xoay vong"
               + eol
               + T + "// sau MOI DONG va mat sach log. So sanh bang phep chia."
               + eol
               + T + "if (g_nLogHeThongTranMB > 0 &&" + eol
               + T*2 + "(nCo / 1024 / 1024) > (long)g_nLogHeThongTranMB)")
        nd = nd.replace(cu, moi)
        print("  KCore.cpp: vá tràn ngưỡng xoay vòng".encode("ascii", "replace")
              .decode("ascii"))

        # 2b) dua khoi doc [Log] ra ngoai #ifdef _SERVER + kep pham vi [Exp]
        # chu thich con mang nhan cua t06 (HELOG) vi t06b chi doi 3 dong doc
        cu2 = (T*2 + "// [HELOG 29/08] nhom [Log]: bat ghi g_DebugLog ra tep.")
        if nd.count(cu2) != 1:
            print("!!! LOI TO: khong khop chu thich khoi [Log]")
            return 1
        # lay ca khoi 3 dong doc
        i0 = nd.find(cu2)
        i1 = nd.find("g_szTepLogHeThong));", i0)
        if i1 < 0:
            print("!!! LOI TO: khong thay het khoi doc [Log]")
            return 1
        i1 += len("g_szTepLogHeThong));")
        khoi = nd[i0:i1]
        # bo khoi khoi cho cu
        nd = nd[:i0].rstrip(" \t") + nd[i1:]
        # dat lai NGOAI #ifdef _SERVER: ngay truoc dong `#ifdef _SERVER` gan nhat
        # phia tren InitGameSetting
        moc_if = eol + "#ifdef _SERVER" + eol
        j = nd.rfind(moc_if, 0, i0)
        if j < 0:
            print("!!! LOI TO: khong thay vung chi-may-chu de dat lai")
            return 1
        khoi_moi = (eol
                    + T*2 + "// " + NHAN + " khoi nay phai nam NGOAI vung bien dich"
                    + eol
                    + T*2 + "// chi-may-chu: ba bien va ham g_GhiLogHeThong deu o"
                    + eol
                    + T*2 + "// ngoai, va LuaGhiLog bien dich cho ca client - de"
                    + eol
                    + T*2 + "// trong day thi ban client ket cung o gia tri mac dinh."
                    + eol
                    + T*2 + "// (mac dinh GhiTep = 1: he log chi ghi khi script goi"
                    + eol
                    + T*2 + "//  GhiLog, hien chua noi vao dau nen khong ton gi.)"
                    + eol
                    + khoi.replace(cu2 + eol, "").rstrip() + eol)
        nd = nd[:j] + khoi_moi + nd[j:]
        print("  KCore.cpp: dua khoi doc [Log] ra ngoai vung chi-may-chu")

        # 2c) kep pham vi 23 khoa [Exp]
        moc_kep = 'g_GameSetting.GetInteger("Exp", "ToDoiKhac", 60, &g_nExpToDoiKhac);'
        if nd.count(moc_kep) != 1:
            print("!!! LOI TO: khong thay dong doc khoa [Exp] cuoi cung")
            return 1
        kep = [moc_kep, "",
               T*2 + "// " + NHAN + " kep tung khoa ve khoang hop ly. Truoc ban va",
               T*2 + "// chung la hang so bien dich nen khong the sai; nay mot dau",
               T*2 + "// tru go nham trong ini co the lam exp dung hoac tut.",
               T*2 + "{",
               T*3 + "struct { int* p; int nMin; int nMax; const char* sz; } aKep[] = {"]
        for ten, lo, hi in KHOANG:
            kep.append(T*4 + "{ &%s, %d, %d, \"%s\" }," % (ten, lo, hi, ten))
        kep += [
            T*3 + "};",
            T*3 + "for (int iKep = 0; iKep < sizeof(aKep)/sizeof(aKep[0]); iKep++)",
            T*3 + "{",
            T*4 + "if (*aKep[iKep].p < aKep[iKep].nMin)",
            T*4 + "{",
            T*5 + 'g_DebugLog((LPSTR)"[Exp] %s = %d ngoai khoang, kep ve %d",',
            T*6 + "(LPSTR)aKep[iKep].sz, *aKep[iKep].p, aKep[iKep].nMin);",
            T*5 + "*aKep[iKep].p = aKep[iKep].nMin;",
            T*4 + "}",
            T*4 + "else if (*aKep[iKep].p > aKep[iKep].nMax)",
            T*4 + "{",
            T*5 + 'g_DebugLog((LPSTR)"[Exp] %s = %d ngoai khoang, kep ve %d",',
            T*6 + "(LPSTR)aKep[iKep].sz, *aKep[iKep].p, aKep[iKep].nMax);",
            T*5 + "*aKep[iKep].p = aKep[iKep].nMax;",
            T*4 + "}",
            T*3 + "}",
            T*2 + "}",
        ]
        nd = nd.replace(moc_kep, eol.join(kep))
        print("  KCore.cpp: kep pham vi 23 khoa [Exp]")
        viec.append((p, nd, raw))

    # ================= 3) KPlayer.cpp: va not 2 phep nhan int =================
    p2 = os.path.join(CORE, "KPlayer.cpp")
    raw2 = doc(p2)
    if NHAN in raw2:
        print("  KPlayer.cpp DA VA - bo qua")
    else:
        eol = "\r\n" if raw2.count("\r\n") >= (raw2.count("\n")
                                              - raw2.count("\r\n")) else "\n"
        cu3 = eol.join([
            T + "if(Npc[m_nIndex].m_CurrentExpEnhance > 0)",
            T + "{",
            T*2 + "if(Npc[m_nIndex].m_CurrentExpEnhance <= MAX_PERCENT)",
            T*3 + "nGetExp = nGetExp * (Npc[m_nIndex].m_CurrentExpEnhance"
                  " + MAX_PERCENT) / MAX_PERCENT;",
            T*2 + "else",
            T*3 + "nGetExp = nGetExp * Npc[m_nIndex].m_CurrentExpEnhance"
                  " / MAX_PERCENT;",
            T + "}",
        ])
        if raw2.count(cu3) != 1:
            print("!!! LOI TO: khong khop khoi bua exp (khop %d lan)"
                  % raw2.count(cu3))
            return 1
        moi3 = eol.join([
            T + "if(Npc[m_nIndex].m_CurrentExpEnhance > 0)",
            T + "{",
            T*2 + "// " + NHAN + " hai phep nhan nay cung tinh trong `int` nen cung",
            T*2 + "// tran nhu dong ben duoi (bua x2 exp lam nguong tran tut con",
            T*2 + "// mot nua). Va nua voi khong va la tinh trang xau nhat, nen doi",
            T*2 + "// ca chuoi sang double.",
            T*2 + "double dEnh = (double)nGetExp;",
            T*2 + "if(Npc[m_nIndex].m_CurrentExpEnhance <= MAX_PERCENT)",
            T*3 + "dEnh = dEnh * (double)(Npc[m_nIndex].m_CurrentExpEnhance"
                  " + MAX_PERCENT) / (double)MAX_PERCENT;",
            T*2 + "else",
            T*3 + "dEnh = dEnh * (double)Npc[m_nIndex].m_CurrentExpEnhance"
                  " / (double)MAX_PERCENT;",
            T*2 + "if (dEnh > 2147483647.0)",
            T*3 + "nGetExp = 2147483647;",
            T*2 + "else",
            T*3 + "nGetExp = (int)dEnh;",
            T + "}",
        ])
        nd2 = raw2.replace(cu3, moi3)
        print("  KPlayer.cpp: va not phep nhan int o nhanh bua exp")
        viec.append((p2, nd2, raw2))

    if not viec:
        print("Khong co gi de lam.")
        return 0
    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    for p, nd, raw in viec:
        if sum(1 for c in nd if ord(c) > 127) != sum(1 for c in raw if ord(c) > 127):
            print("!!! LOI TO: byte cao doi: %s" % os.path.basename(p))
            return 1
        sao = p + ".truoc_pbcpp"
        if not os.path.isfile(sao):
            shutil.copy2(p, sao)
        with io.open(p, "wb") as f:
            f.write(nd.encode("latin-1"))
        if doc(p) != nd:
            print("!!! LOI TO: doc lai KHONG khop: %s" % p)
            return 1
        print("  DA GHI %s" % os.path.basename(p))
    print("\nBUILD LAI Core CA HAI cau hinh.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
