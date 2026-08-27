# -*- coding: utf-8 -*-
"""v25 - tach cau truc goi xuc xac ra 'Headers/KDiceProtocol.h' (guard rieng).

LY DO (phat hien khi phan bien ban va v24):

  * Cay nay co HAI tep KProtocol.h khac nhau, CUNG mot include guard
    "KPROTOCOL_H" -> trong mot don vi bien dich, ban nao vao truoc thi ban kia
    bi nuot IM LANG.
  * Ba tep cua client nap thang ban Headers bang DUONG DAN TUONG MINH:
        Sources\\S3Client\\NetConnect\\NetConnectAgent.h:8
        Sources\\S3Client\\Ui\\UiCase\\UiItem.cpp:29
        Sources\\S3Client\\Ui\\UiCase\\UiPlayerBar.cpp:39
    (nen thu tu AdditionalIncludeDirectories KHONG cuu duoc)
  * Hai ban dinh nghia SViewSellItemInfo lech nhau 44 byte:
        Headers  : int m_btMagicLevel[6]                    , khong co m_nNature
        Core\\Src : int m_btMagicLevel[MAX_ITEM_MAGICLEVEL]=16, co m_nNature

  => v24 nhung SViewSellItemInfo vao DICE_ITEM_SYNC, nen sizeof() se KHAC NHAU
     giua cac don vi bien dich -> dung cai bay "lech do dai goi" ma chu game
     dan phai tranh. Ban va nay thay bang mot ban mo ta TU CHUA.

Cach lam: xoa khoi cau truc da chen o Sources/Core/Src/KProtocol.h, thay bang
`#include "KDiceProtocol.h"`, va them cung dong do vao Headers/KProtocol.h -
guard rieng nen nap hai lan van an toan.
"""
import io
import os
import shutil
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

CAY = r"D:\GAMEDEVNEW"
HAU_TO = ".truoc_xucxac_2608"
F_MOI = os.path.join(CAY, "Headers", "KDiceProtocol.h")
F_H_CORE = os.path.join(CAY, "Sources", "Core", "Src", "KProtocol.h")
F_H_HDR = os.path.join(CAY, "Headers", "KProtocol.h")

# khoi ma v24 da chen (phai xoa di)
KHOI_V24_DAU = "// ---- he XUC XAC chia do (DICEITEM) ----"
KHOI_V24_CUOI = "} DICE_CHOICE_DATA;\r\n"

NAP = ('// he XUC XAC chia do - cau truc goi nam o tep rieng vi trong cay nay co\r\n'
       '// HAI ban KProtocol.h dung chung include guard; xem ghi chu dau KDiceProtocol.h\r\n'
       '#include "KDiceProtocol.h"\r\n')


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def ghi(p, s):
    bak = p + HAU_TO
    if not os.path.isfile(bak):
        shutil.copy2(p, bak)
    b = s.encode("latin-1")
    # Headers/KProtocol.h VON DA co san vai dong LF le tu truoc; khong doi hoi
    # ve 0, chi doi hoi ban va KHONG LAM TANG THEM.
    goc = io.open(bak, "rb").read()
    le_goc = goc.count(b"\n") - goc.count(b"\r\n")
    le_moi = b.count(b"\n") - b.count(b"\r\n")
    if le_moi > le_goc:
        raise SystemExit("!! %s: LF le tang %d -> %d" % (p, le_goc, le_moi))
    if le_goc:
        print("   (ghi chu: %s von co san %d dong LF le - giu nguyen)"
              % (os.path.basename(p), le_goc))
    tmp = p + ".tmp"
    io.open(tmp, "wb").write(b)
    os.replace(tmp, p)


def main():
    # 0. tep header moi phai la CRLF
    raw = io.open(F_MOI, "rb").read()
    if raw.count(b"\n") != raw.count(b"\r\n"):
        raw = raw.replace(b"\r\n", b"\n").replace(b"\n", b"\r\n")
        io.open(F_MOI, "wb").write(raw)
        print("   > da doi KDiceProtocol.h sang CRLF (%d dong)" % raw.count(b"\r\n"))
    else:
        print("   = KDiceProtocol.h da la CRLF")

    # 1. bo khoi cau truc v24 trong Core/Src/KProtocol.h, thay bang #include
    d = doc(F_H_CORE)
    i = d.find(KHOI_V24_DAU)
    if i < 0:
        if '#include "KDiceProtocol.h"' in d:
            print("   = Core/Src/KProtocol.h da nap KDiceProtocol.h")
        else:
            print("!! khong tim thay khoi v24 trong Core/Src/KProtocol.h")
            return 2
    else:
        j = d.find(KHOI_V24_CUOI, i)
        if j < 0:
            print("!! khong tim thay cuoi khoi v24")
            return 2
        j += len(KHOI_V24_CUOI)
        d = d[:i] + NAP + d[j:]
        ghi(F_H_CORE, d)
        print("   > Core/Src/KProtocol.h: thay khoi cau truc bang #include")

    # 2. them #include vao ban Headers (de don vi bien dich nao an ban nay
    #    cung thay duoc cau truc)
    d = doc(F_H_HDR)
    if '#include "KDiceProtocol.h"' in d:
        print("   = Headers/KProtocol.h da nap KDiceProtocol.h")
    else:
        # ban Headers KHONG co VIEW_ITEM_SYNC -> neo vao SViewSellItemInfo
        neo = "} SViewSellItemInfo;\r\n"
        if d.count(neo) != 1:
            print("!! mo neo VIEW_ITEM_SYNC trong Headers/KProtocol.h khong duy nhat (%d)"
                  % d.count(neo))
            return 2
        d = d.replace(neo, neo + "\r\n" + NAP, 1)
        ghi(F_H_HDR, d)
        print("   > Headers/KProtocol.h: them #include KDiceProtocol.h")

    # 3. kiem lai
    print()
    for p in (F_H_CORE, F_H_HDR):
        s = doc(p)
        print("   %-46s nap KDiceProtocol.h: %s | con DICE_ITEM_SYNC noi tuyen: %s"
              % (os.path.basename(p),
                 "CO" if '#include "KDiceProtocol.h"' in s else "KHONG",
                 "CO" if "} DICE_ITEM_SYNC;" in s else "khong"))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
