# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 LUAN d] VONG SANG CHUA LUAN CHUYEN - sua goc.
# Benh: KyNang_OLaAura() goi KyNang_HoiCore(); ham nay TRA VE (nAura = 0) ngay ca khi Core CHUA TRA LOI DUOC
# (GetGameData(GDI_KYNANG_MOBILE) tra 0 vi Player.m_nIndex = 0 luc chua vao game). Ket qua 0 bi ghi vao bo dem
# s_uKNAuraId/s_nKNAuraLa theo MA KY NANG -> ca phien coi o vong sang la "khong phai vong sang": khong luan
# chuyen, khong vong xoay. JxKyNang_Nhip() chay tu vong lap KSdlApp::Run ngay tu MAN HINH DANG NHAP va
# KyNangMobile.ini da co san (gan tu phien truoc) -> moi lan mo app bo dem deu bi doc sai truoc khi vao game.
# Sua:
#   1. KyNang_OLaAura hoi thang Core, CHI ghi bo dem khi Core tra loi duoc (GetGameData tra 1).
#   2. KyNang_LuanChuyen: chi lam viec moi LuanChuyenMs (truoc: quet 8 o moi vong lap 1 ms), khong lam gi khi
#      chua vao game (chua co ky nang danh trai), cap nhat s_KNPhai ngay khi doi de vong xoay chuyen theo.
#   3. Cham o vong sang / go vong sang: cap nhat s_KNPhai ngay.
#   4. Nhat ky [KYNANG] luan chuyen: 20 dong dau roi moi 200 lan mot dong (khong day logcat).
# Chi sua tep Android (JxCanDieuKhien.cpp chi bien dich khi JX_ANDROID) -> ban PC khong doi.

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 11/09 LUAN d]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def thay(s, cu, moi, ten, so=1):
    # ket thuc dong theo TEP (khong theo "\r\n" truoc): tep LF ma cu chi co MOT dong thi "\r\n".join(cu) van khop
    # -> moi ghi ra "\r\n" -> tep lan lon CRLF/LF
    nl = "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"
    c = nl.join(cu)
    if s.count(c) == so:
        return s.replace(c, nl.join(moi))
    raise SystemExit("khong tim thay dung %d cho: %s" % (so, ten))


P = "Sources/S3Client/Platform/JxCanDieuKhien.cpp"
s = doc(P)
if DAU in s:
    print("da va roi, bo qua:", P)
    raise SystemExit

# 1. KyNang_OLaAura: chi ghi bo dem khi Core tra loi duoc
s = thay(s, [
    "\tif (s_uKNAuraId[i] != pRa->uId)",
    "\t{",
    "\t\tint nAura = 0;",
    "",
    "\t\tKyNang_HoiCore((int)pRa->uId, NULL, &nAura, NULL, NULL, NULL, 1);",
    "\t\ts_uKNAuraId[i] = pRa->uId;",
    "\t\ts_nKNAuraLa[i] = nAura;",
    "\t}",
    "\treturn s_nKNAuraLa[i];",
], [
    "\tif (s_uKNAuraId[i] != pRa->uId)",
    "\t{",
    "\t\t// %s Hoi THANG Core va CHI ghi bo dem khi Core TRA LOI DUOC (GetGameData tra 1)." % DAU,
    "\t\t// Truoc: KyNang_HoiCore tra ve (nAura = 0) ca khi Core chua tra loi duoc (chua vao game: Player.m_nIndex = 0",
    "\t\t// -> Core break, nRet 0) -> 0 bi ghi vao bo dem theo MA KY NANG cho ca phien -> o vong sang bi coi la",
    "\t\t// \"khong phai vong sang\" -> khong luan chuyen, khong vong xoay. JxKyNang_Nhip chay tu vong lap KSdlApp::Run",
    "\t\t// ngay tu man hinh dang nhap (KyNangMobile.ini da co) nen bo dem luon bi doc sai truoc khi vao game.",
    "\t\tKJxKyNangHoi oHoi;",
    "",
    "\t\tif (g_pCoreShell == NULL)",
    "\t\t\treturn 0;",
    "\t\tmemset(&oHoi, 0, sizeof(oHoi));",
    "\t\toHoi.nSkillId = (int)pRa->uId;",
    "\t\tif (!g_pCoreShell->GetGameData(GDI_KYNANG_MOBILE, (KUPARAM)&oHoi, 1))",
    "\t\t\treturn 0;\t\t// chua tra loi duoc: KHONG ghi bo dem, lan sau hoi lai",
    "\t\ts_uKNAuraId[i] = pRa->uId;",
    "\t\ts_nKNAuraLa[i] = oHoi.nLaAura;",
    "\t}",
    "\treturn s_nKNAuraLa[i];",
], "KyNang_OLaAura")

# 2. KyNang_LuanChuyen: moi LuanChuyenMs mot lan, chi khi da vao game, cap nhat s_KNPhai
s = thay(s, [
    "\tif (g_pCoreShell == NULL)",
    "\t\treturn;",
    "\tfor (i = 1; i <= KYNANG_SO_PHU; i++)",
    "\t{",
    "\t\tKUiGameObject o;",
    "",
    "\t\tif (KyNang_OLaAura(i, &o))",
    "\t\t\taAura[n++] = o;",
    "\t}",
    "\tif (n == 0)",
    "\t\treturn;",
    "\tuNay = (unsigned int)GetTickCount();",
    "\tif (s_uKNLuanLuc && uNay - s_uKNLuanLuc < (unsigned int)s_nKNLuanMs)",
    "\t\treturn;",
    "\ts_uKNLuanLuc = uNay;",
    "\toMuon = aAura[s_nKNLuanK % n];",
    "\ts_nKNLuanK++;",
    "\tmemset(&oTay, 0, sizeof(oTay));",
    "\tg_pCoreShell->GetGameData(GDI_PLAYER_IMMED_ITEMSKILL, (KNPARAM)&oTay, 0);",
    "\tif (n == 1 && oTay.IMmediaSkill[1].uId == oMuon.uId)",
    "\t\treturn;\t\t// mot vong sang va dang bat dung no",
    "\tg_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&oMuon, 1);\t// SetRightSkill -> SetAuraSkill",
    "\tg_DebugLog(\"[KYNANG] luan chuyen vong sang -> %u (%d/%d)\", oMuon.uId, (s_nKNLuanK - 1) % n + 1, n);",
    "}",
], [
    "\tif (g_pCoreShell == NULL)",
    "\t\treturn;",
    "\t// %s moi s_nKNLuanMs chi lam MOT lan (truoc: quet 8 o + hoi Core moi vong lap ~1 ms)" % DAU,
    "\tuNay = (unsigned int)GetTickCount();",
    "\tif (s_uKNLuanLuc && uNay - s_uKNLuanLuc < (unsigned int)s_nKNLuanMs)",
    "\t\treturn;",
    "\ts_uKNLuanLuc = uNay;",
    "\tmemset(&oTay, 0, sizeof(oTay));",
    "\tg_pCoreShell->GetGameData(GDI_PLAYER_IMMED_ITEMSKILL, (KNPARAM)&oTay, 0);",
    "\tif (oTay.IMmediaSkill[0].uId == 0)",
    "\t\treturn;\t\t// %s chua vao game (chua co ky nang danh trai): khong hoi, khong doi gi" % DAU,
    "\ts_KNPhai = oTay.IMmediaSkill[1];\t// %s vong xoay bam dung o dang bat, khong doi KyNang_DocBang (2 s)" % DAU,
    "\tfor (i = 1; i <= KYNANG_SO_PHU; i++)",
    "\t{",
    "\t\tKUiGameObject o;",
    "",
    "\t\tif (KyNang_OLaAura(i, &o))",
    "\t\t\taAura[n++] = o;",
    "\t}",
    "\tif (n == 0)",
    "\t\treturn;",
    "\toMuon = aAura[s_nKNLuanK % n];",
    "\ts_nKNLuanK++;",
    "\tif (n == 1 && oTay.IMmediaSkill[1].uId == oMuon.uId)",
    "\t\treturn;\t\t// mot vong sang va dang bat dung no",
    "\tg_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&oMuon, 1);\t// SetRightSkill -> SetAuraSkill",
    "\ts_KNPhai = oMuon;\t// %s vong xoay chuyen NGAY sang o vua bat" % DAU,
    "\tif (s_nKNLuanLog < 20 || (s_nKNLuanLog %% 200) == 0)\t// %s 20 dong dau, sau do moi 200 lan mot dong" % DAU,
    "\t\tg_DebugLog(\"[KYNANG] luan chuyen vong sang -> %u (%d/%d) lan %d\", oMuon.uId, (s_nKNLuanK - 1) % n + 1, n, s_nKNLuanLog);",
    "\ts_nKNLuanLog++;",
    "}",
], "KyNang_LuanChuyen")

# bien dem nhat ky
s = thay(s, [
    "static int\t\t\ts_nKNLuanK = 0;",
], [
    "static int\t\t\ts_nKNLuanK = 0;",
    "static int\t\t\ts_nKNLuanLog = 0;\t// %s so lan da luan chuyen (de han che nhat ky)" % DAU,
], "khai bao s_nKNLuanK")

# 3a. cham o vong sang -> s_KNPhai theo ngay
s = thay(s, [
    "\t\t// (Ban truoc dat nham vao o danh TRAI nen khong bat gi ca.)",
    "\t\tg_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&o, 1);",
    "\t\treturn;",
], [
    "\t\t// (Ban truoc dat nham vao o danh TRAI nen khong bat gi ca.)",
    "\t\tg_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&o, 1);",
    "\t\ts_KNPhai = o;\t// %s vong xoay chuyen ngay sang o vua cham" % DAU,
    "\t\treturn;",
], "cham o vong sang")

# 3b. go vong sang -> ky nang phai = ky nang chinh
s = thay(s, [
    "\t\tif (nAura && s_KNChinh.uId && g_pCoreShell)",
    "\t\t\tg_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&s_KNChinh, 1);",
], [
    "\t\tif (nAura && s_KNChinh.uId && g_pCoreShell)",
    "\t\t{",
    "\t\t\tg_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&s_KNChinh, 1);",
    "\t\t\ts_KNPhai = s_KNChinh;\t// %s" % DAU,
    "\t\t}",
], "go vong sang")

ghi(P, s)
print("da va:", P)
