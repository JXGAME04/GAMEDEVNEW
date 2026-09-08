# -*- coding: utf-8 -*-
"""Vong 2 chong cat cut con tro (sau dump sap 12:42 KUiMailList::AddHeader, dia chi FFFFFFFFFCB9D3F0 = con tro 64-bit bi cat 32-bit):
 - Core: sNotify(int nCmd, int nParam) trong KMailClient/KAuctionClient/KChienLenhClient -> KNPARAM nParam (goi (KNPARAM)&struct roi
   chuyen vao CoreDataChanged); log '%d' cua nParam ep (int).
 - S3Client: sSendOp(int nOp, int nParam) UiAuction/UiChienLenh, sSendOp(int nOp, int nExtra, int nParam) UiMail -> KNPARAM nParam.
 - UiChatCentre.cpp: hai offsetof idiom (KNPARAM)&(((T*)0)->x) -> (int)(KNPARAM)... (gia tri nho, tranh C4244).
Byte-safe, chi doi ASCII."""
import io, re, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
S = ROOT + r"\Sources"
def rd(p): return io.open(p, "rb").read()
def wr(p, b, nb):
    assert sum(1 for c in b if c > 127) == sum(1 for c in nb if c > 127), p
    io.open(p, "wb").write(nb)
tot = 0
for f in (r"\Core\Src\KMailClient.cpp", r"\Core\Src\KAuctionClient.cpp", r"\Core\Src\KChienLenhClient.cpp"):
    p = S + f; b = rd(p)
    nb, n1 = re.subn(rb"static void sNotify\(int nCmd, int nParam\)", b"static void sNotify(int nCmd, KNPARAM nParam)\t// [X64 08/09] nParam mang con tro struct -> rong bang con tro", b)
    nb, n2 = re.subn(rb'(param=%d", nCmd, )nParam\)', lambda m: m.group(1) + b"(int)nParam)", nb)
    if nb != b: wr(p, b, nb)
    print(f, "sNotify:", n1, "log:", n2); tot += n1
for f in (r"\S3Client\Ui\UiCase\UiAuction.cpp", r"\S3Client\Ui\UiCase\UiChienLenh.cpp", r"\S3Client\Ui\UiCase\UiMail.cpp"):
    p = S + f; b = rd(p)
    nb, n1 = re.subn(rb"static void sSendOp\(int nOp, int nParam\)", b"static void sSendOp(int nOp, KNPARAM nParam)\t// [X64 08/09] nParam mang con tro", b)
    nb, n2 = re.subn(rb"static void sSendOp\(int nOp, int nExtra, int nParam\)", b"static void sSendOp(int nOp, int nExtra, KNPARAM nParam)\t// [X64 08/09] nParam mang con tro", nb)
    if nb != b: wr(p, b, nb)
    print(f, "sSendOp:", n1 + n2); tot += n1 + n2
p = S + r"\S3Client\Ui\UiCase\UiChatCentre.cpp"; b = rd(p)
nb = b.replace(b"SetCustomColorOffset((KNPARAM)&(((KUiPlayerItem*)0)->nData))", b"SetCustomColorOffset((int)(KNPARAM)&(((KUiPlayerItem*)0)->nData))")
nb = nb.replace(b"\t\t\t\t(KNPARAM)&(((KUiPlayerItem*)0)->Name));", b"\t\t\t\t(int)(KNPARAM)&(((KUiPlayerItem*)0)->Name));")
print("UiChatCentre offsetof:", int(nb != b)); wr(p, b, nb)
print("tong ham doi:", tot)
assert tot == 6, tot
