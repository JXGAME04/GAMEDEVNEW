# -*- coding: utf-8 -*-
#
# [VATPHAM 12/09 f] Chu 16:15: "kich vao item nem do ra ngoai thi no cam tren tay khong nem ra ngoai".
#
# Nut "Nem" dang lam hai viec LIEN TIEP trong cung mot nhip:
#     GOI_SWITCH_OBJECT (nhac mon len tay)  ->  ThrowAwayItem()
# nhung GOI_SWITCH_OBJECT ket thuc bang SendClientCmdMoveItem - mot GOI GUI CHO MAY CHU, tay chi co mon SAU khi
# may chu tra loi. Nen ThrowAwayItem() chay ngay dong sau thay m_ItemList.Hand() == 0 va tra 0 (khong nem gi),
# roi mon ve tay khi may chu tra loi -> dung nhu chu ta: "no cam tren tay".
#
# Sua: NEM 2 PHA nhu ban tham khao USVOLAM (KuiAutoPlay::ThrowItem + JX1M_ThrowPhase2):
#   pha 1 (luc bam nut): nho ma dw cua mon roi gui lenh nhac len tay.
#   pha 2 (moi nhip ve): tay co mon VA dung ma dw do -> ThrowAwayItem(); tay giu mon khac -> huy (chong nem nham);
#                        qua 3 giay khong thay -> huy cho.
# Chi Android.
import io
import os

os.chdir(r"D:\GAMEDEVNEW\.claude\worktrees\full-wauto-mobile-analysis-f781df")
DAU = "[VATPHAM 12/09 f]"


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


# ---------------- 1) Core: hoi mon dang cam tren tay ----------------
C = "Sources/Core/Src/CoreShell.cpp"
s = doc(C)
if DAU not in s:
    s0 = s
    s = thay(s, ["\tif (pnManaMax)",
                 "\t\t*pnManaMax = Npc[nIdx].m_CurrentManaMax;",
                 "\treturn 1;",
                 "}"],
                ["\tif (pnManaMax)",
                 "\t\t*pnManaMax = Npc[nIdx].m_CurrentManaMax;",
                 "\treturn 1;",
                 "}",
                 "",
                 "// %s Mon dang cam TREN TAY: tra 1 va ghi ma dw (Item::GetID) neu co, 0 neu tay trong." % DAU,
                 "// Dai nut vat pham (S3Client/Ui/UiCase/UiVatPham.cpp) dung de biet may chu da nhac mon len tay chua",
                 "// truoc khi goi ThrowAwayItem - xem chu thich nem 2 pha o do. Chi Android.",
                 "extern \"C\" int JxCore_MonTrenTay(unsigned int* puDwId)",
                 "{",
                 "\tint nIdx = Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand();",
                 "\tif (nIdx <= 0)",
                 "\t\treturn 0;",
                 "\tif (puDwId)",
                 "\t\t*puDwId = (unsigned int)Item[nIdx].GetID();",
                 "\treturn 1;",
                 "}"], "JxCore_MonTrenTay")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (CoreShell.cpp)")
    ghi(C, s)
    print("da va:", C)

# ---------------- 2) UiVatPham: nem 2 pha ----------------
U = "Sources/S3Client/Ui/UiCase/UiVatPham.cpp"
s = doc(U)
if DAU not in s:
    s0 = s
    #   (a) trang thai cho nem + ham nhip
    s = thay(s, ["void KUiVatPham::LamNut(int nMa)",
                 "{"],
                ["//---------------------------------------------------------------------------",
                 "//\t%s NEM 2 PHA. GOI_SWITCH_OBJECT chi GUI lenh cho may chu (KPlayer::MoveItem ->" % DAU,
                 "//\tSendClientCmdMoveItem) nen ngay dong sau tay VAN TRONG: ThrowAwayItem() tra 0 va mon chi ve tay khi",
                 "//\tmay chu tra loi -> chu bao \"kich nem thi no cam tren tay, khong nem ra ngoai\" (16:15).",
                 "//\tNay: pha 1 nho ma dw roi gui lenh nhac len tay; pha 2 (JxVatPham_Nhip, moi nhip ve) doi tay co DUNG",
                 "//\tmon do roi moi nem. Tay giu mon khac -> huy (chong nem nham mon vua nhat duoc). Qua han -> huy cho.",
                 "//---------------------------------------------------------------------------",
                 "extern \"C\" int JxCore_MonTrenTay(unsigned int* puDwId);\t// CoreShell.cpp",
                 "",
                 "static unsigned int\ts_uNemCho  = 0;\t// ma dw mon dang cho nem (0 = khong cho)",
                 "static int\t\t\ts_nNemNhip = 0;\t// so nhip da doi",
                 "",
                 "extern \"C\" void JxVatPham_Nhip(void)",
                 "{",
                 "\tunsigned int uTay = 0;",
                 "",
                 "\tif (s_uNemCho == 0)",
                 "\t\treturn;",
                 "\tif (++s_nNemNhip > 180)\t\t// ~3 giay: may chu khong tra loi (mon khoa / khong nhac duoc) -> thoi",
                 "\t{",
                 "\t\ts_uNemCho = 0;",
                 "\t\treturn;",
                 "\t}",
                 "\tif (!JxCore_MonTrenTay(&uTay))",
                 "\t\treturn;\t\t\t\t\t// may chu chua tra loi, doi nhip sau",
                 "\tif (uTay == s_uNemCho && g_pCoreShell)",
                 "\t\tg_pCoreShell->ThrowAwayItem();\t// tay da co DUNG mon -> nem",
                 "\ts_uNemCho = 0;\t\t\t\t\t// tay giu mon khac thi huy luon (chong nem nham)",
                 "}",
                 "",
                 "void KUiVatPham::LamNut(int nMa)",
                 "{"], "ham nhip")
    #   (b) nut Nem: chi lam pha 1
    s = thay(s, ["\tcase VP_NEM:",
                 "\t\t//\t[VATPHAM 12/09 d] chu: \"nem 1 vien thi ra 1 vien thoi chu khong nem toan bo\".",
                 "\t\t//\tGDI_THROW_ALL_ITEM nem CA LOAI -> thay bang: nhac DUNG mon nay len tay",
                 "\t\t//\t(GOI_SWITCH_OBJECT chi co Pick) roi ThrowAwayItem() nem mon dang cam.",
                 "\t\t//\tLoi tu chan do khoa / hoang kim / nhiem vu: nem hong thi tra mon ve dung o cu.",
                 "\t\tg_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT, (KUPARAM)(&Obj), 0);",
                 "\t\tif (g_pCoreShell->ThrowAwayItem() == 0)",
                 "\t\t\tg_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT, 0, (KNPARAM)(&Obj));",
                 "\t\tbreak;"],
                ["\tcase VP_NEM:",
                 "\t\t//\t[VATPHAM 12/09 d] chu: \"nem 1 vien thi ra 1 vien thoi chu khong nem toan bo\".",
                 "\t\t//\tGDI_THROW_ALL_ITEM nem CA LOAI -> thay bang: nhac DUNG mon nay len tay roi nem mon dang cam.",
                 "\t\t//\t%s PHA 1: nho ma dw roi gui lenh nhac len tay; JxVatPham_Nhip nem o pha 2 khi tay da co mon." % DAU,
                 "\t\t{",
                 "\t\t\tChatItem CItem;",
                 "",
                 "\t\t\tmemset(&CItem, 0, sizeof(CItem));",
                 "\t\t\ts_uNemCho  = 0;",
                 "\t\t\ts_nNemNhip = 0;",
                 "\t\t\tif (g_pCoreShell->GetGameData(GDI_GET_ITEM_PARAM, (KUPARAM)&CItem, m_Obj.uId))",
                 "\t\t\t\ts_uNemCho = (unsigned int)CItem.m_nID;",
                 "\t\t\tif (s_uNemCho)",
                 "\t\t\t\tg_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT, (KUPARAM)(&Obj), 0);",
                 "\t\t}",
                 "\t\tbreak;"], "nut nem pha 1")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (UiVatPham.cpp)")
    ghi(U, s)
    print("da va:", U)

# ---------------- 3) KSdlApp: goi pha 2 moi nhip ----------------
K = "Sources/S3Client/Platform/KSdlApp.cpp"
s = doc(K)
if DAU not in s:
    s0 = s
    s = thay(s, ["extern \"C\" void JxSdl_BanPhimNhip(void);\t// [DANGNHAP 12/09] dinh nghia phia duoi, vong lap chinh goi"],
                ["extern \"C\" void JxSdl_BanPhimNhip(void);\t// [DANGNHAP 12/09] dinh nghia phia duoi, vong lap chinh goi",
                 "extern \"C\" void JxVatPham_Nhip(void);\t// %s UiVatPham.cpp - pha 2 cua nem 2 pha" % DAU],
                "khai bao nhip")
    s = thay(s, ["\t\tJxKyNang_Nhip();\t// [ANDROID 09/09 KYNANG I] dang de nut ky nang thi cu danh tiep"],
                ["\t\tJxKyNang_Nhip();\t// [ANDROID 09/09 KYNANG I] dang de nut ky nang thi cu danh tiep",
                 "\t\tJxVatPham_Nhip();\t// %s nut Nem: doi may chu nhac mon len tay roi moi nem" % DAU],
                "goi nhip")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (KSdlApp.cpp)")
    ghi(K, s)
    print("da va:", K)
print("xong")
