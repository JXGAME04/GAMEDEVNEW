# -*- coding: utf-8 -*-
"""y2_log_kiem_ra_tep.py - Doi log [LOREN-KIEM] tu printf sang GHI TEP.

Log chan doan duong kiem nguyen lieu (w3) dung `printf`, tuc chi ra CONSOLE cua
GameServer - khong vao GameServer.log nen khong doc lai duoc. Doi sang ghi tep
`loren_kiem.log` canh GameServer de doc duoc.

Vi sao can: chu game bao "nguyen lieu khong dung luat ghep" (ma 8 =
FOUNDRY_RESULT_RULE_ERROR). Ma 8 sinh ra o HAI cho khac nhau, va hai cho ay sua
hai kieu khac han:
  (a) `Check()` - vector o CHINH: con mon THUA khong khop khoa `s_anKhoaThua`
  (b) `CheckTuChon()` - vector o PHU: mon o o tu chon khong khop
      `FRK_CompoundGold_EnhanceItem` (= ConsumeItem)
Log se in ro type / CHINH hay TUCHON / mon nao truot / genre-detail-ptc-cap cua
tung mon, nen doc mot cai la biet.

Nghi van dang manh (chua ket luan): log client cho thay o thu 8 (ItemBox) CO
mon. Theo `LR_UI_Lam(10, 9, 8, 6)` thi o 8 la o PHU, ma o phu chi nhan
ConsumeItem (Than Bi Khoang Thach / Thuy Tinh / Phuc Duyen...). Neu chu game bo
khoang thuong vao o lon giua man hinh (ItemBox) thi dung la ma 8. Nhung phai co
log moi chac.

Chi doi cach GHI, khong doi mot dong logic nao.
Tep dich: Sources\\Core\\Src\\KItemCompound.cpp => build Core CA HAI cau hinh.
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_logtep lan dau).
"""
import io
import os
import shutil
import sys

T = "\t"
NHAN = "[LOREN 28/08] log kiem ghi ra tep"
DICH = r"D:\GAMEDEVNEW\Sources\Core\Src\KItemCompound.cpp"
HAU_TO = ".truoc_logtep"


def K(*d):
    return list(d)


HAM = K(
    "// " + NHAN + " - ghi ra tep de doc lai duoc (printf chi ra console).",
    "static void sLoRenGhiLog(const char* szDinhDang, ...)",
    "{",
    T + "FILE* f = fopen(\"loren_kiem.log\", \"a\");",
    T + "if (!f)",
    T*2 + "return;",
    T + "va_list ap;",
    T + "va_start(ap, szDinhDang);",
    T + "vfprintf(f, szDinhDang, ap);",
    T + "va_end(ap);",
    T + "fclose(f);",
    "}",
    "",
)

MIENG = [
    ("ham ghi tep",
     K("static void sLoRenInMon(const char* szNhan, int nThuTu, int nItemIdx)"),
     HAM + K("static void sLoRenInMon(const char* szNhan, int nThuTu, int nItemIdx)")),
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== y2_log_kiem_ra_tep - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH)
        return 1
    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)

    if NHAN in raw:
        print("  DA CO - bo qua (idempotent)")
        return 0
    if "sLoRenInMon" not in raw:
        print("!!! LOI TO: chua co log w3 - chay w3_log_kiemnguyenlieu.py --ghi truoc")
        return 1

    dong = raw.split(eol)
    for ten, cu, moi in MIENG:
        vt = [i for i in range(len(dong) - len(cu) + 1) if dong[i:i + len(cu)] == cu]
        if len(vt) != 1:
            print("  !!! %-20s khop %d lan (can 1)" % (ten, len(vt)))
            return 1
        print("  ok  %-20s dong %d" % (ten, vt[0] + 1))
        dong = dong[:vt[0]] + moi + dong[vt[0] + len(cu):]

    # doi moi printf("[LOREN-KIEM]...") thanh sLoRenGhiLog(...)
    n = 0
    for i, l in enumerate(dong):
        if "printf(\"[LOREN-KIEM]" in l:
            dong[i] = l.replace("printf(\"[LOREN-KIEM]", "sLoRenGhiLog(\"[LOREN-KIEM]")
            n += 1
    print("  ok  doi %d lenh printf -> sLoRenGhiLog" % n)
    if n < 3:
        print("!!! LOI TO: mong it nhat 3 lenh log")
        return 1

    nd = eol.join(dong)
    if "#include <stdarg.h>" not in nd:
        # chen sau include dau tien
        for i, l in enumerate(dong):
            if l.startswith("#include"):
                dong = dong[:i] + ["#include <stdarg.h>	// " + NHAN] + dong[i:]
                break
        nd = eol.join(dong)
        print("  ok  them #include <stdarg.h>")

    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi")
        return 1
    if nd.count("{") - raw.count("{") != nd.count("}") - raw.count("}"):
        print("!!! LOI TO: ngoac lech")
        return 1
    print("  byte cao %d (khong doi) | ngoac can bang" % hi0)

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    sao = DICH + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(DICH, sao)
    with io.open(DICH, "wb") as f:
        f.write(nd.encode("latin-1"))
    if io.open(DICH, "rb").read().decode("latin-1") != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI. => build Core, dat CoreServer.dll; log ra bin\\server\\loren_kiem.log")
    return 0


if __name__ == "__main__":
    sys.exit(main())
