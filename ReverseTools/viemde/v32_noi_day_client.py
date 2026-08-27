# -*- coding: utf-8 -*-
"""v32 - noi day phia CLIENT cho he xuc xac.

  1. CoreShell.h        : GOI_DICE_CHOICE vao CUOI enum GAMEOPERATION_INDEX
  2. KProtocol.h        : nguyen mau SendClientDiceItem
  3. KProtocol.cpp      : than SendClientDiceItem (trong khoi #ifndef _SERVER)
  4. CoreShell.cpp      : case GOI_DICE_CHOICE -> SendClientDiceItem
  5. GameSpaceChangedNotify.cpp : #include + case GDCNI_DICE_ITEM
  6. UiShell.cpp        : #include + 3 cho dang ky cua so

Vi sao GOI_DICE_CHOICE dat CUOI enum: enum nay do CA Core (CoreClient.dll) lan
S3Client (Game.exe) bien dich; chen giua se DICH gia tri cua moi thanh vien sau
no, build lech mot ben la goi nham lenh.

Moi tep deu CRLF (UiShell.cpp co san 2 dong LF le - giu nguyen).
"""
import io
import os
import shutil
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

HAU_TO = ".truoc_xucxac_2608"
CORE = r"D:\GAMEDEVNEW\Sources\Core\Src"
UI = r"D:\GAMEDEVNEW\Sources\S3Client\Ui"

F_CSH = os.path.join(CORE, "CoreShell.h")
F_KPH = os.path.join(CORE, "KProtocol.h")
F_KPC = os.path.join(CORE, "KProtocol.cpp")
F_CSC = os.path.join(CORE, "CoreShell.cpp")
F_GSC = os.path.join(UI, "GameSpaceChangedNotify.cpp")
F_USH = os.path.join(UI, "UiShell.cpp")

VA = []

# 1 -------------------------------------------------------------- CoreShell.h
NEO = ("\tGOI_TASKGUIDE_GOTO_SATTHU,\t// [3HD C20] bam nhiem vu Sat Thu tren F11 "
       "-> tu chay toi boss (bam lai = huy)\r\n")
VA.append((F_CSH, NEO, NEO +
           "\r\n"
           "\t// He XUC XAC chia do (DICEITEM) 26/08: nguoi choi bam nut trong cua so.\r\n"
           "\t// uParam = ma phien xuc xac, nParam = DICE_CHOICE_GIVEUP / DICE_CHOICE_NEED.\r\n"
           "\t// Dat o CUOI enum: enum nay do CA Core lan S3Client bien dich, chen giua\r\n"
           "\t// se dich gia tri cua moi thanh vien sau no.\r\n"
           "\tGOI_DICE_CHOICE,\r\n",
           "CoreShell.h: GOI_DICE_CHOICE (cuoi enum)"))

# 2 -------------------------------------------------------------- KProtocol.h
NEO = "void SendClientBaucua(char* Data);\r\n"
VA.append((F_KPH, NEO, NEO +
           "void SendClientDiceItem(int nDiceId, int nChoice);\t// DICEITEM 26/08\r\n",
           "KProtocol.h: nguyen mau SendClientDiceItem"))

# 3 ------------------------------------------------------------ KProtocol.cpp
NEO = ("void SendClientBaucua(char* Data)\r\n"
       "{\r\n"
       "\tBAUCUA_DATA BauCuaData;\r\n"
       "\r\n"
       "\tBauCuaData.ProtocolType = c2s_baucua;\r\n"
       "\tmemcpy((void*)&BauCuaData.Data, Data, sizeof(BauCuaData.Data));\r\n"
       "\tif (g_pClient)\r\n"
       "\t\tg_pClient->SendPackToServer((BYTE*)&BauCuaData, sizeof(BAUCUA_DATA));\r\n"
       "}\r\n")
VA.append((F_KPC, NEO, NEO +
           "\r\n"
           "// He XUC XAC chia do (DICEITEM): nguoi choi chon \"Tham du nhan\" / \"Huy bo nhan\".\r\n"
           "// Truyen thang hai so thay vi con tro - khoi lo vong doi bo nho ben UI.\r\n"
           "void SendClientDiceItem(int nDiceId, int nChoice)\r\n"
           "{\r\n"
           "\tDICE_CHOICE_DATA\tData;\r\n"
           "\r\n"
           "\tData.ProtocolType = (BYTE)c2s_diceitem;\r\n"
           "\tData.m_nDiceId    = nDiceId;\r\n"
           "\tData.m_btChoice   = (BYTE)((nChoice == DICE_CHOICE_NEED)\r\n"
           "\t\t? DICE_CHOICE_NEED : DICE_CHOICE_GIVEUP);\r\n"
           "\tif (g_pClient)\r\n"
           "\t\tg_pClient->SendPackToServer((BYTE*)&Data, sizeof(DICE_CHOICE_DATA));\r\n"
           "}\r\n",
           "KProtocol.cpp: than SendClientDiceItem"))

# 4 ------------------------------------------------------------- CoreShell.cpp
NEO = ("\tcase GOI_BAUCUA:\r\n"
       "\t{\r\n"
       "\t\tif (uParam)\r\n"
       "\t\t{\r\n"
       "\t\t\tSendClientBaucua((char*)uParam);\r\n"
       "\t\t}\r\n"
       "\t}\r\n"
       "\tbreak;\r\n")
VA.append((F_CSC, NEO, NEO +
           "\tcase GOI_DICE_CHOICE:\t// DICEITEM 26/08\r\n"
           "\t{\r\n"
           "\t\tSendClientDiceItem((int)uParam, nParam);\r\n"
           "\t}\r\n"
           "\tbreak;\r\n",
           "CoreShell.cpp: case GOI_DICE_CHOICE"))

# 5 --------------------------------------------- GameSpaceChangedNotify.cpp
NEO = '#include "UiCase/UiTrembleItem.h"\r\n'
VA.append((F_GSC, NEO, NEO + '#include "UiCase/UiDiceItem.h"\t// DICEITEM 26/08\r\n',
           "GameSpaceChangedNotify.cpp: #include UiDiceItem.h"))

NEO = "\tcase GDCNI_OPEN_TREMBLE_ITEM:\r\n"
VA.append((F_GSC,
           NEO,
           "\tcase GDCNI_DICE_ITEM:\t// DICEITEM 26/08: uParam = DICE_ITEM_SYNC*\r\n"
           "\t\tif (uParam)\r\n"
           "\t\t{\r\n"
           "\t\t\tKUiDiceItem::OnDiceMsg((void*)uParam);\r\n"
           "\t\t}\r\n"
           "\t\tbreak;\r\n" + NEO,
           "GameSpaceChangedNotify.cpp: case GDCNI_DICE_ITEM"))

# 6 ----------------------------------------------------------- UiShell.cpp
NEO = '#include "UiCase/UiTrembleItem.h"\r\n'
VA.append((F_USH, NEO, NEO + '#include "UiCase/UiDiceItem.h"\t// DICEITEM 26/08\r\n',
           "UiShell.cpp: #include UiDiceItem.h"))

NEO = "\t\t\tKUiTrembleItem::GetIfVisible() == NULL &&\r\n"
VA.append((F_USH, NEO, NEO + "\t\t\tKUiDiceItem::GetIfVisible() == NULL &&\r\n",
           "UiShell.cpp: danh sach GetIfVisible"))

NEO = "\t\tKUiTrembleItem::CloseWindow(true);\r\n"
VA.append((F_USH, NEO, NEO + "\t\tKUiDiceItem::CloseWindow(true);\r\n",
           "UiShell.cpp: CloseWindow(true)"))

NEO = "\tKUiTrembleItem::CloseWindow(bAll);\r\n"
VA.append((F_USH, NEO, NEO + "\tKUiDiceItem::CloseWindow(bAll);\r\n",
           "UiShell.cpp: CloseWindow(bAll)"))


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def ghi(p, s):
    bak = p + HAU_TO
    if not os.path.isfile(bak):
        shutil.copy2(p, bak)
    b = s.encode("latin-1")
    goc = io.open(bak, "rb").read()
    if (b.count(b"\n") - b.count(b"\r\n")) > (goc.count(b"\n") - goc.count(b"\r\n")):
        raise SystemExit("!! %s: sinh them dong LF le" % p)
    tmp = p + ".tmp"
    io.open(tmp, "wb").write(b)
    os.replace(tmp, p)


def main():
    theo_tep = {}
    for p, cu, moi, nhan in VA:
        theo_tep.setdefault(p, []).append((cu, moi, nhan))

    noidung = {}
    for p, ds in theo_tep.items():
        if not os.path.isfile(p):
            print("!! khong co tep:", p)
            return 2
        d = doc(p)
        for cu, moi, nhan in ds:
            if moi in d:
                print("   = da co san:", nhan)
                continue
            if d.count(cu) != 1:
                print("!! MO NEO khong duy nhat (%d lan): %s" % (d.count(cu), nhan))
                return 2
        noidung[p] = d
    print("moi mo neo deu duy nhat - bat dau sua\n")

    for p, ds in theo_tep.items():
        d = noidung[p]
        for cu, moi, nhan in ds:
            if moi in d:
                continue
            d = d.replace(cu, moi, 1)
            print("   > " + nhan)
        ghi(p, d)

    print()
    for p, ten in ((F_CSH, "GOI_DICE_CHOICE"), (F_KPH, "SendClientDiceItem"),
                   (F_KPC, "SendClientDiceItem"), (F_CSC, "GOI_DICE_CHOICE"),
                   (F_GSC, "KUiDiceItem"), (F_USH, "KUiDiceItem")):
        print("   %-30s %-22s %s"
              % (os.path.basename(p), ten, "CO" if ten in doc(p) else "!! KHONG"))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
