# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 TTMT] Chu: (1) "kich vao nguoi choi hien thong tin chua co dinh - lo di chuot di cho khac la doi muc tieu -
# dung la kich vao se tu bam theo nguoi choi khac de hien thi thong tin ma kich vao"; (2) "lam icon hien thi thong tin nguoi choi
# nho lai va di chuyen xuong goc trong ben phai man hinh - duoi may icon".
#
# (1) KHOAMT b: thanh thong tin (NPC_OI_TARGET_INFO nParam 0) lay muc tieu DA CHAM (g_nJxMucTieuKhoa) thang moi thu; cham dat
#     KHONG bo khoa (chi cham trung nguoi/NPC khac moi doi; NPC bien mat thi bo). Bo viec ep m_nPeapleIdx trong OnMouseMove (va 79)
#     de hover/Walk giu dung nhu PC. Ly do va 79 chua du: keo ngon = WM_MOUSEMOVE co MK_LBUTTON -> OnButtonMove (khong qua
#     OnMouseMove), va thanh chi lam moi moi 1 s.
# (2) Thanh mau cua KUiTargetInfo do Core ve o toa do CUNG (KNpc::PaintTargetInfo: 420,65) nen doi cho / thu nho cua so bang
#     UiToaDo thi thanh mau dung yen. Nay UI dua vi tri tuyet doi + ti le cua cua so (KUiPlayerPaintTeamMNG.nGocX/nGocY/nTiLe,
#     them cuoi struct, chi JX_ANDROID) va Core ve theo do. Bo cuc mac dinh: KUiTargetInfo|Main=868,300,700,0 (goc phai, duoi
#     hang icon, 70 %). Chu keo/thu them trong "Sua giao dien".
# Windows: khong doi (moi thu rao JX_ANDROID).

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 11/09 TTMT]"


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


def va(p, dau, ham):
    s = doc(p)
    if dau in s:
        print("da va roi, bo qua:", p)
        return
    s2 = ham(s)
    if cao(s) != cao(s2):
        raise SystemExit("so byte cao doi o " + p)
    ghi(p, s2)
    print("da va:", p)


def va_gamedatadef(s):
    return thay(s, ["\tint\t\tnHei_life;", "\tint\t\tnHei_mana;", "};"],
                ["\tint\t\tnHei_life;", "\tint\t\tnHei_mana;",
                 "#ifdef JX_ANDROID",
                 "\tint\t\tnGocX;\t\t// %s vi tri tuyet doi cua cua so KUiTargetInfo (UiToaDo co the doi cho)" % DAU,
                 "\tint\t\tnGocY;",
                 "\tint\t\tnTiLe;\t\t// %s ti le cua so (phan nghin, 1000 = 100%%); 0 = chua dien -> Core dung 370,0,1000" % DAU,
                 "#endif",
                 "};"], "GameDataDef.h: KUiPlayerPaintTeamMNG")


def va_uitargetinfo(s):
    s = thay(s, ["\t\t\tnPainTMG->nHei_mana = nHei_mana;"],
             ["\t\t\tnPainTMG->nHei_mana = nHei_mana;",
              "#ifdef JX_ANDROID",
              "\t\t\tnPainTMG->nGocX = 0; nPainTMG->nGocY = 0; nPainTMG->nTiLe = 1000;\t// %s PaintWindow dien lai moi khung" % DAU,
              "#endif"], "UiTargetInfo.cpp: dien nPainTMG")
    s = thay(s, ["\tg_pCoreShell->OperationRequest(GOI_DRAW_TARGET_INFO, (KUPARAM)m_pPlayersList, (KUPARAM)nPainTMG);"],
             ["#ifdef JX_ANDROID",
              "\tif (nPainTMG)",
              "\t{\t// %s Core ve thanh mau theo vi tri + ti le THAT cua cua so (chu doi cho / thu nho bang UiToaDo)" % DAU,
              "\t\tint nL = 0, nT = 0;",
              "\t\tGetAbsolutePos(&nL, &nT);",
              "\t\tnPainTMG->nGocX = nL;",
              "\t\tnPainTMG->nGocY = nT;",
              "\t\tnPainTMG->nTiLe = (UiLayTiLe() > 0) ? UiLayTiLe() : 1000;",
              "\t}",
              "#endif",
              "\tg_pCoreShell->OperationRequest(GOI_DRAW_TARGET_INFO, (KUPARAM)m_pPlayersList, (KUPARAM)nPainTMG);"],
             "UiTargetInfo.cpp: PaintWindow")
    return s


def va_knpc(s):
    nl = nl_cua(s)
    s = thay(s, ["\t\tint nWid = 120;// nPainTMG->nWid;//58;"],
             ["\t\tint nWid = 120;// nPainTMG->nWid;//58;",
              "#ifdef JX_ANDROID",
              "\t\t// %s ve theo vi tri + ti le that cua cua so KUiTargetInfo (goc ini 370,0); chua dien -> nhu cu" % DAU,
              "\t\tconst int nTtTL = (nPainTMG->nTiLe > 0) ? nPainTMG->nTiLe : 1000;",
              "\t\tconst int nTtX0 = (nPainTMG->nTiLe > 0) ? nPainTMG->nGocX : 370;",
              "\t\tconst int nTtY0 = (nPainTMG->nTiLe > 0) ? nPainTMG->nGocY : 0;",
              "\t\tnWid = nWid * nTtTL / 1000;",
              "#endif"], "KNpc.cpp: nWid")
    s = thay(s, ["\t\tBlood.oPosition.nX = 420;",
                 "\t\tBlood.oPosition.nY = 65 + i * verDistance - 10;"],
             ["#ifdef JX_ANDROID",
              "\t\tBlood.oPosition.nX = nTtX0 + (420 - 370) * nTtTL / 1000;\t// %s" % DAU,
              "\t\tBlood.oPosition.nY = nTtY0 + (65 + i * verDistance - 10) * nTtTL / 1000;",
              "#else",
              "\t\tBlood.oPosition.nX = 420;",
              "\t\tBlood.oPosition.nY = 65 + i * verDistance - 10;",
              "#endif"], "KNpc.cpp: oPosition")
    # dong co khoang trang cuoi: "Blood.oEndPos.nY = 75 + i * verDistance - 10; "
    dong = s.split(nl)
    idx = [i for i, d in enumerate(dong) if d.startswith("\t\tBlood.oEndPos.nY = 75 + i * verDistance - 10;")]
    if len(idx) != 1:
        raise SystemExit("KNpc.cpp: oEndPos.nY (%d)" % len(idx))
    i = idx[0]
    dong[i:i + 1] = ["#ifdef JX_ANDROID",
                     "\t\tBlood.oEndPos.nY = nTtY0 + (75 + i * verDistance - 10) * nTtTL / 1000;\t// %s" % DAU,
                     "#else",
                     dong[i],
                     "#endif"]
    return nl.join(dong)


def va_coreshell(s):
    return thay(s, ["\t\tint idx = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx;",
                    "\t\tint idx_hover = Player[CLIENT_PLAYER_INDEX].GetTargetNpc();",
                    "\t\tif (idx_hover)",
                    "\t\t\tidx = idx_hover;"],
                   ["\t\tint idx = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx;",
                    "\t\tint idx_hover = Player[CLIENT_PLAYER_INDEX].GetTargetNpc();",
                    "\t\tif (idx_hover)",
                    "\t\t\tidx = idx_hover;",
                    "#ifdef JX_ANDROID",
                    "\t\t{\t// %s [KHOAMT b] muc tieu DA CHAM (KPlayer::OnButtonDown) thang hover: keo ngon / re / cham dat khong doi;" % DAU,
                    "\t\t\t// chi cham trung nguoi/NPC khac moi doi, NPC bien mat thi bo. Chu: \"kich vao se tu bam theo nguoi choi\".",
                    "\t\t\textern int g_nJxMucTieuKhoa;",
                    "\t\t\tif (g_nJxMucTieuKhoa > 0 && g_nJxMucTieuKhoa < MAX_NPC && Npc[g_nJxMucTieuKhoa].m_dwID && Npc[g_nJxMucTieuKhoa].m_RegionIndex >= 0)",
                    "\t\t\t\tidx = g_nJxMucTieuKhoa;",
                    "\t\t\telse",
                    "\t\t\t\tg_nJxMucTieuKhoa = 0;",
                    "\t\t}",
                    "#endif"], "CoreShell.cpp: NPC_OI_TARGET_INFO khoa")


def va_kplayer(s):
    s = thay(s, ["\tg_nJxMucTieuKhoa = m_nPeapleIdx;\t// [ANDROID 11/09 KHOAMT] cham trung ai thi KHOA muc tieu do; cham dat = bo khoa"],
             ["\tif (m_nPeapleIdx)",
              "\t\tg_nJxMucTieuKhoa = m_nPeapleIdx;\t// [ANDROID 11/09 KHOAMT] %s cham trung ai thi KHOA; cham dat KHONG bo khoa" % DAU],
             "KPlayer.cpp: OnButtonDown")
    s = thay(s, ["#ifdef JX_ANDROID",
                 "\t// [ANDROID 11/09 KHOAMT] dien thoai khong co \"re chuot\": ngon tay keo di (cam can, keo cua so, hover gia lap) KHONG doi muc tieu",
                 "\t// dang xem; giu muc tieu da cham cho toi khi cham cho khac (OnButtonDown). Chu: \"kich vao hien thong tin",
                 "\t// neu lo di chuot di cho khac la no doi muc tieu\".",
                 "\tif (g_nJxMucTieuKhoa > 0)",
                 "\t{",
                 "\t\tif (g_nJxMucTieuKhoa >= MAX_NPC || Npc[g_nJxMucTieuKhoa].m_dwID == 0 || Npc[g_nJxMucTieuKhoa].m_RegionIndex < 0)",
                 "\t\t\tg_nJxMucTieuKhoa = 0;",
                 "\t\telse",
                 "\t\t\tm_nPeapleIdx = g_nJxMucTieuKhoa;",
                 "\t}",
                 "#endif"],
                ["\t// %s [KHOAMT b] khong ep m_nPeapleIdx o day nua (hover/Walk giu nhu PC); khoa ap tai NPC_OI_TARGET_INFO (CoreShell.cpp)" % DAU],
                "KPlayer.cpp: OnMouseMove bo ep")
    return s


def va_layout(s):
    nl = nl_cua(s)
    dong = s.split(nl)
    idx = [i for i, d in enumerate(dong) if d.startswith("KUiPlayerBar|Main=")]
    if len(idx) != 1:
        raise SystemExit("uitoado_macdinh.ini: KUiPlayerBar|Main")
    dong[idx[0] + 1:idx[0] + 1] = ["; " + DAU + " thanh thong tin muc tieu: goc phai duoi hang icon, 70% (chu keo/thu them trong Sua giao dien)",
                                   "KUiTargetInfo|Main=868,300,700,0"]
    return nl.join(dong)


va("Sources/Core/Src/GameDataDef.h", DAU, va_gamedatadef)
va("Sources/S3Client/Ui/UiCase/UiTargetInfo.cpp", DAU, va_uitargetinfo)
va("Sources/Core/Src/KNpc.cpp", DAU, va_knpc)
va("Sources/Core/Src/CoreShell.cpp", DAU, va_coreshell)
va("Sources/Core/Src/KPlayer.cpp", DAU, va_kplayer)
for p in ["android/du_lieu_ghi_de/ui/uitoado_macdinh.ini", "D:/jx1_android_data/ui/uitoado_macdinh.ini"]:
    if os.path.isfile(p):
        va(p, DAU, va_layout)
print("xong")
