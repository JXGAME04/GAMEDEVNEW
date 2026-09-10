# -*- coding: utf-8 -*-
#
# [DANGNHAP 12/09] Phan dang nhap tren dien thoai (chu 03:10):
#   1. "phai lam cho vua kich thuoc man hinh tuy loai may": nen dang nhap / menu (KUiLoginBackGround, anh 800x600 Init_Login.jpg)
#      ve HAI lop: ban keo toan man (toi) + ban that vua chieu cao can giua -> khong con dai den, khong meo, moi ti le man deu vua.
#      (Cac bang menu / dang nhap / chon may chu neo GIUA bang bo cuc UiToaDo - sinh_bocuc_rong.py.)
#   2. "nhap tai khoan xong xuong o mat khau phai hien ban phim lien": IME Android tu dong sau phim Enter -> KSdlApp mo lai ban phim
#      0,3 s sau khi o nhap moi nhan tieu diem (JxSdl_BanPhimNhip trong vong lap).
#   3. "chu cai dau tai khoan phai viet thuong, ban phim dien thoai tu viet hoa": SDL_StartTextInputWithProperties voi
#      SDL_CAPITALIZE_NONE + khong tu sua chu (o mat khau = kieu mat khau an); KUiLogin::GetInputInfo ha chu dau tai khoan.
# Tep: KSdlApp.cpp (chi Android), WndEdit.cpp / UiLogin.cpp / UiLoginBg.h / UiLoginBg.cpp (rao JX_ANDROID, ban PC khong doi).
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[DANGNHAP 12/09]"


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


def va(p, ham):
    s = doc(p)
    if DAU in s:
        print("da va roi:", p)
        return
    s2 = ham(s)
    if cao(s) != cao(s2):
        raise SystemExit("so byte cao doi o " + p)
    ghi(p, s2)
    print("da va:", p)


# ---------------------------------------------------------------- KSdlApp.cpp: ban phim ao co kieu + mo lai
def va_sdl(s):
    s = thay(s, ['extern "C" void JxSdl_BanPhimAo(int bBat)',
                 "{",
                 "\tSDL_Window* pWin = (SDL_Window*)JxPosix_MainWindow();",
                 "\tif (!pWin)",
                 "\t\treturn;",
                 "\tif (bBat)",
                 "\t\tSDL_StartTextInput(pWin);",
                 "\telse",
                 "\t\tSDL_StopTextInput(pWin);",
                 "}"],
                ["// %s ban phim theo KIEU o nhap: khong tu viet hoa chu dau (tai khoan phai chu thuong), khong tu sua chu," % DAU,
                 "// o mat khau = kieu mat khau an. IME Android hay tu dong sau phim Enter du o nhap ke tiep da nhan tieu diem ->",
                 "// hen mo lai sau 0,3 s (JxSdl_BanPhimNhip trong vong lap chinh).",
                 "static Uint64 s_uBanPhimLai = 0;",
                 "static int s_nBanPhimMatKhau = 0;",
                 "static void BanPhimMo(SDL_Window* pWin, int nMatKhau)",
                 "{",
                 "\tSDL_PropertiesID p = SDL_CreateProperties();",
                 "\tSDL_SetNumberProperty(p, SDL_PROP_TEXTINPUT_TYPE_NUMBER, nMatKhau ? SDL_TEXTINPUT_TYPE_TEXT_PASSWORD_HIDDEN : SDL_TEXTINPUT_TYPE_TEXT);",
                 "\tSDL_SetNumberProperty(p, SDL_PROP_TEXTINPUT_CAPITALIZATION_NUMBER, SDL_CAPITALIZE_NONE);",
                 "\tSDL_SetBooleanProperty(p, SDL_PROP_TEXTINPUT_AUTOCORRECT_BOOLEAN, false);",
                 "\tSDL_StartTextInputWithProperties(pWin, p);",
                 "\tSDL_DestroyProperties(p);",
                 "}",
                 'extern "C" void JxSdl_BanPhimAo(int bBat, int nMatKhau)',
                 "{",
                 "\tSDL_Window* pWin = (SDL_Window*)JxPosix_MainWindow();",
                 "\tif (!pWin)",
                 "\t\treturn;",
                 "\tif (bBat)",
                 "\t{",
                 "\t\tBanPhimMo(pWin, nMatKhau);",
                 "\t\ts_nBanPhimMatKhau = nMatKhau;",
                 "\t\ts_uBanPhimLai = SDL_GetTicks() + 300;",
                 "\t}",
                 "\telse",
                 "\t{",
                 "\t\tSDL_StopTextInput(pWin);",
                 "\t\ts_uBanPhimLai = 0;",
                 "\t}",
                 "}",
                 "// goi moi vong lap: den hen ma o nhap van giu tieu diem -> dong roi mo lai ban phim (IME da tu dong thi hien lai)",
                 'extern "C" void JxSdl_BanPhimNhip(void)',
                 "{",
                 "\tif (!s_uBanPhimLai || SDL_GetTicks() < s_uBanPhimLai)",
                 "\t\treturn;",
                 "\ts_uBanPhimLai = 0;",
                 "\tSDL_Window* pWin = (SDL_Window*)JxPosix_MainWindow();",
                 "\tif (!pWin)",
                 "\t\treturn;",
                 "\tSDL_StopTextInput(pWin);",
                 "\tBanPhimMo(pWin, s_nBanPhimMatKhau);",
                 "}"], "KSdlApp BanPhimAo")
    s = thay(s, ["\t\tNhipCham();\t\t// [ANDROID 09/09 CHAM] giu ngon du lau ma khong xe dich -> chuot phai"],
                ["\t\tNhipCham();\t\t// [ANDROID 09/09 CHAM] giu ngon du lau ma khong xe dich -> chuot phai",
                 "\t\tJxSdl_BanPhimNhip();\t// %s mo lai ban phim sau khi IME tu dong" % DAU], "KSdlApp vong lap")
    return s


# ---------------------------------------------------------------- WndEdit.cpp: bao kieu o nhap
def va_edit(s):
    s = thay(s, ['extern "C" void JxSdl_BanPhimAo(int bBat);\t// KSdlApp.cpp'],
                ['extern "C" void JxSdl_BanPhimAo(int bBat, int nMatKhau);\t// KSdlApp.cpp; %s them kieu o (mat khau)' % DAU], "WndEdit extern")
    s = thay(s, ["\t\tJxSdl_BanPhimAo(1);\t\t// [ANDROID 09/09 CHAM] o nhap co tieu diem -> day ban phim ao len"],
                ["\t\tJxSdl_BanPhimAo(1, (m_Flag & WNDEDIT_ES_MASK_CHARACTER) ? 1 : 0);\t\t// [ANDROID 09/09 CHAM] o nhap co tieu diem -> day ban phim ao len; %s kieu o" % DAU], "WndEdit bat")
    s = thay(s, ["\t\tJxSdl_BanPhimAo(0);\t\t// [ANDROID 09/09 CHAM] khong con o nhap nao -> cat ban phim ao di"],
                ["\t\tJxSdl_BanPhimAo(0, 0);\t\t// [ANDROID 09/09 CHAM] khong con o nhap nao -> cat ban phim ao di"], "WndEdit tat")
    return s


# ---------------------------------------------------------------- UiLogin.cpp: chu dau tai khoan viet thuong
def va_login(s):
    s = thay(s, ["\tif (m_Account.GetText(pszAccount, 32, false) &&",
                 "\t\tm_PassWord.GetText(pszPassword, 32, false))",
                 "\t{",
                 "\t\treturn true;"],
                ["\tif (m_Account.GetText(pszAccount, 32, false) &&",
                 "\t\tm_PassWord.GetText(pszPassword, 32, false))",
                 "\t{",
                 "#ifdef JX_ANDROID",
                 "\t\t// %s quy che game: chu dau tai khoan phai viet thuong; ban phim dien thoai hay tu viet hoa -> ha xuong" % DAU,
                 "\t\tif (pszAccount[0] >= 'A' && pszAccount[0] <= 'Z')",
                 "\t\t{",
                 "\t\t\tpszAccount[0] = (char)(pszAccount[0] - 'A' + 'a');",
                 "\t\t\tm_Account.SetText(pszAccount);",
                 "\t\t}",
                 "#endif",
                 "\t\treturn true;"], "UiLogin GetInputInfo")
    return s


# ---------------------------------------------------------------- UiLoginBg: nen vua man
def va_loginbg_h(s):
    s = thay(s, ["\tvoid\tBreathe();"],
                ["\tvoid\tBreathe();",
                 "#ifdef JX_ANDROID",
                 "\tvirtual void PaintWindow();\t// %s nen 800x600 ve vua moi ti le man: lop keo toan man (toi) + lop that vua chieu cao can giua" % DAU,
                 "#endif"], "UiLoginBg.h")
    return s


def va_loginbg_cpp(s):
    nl = nl_cua(s)
    khoi = [
        "",
        "#ifdef JX_ANDROID",
        "extern int SCREEN_WIDTH;",
        "extern int SCREEN_HEIGHT;",
        "// %s Chu: 'phan dang nhap phai lam cho vua kich thuoc man hinh tuy loai may'. Anh nen 800x600 (4:3) tren man 16:9..21:9:" % DAU,
        "// ve ban keo toan man lam nen toi (khong con dai den hai ben), roi ve ban that vua chieu cao, can giua (khong meo, khong cat).",
        "void KUiLoginBackGround::PaintWindow()",
        "{",
        "\tKRPosition2 oOff = {0, 0}, oCo = {0, 0};",
        "\tif (!g_pRepresentShell || !m_Image.szImage[0] || SCREEN_WIDTH <= 0 || SCREEN_HEIGHT <= 0",
        "\t\t|| !g_pRepresentShell->GetImageFrameParam(m_Image.szImage, m_Image.nFrame, &oOff, &oCo, m_Image.nType)",
        "\t\t|| oCo.nX <= 0 || oCo.nY <= 0)",
        "\t{",
        "\t\tKWndImage::PaintWindow();",
        "\t\treturn;",
        "\t}",
        "\tKWndWindow::PaintWindow();",
        "\tKUiImageRef a = m_Image;",
        "\tunsigned int uMauGoc = a.Color.Color_dw;",
        "\ta.oPosition.nX = 0; a.oPosition.nY = 0; a.oPosition.nZ = 0;",
        "\ta.oEndPos.nX = SCREEN_WIDTH; a.oEndPos.nY = SCREEN_HEIGHT; a.oEndPos.nZ = 0;",
        "\ta.Color.Color_dw = 0xff484848;\t// lop nen keo toan man, toi di",
        "\tg_pRepresentShell->DrawPrimitives(1, &a, RU_T_IMAGE_STRETCH, true);",
        "\tint nCao = SCREEN_HEIGHT;",
        "\tint nRong = oCo.nX * nCao / oCo.nY;",
        "\tif (nRong > SCREEN_WIDTH)",
        "\t{",
        "\t\tnRong = SCREEN_WIDTH;",
        "\t\tnCao = oCo.nY * nRong / oCo.nX;",
        "\t}",
        "\tint nX = (SCREEN_WIDTH - nRong) / 2, nY = (SCREEN_HEIGHT - nCao) / 2;",
        "\ta.Color.Color_dw = uMauGoc ? uMauGoc : 0xffffffff;",
        "\ta.oPosition.nX = nX; a.oPosition.nY = nY;",
        "\ta.oEndPos.nX = nX + nRong; a.oEndPos.nY = nY + nCao;",
        "\tg_pRepresentShell->DrawPrimitives(1, &a, RU_T_IMAGE_STRETCH, true);",
        "}",
        "#endif",
        "",
    ]
    # them vao cuoi tep
    if not s.endswith(nl):
        s += nl
    return s + nl.join(khoi)


va("Sources/S3Client/Platform/KSdlApp.cpp", va_sdl)
va("Sources/S3Client/Ui/Elem/WndEdit.cpp", va_edit)
va("Sources/S3Client/Ui/UiCase/UiLogin.cpp", va_login)
va("Sources/S3Client/Ui/UiCase/UiLoginBg.h", va_loginbg_h)
va("Sources/S3Client/Ui/UiCase/UiLoginBg.cpp", va_loginbg_cpp)
print("xong")
