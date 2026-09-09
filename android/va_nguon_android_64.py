# -*- coding: utf-8 -*-
#
# [ANDROID 10/09 MENU] Chu: menu chon kenh chat "bi chen chu - mat 1/3 chu".
#
# [ANDROID 09/09 CHAM] noi dem tren/duoi moi dong menu len 7 cho de cham, va chi noi DONG khi dong cao
# con la mac dinh (nItemHeight == MENU_ITEM_DEFAULT_HEIGHT). Menu kenh chat (KUiPlayerBar::
# PopupChannelMenu) dat san nItemHeight = chieu cao anh kenh (~22) -> chu ve tu y+7, co 12 + dau tieng
# Viet cao ~15 -> tran qua 22, dong sau ve de len -> mat phan duoi chu.
# Sua: dong cao do ben goi dat san ma nho hon (chu * so dong + 2 * dem) thi noi len cho vua.

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
DAU = "[ANDROID 10/09 MENU]"
P = "Sources/S3Client/Ui/Elem/PopupMenu.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
    raise SystemExit
NL = "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"
CU = NL.join([
    "\tif (m_pMenu->nItemHeight < 1)",
    "\t\tm_pMenu->nItemHeight = 1;",
])
assert s.count(CU) == 1, "khong tim thay chan nItemHeight < 1 (%d)" % s.count(CU)
MOI = NL.join([
    "#ifdef JX_ANDROID",
    "\t// %s Dong cao do ben goi dat san (menu kenh chat = cao anh kenh ~22) nhung dem tren da noi" % DAU,
    "\t// len 7 ([ANDROID 09/09 CHAM]) -> chu 12 + dau tieng Viet tran xuong, dong sau ve de len (\"mat 1/3",
    "\t// chu\"). Noi dong len cho vua chu.",
    "\t{",
    "\t\tint nCan = m_pMenu->byFontSize * nMaxItemHei + 2 * m_pMenu->byItemTitleUpSpace;",
    "",
    "\t\tif (m_pMenu->nItemHeight < nCan)",
    "\t\t\tm_pMenu->nItemHeight = nCan;",
    "\t}",
    "#endif",
    CU,
])
s = s.replace(CU, MOI)
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("da va:", P, "(ket dong %s)" % ("CRLF" if NL == "\r\n" else "LF"))
