# -*- coding: utf-8 -*-
#
# [DPG 12/09 1024] Chu 17:30: "mot so man hinh mat luon icon mat cuoi va icon gui tin nhan / mot so loi cac icon
# goc trai bi ho ra ngoai / bang thong bao dang dang nhap va vao game chua nam giua".
#
# Bo giao dien JX1 co HAI ban: 800x600 va 1024x768. Game chon ban nao bang dieu kien "SCREEN_WIDTH == 1024" -
# DUNG BANG, o 51 cho (UiPlayerBar, UiInit, UiLogin, UiConnectInfo, UiSysMsgCentre, UiItem, UiChatCentre...).
# Bo cuc mobile (Ui\UiToaDo_MacDinh.ini) dung ban 800x600, nen khi khung ve roi DUNG 1024 thi game nhay sang ban
# 1024x768 con bang toa do van la cua ban kia -> do duoc o khung 1024x768:
#     KUiPlayerBar|Main   -7,165 co 1024x768   (cac khung khac: 800x600)
#     KUiPlayerBar|InputEdit 519,905  Face 749,903  SendBtn 779,903   <-- duoi day man hinh 768
# tuc la o go chat, icon mat cuoi va nut gui NAM NGOAI MAN HINH, dung nhu chu bao.
#
# Sua (chi Android): khong bao gio de khung ve rong DUNG 1024 - lech 2 diem anh khong ai thay, ma tranh duoc han
# ca 51 cho re nhanh. Vao ca hai duong dat do phan giai: tu tinh theo man hinh (KSdlApp) va doc tu config
# (S3Client LoadResolutionFromConfig, dung khi TheoManHinh=0).
import io
import os

os.chdir(r"D:\GAMEDEVNEW\.claude\worktrees\full-wauto-mobile-analysis-f781df")
DAU = "[DPG 12/09 1024]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def nl_cua(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def thay(s, cu, moi, ten):
    nl = nl_cua(s)
    c = nl.join(cu)
    if s.count(c) != 1:
        raise SystemExit("khong tim thay dung 1 cho (%d): %s" % (s.count(c), ten))
    return s.replace(c, nl.join(moi))


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


K = "Sources/S3Client/Platform/KSdlApp.cpp"
s = doc(K)
if DAU not in s:
    s0 = s
    s = thay(s, ["\tint nVeW = (nW * 100 / nHeSo) & ~1;",
                 "\tint nVeH = (nH * 100 / nHeSo) & ~1;"],
                ["\tint nVeW = (nW * 100 / nHeSo) & ~1;",
                 "\tint nVeH = (nH * 100 / nHeSo) & ~1;",
                 "\t//\t%s Bo giao dien JX1 co HAI ban (800x600 va 1024x768), game chon bang \"SCREEN_WIDTH == 1024\"" % DAU,
                 "\t//\to 51 cho. Bo cuc mobile dung ban 800x600, nen khung ve rong DUNG 1024 lam game nhay sang ban kia:",
                 "\t//\to go chat tut xuong duoi man (mat icon mat cuoi / nut gui), icon goc trai ho ra ngoai, bang",
                 "\t//\t\"dang dang nhap\" khong nam giua (chu bao 17:30). Lech 2 diem anh khong ai thay.",
                 "\tif (nVeW == 1024)",
                 "\t\tnVeW = 1026;"], "khung ve 1024")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (KSdlApp.cpp)")
    ghi(K, s)
    print("da va:", K)

S = "Sources/S3Client/S3Client.cpp"
s = doc(S)
if DAU not in s:
    s0 = s
    s = thay(s, ["\tSCREEN_WIDTH = GetPrivateProfileInt(\"Resolution\", \"Width\", SCREEN_WIDTH, configPath);",
                 "\tSCREEN_HEIGHT = GetPrivateProfileInt(\"Resolution\", \"Height\", SCREEN_HEIGHT, configPath);"],
                ["\tSCREEN_WIDTH = GetPrivateProfileInt(\"Resolution\", \"Width\", SCREEN_WIDTH, configPath);",
                 "\tSCREEN_HEIGHT = GetPrivateProfileInt(\"Resolution\", \"Height\", SCREEN_HEIGHT, configPath);",
                 "#ifdef JX_ANDROID",
                 "\t//\t%s Xem chu thich o KSdlApp.cpp: rong DUNG 1024 thi game doi sang bo giao dien 1024x768 trong khi" % DAU,
                 "\t//\tbang toa do cua ban mobile la cua bo 800x600 -> o go chat ra ngoai man hinh, bang thong bao lech.",
                 "\tif (SCREEN_WIDTH == 1024)",
                 "\t\tSCREEN_WIDTH = 1026;",
                 "#endif"], "doc config 1024")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (S3Client.cpp)")
    ghi(S, s)
    print("da va:", S)
print("xong")
