# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 WAUTO B2 i] Ba nut Auto tren thanh cong cu lay NGUYEN nut kho VNKU (chu: "goc no co icon luon ko can ghep") + nut NHAT
# (chu: "them icon nhat do (hinh ban tay) bam vao tu chay toi nhat, uu tien hon tu danh").
#
#  1. VA  Sources/S3Client/Platform/JxWAutoNoiBo.h/.cpp: JxWAuto_NhatNgay(nMs) / JxWAuto_DangNhat(): "dot nhat" nMs mili giay - gui nhip
#         PRT_GAMELOOP ke ca khi auto dang tat, voi cau hinh tam: bFight=0, bOnPK=0 (khong danh), bPickUp=1, bFollowPick=1 (chay toi
#         nhat), nPickVision >= 800; go khoa muc tieu luc bat dau; het dot ma auto dang tat thi gui PRT_TICKSTART(0) IM LANG de bo nao
#         ATYPE_CLEAR (dung nhan vat). Auto dang bat thi het dot tro ve cau hinh that.
#  2. VA  Sources/S3Client/Ui/UiShell.h/.cpp: Player_WAutoBat (bam = bat/tat auto ngay, CheckBox theo JxWAuto_DangBat), Player_NhatDo
#         (bam = JxWAuto_NhatNgay(6000), CheckBox khi dang nhat); Player_WAuto (cu) thanh nut "Thiet lap": mo/dong khung, CheckBox khi khung mo.
#  3. VA  android/du_lieu_ghi_de/ui/ui3/uitoolscontrolbar.ini: [WAuto] -> anh thiet_lap_m.spr 102x40 (8,344); them Button7=WAutoBat
#         [WAutoBat] bat_auto_m.spr 102x40 (8,300) Up=0 Down=2 CheckBox=1; Button8=NhatDo [NhatDo] nhat_m.spr 48x48 (8,390).
#         Anh: android/anh_wauto_vnku.py nut_thanh_cong_cu(). Chi Android. Chay lai vo hai.

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 11/09 WAUTO B2 i]"


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


# ---------------------------------------------------------------- 1. JxWAutoNoiBo.h / .cpp
def va_noibo_h(s):
    return thay(s, ["unsigned int\tJxWAuto_IdNhanVat();\t\t// ma nhan vat cua cau hinh dang nap (0 = chua vao game)"],
                   ["unsigned int\tJxWAuto_IdNhanVat();\t\t// ma nhan vat cua cau hinh dang nap (0 = chua vao game)",
                    "// %s nut ban tay: nhat ngay trong nMs mili giay (khong danh, chay toi nhat), ke ca khi auto dang tat" % DAU,
                    "void\t\t\tJxWAuto_NhatNgay(int nMs);",
                    "int\t\t\t\tJxWAuto_DangNhat();"], "JxWAutoNoiBo.h: NhatNgay")


def va_noibo_cpp(s):
    s = thay(s, ["static int\t\t\ts_nTickDaBao = -1;\t// trang thai da bao cho bo nao (PRT_TICKSTART): -1 chua, 0 tat, 1 bat"],
                ["static int\t\t\ts_nTickDaBao = -1;\t// trang thai da bao cho bo nao (PRT_TICKSTART): -1 chua, 0 tat, 1 bat",
                 "static UINT\t\t\ts_uNhatDen = 0;\t\t// %s dot nhat ngay: gui nhip toi luc nay (0 = khong)" % DAU,
                 "static int\t\t\ts_nNhatMuonAuto = 0;\t// auto dang tat luc bat dau dot -> het dot phai bao bo nao dung (ATYPE_CLEAR)"],
                "JxWAutoNoiBo.cpp: bien dot nhat")
    # WA_BaoTick: them tham so im lang
    s = thay(s, ["static void WA_BaoTick(int bBat)",
                 "{",
                 "\tIPCHideGame s;",
                 "\ts.CmdID = PRT_TICKSTART;",
                 "\ts.Size = sizeof(IPCHideGame);",
                 "\ts.bHide = bBat;",
                 "\tWA_GuiGoi(&s, sizeof(s), 1);"],
                ["static void WA_BaoTick(int bBat, int bIm = 0)\t// %s bIm = 1: chi bao bo nao (het dot nhat), khong ghi chat" % DAU,
                 "{",
                 "\tIPCHideGame s;",
                 "\ts.CmdID = PRT_TICKSTART;",
                 "\ts.Size = sizeof(IPCHideGame);",
                 "\ts.bHide = bBat;",
                 "\tWA_GuiGoi(&s, sizeof(s), 1);",
                 "\tif (bIm)",
                 "\t{",
                 "\t\tg_DebugLog(\"[WAUTO] het dot NHAT NGAY (PRT_TICKSTART -> ATYPE_CLEAR, im lang)\");",
                 "\t\treturn;",
                 "\t}"], "JxWAutoNoiBo.cpp: WA_BaoTick im")
    # NhipVongLap: dot nhat
    s = thay(s, ["\tif (!s_nBat)",
                 "\t\treturn;",
                 "\tUINT uNow = timeGetTime();",
                 "\tif (uNow < s_uNhipKe)",
                 "\t\treturn;"],
                ["\tUINT uNow = timeGetTime();",
                 "\tint bNhat = (s_uNhatDen != 0);\t\t// %s dot nhat ngay (nut ban tay)" % DAU,
                 "\tif (bNhat && (int)(uNow - s_uNhatDen) >= 0)",
                 "\t{",
                 "\t\ts_uNhatDen = 0;",
                 "\t\tbNhat = 0;",
                 "\t\tif (s_nNhatMuonAuto)\t\t\t// auto von tat: bao bo nao xoa trang thai, dung nhan vat",
                 "\t\t\tWA_BaoTick(0, 1);",
                 "\t\ts_nNhatMuonAuto = 0;",
                 "\t}",
                 "\tif (!s_nBat && !bNhat)",
                 "\t\treturn;",
                 "\tif (uNow < s_uNhipKe)",
                 "\t\treturn;"], "JxWAutoNoiBo.cpp: gate dot nhat")
    s = thay(s, ["\tpGL->setting.bAcChinhVaoMap = 0;",
                 "\tpGL->setting.uACTuoi = 0;"],
                ["\tpGL->setting.bAcChinhVaoMap = 0;",
                 "\tpGL->setting.uACTuoi = 0;",
                 "\tif (bNhat)",
                 "\t{",
                 "\t\t// %s dot nhat ngay: KHONG danh (bFight / bOnPK = 0), NHAT + chay toi (bFollowPick), tam nhat >= 800" % DAU,
                 "\t\tpGL->setting.bFight = 0;",
                 "\t\tpGL->setting.bOnPK = 0;",
                 "\t\tpGL->setting.bPickUp = 1;",
                 "\t\tpGL->setting.bFollowPick = 1;",
                 "\t\tif (pGL->setting.nPickVision < 800)",
                 "\t\t\tpGL->setting.nPickVision = 800;",
                 "\t\tpGL->setting.bTongKim = 0;\t// cac may hoat dong cung nghi trong dot",
                 "\t\tpGL->setting.bDaTau = 0;",
                 "\t}"], "JxWAutoNoiBo.cpp: cau hinh dot nhat")
    s = thay(s, ["int JxWAuto_DangBat()\t\t\t\t{ return s_nBat; }"],
                ["int JxWAuto_DangBat()\t\t\t\t{ return s_nBat; }",
                 "int JxWAuto_DangNhat()\t\t\t\t{ return s_uNhatDen != 0; }\t// %s" % DAU,
                 "void JxWAuto_NhatNgay(int nMs)",
                 "{",
                 "\tif (nMs <= 0)",
                 "\t{",
                 "\t\ts_uNhatDen = 0;",
                 "\t\treturn;",
                 "\t}",
                 "\tif (!s_uNhatDen)",
                 "\t{",
                 "\t\ts_nNhatMuonAuto = !s_nBat;",
                 "\t\tif (g_pCoreShell)\t\t\t// dang khoa muc tieu (danh ai) thi go de nhan vat chiu roi cho di nhat",
                 "\t\t{",
                 "\t\t\tg_pCoreShell->LockSomeoneAction(0);",
                 "\t\t\tg_pCoreShell->LockObjectAction(0);",
                 "\t\t}",
                 "\t}",
                 "\ts_uNhatDen = timeGetTime() + (UINT)nMs;",
                 "\tif (!s_uNhatDen)",
                 "\t\ts_uNhatDen = 1;",
                 "\ts_uNhipKe = 0;\t\t\t\t\t// gui nhip dau ngay khung nay",
                 "\tg_DebugLog(\"[WAUTO] NHAT NGAY %d ms (auto dang %s)\", nMs, s_nBat ? \"bat\" : \"tat\");",
                 "}"], "JxWAutoNoiBo.cpp: JxWAuto_NhatNgay")
    return s


# ---------------------------------------------------------------- 2. UiShell.h / .cpp
def va_uishell_h(s):
    return thay(s, ["class Player_WAuto : public KWndButton",
                    "{",
                    "public:",
                    "\tDECLARE_COMCLASS(Player_WAuto)",
                    "\tvoid OnButtonClick();",
                    "\tvoid UpdateData();",
                    "\tconst char*\tGetShortKey();",
                    "};"],
                   ["class Player_WAuto : public KWndButton\t\t// %s nay la nut \"Thiet lap\" (thietlapauto.spr VNKU): mo / dong khung" % DAU,
                    "{",
                    "public:",
                    "\tDECLARE_COMCLASS(Player_WAuto)",
                    "\tvoid OnButtonClick();",
                    "\tvoid UpdateData();",
                    "\tconst char*\tGetShortKey();",
                    "};",
                    "// %s nut \"Bat Auto / Tat Auto\" (bat_auto.spr + tat_auto.spr VNKU, [WAutoBat]): bam = bat / tat may auto ngay" % DAU,
                    "class Player_WAutoBat : public KWndButton",
                    "{",
                    "public:",
                    "\tDECLARE_COMCLASS(Player_WAutoBat)",
                    "\tvoid OnButtonClick();",
                    "\tvoid UpdateData();",
                    "\tconst char*\tGetShortKey();",
                    "};",
                    "// %s nut ban tay (nut_nhat.spr VNKU, [NhatDo]): nhat ngay 6 giay, uu tien hon danh (JxWAuto_NhatNgay)" % DAU,
                    "class Player_NhatDo : public KWndButton",
                    "{",
                    "public:",
                    "\tDECLARE_COMCLASS(Player_NhatDo)",
                    "\tvoid OnButtonClick();",
                    "\tvoid UpdateData();",
                    "\tconst char*\tGetShortKey();",
                    "};"], "UiShell.h: 2 lop nut")


def va_uishell_cpp(s):
    s = thay(s, ["void Player_WAuto::UpdateData()",
                 "{",
                 "\tCheckButton(JxWAuto_DangBat() ? 1 : 0);",
                 "}"],
                ["void Player_WAuto::UpdateData()",
                 "{",
                 "\tCheckButton(KUiWAuto::GetIfVisible() ? 1 : 0);\t// %s nut \"Thiet lap\": sang xam khi khung dang mo" % DAU,
                 "}",
                 "",
                 "// %s" % DAU,
                 "IMPLEMENT_COMCLASS(Player_WAutoBat)",
                 "void Player_WAutoBat::OnButtonClick()",
                 "{",
                 "\tJxWAuto_Bat(!JxWAuto_DangBat());",
                 "\tCheckButton(JxWAuto_DangBat() ? 1 : 0);",
                 "}",
                 "",
                 "void Player_WAutoBat::UpdateData()",
                 "{",
                 "\tCheckButton(JxWAuto_DangBat() ? 1 : 0);\t\t// Up = \"Bat Auto\" (dang tat), Down = \"Tat Auto\" (dang bat)",
                 "}",
                 "",
                 "const char* Player_WAutoBat::GetShortKey()",
                 "{",
                 "\treturn NULL;",
                 "}",
                 "",
                 "IMPLEMENT_COMCLASS(Player_NhatDo)",
                 "void Player_NhatDo::OnButtonClick()",
                 "{",
                 "\tJxWAuto_NhatNgay(6000);",
                 "\tCheckButton(1);",
                 "}",
                 "",
                 "void Player_NhatDo::UpdateData()",
                 "{",
                 "\tCheckButton(JxWAuto_DangNhat() ? 1 : 0);",
                 "}",
                 "",
                 "const char* Player_NhatDo::GetShortKey()",
                 "{",
                 "\treturn NULL;",
                 "}"], "UiShell.cpp: 2 lop nut")
    s = thay(s, ["\tPlayer_WAuto::RegisterSelfClass();\t// [ANDROID 11/09 WAUTO B1] icon Auto (khung WAuto trong game)"],
                ["\tPlayer_WAuto::RegisterSelfClass();\t// [ANDROID 11/09 WAUTO B1] icon Auto (khung WAuto trong game)",
                 "\tPlayer_WAutoBat::RegisterSelfClass();\t// %s nut Bat/Tat Auto + nut nhat" % DAU,
                 "\tPlayer_NhatDo::RegisterSelfClass();"], "UiShell.cpp: dang ky")
    return s


# ---------------------------------------------------------------- 3. uitoolscontrolbar.ini
# [B2 i c] Chu gui anh: icon Auto = vong tron kiem cheo + nhan "Auto" dat trong COT ICON BEN PHAI duoi "Trao doi / Xuong / Chay"
# (kho VNKU khong co san tep nay - ghep tu chinh nut "Bat Auto" cua kho: auto_m.spr 48x56; xam = tat, vang = bat). Nut ban tay (nhat)
# ngay duoi. Hai nut lon "Bat Auto" / "Thiet lap" (B2 i) BO khoi ini (lop Player_WAutoBat van dich, khong dat).
DAU_INI = "[ANDROID 11/09 WAUTO B2 i c]"


def va_ini(s):
    nl = nl_cua(s)
    s = s.replace("Button7=WAutoBat" + nl, "").replace("Button8=NhatDo" + nl, "")
    if "Button7=NhatDo" not in s:
        s = thay(s, ["Button6=WAuto"], ["Button6=WAuto", "Button7=NhatDo"], "ini: Button7")
    # bo cac khoi cu [WAuto] [WAutoBat] [NhatDo] (moi khoi toi dau khoi ke tiep hoac het tep)
    for ten in ("[WAuto]", "[WAutoBat]", "[NhatDo]"):
        while ten in s:
            a = s.index(ten)
            b = s.find(nl + "[", a + 1)
            s = s[:a] + (s[b + len(nl):] if b > 0 else "")
    khoi = [
        "[WAuto]",
        "; %s icon Auto trong cot icon ben phai (duoi Chay): vong tron kiem cheo + nhan \"Auto\" (auto_m.spr 48x56, ghep tu bat_auto.spr" % DAU_INI,
        "; VNKU); cham = mo / dong khung thiet lap; xam = auto tat, vang = auto bat",
        "Left=868", "Top=300", "Width=48", "Height=56", "Trans=0",
        "Image=\\Spr\\UiNew\\UiToolsControlBar\\auto_m.spr", "Up=0", "Down=1", "CheckBox=1", "ClassType=Player_WAuto",
        "",
        "[NhatDo]",
        "; %s nut ban tay (nut_nhat.spr VNKU): nhat ngay 6 giay, uu tien hon danh; vang khi dang nhat" % DAU_INI,
        "Left=868", "Top=362", "Width=48", "Height=48", "Trans=0",
        "Image=\\Spr\\UiNew\\UiToolsControlBar\\nhat_m.spr", "Up=0", "Down=1", "CheckBox=1", "ClassType=Player_NhatDo",
        "",
    ]
    if not s.endswith(nl):
        s += nl
    return s + nl.join(khoi) + nl


DAU3 = "[ANDROID 11/09 WAUTO B2 i c]"


def va_uishell_cpp_c(s):
    # icon Auto: vang khi may auto dang bat (nhu B1), khong theo khung mo / dong
    return thay(s, ["\tCheckButton(KUiWAuto::GetIfVisible() ? 1 : 0);\t// %s nut \"Thiet lap\": sang xam khi khung dang mo" % DAU],
                   ["\tCheckButton(JxWAuto_DangBat() ? 1 : 0);\t\t// %s icon Auto: vang khi may auto dang bat" % DAU3], "UiShell.cpp: icon theo DangBat")


DAU2 = "[ANDROID 11/09 WAUTO B2 i b]"


def va_noibo_cpp_b(s):
    # LockSomeoneAction / LockObjectAction can dinh nghia day du cua iCoreShell (truoc chi khai bao truoc)
    return thay(s, ["class iCoreShell;",
                    "extern iCoreShell*\t\tg_pCoreShell;"],
                   ["#include \"../../Core/src/coreshell.h\"\t// %s iCoreShell day du: LockSomeoneAction / LockObjectAction" % DAU2,
                    "extern iCoreShell*\t\tg_pCoreShell;"], "JxWAutoNoiBo.cpp: include coreshell.h")


DAU4 = "[ANDROID 11/09 WAUTO B2 i d]"


def va_noibo_cpp_d(s):
    # Chu (00:10): "kich vao chua tu chay toi nhat". ATYPE_PICKUP tra 0 khi nhan vat KHONG o the chien dau va bCityPick = 0
    # (CoreShell.cpp: if(!Npc.m_FightMode && !pApData->bCityPick) return 0) -> dung trong thanh / chua danh thi khong nhat.
    # Dot nhat ngay: ep bCityPick = 1 (nhat ca khi khong o the chien dau).
    return thay(s, ["\t\tif (pGL->setting.nPickVision < 800)",
                    "\t\t\tpGL->setting.nPickVision = 800;"],
                   ["\t\tif (pGL->setting.nPickVision < 800)",
                    "\t\t\tpGL->setting.nPickVision = 800;",
                    "\t\tpGL->setting.bCityPick = 1;\t// %s ATYPE_PICKUP bo qua khi khong o the chien dau tru khi bCityPick" % DAU4], "JxWAutoNoiBo.cpp: bCityPick trong dot nhat")


va("Sources/S3Client/Platform/JxWAutoNoiBo.h", DAU, va_noibo_h)
va("Sources/S3Client/Platform/JxWAutoNoiBo.cpp", DAU, va_noibo_cpp)
va("Sources/S3Client/Platform/JxWAutoNoiBo.cpp", DAU2, va_noibo_cpp_b)
va("Sources/S3Client/Platform/JxWAutoNoiBo.cpp", DAU4, va_noibo_cpp_d)
va("Sources/S3Client/Ui/UiShell.h", DAU, va_uishell_h)
va("Sources/S3Client/Ui/UiShell.cpp", DAU, va_uishell_cpp)
va("Sources/S3Client/Ui/UiShell.cpp", DAU3, va_uishell_cpp_c)
va("android/du_lieu_ghi_de/ui/ui3/uitoolscontrolbar.ini", DAU_INI, va_ini)
print("xong")
