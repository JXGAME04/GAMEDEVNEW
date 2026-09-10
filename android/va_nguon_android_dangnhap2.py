# -*- coding: utf-8 -*-
#
# [DANGNHAP 12/09 b] Chu (tai-j/tai-k): "bam vao o tai khoan ban phim hien roi tat di, khong nhap duoc". Goc: JxSdl_BanPhimNhip
# cua DANGNHAP 12/09 hen 0,3 s roi TAT + MO lai ban phim (chong IME tu dong); tren Android tat roi mo lien nhau lam ban phim mat luon
# (cung goc voi loi cu "sang o mat khau khong hien ban phim": KILL_FOCUS tat, SET_FOCUS mo ngay sau -> mat).
# Sua: KILL_FOCUS chi HEN tat sau 0,2 s; SET_FOCUS den truoc thi huy hen va mo (ban phim giu nguyen, doi kieu o); khong con tat+mo lai.
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[DANGNHAP 12/09 b]"
TEP = "Sources/S3Client/Platform/KSdlApp.cpp"


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


s = doc(TEP)
if DAU in s:
    raise SystemExit("da va roi")
if "[DANGNHAP 12/09]" not in s:
    raise SystemExit("can va DANGNHAP 12/09 truoc")
s = thay(s, ["static Uint64 s_uBanPhimLai = 0;",
             "static int s_nBanPhimMatKhau = 0;"],
            ["static Uint64 s_uBanPhimTat = 0;\t// %s hen TAT ban phim (KILL_FOCUS); SET_FOCUS den truoc thi huy" % DAU,
             "static int s_nBanPhimMatKhau = 0;"], "bien")
s = thay(s, ["\tif (bBat)",
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
             "}"],
            ["\tif (bBat)",
             "\t{",
             "\t\ts_uBanPhimTat = 0;\t\t// %s o moi nhan tieu diem: huy hen tat, ban phim giu nguyen (doi kieu o neu can)" % DAU,
             "\t\tif (!SDL_TextInputActive(pWin) || nMatKhau != s_nBanPhimMatKhau)",
             "\t\t\tBanPhimMo(pWin, nMatKhau);",
             "\t\ts_nBanPhimMatKhau = nMatKhau;",
             "\t}",
             "\telse",
             "\t\ts_uBanPhimTat = SDL_GetTicks() + 200;\t// chi hen tat: Enter sang o ke tiep thi khong tat-mo lien nhau",
             "}"], "BanPhimAo")
s = thay(s, ["\tif (!s_uBanPhimLai || SDL_GetTicks() < s_uBanPhimLai)",
             "\t\treturn;",
             "\ts_uBanPhimLai = 0;",
             "\tSDL_Window* pWin = (SDL_Window*)JxPosix_MainWindow();",
             "\tif (!pWin)",
             "\t\treturn;",
             "\tSDL_StopTextInput(pWin);",
             "\tBanPhimMo(pWin, s_nBanPhimMatKhau);",
             "}"],
            ["\tif (!s_uBanPhimTat || SDL_GetTicks() < s_uBanPhimTat)",
             "\t\treturn;",
             "\ts_uBanPhimTat = 0;",
             "\tSDL_Window* pWin = (SDL_Window*)JxPosix_MainWindow();",
             "\tif (!pWin)",
             "\t\treturn;",
             "\tSDL_StopTextInput(pWin);\t// %s den hen ma khong o nao nhan tieu diem -> tat that" % DAU,
             "}"], "BanPhimNhip")
ghi(TEP, s)
print("da va:", TEP)
