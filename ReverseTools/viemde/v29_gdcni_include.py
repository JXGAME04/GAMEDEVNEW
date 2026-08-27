# -*- coding: utf-8 -*-
"""v29 - hai manh cuoi de phia may chu + phia client bien dich duoc:
   1. GDCNI_DICE_ITEM vao enum trong CoreShell.h (client: s2cDiceItem dung toi)
   2. #include "KItemDice.h" trong KProtocolProcess.cpp (may chu: c2sDiceItem dung
      g_ItemDiceSet / KItemDice)

Ca hai tep deu CRLF (CoreShell.h co san 1 dong LF le tu truoc - giu nguyen).
"""
import io
import os
import shutil
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

CAY = r"D:\GAMEDEVNEW\Sources\Core\Src"
HAU_TO = ".truoc_xucxac_2608"

F_CS = os.path.join(CAY, "CoreShell.h")
F_PP = os.path.join(CAY, "KProtocolProcess.cpp")

NEO_GDCNI = "\tGDCNI_PLAYER_BAUCUA_RESULT_SYNC,\r\n"
GDCNI = NEO_GDCNI + (
    "\r\n"
    "\t// He XUC XAC chia do (DICEITEM) 26/08: may chu bao mo/dong/cap nhat mot o.\r\n"
    "\t// uParam = con tro DICE_ITEM_SYNC, CHI SONG trong loi goi - phai chep ra ngay.\r\n"
    "\tGDCNI_DICE_ITEM,\r\n")

# KProtocolProcess.cpp: dat include canh cac include cung nhom
NEO_INC = '#include "KProtocolProcess.h"\r\n'
INC = NEO_INC + '#include "KItemDice.h"\t// 26/08: he xuc xac chia do (DICEITEM)\r\n'


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def ghi(p, s):
    bak = p + HAU_TO
    if not os.path.isfile(bak):
        shutil.copy2(p, bak)
    b = s.encode("latin-1")
    goc = io.open(bak, "rb").read()
    le_goc = goc.count(b"\n") - goc.count(b"\r\n")
    le_moi = b.count(b"\n") - b.count(b"\r\n")
    if le_moi > le_goc:
        raise SystemExit("!! %s: dong LF le tang %d -> %d" % (p, le_goc, le_moi))
    if le_goc:
        print("   (ghi chu: %s von co san %d dong LF le - giu nguyen)"
              % (os.path.basename(p), le_goc))
    tmp = p + ".tmp"
    io.open(tmp, "wb").write(b)
    os.replace(tmp, p)


def main():
    # 1. GDCNI
    d = doc(F_CS)
    if "GDCNI_DICE_ITEM" in d:
        print("   = CoreShell.h da co GDCNI_DICE_ITEM")
    else:
        if d.count(NEO_GDCNI) != 1:
            print("!! mo neo GDCNI khong duy nhat (%d)" % d.count(NEO_GDCNI))
            return 2
        ghi(F_CS, d.replace(NEO_GDCNI, GDCNI, 1))
        print("   > CoreShell.h: them GDCNI_DICE_ITEM")

    # 2. include
    d = doc(F_PP)
    if '#include "KItemDice.h"' in d:
        print("   = KProtocolProcess.cpp da nap KItemDice.h")
    else:
        if d.count(NEO_INC) != 1:
            print("!! mo neo include khong duy nhat (%d)" % d.count(NEO_INC))
            return 2
        ghi(F_PP, d.replace(NEO_INC, INC, 1))
        print("   > KProtocolProcess.cpp: them #include KItemDice.h")

    print()
    print("   CoreShell.h GDCNI_DICE_ITEM: %s"
          % ("CO" if "GDCNI_DICE_ITEM" in doc(F_CS) else "KHONG"))
    print("   KProtocolProcess.cpp nap KItemDice.h: %s"
          % ("CO" if '#include "KItemDice.h"' in doc(F_PP) else "KHONG"))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
