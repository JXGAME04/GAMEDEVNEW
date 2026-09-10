# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 TTMT h] Chu: "da di theo nhung hien thi thong tin bi lech va mat ong mau do".
#
# GOC (doc ma + so ini, khong doan): thu nho ca thanh bang he so (va 85) KHONG the dung, vi chu la phong chu 12 px
# CO DINH - khong thu duoc. Ini goc: [Name] Top=37, [LifePercent] Top=54 (cach nhau 17 px, vua du cho chu cao 12).
# Nhan 50%: Top 18 va 27 - cach nhau 9 px < 12 -> HAI DONG DE NHAU = "hien thi bi lech". Con ong mau thi Core ve o
# (420-370)*50% = x+25, y 55*50% = y+27, trung dung cho dong chu % mau nen bi chu che.
#
# SUA: bo han cach nhan he so; viet BO CUC GON THANG TRONG INI (nhu ban VNKU lam voi cac cua so khac), va cho Core
# lay TOA DO ONG MAU TU INI ([Main] MauLeft/MauTop/MauRong/MauCao) thay vi so cung 420/55/120/10. Nho vay:
#   - Chu khong bao gio de nhau (khoang cach dong do minh dat, >= 13 px).
#   - Ong mau luon nam dung cho trong khung, o moi vi tri cua so.
#   - Van la ONG MAU GOC do Core ve (hai hinh chu nhat do + xam) - khong muon anh cua cua so khac.
# Bo cuc gon (~88x30 thay vi ~170x66): anh dau 24, ten mot dong, ong mau + % mau mot dong.
# ThongTinTiLe mac dinh ve 100 = khong nhan he so nua (van giu cong tac de chu thu neu muon).
# Windows: khong doi (rao JX_ANDROID; nhanh #else cua KNpc::PaintTargetInfo giu nguyen).

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 11/09 TTMT h]"
LIVE = "D:/jx1_android_data"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def nl_cua(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def thay(s, cu, moi, ten, so=1):
    nl = nl_cua(s)
    c = nl.join(cu)
    if s.count(c) == so:
        return s.replace(c, nl.join(moi))
    raise SystemExit("khong tim thay dung %d cho (%d): %s" % (so, s.count(c), ten))


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


def va(p, ham):
    s = doc(p)
    if DAU in s:
        print("da va roi, bo qua:", p)
        return
    s2 = ham(s)
    if cao(s) != cao(s2):
        raise SystemExit("so byte cao doi o " + p)
    ghi(p, s2)
    print("da va:", p)


# ---------------------------------------------------------------- struct: them toa do ong mau
def va_gamedatadef(s):
    return thay(s, ["\tint\t\tnTiLe;\t\t// [ANDROID 11/09 TTMT] ti le cua so (phan nghin, 1000 = 100%); 0 = chua dien -> Core dung 370,0,1000"],
                ["\tint\t\tnTiLe;\t\t// [ANDROID 11/09 TTMT] ti le cua so (phan nghin, 1000 = 100%); 0 = chua dien -> Core dung 370,0,1000",
                 "\tint\t\tnMauL;\t\t// %s cho ve ONG MAU, lech so voi goc cua so (doc tu [Main] MauLeft/MauTop/MauRong/MauCao)" % DAU,
                 "\tint\t\tnMauT;",
                 "\tint\t\tnMauW;",
                 "\tint\t\tnMauH;"], "GameDataDef.h: toa do ong mau")


# ---------------------------------------------------------------- UI: doc ini + gui xuong Core
def va_uitargetinfo(s):
    s = thay(s, ["\t\ts_nTtTiLe = GetPrivateProfileInt(\"Cham\", \"ThongTinTiLe\", 50, szCfg);"],
             ["\t\ts_nTtTiLe = GetPrivateProfileInt(\"Cham\", \"ThongTinTiLe\", 100, szCfg);\t// %s 100 = KHONG nhan he so (bo cuc gon nam trong ini)" % DAU],
             "ThongTinTiLe mac dinh 100")
    s = thay(s, ["\t\tIni.GetInteger(\"Main\", \"nHei_mana\",  4, &nHei_mana);"],
             ["\t\tIni.GetInteger(\"Main\", \"nHei_mana\",  4, &nHei_mana);",
              "#ifdef JX_ANDROID",
              "\t\t// %s cho ve ong mau lay TU INI (lech so voi goc cua so), khong con so cung 420/55/120/10" % DAU,
              "\t\tIni.GetInteger(\"Main\", \"MauLeft\",  50, &m_nMauL);",
              "\t\tIni.GetInteger(\"Main\", \"MauTop\",   55, &m_nMauT);",
              "\t\tIni.GetInteger(\"Main\", \"MauRong\", 120, &m_nMauW);",
              "\t\tIni.GetInteger(\"Main\", \"MauCao\",   10, &m_nMauH);",
              "#endif"], "LoadScheme: doc toa do ong mau")
    s = thay(s, ["\t\tnPainTMG->nTiLe = Tt_LayTiLe() * 10;\t// [ANDROID 11/09 TTMT b] ti le CUA O CON (UiDatTiLe khong thu o con) - phai khop ong mau"],
             ["\t\tnPainTMG->nTiLe = Tt_LayTiLe() * 10;\t// [ANDROID 11/09 TTMT b] ti le CUA O CON (UiDatTiLe khong thu o con) - phai khop ong mau",
              "\t\tnPainTMG->nMauL = m_nMauL;\t// %s" % DAU,
              "\t\tnPainTMG->nMauT = m_nMauT;",
              "\t\tnPainTMG->nMauW = m_nMauW;",
              "\t\tnPainTMG->nMauH = m_nMauH;"], "PaintWindow: gui toa do ong mau")
    return s


def va_uitargetinfo_h(s):
    return thay(s, ["\t// [ANDROID 11/09 TTMT e] ong mau ve theo dung cach GOC (Core: KNpc::PaintTargetInfo), khong muon anh cua cua so khac"],
                ["\t// [ANDROID 11/09 TTMT e] ong mau ve theo dung cach GOC (Core: KNpc::PaintTargetInfo), khong muon anh cua cua so khac",
                 "#ifdef JX_ANDROID",
                 "\tint\t\t\t\tm_nMauL, m_nMauT, m_nMauW, m_nMauH;\t// %s cho ve ong mau, doc tu [Main] cua kuitargetinfo.ini" % DAU,
                 "#endif"], "UiTargetInfo.h: bien toa do ong mau")


# ---------------------------------------------------------------- Core: ve theo toa do ini
def va_knpc(s):
    s = thay(s, ["\t\tnWid = nWid * nTtTL / 1000;"],
             ["\t\tif (nPainTMG->nMauW > 0)",
              "\t\t\tnWid = nPainTMG->nMauW;\t// %s be rong ong mau lay tu ini" % DAU,
              "\t\telse",
              "\t\t\tnWid = nWid * nTtTL / 1000;"], "KNpc.cpp: rong ong mau")
    s = thay(s, ["\t\tBlood.oPosition.nX = nTtX0 + (420 - 370) * nTtTL / 1000;\t// [ANDROID 11/09 TTMT]",
                 "\t\tBlood.oPosition.nY = nTtY0 + (65 + i * verDistance - 10) * nTtTL / 1000;"],
                ["\t\t// %s cho ve lay tu ini ([Main] MauLeft/MauTop) - khong con so cung 420/55" % DAU,
                 "\t\tBlood.oPosition.nX = (nPainTMG->nMauW > 0) ? (nTtX0 + nPainTMG->nMauL)",
                 "\t\t\t: (nTtX0 + (420 - 370) * nTtTL / 1000);",
                 "\t\tBlood.oPosition.nY = (nPainTMG->nMauW > 0) ? (nTtY0 + nPainTMG->nMauT + i * verDistance)",
                 "\t\t\t: (nTtY0 + (65 + i * verDistance - 10) * nTtTL / 1000);"], "KNpc.cpp: cho ve ong mau")
    s = thay(s, ["\t\tBlood.oEndPos.nY = nTtY0 + (75 + i * verDistance - 10) * nTtTL / 1000;\t// [ANDROID 11/09 TTMT]",
                 "\t\tif (Blood.oEndPos.nY - Blood.oPosition.nY < 6)",
                 "\t\t\tBlood.oEndPos.nY = Blood.oPosition.nY + 6;\t// [ANDROID 11/09 TTMT b] thu nho nhieu thi ong mau van day >= 6 px"],
                ["\t\tBlood.oEndPos.nY = (nPainTMG->nMauW > 0) ? (Blood.oPosition.nY + nPainTMG->nMauH)\t// %s" % DAU,
                 "\t\t\t: (nTtY0 + (75 + i * verDistance - 10) * nTtTL / 1000);",
                 "\t\tif (Blood.oEndPos.nY - Blood.oPosition.nY < 6)",
                 "\t\t\tBlood.oEndPos.nY = Blood.oPosition.nY + 6;\t// [ANDROID 11/09 TTMT b] ong mau luon day >= 6 px"],
                "KNpc.cpp: day ong mau tu ini")
    return s


# ---------------------------------------------------------------- ini: bo cuc gon
def dat_khoa(dong, muc, khoa, gt):
    """Dat khoa=gt trong muc; them moi neu chua co."""
    i = -1
    for k, d in enumerate(dong):
        if d.strip() == muc:
            i = k
            break
    if i < 0:
        raise SystemExit("khong thay muc " + muc)
    j = i + 1
    cuoi = i
    while j < len(dong):
        t = dong[j].strip()
        if t.startswith("[") and t.endswith("]"):
            break
        if t:
            cuoi = j
        if t.lower().startswith(khoa.lower() + "="):
            dong[j] = "%s=%d" % (khoa, gt)
            return
        j += 1
    dong.insert(cuoi + 1, "%s=%d" % (khoa, gt))


def va_ini(p):
    s = doc(p)
    if DAU in s:
        print("ini da va roi, bo qua:", p)
        return
    nl = nl_cua(s)
    dong = s.split(nl)
    # bo cuc gon: anh dau 24 px ben trai, ten mot dong, ong mau + % mau mot dong
    for muc, bo in (
        ("[BackGround0]", (("Left", 0), ("Top", 0), ("Width", 88), ("Height", 30))),
        ("[Head0]",       (("Left", 3), ("Top", 3), ("Width", 24), ("Height", 24))),
        ("[Name]",        (("Left", 29), ("Top", 2), ("Width", 58), ("Height", 12))),
        ("[LifePercent]", (("Left", 29), ("Top", 16), ("Width", 58), ("Height", 12))),
        ("[Main]",        (("MauLeft", 29), ("MauTop", 17), ("MauRong", 58), ("MauCao", 9))),
    ):
        for k, v in bo:
            dat_khoa(dong, muc, k, v)
    dong.insert(0, "; %s Bo cuc GON cho dien thoai: anh dau 24 px, ten mot dong, ong mau + %% mau mot dong (~88x30)." % DAU)
    dong.insert(1, "; [Main] MauLeft/MauTop/MauRong/MauCao = cho ve ONG MAU (Core ve, lech so voi goc cua so).")
    dong.insert(2, "; Doi so o day la doi ngay, khong phai dung lai APK. Ban PC dung tep rieng, khong bi anh huong.")
    ghi(p, nl.join(dong))
    print("da va ini:", p)


va("Sources/Core/Src/GameDataDef.h", va_gamedatadef)
va("Sources/S3Client/Ui/UiCase/UiTargetInfo.h", va_uitargetinfo_h)
va("Sources/S3Client/Ui/UiCase/UiTargetInfo.cpp", va_uitargetinfo)
va("Sources/Core/Src/KNpc.cpp", va_knpc)
va_ini(LIVE + "/ui/Ui3/kuitargetinfo.ini")
va_ini("android/du_lieu_ghi_de/ui/ui3/kuitargetinfo.ini")
print("xong")
