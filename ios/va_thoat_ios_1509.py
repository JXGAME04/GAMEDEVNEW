# -*- coding: utf-8 -*-
r"""[IOS-THOAT 15/09] Tren iOS: GIU nut Thoat nhung bam khong ket thuc tien trinh.

Vi sao:
  SDL 3.2.30 da tat exit() khi main tra ve (SDL_uikitappdelegate.m:398-409), nen moi duong "thoat"
  deu de lai tien trinh song ma khong con cua so. Nguoi duyet cua Apple bam Thoat se thay app treo
  -> tu choi theo quy dinh 2.1. Apple cung khong cho app tu dong minh (HIG).
  Chu game (15/09): "giu nut nhung bam khong thoat la duoc".

Sua o 6 cho, deu rao #ifdef JX_IOS + nhanh #else giu nguyen ma cu (de ios/kiem_rao.py chung minh
ban Android va Windows khong doi):
  1. UiInit.cpp        nut Thoat o man dau      -> khong lam gi (KHONG goi CloseWindow: dong roi khong
                                                   mo lai gi thi man hinh den)
  2. UiConnectInfo.cpp hop loi ket noi          -> ve man chon may chu (giong nhanh default ngay duoi)
  (Da BO ban va cho UiUpdatePatch.cpp: tep do khong duoc dung cho BAT KY nen tang nao - khong co trong
   android/lists, ios/CMakeLists.txt, macos/CMakeLists.txt hay .vcxproj - nen va vao chi lam ban va to them
   ma khong doi hanh vi.)
  4. ShortcutKey.cpp   ham Exit() cho script    -> khong lam gi
  5. UiShell.cpp       UiPostQuitMsg()          -> khong dat co chet (chan chung)
  6. KPosixWin32.cpp   PostQuitMessage()        -> khong day su kien thoat SDL (chan chung: 9 loi auto
                                                   tu thoat trong S3Client.cpp, WM_DESTROY, trinh chieu phim)
KHONG dung toi hop ESC trong game: no von da goi ReturnToIdle + mo lai man dau, khong thoat tien trinh.

Cac tep nay la ISO-8859-1/GBK nen doc-ghi bang latin-1 de giu nguyen tung byte.
Chay lai nhieu lan khong sao (thay dau [IOS-THOAT] thi bo qua).

Dung:  python3 ios/va_thoat_ios_1509.py
"""
import os
import re
import sys

GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DAU = "IOS-THOAT 15/09"

# (duong dan, mo ta, mau tim, ham dung ban thay)
VA = []


def them(duong_dan, mo_ta, mau, lam):
    VA.append((duong_dan, mo_ta, re.compile(mau), lam))


them(
    "Sources/S3Client/Ui/UiCase/UiInit.cpp",
    "nut Thoat o man dau -> khong lam gi",
    r"(else if \(pWnd == &m_ExitGame\)\r?\n(\s*)\{\r?\n)(\s*)CloseWindow\(\);\r?\n(\s*)UiPostQuitMsg\(\);\r?\n",
    lambda m: (
        m.group(1)
        + "#ifdef JX_IOS\t// [%s] iOS khong cho app tu dong: giu nut, bam khong lam gi.\n" % DAU
        + "\t\t// KHONG goi CloseWindow() o day - dong cua so ma khong mo lai gi thi man hinh den.\n"
        + "#else\n"
        + m.group(3) + "CloseWindow();\n"
        + m.group(4) + "UiPostQuitMsg();\n"
        + "#endif\n"
    ),
)

them(
    "Sources/S3Client/Ui/UiCase/UiConnectInfo.cpp",
    "hop loi ket noi -> ve man chon may chu",
    r"(case CI_NS_EXIT_PROGRAM:\r?\n(\s*)Hide\(\);\r?\n)(\s*)UiPostQuitMsg\(\);\r?\n",
    lambda m: (
        m.group(1)
        + "#ifdef JX_IOS\t// [%s] khong thoat app; ve man chon may chu nhu nhanh default o duoi.\n" % DAU
        + m.group(2) + "g_LoginLogic.ReturnToIdle();\n"
        + m.group(2) + "KUiSelServer::OpenWindow();\n"
        + "#else\n"
        + m.group(3) + "UiPostQuitMsg();\n"
        + "#endif\n"
    ),
)

them(
    "Sources/S3Client/Ui/ShortcutKey.cpp",
    "ham Exit() cho script -> khong lam gi",
    r"(int LuaExit\(Lua_State \* L\)\r?\n\{\r?\n)(\s*)UiPostQuitMsg\(\);\r?\n",
    lambda m: (
        m.group(1)
        + "#ifndef JX_IOS\t// [%s] script goi Exit() cung khong duoc dong app tren iOS.\n" % DAU
        + m.group(2) + "UiPostQuitMsg();\n"
        + "#endif\n"
    ),
)

them(
    "Sources/S3Client/Ui/UiShell.cpp",
    "UiPostQuitMsg() -> khong dat co chet",
    r"(void UiPostQuitMsg\(\)\r?\n\{\r?\n)(\s*)s_UiLiveSeed = UI_LIVING_S_DEAD;\r?\n",
    lambda m: (
        m.group(1)
        + "#ifndef JX_IOS\t// [%s] chan chung: iOS khong dat co chet cho giao dien.\n" % DAU
        + m.group(2) + "s_UiLiveSeed = UI_LIVING_S_DEAD;\n"
        + "#endif\n"
    ),
)

them(
    "Sources/Engine/Src/Platform/KPosixWin32.cpp",
    "PostQuitMessage() -> khong day su kien thoat SDL",
    r"(void PostQuitMessage\(int n\)\r?\n\{\r?\n)(\s*)(\(void\)n; SDL_Event ev;[^\r\n]*)\r?\n",
    lambda m: (
        m.group(1)
        + "#ifdef JX_IOS\t// [%s] iOS: nuot lenh thoat (auto tu thoat, WM_DESTROY, trinh chieu phim).\n" % DAU
        + m.group(2) + "(void)n;\n"
        + "#else\n"
        + m.group(2) + m.group(3) + "\n"
        + "#endif\n"
    ),
)


def main():
    da_sua = 0
    for duong_dan, mo_ta, mau, lam in VA:
        tep = os.path.join(GOC, duong_dan)
        if not os.path.isfile(tep):
            sys.exit("HONG: khong thay %s" % duong_dan)
        with open(tep, "rb") as f:
            goc = f.read().decode("latin-1")

        if DAU in goc:
            print("bo qua (da va): %s" % duong_dan)
            continue

        so = len(mau.findall(goc))
        if so != 1:
            sys.exit("HONG: %s - mau tim thay %d lan, phai dung 1 lan" % (duong_dan, so))

        moi = mau.sub(lam, goc, count=1)
        if moi == goc:
            sys.exit("HONG: %s - thay the khong doi gi" % duong_dan)

        with open(tep, "wb") as f:
            f.write(moi.encode("latin-1"))
        print("da va: %-52s %s" % (duong_dan, mo_ta))
        da_sua += 1

    print("---")
    print("so tep da va: %d / %d" % (da_sua, len(VA)))
    print("Nho chay: python3 ios/kiem_rao.py  va  python3 ios/kiem_rao.py --pc")


if __name__ == "__main__":
    main()
