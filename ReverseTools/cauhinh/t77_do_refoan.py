# -*- coding: utf-8 -*-
"""t77_do_refoan.py - them nhan [REFOAN] de NGHIEM THU ban va t76.

VI SAO CAN:
    Ban va t76 go DecRef thua trong KNpc::OnRevive. Neu khong co cach do, muon biet
    va co an hay khong thi chi con cach "vao game danh thu xem co trung khong" -
    dung dung cai kieu mo ho da lam mat ca ngay hom nay.

DAT O DAU: KRegion::DecRef, ngay TRONG nhanh that bai da co san (KRegion.cpp:1194):
        if (nRef == 0)
            return FALSE;        <-- co ai do vua tru mot o VON DA RONG
    Nhanh nay CHI chay khi da co loi ke toan, nen o duong chay binh thuong chi phi
    dung bang 0 (khong them mot phep so sanh nao vao duong nong).

DOC KET QUA:
    TRUOC va : moi con quai Viem De chet la mot lan tru oan -> hang tram dong/tran.
    SAU va   : phai ve gan 0. Neu VAN con nhieu -> con nguon lech KHAC, mo lai dieu tra.

KHONG kep nhip (KHONG dung AUTOLOG_EVERY): sang nay chinh toi da suyt doc sai
    [DMG-SKIP-TARGETKIND] 3808/3809 vi hai bo dem deu bi kep 500ms. Bo dem ma bi kep
    nhip thi khong con la bo dem. He AutoLog von da tu chan tran 1200 dong/giay va
    dem so dong bi bo (KCore.cpp:962-968) nen khong so phinh tep.

Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import shutil
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

P = r"D:\GAMEDEVNEW\Sources\Core\Src\KRegion.cpp"
MOC = "[REFOAN]"

CU = (
    "\t\tif (nRef == 0)\n"
    "\t\t\treturn FALSE;\n"
    "\t\tpBuffer[index]--;\n"
)

MOI = (
    "\t\tif (nRef == 0)\n"
    "\t\t{\n"
    "\t\t\t// [DO 31/08] Ai do vua tru mot o VON DA RONG => ke toan AddRef/DecRef da lech.\n"
    "\t\t\t// O nao bi tru oan se doc ra 0 trong khi van con NPC dung do, va KRegion::FindNpc\n"
    "\t\t\t// (KRegion.h:191) thoat ngay o cua chan == 0 => moi thu dung tren o ay TANG HINH\n"
    "\t\t\t// truoc va cham. Nhan nay de nghiem thu ban va KNpc.cpp:2344 (DecRef thua trong\n"
    "\t\t\t// OnRevive nhanh m_bNoRevive). Chi chay trong nhanh VON DA that bai => duong\n"
    "\t\t\t// chay binh thuong khong ton them gi. KHONG kep nhip: bo dem bi kep khong con\n"
    "\t\t\t// la bo dem (AutoLog da tu chan tran 1200 dong/giay).\n"
    "\t\t\tAUTOLOG(\"[REFOAN] DecRef tru o RONG: o=(%d,%d) type=%d rgn=(%d,%d)\",\n"
    "\t\t\t\tnMapX, nMapY, (int)nType, (int)LOWORD(m_RegionID), (int)HIWORD(m_RegionID));\n"
    "\t\t\treturn FALSE;\n"
    "\t\t}\n"
    "\t\tpBuffer[index]--;\n"
)


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def hi(s):
    return sum(1 for c in s if ord(c) > 127)


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t77 them nhan [REFOAN] de nghiem thu - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    print()

    raw = doc(P)
    if MOC in raw:
        print("  da them roi - bo qua")
        return 0

    crlf = "\r\n" in raw
    tam = raw.replace("\r\n", "\n")

    n = tam.count(CU)
    if n != 1:
        print("!!! LOI TO: neo khop %d lan (can dung 1). DUNG LAI." % n)
        return 1

    for i, l in enumerate(tam.split("\n"), 1):
        if l.strip() == "if (nRef == 0)":
            print("  chen sau dong %d (trong KRegion::DecRef)" % i)
            break

    hi0 = hi(tam)
    nd = tam.replace(CU, MOI)

    if hi(nd) != hi0:
        print("!!! LOI TO: byte cao doi (%d -> %d)" % (hi0, hi(nd)))
        return 1
    if nd.count("{") - tam.count("{") != 1 or nd.count("}") - tam.count("}") != 1:
        print("!!! LOI TO: ngoac nhon khong can (them %d mo / %d dong)"
              % (nd.count("{") - tam.count("{"), nd.count("}") - tam.count("}")))
        return 1
    # khong duoc dong cham toi so lan DecRef/AddRef
    for t in ("pBuffer[index]--", "pBuffer[index]++", "return TRUE"):
        if nd.count(t) != tam.count(t):
            print("!!! LOI TO: so lan '%s' doi" % t)
            return 1

    print("  byte cao giu nguyen: %d" % hi0)
    print("  ngoac nhon: can (+1 mo, +1 dong)")
    print("  khong dong toi pBuffer[index]--/++ va return TRUE")

    if crlf:
        nd = nd.replace("\n", "\r\n")

    if not ghi:
        print()
        print("DIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    sao = P + ".truoc_refoan_3108"
    if not os.path.isfile(sao):
        shutil.copy2(P, sao)
        print("  sao luu: %s" % sao)
    with io.open(P, "wb") as f:
        f.write(nd.encode("latin-1"))
    if doc(P) != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI KRegion.cpp")
    return 0


if __name__ == "__main__":
    sys.exit(main())
